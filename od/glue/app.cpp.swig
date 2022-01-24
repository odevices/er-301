%module app
%import <od/glue/prologue.cpp.swig>
%typemap(out) std::vector<std::string>
%{
	stringVectorToLuaTable(L, $1);
	SWIG_arg++;
%}

%{
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#undef SWIGLUA

#include <hal/log.h>
#include <hal/constants.h>
#include <hal/audio.h>
#include <hal/modulation.h>
#include <hal/channels.h>
#include <hal/encoder.h>
#include <hal/gpio.h>
#include <hal/card.h>
#include <od/config.h>
#include <hal/usb.h>
#include <hal/uart.h>
#include <hal/adc.h>
#include <hal/pwm.h>
#include <hal/display.h>
#include <hal/rng.h>
#include <hal/heap.h>
#include <hal/events.h>
#include <hal/fileops.h>
#include <hal/reboot.h>

#include <od/constants.h>
#include <od/extras/ReferenceCounted.h>
#include <hal/concurrency/Thread.h>
#include <od/audio/Sample.h>
#include <od/audio/Slices.h>

#include <od/objects/Inlet.h>
#include <od/objects/Outlet.h>
#include <od/objects/Parameter.h>
#include <od/objects/Option.h>
#include <od/objects/StateMachine.h>
#include <od/objects/Object.h>

#include <od/extras/Utils.h>
#include <od/extras/CardInfo.h>
#include <od/extras/Random.h>
#include <od/ui/Busy.h>
#include <od/ui/LUTDialMap.h>
#include <od/ui/LinearDialMap.h>
#include <od/extras/ZipArchiveReader.h>
#include <od/extras/ZipArchiveWriter.h>
#include <od/ui/JobQueue.h>
#include <od/ui/EncoderHysteresis.h>
#include <od/extras/FileReader.h>
#include <od/audio/WavFileReader.h>
#include <od/audio/WavFileWriter.h>	
#include <od/audio/SampleLoader.h>
#include <od/audio/SampleSaver.h>

#include <od/glue/Expression.h>
#include <od/glue/LongestPath.h>

// tasks

#include <od/tasks/UnitChain.h>
#include <od/tasks/ObjectList.h>
#include <od/tasks/InputTask.h>
#include <od/tasks/OutputTask.h>

// units

#include <od/units/Unit.h>
#include <od/units/EffectUnit.h>
#include <od/units/SourceUnit.h>

// objects

#include <od/objects/math/Sum.h>
#include <od/objects/math/Multiply.h>
#include <od/objects/math/Constant.h>
#include <od/objects/math/ConstantOffset.h>
#include <od/objects/math/ConstantGain.h>
#include <od/objects/math/GainBias.h>
#include <od/objects/math/Gain.h>

#include <od/objects/timing/Comparator.h>

#include <od/objects/mixing/Mixer.h>
#include <od/objects/mixing/CrossFade.h>
#include <od/objects/mixing/MonoPanner.h>
#include <od/objects/mixing/StereoPanner.h>
#include <od/objects/mixing/StereoCrossFade.h>
#include <od/objects/mixing/StereoToMono.h>

#include <od/objects/measurement/Monitor.h>
#include <od/objects/measurement/MinMax.h>
#include <od/objects/measurement/Stress.h>

#include <od/objects/heads/Head.h> 
#include <od/objects/heads/TapeHead.h>
#include <od/objects/heads/SliceHead.h> 

#include <od/objects/file/FileSinkThread.h>
#include <od/objects/file/FileSink.h>
#include <od/objects/file/MonoFileSink.h>
#include <od/objects/file/StereoFileSink.h>
#include <od/objects/file/FileSource.h>

#include <od/objects/adapters/ParameterAdapter.h>
#include <od/objects/control/ParamSetMorph.h>

#include <od/objects/TestOscillator.h>

// graphics

#include <od/graphics/FrameBuffer.h>
#include <od/graphics/MainFrameBuffer.h>
#include <od/graphics/SubFrameBuffer.h>

#include <od/graphics/DrawingInstructions.h>
#include <od/graphics/Graphic.h>
#include <od/graphics/GraphicContext.h>
#include <od/graphics/Drawing.h>

#include <od/graphics/text/Label.h>
#include <od/graphics/text/TextEditor.h>
#include <od/graphics/text/TextPanel.h>
#include <od/graphics/text/RichTextBox.h>

#include <od/graphics/lists/ListBox.h>
#include <od/graphics/lists/SlidingList.h>
#include <od/graphics/lists/MondrianList.h>

#include <od/graphics/charts/StripChart.h>

#include <od/graphics/meters/MiniScope.h>
#include <od/graphics/meters/VerticalMeter.h>

#include <od/graphics/spotted/SpottedSection.h>
#include <od/graphics/spotted/SpottedStrip.h>

#include <od/graphics/controls/Readout.h>
#include <od/graphics/controls/SoftButton.h>
#include <od/graphics/controls/ComparatorView.h>
#include <od/graphics/controls/StateMachineView.h>
#include <od/graphics/controls/Fader.h>
#include <od/graphics/controls/BinaryIndicator.h>
#include <od/graphics/controls/LinkDisplay.h>
#include <od/graphics/controls/DurationControl.h>
#include <od/graphics/controls/SixDigits.h>
#include <od/graphics/controls/MenuArc.h>

#include <od/graphics/holdview/MorphFader.h>
#include <od/graphics/holdview/TriggerGroup.h>

#include <od/graphics/fs/FileBrowser.h>

#include <od/graphics/recording/RecordingGraphic.h>

#include <od/graphics/sampling/SliceList.h>
#include <od/graphics/sampling/FileSourceDisplay.h>
#include <od/graphics/sampling/PositionDisplay.h>
#include <od/graphics/sampling/SampleView.h>

#include <od/graphics/sampling/HeadDisplay.h>
#include <od/graphics/sampling/HeadSubDisplay.h>
#include <od/graphics/sampling/TapeHeadDisplay.h>
#include <od/graphics/sampling/SliceHeadDisplay.h>
#include <od/graphics/sampling/SliceHeadSubDisplay.h>

#include <od/graphics/sampling/SampleEditorMainDisplay.h>
#include <od/graphics/sampling/SampleEditorSubDisplay.h>

#include <od/graphics/sampling/SlicingViewMainDisplay.h>
#include <od/graphics/sampling/SlicingViewSubDisplay.h>

#include <od/graphics/wayfinding/MoreThisWay.h>

#include <od/graphics/birdseye/ChainOverview.h>
#include <od/graphics/birdseye/XPath.h>

#include <od/graphics/text/Utils.h>

// systems

#include <od/ui/ChannelLEDs.h>
#include <od/AudioThread.h>
#include <od/UIThread.h>
#include <od/extras/BigHeap.h>
#include <od/extras/Profiler.h>

#define SWIGLUA

int stringVectorToLuaTable(lua_State* L, std::vector<std::string> & vec);

using namespace od;

%}

