
ARCH ?= am335x
#ARCH = linux
PROFILE ?= testing
#PROFILE = release
#PROFILE = debug

scriptname = $(word 1, $(MAKEFILE_LIST))

# colorized labels
describe_input = "\033[33m$(subst $(top_dir)/,"",$<)\033[0m"
describe_target = "\033[33m$(subst $(top_dir)/,"",$@)\033[0m"
describe_env = "\033[34m[$(scriptname) $(ARCH) $(PROFILE)]\033[0m"

# Frequently used paths
top_dir = ..

build_dir = $(top_dir)/$(PROFILE)/$(ARCH)
libs_build_dir = $(build_dir)/libs
app_build_dir = $(build_dir)/app
emu_build_dir = $(build_dir)/emu
mods_build_dir = $(build_dir)/mods

arch_dir = $(top_dir)/arch/$(ARCH)
mods_dir = $(top_dir)/mods
od_dir = $(top_dir)/od
libs_dir = $(top_dir)/libs
hal_dir = $(top_dir)/hal

arch_hal_dir = $(arch_dir)/hal

miniz_dir = $(libs_dir)/miniz
lodepng_dir = $(libs_dir)/lodepng
lua_name = lua54
lua_dir = $(libs_dir)/$(lua_name)

CFLAGS.common = -Wall -ffunction-sections -fdata-sections
CFLAGS.only = -std=gnu11 -Werror=implicit-function-declaration
CPPFLAGS.only = -std=gnu++11

CFLAGS.speed = -O3 -ftree-vectorize -ffast-math
CFLAGS.size = -Os

symbols = 
includes =

###########################

#### am335x-specific
ifeq ($(ARCH),am335x)

CFLAGS.release ?= $(CFLAGS.speed) -Wno-unused
CFLAGS.testing ?= $(CFLAGS.speed) -DBUILDOPT_TESTING
CFLAGS.debug ?= -g -DBUILDOPT_TESTING

ti_dir = $(arch_dir)/ti
sysbios_build_dir = $(build_dir)/sysbios
sysbios_dir = $(arch_dir)/sysbios
ne10_dir = $(libs_dir)/ne10
front_card_label = FRONT
front_card_dir = /media/$(shell whoami)/$(front_card_label)
rear_card_label = REAR
rear_card_dir = /media/$(shell whoami)/$(rear_card_label)

include am335x.4.01.00.06.mk
#include am335x.4.02.00.09.mk
#include am335x.5.03.00.07.mk
#include am335x.6.01.00.08.mk

symbols += gcc am335x am3359 evmAM335x
CFLAGS.am335x = -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -Dfar= -D__DYNAMIC_REENT__ 

endif

#### linux-specific
ifeq ($(ARCH),linux)

CFLAGS.release ?= $(CFLAGS.speed) -Wno-unused
CFLAGS.testing ?= -g -DBUILDOPT_TESTING
CFLAGS.debug ?= -g -DBUILDOPT_TESTING

front_card_label = front
front_card_dir = ~/.od/$(front_card_label)
rear_card_label = rear
rear_card_dir = ~/.od/$(rear_card_label)

include linux.mk

# symbols += BUILDOPT_LUA_USE_REALLOC
CFLAGS.linux = -Wno-deprecated-declarations -msse4 -fPIC
endif

###########################

ifndef CFLAGS.$(PROFILE)
$(error Error: '$(PROFILE)' is not a valid build profile.)
endif

CFLAGS += $(CFLAGS.common) $(CFLAGS.$(ARCH)) $(CFLAGS.$(PROFILE))
CFLAGS += $(addprefix -I,$(includes)) 
CFLAGS += $(addprefix -D,$(symbols))

# swig-specific flags
SWIGFLAGS = -lua -no-old-metatable-bindings
SWIGFLAGS += $(addprefix -I,$(includes)) 
# Replace speed optimization flags with size optimization flags.
CFLAGS.swig = $(subst $(CFLAGS.speed),$(CFLAGS.size),$(CFLAGS))
#CFLAGS.swig = $(CFLAGS)
