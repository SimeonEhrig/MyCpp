# About

The code shows how different compilers handle the different standard library implementations `libstdc++` (shipped with gcc) and `libc++` (shipped with llvm). When the application is compiled and executed, it displays information about the compiler and standard library used for the build process.

```bash
# hipcc + libc++
$ ./showLibCppImplementation
host compiler: Clang
host compiler version: 17.0.0
device compiler: Clang
device compiler version: 17.0.0

use libc++ (LLVM's standard library implementation)
version: 170006
```

The following combinations are supported:

- gcc + libstdc++ [1]
- clang + libstdc++
- clang + libc++
- nvcc + gcc + libstdc++
- nvcc + clang + libstdc++
- nvcc + clang + libc++ [2]
- hipcc + libstdc++ [3]
- hipcc + libc++ [4]

  - [1] The use of gcc with `libc++` should be possible, but is very uncommon: https://libcxx.llvm.org/UsingLibcxx.html#using-a-custom-built-libc
  - [2] It only works in theory. In practice, the `libc++` is not supported by the nvcc: https://forums.developer.nvidia.com/t/cuda-issues-with-clang-compiler/177589/11
  - [3] hipcc is a wrapper for the AMD clang fork..
  - [4] It is not natively supported because AMD does not ship `libc++` with its clang fork. Therefore another vanila clang installation with the `libc++` is required. The path to this installation can be set via the CMake argument `-DLIBCPP_PATH=/path/to/llvm/root`.

# Building

The example supports [CMake-Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html), which makes it easy to create different combinations. You can display all presets by running `cmake --list-presets` in the project folder.

Example:

```bash
# build clang with libstdc++
root@20f22105a998:/code# cmake --preset clang-libstdcpp
Preset CMake variables:

  CMAKE_CXX_COMPILER="clang++"
  CMAKE_C_COMPILER="clang"
  CMAKE_EXPORT_COMPILE_COMMANDS="ON"

-- The C compiler identification is Clang 18.1.8
-- The CXX compiler identification is Clang 18.1.8
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/clang - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/clang++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /code/build/clang-libstdcpp
root@20f22105a998:/code# cmake --build --preset clang-libstdcpp
[ 50%] Building CXX object CMakeFiles/showLibCppImplementation.dir/main.cpp.o
[100%] Linking CXX executable showLibCppImplementation
[100%] Built target showLibCppImplementation
root@20f22105a998:/code# ./build/clang-libstdcpp/showLibCppImplementation
host compiler: Clang
host compiler version: 18.1.8
device compiler: no device compiler
device compiler version: 0

use libstdc++ (GNU GCC's standard library implementation)
version: 13
```

## Special CMake Parameters

There are two CMake parameters that can be set additionally during the cmake configuration and are system-dependent. They are therefore not included in the CMake presets.

- `cmake --preset clang-libstdcpp -DCMAKE_CXX_FLAGS="--gcc-toolchain=/path/to/gcc/build`: Sets a different `libstdc++` version for clang. **Attention:** The standard installation via apt in Ubuntu does not work, because the folders `include`, `lib` and others do not use the same root folder.
- `cmake --preset hip-libcpp -DLIBCPP_PATH=/path/to/llvm/root`: Use the `libc++` of a vanilla clang installation. At the moment AMD does not ship hipcc with a libc++ installation.

# Observations

- When clang uses the `libstdc++` on Ubuntu, it always takes the latest version.
- If hipcc uses a different `libc++`, the automatic detection of the standard library is deactivated. Instead, include and library paths and linker flags are set manually.
