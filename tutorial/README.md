# Tutorial

![](tutorial-screenshot.png)

## Table of Contents

  * [Introduction](#introduction)
  * [Step 1: Make your first unit](#step-1-make-your-first-unit)
  * [Step 2: Add custom DSP code](#step-2-add-custom-dsp-code)
  * [Step 3: Add custom graphics](#step-3-add-custom-graphics)
  * [Further Reading](#further-reading)
  * [Failed to load ELF File](#failed-to-load-elf-file)
  * [Tips for coding with NEON intrinsics](#tips-for-coding-with-neon-intrinsics)
  * [Glossary](#glossary)

## Introduction

This folder contains 3 example projects that progressively increase in complexity and sophistication.  In each step, you create a package (called tutorial) containing one or more units.  First you will create a unit (called FM Operator) using only lua scripting.  Next, you will create yet another unit (call Euclid's Cats) that has your C++ DSP code.  Finally, you will enhance the Euclid's Cats unit with your own custom graphics.

:warning: You have a choice to either use these tutorial projects in-place OR you can copy them out and work on them outside of this source tree.  If you choose to copy them then make sure to edit the **SDKPATH** variable in each step's Makefile to point to the er-301 source tree.

## Step 1: Make your first unit

Our first [unit](#unit) is 1-operator FM oscillator that is built entirely out of existing DSP objects that can be found in the firmware and the core library.  Since the implementation is pure Lua, there are no files to compile.  The *Step 1* makefile just zips up the *.lua files and names it appropriately with a version string.  

```bash
cd step1
make
```

This will generate a file called **tutorial-0.0.1.pkg** in this directory.  To try it out, manually copy this pkg to the packages folder of the emulator or your SD card.  Since this package has only lua scripts, there is no need for cross-compilation.  Alternatively, you can use the make install targets:

```bash
# To install to the emulator's package folder:
make emu-install

# To install to the front SD card (assuming it is mounted):
make card-install
```

Take a look at the [Makefile](step1/Makefile) for more details.

## Step 2: Add custom DSP code

Now we add your own custom C++ [Object](#object) (implemented by [EuclideanSequencer.h](step2/EuclideanSequencer.h) and [EuclideanSequencer.cpp](step2/EuclideanSequencer.cpp)) and define a new unit (implemented in [EuclidsCatsUnit.lua](step3/EuclidsCatsUnit.lua)) that uses your custom Object in its DSP graph.

To compile:
```bash
cd ../step2

# Compile a testing build for the emulator (linux):
make

# Compile a release build for am335x architecture:
make ARCH=am335x PROFILE=release
```

This will generate a file called **tutorial-0.0.2.pkg** in the build directory.  The build directory is constructed from build profile and target architecture.  For example, the default build profile is **testing** and the default target architecture is **linux**, so the default build directory is **testing/linux**.  To try out the new units, manually copy the tutorial package to the packages folder of the emulator or your SD card.  Alternatively, you can use the make install targets:

```bash
# To install to the emulator's package folder:
make install

# To install to the front SD card (assuming it is mounted):
make install PROFILE=release ARCH=am335x
```

Take a look at the [Makefile](step2/Makefile) for more details.

## Step 3: Add custom graphics

Finally, we add our own custom graphics to the [EuclidsCatsUnit.lua](step3/EuclidsCatsUnit.lua) created in Step 2.  The C++ [Graphic](#graphic) is implemented in [CatCircle.h](step3/CatCircle.h) and [CatCircle.cpp](step3/CatCircle.cpp).  To actually use this graphic in a unit, we must also wrap the C++ CatCircle in a lua [ViewControl](#viewcontrol) as done in [CatCircle.lua](step3/CatCircle.lua).

This project will generate a file called **tutorial-0.0.3.pkg** in the build directory.  All the build and install commands are the same as in Step 2.  Have a look at the [Makefile](step3/Makefile) for more details.

## Further Reading

There is still plenty to learn outside of the above 3 examples.  Here are some topics along with pointers to examples in the source tree to get you started:

| Topic | Code Example |
| --- | ----------- |
| Playing from a sample buffer. | core: [Raw Player Unit](../mods/core/assets/Player/Raw.lua) |
| Streaming from a sample file. | core: [Card Player Unit](../mods/core/assets/File/CardPlayerUnit.lua) |
| Using the Task class to scheduling code to run on the audio thread. | teletype: [Dispatcher.h](../mods/teletype/Dispatcher.h), [Dispatcher.cpp](../mods/teletype/Dispatcher.cpp) |
| Optimizing DSP code with NEON intrinsics. | builtin: [SawtoothOscillator.h](../mods/core/objects/oscillators/SawtoothOscillator.h), [SawtoothOscillator.cpp](../mods/core/objects/oscillators/SawtoothOscillator.cpp) |
| Adding a configuration menu to your package. | teletype: [init.lua](../mods/teletype/assets/init.lua) |

## Failed to load ELF File
You have a package that works in the emulator but fails to load on the actual device, so you inspect the error log and see that your package's shared library (*.so) has failed to load with the message **Failed to load ELF File**.  Most of the time the reason will be that one or more symbols referenced by your library could not be resolved by the dynamic loader.  To fix this problem, you need to determine what symbols are missing and try to figure out how they got compiled into your shared library.  Most likely, you have linked against a standard library that is available on Linux but not on the ER-301, something that can be triggered by an errant ```#include <iostream>``` statement in your code.

So how should you proceed to debug this error?  Here are two options.

### Inspect the system logs
The ELF loader (and all other messages that originate in the C/C++ code) prints its error messages to the UART/USB serial console.  There you will see more detailed messages describing the reason for the load failure.  In the case of missing (unresolved) symbols, those will all be listed, one per line, like this:

```
WARN Unresolved symbol: __some_symbol_name__
.
.
.
WARN Unresolved symbol: __another_symbol_name__
ERROR Aborting ELF load due to N unresolved symbol(s).
```

There are of course other reasons that the loader could fail.  The system log will help you debug those too.

### Check for missing symbols during compilation
It is possible to check for any missing symbols when compiling your package.  However, you need to first compile the base firmware so that you have a reference listing of the symbols that the firmware exports.  

To compile the base firmware for the ER-301 hardware:

```bash
cd <top-of-er-301-source-tree>
make app PROFILE=release ARCH=am335x
```

This will generate symbol listings at ```er-301/release/am335x/app/exports.sym```.  Assuming this has completed successfully then thereafter whenever you want to check for missing symbols just run:

```bash
cd <top-of-your-package-source-tree>
make missing
```

This of course assumes that your Makefile is based on the one provided with the tutorial projects.  If not, it is not hard to role your own ```missing``` target to your custom Makefile.  The procedure involves running ```nm --undefined-only``` on your shared library to generate a listing of imported symbols, and then comparing that to the exported symbols provided by the firmware (again located at ```er-301/release/am335x/app/exports.sym``` after a successful compilation of the firmware).

## Tips for coding with NEON intrinsics

* You are basically telling the compiler that you want to take over (i.e. hold my beer/milk/tea while I show you how it is done) and thus many optimizations will not be attempted on code sections that use NEON intrinsics and/or data types.
* Do not expect consistent behavior across processor types and compiler versions like you can for higher-level language constructs.
* Ideally, you would first implement in idiomatic C. If after inspecting the disassembly, you see that the compiler has not vectorized at all (or not very well) then try to refactor so that the compiler can recognize the vectorization opportunity.
* If that fails, then replace the smallest (most contained) section possible with your own vectorization via NEON intrinsics. If you can turn your optimization on and off with a simple set of #ifdef/#endif statements then you know you have done it right.
* In other words, the ideal coding style for NEON intrinsics is to treat it like inline assembly: make the smallest surgical strike as possible.
* NEON (and in fact all SIMD) data types are not proper citizens of high-level language design. They have special requirements (memory alignment, special registers, timing constraints) and sometimes do not (and can not) fit neatly within language standards (especially C++). The correct level of abstraction is to think of them as simple macros for assembly.

## Glossary

### Unit

A Unit is any Lua class that inherits from [Unit/init.lua](../xroot/Unit/init.lua).  It encapsulates a custom DSP graph of [Objects](#objects) and the UI for interacting with it.  The UI implementation will optionally include menu commands as well as a list of [ViewControls](#viewcontrol) where each control exposes a part of the Unit's DSP graph for manipulation via fader or modulation.  The Unit implementation also defines any special preset saving/loading logic that the default system logic will not handle correctly.

### Object

An Object is any C++ class that derives from the abstract class defined in [Object.h](../od/objects/Object.h).  The core purpose of any Object is to encapsulate a DSP algorithm (written in C++), exposing inputs and outputs, so that it can be instantiated and utilized in a signal processing graph.  In order to accomplish this, an Object will define some of the following:

* *Outlets*: Named audio-rate (float) outputs, typically audio or CV.
* *Inlets*: Named audio-rate (float) inputs, typically audio or CV.
* *Parameters*: Named (float) values that are updated at frame-rate, good for mapping to faders.
* *Options*: Named (int) values are updated by the GUI, good for mapping to buttons.
* *process()*: A callback that is called at frame-rate by the audio thread.  You should implement the Object's DSP algorithm here.  This typically, involves reading data from any Inlets, calculating the desired output based on values from any Parameters or Options, and then writing that output to Outlets.

Yes, I know this is not the greatest name.  *Object* was meant to just be a place-holder until I could find a better name.  However, that has not happened yet.  Some candidates are: Node, Processor, Algorithm, GraphNode, DSPGraphNode, and so on.

### Graphic

A Graphic is any C++ class that derives from the abstract class defined in [Graphic.h](../od/graphics/Graphic.h).  A Graphic is responsible for drawing to the screen (via its *draw()* callback) as well as managing any of its children Graphics.  A Graphic's local coordinate system is defined by its parent Graphic.  A Graphic does not respond to user input.  User events are instead handled by the Lua-implemented event engine.

### ViewControl

A ViewControl represents a UI element on a Unit.  Here are a few common types of controls provided by the system:

* [Fader](../xroot/Unit/ViewControl/Fader.lua): A simple fader with no modulation.  This control simply wraps a Parameter.
* [GainBias](../xroot/Unit/ViewControl/GainBias.lua): An attenuverting (i.e. gain) control with offset (i.e. bias) fader that accepts modulation.  You will almost always wrap a GainBias-like Object and a Branch with this control
* [Pitch](../xroot/Unit/ViewControl/Pitch.lua): A control designed for accepting V/oct modulation with an offset in cents.  Wraps an Offset-like Object and a Branch.
* [Gate](../xroot/Unit/ViewControl/Gate.lua): A control for wrapping a Comparator-like Object and a Branch.
* [Clock](../xroot/Unit/ViewControl/Clock.lua): A control for wrapping a TapTempo-like Object and a Branch.

More control types can be made by deriving from [Unit/ViewControl/init.lua](../xroot/Unit/ViewControl/init.lua).  ViewControls are derived from [Observable](../xroot/Base/Observable.lua) and [Widget](../xroot/Base/Widget.lua), which means they can consume signals (e.g. card ejected, USB cable unplugged, unit renamed, etc.) and events (button presses, encoder movement, etc.).  The look of a ViewControl is controlled by one or more instances of [Graphics](#graphic).