# top-level makefile

all: app sbl pbl emu

app: 
	+$(MAKE) -C scripts -f lua.mk
	+$(MAKE) -C scripts -f miniz.mk
	+$(MAKE) -C scripts -f ne10.mk
	+$(MAKE) -C scripts -f lodepng.mk
	+$(MAKE) -C scripts -f core.mk
	+$(MAKE) -C scripts -f app.mk

app-clean: 
	+$(MAKE) -C scripts -f lua.mk clean
	+$(MAKE) -C scripts -f miniz.mk clean
	+$(MAKE) -C scripts -f ne10.mk clean
	+$(MAKE) -C scripts -f lodepng.mk clean
	+$(MAKE) -C scripts -f core.mk clean
	+$(MAKE) -C scripts -f app.mk clean

sbl: 
	+$(MAKE) -C scripts -f sbl.mk

sbl-clean: 
	+$(MAKE) -C scripts -f sbl.mk clean


pbl: 
	+$(MAKE) -C scripts -f pbl.mk

pbl-clean: 
	+$(MAKE) -C scripts -f pbl.mk clean

emu: 
	+$(MAKE) -C scripts -f lua.mk ARCH=linux
	+$(MAKE) -C scripts -f miniz.mk ARCH=linux
	+$(MAKE) -C scripts -f lodepng.mk ARCH=linux
	+$(MAKE) -C scripts -f core.mk ARCH=linux
	+$(MAKE) -C scripts -f emu.mk

emu-clean: 
	+$(MAKE) -C scripts -f lua.mk ARCH=linux clean
	+$(MAKE) -C scripts -f miniz.mk ARCH=linux clean
	+$(MAKE) -C scripts -f lodepng.mk ARCH=linux clean
	+$(MAKE) -C scripts -f core.mk ARCH=linux clean
	+$(MAKE) -C scripts -f emu.mk clean

dist-clean:
	rm -rf testing debug release

.PHONY: app app-clean sbl sbl-clean pbl pbl-clean emu emu-clean