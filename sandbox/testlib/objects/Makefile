######################################################
# Start Configuration

# Choose a build profile.
# profile = release
# profile = debug
profile = testing

# Output filename
output := objects.elf

# List include dirs for the preprocessor
include_dirs := . ../../../sdk

# Any define symbols for the preprocessor
symbols :=

# List of C, CPP, and SWIG source files
c_sources :=
cpp_sources := TestObject.cpp
swig_sources:= objects.cpp.swig

# GCC install location and prefix
gcc4_bin_dir := /home/clarkson/ti-4.01.00.06/gcc-arm-none-eabi-4_9-2015q3/bin
gcc7_bin_dir := /home/clarkson/ti-6.01.00.08/gcc-arm-none-eabi-7-2018-q2-update/bin
gcc_prefix := $(gcc7_bin_dir)/arm-none-eabi

# build tools
CC := $(gcc_prefix)-gcc -fdiagnostics-color -fmax-errors=5
CPP := $(gcc_prefix)-g++ -fdiagnostics-color -fmax-errors=5
SIZE := $(gcc_prefix)-size
SWIG := swig

# End Configuration
######################################################

# Where is this makefile being executed?
cur_dir := .

# Compilation output will be created here.
build_dir := $(cur_dir)/$(profile)

# Construct list of object files from source files.
objects := $(addprefix $(build_dir)/, $(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o) $(swig_sources:%.cpp.swig=%_swig.o))

# arch-specific flags
CFLAGS.arch = -fno-common -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -ffunction-sections -fdata-sections -Werror=implicit-function-declaration -Dfar= -D__DYNAMIC_REENT__

# profile-specific flags
CFLAGS.release := -O3 -Wall -ftree-vectorize -ffast-math
CFLAGS.testing := -DBUILDOPT_TESTING $(CFLAGS.release)
CFLAGS.debug := -Wall 

# Check that the chosen build profile is valid.
ifndef CFLAGS.$(profile)
$(error Error: '$(profile)' is not a valid build profile.)
endif

CFLAGS = $(CFLAGS.arch) $(CFLAGS.$(profile))
CFLAGS += $(addprefix -I,$(include_dirs)) 
CFLAGS += $(addprefix -D,$(symbols))

# Optimize size when compiling the SWIG wrapper source.
CFLAGS.swig = $(CFLAGS.arch) -Os -Wall
CFLAGS.swig += $(addprefix -I,$(include_dirs)) 
CFLAGS.swig += $(addprefix -D,$(symbols))

# Linker flags
LFLAGS = -nostdlib -nodefaultlibs -fvisibility=hidden -r

#### Targets
all: $(build_dir)/$(output)

$(build_dir)/$(output): $(objects)
	@echo LINK $(describe_target)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $(objects) $(LFLAGS)
	@$(SIZE) $@

clean:
	rm -rf $(build_dir)

#### Generic build rules	
DEPFLAGS = -MT $@ -MMD -MP
describe_input = $(subst $(cur_dir)/,"",$<)
describe_target = $(subst $(cur_dir)/,"",$@)

# Set search path for prerequisites (i.e. sources)
vpath %.c $(cur_dir)
vpath %.cpp $(cur_dir)
vpath %.i $(cur_dir)
vpath %.cpp.swig $(cur_dir)

$(build_dir)/%.o: %.c
	@echo C $(describe_input)
	@mkdir -p $(@D)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -std=c11 -c $< -o $@

$(build_dir)/%.o: %.cpp
	@echo C++ $(describe_input)
	@mkdir -p $(@D)
	@$(CPP) $(DEPFLAGS) $(CFLAGS) -std=gnu++11 -c $< -o $@

#### SWIG rules
SWIGFLAGS = -lua -nomoduleglobal -small -MMD -MP $(addprefix -I,$(include_dirs)) 

# Keep this intermediate file around after building.
.PRECIOUS: $(build_dir)/%_swig.cpp

$(build_dir)/%_swig.cpp: %.cpp.swig
	@echo "SWIG(C++)" $(describe_input)
	@mkdir -p $(@D)
	@$(SWIG) $(SWIGFLAGS) -c++ -o $@ $<

#### Rules for files generated in the build directory.
$(build_dir)/%.o: $(build_dir)/%.cpp
	@echo C++ $(describe_input)
	@mkdir -p $(@D)
	@$(CPP) $(DEPFLAGS) $(CFLAGS.swig) -std=gnu++11 -c $< -o $@

#### Detect changes in dependencies
DEPFILES = $(objects:%.o=%.d)
$(DEPFILES): ;
-include $(DEPFILES)