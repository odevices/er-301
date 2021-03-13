PROFILE ?= release
ARCH = am335x
include scripts/env.mk

firmware_archive = $(build_dir)/er-301-v$(FIRMWARE_VERSION)-$(FIRMWARE_STATUS).zip

firmware_contents = $(build_dir)/app/kernel.bin
firmware_contents += $(build_dir)/sbl/SBL
firmware_contents += $(build_dir)/pbl/MLO
firmware_contents += $(build_dir)/mods/core-$(FIRMWARE_VERSION).pkg
firmware_contents += $(build_dir)/mods/teletype-$(FIRMWARE_VERSION).pkg

$(firmware_archive): $(firmware_contents)
	+$(MAKE) -f scripts/lua.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/miniz.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/lodepng.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/ne10.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/core.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/teletype.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/app.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/sbl.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/pbl.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	@echo $(describe_env) ZIP $(describe_target)
	@rm -rf $(firmware_archive)
	@$(ZIP) -j $(firmware_archive) $(firmware_contents)	

clean:
	rm -rf $(firmware_archive)
	+$(MAKE) -f scripts/lua.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) -f scripts/miniz.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) -f scripts/lodepng.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) -f scripts/ne10.mk PROFILE=$(PROFILE) ARCH=$(ARCH)	clean
	+$(MAKE) -f scripts/core.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) -f scripts/teletype.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) -f scripts/app.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) -f scripts/sbl.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
	+$(MAKE) -f scripts/pbl.mk PROFILE=$(PROFILE) ARCH=$(ARCH) clean
