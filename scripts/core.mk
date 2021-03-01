include scripts/env.mk

MODNAME := core
src_dir = $(mods_dir)/$(MODNAME)
asset_dir = $(src_dir)/assets

includes += . $(mods_dir) $(arch_dir) $(ne10_dir)/inc $(lua_dir)

include scripts/mod-builder.mk
