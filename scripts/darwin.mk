# Build Tools for OSX

# If the processor arch is arm (aka Apple M1) use gcc@11 instead. At the moment
# there isn't a gcc@10 build available for the M1 chip and even worse the
# gcc@11 install from homebrew can't seem to find certain header files...
#
# For now this will have to do, may want to update this in the future if
# eventually they can use the same version.
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