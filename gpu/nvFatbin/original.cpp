#include <cuda.h>
#include <iostream>
#include <nvFatbin.h>
#include <nvrtc.h>

#define NUM_THREADS 128
#define NUM_BLOCKS 32

#define NVRTC_SAFE_CALL(x)                                                     \
  do {                                                                         \
    nvrtcResult result = x;                                                    \
    if (result != NVRTC_SUCCESS) {                                             \
      std::cerr << "\nerror: " #x " failed with error "                        \
                << nvrtcGetErrorString(result) << '\n';                        \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define CUDA_SAFE_CALL(x)                                                      \
  do {                                                                         \
    CUresult result = x;                                                       \
    if (result != CUDA_SUCCESS) {                                              \
      const char *msg;                                                         \
      cuGetErrorName(result, &msg);                                            \
      std::cerr << "\nerror: " #x " failed with error " << msg << '\n';        \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define NVFATBIN_SAFE_CALL(x)                                                  \
  do {                                                                         \
    nvFatbinResult result = x;                                                 \
    if (result != NVFATBIN_SUCCESS) {                                          \
      std::cerr << "\nerror: " #x " failed with error "                        \
                << nvFatbinGetErrorString(result) << '\n';                     \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

const char *fatbin_saxpy = "                                  \n\
__device__  float compute(float a, float x, float y) {        \n\
return a * x + y;                                             \n\
}                                                             \n\
                                                             \n\
extern \"C\" __global__                                       \n\
void saxpy(float a, float *x, float *y, float *out, size_t n) \n\
{                                                             \n\
size_t tid = blockIdx.x * blockDim.x + threadIdx.x;           \n\
if (tid < n) {                                                \n\
  out[tid] = compute(a, x[tid], y[tid]);                     \n\
}                                                             \n\
}                                                             \n";

size_t process(const void *input, const char *input_name, void **output,
               const char *arch) {
  // Create an instance of nvrtcProgram with the code string.
  nvrtcProgram prog;
  NVRTC_SAFE_CALL(nvrtcCreateProgram(&prog,               // prog
                                     (const char *)input, // buffer
                                     input_name,          // name
                                     0,                   // numHeaders
                                     NULL,                // headers
                                     NULL));              // includeNames

  // specify that LTO IR should be generated for LTO operation
  const char *opts[1];
  opts[0] = arch;
  nvrtcResult compileResult = nvrtcCompileProgram(prog,  // prog
                                                  1,     // numOptions
                                                  opts); // options
  // Obtain compilation log from the program.
  size_t logSize;
  NVRTC_SAFE_CALL(nvrtcGetProgramLogSize(prog, &logSize));
  char *log = new char[logSize];
  NVRTC_SAFE_CALL(nvrtcGetProgramLog(prog, log));
  std::cout << log << '\n';
  delete[] log;
  if (compileResult != NVRTC_SUCCESS) {
    exit(1);
  }
  // Obtain generated CUBIN from the program.
  size_t CUBINSize;
  NVRTC_SAFE_CALL(nvrtcGetCUBINSize(prog, &CUBINSize));
  char *CUBIN = new char[CUBINSize];
  NVRTC_SAFE_CALL(nvrtcGetCUBIN(prog, CUBIN));
  // Destroy the program.
  NVRTC_SAFE_CALL(nvrtcDestroyProgram(&prog));
  *output = (void *)CUBIN;
  return CUBINSize;
}

int main(int argc, char *argv[]) {
  void *known = NULL;
  size_t known_size =
      process(fatbin_saxpy, "fatbin_saxpy.cu", &known, "-arch=sm_52");

  CUdevice cuDevice;
  CUcontext context;
  CUmodule module;
  CUfunction kernel;
  CUDA_SAFE_CALL(cuInit(0));
  CUDA_SAFE_CALL(cuDeviceGet(&cuDevice, 0));
  CUDA_SAFE_CALL(cuCtxCreate(&context, 0, cuDevice));

  // Dynamically determine the arch to make one of the entries of the fatbin
  // with
  int major = 0;
  int minor = 0;
  CUDA_SAFE_CALL(cuDeviceGetAttribute(
      &major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, cuDevice));
  CUDA_SAFE_CALL(cuDeviceGetAttribute(
      &minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, cuDevice));
  int arch = major * 10 + minor;
  char smbuf[16];
  sprintf(smbuf, "-arch=sm_%d", arch);

  void *dynamic = NULL;
  size_t dynamic_size =
      process(fatbin_saxpy, "fatbin_saxpy.cu", &dynamic, smbuf);
  sprintf(smbuf, "%d", arch);

  // Load the dynamic CUBIN and the statically known arch CUBIN
  // and put them in a fatbin together.
  nvFatbinHandle handle;
  const char *fatbin_options[] = {"-cuda"};
  NVFATBIN_SAFE_CALL(nvFatbinCreate(&handle, fatbin_options, 1));

  NVFATBIN_SAFE_CALL(nvFatbinAddCubin(handle, (void *)dynamic, dynamic_size,
                                      smbuf, "dynamic"));
  NVFATBIN_SAFE_CALL(
      nvFatbinAddCubin(handle, (void *)known, known_size, "52", "known"));

  size_t fatbinSize;
  NVFATBIN_SAFE_CALL(nvFatbinSize(handle, &fatbinSize));
  void *fatbin = malloc(fatbinSize);
  NVFATBIN_SAFE_CALL(nvFatbinGet(handle, fatbin));
  NVFATBIN_SAFE_CALL(nvFatbinDestroy(&handle));

  CUDA_SAFE_CALL(cuModuleLoadData(&module, fatbin));
  CUDA_SAFE_CALL(cuModuleGetFunction(&kernel, module, "saxpy"));

// Generate input for execution, and create output buffers.
#define NUM_THREADS 128
#define NUM_BLOCKS 32
  size_t n = NUM_THREADS * NUM_BLOCKS;
  size_t bufferSize = n * sizeof(float);
  float a = 5.1f;
  float *hX = new float[n], *hY = new float[n], *hOut = new float[n];
  for (size_t i = 0; i < n; ++i) {
    hX[i] = static_cast<float>(i);
    hY[i] = static_cast<float>(i * 2);
  }
  CUdeviceptr dX, dY, dOut;
  CUDA_SAFE_CALL(cuMemAlloc(&dX, bufferSize));
  CUDA_SAFE_CALL(cuMemAlloc(&dY, bufferSize));
  CUDA_SAFE_CALL(cuMemAlloc(&dOut, bufferSize));
  CUDA_SAFE_CALL(cuMemcpyHtoD(dX, hX, bufferSize));
  CUDA_SAFE_CALL(cuMemcpyHtoD(dY, hY, bufferSize));
  // Execute SAXPY.
  void *args[] = {&a, &dX, &dY, &dOut, &n};
  CUDA_SAFE_CALL(cuLaunchKernel(kernel, NUM_BLOCKS, 1, 1, // grid dim
                                NUM_THREADS, 1, 1,        // block dim
                                0, NULL,   // shared mem and stream
                                args, 0)); // arguments
  CUDA_SAFE_CALL(cuCtxSynchronize());
  // Retrieve and print output.
  CUDA_SAFE_CALL(cuMemcpyDtoH(hOut, dOut, bufferSize));

  for (size_t i = 0; i < n; ++i) {
    std::cout << a << " * " << hX[i] << " + " << hY[i] << " = " << hOut[i]
              << '\n';
  }
  // Release resources.
  CUDA_SAFE_CALL(cuMemFree(dX));
  CUDA_SAFE_CALL(cuMemFree(dY));
  CUDA_SAFE_CALL(cuMemFree(dOut));
  CUDA_SAFE_CALL(cuModuleUnload(module));
  CUDA_SAFE_CALL(cuCtxDestroy(context));
  delete[] hX;
  delete[] hY;
  delete[] hOut;
  // Release resources.
  free(fatbin);
  delete[] ((char *)known);
  delete[] ((char *)dynamic);

  return 0;
}
