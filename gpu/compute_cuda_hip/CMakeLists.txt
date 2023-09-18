cmake_minimum_required(VERSION 3.22)

project(computeCudaHip LANGUAGES CXX HIP CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_HIP_STANDARD 20)
set(CMAKE_HIP_STANDARD_REQUIRED ON)

set_source_files_properties(compute_hip.cpp PROPERTIES LANGUAGE HIP)
add_library(hipDevice)
target_include_directories(hipDevice PUBLIC include)
target_sources(hipDevice PRIVATE compute_hip.cpp)

set_source_files_properties(compute_cuda.cpp PROPERTIES LANGUAGE CUDA)
add_library(cudaDevice)
target_include_directories(cudaDevice PUBLIC include)
target_sources(cudaDevice PRIVATE compute_cuda.cpp)


add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
PRIVATE
main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE hipDevice cudaDevice)
