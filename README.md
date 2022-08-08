
# Repository Structure

```
+- cmake        : git submodule reference to repository 'cmake-scripts'
+- debug        : debugger configurations
+- dependencies : cmake definition to external projects 'bsp-stm32f769' and 'canopen-stack'
+- src          : quickstart
|  +- app       : application source code
|  +- board     : configuration to hardware board
|  +- driver    : canopen drivers
```

# CANopen Quickstart

The CANopen quickstart application for the STM32F769 discovery board.

# Collaboration

## Build instructions

The application is build with CMake using the provided presets:

```bash
# configure the project for ARM target with GCC cross-compiler
$ cmake --preset gcc-arm
# or: re-configure the whole project from scratch (delete cache)
$ cmake --preset gcc-arm --fresh

# build the library and application for your target
$ cmake --build --preset debug
# or: clean and re-build the library and application
$ cmake --build --preset debug --clean-first
```
