# stm32-event-hal
An event driven Hardware Abstraction Layer (HAL) for STM32 written in C++.

## Table of Contents
- [stm32-event-hal](#stm32-event-hal)
  - [Table of Contents](#table-of-contents)
  - [Project goals](#project-goals)
  - [Supported hardware](#supported-hardware)
  - [Repository structure](#repository-structure)
  - [Development environment](#development-environment)
    - [Required tools](#required-tools)
    - [Preferred IDE](#preferred-ide)
    - [Build, Flash & Debug the project](#build-flash--debug-the-project)
      - [**With a CLI**](#with-a-cli)
      - [**With Visual Studio Code**](#with-visual-studio-code)
    - [Running the tests](#running-the-tests)
      - [Unit tests](#unit-tests)
      - [**With a CLI**](#with-a-cli-1)
      - [**With Visual Studio Code**](#with-visual-studio-code-1)
  - [Useful resources](#useful-resources)

## Project goals
The goal of this project is to write a set of C++ classes that can be used as an HAL for the STM32 platform.  

These classes are intended to be used in a bare metal environment, within an event-driven program.  

As you'll notice, it has not for goal to be the fastest nor the lightest HAL available. I intend to make full use of all of C++'s capabilities: Exceptions, standard library and more.  

On a personal level, this project is a way for me to learn about C++ and how it can be used in reasonably powerful embedded boards. (See [Supported hardware](#supported-hardware))

## Supported hardware
I am currently developping and testing this code on an [STM32F750-DK board from ST](https://www.st.com/en/evaluation-tools/stm32f7508-dk.html) which includes the following hardware :
- An STM32F750N8 MCU with a single Arm Cortex-M7 core @216MHz, 64 KB of flash memory & 320 KB of SRAM
- 16 MB of QSPI flash
- 8 MB of SDRAM

Device code (See [Repository structure](#repository-structure) below) is currently provided only for this chip (STM32F750). Driver code should be platform agnostic by design provided device classes are implemented following the given interfaces.

I'm doing my best to write this library to be as portable as it may be. However this is intended to be a fun little side project for me so the development kit I own is currently the only validated platform.

## Repository structure
stm32-event-hal  
└── .vscode
> Configuration files for Visual Studio Code, which is the recommanded IDE for this project ([See Development environment](#development-environment))  

└── include
> Header files from ST & ARM

└── ld
> Linker scripts  
> A board-specific script is provided for the supported development kit as well as a suggested sections script for your programs. This sections script assumes that the target board possesses a QSPI flash chip as well as an SDRAM chip.

└── src  
> Contains a simple, ready to use event loop class as well as a template main file.  

&nbsp;&nbsp;&nbsp;&nbsp;└── hardware
> MCU and board specific headers

&nbsp;&nbsp;&nbsp;&nbsp;└── device
> Platform specific implementation classes. These are the classes that interract with hardware components.

&nbsp;&nbsp;&nbsp;&nbsp;└── driver
> Platform agnostic driver classes. These classes contain logic that is independant of the system they run on. They are the "HAL" per-say. Device classes for the correct platform must be provided to them at instanciation.

└── tests
> Where all tests go!

&nbsp;&nbsp;&nbsp;&nbsp;└── unit
> Main directory for unit tests. This whole directory structure is taken from [this blog post](https://interrupt.memfault.com/blog/unit-testing-basics#stubs-fakes-and-mocks).

## Development environment
### Required tools
- make
- [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- [STM32 Cube IDE](https://www.st.com/en/development-tools/stm32cubeide.html) - Only the following executables which come packaged with the Cube IDE suite are actually required:
  - ST-Link GDB server for debugging purposes
  - *Specific to STM32F750-DK*: Cube Programmer for flashing the QSPI chip
- CppUTest + CppUMock (test)
- lcov (test)

### Preferred IDE
I included the workspace configuration files I use on Visual Studio Code to develop for this project. The workspace comes with a pre-configured recommanded set of extensions as well as tasks to build, flash and debug the project. 
To properly function, the workspace expects the following environment variables to be defined:
- `STLINK_GDB_SERVER`: Path to the ST Link GDB server
- `CUBE_PROGRAMMER_BIN_DIR`: Path to the directory where the cube
- `STM32_EXT_LOADER`: Path to the external loader ELF file used by Cube Programmer to flash the QSPI. A set of pre-compiled loaders for ST's development kits can be found at `$CUBE_PROGRAMMER_BIN_DIR/ExternalLoader`
- `ARM_GDB_SERVER_PORT`: Port on which the GDB server will await connections.
- `STM32_MCU`: Name of the MCU to compile for, as expected by `hardware/mcu.hpp`
- `STM32_BOARD`: Name of the board to compile for, as expected by your MCU header (from the `hardware` directory)
- `STM32_SVD_FILE`: Worskpace-relative path to the .svd file to use for debugging. SVD files can be retrieved from [this handy GitHub repo](https://github.com/posborne/cmsis-svd/tree/master/data/STMicro).

### Build, Flash & Debug the project
#### **With a CLI**
In a separate shell, start the GDB server:
``` Shell
"$STLINK_GDB_SERVER" -p "$ARM_GDB_SERVER_PORT" -l 31 -v -d -s -cp "$CUBE_PROGRAMMER_BIN_DIR" -k -e -el "$STM32_EXT_LOADER"
```
Then run make:
``` Shell
make all flash-n-debug
```

#### **With Visual Studio Code**
The included `Cortex Debug` debugging configuration will build, flash and break at `main()` provided every environment variable is properly set.

Various workspace tasks are also defined. Shortcuts for these can be found in the command menu, under the *target* folder.

### Running the tests
#### Unit tests
Unit tests are written using the CppUTest & CppUMock framework.
All the boiler plate makefiles are extracted from [this GitHub](https://github.com/memfault/interrupt.git/example/unit-testing). I recommend the following articles about unit testing for embedded software:
- [Embedded C/C++ Unit Testing Basics](https://interrupt.memfault.com/blog/unit-testing-basics)
- [Embedded C/C++ Unit Testing with Mocks](https://interrupt.memfault.com/blog/unit-test-mocking)

#### **With a CLI**
Tests can be run with `make all` from the `tests/unit/` folder. The `lcov` target can be used to generate a coverage report.

#### **With Visual Studio Code**
Various workspace tasks are defined. Shortcuts for these can be found in the command menu, under the *Unit tests* folder.

The *Unit test debug* launch configuration can be used to debug a specific test sheet. Breakpoints should be set before launch.

## Useful resources
The following resources helped me develop this project:
- https://vivonomicon.com : A blog with various articles to start bare metal programming on STM32
- https://alex-robenko.gitbook.io/bare_metal_cpp : An online ebook which provided me with the base architecture for this project
- http://www.lucadavidian.com/category/embedded : A blog with various embedded programming articles
- https://interrupt.memfault.com/blog/zero-to-main-1 : Yet another well known blog with various embedded programming articles
- The C++ Programming Language, Bjarne Stroustrup
