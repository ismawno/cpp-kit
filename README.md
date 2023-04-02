# profile-tools

profile-tools is a performance profiling library for C++ projects, designed to accurately measure and report the elapsed time of specific code blocks. By leveraging the concept of C++ scope and utilizing macros, the library can efficiently profile the execution time of individual processes and their associated subprocesses. Results can be output as a JSON file for visualization in `chrome://tracing`, or as an in-memory hierarchy displaying the relative durations of parent and child processes in percentage terms. The profiler functionality is easily enabled or disabled by defining (or not defining) the PERF macro, offering flexibility to users during development.

## Features

- Efficient and easy-to-use performance profiling for C++ projects using macros
- Accurate measurement of elapsed time for code blocks and their associated subprocesses
- JSON output for compatibility with `chrome://tracing` visualization
- Hierarchical representation of timing data, displaying relative durations of parent and subprocesses
- Simple enabling and disabling of profiler functionality through the PERF macro

## Dependencies

profile-tools has no external dependencies.

## Usage

To use profile-tools, include the `perf.hpp` header in your project. The profiler functionality can be easily enabled or disabled by defining (or not defining) the PERF macro.

Build the project using premake5 as a static library. You must create a premake5 workspace with a user-implemented entry point that uses the profile-tools library. You can then build the workspace with premake5 to create an executable.

It is advisable to include profile-tools in a user-made repository as a git submodule. This allows you to keep the profile-tools code separate from your own code, making it easier to manage dependencies and track changes.

For more information on how to use profile-tools, please refer to the documentation.

## License

profile-tools is licensed under the MIT License. See LICENSE for more information.
