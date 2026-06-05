# Walrus

Walrus is an early C++ game engine scaffold. The project is split into an
`engine` dynamic library and a `game` executable that links against it.

The engine owns the program entrypoint in `engine/EntryPoint.cpp`. The game
defines `Walrus::CreateApplication(...)`, and the engine calls that function to
create and run the game application.

## Project Layout

```text
engine/      Engine headers and source files
game/        Game code that uses the engine
build/       Generated build output
Makefile     Build, format, and link targets
```

## Requirements

- `clang++`
- `clang-format`
- `make`
- Vulkan SDK for Vulkan builds
- Xcode command line tools for Metal framework linking on macOS

## Building

The default backend is Vulkan:

```sh
make
```

You can also choose a backend explicitly:

```sh
make vulkan
make metal
```

Or pass the backend as a Makefile argument:

```sh
make BACKEND=vulkan
make BACKEND=metal
```

Every build runs `clang-format` first.

The engine builds as `build/lib/libwalrus.dylib` on macOS and
`build/lib/libwalrus.so` on Linux. Symbols are hidden by default, and public
engine API should be marked with `WAPI`.

## Running

```sh
./build/bin/game-vulkan
./build/bin/game-metal
```

## Cleaning

```sh
make clean
```

## Vulkan SDK

The Vulkan target uses the `VULKAN_SDK` environment variable for include,
library, and runtime library paths. If it is not set, build with:

```sh
make vulkan VULKAN_SDK=/path/to/VulkanSDK
```
