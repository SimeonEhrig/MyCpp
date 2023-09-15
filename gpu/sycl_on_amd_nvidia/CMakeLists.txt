cmake_minimum_required(VERSION 3.18)

project(sycl_on_amd_nvidia LANGUAGES CXX)

option(ENABLE_NVIDIA "builds for nvidia graphic cards" ON)
option(ENABLE_AMD "builds for amd graphic cards" ON)

# the example only supports Intel's icpx compiler
if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "IntelLLVM")
  message(FATAL_ERROR "CXX compiler needs to be the icpx")
endif()

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
   PRIVATE
   main.cpp)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
)

# needs to set compiler flag "-fsycl" to enable sycl compilation
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-fsycl")
target_link_options(${CMAKE_PROJECT_NAME} PUBLIC "-fsycl")

# spir64 is required for Intel GPUs
set(custom_SYCL_TARGETS "spir64")

if(ENABLE_NVIDIA)
  # TODO: only one architecture is support at the moment
  # The Nvidia backend does not require manual setting of the SM level. But for consistency with the AMD backend, we do.
  if("${CMAKE_CUDA_ARCHITECTURES}" STREQUAL "")
    message(STATUS "set to CMAKE_CUDA_ARCHITECTURES to 50")
    set(CMAKE_CUDA_ARCHITECTURES 50)
  endif()

  target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC "ENABLED_NVIDIA")
  list(APPEND custom_SYCL_TARGETS nvptx64-nvidia-cuda)
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -Xsycl-target-backend=nvptx64-nvidia-cuda --offload-arch=sm_${CMAKE_CUDA_ARCHITECTURES})
  target_link_options(${CMAKE_PROJECT_NAME} PUBLIC -Xsycl-target-backend=nvptx64-nvidia-cuda --offload-arch=sm_${CMAKE_CUDA_ARCHITECTURES})
endif()
  
if(ENABLE_AMD)
  # TODO: only one architecture is support at the moment
  # The AMD does require manual setting the GPU architecture.
  if("${GPU_TARGETS}" STREQUAL "")
    message(STATUS "set to GPU_TARGETS to gfx906")
    set(GPU_TARGETS gfx906)
  endif()

  target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC "ENABLED_AMD")
  list(APPEND custom_SYCL_TARGETS amdgcn-amd-amdhsa)
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -Xsycl-target-backend=amdgcn-amd-amdhsa --offload-arch=${GPU_TARGETS})
  target_link_options(${CMAKE_PROJECT_NAME} PUBLIC -Xsycl-target-backend=amdgcn-amd-amdhsa --offload-arch=${GPU_TARGETS})
endif()

# to enable compilation for multiple GPU architectures, we need to create a list of GPU targets
list(JOIN custom_SYCL_TARGETS "," custom_SYCL_TARGETS_CONCAT)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-fsycl-targets=${custom_SYCL_TARGETS_CONCAT}")
target_link_options(${CMAKE_PROJECT_NAME} PUBLIC "-fsycl-targets=${custom_SYCL_TARGETS_CONCAT}")
