# Tutorials

## Table of Contents

  * [Step 1: Make your first unit](#step-1-make-your-first-unit)
  * [Step 2: Add custom DSP code](#step-2-add-custom-dsp-code)
  * [Step 3: Add custom graphics](#step-3-add-custom-graphics)

This folder contains example projects broken up into 3 steps that progressively increase in complexity and sophistication.  In each step, you create a package (called tutorial) containing one or more units.

## Step 1: Make your first unit

There are no files to compile, so the makefile just zips up the *.lua files and names it appropriately with a version string:

```bash
make
```

This will generate a file called **tutorial-0.0.1.pkg** in this directory.  To try it out, manually copy this pkg to the packages folder of the emulator or your SD card.  Since this package has only lua scripts, there is no need for cross-compilation.  Alternatively, you can use the make install targets:

```bash
# To install to the emulator
make emu-install

# To install to the front SD card (assuming it is mounted)
make card-install
```

Take a look at the [Makefile](step1/Makefile) for further options.

## Step 2: Add custom DSP code

Now we add your own custom C++ [Object](../od/objects/Object.h) (implemented by [EuclideanSequencer.h](step2/EuclideanSequencer.h) and [EuclideanSequencer.cpp](step2/EuclideanSequencer.cpp)) and define a new unit (implemented in [EuclidsCatsUnit.lua](step3/EuclidsCatsUnit.lua)) that uses your custom Object in its DSP graph.

As before to compile:
```bash
# Compile for default architecture (linux) and default build profile (testing):
make

# Compile for am335x architecture for release:
make ARCH=am335x PROFILE=release
```

This will generate a file called **tutorial-0.0.2.pkg** in the build directory.  To try it out, manually copy this pkg to the packages folder of the emulator or your SD card.  Since this package has only lua scripts, there is no need for cross-compilation.  Alternatively, you can use the make install targets:

```bash
# To install to the emulator
make install

# To install to the front SD card (assuming it is mounted)
make install PROFILE=release ARCH=am335x
```

Take a look at the [Makefile](step2/Makefile) for further options.

## Step 3: Add custom graphics

Finally, we add our own custom graphics to the [EuclidsCatsUnit.lua](step3/EuclidsCatsUnit.lua) created in Step 2.  The C++ [Graphic](../od/graphics/Graphic.h) is implemented in [CatCircle.h](step3/CatCircle.h) and [CatCircle.cpp](step3/CatCircle.cpp).  To actually use this graphic in a unit, we must also wrap the C++ CatCircle in a lua ViewControl as done in [CatCircle.lua](step3/CatCircle.lua).

This project will generate a file called **tutorial-0.0.3.pkg** in the build directory.  All the build and install commands are the same as in Step 2.

Take a look at the [Makefile](step3/Makefile) for further options.