# Simple HTTP server

[![CTests](https://github.com/dzen03/simple-http-server/actions/workflows/ctest.yml/badge.svg)](https://github.com/dzen03/simple-http-server/actions/workflows/ctest.yml)

## About

Written from scratch for POSIX and Windows. 
Primary development goes on macOS, so WinAPI might be buggy/slightly outdated.

## Usage

There is an example in [main.cpp](main.cpp)

## Compiling

```bash
cmake -B build
cd build
cmake --build .
./http-server
```

Tests _(Optional)_:
`ctest -V`