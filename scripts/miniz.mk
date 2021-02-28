include env.mk

LIBNAME := miniz
src_dir = $(miniz_dir)
includes += $(miniz_dir) $(arch_dir) $(top_dir)

include lib-builder.mk
