# top-level makefile

all: firmware

firmware:
	+$(MAKE) -f scripts/firmware.mk

firmware-clean:
	+$(MAKE) -f scripts/firmware.mk clean

app: core teletype
	+$(MAKE) -f scripts/lua.mk
	+$(MAKE) -f scripts/miniz.mk
	+$(MAKE) -f scripts/ne10.mk
	+$(MAKE) -f scripts/lodepng.mk
	+$(MAKE) -f scripts/app.mk

app-flash: app
	+$(MAKE) -f scripts/app.mk flash

app-clean: core-clean teletype-clean
	+$(MAKE) -f scripts/lua.mk clean
	+$(MAKE) -f scripts/miniz.mk clean
	+$(MAKE) -f scripts/ne10.mk clean
	+$(MAKE) -f scripts/lodepng.mk clean
	+$(MAKE) -f scripts/app.mk clean

core:
	+$(MAKE) -f scripts/core.mk

teletype:
	+$(MAKE) -f scripts/teletype.mk

core-clean:
	+$(MAKE) -f scripts/core.mk clean

teletype-clean:
	+$(MAKE) -f scripts/teletype.mk clean

sbl: 
	+$(MAKE) -f scripts/sbl.mk

sbl-flash: sbl
	+$(MAKE) -f scripts/sbl.mk flash	

sbl-clean: 
	+$(MAKE) -f scripts/sbl.mk clean

pbl: 
	+$(MAKE) -f scripts/pbl.mk

pbl-flash: 
	+$(MAKE) -f scripts/pbl.mk flash

pbl-clean: 
	+$(MAKE) -f scripts/pbl.mk clean

emu: 
	+$(MAKE) -f scripts/lua.mk ARCH=linux
	+$(MAKE) -f scripts/miniz.mk ARCH=linux
	+$(MAKE) -f scripts/lodepng.mk ARCH=linux
	+$(MAKE) -f scripts/emu.mk

emu-clean: 
	+$(MAKE) -f scripts/lua.mk ARCH=linux clean
	+$(MAKE) -f scripts/miniz.mk ARCH=linux clean
	+$(MAKE) -f scripts/lodepng.mk ARCH=linux clean
	+$(MAKE) -f scripts/emu.mk clean

dist-clean:
	rm -rf testing debug release

.PHONY: app sbl pbl emu