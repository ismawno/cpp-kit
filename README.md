# vector-view

vector-view is a simple, single-header C++ library that implements a templated class compatible with `std::vector`. It enables modification of the contents of a `std::vector` while preventing the user from altering its size or any other properties. This functionality is particularly useful when limiting the reach of a non-const getter of a `std::vector`.

## Features

- Single-header C++ library for easy integration into existing projects
- Templated class compatible with `std::vector`
- Allows modification of `std::vector` contents without changing size or other properties
- Ideal for limiting the reach of non-const getters of a `std::vector`

## Dependencies

vector-view has no external dependencies.

## Usage

To use vector-view, simply include the `vector_view.hpp` header in your project. The library is header-only, so it does not require any compilation or the use of any build system.

For more information on how to use vector-view and the available functionality, please refer to the documentation.

## License

vector-view is licensed under the MIT License. See LICENSE for more information.
