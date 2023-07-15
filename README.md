<<<<<<< HEAD
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
=======
# profile-tools

profile-tools is a performance profiling library for C++ projects, designed to accurately measure and report the elapsed time of specific code blocks. By leveraging the concept of C++ scope and utilizing macros, the library can efficiently profile the execution time of individual processes and their associated subprocesses. Results can be output as a JSON file for visualization in `chrome://tracing`, or as an in-memory hierarchy displaying the relative durations of parent and child processes in percentage terms. The profiler functionality is easily enabled or disabled by defining (or not defining) the `PERF` macro, offering flexibility to users during development.

## Features

- Efficient and easy-to-use performance profiling for C++ projects using macros
- Accurate measurement of elapsed time for code blocks and their associated subprocesses
- JSON output for compatibility with `chrome://tracing` visualization
- Hierarchical representation of timing data, displaying relative durations of parent and subprocesses
- Simple enabling and disabling of profiler functionality through the `PERF` macro

## Dependencies

- [debug-tools](https://github.com/ismawno/debug-tools)

The [fetch_dependencies.py](https://github.com/ismawno/profile-tools/scripts/fetch_dependencies.py) script will automatically add all the dependencies as git submodules, provided that the user has already created their own repository and included the current project as a git submodule (or at least downloaded it into the repository). To ensure all runs smoothly once the script has been executed, do not rename the folders containing the various dependencies. All external dependencies, those not created by the same author, will be added as submodules within the [vendor](https://github.com/ismawno/profile-tools/vendor) folder.

## Building and Usage

1. Ensure you have `premake5` and `make` installed on your system. `premake5` is used to generate the build files, and `make` is used to compile the project.
2. Create your own repository and include the current project as a git submodule (or at least download it into the repository).
3. Run the [fetch_dependencies.py](https://github.com/ismawno/profile-tools/scripts/fetch_dependencies.py) script located in the [scripts](https://github.com/ismawno/profile-tools/scripts) folder to automatically add all the dependencies as git submodules.
4. Create an entry point project with a `premake5` file, where the `main.cpp` will be located. Link all libraries and specify the kind of the executable as `ConsoleApp`. Don't forget to specify the different configurations for the project.
5. Create a `premake5` file at the root of the repository describing the `premake` workspace and including all dependency projects.
6. Build the entire project by running the `make` command in your terminal. You can specify the configuration by using `make config=the_configuration`.
7. To use profile-tools, include the [perf.hpp](https://github.com/ismawno/profile-tools/include/perf/perf.hpp) header in your project. The profiler functionality can be easily enabled or disabled by defining (or not defining) the `PERF` macro.

## License

profile-tools is licensed under the MIT License. See LICENSE for more information.
>>>>>>> profile-tools/master
