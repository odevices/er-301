# Build Tools for OSX
GCC_VERSION = 10
ifeq ($(shell uname -p),arm)
  GCC_VERSION = 11
endif

CC := gcc-$(GCC_VERSION) -fdiagnostics-color -fmax-errors=5
CPP := g++-$(GCC_VERSION) -fdiagnostics-color -fmax-errors=5
OBJCOPY := objcopy
OBJDUMP := objdump
ADDR2LINE := addr2line
LD := ld
AR := gcc-ar-$(GCC_VERSION)
SIZE := size
STRIP := strip
READELF := readelf
NM := nm
SWIG := swig
PYTHON := python3
ZIP := zip