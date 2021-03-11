## Example Setup

## Required
# LIBNAME := miniz
# src_dir = $(miniz_dir)

## Optional
# includes += $(miniz_dir)
# symbols +=

###################################################
include scripts/utils.mk

OUTPUT := lib$(LIBNAME).a
out_dir = $(libs_build_dir)

# Get the parent of the src dir (without the trailing slash)
parent_dir := $(dir $(src_dir))
parent_dir := $(parent_dir:/=)

# Search for sources
c_sources ?= $(call rwildcard, $(src_dir), *.c)
cpp_sources ?= $(call rwildcard, $(src_dir), *.cpp)
s_sources ?= $(call rwildcard, $(src_dir), *.S)

# Exclude specific sources
c_sources := $(filter-out $(foreach x,$(excludes),%$x), $(c_sources))
cpp_sources := $(filter-out $(foreach x,$(excludes),%$x), $(cpp_sources))
s_sources := $(filter-out $(foreach x,$(excludes),%$x), $(s_sources))

objects := $(subst $(parent_dir),$(out_dir),$(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o) $(s_sources:%.S=%.o)) 

# Set search path for prerequisites
vpath %.c $(parent_dir)
vpath %.cpp $(parent_dir)
vpath %.S $(parent_dir)

all: $(out_dir)/$(OUTPUT)

$(objects): scripts/env.mk scripts/lib-builder.mk

$(out_dir)/$(OUTPUT): $(objects)
	@echo $(describe_env) LINK $(describe_target)
	@$(AR) rcs $@ $(objects)

clean:
	rm -rf $(out_dir)/$(LIBNAME) $(out_dir)/$(OUTPUT)

include scripts/rules.mk
