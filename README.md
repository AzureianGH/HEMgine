# HEMgine

Hydrix Emulator Engine

## Overview

HEMgine is an emulator engine designed to perform various emulation tasks. The project is primarily written in C++, the Raylib library for graphical output and additional functionalities. The goal of HEMgine is to show what can be done with enough patience and (no) will.

## Important!
- The emulator does NOT come with a BIOS. You will need to provide your own BIOS file to run the emulator. (This is temporary and will be changed in the future once a stable BIOS is created)
<br>

Here is an example of how to load a BIOS file into the emulator:
```c++
int* fileSize = (int*)malloc(sizeof(int));
uint8_t* BIOS = LoadFileData("resource/yourbiosfile.leaf", fileSize);
SetDefaultProtectedMemory((uint64_t)*fileSize);
LoadIntoMemory(BIOS, 0x0, *fileSize, false, true); // Loads bios and forces past read-only protection
InitVCPU();
```

## Repository Structure

- **include/**: Header files defining the core structures and functions of the emulator.
  - `basics.h`: Contains basic definitions and utility functions used throughout the project.
  - `debuggerprint.h`: Provides functions for debugging output, facilitating easier troubleshooting.
  - `idt.h`: Manages the Interrupt Descriptor Table (IDT) for handling hardware interrupts.
  - `printer.h`: Utilities for formatted output and logging.
  - `raylib.h`, `raymath.h`, `rcamera.h`, `rgestures.h`, `rlgl.h`: Header files from the Raylib library, providing graphics, math, camera, gestures, and OpenGL functionality.
  - `vcpu.h`: Defines the structure and operations of the virtual CPU used in the emulator.

- **lib/**: Library files used by the emulator.
  - `libraylib.a`: Static library for Raylib, used for rendering graphics and handling various multimedia tasks.

- **resource/**: Resource files used by the emulator.
  - `HEM.png`: Logo file, used for branding.
  - `MD.ttf`: A TrueType font file, for rendering text in the emulator (VGA).

- **src/**: Source code files implementing the core functionalities of the emulator.
  - `basics.cpp`, `basics.o`: Implementation of basic definitions and utility functions.
  - `debuggerprint.cpp`, `debuggerprint.o`: Implementation of debugging output functions.
  - `idt.cpp`, `idt.o`: Implementation of the Interrupt Descriptor Table management.
  - `main.cpp`, `main.o`: The main entry point of the emulator, initializing and running the emulation.
  - `printer.cpp`, `printer.o`: Implementation of formatted output and logging utilities.
  - `vcpu.cpp`, `vcpu.o`: Implementation of the virtual CPU, handling the emulation of CPU instructions and states.

## License

This project is licensed under the MIT License.
