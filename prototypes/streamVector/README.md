# About

Experiment with stream operator to test an extended pipe syntax similar to C++ ranges.

- C++ Ranges: `std::view::filter(v) | std::view::transform(v)`
- Our idea: `func1 | func2 << executer`

The example test, how it works if we use an operator, which is already used by other functionality (in this case the `operator<<` of iostream).

# Output

```
v1.merge(v2.merge(v3)): [1, 2, 3, 4, 5, 6, 7, 8, 9]
v1 << v2 << v3: [1, 2, 3][4, 5, 6][7, 8, 9]
(v1 << v2 << v3): [1, 2, 3, 4, 5, 6, 7, 8, 9]
v123: [1, 2, 3, 4, 5, 6, 7, 8, 9]

v3.merge(v1.merge(v2)): [7, 8, 9, 1, 2, 3, 4, 5, 6]
v3 << v1 << v2: [7, 8, 9][1, 2, 3][4, 5, 6]
(v3 << v1 << v2): [7, 8, 9, 1, 2, 3, 4, 5, 6]
v312: [1, 2, 3, 4, 5, 6, 7, 8, 9]
```
