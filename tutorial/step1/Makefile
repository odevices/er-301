# How to use this makefile
# 
# To create the package:
# > make 
#
# Remove build outputs:
# > make clean
#
# Copy the pkg file to the packages folder
# > make install
#
#
# Remove build outputs for all profiles and architectures.
# > make dist-clean
#
# WARNING: In order to keep this makefile simple, dependencies are hard-coded 
# rather than auto-generated.

# Override these defaults on the commandline:
PKGNAME ?= tutorial
PKGVERSION ?= 0.0.1

# Describe your files to this makefile:
assets = toc.lua FMOperator.lua

#######################################################
# Edits are generally not needed beyond this point.

package_file = $(PKGNAME)-$(PKGVERSION).pkg
emu_install_dir = $(HOME)/.od/rear
# This assumes an SD card is mounted to /media/$(USERNAME)/FRONT
card_install_dir = /media/$(USERNAME)/FRONT/ER-301/packages

#######################################################
# Rules

all: $(package_file)

$(package_file): $(assets)
	@echo [ZIP $@]
	@rm -f $@
	@zip -j $@ $(assets)

list: $(package_file)
	@unzip -l $(package_file)

clean:
	rm -rf $(package_file)

emu-install: $(package_file)

card-install: $(package_file)

$(emu_install_dir)/$(package_file): $(package_file)
	cp -f $< $@ && sync

$(card_install_dir)/$(package_file): $(package_file)
	cp -f $< $@ && sync



