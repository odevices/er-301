include scripts/env.mk
include scripts/utils.mk

program_name := sbl
program_dir := $(program_name)
out_dir := $(build_dir)/$(program_name)

src_dirs := $(program_dir) $(hal_dir) $(arch_dir)/$(ARCH)/hal $(ti_dir)

includes += $(ne10_dir)/inc
symbols += BUILDOPT_SBL
libraries :=

# sysbios configuration
XDCLOCAL = $(sysbios_dir)/platforms
XDCPATH = $(bios_install_dir);$(uia_install_dir);$(XDCLOCAL);
sysbios_target = gnu.targets.arm.A8F
sysbios_platform = am335x_DDR3_512MB
sysbios_buildtype = release
sysbios_build_dir = $(out_dir)/sysbios
sysbios_objects = $(sysbios_dir)/sbl.cfg 

# Recursive search for source files
cpp_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.cpp)) 
c_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.c)) 

cpp_sources += $(od_dir)/extras/ReferenceCounted.cpp
cpp_sources += $(od_dir)/graphics/FrameBuffer.cpp
cpp_sources += $(od_dir)/graphics/MainFrameBuffer.cpp
cpp_sources += $(od_dir)/graphics/SubFrameBuffer.cpp
cpp_sources += $(od_dir)/ui/Job.cpp
cpp_sources += $(od_dir)/ui/JobQueue.cpp
cpp_sources += $(od_dir)/extras/FileReader.cpp
cpp_sources += $(od_dir)/extras/Random.cpp
c_sources += $(od_dir)/graphics/fonts/pixelsix48.c

objects := $(addprefix $(out_dir)/,$(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o)) 

CFLAGS += $(sysbios_cflags) -DSBL_VERSION=\"4.0-$(PROFILE)\"
LFLAGS = $(sysbios_lflags) -Wl,--gc-sections -lm -lstdc++ -lc -lnosys -u _printf_float

all: $(out_dir)/SBL

$(objects): $(sysbios_build_dir)/.timestamp scripts/env.mk scripts/sbl.mk

$(sysbios_build_dir)/.timestamp: $(sysbios_objects)
	@echo $(describe_env) XS $<
	@$(XS) -t $(sysbios_target) -p $(sysbios_platform) -r $(sysbios_buildtype) -o $(sysbios_build_dir) $<
	@touch $(sysbios_build_dir)/.timestamp

$(out_dir)/$(program_name).elf: $(objects) $(libraries)
	@echo $(describe_env) LINK $(describe_target)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $(objects) $(libraries) $(LFLAGS)

$(out_dir)/$(program_name).bin: $(out_dir)/$(program_name).elf
	@echo $(describe_env) OBJCOPY $(describe_target)
	@mkdir -p $(@D)
	@$(SIZE) $<
	@$(OBJCOPY) -O binary $< $@

$(out_dir)/SBL: $(out_dir)/$(program_name).bin
	@echo $(describe_env) TIIMAGE $(describe_target)
	@$(TIIMAGE) 0x90000000 MMCSD $< $@	

clean:
	rm -rf $(out_dir)

addr2line: $(out_dir)/$(program_name).elf
	@echo $(describe_env) Find ${ADDRESS} in $(out_dir)/$(program_name).elf
	@$(ADDR2LINE) -p -f -i -C -e $(out_dir)/$(program_name).elf -a $(ADDRESS)

FLASHFILE ?= $(out_dir)/$(program_name).elf
flash: all
	@echo $(describe_env) Flashing $(FLASHFILE)
	@$(uniflash_install_dir)/dslite.sh --config=scripts/uniflash/ER-301-USB560v2.ccxml --verbose --core=1 $(FLASHFILE)

include scripts/rules.mk
