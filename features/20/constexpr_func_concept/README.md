# constexpr function in concepts

The example shows, how a `constexpr` function can be used in a concept to describe a complex requirement. For useful error messages, it uses a class with a string as template parameter to display error messages if the concept is not fulfilled.

# Example error message with GCC 15

```
/usr/bin/g++   -g -std=gnu++20 -fconcepts-diagnostics-depth=4 -MD -MT CMakeFiles/constexprFuncConcept.dir/main.cpp.o -MF CMakeFiles/constexprFuncConcept.dir/main.cpp.o.d -o CMakeFiles/constexprFuncConcept.dir/main.cpp.o -c /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp: In function ‘int main()’:
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:152:6: error: no matching function for call to ‘gen(WrongGenerator<int>)’
[build]   152 |   gen(WrongGenerator<int>{});
[build]       |   ~~~^~~~~~~~~~~~~~~~~~~~~~~
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:152:6: note: there is 1 candidate
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:144:6: note: candidate 1: ‘template<class auto:7>  requires  IGenerator<auto:7> void gen(auto:7)’
[build]   144 | void gen(IGenerator auto) {}
[build]       |      ^~~
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:144:6: note: template argument deduction/substitution failed:
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:144:6: note: constraints not satisfied
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp: In substitution of ‘template<class auto:7>  requires  IGenerator<auto:7> void gen(auto:7) [with auto:7 = WrongGenerator<int>]’:
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:152:6:   required from here
[build]   152 |   gen(WrongGenerator<int>{});
[build]       |   ~~~^~~~~~~~~~~~~~~~~~~~~~~
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:131:9:   required for the satisfaction of ‘IGenerator<auto:7>’ [with auto:7 = WrongGenerator<int>]
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:131:22:   in requirements  [with T = WrongGenerator<int>]
[build] /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:132:12: note: nested requirement ‘same_as<decltype (get_access_operator_type<T>()), typename ExpectedValueType<T>::type>’ is not satisfied, because
[build]   132 |   requires std::same_as<get_access_operator_type_t<T>, ExpectedValueType_t<T>>;
[build]       |   ~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[build] In file included from /usr/include/c++/15/compare:42,
[build]                  from /usr/include/c++/15/bits/stl_pair.h:65,
[build]                  from /usr/include/c++/15/bits/stl_algobase.h:64,
[build]                  from /usr/include/c++/15/algorithm:62,
[build]                  from /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/main.cpp:1:
[build] /usr/include/c++/15/concepts:59:15:   required for the satisfaction of ‘__same_as<_Tp, _Up>’ [with _Tp = S<StaticString<39>{"No reference type but reference access"}>; _Up = S<StaticString<13>{"value access"}>]
[build] /usr/include/c++/15/concepts:64:13:   required for the satisfaction of ‘same_as<decltype (get_access_operator_type<T>()), typename ExpectedValueType<T>::type>’ [with T = WrongGenerator<int>]
[build] /usr/include/c++/15/concepts:59:32: note: the expression ‘is_same_v<_Tp, _Up> [with _Tp = S<StaticString<39>{"No reference type but reference access"}>; _Up = S<StaticString<13>{"value access"}>]’ evaluated to ‘false’
[build]    59 |       concept __same_as = std::is_same_v<_Tp, _Up>;
[build]       |                           ~~~~~^~~~~~~~~~~~~~~~~~~
[build] ninja: build stopped: subcommand failed.
[proc] The command: /usr/bin/cmake --build /home/simeon/projects/MyCpp/features/20/constexpr_func_concept/build --config Debug --target all -- exited with code: 1
[driver] Build completed: 00:00:00.124
[build] Build finished with exit code 1
```
