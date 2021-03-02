PROFILE = release
ARCH = am335x
include scripts/env.mk

firmware_archive = $(build_dir)/er-301-v$(FIRMWARE_VERSION)-$(FIRMWARE_STATUS).zip

firmware_contents = $(build_dir)/app/kernel.bin
firmware_contents += $(build_dir)/sbl/SBL
firmware_contents += $(build_dir)/pbl/MLO
firmware_contents += $(build_dir)/mods/core-$(FIRMWARE_VERSION).pkg
firmware_contents += $(build_dir)/mods/teletype-$(FIRMWARE_VERSION).pkg
firmware_contents += scripts/install.lua

$(firmware_archive): $(firmware_contents)
	@echo $(describe_env) ZIP $(describe_target)
	@rm -rf $(firmware_archive)
	@$(ZIP) -j $(firmware_archive) $(firmware_contents)	

$(firmware_contents): binaries

binaries:
	+$(MAKE) app PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) sbl PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) pbl PROFILE=$(PROFILE) ARCH=$(ARCH)

clean:
	rm -rf $(firmware_archive)
	+$(MAKE) app PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) sbl PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) pbl PROFILE=$(PROFILE) ARCH=$(ARCH) clean

# Force the rule to execute everytime.
.PHONY: binaries