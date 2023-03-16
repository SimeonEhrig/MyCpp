cmake_minimum_required(VERSION 3.18)
project(NDindexing LANGUAGES CXX)

set(_RUNTIME_EXE runtime_version)

add_executable(${_RUNTIME_EXE})
target_sources(${_RUNTIME_EXE}
   PRIVATE
   runtime.cpp)
set_target_properties(${_RUNTIME_EXE} PROPERTIES
  CXX_STANDARD 17
)
