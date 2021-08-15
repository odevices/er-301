# How to use this makefile
# 
# To compile and create the pkg using the default PROFILE and ARCH.
# > make 
#
# Remove build outputs for the default PROFILE and ARCH.
# > make clean
#
# Copy the pkg file to the packages folder
# > make install
#
# To cross-compile for the ER-301 hardware, add ARCH=am335x the above examples.
#
# To create a release version for the am335x:
# make PROFILE=release ARCH=am335x
#
# Remove build outputs for all profiles and architectures.
# > make dist-clean
#
# WARNING: In order to keep this makefile simple, dependencies are hard-coded 
# rather than auto-generated.

# Override these defaults on the commandline:
PKGNAME ?= minimal
PKGVERSION ?= 0.0.1
LIBNAME ?= libMinimal
PROFILE ?= testing
SDKPATH ?= ../..

# Describe your files to this makefile:
headers = MinimalObject.h MinimalGraphic.h
sources = MinimalObject.cpp MinimalGraphic.cpp minimal.cpp.swig
assets = toc.lua MinimalUnit.lua MinimalViewControl.lua
includes = .

# Do you need any additional preprocess symbols?
symbols = 

#######################################################
# Edits are generally not needed beyond this point.

include $(SDKPATH)/scripts/tutorial.mk