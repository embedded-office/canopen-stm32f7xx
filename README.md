
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
# configure project build environment
$ cmake --preset gcc-arm

# build the application
$ cmake --build --preset debug
```
