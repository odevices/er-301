include scripts/env.mk

MODNAME := ladspa
src_dir = $(mods_dir)/$(MODNAME)
includes += $(mods_dir) $(lua_dir) $(mods_dir)/ladspa $(mods_dir)/ladspa/swh

include scripts/mod-builder.mk
