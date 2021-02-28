# Override default optimization flags to reduce size.
CFLAGS.release = -Os -Wall
CFLAGS.debug = -Os -Wall -DBUILDOPT_TESTING
CFLAGS.testing = -Os -Wall -DBUILDOPT_TESTING
include env.mk
include utils.mk

program_name := pbl
program_dir := $(top_dir)/$(program_name)
out_dir := $(build_dir)/$(program_name)
pbl_hal_dir = $(top_dir)/$(program_name)/hal

src_dirs := $(program_dir)

includes += $(top_dir) $(pbl_hal_dir) $(arch_dir)
symbols += BUILDOPT_PBL
libraries :=

# sysbios configuration
XDCLOCAL = $(sysbios_dir)/platforms
XDCPATH = $(bios_install_dir);$(uia_install_dir);$(XDCLOCAL);
sysbios_target = gnu.targets.arm.A8F
sysbios_platform = am335x_SRAM
sysbios_buildtype = release
sysbios_build_dir = $(out_dir)/sysbios
sysbios_objects = pbl.cfg 

# Recursive search for source files
cpp_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.cpp)) 
c_sources := $(foreach D,$(src_dirs),$(call rwildcard,$D,*.c)) 
c_sources += $(ti_dir)/am335x/csl_i2c.c
c_sources += $(ti_dir)/am335x/gpio_lld.c
c_sources += $(ti_dir)/am335x/mmcsd_lld.c
c_sources += $(ti_dir)/am335x/uart.c
c_sources += $(ti_dir)/am335x/wdt_lld.c
c_sources += $(arch_hal_dir)/uart.c

objects := $(subst $(top_dir),$(out_dir),$(c_sources:%.c=%.o) $(cpp_sources:%.cpp=%.o)) 

CFLAGS += $(sysbios_cflags) -DPBL_VERSION=\"2.0-$(PROFILE)\"
LFLAGS = $(sysbios_lflags) -Wl,--gc-sections -lc -lnosys

# Set search path for prerequisites
vpath %.c $(top_dir)
vpath %.cpp $(top_dir)
vpath %.cfg $(sysbios_dir)

all: $(out_dir)/MLO

$(objects): $(sysbios_build_dir)/.timestamp

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

$(out_dir)/MLO: $(out_dir)/$(program_name).bin
	@echo TIIMAGE $(describe_target)
	@$(TIIMAGE) 0x402F0400 MMCSD $< $@	

clean:
	rm -rf $(out_dir)

addr2line: $(out_dir)/$(program_name).elf
	@echo $(describe_env) Find ${ADDRESS} in $(out_dir)/$(program_name).elf
	@$(ADDR2LINE) -p -f -i -C -e $(out_dir)/$(program_name).elf -a $(ADDRESS)

FLASHFILE ?= $(out_dir)/$(program_name).elf
flash: all
	@echo $(describe_env) Flashing $(FLASHFILE)
	@$(uniflash_install_dir)/dslite.sh --config=uniflash/ER-301-USB560v2.ccxml --verbose --core=1 $(FLASHFILE)

include rules.mk
