# Type deduction

Type deduction is a feature of C++ that allows a template type to be deduced from an argument. Depending on the type of the argument value (pointer, value, lvalue reference, rvalue reference), the deduced template parameter type may have various type modifications such as a `const` qualifier or a reference `&`.

The example shows how different argument types are derived into a template type depending on the type of value used to call the function.

## C++20 Concepts

In C++20, the derived type is passed to a concept. This can lead to problems if, for example, a value type was expected but an L-value reference was passed to the concept.

```c++
template <typename T>
concept C = std::same_as<T, Foo>;

template <C T> void func(T &&) {
}

struct Foo{};

int main(){
    Foo f;
    Foo & ref_f = f;
    // func(f); -> does not compile because std::is_same<T, Foo> is false
    // std::is_same<T, Foo> is resolved to std::is_same<Foo&, Foo>
    // std::same_as<std::remove_reference_t<T>, Foo> would work
    func(Foo{})
}
```
