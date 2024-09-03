# is required for the target nvFatBin
cmake_minimum_required(VERSION 3.30)

project(nvFatbin LANGUAGES CXX)

find_package(CUDAToolkit REQUIRED)

set(_TARGET_ORIGINAL original)

add_executable(${_TARGET_ORIGINAL})
target_sources(${_TARGET_ORIGINAL}
    PRIVATE
    original.cpp
)
target_link_libraries(${_TARGET_ORIGINAL}
    PRIVATE
    CUDA::cuda_driver
    CUDA::cudart_static
    CUDA::nvrtc_static
    CUDA::nvfatbin_static
)
