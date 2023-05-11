# About

This code shows how to calculate the linear index from a multi dimensional index and the opposite way.

# Math

Source: https://stackoverflow.com/questions/29142417/4d-position-from-1d-index

## transform multi dimensional index to linear index

The formula to calculate the linear index from a n dimensional index is:
```
Index = xn ( D{n-1} * ... * D1  ) + x{n-1} ( D{n-2} * ... * D1 ) + ... + x2 * D1 + x1
```

`n` is the highest dimensions and `1` the smallest.

## transform linear index to multi dimensional index with modulo (version 1)

The formula to calculate the multi dimensional index from a linear index is:

```
xn = ( ( Index - Index( x1, ..., x{n-1} ) ) / Product( D1, ..., D{N-1} ) ) % Dn
With following cases:
x1 = Index % D1;
x2 = ( ( Index - Index(x1) ) / D1 ) %  D2;
xn  = ( ( Index(x{n-1}) - (xn * D{n-1} * ... * D1 ) ) / D1 * ... * DN ) %  DN;
```

Resolved for 4 Dim:

```
x1 = Index % D1;
x2 = ( ( Index - x1 ) / D1 ) %  D2;
x3 = ( ( Index - x2 * D1 - x1 ) / (D1 * D2) ) % D3;
x4 = ( ( Index - x3 * D2 * D1 - x2 * D1 - x1 ) / (D1 * D2 * D3) ) % D4;
```

## transform linear index to multi dimensional index with C++ integer division (version 2)

This method uses the behavior of C++, that integer division is always rounded down.

```
stepLenght{n} = D{n-1} * ... * D1
stepLenght{1} = 1

x{n} = Index / stepLenght{n}
x{n-1} = (Index - x{n} * stepLenght{n}) / stepLenght{n-1}
x{n-k} = (Index - (x{n} * stepLenght{n}) - ... - (x{n-k+1} * stepLenght{n-k+1})) / stepLenght{n-k}
x{1} = Index
```


# Implementation

* **runtime.cpp**: Implement the algorithm completely in runtime without C++ meta programming. The implementation is focusing on the algorithm.
* *comming* **compile.cpp**: Implement the algorithm with C++ meta programming. Calculate much as possible at compile time. Allows better performance but makes the implementation harder to understand.

Compile time example for linear index to multi dimensional index v2 is located in `features/23/mdspan/linear_index`.
