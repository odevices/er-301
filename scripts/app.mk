include scripts/env.mk
include scripts/utils.mk

program_name := app
program_dir := $(program_name)
xroot_dir := xroot
out_dir := $(build_dir)/$(program_name)
exports_file := $(out_dir)/exports.sym
cleaned_exports_file := $(out_dir)/exports-clean.sym
excluded_symbols_file := $(program_dir)/excluded.sym
extra_symbols_file := $(program_dir)/extra.sym

src_dirs := $(program_dir) $(hal_dir) $(arch_dir)/$(ARCH)/hal $(od_dir) $(ti_dir)

includes += $(lua_dir) $(ne10_dir)/inc $(lodepng_dir) $(miniz_dir)

libraries :=
libraries += $(libs_build_dir)/lib$(lua_name).a
libraries += $(libs_build_dir)/libne10.a
libraries += $(libs_build_dir)/liblodepng.a
libraries += $(libs_build_dir)/libminiz.a

# sysbios configuration
XDCLOCAL = $(sysbios_dir)/platforms
XDCPATH = $(bios_install_dir);$(uia_install_dir);$(XDCLOCAL);
sysbios_target = gnu.targets.arm.A8F
sysbios_platform = am335x_DDR3_512MB
sysbios_buildtype = release
sysbios_build_dir = $(out_dir)/sysbios
#sysbios_objects = er301.cfg
sysbios_objects = $(sysbios_dir)/$(sysbios_buildtype).cfg 
sysbios_objects += $(sysbios_dir)/common.cfg $(XDCLOCAL)/linkcmd_er301.xdt
	 
# Recursive search for source files
cpp_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.cpp)) 
c_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.c)) 
xroot_files := $(call rwildcard,$(xroot_dir),*)

objects := $(addprefix $(out_dir)/,$(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o)) 

# Manually add objects for swig wrappers and the ramdisk image.
objects += $(out_dir)/od/glue/$(program_name)_swig.o
objects += $(out_dir)/$(program_name)/xroot.o

gcc_std_libs_dir = $(gcc_install_dir)/arm-none-eabi/lib/fpu
bios_std_libs_dir = $(bios_install_dir)/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/fpu

exports := $(objects:%.o=%.sym) 
exports += $(libs_build_dir)/lib$(lua_name).sym
exports += $(libs_build_dir)/bios-libm.sym
exports += $(libs_build_dir)/gcc-libstdc++.sym

# Add symtab.o after exports are calculated.
objects += $(out_dir)/$(program_name)/symtab.o

CFLAGS += $(sysbios_cflags)
CFLAGS += -DFIRMWARE_VERSION=\"$(FIRMWARE_VERSION)\"
CFLAGS += -DFIRMWARE_NAME=\"$(FIRMWARE_NAME)\"
CFLAGS += -DFIRMWARE_STATUS=\"$(FIRMWARE_STATUS)\"
LFLAGS = $(sysbios_lflags) -Wl,--gc-sections -lm -lc -lnosys -u _printf_float 

all: $(out_dir)/kernel.bin $(exports)

$(objects): $(sysbios_build_dir)/.timestamp scripts/env.mk

# Configure and build the sysbios library
$(sysbios_build_dir)/.timestamp: $(sysbios_objects)
	@echo $(describe_env) XS $<
	@$(XS) -t $(sysbios_target) -p $(sysbios_platform) -r $(sysbios_buildtype) -o $(sysbios_build_dir) $<
	@touch $(sysbios_build_dir)/.timestamp

# Collect files from xroot and embed in an object file.
$(out_dir)/$(program_name)/xroot.S: $(xroot_files)
	@echo $(describe_env) GEN $(describe_target)
	@mkdir -p $(@D)
	@$(PYTHON) scripts/ramdisk.py $@ $(xroot_dir)

# Generate a listing of extern symbols from object files.
$(out_dir)/%.sym: $(out_dir)/%.oa8fg
	@echo $(describe_env) GEN $(describe_target)
	@mkdir -p $(@D)
	@$(NM) --extern-only --defined-only --format=posix $< | awk '{print $$1;}' > $@	

# Generate a listing of extern symbols from object files.
$(out_dir)/%.sym: $(out_dir)/%.o
	@echo $(describe_env) GEN $(describe_target)
	@mkdir -p $(@D)
	@$(NM) --extern-only --defined-only --format=posix $< | awk '{print $$1;}' > $@	

