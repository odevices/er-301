include scripts/env.mk

LIBNAME := $(lua_name)
src_dir = $(lua_dir)
includes += $(arch_dir)/hal/dynload
excludes = lua.c luac.c luaoslib.c

include scripts/lib-builder.mk
