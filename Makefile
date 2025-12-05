# top-level makefile

# Define all build targets
BUILD_TARGETS = firmware app-libs app core teletype ladspa emu

# Define all clean targets
CLEAN_TARGETS = firmware-clean app-libs-clean app-clean core-clean teletype-clean ladspa emu-clean

# Add new all and clean targets at the top
.PHONY: all clean $(BUILD_TARGETS) $(CLEAN_TARGETS)

# Build everything
all: $(BUILD_TARGETS)

# Clean everything
clean: $(CLEAN_TARGETS)

firmware:
	+$(MAKE) -f scripts/firmware.mk

firmware-clean:
	+$(MAKE) -f scripts/firmware.mk clean

app-libs:
	+$(MAKE) -f scripts/lua.mk
	+$(MAKE) -f scripts/miniz.mk
	+$(MAKE) -f scripts/ne10.mk
	+$(MAKE) -f scripts/lodepng.mk

app-libs-clean:
	+$(MAKE) -f scripts/lua.mk clean
	+$(MAKE) -f scripts/miniz.mk clean
	+$(MAKE) -f scripts/ne10.mk clean
	+$(MAKE) -f scripts/lodepng.mk clean

app: app-libs 
	+$(MAKE) -f scripts/app.mk

app-flash: app-libs 
	+$(MAKE) -f scripts/app.mk flash

app-clean: app-libs-clean 
	+$(MAKE) -f scripts/app.mk clean

core:
	+$(MAKE) -f scripts/core.mk

core-install:
	+$(MAKE) -f scripts/core.mk install

teletype:
	+$(MAKE) -f scripts/teletype.mk

teletype-install:
	+$(MAKE) -f scripts/teletype.mk install

ladspa:
	+$(MAKE) -f scripts/ladspa.mk

ladspa-install:
	+$(MAKE) -f scripts/ladspa.mk install

core-clean:
	+$(MAKE) -f scripts/core.mk clean

teletype-clean:
	+$(MAKE) -f scripts/teletype.mk clean

ladspa-clean:
	+$(MAKE) -f scripts/ladspa.mk clean

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

# Add these lines before the emu target
ifeq ($(shell uname -s),Darwin)
ifeq ($(shell uname -m),arm64)
export ARCH_FLAGS=-march=armv8.2-a
else
export ARCH_FLAGS=-march=native
endif
endif

emu:
	+$(MAKE) -f scripts/lua.mk
	+$(MAKE) -f scripts/miniz.mk
	+$(MAKE) -f scripts/lodepng.mk
	+$(MAKE) -f scripts/emu.mk

emu-clean:
	+$(MAKE) -f scripts/lua.mk clean
	+$(MAKE) -f scripts/miniz.mk clean
	+$(MAKE) -f scripts/lodepng.mk clean
	+$(MAKE) -f scripts/emu.mk clean

dist-clean:
	rm -rf testing debug release
	+$(MAKE) -C tutorial/step1 clean
	+$(MAKE) -C tutorial/step2 dist-clean
	+$(MAKE) -C tutorial/step3 dist-clean

.PHONY: app sbl pbl emu
