PROFILE = release
ARCH = am335x
include scripts/env.mk

FIRMWARE_VERSION ?= 0.6.00
FIRMWARE_TYPE ?= unstable

firmware_archive = $(build_dir)/er-301-v$(FIRMWARE_VERSION)-$(FIRMWARE_TYPE).zip

firmware_contents = $(build_dir)/app/kernel.bin
firmware_contents += $(build_dir)/sbl/sbl.bin
firmware_contents += $(build_dir)/pbl/MLO
firmware_contents += $(build_dir)/mods/core-$(FIRMWARE_VERSION).pkg
firmware_contents += $(build_dir)/mods/teletype-$(FIRMWARE_VERSION).pkg
firmware_contents += scripts/install.lua

firmware: $(firmware_archive)

$(firmware_archive): $(firmware_contents)
	@echo $(describe_env) ZIP $(describe_target)
	@zip -j $(firmware_archive) $(firmware_contents)

$(build_dir)/app/kernel.bin:
	+$(MAKE) app PROFILE=$(PROFILE) ARCH=$(ARCH) VERSION=$(FIRMWARE_VERSION)

$(build_dir)/sbl/sbl.bin:
	+$(MAKE) sbl PROFILE=$(PROFILE) ARCH=$(ARCH)

$(build_dir)/pbl/MLO:
	+$(MAKE) pbl PROFILE=$(PROFILE) ARCH=$(ARCH)

$(build_dir)/mods/core-$(FIRMWARE_VERSION).pkg:
	+$(MAKE) core PROFILE=$(PROFILE) ARCH=$(ARCH) MODVERSION=$(FIRMWARE_VERSION)

$(build_dir)/mods/teletype-$(FIRMWARE_VERSION).pkg:
	+$(MAKE) teletype PROFILE=$(PROFILE) ARCH=$(ARCH) MODVERSION=$(FIRMWARE_VERSION)

clean:
	rm -rf $(firmware_archive)

	
