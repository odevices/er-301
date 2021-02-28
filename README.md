# ER-301 Sound Computer

This project contains the source code for:
* primary boot loader (MLO)
* secondary boot loader (SBL)
* firmware (kernel.bin)
* emulator (Linux only)
* factory mods (core, teletype)
* SDK for building your own mods

## Emulator

Let's start with the emulator.  To compile the emulator you will need the following dependencies installed:

```bash
sudo apt install libsdl2-dev libsdl2-ttf libfftw3-dev swig
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

## Creating your own mods


### Install the TI Processor SDK for AM335x
First, you will need to install the TI-RTOS development tools for the AM335x. 
1. It is important that you use this specific version: **04.01.00.06**.  
2. Navigate to [this](https://software-dl.ti.com/processor-sdk-rtos/esd/AM335X/04_01_00_06/index_FDS.html) page and download the file called **ti-processor-sdk-rtos-am335x-evm-04.01.00.06-Linux-x86-Install.bin**.
3. Executing this file will run an installer program.

### Other required dependencies

```bash
sudo apt install swig
```

### Try compiling the core mod

```bash
make core
```

