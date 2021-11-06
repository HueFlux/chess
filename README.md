# chess

A simple chess program written in C++.

## Compiling

You need:
* SFML 2.5+ development headers and library
* C++17 compliant compiler
* CMake build system

To compile, run CMake on the project directory with `CMAKE_BUILD_TYPE=Release`
and you'll get Makefile or equivalent for your platform, with which you can compile the project.

On Linux and macOS:
```fish
~ $ git clone https://github.com/HueFlux/chess.git
~ $ cd chess
~/chess $ mkdir build/ && cd build/
~/chess/build $ cmake -DCMAKE_BUILD_TYPE=Release ..
~/chess/build $ make
```
