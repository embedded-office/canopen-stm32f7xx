
# Repository Structure

```
+- cmake        : submodule referencing the repository 'cmake-scripts'
+- debug        : debugger configurations
+- dependencies : external managed components
+- src          : canopen example project
|  +- app       : application source code
|  +- config    : configuration of startup and HAL
|  +- driver    : canopen target specific drivers
```

# CANopen Quickstart

The CANopen quickstart application for the STM32F769 discovery board.

TODO: Explain, what is the application doing


## Usage

### Development Tools

Download and install these free tools for your system:

- Install the build tools [Cmake](https://cmake.org/)
- Install the build system [Ninja](https://ninja-build.org/)
- Install the cross-compiler [Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)

*Note: on my Windows machine, I use the [Ozone debugger](https://www.segger.com/downloads/jlink/) with the free [ST-Link Reflash Utility](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/) from Segger. The generated debug information in the ELF image should be suitable for all other debuggers, too.*


### Submodule: cmake-scripts

The basic collection of re-usable CMake scripts are placed as a Git submodule in the directory `/cmake`. The Git submodule is a reference to a specific commit hash of the Git repository [cmake-scripts](https://github.com/embedded-office/cmake-scripts).

*Small reminder: when cloning this repository you need to initialize and update the submodules:*

```bash
# clone Git repository and initialize submodules:
$ git clone --recurse-submodules <repository>

# or, in case you have already cloned the Git repository:
$ git clone <repository>
$ cd <repository-directory>
$ git submodule update --init
```


### Project dependencies

We use two extern managed components to build our target application. When using external projects it is important to define naming rules for exported CMake `target` names to eliminate name collisions.

For the example application in this repository we use:

- Target [stm32f7xx-hal](https://github.com/embedded-office/STM32CubeF7) - a fork of the STM32CubeF7 package with enhancements for usage with CMake
- Target [canopen-stack](https://github.com/embedded-office/canopen-stack) - the free CANopen Stack, provided by Embedded Office

*Note: The used versions of the dependencies are defined in the directory `/dependencies`.*


## Build instructions

Just type in the project root directory:

```bash
# configure the project for debugging
$ cmake --preset debug

# build the application for your target
$ cmake --build ./build/debug
```

The target image file `canopen-stm32f769.elf` and the corresponding map file `canopen-stm32f769.map` are generated and placed in `out/debug`.


## Load and Executing on target

For the Ozone debugger there is a basic debugger configuration `debug/ozone.jdebug`, which loads the image to the target and runs to function `main()`.

- power up your STM32F769 discovery board
- double-click the debugger configuration
- watch the uploading and running to main with wonder...

...and have fun playing with this tiny CANopen quickstart application :)

If you encounter any improvement in project setup, implementation or documentation, please rise an issue and help me to simplify the life of Embedded software engineers.
