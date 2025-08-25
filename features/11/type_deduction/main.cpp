#include <iostream>
#include <type_traits>

struct Foo {
  int mut_value = 3;
  int const_value = 4;
  int &value() { return mut_value; };
  int const &value() const { return const_value; }
};

template <typename T> void h1(T, std::string msg) {
  if constexpr (std::is_const_v<T>) {
    std::cout << "[const] h1: " << msg << "\n";
  } else {
    std::cout << "[mut  ] h1: " << msg << "\n";
  }
}

template <typename T> void h2(T const, std::string msg) {
  if constexpr (std::is_const_v<T>) {
    std::cout << "[const] h2: " << msg << "\n";
  } else {
    std::cout << "[mut  ] h2: " << msg << "\n";
  }
}

template <typename T> void h3(T &, std::string msg) {
  if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
    std::cout << "[const] h3: " << msg << "\n";
  } else {
    std::cout << "[mut  ] h3: " << msg << "\n";
  }
}

template <typename T> void h4(T &&, std::string msg) {
  if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
    std::cout << "[const] h4: " << msg << "\n";
  } else {
    std::cout << "[mut  ] h4: " << msg << "\n";
  }
}

int main(int argc, char **argv) {
  Foo const const_f;
  Foo mut_f;

  h1(const_f, "const_f");             // h1<Foo>(Foo)
  h1(mut_f, "mut_f");                 // h1<Foo>(Foo)
  h1(Foo{}, "Foo{}");                 // h1<Foo>(Foo)
  h1((Foo const){}, "(Foo const){}"); // h1<Foo>(Foo)

  std::cout << "\n";

  h2(const_f, "const_f");             // h2<Foo>(Foo const)
  h2(mut_f, "mut_f");                 // h2<Foo>(Foo const)
  h2(Foo{}, "Foo{}");                 // h2<Foo>(Foo const)
  h2((Foo const){}, "(Foo const){}"); // h2<Foo>(Foo const)

  std::cout << "\n";

  h3(const_f, "const_f");             // h3<Foo const>(Foo&)
  h3(mut_f, "mut_f");               // h3<Foo>(Foo &)
  // does not compile
  // h3<Foo>(Foo const &)
  h3((Foo const){}, "(Foo const){}"); // h3<Foo>(Foo const &)

  std::cout << "\n";

  h4(const_f, "const_f");             // h4<Foo const&>(Foo const&)
  h4(mut_f, "mut_f");               // h4<Foo&>(Foo &)
  h4(Foo{}, "Foo{}");                 // h4<Foo>(Foo &&)
  h4((Foo const){}, "(Foo const){}"); // h4<Foo const>(Foo const &&)

  return 0;
}
