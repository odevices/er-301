include env.mk

MODNAME := teletype
MODVERSION := 0.6.0
src_dir = $(mods_dir)/$(MODNAME)
includes += $(mods_dir) $(arch_dir) $(top_dir) $(lua_dir)

include mod-builder.mk
