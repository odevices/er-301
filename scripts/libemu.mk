include scripts/env.mk
include scripts/utils.mk

program_name := libemu
program_dir := $(program_name)
out_dir := $(build_dir)/$(program_name)

src_dirs := $(program_dir) $(hal_dir) $(arch_dir)/$(ARCH) $(od_dir) $(ti_dir)
includes += $(program_dir) $(lua_dir) $(lodepng_dir) $(miniz_dir) $(libs_dir)/SDL_FontCache

libraries :=
libraries += $(libs_build_dir)/lib$(lua_name).a
libraries += $(libs_build_dir)/liblodepng.a
libraries += $(libs_build_dir)/libminiz.a

# Recursive search for source files
cpp_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.cpp)) 
c_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.c)) 

objects := $(addprefix $(out_dir)/,$(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o)) 

# Manually add objects 
objects += $(out_dir)/od/glue/app_swig.o
objects += $(out_dir)/emu/emu_swig.o
objects += $(out_dir)/libs/SDL_FontCache/SDL_FontCache.o

ifeq ($(ARCH),linux)
LFLAGS += --gc-sections
endif

ifeq ($(ARCH),darwin)
# Locate our deps using brew
sdl2 := $(shell brew --prefix sdl2)
sdl2_ttf := $(shell brew --prefix sdl2_ttf)
fftw := $(shell brew --prefix fftw)

CFLAGS += -I$(sdl2)/include -I$(sdl2)/include/SDL2 -I$(sdl2_ttf)/include
LFLAGS += -L$(sdl2)/lib -L$(sdl2_ttf)/lib -L$(fftw)/lib
endif

CFLAGS += -fpic
LFLAGS += -lSDL2 -lSDL2_ttf -lfftw3f -lm -ldl -lstdc++ 

all: $(out_dir)/$(program_name).so

$(out_dir)/$(program_name).so: $(objects) $(libraries)
	@mkdir -p $(@D)	
	@echo $(describe_env) LINK $(describe_target)
	@$(CC) $(CFLAGS) -o $@ $(objects) $(libraries) $(LFLAGS)

clean:
	rm -rf $(out_dir)	

# WARNING. This will overwrite the shared lib build dir.  Needs to be fixed.
libs: 
	+CFLAGS=-fpic $(MAKE) -f lua.mk
	+CFLAGS=-fpic $(MAKE) -f miniz.mk
	+CFLAGS=-fpic $(MAKE) -f lodepng.mk

clean-libs: 
	+$(MAKE) -f lua.mk clean
	+$(MAKE) -f miniz.mk clean
	+$(MAKE) -f lodepng.mk clean

include scripts/rules.mk
