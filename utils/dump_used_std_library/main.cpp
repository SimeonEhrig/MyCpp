#include "utils.hpp"
#include <iostream>

int main() {
  std::cout << get_compiler_info() << "\n\n";

#ifdef _GLIBCXX_RELEASE
  std::cout << "use libstdc++ (GNU GCC's standard library implementation)"
            << std::endl;
  std::cout << "version: " << _GLIBCXX_RELEASE << std::endl;
#endif

#ifdef _LIBCPP_VERSION
  std::cout << "use libc++ (LLVM's standard library implementation)"
            << std::endl;
  std::cout << "version: " << _LIBCPP_VERSION << std::endl;
#endif
}
