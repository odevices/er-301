# ER-301 Sound Computer

## Table of Contents

  * [Introduction](#introduction)
  * [Emulator](#emulator)
    + [Running the emulator](#running-the-emulator)
    + [Configuring the emulator](#configuring-the-emulator)
    + [Installing packages](#installing-packages)
    + [Limitations](#limitations)
  * [Creating your own mods](#creating-your-own-mods)
    + [Install the TI Processor SDK for AM335x](#install-the-ti-processor-sdk-for-am335x)
    + [Other required dependencies](#other-required-dependencies)
    + [Test your build environment](#test-your-build-environment)
  * [USB Functions](#usb-functions)
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
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev libfftw3-dev swig python3 zip fonts-freefont-ttf
```

To compile, execute this in the top directory:

```bash
make emu
```

### Running the emulator

Just executing the emulator without arguments will start it up with default settings from the top of the project tree:
```bash
testing/linux/emu/emu.elf
```

This will typically be enough to get started.  If you want to dive in deeper execute with '-h' or --help' to get help on the commandline configuration options:
```bash
> testing/linux/emu/emu.elf --help
ER-301 Emulator (v0.6.02)
Usage: emu [OPTIONS]

Examples:
  emu              # Start emulator with default configuration file.
  emu -c foo.cfg   # Start emulator with 'foo.cfg'.

  -h, --help         Show this help.
  -c, --config FILE  Use the given configuration file.
                     (Default: ~/.od/emu.cfg)

```

:warning: If you do not want to execute the emulator from top of the project tree then you will have to configure the path to the lua scripts file tree (XROOT) before starting the emulator.

### Configuring the emulator

If you run the emulator and it cannot find a valid configuration file then the program will create a default one similar to this in ```~/.od/emu.config```:

```bash
## ER-301 Emulator Configuration

## Uncomment lines below to set your own values.

## Root for the Lua interpreter
XROOT ./xroot

## Use this root for the rear SD card.
REAR_ROOT ~/.od/rear

## Use this root for the front SD card.
FRONT_ROOT ~/.od/front

## Key mapping

BUTTON_MAIN1_KEY Q
BUTTON_MAIN2_KEY W
BUTTON_MAIN3_KEY E
BUTTON_MAIN4_KEY R
BUTTON_MAIN5_KEY T
BUTTON_MAIN6_KEY Y
BUTTON_DIAL1_KEY A
BUTTON_DIAL2_KEY S
BUTTON_DIAL3_KEY D
BUTTON_SUB1_KEY F
BUTTON_SUB2_KEY G
BUTTON_SUB3_KEY H
BUTTON_ENTER_KEY V
BUTTON_UP_KEY B
BUTTON_SHIFT_KEY N
BUTTON_SELECT1_KEY 1
BUTTON_SELECT2_KEY 2
BUTTON_SELECT3_KEY 3
BUTTON_SELECT4_KEY 4
STORAGE_FOCUS_KEY Z
MODE_FOCUS_KEY X
ZOOM_IN_KEY =
ZOOM_OUT_KEY -
QUIT_KEY Q

## Knob mapping

##  Scale factor for mouse wheel. Negate to invert.
MOUSE_WHEEL_FACTOR 0.5

##  Scale factor for LEFT/RIGHT arrow keys. Negate to invert.
LEFT_RIGHT_ARROWS_FACTOR 1

##  Scale factor for UP/DOWN arrow keys. Negate to invert.
UP_DOWN_ARROWS_FACTOR 0.25

```

Edit this file with your own values to configure various items like key mappings, knob speed, and paths for lua scripts and file trees.

### Installing packages

To compile and create the installation package for the core mod, execute:

```bash
make core ARCH=linux
```

Next, copy the resulting package file to the ```ER-301/packages``` directory of the emulator's front SD card.  You will need to change the command below to match the correct version.

```bash
cp testing/linux/mods/core-<version>.pkg ~/.od/front/ER-301/packages
```

Finally, run the emulator again using the same command-line as before and install the core mod package from the Package Manager screen.  Now you will have access to all of the core units in the emulator.

### Limitations

**Outputs are mixed down to stereo**

Currently, the 4 output channels are mixed down to stereo for maximal compatibility with different setups.

* Left channel: OUT1+OUT3
* Right channel: OUT2+OUT4

**No external inputs**

There are no external input channels implemented yet.  However, when it comes to the main purpose of this emulator, it hasn't been much of an obstacle since you can generate just about any testing signals that you need within the ER-301 emulation.  In fact, when it comes to testing, it is usually much easier to deal with canned signals for their repeatibility and ease of setup.

**No I2C control**

The teletype mod will compile and install.  You can even instantiate its units.  However, there is no emulation of the I2C bus.

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

**Note:** If you get a warning message from the installer about a "Problem running post-install step.  Installation may not complete correctly.", you can ignore it.

### Other required dependencies

```bash
sudo apt install swig python3 zip gcc-multilib
```

### Test your build environment

A good test of your build environment is to see if you can successfully build the factory mods:

```bash
make core teletype
```

## USB Functions

There is a USB port on the back of the ER-301.  As of v0.6.00 of the firmware, the ER-301 can enumerate as a mass storage device (which provides access to the front SD card) or a virtual serial port device (the ER-301 will output console messages here when connected).  The settings for USB are located in **admin > General Settings**.  However, I would think of the USB functionality as more developer-targeted for because...

:warning: There is a gotcha with having the ER-301 plugged into a USB host.  It will not boot because it will be waiting for the USB host to give it firmware via BOOTP/TFTP.  This is baked into the AM335x ROM unfortunately.  The solution is to have a BOOTP server always running on your host that either gives it the firmware or tells the ER-301 to give up and boot from the SD card.

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