namespace od {
}

using namespace od;

%include <hal/constants.h>
%include <hal/modulation.h>
%include <hal/channels.h>
%include <hal/gpio.h>
%include <hal/events.h>
%include <od/config.h>
%include <hal/fileops.h>
%include <hal/adc.h>
%include <hal/usb.h>
%include <hal/reboot.h>
%include <hal/concurrency/Thread.h>

float wallclock(void);
unsigned long micros(void);
int logCount();

void Heap_print(void);
int Heap_getSize(int units);
int Heap_getFreeSize(int units);

int Encoder_getChange(void);

void Display_start(void);
void Pwm_start(void);

void Audio_start(void);
void Audio_stop(void);
void Audio_restart(void);
void Audio_printErrorStatus(void);
int Audio_getRate(void);
int Audio_getLoad();

uint32_t Rng_read32(void);

bool Card_format(uint32_t drv);
bool Card_mount(uint32_t drv);
bool Card_isMounted(uint32_t drv);
void Card_unmount(uint32_t drv);
void Card_printErrorStatus(void);

void Uart_enable();
void Uart_disable();
bool Uart_isEnabled();

%include <od/constants.h>
%import <od/extras/Lockable.h>
%include <od/extras/ReferenceCounted.h>
%include <od/ui/Busy.h>
%include <od/extras/CardInfo.h>
%include <od/extras/ZipArchiveReader.h>
%include <od/extras/ZipArchiveWriter.h>
%include <od/ui/DialMap.h>
%include <od/ui/LUTDialMap.h>
%include <od/ui/LinearDialMap.h>
%include <od/ui/Job.h>
%include <od/ui/JobQueue.h>
%include <od/ui/EncoderHysteresis.h>  
%include <od/extras/FileReader.h>
%include <od/extras/FileWriter.h>
%include <od/extras/Random.h>

%include <od/audio/Sample.h>
%include <od/audio/Slices.h>
%include <od/audio/SampleLoadInfo.h>
%include <od/audio/SampleLoader.h>
%include <od/audio/SampleSaver.h>
%include <od/audio/SoundFileReader.h>
%include <od/audio/WavFileReader.h>
%include <od/audio/SoundFileWriter.h>
%include <od/audio/WavFileWriter.h>	

%include <od/objects/Port.h>
%include <od/objects/Inlet.h>
%include <od/objects/Outlet.h>
%include <od/objects/Followable.h>
%include <od/objects/Settable.h>
%include <od/objects/Parameter.h>
%include <od/objects/Option.h>
%include <od/objects/StateMachine.h>
%include <od/objects/Object.h> 

// tasks

