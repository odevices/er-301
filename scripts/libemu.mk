ARCH=linux
include env.mk
include utils.mk

program_name := libemu
program_dir := $(top_dir)/$(program_name)
out_dir := $(build_dir)/$(program_name)

src_dirs := $(program_dir) $(hal_dir) $(arch_hal_dir) $(od_dir) $(ti_dir)
includes += $(top_dir) $(arch_dir) $(lua_dir) $(lodepng_dir) $(miniz_dir) $(libs_dir)/SDL_FontCache

libraries :=
libraries += $(libs_build_dir)/lib$(lua_name).a
libraries += $(libs_build_dir)/liblodepng.a
libraries += $(libs_build_dir)/libminiz.a

# Recursive search for source files
cpp_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.cpp)) 
c_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.c)) 

objects := $(subst $(top_dir),$(out_dir),$(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o)) 

# Manually add objects 
objects += $(out_dir)/od/glue/app_swig.o
objects += $(out_dir)/emu/emu_swig.o
objects += $(out_dir)/libs/SDL_FontCache/SDL_FontCache.o
 
CFLAGS += -fpic
LFLAGS = -shared -Wl,--gc-sections -lSDL2 -lSDL2_ttf -lfftw3f -lm -ldl -lstdc++ 

# Set search path for prerequisites
vpath %.c $(top_dir)
vpath %.cpp $(top_dir)
vpath %.c.swig $(top_dir)
vpath %.cpp.swig $(top_dir)
vpath %.cfg $(sysbios_dir)

all: $(out_dir)/$(program_name).so

$(out_dir)/$(program_name).so: $(objects) $(libraries)
	@mkdir -p $(@D)	
	@echo $(describe_env) LINK $(describe_target)
	@$(CC) $(CFLAGS) -o $@ $(objects) $(libraries) $(LFLAGS)

clean:
	rm -rf $(out_dir)	

# WARNING. This will overwrite the shared lib build dir.  Needs to be fixed.
libs: 
	+CFLAGS=-fpic $(MAKE) -f lua.mk ARCH=linux
	+CFLAGS=-fpic $(MAKE) -f miniz.mk ARCH=linux
	+CFLAGS=-fpic $(MAKE) -f lodepng.mk ARCH=linux

clean-libs: 
	+$(MAKE) -f lua.mk clean ARCH=linux
	+$(MAKE) -f miniz.mk clean ARCH=linux
	+$(MAKE) -f lodepng.mk clean ARCH=linux

include rules.mk
