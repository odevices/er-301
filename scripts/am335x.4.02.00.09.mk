# Designed for processor_sdk_rtos_am335x_4_02_00_09.

# TI RTOS SDK install root
TI_INSTALL_DIR := /home/clarkson/ti
TI2_INSTALL_DIR := /home/clarkson/ti-4.02.00.09

# TI RTOS SDK tools
gcc_install_dir := $(TI2_INSTALL_DIR)/gcc-arm-none-eabi-6-2017-q1-update
xdc_install_dir := $(TI2_INSTALL_DIR)/xdctools_3_50_03_33_core

# TI RTOS SDK packages
bios_install_dir := $(TI2_INSTALL_DIR)/bios_6_52_00_12/packages
edma_install_dir := $(TI2_INSTALL_DIR)/edma3_lld_2_12_05_30B/packages
pdk_install_dir := $(TI2_INSTALL_DIR)/pdk_am335x_1_0_9/packages
uia_install_dir := 
uniflash_install_dir := $(TI_INSTALL_DIR)/uniflash_5.3.1

# Use the C system libraries shipped with BIOS (not the toolchain).
sysbios_cflags = @$(sysbios_build_dir)/compiler.opt --specs=nano.specs \
	-I$(gcc_install_dir)/arm-none-eabi/include/newlib-nano
sysbios_lflags = -nostartfiles -static \
	-Wl,-T,$(sysbios_build_dir)/linker.cmd \
	-L$(bios_install_dir)/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/hard

# Set compiler tools
CC := $(gcc_install_dir)/bin/arm-none-eabi-gcc -fdiagnostics-color
CPP := $(gcc_install_dir)/bin/arm-none-eabi-g++ -fdiagnostics-color
OBJCOPY := $(gcc_install_dir)/bin/arm-none-eabi-objcopy
OBJDUMP := $(gcc_install_dir)/bin/arm-none-eabi-objdump
ADDR2LINE := $(gcc_install_dir)/bin/arm-none-eabi-addr2line
LD := $(gcc_install_dir)/bin/arm-none-eabi-gcc -fdiagnostics-color
AR := $(gcc_install_dir)/bin/arm-none-eabi-gcc-ar
SIZE := $(gcc_install_dir)/bin/arm-none-eabi-size
TIIMAGE := $(pdk_install_dir)/ti/starterware/tools/ti_image/tiimage_bin/tiimage
SWIG := swig
XS = $(xdc_install_dir)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c $(gcc_install_dir)
PYTHON := python3