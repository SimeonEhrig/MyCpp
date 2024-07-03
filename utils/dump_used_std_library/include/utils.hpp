#pragma once

#include <ostream>
#include <sstream>

struct CompilerInfo {
  std::string host_compiler_name = "<unknown>";
  std::string host_compiler_version = "0";
  std::string device_compiler_name = "<unknown>";
  std::string device_compiler_version = "0";

  friend std::ostream &operator<<(std::ostream &os, CompilerInfo const &info) {
    os << "host compiler: " << info.host_compiler_name << "\n"
       << "host compiler version: " << info.host_compiler_version << "\n"
       << "device compiler: " << info.device_compiler_name << "\n"
       << "device compiler version: " << info.device_compiler_version;
    return os;
  }
};

inline CompilerInfo get_compiler_info() {
  CompilerInfo info;
#if defined(__clang__) && (defined(__CUDA__) || defined(__HIP__))
  std::stringstream version;
  version << __clang_major__ << "." << __clang_minor__ << "."
          << __clang_patchlevel__;
  info.host_compiler_name = "Clang";
  info.host_compiler_version = version.str();
  info.device_compiler_name = "Clang";
  info.device_compiler_version = version.str();
#endif
#if defined(__NVCC__)
  info.device_compiler_name = "NVCC";
  std::stringstream nvcc_version;
  nvcc_version << __CUDACC_VER_MAJOR__ << "." << __CUDACC_VER_MINOR__ << "."
               << __CUDACC_VER_BUILD__;
  info.device_compiler_version = nvcc_version.str();
#endif

#if not defined(__CUDA__) && not defined(__NVCC__) && not defined(__HIP__)
  info.device_compiler_name = "no device compiler";
#endif

#if defined(__GNUC__)
  info.host_compiler_name = "GCC";
  std::stringstream gcc_version;
  gcc_version << __GNUC__ << "." << __GNUC_MINOR__;
  info.host_compiler_version = gcc_version.str();
#endif

#if defined(__clang__)
  info.host_compiler_name = "Clang";
  std::stringstream clang_version;
  clang_version << __clang_major__ << "." << __clang_minor__ << "."
                << __clang_patchlevel__;
  info.host_compiler_version = clang_version.str();
#endif

  return info;
}
