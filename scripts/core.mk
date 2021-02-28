include env.mk

MODNAME := core
MODVERSION := 0.6.0
src_dir = $(mods_dir)/$(MODNAME)
asset_dir = $(src_dir)/assets

includes += $(mods_dir) $(arch_dir) $(top_dir) $(ne10_dir)/inc $(lua_dir)

include mod-builder.mk
