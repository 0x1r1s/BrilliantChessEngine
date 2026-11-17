# Brilliant Chess Engine

A C++ chess engine with multiple front ends: UCI, a local CLI, a future
Lichess bot, and a browser/WebAssembly adapter. It uses
[chess-library](https://github.com/Disservin/chess-library) for positions and
legal chess moves; the engine implements evaluation and search.

## Layout

```text
engine/     Reusable evaluation and search library (no UI or protocol code)
apps/       Thin executable/adaptor entry points
tests/      Unit, integration, and perft regression tests
web/        Static GitHub Pages site
third_party/ Dependency notes; source is fetched by CMake
```

`apps/web` will expose the C++ engine to WebAssembly. `web` is the static
site that presents the engine and will load that WebAssembly module when it
exists.

See [docs/architecture.md](docs/architecture.md) for the detailed layout.

## Build

Install CMake and a C++ compiler, then configure and build from the repository
root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/bce-cli
```

The first configuration downloads the pinned `chess-library` dependency into
the build directory. Subsequent builds use the downloaded copy.
