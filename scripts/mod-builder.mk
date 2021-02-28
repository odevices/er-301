## Common rules for building mods.

## Required
# MODNAME := core
# src_dir = $(mods_dir)/$(MODNAME)

## Optional
# MODVERSION := 1.0.0
# asset_dir = $(src_dir)/assets
# includes += . $(mods_dir) $(arch_dir) $(ne10_dir)/inc $(lua_dir)
# symbols +=
# excludes =

###################################################
include scripts/utils.mk

LIBNAME := lib$(MODNAME)
MODVERSION ?= 0.0.0
out_dir = $(build_dir)/mods
asset_dir ?= $(src_dir)/assets
lib_file = $(out_dir)/$(MODNAME)/$(LIBNAME).so
all_imports_file = $(out_dir)/$(MODNAME)/imports.txt
missing_imports_file = $(out_dir)/$(MODNAME)/missing.txt
package_file = $(out_dir)/$(MODNAME)-$(MODVERSION).pkg

# Get the parent of the src dir (without the trailing slash)
parent_dir := $(dir $(src_dir))
parent_dir := $(parent_dir:/=)

# search for package assets
assets := $(call rwildcard, $(src_dir)/assets, *)

# Search for sources
c_sources := $(call rwildcard, $(src_dir), *.c)
cpp_sources := $(call rwildcard, $(src_dir), *.cpp)
s_sources := $(call rwildcard, $(src_dir), *.S)

# Exclude specific sources
c_sources := $(filter-out $(foreach x,$(excludes),%$x), $(c_sources))
cpp_sources := $(filter-out $(foreach x,$(excludes),%$x), $(cpp_sources))
s_sources := $(filter-out $(foreach x,$(excludes),%$x), $(s_sources))

objects := $(subst $(parent_dir),$(out_dir),$(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o) $(s_sources:%.S=%.o)) 

# Manually add objects for swig wrappers 
objects += $(out_dir)/$(MODNAME)/$(MODNAME)_swig.o

# Set search path for prerequisites
vpath %.c $(parent_dir)
vpath %.cpp $(parent_dir)
vpath %.S $(parent_dir)
vpath %.c.swig $(parent_dir)
vpath %.cpp.swig $(parent_dir)

# Linker flags

ifeq ($(ARCH),am335x)
LFLAGS = -nostdlib -nodefaultlibs -r
endif

ifeq ($(ARCH),linux)
LFLAGS = -shared
endif

# Prevent swig from placing symbols exported by mods in the global namespace.
SWIGFLAGS += -nomoduleglobal -small -fvirtual

all: $(package_file)

$(lib_file): $(objects)
	@echo $(describe_env) LINK $(describe_target)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $(objects) $(LFLAGS)
	@$(SIZE) $@	

$(package_file): $(lib_file) $(assets)
	@echo $(describe_env) ZIP $(describe_target)
	@rm -f $@
	@echo $(describe_env) + $(yellowON)"$(asset_dir)/*"$(yellowOFF)
	@cd $(asset_dir) && zip -FSrq $(abspath $@) *
	@echo $(describe_env) + $(describe_input)
	@zip -jq $@ $<

# Check for any undefined symbols that are not exported by app.
missing: 
	@echo $(describe_env) NM $(lib_file)
	@$(NM) --undefined-only --format=posix $< | awk '{print $$1;}' > $@
	@$(NM) --undefined-only --format=posix $< | awk '{print $$1;}'
	@sort -u $(build_dir)/app/exports.sym | comm -23 $< - > $@
#	@[ ! -s $@ ] || echo "Missing Symbols:"
#	@cat $@

list: $(package_file)
	unzip -l $(package_file)

install: $(package_file)
	@echo $(describe_env) INSTALL $(describe_input) to $(front_card_dir)
	@cp -f $(package_file) $(front_card_dir)/ER-301/packages && sync

clean:
	rm -rf $(out_dir)/$(MODNAME) $(package_file)

include scripts/rules.mk
