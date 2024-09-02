#include <iostream>
#include <type_traits>

// This file is about to check if a Object has a specific member function. There
// are several checks, which are more strict or relaxed:
// - Check if a member function exist and can be called with some parameters
// (implicit cast allowed).
// - Check if a member function exist and can be called with some parameters
// (implicit cast allowed) and the return value can be casted to a specific
// type.
// - Check if a member function exist and can be called with some parameters
// (implicit cast allowed) and the return value is a specific type.
// - Check if a member function exist and has a specific function signature.
// Therefore no implicit casts of the arguments and return value is allowed.
// Does also work with overloads.
#define CHECK_CONCEPT(X) std::cout << std::boolalpha << #X << ": " << X << "\n";

// ############################
// structs
// ############################

struct Empty {};

struct NoFoo {};

struct FooNoArg {
  void foo() {}
};

struct FooOneArgInt {
  void foo(int) {}
};

struct FooOneArgFloat {
  void foo(int) {}
};

struct FooOneArgReturnInt {
  int foo(int) { return 3; }
};

struct FooOneArgReturnFloat {
  float foo(int) { return 3; }
};

struct FooArgIntIntReturnInt {
  int foo(int, int) { return 0; }
};

struct FooArgFloatIntReturnInt {
  int foo(float, int) { return 0; }
};

struct FooArgFloatFloatReturnInt {
  int foo(float, float) { return 0; }
};

struct FooArgEmptyEmptyReturnInt {
  int foo(Empty, Empty) { return 0; }
};

struct FooOverload1 {
  int foo(int, int) { return 0; }
  int foo(float, float) { return 0; }
};

struct FooOverload2 {
  int foo(float, int) { return 0; }
  int foo(float, float) { return 0; }
};

// ############################
// check for existing argument
// ############################

template <typename T>
concept CHasFoo = requires(T t) {
  { t.foo() };
};

void exampleCHassFoo() {
  std::cout << "############################\n"
            << "## check for existing argument\n"
            << "############################\n";

  CHECK_CONCEPT(CHasFoo<NoFoo>);
  CHECK_CONCEPT(CHasFoo<FooNoArg>);
  CHECK_CONCEPT(CHasFoo<FooOneArgInt>);
  std::cout << "\n";
};

template <typename T>
concept CHasFooOneArg = requires(T t) {
  { t.foo(int()) };
};

void exampleCHasFooOneArg() {
  CHECK_CONCEPT(CHasFooOneArg<NoFoo>);
  CHECK_CONCEPT(CHasFooOneArg<FooNoArg>);
  CHECK_CONCEPT(CHasFooOneArg<FooOneArgInt>);
  CHECK_CONCEPT(CHasFooOneArg<FooOneArgFloat>);
  std::cout << "\n";
};

// ############################
// check for exact return type
// ############################

template <typename T>
concept CHasFooOneArgReturnInt = requires(T t) {
  { t.foo(int()) } -> std::same_as<int>;
};

void exampleCHasFooOneArgReturnInt() {
  std::cout << "############################\n"
            << "## check for exact return type\n"
            << "############################\n";

  CHECK_CONCEPT(CHasFooOneArgReturnInt<FooOneArgInt>);
  CHECK_CONCEPT(CHasFooOneArgReturnInt<FooOneArgReturnInt>);
  CHECK_CONCEPT(CHasFooOneArgReturnInt<FooOneArgReturnFloat>);
  std::cout << "\n";
};

// ############################
// check for exact return type
// ############################

template <typename T>
concept CHasFooOneArgReturnCastToFloat = requires(T t) {
  { t.foo(int()) } -> std::convertible_to<float>;
};

void exampleCHasFooOneArgReturnCastToFloat() {
  std::cout << "############################\n"
            << "## check for convertible return type\n"
            << "############################\n";

  CHECK_CONCEPT(CHasFooOneArgReturnCastToFloat<FooOneArgInt>);
  CHECK_CONCEPT(CHasFooOneArgReturnCastToFloat<FooOneArgReturnInt>);
  CHECK_CONCEPT(CHasFooOneArgReturnCastToFloat<FooOneArgReturnFloat>);
  std::cout << "\n";
};

// ############################
// check for implicit castable argument
// ############################

template <typename T>
concept CHasCallTwoInt = requires(T t) {
  { t.foo(int(), int()) } -> std::same_as<int>;
};

void exampleCHasCallTwoInt() {
  std::cout << "############################\n"
            << "## check for implicit castable argument\n"
            << "############################\n";

  CHECK_CONCEPT(CHasCallTwoInt<FooArgIntIntReturnInt>);
  CHECK_CONCEPT(CHasCallTwoInt<FooArgFloatIntReturnInt>);
  CHECK_CONCEPT(CHasCallTwoInt<FooArgFloatFloatReturnInt>);
  CHECK_CONCEPT(CHasCallTwoInt<FooArgEmptyEmptyReturnInt>);
  std::cout << "\n";
};

// ############################
// check for exact function signature
// ############################

// check for the menber function: int foo(float, int)
// &T::foo: get the function pointer of the member function
// int (T::*)(float, int): defines the signature of the member variable pointer
//  - the first int is the return value
//  - (T::*): means is a member function of the Class T
//  - (float, int): the arguments of the function, here float and int
// the static_cast in static_cast<int (T::*)(float, int)>(&T::foo) is required,
// if the function iis overloaded to the select the correct overload
template <typename T>
concept CHasFuncSignaturArgFloatIntReturnInt = requires {
  requires std::same_as<decltype(static_cast<int (T::*)(float, int)>(&T::foo)),
                        int (T::*)(float, int)>;
};

void exampleCHasFuncSignaturArgFloatIntReturnInt() {
  std::cout << "############################\n"
            << "## check for implicit castable argument\n"
            << "############################\n";

  CHECK_CONCEPT(CHasFuncSignaturArgFloatIntReturnInt<FooArgIntIntReturnInt>);
  CHECK_CONCEPT(CHasFuncSignaturArgFloatIntReturnInt<FooArgFloatIntReturnInt>);
  CHECK_CONCEPT(
      CHasFuncSignaturArgFloatIntReturnInt<FooArgFloatFloatReturnInt>);
  CHECK_CONCEPT(
      CHasFuncSignaturArgFloatIntReturnInt<FooArgEmptyEmptyReturnInt>);

  std::cout << "\n";
};

// ############################
// check for exact function signature for overloaded function
// ############################

void exampleCheckForOverloadedFunctionSignature() {
  std::cout << "############################\n"
            << "## check for exact function signature for overloaded function\n"
            << "############################\n";

  CHECK_CONCEPT(CHasFuncSignaturArgFloatIntReturnInt<FooOverload1>);
  CHECK_CONCEPT(CHasFuncSignaturArgFloatIntReturnInt<FooOverload2>);
  std::cout << "\n";
};

// ############################
// check how the compiler is evaluating concepts
// ############################
// To get a detail compiler error message which part of a concept cannot be full
// filed, call the following function in the main(). To check a different
// Concept, simply change the Concept in the template. To check a different
// Type, which should be fulfill the concept, change the template type in the
// function call.

template <CHasFoo T> void run_compiler_error() {}

int main() {
  exampleCHassFoo();
  exampleCHasFooOneArg();
  exampleCHasFooOneArgReturnInt();
  exampleCHasFooOneArgReturnCastToFloat();
  exampleCHasCallTwoInt();
  exampleCHasFuncSignaturArgFloatIntReturnInt();
  exampleCheckForOverloadedFunctionSignature();

  // run_compiler_error<NoFoo>();

  return 0;
}
