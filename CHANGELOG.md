# Legend

* FIX: A bug fix.  Relevant to users and developers.
* NEW: A new feature. Relevant to users and developers.
* SYS: A system-level change.  Usually only relevant to developers.

# v0.6.10
FIX: Local connections to the output of an empty branch were not being restored.

# v0.6.09
* FIX: File Browser > Make listing order of files/folders more consistent. Folders then files, all in case-insensitive dictionary order.
* FIX: File Browser > Crash when changing focus to folder during audio preview (issue #23).

# v0.6.08
* FIX: The Fixed HPF Unit had some unitialized variables that might be causing loss of audio in random cases.
* FIX: Pressing CANCEL in the **Select from Pool** menu was causing a crash.

# v0.6.07
* FIX: The Fixed HPF Unit implementation was broken by some changes to the NEON calculations that were introduced in v0.6.00.

# v0.6.06
* SYS: Constructors for static objects were not being called during runtime linking.

# v0.6.05
* FIX: Installer fails on pre-v0.v6 firmware.  This is fixed now but in order to get the core and teletype packages installed you will need to run the installer one more time after booting into v0.6 firmware.

# v0.6.04
* FIX: Corrupted WAV files > Any WAV files created by the ER-301 had a corrupted header.
* FIX: Package Creator > Silently ignored missing WAV files.
* SYS: Mod error logs are now created in front:/ER-301/logs/(mod-name).log
ENHANCED: Admin > Log History screen added.

# v0.6.03
* FIX: A chain would sometimes stop producing audio when attaching a sample from the card to a unit. (issue #12)
* SYS: Major improvements to firmware's symbol table used for dynamic linking. (issue #13)

# v0.6.02
* FIX: Unit Browser > Categories were not being display in a consistent order.
* SYS: Package toc.lua > Deprecate category entries in the units table.  Add a category field to each unit entry instead.
* SYS: Emulator > Simplify command-line options and make emulator configurable via the ~/.od/emu.config file.  See README.md.

# v0.6.01
* FIX: All Delay Units > Configuring the max delay time to anything less than 1s resulted in a 0s delay buffer and loss of audio in that chain.

# v0.6.00
* FIX: Soft Keyboard > History was not being saved in some cases.
* FIX: Doppler Delay unit > Maximum delay length can now be configured just like the vanilla Delay unit.
* FIX: Pressing an unrelated button while a long-press menu is showing would result in a crash.
* FIX: The suggested save folder for unit presets has been simplified to ER-301/(version)/presets instead of the now unnecessary ER-301/(version)/presets/(unit-type-name).
* FIX: Teletype CV Unit > Slew limiting is enabled by default with a 2ms rate.
* FIX: Quicksaving when there are unsaved buffers > If you pressed UP or HOME when in the Sample Pool saving buffers, the quicksave was being canceled.  The quicksave should only be canceled if you press CANCEL.
* FIX: G inputs > ADC driver will now reset the ADC hardware when it detects that it is no longer getting interrupts (i.e. timeout).
* NEW: Package Manager > sharing of code and presets via packages.
* NEW: Low Level (C/C++) SDK > It is now possible to dynamically link shared libraries (*.so) to the bare-metal firmware during runtime.  This functionality is accessible via lua's require and package.loadlib interfaces.  Please refer to the core and teletype mod source code for a comprehensive example.
* NEW: System Settings > USB > The ER-301 can now enumerate as a mass storage device when connected to a USB host.  This provides access to the front SD card only for now.
* NEW: System Settings > USB > The ER-301 can now enumerate as a virtual serial port when connected to a USB host.  The ER-301 will log to this port when connected.
* NEW: System Settings > Sample rate and frame size are now configurable but require a reboot for changes to take effect.
* NEW: File Browser > Selecting multiple files can now span multiple folders.
* NEW: File Browser > When selecting multiple files, pressing ENTER toggles the checked state, and COMMIT (i.e. SHIFT+ENTER) will finish the selection process.
* NEW: File Browser > After selecting multiple files, the user is presented with an intermediate staging UI that displays all of files selected so far.  Here the user can make changes in the order of the files, select/deselect, or add more files before proceeding to the next step.
* NEW: An emulation of the ER-301 is now available for Linux.
* SYS: Mod > teletype-0.6.0 > These units have been separated out into their own mod package.
* SYS: Mod > core-0.6.0 > Most of the factory units (and their objects) have been separated out into their own mod package.
* SYS: Lua > Updated the embedded lua interpreter from 5.3 to 5.4.
* SYS: Lua > Changed garbage collection method from incremental to generational.
* SYS: Unit:createObject(type, name, ...) --> Unit:addObject(name, o)
* SYS: Unit:createMonoBranch --> Unit:addMonoBranch
* SYS: Unit:createStereoBranch --> Unit:addStereoBranch
* SYS: Unit:onLoadMenu --> Unit:onShowMenu
* SYS: Object:optionSet --> Object:setOptionValue

# v0.5.04
* No changes. The purpose of this version is just to upgrade v0.5.03 to a stable release.

# v0.5.03
* NEW: Multitrack Recorder > Number of channels is now configurable (from 6 to 12 channels) via Admin > System Settings > Multitrack Recorder > Channel Count.  Please keep in mind that not all SD cards are fast enough to record this many channels of 48kHz/96kHz audio.  Please test beforehand.
* NEW: SC.TR > Added the 'any' port to the fader (scroll below 1).
* MAYBE FIX: Teletype Integration > Occasionally, an i2c message would sit in the buffer and not get processed until the next message arrives.
* FIX: Slight performance degradation introduced in v0.5.02.
* FIX: Quicksave > The UP button brought the focus to the first slot (HOME) instead of returning you to your previous screen.
* FIX: Multitrack Recorder > Freeze when the SD card cannot keep up with audio recording (i.e. overrun).

# v0.5.02
* SYS: Moved build environment from Code Composer Studio on Windows to GNU make on Linux.
* FIX: Oscilloscope > Reduced the appearance of gaps in traces of waves with discontinuities (e.g. square, saw, and so on). 
* FIX: Teletype Integration > Jitter in message timing grew over time.
* MAYBE FIXED?: Teletype Integration > Need to wait for ER-301 to finish booting before booting other I2C devices.
* FIX: Sample-and-Hold, Track-and-Hold units > Held values were not being restored properly from presets/quicksaves.
* FIX: HOLD mode > When adding an empty PinSet, the new (empty) PinSet rendered with zero width.

# v0.5.01
* FIX: Clicks in the audio when inserting or deleting units or loading presets.
* NEW: Text Entry > Holding a button will repeatedly insert the character or perform the action (i.e. backspace).
* NEW: All sample players that can play slices > Added new Play Extent option called 'cue'. Triggering the player will cause playback to start from the current active slice/cue and play until the end (or beginning if speed < 0).  Afterwards, if looping, then the entire sample will loop.

# v0.5.00
* FIX: 48kHz Low-latency (frame size = 32) firmware > Clicks when recording from IN1-4.
* NEW: 48kHz Low-latency (frame size = 32) firmware > No overruns up to approximately 94% CPU load (previously it was up to 70%).
* NEW: 48kHz Normal-latency (frame size = 128) firmware > No overruns up to approximately 94% CPU load (previously it was up to 84%).
* NEW: 96kHz Normal-latency (frame size = 128) firmware > No overruns up to approximately 92% CPU load (previously it was up to 64%).
* NEW: If there is no ER-301/v0.5 folder on your front SD card then the ER-301/v0.4 folder will be used to populate the new ER-301/v0.5 folder. Previously, you had to do this copy manually when the firmware version had a change in the minor version number (e.g. v0.3 to v0.4 and so on).

# v0.4.27
* FIX: Hold Mode > Enter/Commit and Home/Zero were not working properly when the cursor was over the master fader of a pin set.
* FIX: Custom Units and Bypass > Custom Units were not completely disabled when bypassed.
* FIX: Unit Editor > Duplicate control names were allowed when they should not be allowed.
* FIX: Slice Players > When restoring a preset/quicksave, the 'Play Extent' setting was getting overridden by a change introduced in v0.4.21.
* FIX: Track-and-Hold > "Track when low" mode was not working.
* FIX: Middle Layer > RoundRobin > Implementation was not finished.
* NEW: Saving Unit Preset > If the unit has been renamed, then suggested filename should be based on the unit name.
* NEW: Unit Browser > If a category only contains units from one external library then the library name will be added to the category rather than each unit.

# v0.4.26
* FIX: Hold Mode > Immediately after unpinning a control, it was possible to cause a crash by pressing S2.
* FIX: 6-track recorder > Freeze when trying to record with no SD card mounted.
* FIX: Screensaver > Dialed down the bubbles a bit.
* NEW: Screensaver > Now there are 4 modes: blank, 2lines, grid, bubbles.
* FIX: Sample Pool > Trying to load a sample when there was insufficient sample memory would give a parsing error instead of an out-of-memory error.
* FIX: Sample Pool > Pressing CANCEL when asked to load multiple samples as a chain (or not) will now cancel the load.
* FIX: Unit > Attach from Card > No error message when out of sample memory.
* NEW: Unit > Attach from Card > You can now load multiple samples as a sample chain when using this command.  Previously this was only available via the Sample Pool.

# v0.4.25
* FIX: Unit Chooser > Stereo-only units were listed in Recents even for mono chains.
* NEW: Mixer Channel > Renamed to Mix for mono chains.  Split into 2 different units for stereo chains, Mono Mix and Stereo Mix.

# v0.4.24
* FIX: Control Editor > A long press on the test fader in the property edit screen would crash the ER-301.
* FIX: 6-track Recorder > When saving multiple tracks, pressing ENTER in the folder browser while having nothing selected would silently fail.
* FIX: Replacing Units > There is some ambiguity about how to order the controls when replacing a unit with another unit that has different controls, especially considering that users can reorder controls at will, as well as add new controls.  Previously the ordering of the replaced control had priority but now I've changed it so that the control order of the new unit (replacer) has priority.
* NEW: Sample Pool > If the "save as" option is not available (because the sample already matches what is on your card) then just hold SHIFT to make it appear.

# v0.4.23
* FIX: Single Cycle > The scan signal was being sampled at frame rate (370Hz) rather than at the audio rate.

# v0.4.22
* FIX: Some frequency readouts were not showing decimals for values less than 1Hz.
* NEW: Single Cycle > A simple and naive wavetable oscillator with morphing.  Load it with a wav file containing a single cycle of your desired waveform, or, load it with a sample chain of single-cycle waveforms and scan.

# v0.4.21
* FIX: Firmware Updater > v0.4.20 broke the internal firmware updater.
* FIX: Screenshots > v0.4.20 broke the ability to create png files.
* FIX: Encoder > Super fine was the same step size as fine for some parameters.
* FIX: 6-track recorder > No way to clear assignment on a single track.
* FIX: Slice Players > Upon assigning a sample, the Play Extent setting will be initialized to 'all' if there are no slices, and to 'slice' if there are slices.
* FIX: Clocked Player, Clocked Stretch, and any unit with a trigger input > These units were causing a slow memory leak.
* FIX: Scope View > Resource stress by keeping deleted units in memory until the next time you open the scope view.

# v0.4.20
* FIX: Soft Keyboard > Combined text history for file-related operations and unit renaming.
* FIX: Crashes when opening the Source Chooser after loading large quicksaves.
* FIX: More resource leaks plugged. (GraphicContext)

# v0.4.19
* FIX: Memory corruption in the loudness meters and scopes that might have been causing some crashes.
* FIX: Source Chooser > Resource leak when switching between Jacks, Locals, and Globals.

# v0.4.18
* FIX: The data in 8-bit WAV files was being read as signed integers rather than unsigned integers.
* FIX: Move to Mixer > Pinned controls were unpinned after being part of a "Move to Mixer" operation.
* FIX: Hold Mode > Renaming a control in one Pin Set was not propagating to the other Pin Sets that contained the same control.
* FIX: Slice Editor > Pops up message if you attempt to create a loop point where there is no slice.
* NEW: All pin-able controls > Added "pin to all" and "unpin from all" commands to the long press menu.
* NEW: Soft Keyboard > You can now choose from a list of text snippets that were previously entered in a similar UI context (i.e. history).

# v0.4.17
* FIX: Crashes when loading some presets/quicksaves with local connections.
* FIX: 6-track Recorder > When recording a single track, it was not possible to save to an empty folder.
* FIX: Hold Mode > Unusually high CPU load when manipulating a Master Fader.
* FIX: Sample Audition > When browsing for a sample via a unit (not via the Sample Pool) the default audition channel was 1 or last used channel.  Now it will audition on the channel where the unit is located.  Auditioning from the Sample Pool is unchanged.
* FIX: Contextual View > Sample View > The "open editor" command was not working.

# v0.4.16
* FIX: Hold Mode > Sub menu zombies when using any command that removes pinned controls or PinSets.
* NEW: Hold Mode > Added 'Reset Targets' command to PinSets and 'Reset Target' command to pinned faders.  Resetting a target means to move the target value to the current value of the control.

# v0.4.15
* FIX: Unit Header > Swapped 'Replace' and 'Config' commands so that the more often used 'Config' command does not require a SHIFT press.
* FIX: Unit Controls > ZERO (SHIFT+HOME) was not working.
* FIX: Unit Menu > Reinstated access via focus press on a unit header. This means there are 3 ways to access the unit menu: (1) Focus press the header, (2) long press the header and choose 'config', and (3) focus the header and press S3.
* FIX: Hold Mode > Pressing ZERO on a Master Fader would sometimes not work.
* FIX: Hold Mode > Pinned Control > Unpin command was leaving a 'ghost' control behind.
* NEW: Hold Mode > Pressing COMMIT on a Master Fader will immediately set it to 100%.

# v0.4.14
* FIX: Hold Mode > Master faders were reset only on positive movements.
* FIX: Hold Mode > Zero (SHIFT+HOME) was not working on pinned controls.
* FIX: Hold Mode > Pinned Control > Unpin menu item was not working.
* FIX: Hold Mode > Pinned frequency controls were not using log scaling.

# v0.4.13
* FIX: Unit Header > Crash when focusing a unit header and turning the encoder.
* FIX: Unit Config Menu > Was not reachable.

# v0.4.12

* FIX: Custom Controls > Renaming a control was not propagating to other parts of the UI properly.
* FIX: Audition > Auditioning on ch2 mutes ch1 and leaves it muted.
* FIX: Quicksaves > Sometimes output chains were not unmuted after loading a quicksave.
* FIX: Unit Deletion > GUI would become unresponsive during deletion of a unit with lots of sub-chains.
* FIX: Local Source Selection > Problems when assigning mono sources to mono destinations within a stereo output chain.
* FIX: File Browser > Saving > Checking if an existing file will be overwritten was not done when entering a new filename.
* FIX: Contextual Sample Views > Zooming interface is now standardized to use the KNOBMODE button like the sample/slice editors.
* FIX: Contextual Sample Views > Channel selection now affects whether the left or right channel of stereo samples are rendered.
* FIX: Offset unit > In stereo chains, the right sub-chain was inadvertently set to output to the left channel and the left input channel was ignored.
* FIX: Loading mono presets into stereo chains (and vice versa) sometimes failed to restore some parameters.
* FIX: Sample Players > 'Play Duration != repeat' should not play on start-up or when changing samples.
* FIX: Locals in Stereo Chains > Having the right channel selected when connecting a mono control yielded "No Signal".
* NEW: Raw Player > Added 'speed' parameter which must be an integer since the Raw Player has no interpolation but allowing reverse playback, pausing, and integer decimation all under remote control.  The default range is -4 to +4 but this can be expanded or reduced via the control editor.
* NEW: New bootloader > This firmware comes with a new bootloader that shortens boot times.
* NEW: Screen Saver > Set activation time in System Settings. Default is 30 mins.
* NEW: Floating Menu > Push and hold (>0.3s) a unit header or control to make a floating menu appear.  Make your selection with the knob and release the button to execute.
* NEW: Custom Units > Renamed to Custom Effect.
* NEW: Hold Mode introduced.
* NEW: Custom Unit > The Custom Unit was split into Custom Source and Custom Effect units.

# v0.4.11

* FIX: Sample & Hold > Mono version was failing to load.
* NEW: Grain Stretch > Jitter control is now modulate-able.

# v0.4.10

* FIX: Units > Header border disappears when bypassing a unit.
* FIX: Sample Slicing/Editing View > S1 in slice view should play from the cursor and suppress external triggering and slice selection.  Additionally, if in slice navigation mode, then S1 actives the highlighted slice.
* FIX: Track|Sample & Hold > Was (still) not saving and restoring its last held value from presets.
* FIX: Multi-band Units > Copying a Custom unit into a Multi-band unit will lose of all of its local connections.
* NEW: Container Unit Name Generation > 3 modes: poet, robot, and off.  The ‘robot’ and ‘off’ modes are completely generic while the ‘poet’ mode uses an internal list of English adjectives and nouns to generate a (nonsense) name. Users can override either or both internal lists by creating and populating ‘ER-301/namegen/adjectives.txt’ and/or ‘ER-301/namegen/nouns.txt’. You can also provide a list of names to suppress by creating ‘ER-301/namegen/suppress.txt’.  This all applies to Multi-band units also.
* NEW: Track|Sample & Hold > Added a fader to visualize and manually change the held value.

# v0.4.09

* FIX: Legacy Presets > v0.3.25 presets and quicksaves should now load.
* FIX: Track|Sample & Hold > Was not saving and restoring its last held value from presets.
* FIX: Control Editor > Property menu was not focused initially.
* FIX: Global Chains > Renaming screen did not auto-populate previous name.
* FIX: 6-track > Assignments to global chains were not persisted.
* FIX: Multi-Band Units > Bypassing the first unit blocked the input signal.
* FIX: Quicksave Sub-display > Total # of units in the quicksave was not being counted correctly.
* FIX: Channel Mute > Not persisted in quicksave.
* NEW: Sample Scanner > Added option to disable interpolation.
* NEW: Insert Unit > Unit and Chain presets can now be loaded directly from the Insert Unit screen.  Unit presets will automatically instantiate the correct unit and chain presets will be loaded inside a Mixer unit.
* NEW: Insert Unit > Browse units alphabetically and by category.  Set default behavior in admin > Settings.
* NEW: Insert Unit > Bespoke units loaded from external libraries (i.e. not built-in) are now labeled with their library name.
* NEW: Bespoke Units > If a bespoke is not found in the original unit library (because the library was renamed or not loaded) then the ER-301 will look for a unit with the same name in the other (loaded) unit libraries.

# v0.4.08

* FIX: Unit > Custom Controls > Occasional crash when performing an operation that affects the position of a control.
* FIX: System > Memory resources were not being released in a timely manner for certain lengthy operations (note to self: Application.postTrigger).

# v0.4.07

* FIX: 6-track Recorder > Crash when trying to assign a local source.
* FIX: Convolution Unit > Removed from the low-latency firmware.
* FIX: Move to Mixer > Outgoing connections were being cleared.
* FIX: Unlinking Channels > Original content is kept in the top (mono) channel only instead of copying to both channels.  This prevents CPU usage from increasing when unlinking channels.
* FIX: Unit > Replace > Missing controls after replacing a unit.
* FIX: Unit > Replace > After replacing a unit, you might get a crash if when accessing the locals view in any of the unit's sub-chains.
* FIX: Unit > Edit Controls > Sometimes sub display graphics remain from a previous operation or focus.
* FIX: Unit > Context Scope > Leaving the context scope view sometimes results in a blank sub display that can only be fixed by moving the cursor away from the control and back on to it.
* FIX: Sample Pool > Edited samples were losing their path information and given generated names such as tmp-buffer-1005.wav and so on, instead of using the original filename and incrementing it.
* NEW: Quicksaves > Cleaned up summary display and now displaying the firmware version of the quicksave, graying out quicksaves that are not compatible.
* NEW: Unit Control > Number Widget > Committing a value will now also defocus the control to prevent inadvertently changing the value with the encoder.
* NEW: Sample Pool > Pressing ENTER will open the sample editor and slicer on the currently highlighted sample.

# v0.4.06

* FIX: Source Assignments > The output of certain sub-chains (note to self: branch output is a ParameterAdapter) was not propagating.
* FIX: Scale Quantizer > Scales are now sorted in dictionary order.
* FIX: Control Editor > Renamed controls were not showing the new name in the editor.
* FIX: Control Editor > Renaming a custom control was causing the control to disappear from presets.
* FIX: All Faders > Sometimes displayed values would have rounding errors (-0.1 would show as -0.099, etc.)
* FIX: Countdown Unit > There were two versions causing confusion, one in testlib and one in builtins.  Removed the one in testlib and fixed the one in builtins to properly count down again.

# v0.4.05

* FIX: All units > Crash when bypassing a unit.
* FIX: All units > Unable to clear the input assignment.
* FIX: File Browser > Crash when auditioning a sample.
* FIX: Clipboard > Crash when using the 'Move to Mixer' feature.
* FIX: Teletype > Failed to load SC.TR and SC.CV units.
* FIX: Filter units > Allow zero bias on the cutoff.
* FIX: Control Editor > Crash when deleting a control.
* FIX: Control Editor > Some edits to pitch controls were having no effect.

# v0.4.04

* WARNING: This version has no support for presets/quicksaves made with previous firmware versions. I will be looking at what it takes to support the legacy presets over the next few weeks.
* WARNING: There are changes in the middle layer API which means that bespoke units will need to be ported.
* FIX: Custom Units > Are back!
* FIX: Manual Loops > Sample Editor screen loses track of the active sample buffer when making edits.
* FIX: Source Assignments > There was a potential for source assignments using duplicated chains or units (such as when loading presets or pasting from the clipboard) to restore incorrectly.
* FIX: Slices > It was possible for slices to become unsorted and thus out of order with respect to their slice number.
* FIX: Local Sources > Sub-chains were tapped pre-control rather than post-control.
* NEW: Channel Link/Unlink > Linking will keep the contents of the upper chain.  Unlinking will duplicate the contents of the original stereo chain into the 2 resulting mono chains.
* NEW: Buttons > Improved button debouncing.
* NEW: All Faders > Added hysteresis to the encoder control for more precise dialing.
* NEW: All Faders > Focus-pressing Sx or SHIFT+Sx will bring up a numeric widget for dialing in an exact value (between -999,999 and 999,999, ignoring any fader minimum/maximum).  ENTER to commit new value and return.  SHIFT+ENTER to just commit but stay on the widget.  UP or CANCEL to return without committing.
* NEW: Locals > Assignment of the left vs right channel is dictated by which (OUTx) channel is currently selected.
* NEW: Unit Chooser > The 'Recent' category includes any recently pasted unit(s).
* NEW: All Units > You can now add custom controls to any unit and then use them as local sources in any of the sub-chains.
* NEW: Control Editor > Almost all unit controls (both custom and builtin) can be edited now.  Does the range or stepping of the bias or gain not suit your needs? Do you want to invert the output of a gate control or change its latching behavior? Just go to the unit menu and select 'Edit Controls'.
* NEW: Scale Quantizer > Any scala (*.scl) files located in ER-301/scales will be automatically added to the list of available scales.
* NEW UNITS: Two Bands .. Six Bands > Multi-band versions of the Custom Unit which can be used for parallel processing.  The unit's input is routed to each band untouched.  Each band is mixed to unity to produce the unit's output.
* SYS: Refactor > onLoadGraph(channelCount) > Use 'self' in place of 'pUnit'
* SYS: Refactor > ViewControl.Comparator --> ViewControl.Gate
* SYS: Refactor > ViewControl.InputComparator --> ViewControl.InputGate
* SYS: Refactor > ViewControl.PitchControl --> ViewControl.Pitch
* SYS: Refactor > edge Object --> comparator Object
* SYS: Refactor > onLoadViews(objects,branches) ...  return controls, views
* SYS: Refactor > onLoadMenu(objects,branches) ... return controls, menu, (sub)
* SYS: Refactor > self:addBranch(..) --> self:create(Mono|Stereo)Branch("tune", tune, "In", tune,"Out")

# v0.4.03

* FIX: Quicksaves > Since v0.4.02, OUTx chains were being restored on top of each other and sometimes in the wrong channel group.

# v0.4.02

* FIX: Memory leaks when loading quicksaves and certain types of units.
* FIX: Channel Mute > Garbage in the audio buffers was causing audible noise.
* FIX: Channel Mute > Flashing LEDs were not synced and channel selection was hard to see.
* FIX: Teletype > Removed old i2c addresses and set the default i2c address to 0x31 as required by Teletype v3.0.0 and above.
* FIX: Admin > Renamed Load screen to CPU Load.
* FIX: Middle Layer > Enabled compilation of units with internal feedback loops.
* NEW: Channel Setup > Moved out of the admin area and is completely accomplished in the user area by just chording the two channels that you want to link or unlink.

# v0.4.01

* FIX: Sample Auditioning > OUTx chain output would break after auditioning files on the same channels.
* FIX: Mixer Unit > Inserting a Mixer unit into a mono chain incorrectly showed stereo faders.
* FIX: Scope Mode > Button columns were too bright.

# v0.4.00

* FIX: Offset > In stereo chains, the offset parameter for the right channel was not being shown.
* FIX: Quicksaves > Occasional freeze when loading a CPU-heavy quicksave while running a CPU-heavy patch.
* FIX: Quicksaves > Sub-chain view from previous patch would remain after loading a quicksave.
* FIX: Level meters > Brief (ghost) rise in the displayed level when meter comes into view.
* JANITORIAL: CPU/Memory Load View > This view is now an item in the admin menu rather than its own context.  So you no longer use the STORAGE and MODE switches to access it.
* NEW: Preset files no longer use the *.lua extension regardless of the type of preset.
* NEW: Scope Mode > Navigate your OUTx chains with this bird's eye view.
* NEW: Chain Source Selection > Improved source selection UI and locals now include ANY unit or branch in the entire chain hierarchy.  Beware of feedback.

-----------------------------------

# v0.3.25

* FIX: All delay units > Feedback parameter turns to NaN when dialed all the way up.  The delay unit stops producing audio.
* FIX: Global Chains > Right channel of stereo global chains were not being restored properly from presets and quicksaves.

# v0.3.24

* FIX: System Settings > Crash when autosaving the system settings.
* TEMPFIX: Teletype Integration > Added i2c addresses 0x31 and 0x32.  Will remove conflicting addresses in v0.4.

# v0.3.23

* FIX: Bypass state of sub-chains was not being restored properly.
* FIX: Global Chains > Chain order was not being restored properly from presets and quicksaves.
* FIX: Slicing View > Sample region selection not working in slice navigation mode.
* FIX: All Slice Players > Looping broken for negative speeds in slice mode when there was only 2 slices, one at the beginning and one at the end.
* FIX: Sample Preview in File Browser > Sample preview UI failed to reset when end of playback was reached.
* FIX: Pedal Looper > In a stereo chain, the Pedal Looper undo and overdub functions were broken.
* FIX: Destructively editing a sample assigned to the Sample Scanner unit causes a crash.
* FIX: Slice Editor > Slices were not being wrapped around the beginning or end when shifting.
* NEW: Clocked Stretch and Clocked VariSpeed > Added menu option to specify the 'Stretch Duration' as 'all' or 'slice' (default).  When the 'Stretch Duration' is set to 'slice' then the playback speed will be set so that the current active slice (or the entire sample if no slices) has the same playback duration as the period of the tempo clock.  When the setting is set to 'all', then regardless of the active slice, the speed will be set so that the entire sample's duration matches the period of the tempo clock.
* NEW: Grid Quantizer > Levels parameter is now modulate-able.
* NEW: Sample Preview in File Browser > In addition to auto-looping very short samples (<50ms), the preview will now loop or not loop depending on the loop settings of the current unit.
* NEW: Sample Preview in File Browser > Added a system setting that lets you have the preview continue playing even if the focused sample changes.
* NEW: Slicing View > A new auto-slicer was added, called 'Slice to Division', which allows you to slice a sample into N equal length pieces.  For correctness and contrast, 'Slice to Grid' was renamed to 'Slice to Period'.

# v0.3.22

* FIX: Slicing and Sample Edit Views > Forgot to add Yes/No confirmations to cut and trim operations.
* FIX: Destructive Sample Edits > Loop points were not surviving the edit operations.
* FIX: Sample Players > Audible pop at the end of some samples even if faded out.

# v0.3.21

* FIX: Clocked Stretch > CPU spikes when clocked very fast (>10Hz) and attached to a long sample (> 1min).
* FIX: Oscillator units > Level was defaulting to 0.480 instead of 0.5.
* FIX: Mixer Unit > Ducking to silence during bypass is now applied to the Mixer's child chain instead of the Mixer's parent chain.
* FIX: V/oct Controls > Rounding error in the display of the cents value.
* NEW: Slicing and Sample Edit Views > Added Yes/No confirmations to destructive operations.
* NEW: Pedal Looper > Additional After Record Option: stop.

# v0.3.20

* FIX: Loading a sample chain with a mix of mono and stereo files would yield reversed slices.
* FIX: Manual Loops > Bias fader range was 0 to 1 when it should have been -1 to 1.
* NEW: Manual Loops > added a parameter called max.len which sets the loop length (in seconds) that corresponds to a value of 100% on the (loop) len parameter.
* NEW: Manual Loops > Added contextual waveform view.
* NEW: Manual Loops > Added (sample) edit command to the unit menu.

# v0.3.19

* FIX: Now catching errors during unit instantiation to prevent crashes when loading units with errors.
* FIX: Linear VCA is now renamed to Linear Bipolar VCA to differentiate it from the unipolar version.
* FIX: Clocked delays and Spread Delay were consuming more CPU than necessary (reduced CPU about 40%) due to inadvertent use of double floats in some parts of the calculation.
* FIX: Delay, Spread Delay, Clocked Delay, and Grain Delay > Delay times were being rounded to the control's coarse grid when restoring from presets or quicksaves.
* FIX: Crash when loading presets containing units with names that have consecutive spaces.
* FIX: Grain Stretch > Grain rendering was passing through slice boundaries.
* FIX: Grain Stretch > Periodic grain production was free-running rather than synced to incoming trigger.
* FIX: Slicing Menu > Disabled commands are shown grayed out instead of not shown at all.
* FIX: Sample Pool > Purge Unused > Would also purge unsaved buffers without warning.
* FIX: Waveform Views > Vertical and horizontal zoom were not saved with presets.
* NEW: All oscillator units > Oscillator phase is now saved with quicksaves and presets.
* NEW: Mute chain during unit bypass/delete/replace and certain menu operations > Parent chain is muted before performing these operations and then unmuted afterwards.
* NEW: Sustained Slices > Sustain option added to all players (unit menu > Play Duration > sustain) that support slices.  Go to the slicing view to specify the sustained (i.e. looped) segment of each slice.
* NEW: Quicksaving > If there is unsaved audio then a message will be shown giving you the option to go to the Pool and give the unsaved audio buffers a proper place to live on your SD card.
* NEW: Sample Pool > Entries with unsaved audio are now marked with an asterisk (*).
* NEW: Sample Scanner, Manual Grains now have contextual waveform views.
* NEW: Create sample chains On-the-Fly > Whenever you select multiple WAV files to load from the card, you can choose to load them as a (pre-sliced) sample chain.
* NEW: Sample Pool > You can now replace (M5) a focused sample with a new sample, updating all units that were using the old sample to use the new sample.
* NEW: Linear Unipolar VCA > This VCA includes a rectifier on the level modulation input.
* NEW: V/oct to Seconds > Outputs a base time (s) amount multiplied by the (fractional) number of octaves represented by the input (pitch) voltage.
* NEW: V/oct to Hertz >  Outputs a base frequency (Hz) amount multiplied by the (fractional) number of octaves represented by the input (pitch) voltage.
* NEW: Clocked Player > Speed of playback (and therefore the duration of the sample or active slice) is adjusted in real-time to match a clock.  Changes in tempo will cause changes in pitch.
* NEW: Clocked Stretch > Speed of playback (and therefore the duration of the sample or active slice) is adjusted in real-time to match a clock. Changes in tempo will NOT cause changes in pitch.
* NEW: Manual Loops > sample player with CV-able speed, loop start and length.

RENAMED UNITS:
Sample Player --> Variable Speed Player
Native Player --> Raw Player

# v0.3.18

* FIX: Crash when loading a preset in which a unit fails to load AND it subsequently tries to restore focus to this failed unit because that was what was focused when the preset was saved.
* FIX: When restoring a preset/quicksave from early v0.3.xx firmware, some units were not loading due to changes in unit type names.
* FIX: All gain parameters now go from -10 to 10 (previously -5 to 5).  FM gain now goes from -10k to 10k (prev -5k to 5k).
* NEW: Lua API > Added the ability to use arbitrary functional expression when tying object parameters together inside units.
* NEW: The "restore quicksave on boot" behavior now has a 3 second delay (with message) allowing you to cancel if desired.

# v0.3.17

* NEW: Manual Looper > Renamed to Feedback Looper.
* NEW: Brought back the original Sample Recorder (Looper) as Dub Looper.
* NEW: Counter > Trigger on the input causes output value to increment by 'step'.  Can be set to wrap or not.  Resettable with modulate-able start and finish.

# v0.3.16

* FIX: Loading Presets > Crash while loading certain presets with incorrect version information.
* FIX: Manual Grains > Pan control was broken by v0.3.14 changes.
* FIX: Grain Stretch > Clicks when using jitter on small grains and/or slow speeds.
* NEW: Pop up a warning message when the user tries to load (unit, chain, or quicksave) preset created on later firmware than what they are running.

# v0.3.15

* FIX: Players > Some options settings in the unit's menu were not adjustable.
* FIX: Unit Controls > V/oct controls were not adjustable when System Settings > "Displayed value for unit controls" set to "actual".
* FIX: Sample Recorder (Looper) renamed as Manual Looper.
* NEW: Manual Looper > Added menu item to turn latching on and off for the engage and punch controls.
* NEW: Unit Header > Removed the "re-init" command that no one uses and in its place there is now a "replace" command!  Certain units that are similar enough will even bring over control settings and their subchains.

# v0.3.14

* FIX: Pedal Looper > Pressing Undo while Overdubbing was broken.
* NEW: Pedal Looper > Menu item added to choose between rec>play and rec>overdub behavior.
* NEW: Pedal Looper > Menu item added to allow choosing exactly when and when not to pass input.
* NEW: Manual Grains > Added 'squash' parameter which you can use to squash the grain envelope smoothily into a box shape. The effect is the same as passing a Sine Envelope through a Limiter with the squash parameter corresponding to the pre-gain of the Limiter.
* NEW: Drastically reduced the amount of kernel memory needed when saving a preset or quicksave.

# v0.3.13

* FIX: Quantize To Clock > Was outputting a series of fast triggers rather than gates.
* NEW: Pedal Looper > Added menu items to export recorded audio to a buffer in the Sample Pool.

# v0.3.12

* FIX: Pedal Looper > Only recorded left channel when in a stereo chain.
* FIX: Clocked Delay > Was being created with a zero maximum loop time.
* NEW: Clocked Doppler Delay > Just like the Clocked Delay but using the Doppler (tape-style) Delay inside.

# v0.3.11

* FIX: Teletype units > Failing to load.

# v0.3.10

* FIX: Grain Stretch > Crash in the unit menu. (@x2mirko)

# v0.3.09

* FIX: Unit Control > Focus pressing a unit control no longer toggles the context view.  Instead, focus pressing will unfocus the control.  Pressing ENTER still toggles views, however. (@leverkusen)
* FIX: Any Player > Crash when leaving slice view that has no sample assigned. (@x2mirko @sixnon)
* FIX: Sample Recorder > Zipper noise when decreasing fade time.
* FIX: Fixed Delay > Feedback path had a fixed 128 sample delay, now reduced to 0 samples.
* FIX: Fixed Delay > Runaway feedback when modulating feedback above 0dB. (@tomk)
* FIX: Fixed Delay > Clicks and pops when changing the delay time.
* FIX: Delays have been renamed for clarity. Fixed Delay is now just Delay and is the most typical form of the delay.  Variable Delay has been renamed to Doppler Delay to emphasize its FM character (i.e. pitch changes when you modulate the delay time).  The Pitch Shifting Delay has been renamed to Grain Delay to reflect its underlying algorithm.
* FIX: Slicing View > Zero crossing search made more robust against long stretches of a constant value (i.e. silence).
* FIX: Clocked Delay > Ambiguous 'numerator' and 'divisor' terminology replaced with clock 'multiplier' and 'divider' terminology to match other units.
* FIX: Slicing View > Crash when zooming in the Slice Shift tool. (@tomk)
* FIX: All units that use samples > There was no way to detach a sample from a unit.
* NEW: Slicing View > Deleting slice in the same pixel as the cursor regardless of order. (@sunshinelover)
* NEW: Slicing View > Upon pressing delete (slice), if there is no slice inside the current view then no slice will be deleted.
* NEW: File Browser > Sample preview loops when the sample is less than 50ms, otherwise it plays it once.
* NEW: Sample Recorder > Replaced dub parameter with feedback parameter which controls how much of the original recording to mix with the input.
* NEW: Sample Recorder > Create and attach a buffer in one step without going to the Sample Pool.
* NEW: Clocked Delay > No longer changes pitch when clock tempo changes.
* NEW: Clocked Delay > CPU usage reduced 50% with no reduction in quality.
* NEW: Clocked Delay > Added spread/nudge parameter to offset the tap forward or backward from the clock.  In the stereo version of this unit, nudge becomes spread which nudges the left channel forward in time while nudging the right channel backwards.
* NEW: Delay, Spread Delay and Clocked Delay > Added menu item to set maximum delay time (0.1s, 1s, 10s, or 30s).
* NEW: Card Player > Added menu item to switch between 'play once' and 'loop'.
* NEW: Sample Player > Added menu item to select interpolation quality (none, linear, quadratic).
* NEW: All units that use samples > Now possible to choose a sample directly from the card without going to the Sample Pool.
* NEW: Tap Tempo > Removed tempo parameter (use a Clock unit instead).  Added multiplication parameter.
* NEW: Clock and Tap Tempo > Added menu item to enable/disable integer rounding of multipliers and dividers.
* NEW: Pulse to (Seconds, Frequency) units > Added multiplier and divider controls.
* NEW: System Settings > New settings for enabling/disabling various confirmations.
* NEW: Unit Controls > Digital readouts on the modulate-able controls can now show the post-modulation value (instead of the bias value).  Just go over to Admin > System Settings to enable the new behavior.
* NEW: Middle Layer Exposed > Place your custom unit library in ER-301/libs and it will be automatically loaded when the front SD card mounts.
* NEW: Spread Delay > Instead of separate left and right delay times, one delay time is shared by both channels while the (%) spread parameter nudges the left channel forward in time and the right channel backwards in time, or vice versa.  Available in stereo chains only.
* NEW: Clock > Simple clock source with modulate-able rate, multiplier, divider and pulse width.  Comes in 3 flavors: secs, BPM, and Hz.
* NEW: Pedal Looper > Simple looper inspired by pedal loopers.
* NEW: Quantize To Clock > Quantizes gates to a given clock.
* NEW: Deadband Filter > Output only changes when a change in the input exceeds a given threshold. Used for cleaning up noisy control signals without sacrificing responsiveness.

in progress
* NEW: Slicing View > Save selection to file.
* NEW: Clock division/multiplication
* NEW: Sample Pool > Load multiple samples as one sample, pre-sliced.

# v0.3.08

* (HOPEFULLY) FIX: Crash when recording to some SD cards.
* (HOPEFULLY) FIX: Card formatting sometimes failed.
* FIX: Card speed measurements were not reflecting actual speeds in practice.
* FIX: Card speed measurement would use too much memory and silently fail, yielding 0MB/s speed measurements.
* FIX: Teletype needed to disabled and re-enabled in order for a newly selected slave address to take effect.
* FIX: Minimum slew for Teletype CV changes was 2ms.  Now the minimum is zero.

# v0.3.07

* NEW: SD card file system improvements (speed and other things...).  You should format your card with the ER-301 > Admin > Card Console > Format Card (S2) to get the advantages.
* FIX: Slicing View > rewind(S1) was playing from the sample start rather than the cursor.

# v0.3.06

* NEW: Slew Limiter, 3ms to 786s.
* NEW: -10V to 10V are now mapped to -1 to 1 (previously -10.24V to 10.24V was mapped to -1 to 1).
* NEW: Teletype support added.

# v0.3.05

* FIX: Freeverb > Uninitialized comb filter state variable causing havoc (no audio, broken level meters) downstream of this unit.

# v0.3.04

* FIX: Sample & Hold > Broken by v0.3.03 update.
* FIX: Freeverb > Stops producing audio if 'size' is modulated out of range.
* NEW: Variable Delay > Feedback is now modulate-able.
* NEW: Clocked Delay > Feedback is now modulate-able.
* NEW: Freeverb > Reduced CPU usage by more than 50%.  Now 4% on 48kHz and 8% on 96kHz firmware.
* NEW: uDelay > Cheaper and simpler version of the Fixed Delay unit.  Use this instead whenever you need to nudge a signal forward in time for synchronicity.

# v0.3.03

* FIX: Legacy (v0.2) presets are loadable again.
* FIX: Slice on Onset > Results from median filter of peak heights were being ignored.
* FIX: Slice on Onset > Detection algorithm was not starting from the beginning of the buffer.
* FIX: Put back the missing expand all/shrink all function.  Focus the chain header and cycle with the ENTER button.
* FIX: Unit menu was activated on a button press (instead of a release) which allowed the button release to inadvertently activate a menu item.
* NEW: Added visual hints as to what will happen if you press a soft button again after already focusing a unit control or header (i.e. focused press).
* NEW: Bump Map now takes 1/3 of the CPU usage as before.  Also renamed Bump Map to Bump Scanner.
* NEW: Sample Scanner > Same as Bump Scanner except you can also 'Attach Sample' to be scanned by the input.  Set fade to 0 and width to 1 and height to 1 to use a sample as a lookup table (LUT).

# v0.3.02

* FIX: Preamp Settings > Crash on load/save preset.
* FIX: Grain Stretch + Sample Player > Not responding to triggers in some modes.

# v0.3.01

* FIX: Sample Player > Slice View > Cut operation was copying the wrong portions.
* FIX: Sample Player > Slice View > Zooming while onset/grid slicing caused a crash.
* FIX: Looper > Context View > Zooming causes a crash.
* FIX: Memory leak. GUI slows down (and possible crashes) when Looper context view is visible.

# v0.3

* NEW: Sample Player > Menu > 3 new attributes to control play head behavior: How often? (loop vs once). How much? (all vs slice). Which side? (left vs both vs right).
* NEW: Improved menu system that reduces scrolling and makes better use of screen real estate.
* NEW: Sample Player > Slice View > Slice on spectral onset.
* NEW: Sample Player > Slice View > Slice on grid.
* NEW: Sample Player > Slice View > Shift multiple slices
* NEW: Sample Player > Slice View > Select region with SHIFT+KNOB. Cancel clears the selection.
* NEW: Sample Player > Slice View > Destructive operations: trim, cut, normalize, silence, fade in, fade out, and remove DC.
* NEW: The KNOBMODE button toggles between time and amplitude zoom in the waveform displays.  Note the horizontal and vertical arrows next to the LEDs on the panel.
* NEW: Admin > Preamp Settings > IN1-4 preamp hardware is now configurable from the admin screen. Line level recording!
* NEW: All load and save dialogs remember their last file or folder choice.
* NEW: Sample Pool > When entering the Sample Pool from a unit menu, the unit's currently assigned sample is highlighted for easier reference.
* NEW: Sample Player > Slice View > Audition individual slices.
* NEW: Track & Hold > Two new modes: low = track on low and hold on high, minmax = running max on high and running min on low.
* NEW: System directory structure on front card is now organized by the major and minor version of the firmware (i.e. /ER-301/v0.3 instead of /ER-301/sc).
* FIX: Standardized (and minimized!) button presses required to navigate a chain and their units. Can you figure out how to bypass a unit? (^_^)
* FIX: Period-o-meter renamed as Pulse to Seconds to match the Pulse to Hertz unit.
* FIX: Sample Player > Last active slice shift was not being restored when loading a preset or quicksave.

* NEW: Grain Stretch, a slice-able sample player with independent control of pitch and speed.  Has the same slicing functionality as the Sample Player unit.
* NEW: Freeverb, the public domain and ubiquitous Schroeder reverberator from "Jezar at Dreampoint".
* NEW: Bump Map, maps the input signal to a bump function with adjustable center, width, height and fade.
* NEW: Scale Quantizer, quantize pitch CV to various scales.
* NEW: Velvet Noise, randomized impulse train.  Smoother than white noise but without the low roar of pink noise.
* NEW: Pulse to Hertz, measures the frequency of the incoming signal (after threspinned) and outputs the measured value in Hz.

# v0.2.27

* SYS: Updated to PROCESSOR-SDK-RTOS-AM335X 04_01_00_06
* FIX: Sample Recorder (Looper) sometimes failed to produce sound due to a bug in the fading algorithm.
* FIX: Sample Recorder (Looper) punch fade and reset fade time are now both set by the 'fade' parameter.
* FIX: Sample Recorder (Looper) clicks and pops when dynamically changing the reset period.
* FIX: Sample Recorder (Looper) has difficulty controlling loudness when doing sound-on-sound.
* FIX: Sample Recorder (Looper) exhibits erratic behavior when assigning mono buffer to a stereo unit.
* NEW: Buffer creation defaults to stereo when initiated from a stero unit.

# v0.2.26

* FIX: Card Player was causing presets to fail to load.

# v0.2.25

* FIX: Card Player was failing to load since v0.2.21.

# v0.2.24

* FIX: Some faders will go out of bounds when using the fine adjustment near the top of the fader's range.

# v0.2.23

* FIX: Activating contextual scope for the 2nd instance of some units caused various odd GUI behavior (collapsed units).

# v0.2.22

* FIX: Could not dial in 0Hz on oscillator units.
* NEW: On power up, show the name of the restored (or to be restored) quicksave.
* NEW: Added contextual scopes to all parameters.
* NEW: Offset unit is now modulate-able.

# v0.2.21

* NEW: Super Fine knob state added.  While in Fine mode, hold SHIFT and turn the knob.
* NEW: Super Coarse knob state added.  While in Coarse mode, hold SHIFT and turn the knob.
* NEW: Coarse/Fine knob state are now persisted for each unit control and saved/restored with presets.
* NEW: Pitch Shifting Delay wet parameter is now modulate-able.
* FIX: Tap Tempo pulse width was being calculated without taking into account the clock divider factor.
* NEW: Tap Tempo width parameter is now modulate-able.

# v0.2.20

* FIX: Crash when scrolling in (non-empty) slice list after deleting slices.
* FIX: Paste command would not appear after Copy/Cut unless you first moved the cursor.

# v0.2.19

* FIX: Crash when loading chain presets inside a Custom Unit's encapsulated chain.
* FIX: Occasional crash when deleting slices.
* EHNANCE: Clipboard for units and sequences of units. Hold SHIFT and turn the knob to select units, then Cut, Copy or Move to Mixer.  If configured in Admin>Settings, quicksaves persist clipboard contents.

# v0.2.18

* WARNING: Quicksaves produced with this version are not backwards compatible.

* FIX: Starvation of UI thread which caused freezes or very slow preset load times when CPU load is high.
* FIX: Inconsistencies in Sample Pool serialization/deserialization behavior.
* FIX: Unit and chain header text extending beyond boundaries.
* NEW: Exact Convolution, an experimental IR convolution unit that is rather heavy on CPU.  This unit will only load the first 48000 samples (48kHz firmware) or 24000 samples (96kHz firmware), and ignore the audio file's sampling rate.

# v0.2.17

* FIX: Channel linkage data was not restored properly from quicksaves.

# v0.2.16

* FIX: Sometimes loading a quicksave would leave the top level chains in a disabled state.

# v0.2.15

* FIX: In 6-track Recorder, saving a single track to a single file will fail if the destination file already exists.
* FIX: Possible float pointing exception (division by zero) in the Manual Grains unit when duration is zero.
* FIX: Pops would occasionally appear in IN1-4 and ABCD inputs at CPU loads above only 65%.
* FIX: Ladder Filter, Sine Osc, Aliasing Saw, and Aliasing Triangle units were not restoring f0 parameter from presets created prior to v0.2.12.
* FIX: Unable to re-enable a unit that was loaded from a preset in a disabled state.
* FIX: Display process was starved when loading quicksaves or presets.
* FIX: Unit control still focused when exiting from a subchain via UP button.
* FIX: Rational VCA was not rounding the numerator and denominator to the nearest integer since v0.2.14.

# v0.2.14

* FIX: Mixer unit's mute/solo state not being saved in presets and quicksaves.
* FIX: Unstable tracking of clock period in Clocking Delay unit.
* FIX: Occasional pops when modulating the delay time past the max delay of any of the Delay units.
* FIX: Ladder Filter would not self-oscillate unless provided with a fluctuating input.
* FIX: Disabled logging to the UART port unless development mode is enabled.
* NEW: Reduced CPU usage of White Noise unit by 70% with no change in quality.
* NEW: Phase modulation added to Sine Osc unit.
* NEW: Feedback (PM type) added to Sine Osc unit.
* NEW: Ladder High Pass Filter (HPF), 4-pole and resonant.
* NEW: Fixed High Pass Filter (HPF), 1-pole very efficient implementation for DC blocking applications (e.g. on FM inputs or after wave-shaping).
* NEW SETTING: Does bypassing a unit also disable it? Yes or no.

# v0.2.13

* FIX: TZFM was not inverting oscillator phase for negative frequencies.

# v0.2.12

* FIX: Custom Unit control assignments were arbitrarily reordered upon restoring from a preset.
* FIX: Duplicate control names in Custom Unit were allowed when renaming controls.
* FIX: XPath display stuck in sub display.
* NEW: Linear FM unlocked for Ladder Filter, Sine Osc, Aliasing Saw, and Aliasing Triangle.

# v0.2.11

* FIX: Custom Unit control assignments were not restored when loading legacy presets and quicksaves.
* FIX: Renaming or deleting any local or global sources (such as Custom Unit controls or global chains) were not being reflected in chain input assignments.
* FIX: Global Chains were not being restored from quicksaves but not showing up in the admin->Global Chains screens.
* NEW: Verification screen added to the "Clear All" command in the Custom Unit's control editor.
* NEW: Press SHIFT while the cursor is over any chain header to display an 'xpath' that describes the location of the chain in its hierarchy.

# v0.2.10

* FIX: Presets are not properly restoring shared (non-file-backed) buffers.
* FIX: Re-assigning a sample to a playing Manual Grains would sometimes cause the unit to "lose" volume due to resource contention in the grain pool.
* FIX: Global chains that refer to each other might not load correctly from presets (delayed name resolution needed)
* FIX: Ladder filter becomes unstable (and stops making sound) if the V/oct input sees a large positive input.
* FIX: Variable Delay has a 12kHz artifact when modulated in small amounts.
* FIX: Variable Delay slewing when modulated.
* FIX: Remove illegal characters from keyboard when naming a file or folder.
* FIX: Bypass does not work in top level of a Custom Unit.
* FIX: Crash when assigning local or global stereo sources that have names that do not end with a number.
* FIX: Stereo Pitch Shifting Delay was missing the V/oct input.
* FIX: High frequency artifacts in the Pitch Shifting Delay for non-integer speeds.
* NEW: Ladder Filter CPU usage reduced 50% with no change in quality.
* NEW: Added 1V/oct control to the Pitch Shifting Delay.
* NEW: Pressing UP when in a scoped parameter view will return the unit to its original parameter view.
* NEW: Parameters can now be renamed and moved in the Custom Unit.
* NEW: Toggle and Trigger parameter types were added to the Custom Unit.
* NEW: Sample Player unit now has a Stereo Routing option to control how stereo samples are played in mono (i.e. left, both, or right)
* NEW: Pitch Shifting Delay CPU usage reduced 50% with improvement in quality.

# v0.2.9

* NEW: Take a screenshot anytime using SHIFT+CANCEL.

# v0.2.8

* FIX: Occasional crash during WAV file auditioning (due to race condition).

# v0.2.7

* FIX: Error in resampling code causing some high frequency content to be attenuated.  Sample Player, Manual Grains, and Pitch-shifting Delay should sound slightly brighter now.

# v0.2.6

* FIX: Freeze when removing a triggering Manual Grain unit with an assigned buffer or sample that is short(ish).
* NEW: 6-track recorder will now append an auto-generated take number when saving to a folder that already contains previously saved audio tracks.
* NEW: 6-track recorder will use "save to file" semantics when there is one track to save, and, use "save to folder" semantics when there is more than one track to save.
* NEW SETTING: Configure 6-track recorder save semantics for single tracks (default is file).

# v0.2.5

* NEW: Rectify with full, positive-half and negative-half rectification.  Amongst other uses, put this in the level chain of the (normally bipolar) VCA to get a unipolar VCA.
* NEW: Skewed Sine Envelope with skew, duration and level parameters.
* NEW: Fold, splits incoming signal into upper and lower portions, applies separate gains and recombines.

* NEW: Global Chains were added to the Admin and Source chooser menus.
* NEW: Quicksave slots can be named.
* NEW: Envelope Follower unit parameters are now modulatable.
* NEW: Added contextual scope views (of unit output) to some more parameters.

* FIX: Zippering noise in Envelope Follower unit.
* FIX: Manual Grains sometimes stops making sound after assigning a new sample.
* FIX: After loading a preset, enumerated options were not reflecting new values.
* FIX: Quicksave UI was shown even if no card was mounted.

# v0.2.4

* FIX: Inconsistent triggering when slice 0 is at the exact beginning of the sample.
* FIX: Displayed slice number was off by one in the slicing view's slice list (but was correct in the sub display).

# v0.2.3

* FIX: Crash when removing LOOPER unit.
* FIX: Irrelevant pan parameter showing on mono Manual Grains.
* FIX: In sample pool, the 'purge unused' command would sometimes stop prematurely.

# v0.2.2

* FIX: Sound drops out when inserting a unit in a subchain after auditioning a file.
* NEW: Pan parameter added to Manual Grains.

# v0.2.1

* FIX: Out-of-bounds buffer access in Manual Grains.
* FIX: Insert button hidden by waveform display.
* FIX: Decibel label on signal meters are now fixed.

# v0.2.0

* NEW: WAV files can be auditioned in the file browser.
* NEW: Card speed measurement tools added.
* FIX: Multiple references to the same sample were not being saved properly in presets.
* NEW: last saved/loaded preset is used as suggestion when loading/saving presets
* NEW: Slice view directly accessible from context view in PLAYER unit.
* FIX: SHIFT+insert was ignored in the slicing view.
* NEW: SINE OSC unit CPU usage reduced to 40% of previous.
* NEW: LADDER FILTER unit CPU usage reduced to 75% of previous.
* NEW: Granular Player
* NEW: Aliasing Triangle
* NEW: Aliasing Saw
* NEW: Card Player
* NEW: Native Player
* NEW: In sample pool, 'Clear Pool' command has been changed to 'Purge Unused'.
* NEW: Additional slice addressing modes called 'nearest' and '12ET' were added.
* NEW: Decibel control added to Custom Unit.

# v0.1.6

* FIX: button debouncing time was incorrectly configured resulting in little to no debouncing.
* FIX: audio dropout when inserting LIMITER unit
* FIX: upon activation, input channel selection UI resets to collapsed state
* FIX: SAMPLE PLAYER's fade time parameter not getting updated when moving fader and stereo sample is loaded
* FIX: fixed potential race condition when deleting slices (hard to test though)
* FIX: zipper noise when changing modulation gain/bias via knob
* FIX: WET/DRY parameters were not clamped to [0,1]
* FIX: some obvious parameters were not being set when loading a mono in preset into a stereo context (and vice versa)
* FIX: when parameter is focused, pressing CANCEL now restores the original value (rather than zeroing it)
* FIX: bouncing cursor does not match selected spot sometimes
* NEW: slice offset parameter added to SAMPLE PLAYER
* NEW: scrolling in SAMPLE PLAYER's slice view now wraps at the begin/end of the sample
* NEW: added hard sync to SINE OSC unit
* NEW: you can now undo the following actions: clear chain, delete unit
* NEW: now in some units, if you can focus press a parameter to get its contextual view
* NEW: keyword filtering added to the unit selection screen
* NEW: 24 quicksave slots
* NEW: OUT1-4 can be routed to chain inputs
* NEW: units can be renamed
* QUESTIONABLE TWEAK: replaced SAMPLE PLAYER's linear fade with a cosine fade
* NEW: Custom Unit (encapsulate a chain of units)
(patch 2)
* FIX: firmware updater crashes when pressing the update button
(patch 3)
* FIX: focus pressing a control caused it to lose focus
* FIX: fixed some visual issues with fonts (0, r, and t)
* FIX: sub-chain edits were not being reflected in the parent parameter's miniscope
* FIX: local sources are placed first, in the source selection screen
* FIX: accidentally, left out the SINE unit's level parameter from UI
* NEW: GUI animation settings added (slow, normal, fast, disabled)
* NEW: Custom Unit names are randomly generated
* NEW: Custom Unit control editor, pressing S1 will cycle through options
* NEW: Unit presets that are saved in ER-301/sc/presets/units/chooser now show up in the unit selection screen.
(patch 4)
* FIX: Characters were being inserted on button release (should be on the press) in the keyboard screen.
* FIX: "Prompt" setting for reloading quicksaves on boot was not working.
* FIX: Unit names were not restoring properly when loading presets due to incorrect saving logic.
* FIX: Crash when inserting unit and front SD card is not mounted
* FIX: Suggested filenames are not longer erased when typing
(patch 5)
* FIX: Crash in sample slice view.
* FIX: Disabled wrapping around in slice view.
* FIX: Cursor not showing in channel setup.
* NEW: Focus press unit header to switch between unit's control view and menu view.
* NEW: Normal press unit header to left justify the unit.
* NEW: Unit bypass now implemented.
(patch 6)
* FIX: Crash when CPU is overloaded.
* FIX: Removed zombie sub-screens in admin menu.
* FIX: 6-track recorder is back with cleaner UI (and internals).
* NEW: 6-track recorder now has presets and it's state is saved with quicksaves.
* NEW: file/folder dialogs have been consolidated and improved.
* NEW: At anytime, SHIFT + channel buttons 1-4 will mute/unmute the corresponding chain.
* NEW: I've been stress testing the firmware with this CV-able STRESS unit.
(patch 7)
* FIX: Mono version of STRESS unit crashed on load.
* FIX: Garbage values in frequency measurement display over gate control in the CUSTOM unit.
(patch 8)
* FIX: limited pitch modulation range on CUSTOM unit
* FIX: In Chain Preset Saving, the button release from pressing "Save" was inadvertently activating the "New Folder" option on the next screen.
(patch 9)
* FIX: creating a new folder while saving chain/unit presets caused .lua to be appended to the folder name.
(patch 10)
* FIX: Got rid of the intermediate "switch" state on the units.
* FIX: 6-track recorder UI behaved weirdly when recording buffers overflowed.
* NEW: added context-sensitive hints to the 6-track recorder setup screen.
* NEW: audio is faded out/in whenever you mute/unmute a chain
* NEW: chain is now auto-muted before you clear or link/unlink channels
(patch 11)
* FIX: freeze when automatically restoring (w/o prompt) the last quicksave on boot.
* FIX: incorrect "failed to load quicksave" error message when loading quicksaves made before patch 6.
(patch 12)
* FIX: Sometimes unit subscreen contents are blank after returning from a menu action.
* FIX: When loading a quicksave, 6-track configuration is only loaded if not already recording.
* FIX: Track-and-Hold unit was not tracking when used in a mono chain.
* FIX: Unit bypass menu item was not working when the unit was collapsed.
* NEW: Changed default unit settings for MIXER and PLAYER.
* NEW: ADSR parameters have been extended out to 10sec.
(patch 13)
* FIX: LOOPER's fade on reset functionality was missing.
* FIX: Sequential channel bleed on the G inputs.
(patch 14)
* FIX: SHIFT+HOME (ZERO) now uses the press event rather than the release event.
* FIX: LOOPER preset will now restore buffer characteristics (max loop length and # of channels).
* FIX: Negative start parameter values in the LOOPER unit didn't wrap around.
* FIX: SAMPLE PLAYER would not stop at previous slice when speed is negative.
* NEW: LOOPER start parameter has been changed to proportion of maximum loop length (was seconds).
* NEW: Changed default frequency settings for SIN OSC and LADDER FILTER.

# v0.1.5p23
* FIX: overflow in feedback path of LADDER FILTER unit
* FIX: tune(v/oct) subchain of SAMPLE PLAYER was accidentally disabled
* FIX: deleting active slice did not deactivate it
* FIX: reduced SINE OSC unit output amplitude
* FIX: lower frequency range for SINE OSC unit
* FIX: frequency faders are now log-scaled

# v0.1.5p22
* FIX: occasional crash when quicksaving a large number of units
* FIX: can't delete sample when its status pending
* FIX: pressing shift in sample pool brings up obsolete screen
* STOPGAP: mute DAC on crash
* STOPGAP: units are presented in a fixed order in the (currently primitive) unit insert menu
* NEW: replaced tanh with cubic mode in LIMITER unit
* NEW: gains are modulatable in EQ3 unit (but still needs more attention)
* NEW: Sine Oscillator (exponential FM only for now)
* NEW: Ladder Filter (4-pole LPF with non-linearity in feedback)

# v0.1.5p21

- fixed crash when playing a slice in reverse at a speed above x2.5
- removed display of logs during loading/saving of presets
- added popup messages to many situations
- fixed cursor ending up off screen after loading chain preset
- crash using quicksave screen while cpu load screen (admin+scope) is showing
- increased button debounce time to 100ms

# v0.1.5p20

- support for 8-bit and 32-bit integer WAV files added
- capped CPU usage for PLAYER unit when playing at higher speeds without reducing fidelity

bug fixes
- crash when ejecting card that is in use (i.e. loading samples)
- occasional crash when deleting slices in player unit
- scrolling freaks out when chain is shorter than display
- buttons obscured by large amplitude waveforms
- parameter scopes don't update their targets when there is a change in the subchain.
- MIXER meters don't update their targets when there is a change.
- made preset loading more robust against file corruption

requests
- save state of engage/punch buttons in presets

# v0.1.5p19

bug fixes
- increased debounce time from 1ms to 50ms

# v0.1.5p17

bug fixes
- cleaned up some inconsistencies in the sample pool UI: better error messages and proper clearing of multi-selections.
- added extended WAVE file format support (some converters write in this format)
- added broadcast WAVE format (BWF) support (some portable recorders write in this format)

# v0.1.5p16

bug fixes
- default load path for unit preset did not use unit type
- fixed "crash" when aggressively modulating sliced sample playback speed
- clearing a chain now clears the input assignment

ui enhancements
- non-modulatable faders now have scopes that monitor unit output

looper
- renamed mix parameter to dub
- added wet/dry parameter to facilitate input monitoring

limiter
- added pre and post gain level meters

quantizer
- added new parameters: pre-scale, # of levels, post-scale

clocked delay
- time factor parameter has been substituted with 2 modulatable parameters: numerator and divisor

new units
- sample and hold
- track and hold
- rational vca
- period meter

new settings
- enable dev mode (please ignore this for now, it's just for me)

# v0.1.5p10

bug fixes
- empty chains now pass audio

new settings
- quicksave restore on start-up: yes, no, prompt
- set snap to zero-crossing commands
