# cpp-kit

This project is a versatile collection of C++ utilities that I have found invaluable during the development of my 2D physics engine, [poly-physx](https://github.com/ismawno/poly-physx).

## Features

- A variety of useful containers not included in the STL, such as a dynamic array with fixed capacity, hashable tuple, linked list (using elements as nodes), multi-dimensional array, and more.
- Debug capabilities easily toggled through macros.
- Callbacks and event handling mechanisms.
- Convenient interfaces.
- Custom allocators for efficient memory management.
- Multithreading utilities, including parallel for-loops and thread pools.
- Profiling tools that can be disabled via macros.
- Serialization interfaces.

## Dependencies

cpp-kit has optional dependencies, depending on the features you wish to utilize:

- [glm](https://github.com/g-truc/glm)
- [yaml-cpp](https://github.com/ismawno/yaml-cpp)
- [spdlog](https://github.com/gabime/spdlog)

## Building and Usage

This project is designed to be used as a git submodule in another project (parent repo). A premake file is provided for building and linking cpp-kit, though much of it is header-only.

While these build instructions are minimal, this project is primarily for personal use. Although it has been built and tested on multiple machines (MacOS and Windows), it is not necessarily fully cross-platform or easy to build.

## License

cpp-kit is licensed under the MIT License. See LICENSE for more details.
