include env.mk

LIBNAME := lodepng
src_dir = $(lodepng_dir)
includes += $(lodepng_dir)

include lib-builder.mk
