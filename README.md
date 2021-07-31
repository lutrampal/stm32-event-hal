# stm32-event-hal
An event driven Hardware Abstraction Layer (HAL) for STM32 written in C++.

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

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── hardware
> MCU and board specific headers

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── device
> Platform specific implementation classes. These are the classes that interract with hardware components.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── driver
> Platform agnostic driver classes. These classes contain logic that is independant of the system they run on. They are the "HAL" per-say. Device classes for the correct platform must be provided to them at instanciation.

## Development environment
TODO

## Compiling the project
TODO

## Debugging the project
TODO

## Useful resources
The following resources helped me develop this project:
- https://vivonomicon.com : A blog with various articles to start bare metal programming on STM32
- https://alex-robenko.gitbook.io/bare_metal_cpp : An online ebook which provided me with the base architecture for this project
- http://www.lucadavidian.com/category/embedded : A blog with various embedded programming articles
- https://interrupt.memfault.com/blog/zero-to-main-1 : Yet another blog with various embedded programming articles
- The C++ Programming Language, Bjarne Stroustrup
