# Build Tools for Linux
CC := gcc-10 -fdiagnostics-color -fmax-errors=5 -I/usr/local/Cellar/sdl2/2.0.14/include/ -I/usr/local/Cellar/sdl2_ttf/2.0.15/include/ -I/usr/local/Cellar/sdl2/2.0.14/include/SDL2/ -rdynamic
CPP := g++-10 -fdiagnostics-color -fmax-errors=5 -I/usr/local/Cellar/sdl2/2.0.14/include/ -I/usr/local/Cellar/sdl2_ttf/2.0.15/include/ -I/usr/local/Cellar/sdl2/2.0.14/include/SDL2/ -lm
OBJCOPY := objcopy
OBJDUMP := objdump
ADDR2LINE := addr2line
LD := gcc-10 -fdiagnostics-color
AR := gcc-ar-10
SIZE := size
STRIP := strip
READELF := readelf
NM := nm
SWIG := swig
PYTHON := python3
ZIP := zip
