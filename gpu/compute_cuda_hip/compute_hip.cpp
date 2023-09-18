#include "compute_hip.hpp"
#include <hip/hip_runtime.h>
#include <iostream>
#include <string>

namespace cHip {

inline void checkFunc(hipError_t code, const char *file, int line) {
  if (code != hipSuccess) {
    std::cout << "[" << file << ":" << line << "] Error Code " << code << ": "
              << hipGetErrorString(code) << "\n";
    std::exit(1);
  }
}
#define hipCheck(code)                                                         \
  { checkFunc((code), __FILE__, __LINE__); }

void printDevices() {
  std::cout << "Compiler Version: " << HIP_VERSION_MAJOR << "."
            << HIP_VERSION_MINOR << "." << HIP_VERSION_PATCH << "\n";
  int runtimeVersion = 0;
  hipCheck(hipRuntimeGetVersion(&runtimeVersion));
  std::string const runtimeVersionStr = std::to_string(runtimeVersion);
  std::cout << "Runtime Version: " << runtimeVersionStr[0] << "."
            << runtimeVersionStr[2] << "." << runtimeVersionStr[4] << "\n";

  int numberOfDevice = -1;
  hipCheck(hipGetDeviceCount(&numberOfDevice));
  for (auto dev_id = 0; dev_id < numberOfDevice; ++dev_id) {
    hipDeviceProp_t prop;
    hipCheck(hipGetDeviceProperties(&prop, dev_id));
    std::cout << "GPU " << dev_id << ": " << prop.name << "\n";
  }
}

int getNumberDevices() {
  int numberDevices = 0;
  hipCheck(hipGetDeviceCount(&numberDevices));
  return numberDevices;
}

__global__ void iotaKernel(int *out, int const size) {
  int id = blockIdx.x * blockDim.x + threadIdx.x;
  if (id < size) {
    out[id] = id;
  }
}

__global__ void matmulKernel(int *const A, int *const B, int *C) {
  int n = blockDim.x * gridDim.x;
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;
  int sum = 0.0;

  for (int k = 0; k < n; k++) {
    sum += A[row * n + k] * B[k * n + col];
  }

  C[row * n + col] = sum;
}

void compute(int const dev, int const dim, std::vector<int> &output) {
  int constexpr threads = 32;
  if (dim % threads) {
    std::cout << "[HIP " << dev << "] "
              << "Error: Dim needs to be multiple of number of threads ("
              << threads << ")\n";
    return;
  }

  hipCheck(hipSetDevice(dev));

  if (output.size() != dim * dim) {
    output = std::vector<int>(dim * dim);
  }

  int *devA, *devB, *devC;
  hipCheck(hipMalloc((void **)&devA, dim * dim * sizeof(int)));
  hipCheck(hipMalloc((void **)&devB, dim * dim * sizeof(int)));
  hipCheck(hipMalloc((void **)&devC, dim * dim * sizeof(int)));

  dim3 blocks_1D((dim * dim) / threads);

  hipLaunchKernelGGL(iotaKernel, dim3(blocks_1D), dim3(threads), 0, 0, devA,
                     dim * dim);
  hipCheck(hipGetLastError());
  hipLaunchKernelGGL(iotaKernel, dim3(blocks_1D), dim3(threads), 0, 0, devB,
                     dim * dim);
  hipCheck(hipGetLastError());

  std::cout << "[HIP " << dev << "] "
            << "Start compute\n";
  hipLaunchKernelGGL(matmulKernel, dim3(dim / threads, dim / threads, 1),
                     dim3(threads, threads, 1), 0, 0, devA, devB, devC);
  hipCheck(hipGetLastError());
  hipCheck(hipDeviceSynchronize());
  std::cout << "[HIP " << dev << "] "
            << "end compute\n";

  hipCheck(hipMemcpy(output.data(), devC, dim * dim * sizeof(int),
                     hipMemcpyDeviceToHost));

  hipCheck(hipFree(devA));
  hipCheck(hipFree(devB));
  hipCheck(hipFree(devC));
}

} // namespace cHip
