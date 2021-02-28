# Unassigned

## Clicks in Shared Buffers
The basic idea will be to push a copy of the un-dubbed audio that is about to be overdubbed into a ring buffer. Then whenever a play-head approaches the record-head, fade into the un-dubbed material that was saved on the ring buffer instead. So by the time the play-head reaches the record-head’s position the mix will be 100% of the un-dubbed material in the ring buffer and 0% of the looper buffer (which has the click in it). If the play-head is going in reverse you would fade out instead.

## End of Slice Triggers
Shared buffers could be extended to include a shared playhead, or, at least awareness of a given playhead so that a unit's output could be a function of another sample player's state.


# Low Priority

ENHANCE: Add a file browser.
ENHANCE: Add a log viewer.

ENHANCE: Bypass toggle via trigger
ENHANCE: A unit that when triggered causes its parent chain to load a given preset.

ENHANCE: Replace direct calls to neon intrinsics with xsimd calls (https://github.com/JohanMabille/xsimd).
ENHANCE: Sample preview volume adjustment.

NEW UNIT: Sequencer > I've already made the object.
CONFUSING: Custom Units > The replace command appears to have no effect.

ENHANCE: Shared circular buffer.
ENHANCE: Sample Pool > Save all "unsaved" buffers.

ENHANCE: Teletype Slew > add constant time mode

ENHANCE: Settings > Automatically purge unused samples after certain actions: clear chain, delete unit, etc.

ENHANCE: Add volume control for sample previewing.
ENHANCE: Allow multiselect across folders.

ENHANCE: System Setting > Default F0 for oscillators and filters.
or
ENHANCE: Ability to save default preset for any unit.  This default preset will be automatically applied when you insert a unit from the unit selection screen.

ENHANCE: Control Editor > copy/paste controls

ENHANCE: Saving files, naming things > Option to generate a unique name for the user.

# Medium Priority

FIX: Make ER-301 audio output sync more robust against tiny brown outs.
FIX: Paste option does not always show up when it should.
FIX: Sample Editor > Multiple tmp buffers being created when fading in/out.
FIX: Sample Loading > Gives a parser error message (or no message!) instead of "out of memory" error message when trying to load a sample that is larger than the remaining free RAM.

FIX: Units > The order of the controls is not consistent.

FIX: 6-track > Allows saving to blank filename.

FIX: Sample Pool > Disable comands while a sample is saving to the card.
FIX: Saving Presets > Invalid characters can still get into the filename via the text history.

FIX: When in HOLD mode and there are no pinned controls, pop up a message when the user presses buttons that don't do anything.
FIX: Preset folders per unit type are no longer needed.

FIX: Multitrack or any save-file situation > When saving a single track, pressing enter on a folder XYZ will save the track as XYZ.wav.
FIX: Presseing ENTER while focused on chain source should enter the source selection menu.

FIX: https://forum.orthogonaldevices.com/t/grainstretch-not-jittering-at-end-of-sample/5121

# High Priority

FIX: Rear SD card does not remount after being reinserted.
FIX: USB boot interferes with SD card boot when cable is plugged in.

## SDK
* A user module may implement classes derived from Task, Object and/or Graphic.
* Divide source into firmware, corelib, and sdk
* Corelib and user modules link against sdk.
* Firmware dynamically loads corelib and user modules.
* User modules do not see symbols in corelib nor each other.
* However, user modules may use corelib and firmware/sdk functions and classes in lua.
* Changes in firmware do not affect corelib and user modules.
* Changes in sdk affect corelib and user modules.
* package the teletype functionality separately
* package devs will need a way to add their own settings (see teletype)

### packages vs mods
* Packages are archives of a file tree that can be installed/uninstalled by the Package Manager.
* Mods are libraries of pure lua and/or native machine code that extend or modify the existing behavior of the base firmware.

Start-up sequence:

MLO
SBL
app.bin/firmware.bin/kernel.bin
c_init
main
main_task
Application_init
Application_loop
start.lua
Application.lua