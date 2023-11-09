# C++ Modules

Pretty basic example to use C++ modules with `CMake`.

# Requirements

This example is only tested with:

* CMake 3.28
* Ninja 1.11
* Clang 16 and 17

GCC 14 (not released yet) should also work.

# Usage

```bash
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER=clang++
cmake --build .
./cxx_modules
echo $? # should display 3
```

# Sources

- https://blog.conan.io/2023/10/17/modules-the-packaging-story.html
- https://github.com/fmtlib/fmt/pull/3679
- https://www.modernescpp.com/index.php/c20-more-details-about-module-support-of-the-big-three/
