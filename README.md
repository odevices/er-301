# ER-301 Sound Computer

## Table of Contents

  * [Introduction](#introduction)
  * [Emulator](#emulator)
    + [Running the emulator](#running-the-emulator)
    + [Installing packages](#installing-packages)
    + [Limitations](#limitations)
  * [Creating your own mods](#creating-your-own-mods)
    + [Install the TI Processor SDK for AM335x](#install-the-ti-processor-sdk-for-am335x)
    + [Other required dependencies](#other-required-dependencies)
    + [Test your build environment](#test-your-build-environment)
  * [Build Profiles and Target Architectures](#build-profiles-and-target-architectures)

## Introduction

This project contains the source code for:
* primary boot loader (MLO)
* secondary boot loader (SBL)
* firmware (kernel.bin)
* emulator (Linux only)
* factory mods (core, teletype)
* SDK for building your own mods

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

### Running the emulator

To run the emulator, execute the following from the top directory:
```bash
testing/linux/emu/emu.elf ./xroot ~/.od/rear ~/.od/front
```

In the above, you are telling the emulator the location of the following 3 required directories:
* lua scripts file tree (Usually the xroot folder of this project.)
* file tree for the rear SD card (This will be created if it doesn't exist.)
* file tree for the front SD card (This will be created if it doesn't exist.)

Since you are essentially booting up the ER-301 with empty (virtual) SD cards, it will populate the file trees with the default files but you will not have the core mod installed.  

### Installing packages

To compile and create the installation package for the core mod, execute:

```bash
make core ARCH=linux
```

Next, copy the resulting ```testing/linux/mods/core-x.x.x.pkg``` file to the ```ER-301/packages``` directory of the emulator's front SD card:

```bash
cp testing/linux/mods/core-0.6.0.pkg ~/.od/front/ER-301/packages
```

Finally, run the emulator again using the same command-line as before and install the core mod package from the Package Manager screen.  Now you will have access to all of the core units in the emulator.

### Limitations

Currently, the 4 output channels are mixed down to stereo for maximal compatibility with different setups.

* Left channel: OUT1+OUT3
* Right channel: OUT2+OUT4

There are no external input channels implemented yet.  However, when it comes to the main purpose of this emulator, it has really been a necessity since you can generate just about any testing signals that you need within the ER-301 emulation.

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

### Test your build environment

A good test of your build environment is to see if you can successfully build the factory mods:

```bash
make core teletype
```

## Build Profiles and Target Architectures

All build outputs are placed in a sub folder composed of the build profile and the target architecture.  There are 3 build profiles:

* testing: All optimizations and logging enabled.
* debug: No optimizations.  Logging enabled.
* release: All optimizations enabled.  Logging disabled.

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