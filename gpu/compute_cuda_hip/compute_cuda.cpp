#include "compute_cuda.hpp"
#include <cuda.h>
#include <iostream>
#include <string>

namespace cCuda {

inline void checkFunc(cudaError_t code, const char *file, int line) {
  if (code != cudaSuccess) {
    std::cout << "[" << file << ":" << line << "] Error Code " << code << ": "
              << cudaGetErrorString(code) << "\n";
    std::exit(1);
  }
}
#define cudaCheck(code)                                                        \
  { checkFunc((code), __FILE__, __LINE__); }

void printDevices() {
  std::cout << "Compiler Version: " << CUDA_VERSION / 1000 << "."
            << CUDA_VERSION / 10 % 100 << "\n";
  int runtimeVersion = 0;
  cudaCheck(cudaRuntimeGetVersion(&runtimeVersion));
  std::string const runtimeVersionStr = std::to_string(runtimeVersion);
  std::cout << "Runtime Version: " << runtimeVersionStr.substr(0, 2) << "."
            << runtimeVersionStr.substr(3, 3) << "\n";

  int numberOfDevice = -1;
  cudaCheck(cudaGetDeviceCount(&numberOfDevice));
  for (auto dev_id = 0; dev_id < numberOfDevice; ++dev_id) {
    cudaDeviceProp prop;
    cudaCheck(cudaGetDeviceProperties(&prop, dev_id));
    std::cout << "GPU " << dev_id << ": " << prop.name << "\n";
  }
}

int getNumberDevices() {
  int numberDevices = 0;
  cudaCheck(cudaGetDeviceCount(&numberDevices));
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
    std::cout << "[CUDA " << dev << "] "
              << "Error: Dim needs to be multiple of number of threads ("
              << threads << ")\n";
    return;
  }

  cudaCheck(cudaSetDevice(dev));

  if (output.size() != dim * dim) {
    output = std::vector<int>(dim * dim);
  }

  int *devA, *devB, *devC;
  cudaCheck(cudaMalloc((void **)&devA, dim * dim * sizeof(int)));
  cudaCheck(cudaMalloc((void **)&devB, dim * dim * sizeof(int)));
  cudaCheck(cudaMalloc((void **)&devC, dim * dim * sizeof(int)));

  iotaKernel<<<(dim * dim) / threads, threads>>>(devA, dim * dim);
  cudaCheck(cudaGetLastError());
  iotaKernel<<<(dim * dim) / threads, threads>>>(devB, dim * dim);
  cudaCheck(cudaGetLastError());

  std::cout << "[CUDA " << dev << "] "
            << "Start compute\n";
  matmulKernel<<<dim3(dim / threads, dim / threads, 1),
                 dim3(threads, threads, 1)>>>(devA, devB, devC);
  cudaCheck(cudaGetLastError());
  cudaCheck(cudaDeviceSynchronize());
  std::cout << "[CUDA " << dev << "] "
            << "end compute\n";

  cudaCheck(cudaMemcpy(output.data(), devC, dim * dim * sizeof(int),
                       cudaMemcpyDeviceToHost));

  cudaCheck(cudaFree(devA));
  cudaCheck(cudaFree(devB));
  cudaCheck(cudaFree(devC));
}

} // namespace cCuda
