# About

The example implements an adapter which allows to access the elements of a mdspan with a linear index. The adapter is useful for algorithm, where the position of a single element is not import, for example if you want to transform (`std::transform`) each element or reduce (`std::reduce`) all elements. Therefore it is much easier to implement and optimize the algorithm in 1D.

# Why C++ 17?

This implementation is a prototype for the [vikunja](https://github.com/alpaka-group/vikunja) library which is written in C++ 17. 
