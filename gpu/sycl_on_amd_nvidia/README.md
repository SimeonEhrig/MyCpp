# About

This example shows, how to compile the same sycl code for AMD, Nvidia and Intel GPUs and CPUs. The example uses Intel OneAPI and the AMD and Nvidia Plugin from Codeplay.

- https://developer.codeplay.com/
- https://developer.codeplay.com/products/oneapi/nvidia/2023.2.1/guides/get-started-guide-nvidia
- https://developer.codeplay.com/products/oneapi/amd/2023.2.1/guides/get-started-guide-amd

# Usage

By default, support for AMD and Nvidia GPU is enabled.

```bash
mkdir build && cd build
cmake .. -DCMAKE_CXX_COMPILER=icpx
cmake --build .
./sycl_on_amd_nvidia
```

To disable AMD and/or NVIDIA support, set `-DENABLE_AMD=OFF` and/or `-DENABLE_NVIDIA=OFF` and CMake configuration time. The variables `-DGPU_TARGETS=gfx906` (AMD) and `-DCMAKE_CUDA_ARCHITECTURES=80` (NVIDIA) set the GPU architectures. At the moment, the `CMakeLists.txt` only supports one architecture at the same time. In general, it should be possible to compile for more than one GPU architecture.
