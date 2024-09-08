# Simple HTTP server

[![CTests](https://github.com/dzen03/simple-http-server/actions/workflows/ctest.yml/badge.svg)](https://github.com/dzen03/simple-http-server/actions/workflows/ctest.yml)

## About

Written from scratch for POSIX and Windows. 
Primary development goes on macOS[^1], so WinAPI might be buggy/slightly outdated[^2].

There is configured [Github Action on Linux + Windows](https://github.com/dzen03/simple-http-server/actions/workflows/ctest.yml). Both platform utilise clang-tidy.

## Usage

There is an example in [main.cpp](main.cpp)

## Compile

### Requirements

#### *nix

CMake, GCC/LLVM Clang[^1].

#### Windows

CMake, Some kind of C++ compiler: mingw, cygwin or MSVC[^2].

### Code

```bash
cmake -B build
cd build
cmake --build .
./http-server
```

### Tests _(Optional)_:

```bash
ctest -V
```

[^1]: I am using clang, [ninja](https://github.com/ninja-build/ninja), so they are tested.

[^2]: I am using [llvm-mingw](https://github.com/mstorsjo/llvm-mingw), [ninja](https://github.com/ninja-build/ninja) (other things might not work; **llvm's mingw-make does NOT work**).
