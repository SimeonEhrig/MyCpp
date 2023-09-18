#include "compute_cuda.hpp"
#include "compute_hip.hpp"
#include <iostream>
#include <thread>
#include <vector>

struct HipMatrix {
  const int dim;
  std::vector<int> &output;

  void operator()(int const dev) { cHip::compute(dev, dim, output); }
};

struct CudaMatrix {
  const int dim;
  std::vector<int> &output;

  void operator()(int const dev) { cCuda::compute(dev, dim, output); }
};

int main() {
  int constexpr dim = 1024 * 20;
  int constexpr size = dim * dim;

  std::cout << "==== AMD ===== \n";
  cHip::printDevices();
  std::cout << "\n=== NVIDIA === \n";
  cCuda::printDevices();

  std::cout << "\n";

  int const number_amd_gpus = cHip::getNumberDevices();
  int const number_nvidia_gpus = cCuda::getNumberDevices();

  std::vector<std::vector<int>> hip_results;
  for (int dev = 0; dev < number_amd_gpus; ++dev) {
    hip_results.emplace_back(size, 0);
  }

  std::vector<std::vector<int>> cuda_results;
  for (int dev = 0; dev < number_nvidia_gpus; ++dev) {
    cuda_results.emplace_back(size, 0);
  }

  std::vector<std::thread> threads;

  for (int dev = 0; dev < number_amd_gpus; ++dev) {
    std::cout << "Run matrix multiplication on AMD GPU Nr. " << dev << "\n";
    HipMatrix j(dim, hip_results[dev]);
    threads.emplace_back(j, dev);
  }

  for (int dev = 0; dev < number_nvidia_gpus; ++dev) {
    std::cout << "Run matrix multiplication on NVIDIA GPU Nr. " << dev << "\n";
    CudaMatrix j(dim, cuda_results[dev]);
    threads.emplace_back(j, dev);
  }

  for (std::thread &t : threads) {
    t.join();
  }

  std::cout << "all threads are joined\n";

  return 0;
}
