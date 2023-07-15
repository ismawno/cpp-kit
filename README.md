# container-view

container-view is a simple, single-header C++ library that implements a templated class compatible with `std::vector`. It enables modification of the contents of a `std::vector` while preventing the user from altering its size or any other properties. This functionality is particularly useful when limiting the reach of a non-const getter of a `std::vector`.

## Features

- Single-header C++ library for easy integration into existing projects
- Templated class compatible with `std::vector`
- Allows modification of `std::vector` contents without changing size or other properties
- Ideal for limiting the reach of non-const getters of a `std::vector`

## Dependencies

container-view has no external dependencies.

## Usage

To use container-view, simply include the [container_view.hpp](https://github.com/ismawno/container-viewutils/container-view.hpp) header in your project. The library is header-only, so it does not require any compilation or the use of any build system.

For more information on how to use container-view and the available functionality, please refer to the documentation.

## License

container-view is licensed under the MIT License. See LICENSE for more information.
# debug-tools

debug-tools is a header-only C++ project containing a single file with useful debug macros. These macros are enabled when the user defines the DEBUG macro in their project. To use debug-tools, simply include the [debug.hpp](https://github.com/ismawno/debug-log-tools/include/dbg/log.hpp) file.

## Features

- Header-only project for easy integration into existing C++ projects
- Useful debug macros to streamline development and debugging processes
- Enabled by defining the DEBUG macro in your project

## Dependencies

debug-tools has no external dependencies.

## Usage

To use debug-tools, include the [debug.hpp](https://github.com/ismawno/debug-log-tools/include/dbg/log.hpp) header in your project. The debug macros will be enabled when the DEBUG macro is defined in your project.

Since debug-tools is a header-only project, it does not require compilation or the use of any build system.

For more information on how to use debug-tools and the available macros, please refer to the documentation.

## License

debug-tools is licensed under the MIT License. See LICENSE for more information.
