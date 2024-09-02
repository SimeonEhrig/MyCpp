cmake_minimum_required(VERSION 3.18)
project(concepts LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(_TARGET_FUNCTION_SIGNATURE_MATICHING functionSignatureMatching)
add_executable(${_TARGET_FUNCTION_SIGNATURE_MATICHING})
target_sources(${_TARGET_FUNCTION_SIGNATURE_MATICHING}
   PRIVATE
   functionSignatureMatching.cpp)
