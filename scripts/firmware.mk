PROFILE ?= release
ARCH = am335x
include scripts/env.mk

firmware_archive = $(build_dir)/er-301-v$(FIRMWARE_VERSION).zip

firmware_contents = $(build_dir)/app/kernel.bin
firmware_contents += $(build_dir)/sbl/SBL
firmware_contents += $(build_dir)/pbl/MLO
firmware_contents += $(build_dir)/mods/core-$(FIRMWARE_VERSION).pkg
firmware_contents += $(build_dir)/mods/teletype-$(FIRMWARE_VERSION).pkg
firmware_contents += $(build_dir)/install.lua

$(firmware_archive): $(firmware_contents)
	@echo $(describe_env) ZIP $(describe_target)
	@rm -rf $(firmware_archive)
	@$(ZIP) -j $(firmware_archive) $(firmware_contents)	

$(build_dir)/install.lua: scripts/install.lua scripts/env.mk
	@echo $(describe_env) SED $(describe_target)
	@sed 's/FIRMWARE_VERSION/$(FIRMWARE_VERSION)/g' $< > $@

app-libs:
	+$(MAKE) -f scripts/lua.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/miniz.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/lodepng.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	+$(MAKE) -f scripts/ne10.mk PROFILE=$(PROFILE) ARCH=$(ARCH)	

$(build_dir)/app/kernel.bin: app-libs
	+$(MAKE) -f scripts/app.mk PROFILE=$(PROFILE) ARCH=$(ARCH)

$(build_dir)/mods/core-$(FIRMWARE_VERSION).pkg:
	+$(MAKE) -f scripts/core.mk PROFILE=$(PROFILE) ARCH=$(ARCH)

$(build_dir)/mods/teletype-$(FIRMWARE_VERSION).pkg:
	+$(MAKE) -f scripts/teletype.mk PROFILE=$(PROFILE) ARCH=$(ARCH)
	
$(build_dir)/sbl/SBL:
	+$(MAKE) -f scripts/sbl.mk PROFILE=$(PROFILE) ARCH=$(ARCH)

$(build_dir)/pbl/MLO:
	+$(MAKE) -f scripts/pbl.mk PROFILE=$(PROFILE) ARCH=$(ARCH)

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
