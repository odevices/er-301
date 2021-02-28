PROFILE = testing

top_dir := ../..
bin_dir := .
out_dir := $(bin_dir)/$(PROFILE)

include_dirs := $(top_dir)/common $(top_dir)/firmware $(top_dir)/libs/lua/src $(top_dir)/examples/plugin

# arch-specific
#CFLAGS = -fno-common -mcpu=cortex-a8 -mtune=cortex-a8 -march=armv7-a -marm -mfloat-abi=hard -mfpu=neon -ffunction-sections -fdata-sections -Werror=implicit-function-declaration -Dfar= -D__DYNAMIC_REENT__
CFLAGS.arch = -fno-common -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -ffunction-sections -fdata-sections -Werror=implicit-function-declaration -Dfar= -D__DYNAMIC_REENT__
#CFLAGS.arch += -g -fdebug-prefix-map=/=
# profile-specific
CFLAGS.release := -O3 -Wall -ftree-vectorize -ffast-math
#CFLAGS.release :=  -Wall -ftree-vectorize -g
CFLAGS.release_48kHz_128f := -DBUILDOPT_SAMPLERATE=48000 -DBUILDOPT_FRAME_LENGTH=128 $(CFLAGS.release)
CFLAGS.release_96kHz_128f := -DBUILDOPT_SAMPLERATE=96000 -DBUILDOPT_FRAME_LENGTH=128 $(CFLAGS.release)
CFLAGS.release_48kHz_32f := -DBUILDOPT_SAMPLERATE=48000 -DBUILDOPT_FRAME_LENGTH=32 $(CFLAGS.release)
CFLAGS.testing := -DBUILDOPT_TESTING $(CFLAGS.release_48kHz_128f)
CFLAGS.debug := -Wall -DBUILDOPT_SAMPLERATE=48000 -DBUILDOPT_FRAME_LENGTH=128

ifndef CFLAGS.$(PROFILE)
$(error Error: '$(PROFILE)' is not a valid build profile.)
endif

include $(top_dir)/build/tests/tools.4.01.00.06.mk

CFLAGS = $(CFLAGS.arch) $(CFLAGS.$(PROFILE))
CFLAGS += $(addprefix -I,$(include_dirs)) 
CFLAGS += $(addprefix -D,$(symbols))

CFLAGS.swig := $(CFLAGS.arch) -Os -Wall
CFLAGS.swig += $(addprefix -I,$(include_dirs)) 
CFLAGS.swig += $(addprefix -D,$(symbols))

OUTPUT = test
# Other potentially useful linker flags: -nostartfiles -nodefaultlibs
# when building a shared library: -shared -Wl,--version-script,version.map
LFLAGS = -nostdlib -nodefaultlibs -r -Wl,-Map,$(out_dir)/$(OUTPUT).map
#objects = $(out_dir)/testC.o $(out_dir)/testCpp.o $(out_dir)/testStdCpp.o $(out_dir)/testObject.o $(out_dir)/testPlugin.o
#objects = $(out_dir)/testC.o $(out_dir)/testCpp.o $(out_dir)/testPlugin.o
#objects = $(out_dir)/testC.o $(out_dir)/testPlugin.o
objects = $(out_dir)/testObject.o $(out_dir)/testPlugin.o

# Manually add objects for swig wrappers
objects += $(out_dir)/testPlugin_swig.o
		 
# Set search path for prerequisites
vpath %.c $(top_dir)
vpath %.cpp.swig $(top_dir)

all: $(out_dir)/$(OUTPUT).plugin

$(out_dir)/$(OUTPUT).plugin: $(out_dir)/$(OUTPUT).elf
	@echo ZIP $(describe_target)
	@zip -j -q $@ $< 

$(out_dir)/$(OUTPUT).elf: $(objects) version.map
	@echo LINK $(describe_target)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $(objects) $(LFLAGS)
	@$(SIZE) $@

install: $(out_dir)/$(OUTPUT).elf
	@echo INSTALL $(describe_input)
	@cp $(out_dir)/$(OUTPUT).elf $(top_dir)/ramdisk/root

clean:
	@rm -rf $(out_dir) $(top_dir)/ramdisk/root/$(OUTPUT).elf

DEPFLAGS = -MT $@ -MMD -MP
describe_input = $(subst $(top_dir)/,"",$<)
describe_target = $(subst $(top_dir)/,"",$@)

$(out_dir)/%.o: %.c
	@echo C $(describe_input)
	@mkdir -p $(@D)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -std=c11 -c $< -o $@
#	@# Here is how to generate assembly next to the object files for viewing	
#	@echo GEN ${@:.o=.S} 
#	@$(OBJDUMP) -d -l -S $@ > ${@:.o=.S}  

$(out_dir)/%.o: %.cpp
	@echo C++ $(describe_input)
	@mkdir -p $(@D)
	@$(CPP) $(DEPFLAGS) $(CFLAGS) -std=gnu++11 -c $< -o $@

# SWIG
swig_flags := -lua -nomoduleglobal -small -MMD -MP

.PRECIOUS: $(out_dir)/%_swig.cpp

$(out_dir)/%_swig.cpp: %.cpp.swig
	@echo "SWIG(C++)" $(describe_input)
	@mkdir -p $(@D)
	@$(SWIG) $(swig_flags) -c++ -I$(top_dir)/firmware -I$(top_dir)/common -o $@ $<

# Rules for files generated in the build directory.

$(out_dir)/%.o: $(out_dir)/%.cpp
	@echo C++ $(describe_input)
	@mkdir -p $(@D)
	@$(CPP) $(DEPFLAGS) $(CFLAGS.swig) -std=gnu++11 -c $< -o $@

# Dependencies

DEPFILES = $(objects:%.o=%.d)
$(DEPFILES): ;
-include $(DEPFILES)