# Generate a listing of extern symbols from libraries.
$(libs_build_dir)/%.sym: $(libs_build_dir)/%.a
	@echo $(describe_env) GEN $(describe_target)
	@mkdir -p $(@D)
	@$(NM) --extern-only --defined-only --print-file-name --format=posix $< | awk '{print $$2;}' > $@	

# Generate a listing of extern symbols from std libs in the gcc tree.
$(libs_build_dir)/gcc-%.sym: $(gcc_std_libs_dir)/%.a
	@echo $(describe_env) GEN $(describe_target)
	@mkdir -p $(@D)
	@$(NM) --extern-only --defined-only --print-file-name --format=posix $< | awk '{print $$2;}' > $@	

# Generate a listing of extern symbols from std libs in the bios tree.
$(libs_build_dir)/bios-%.sym: $(bios_std_libs_dir)/%.a
	@echo $(describe_env) GEN $(describe_target)
	@mkdir -p $(@D)
	@$(NM) --extern-only --defined-only --print-file-name --format=posix $< | awk '{print $$2;}' > $@	

# Sort and collect the symbol files into one big file.
$(exports_file): $(exports) $(extra_symbols_file)
	@echo $(describe_env) GEN $(describe_target)
	@sort -u $(exports) $(extra_symbols_file) > $@

# Filter out excluded symbols
$(cleaned_exports_file): $(exports_file) $(excluded_symbols_file)
	@echo $(describe_env) FILTER $(describe_target)
	@sort -u $(excluded_symbols_file) | comm -23 $< - > $@

# Generate symtab.cpp from the listing of exported symbols.
$(out_dir)/$(program_name)/symtab.cpp: $(program_dir)/symtab.cpp.awk $(out_dir)/$(program_name)/symtab.h $(cleaned_exports_file)
	@echo $(describe_env) AWK $(describe_target)
	@sort -u $(cleaned_exports_file) | awk -f $< > $@

# Generate symtab.h from the listing of exported symbols.
$(out_dir)/$(program_name)/symtab.h: $(program_dir)/symtab.h.awk $(cleaned_exports_file)
	@echo $(describe_env) AWK $(describe_target)
	@sort -u $(cleaned_exports_file) | awk -f $< > $@

# Final linking of the ELF executable.
$(out_dir)/$(program_name).elf: $(objects) $(libraries)
	@mkdir -p $(@D)	
	@echo $(describe_env) LINK $(describe_target)
	@$(CC) $(CFLAGS) -o $@ $(objects) $(libraries) $(LFLAGS)
	@echo $(describe_env) NM $(program_name).sym
	@$(NM) --extern-only --defined-only --format=posix $@ | awk '{print $$1;}' > $(out_dir)/$(program_name).sym

# Strip the executable down to a memory-loadable binary.
$(out_dir)/$(program_name).bin: $(out_dir)/$(program_name).elf
	@echo $(describe_env) OBJCOPY $(describe_target)
	@mkdir -p $(@D)
	@$(SIZE) $<
	@$(OBJCOPY) -O binary $< $@

# Add the TI image header to the binary executable.
$(out_dir)/kernel.bin: $(out_dir)/$(program_name).bin
	@echo $(describe_env) TIIMAGE $(describe_target)
	@$(TIIMAGE) 0x80000000 MMCSD $< $@	

clean:
	rm -rf $(out_dir)

clean-syms:
	@rm -rf $(exports)

# Look up the source file and line that corresponds to an address in the binary exe.
addr2line: $(out_dir)/$(program_name).elf
	@echo $(describe_env) Find ${ADDRESS} in $(out_dir)/$(program_name).elf
	@$(ADDR2LINE) -p -f -i -C -e $(out_dir)/$(program_name).elf -a $(ADDRESS)

# Flash the binary using JTAG debugger.
FLASHFILE ?= $(out_dir)/$(program_name).elf
flash: all
	@echo $(describe_env) Flashing $(FLASHFILE)
	@$(uniflash_install_dir)/dslite.sh --config=scripts/uniflash/ER-301-USB560v2.ccxml --verbose --core=1 $(FLASHFILE)

include scripts/rules.mk
