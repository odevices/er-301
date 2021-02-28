# Designed for processor_sdk_rtos_am335x_4_01_00_06.

# TI RTOS SDK install root
TI_INSTALL_DIR ?= $(HOME)/ti

# TI RTOS SDK tools
gcc_install_dir := $(TI_INSTALL_DIR)/gcc-arm-none-eabi-4_9-2015q3
xdc_install_dir := $(TI_INSTALL_DIR)/xdctools_3_32_01_22_core

# TI RTOS SDK packages
bios_install_dir := $(TI_INSTALL_DIR)/bios_6_46_05_55/packages
edma_install_dir := $(TI_INSTALL_DIR)/edma3_lld_2_12_05_29/packages
pdk_install_dir := $(TI_INSTALL_DIR)/pdk_am335x_1_0_8/packages
uia_install_dir := 
uniflash_install_dir := /home/clarkson/ti-other/uniflash_5.3.1

# Use the C system libraries shipped with BIOS (not the toolchain).
sysbios_cflags = @$(sysbios_build_dir)/compiler.opt
sysbios_lflags = -nostartfiles -static \
	-Wl,-T,$(sysbios_build_dir)/linker.cmd \
	-L$(bios_install_dir)/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/fpu \
	-L$(gcc_install_dir)/lib/gcc/arm-none-eabi/4.9.3/fpu -lgcc \
	-L$(gcc_install_dir)/arm-none-eabi/lib/fpu -lstdc++

# Set compiler tools
CC := $(gcc_install_dir)/bin/arm-none-eabi-gcc -fdiagnostics-color -fmax-errors=5
CPP := $(gcc_install_dir)/bin/arm-none-eabi-g++ -fdiagnostics-color -fmax-errors=5
OBJCOPY := $(gcc_install_dir)/bin/arm-none-eabi-objcopy
OBJDUMP := $(gcc_install_dir)/bin/arm-none-eabi-objdump
ADDR2LINE := $(gcc_install_dir)/bin/arm-none-eabi-addr2line
LD := $(gcc_install_dir)/bin/arm-none-eabi-gcc -fdiagnostics-color
AR := $(gcc_install_dir)/bin/arm-none-eabi-gcc-ar
SIZE := $(gcc_install_dir)/bin/arm-none-eabi-size
STRIP := $(gcc_install_dir)/bin/arm-none-eabi-strip
READELF := $(gcc_install_dir)/bin/arm-none-eabi-readelf
NM := $(gcc_install_dir)/bin/arm-none-eabi-nm
TIIMAGE := $(pdk_install_dir)/ti/starterware/tools/ti_image/tiimage_bin/tiimage
SWIG := swig
XS = $(xdc_install_dir)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c $(gcc_install_dir)
PYTHON := python3
ZIP := zip