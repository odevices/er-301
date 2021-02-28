include scripts/env.mk

LIBNAME := lodepng
src_dir = $(lodepng_dir)
includes += $(lodepng_dir)

include scripts/lib-builder.mk
