# Raspberry Pi Pico Template Project

There are 2 options for compiling this package:
1. By using [VS Code Extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico) and right away importing sources as Pico Project, no initialisation needed, works fine on Windows 10, allows simple SWD debug using Debug Probe;
2. Manual compiling the package with CMake and Make, **pico-sdk** submodule MUST be fetched before building the project, works fine on Ubuntu 22.04.5

            git submodule update --init --recursive

If, while working with VS Code Extension, an error occurs upon starting debugging with message of this kind: 
<br>***Unable to find executable file at C:/Path/To/Project/build/${PROJECT_NAME}.elf***

Import the project and enable CMake Tools, to do that:
1. Open "Import Project" window;
2. Click "Show Advanced Options";
3. Check **"Enable CMake-Tools extension integration"**;
4. Set **"Pico"** kit as CMake Kit, defining C and CXX compilers.