%import <od/tasks/Task.h>
%include <od/tasks/UnitChain.h>
%include <od/tasks/ObjectList.h>
%include <od/tasks/InputTask.h>
%include <od/tasks/OutputTask.h>

// units

%include <od/units/Unit.h>
%include <od/units/CustomUnit.h>
%include <od/units/EffectUnit.h>
%include <od/units/SourceUnit.h>

// objects

%include <od/objects/math/Sum.h>
%include <od/objects/math/Multiply.h>
%include <od/objects/math/Constant.h>
%include <od/objects/math/ConstantOffset.h>
%include <od/objects/math/ConstantGain.h>
%include <od/objects/math/GainBias.h>
%include <od/objects/math/Gain.h>

%include <od/objects/timing/Comparator.h>

%include <od/objects/mixing/Mixer.h>
%include <od/objects/mixing/CrossFade.h>
%include <od/objects/mixing/MonoPanner.h>
%include <od/objects/mixing/StereoPanner.h>
%include <od/objects/mixing/StereoCrossFade.h>
%include <od/objects/mixing/StereoToMono.h>

%include <od/objects/measurement/MinMax.h>
%include <od/objects/measurement/Stress.h>
%include <od/objects/measurement/Monitor.h>

%include <od/objects/heads/Head.h> 
%include <od/objects/heads/TapeHead.h>
%include <od/objects/heads/SliceHead.h> 

%include <od/objects/file/FileSinkThread.h>
%include <od/objects/file/FileSink.h>
%include <od/objects/file/MonoFileSink.h>
%include <od/objects/file/StereoFileSink.h>
%include <od/objects/file/FileSource.h>

%include <od/objects/adapters/ParameterAdapter.h>
%include <od/objects/control/ParamSetMorph.h>

%include <od/objects/TestOscillator.h>

%include <od/glue/LongestPath.h>
%include <od/glue/Expression.h>

%include <od/graphics/constants.h>
%include <od/graphics/primitives.h>
%include <od/graphics/DrawingInstructions.h>
%include <od/graphics/Graphic.h>
%include <od/graphics/GraphicContext.h>
%include <od/graphics/Drawing.h>

%include <od/graphics/text/Label.h>
%include <od/graphics/text/TextEditor.h>
%include <od/graphics/text/TextPanel.h>
%include <od/graphics/text/RichTextBox.h>

%include <od/graphics/lists/ListBox.h>
%include <od/graphics/lists/SlidingList.h>
%include <od/graphics/lists/MondrianList.h>

%include <od/graphics/charts/StripChart.h>

%include <od/graphics/meters/MiniScope.h>
%include <od/graphics/meters/VerticalMeter.h>

%include <od/graphics/spotted/SpottedSection.h>
%include <od/graphics/spotted/SpottedStrip.h>

%include <od/graphics/controls/Readout.h>
%include <od/graphics/controls/SoftButton.h>
%include <od/graphics/controls/ComparatorView.h>
%include <od/graphics/controls/StateMachineView.h>
%include <od/graphics/controls/Fader.h>
%include <od/graphics/controls/BinaryIndicator.h>
%include <od/graphics/controls/LinkDisplay.h>
%include <od/graphics/controls/DurationControl.h>
%include <od/graphics/controls/SixDigits.h>
%include <od/graphics/controls/MenuArc.h>

%include <od/graphics/holdview/MorphFader.h>
%include <od/graphics/holdview/TriggerGroup.h>

%include <od/graphics/fs/FileBrowser.h>

%include <od/graphics/recording/RecordingGraphic.h>

%include <od/graphics/sampling/SliceList.h>
%include <od/graphics/sampling/FileSourceDisplay.h>
%include <od/graphics/sampling/PositionDisplay.h>
%include <od/graphics/sampling/SampleView.h>

%include <od/graphics/sampling/HeadDisplay.h>
%include <od/graphics/sampling/HeadSubDisplay.h>
%include <od/graphics/sampling/TapeHeadDisplay.h>
%include <od/graphics/sampling/SliceHeadDisplay.h>
%include <od/graphics/sampling/SliceHeadSubDisplay.h>

%include <od/graphics/sampling/SampleEditorMainDisplay.h>
%include <od/graphics/sampling/SampleEditorSubDisplay.h>

%include <od/graphics/sampling/SlicingViewMainDisplay.h>
%include <od/graphics/sampling/SlicingViewSubDisplay.h>

%include <od/graphics/wayfinding/MoreThisWay.h>

%include <od/graphics/birdseye/ChainOverview.h>
%include <od/graphics/birdseye/XPath.h>

// ui

%include <od/ui/ChannelLEDs.h>
%include <od/AudioThread.h>
%include <od/UIThread.h>
%include <od/extras/BigHeap.h>
%include <od/extras/Profiler.h>

bool glob(const char * text, const char * pattern);
int getTextWidth(const char * text, int fontSize);
