cmake_minimum_required(VERSION 3.18)
project(linear_index LANGUAGES CXX)

option(ENABLE_BENCH "Enable Benchmarks. Require Google benchmark" OFF)

find_package(mdspan)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
   PRIVATE
   main.cpp)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE std::mdspan)

if(ENABLE_BENCH)
  set(_BENCH_TARGET bench)

  find_package(benchmark REQUIRED)

  add_executable(${_BENCH_TARGET})
  target_sources(${_BENCH_TARGET}
    PRIVATE
    bench.cpp)
  set_target_properties(${_BENCH_TARGET} PROPERTIES
    CXX_STANDARD 17
  )
  target_include_directories(${_BENCH_TARGET}
    PRIVATE
    include
    bench)

  set(_BASELINE_FULLY_DYNAMIC_TARGET baseline_fully_dynamic_target)

  add_library(${_BASELINE_FULLY_DYNAMIC_TARGET} SHARED bench/baselineDynamic.cpp)
  target_link_libraries(${_BASELINE_FULLY_DYNAMIC_TARGET}
    PRIVATE
    std::mdspan
    benchmark::benchmark)

  target_link_libraries(${_BENCH_TARGET}
    PRIVATE
    std::mdspan
    benchmark::benchmark
    ${_BASELINE_FULLY_DYNAMIC_TARGET})
endif()
