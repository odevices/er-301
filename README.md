# ER-301 Sound Computer

## Table of Contents

  * [Introduction](#introduction)
  * [Emulator](#emulator)
  * [Creating your own mods](#creating-your-own-mods)
    + [Install the TI Processor SDK for AM335x](#install-the-ti-processor-sdk-for-am335x)
    + [Other required dependencies](#other-required-dependencies)
    + [Try compiling the core mod](#try-compiling-the-core-mod)

## Introduction

This project contains the source code for:
* primary boot loader (MLO)
* secondary boot loader (SBL)
* firmware (kernel.bin)
* emulator (Linux only)
* factory mods (core, teletype)
* SDK for building your own mods

## Build Output

All build outputs are placed in a sub folder composed of the build profile and the target architecture.  There are 3 build profiles:

* testing
* debug
* release

And here are the 2 supported target architectures:

* am335x
* linux

So for example, if I execute the following make command:

```bash
make core PROFILE=debug ARCH=linux
```

Then the build outputs will be placed in the ```debug/linux``` directory of the project root.  Generally, the default profile is **testing** and the default architecture for everything except the emulator is **am335x**.  So if you execute just:

```bash
make core
```

Then the build outputs will appear in the ```testing/am335x``` directory of the project root.  See the [top-level Makefile](Makefile) for more details.

## Emulator

:warning: **The following instructions have been tested on Ubuntu 20.xx.**

Since cross-compilation has its own complexities, it is probably easier to first start with getting the emulator running on your development machine.  To compile and run the emulator you will need the following dependencies installed:

```bash
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev libfftw3-dev swig python3 zip
```

To compile, execute this in the top directory:

```bash
make emu
```

To run the emulator, execute the following from the top directory:
```bash
testing/linux/emu/emu.elf ./xroot ~/.od/rear ~/.od/front
```

In the above, you are telling the emulator the location of the following 3 required directories:
* lua scripts file tree (Usually the xroot folder of this project.)
* file tree for the rear SD card (This will be created if it doesn't exist.)
* file tree for the front SD card (This will be created if it doesn't exist.)

Since you are essentially booting up the ER-301 with empty (virtual) SD cards, it will populate the file trees with the default files but you will not have the core mod installed.  To compile and create the installation package for the core mod, execute:

```bash
make core ARCH=linux
```

Next, copy the resulting ```testing/linux/mods/core-x.x.x.pkg``` file to the ```ER-301/packages``` directory of the emulator's front SD card:

```bash
cp testing/linux/mods/core-0.6.0.pkg ~/.od/front/ER-301/packages
```

Finally, run the emulator again using the same command-line as before and install the core mod package from the Package Manager screen.  Now you will have access to all of the core units in the emulator.

## Creating your own mods

### Install the TI Processor SDK for AM335x
First, you will need to install the TI-RTOS development tools for the AM335x. 
1. It is important that you use this specific version: **04.01.00.06**.  
2. Navigate to [this](https://software-dl.ti.com/processor-sdk-rtos/esd/AM335X/04_01_00_06/index_FDS.html) page and download the file called **ti-processor-sdk-rtos-am335x-evm-04.01.00.06-Linux-x86-Install.bin**.
3. Executing this file will run an installer program.
4. ***When asked, please set the destination folder to ```~/ti```.***  This is the path assumed by the project makefiles.  If instead you install to a different folder then before executing any cross-compilation commands, you will need to set the TI_INSTALL_DIR shell environment variable to the actual path that you used.  If you don't want to change your shell environment then you can also pass the path on the commandline like this:

```bash
make core TI_INSTALL_DIR=~/your-path-to-ti-sdk
```

This documentation will assume that you used the default location.

### Other required dependencies

```bash
sudo apt install swig python3 zip
```

### Try compiling the core mod

```bash
make core
```

