# Determine PROFILE if it's not provided...
# testing | release | debug
PROFILE ?= testing

# Determine ARCH if it's not provided...
# linux | darwin | am335x
ifndef ARCH
  SYSTEM_NAME := $(shell uname -s)
  ifeq ($(SYSTEM_NAME),Linux)
    ARCH = linux
  else ifeq ($(SYSTEM_NAME),Darwin)
    ARCH = darwin
  else
    $(error Unsupported system $(SYSTEM_NAME))
  endif
endif

FIRMWARE_NAME = Thoon
FIRMWARE_VERSION = 0.6.13

FIRMWARE_STATUS.release = stable
FIRMWARE_STATUS.debug = debug
FIRMWARE_STATUS.testing = testing

FIRMWARE_STATUS = $(FIRMWARE_STATUS.$(PROFILE))

scriptname := $(word 1, $(MAKEFILE_LIST))
scriptname := $(scriptname:scripts/%=%)

# colorized labels
yellowON = "\033[33m"
yellowOFF = "\033[0m"
blueON = "\033[34m"
blueOFF = "\033[0m"
describe_input = $(yellowON)$<$(yellowOFF)
describe_target = $(yellowON)$@$(yellowOFF)
describe_env = $(blueON)[$(scriptname) $(ARCH) $(PROFILE)]$(blueOFF)

# Frequently used paths
build_dir = $(PROFILE)/$(ARCH)
libs_build_dir = $(build_dir)/libs
arch_dir = arch
mods_dir = mods
od_dir = od
libs_dir = libs
hal_dir = hal

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
includes = . $(arch_dir)/$(ARCH)

# Default card locations
front_card_label = front
front_card_dir = ~/.od/$(front_card_label)
rear_card_label = rear
rear_card_dir = ~/.od/$(rear_card_label)

###########################

#### am335x-specific
ifeq ($(ARCH),am335x)

CFLAGS.release ?= $(CFLAGS.speed) -Wno-unused
CFLAGS.testing ?= $(CFLAGS.speed) -DBUILDOPT_TESTING
CFLAGS.debug ?= -g -DBUILDOPT_TESTING

ti_dir = $(arch_dir)/$(ARCH)/ti
sysbios_build_dir = $(build_dir)/sysbios
sysbios_dir = $(arch_dir)/$(ARCH)/sysbios
ne10_dir = $(libs_dir)/ne10
pkg_install_dir = /media/$(USERNAME)/FRONT/ER-301/packages

include scripts/am335x.mk

symbols += gcc am335x am3359 evmAM335x
CFLAGS.am335x = -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -Dfar= -D__DYNAMIC_REENT__ 

endif

#### linux-specific
ifeq ($(ARCH),linux)

CFLAGS.release ?= $(CFLAGS.speed) -Wno-unused
CFLAGS.testing ?= -g -DBUILDOPT_TESTING
CFLAGS.debug ?= -g -DBUILDOPT_TESTING

pkg_install_dir = $(HOME)/.od/rear

include scripts/linux.mk

# symbols += BUILDOPT_LUA_USE_REALLOC
includes += emu
CFLAGS.linux = -Wno-deprecated-declarations -msse4 -fPIC

endif

### darwin-specific
ifeq ($(ARCH),darwin)

CFLAGS.release ?= $(CFLAGS.speed) -Wno-unused
CFLAGS.testing ?= -g -DBUILDOPT_TESTING
CFLAGS.debug ?= -g -DBUILDOPT_TESTING

pkg_install_dir = $(HOME)/.od/rear

include scripts/darwin.mk

includes += emu
CFLAGS.darwin = -Wno-deprecated-declarations -msse4 -fPIC
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
