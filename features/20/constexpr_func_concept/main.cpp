#include <algorithm>
#include <concepts>
#include <string_view>

// ##################################
// Static String
// ##################################

template <auto N> struct StaticString {
  constexpr StaticString(char const (&str)[N]) {
    std::copy(str, str + N, value);
  }

  char value[N];
};

template <StaticString TName> struct S {
  template <StaticString TNameOther>
  constexpr bool operator==(S<TNameOther> const &) const {
    return std::string_view(TName.value) == std::string_view(TNameOther.value);
  }
};

// ##################################
// concrete types
// ##################################

struct NoContainer {};

template <typename T> struct Generator {
  using value_type = T;
  T v;

  value_type operator[](int) { return v; }
};

template <typename T> struct WrongGenerator {
  using value_type = T;
  T v;

  value_type &operator[](int) { return v; }
};

template <typename T> struct Span {
  using value_type = T;
  using reference = T &;
  T v;

  reference operator[](int) { return v; }
};

template <typename T> struct WrongSpan {
  using value_type = T;
  using reference = T &;
  T v;

  value_type operator[](int) { return v; }
};

// ##################################
// meta functions
// ##################################

template <typename T> constexpr bool has_value_type() {
  return requires { typename T::value_type; };
}

template <typename T> constexpr bool has_reference() {
  return requires { typename T::reference; };
}

template <typename T> struct ExpectedValueType {
  using type = S<"NotSupported">;
};

template <typename T>
  requires(has_value_type<T>() && !has_reference<T>())
struct ExpectedValueType<T> {
  using type = S<"value access">;
};

template <typename T>
  requires(has_value_type<T>() && has_reference<T>())
struct ExpectedValueType<T> {
  using type = S<"reference access">;
};

template <typename T> using ExpectedValueType_t = ExpectedValueType<T>::type;

template <typename T> consteval auto get_access_operator_type() {
  if constexpr (!requires { std::declval<T>()[0]; })
    return S<"MissingAccessOperator">{};

  if constexpr (!has_reference<T>())
    if constexpr (requires {
                    {
                      std::declval<T>()[0]
                    } -> std::same_as<typename T::value_type>;
                  })
      return S<"value access">{};
    else
      return S<"No reference type but reference access">{};

  if constexpr (has_reference<T>())
    if constexpr (requires {
                    {
                      std::declval<T>()[0]
                    } -> std::same_as<typename T::reference>;
                  })
      return S<"reference access">{};
    else
      return S<"reference type but value access">{};
  // all returns needs to be optional
  // the default return type needs to be void, otherwise it does not compile
  // so void also means unknown error
}

template <typename T>
using get_access_operator_type_t = decltype(get_access_operator_type<T>());

// ##################################
// Concepts
// ##################################

template <typename T>
concept CReference = requires {
  requires std::same_as<ExpectedValueType_t<T>, S<"reference access">>;
};

template <typename T>
concept IGenerator = requires {
  requires std::same_as<get_access_operator_type_t<T>, ExpectedValueType_t<T>>;
};

template <typename T>
concept ISpan = requires { requires IGenerator<T>; };

// ##################################
// user functions
// ##################################

void run_ref_type(CReference auto) {}

void gen(IGenerator auto) {}

int main() {
  run_ref_type(Span<int>{});
  // run_ref_type(Generator<int>{});

  gen(Generator<int>{});
  gen(Span<int>{});
  // gen(WrongGenerator<int>{});
  // gen(WrongSpan<int>{});

  return 0;
}
