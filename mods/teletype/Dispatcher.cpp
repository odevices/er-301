#include <teletype/Dispatcher.h>
#include <od/extras/LookupTables.h>
#include <od/AudioThread.h>
#include <hal/i2c.h>
#include <hal/timing.h>
#include <od/config.h>
#include <hal/ops.h>
#include <hal/log.h>
#include <hal/uart.h>
#include <limits.h>
#include <hal/simd.h>

// Command constants duplicated from:
// https://github.com/bpcmusic/telex/blob/master/software/TELEXo/telex.h

#define TO_TR 0x00
#define TO_TR_TOG 0x01
#define TO_TR_TIME 0x02
#define TO_TR_TIME_S 0x03
#define TO_TR_TIME_M 0x04
#define TO_TR_PULSE 0x05
#define TO_TR_POL 0x06

#define TO_CV 0x10
#define TO_CV_SET 0x11
#define TO_CV_SLEW 0x12
#define TO_CV_SLEW_S 0x13
#define TO_CV_SLEW_M 0x14
#define TO_CV_OFF 0x15

#define LOCAL_USE_NEON 1

namespace teletype
{

#define SUPPRESS_I2C 0

  Dispatcher::Dispatcher() : Task("Teletype Dispatcher")
  {
    mSamplesPerTick = globalConfig.sampleRate * ticks2secsD(1);
    mLastTimestamp = ticks();
    hardReset();
  }

  Dispatcher::~Dispatcher()
  {
    disable();
  }

  void Dispatcher::hardReset()
  {
    for (int port = 0; port < PortCount; port++)
    {
      mTRPolarity[port] = true;
      mTRPulseTime[port] = globalConfig.frameLength;

      mCVOffset[port] = 0.0f;
      commandCVSlew(port, mDefaultSlewTimeMs);
    }
    softReset();
  }

  void Dispatcher::softReset()
  {
    for (int port = 0; port < PortCount; port++)
    {
      mTRLevel0[port] = mTRPolarity[port] ? 0.0f : 1.0f;
      mTRLevel1[port] = mTRPolarity[port] ? 0.0f : 1.0f;
      mTRDelays[port] = 0;
      mTRPulseActive[port] = false;
      mTRPulseRemaining[port] = 0;

      mCVTarget[port] = 0.0f;
      mCVCurrent[port] = 0.0f;
      mCVDelays[port] = 0;
    }
  }

  void Dispatcher::commandTRSet(int delay, int port, int level)
  {
    mTRLevel0[port] = mTRLevel1[port];
    if (level)
    {
      mTRLevel1[port] = mTRPolarity[port] ? 1.0f : 0.0f;
    }
    else
    {
      mTRLevel1[port] = mTRPolarity[port] ? 0.0f : 1.0f;
    }
    mTRDelays[port] = delay;
    mTRPulseActive[port] = false;
    mTRPulseRemaining[port] = 0;
  }

  void Dispatcher::commandTRToggle(int delay, int port)
  {
    mTRLevel0[port] = mTRLevel1[port];
    mTRLevel1[port] = mTRLevel1[port] > 0 ? 0.0f : 1.0f;
    mTRDelays[port] = delay;
    mTRPulseActive[port] = false;
    mTRPulseRemaining[port] = 0;
  }

  void Dispatcher::commandTRPulse(int delay, int port)
  {
    mTRPulseActive[port] = true;
    mTRPulseRemaining[port] = mTRPulseTime[port] - globalConfig.frameLength + delay;
    mTRLevel0[port] = mTRLevel1[port];
    mTRLevel1[port] = mTRPolarity[port] ? 1.0f : 0.0f;
    mTRDelays[port] = delay;
  }

  void Dispatcher::commandTRPulseTime(int port, int ms)
  {
    static const int maxPulseTime = 60 * 60 * 1000;
    int sampleCount = MAX(
        (int)globalConfig.frameLength,
        MIN(maxPulseTime, ms) * ((int)globalConfig.sampleRate / 1000));
    mTRPulseTime[port] = sampleCount;
    mTRPulseRemaining[port] = MIN(mTRPulseRemaining[port], sampleCount);
  }

  void Dispatcher::commandTRPolarity(int delay, int port, bool polarity)
  {
    if (polarity != mTRPolarity[port])
    {
      mTRPolarity[port] = polarity;
      commandTRToggle(delay, port);
    }
  }

  void Dispatcher::updateTR()
  {
    for (int port = 0; port < PortCount; port++)
    {
      if (mTRPulseActive[port])
      {
        // decrement pulse remaining count
        if (mTRPulseRemaining[port] < (int)globalConfig.frameLength)
        {
          commandTRSet(mTRPulseRemaining[port], port, false);
        }
        else
        {
          mTRPulseRemaining[port] -= globalConfig.frameLength;
          // continue level as-is
          mTRLevel0[port] = mTRLevel1[port];
          mTRDelays[port] = 0;
        }
      }
      else
      {
        // continue level as-is
        mTRLevel0[port] = mTRLevel1[port];
        mTRDelays[port] = 0;
      }
    }
  }

  static inline float convertCV(int value)
  {
    return value * (1.0f / 16384);
  }

  void Dispatcher::commandCV(int delay, int port, int value)
  {
    mCVTarget[port] = convertCV(value) + mCVOffset[port];
    mCVDelays[port] = delay;
  }

  void Dispatcher::commandCVSlew(int port, int ms)
  {
    static const int maxSlewTime = 60 * 60 * 1000;
    float slewTime = MAX(globalConfig.framePeriod, MIN(maxSlewTime, ms) * 0.001f);
    // Travel at most this much in one frame.
    float maxDiff = globalConfig.framePeriod / slewTime;
    mCVMaxDiff[port] = maxDiff;
    mCVSlewEnabled[port] = ms > 0;
  }

  void Dispatcher::commandCVSet(int delay, int port, int value)
  {
    commandCV(delay, port, value);
    mCVCurrent[port] = mCVTarget[port];
  }

  void Dispatcher::commandCVOffset(int delay, int port, int value)
  {
    float oldOffset = mCVOffset[port];
    mCVOffset[port] = convertCV(value);
    commandCV(delay, port, mCVTarget[port] - oldOffset);
  }

  void Dispatcher::updateCV()
  {
    float inverseFrameLength = globalConfig.frameRate * globalConfig.samplePeriod;
    int port = 0;
#if LOCAL_USE_NEON
    int n4 = 4 * (PortCount / 4);
    float32x4_t r = vdupq_n_f32(inverseFrameLength);
    int32x4_t n = vdupq_n_s32(globalConfig.frameLength);
    int32x4_t zero = vdupq_n_s32(0);
    for (port = 0; port < n4; port += 4)
    {
      int32x4_t delay = vld1q_s32((int32_t *)mCVDelays + port);
      float32x4_t maxDiff = vcvtq_f32_s32(n - delay) * r * vld1q_f32(mCVMaxDiff + port);
      float32x4_t target = vld1q_f32(mCVTarget + port);
      float32x4_t current = vld1q_f32(mCVCurrent + port);
      float32x4_t diff = target - current;
      uint32x4_t more = vcgtq_f32(diff, maxDiff);
      uint32x4_t less = vcltq_f32(diff, -maxDiff);
      diff = vbslq_f32(more, maxDiff, diff);
      diff = vbslq_f32(less, -maxDiff, diff);
      current += diff;
      vst1q_f32(mCVCurrent + port, current);
      vst1q_s32((int32_t *)mCVDelays + port, zero);
    }
#endif
    for (; port < PortCount; port++)
    {
      float maxDiff = mCVMaxDiff[port] * (globalConfig.frameLength - mCVDelays[port]) * inverseFrameLength;
      float diff = CLAMP(-maxDiff, maxDiff,
                         mCVTarget[port] - mCVCurrent[port]);
      mCVCurrent[port] += diff;
      mCVDelays[port] = 0;
    }
  }

  static inline int getPort(I2cMessage &msg)
  {
    if (msg.length > 1)
    {
      return CLAMP(Dispatcher::MinPort,
                   Dispatcher::MaxPort, msg.data[1] + 1);
    }
    else
    {
      return Dispatcher::NullPort;
    }
  }

  static inline int getValue(I2cMessage &msg)
  {
    if (msg.length > 3)
    {
      uint16_t temp = (uint16_t)((msg.data[2] << 8) + (msg.data[3]));
      int16_t temp2 = (int16_t)temp;
      return temp2;
    }
    else
    {
      return 0;
    }
  }

  int Dispatcher::ticksToSamples(tick_t nticks)
  {
    return nticks * mSamplesPerTick;
  }

  void Dispatcher::process(float *inputs, float *outputs)
  {
    updateTR();
    updateCV();

    I2cMessage msg;
    while (I2c_popMessage(&msg))
    {

      if (msg.length == 0)
      {
        continue;
      }

      int delay = CLAMP(0, FRAMELENGTH - 1, ticksToSamples(msg.timestamp - mLastTimestamp));

      // Diagnostic: pulse the null port for every received message.
      commandTRPulse(delay, 0);

      switch (msg.data[0])
      {
      case TO_TR:
        // SC.TR 1-n �� --> Set TR value to �� (0/1)
        commandTRSet(delay, getPort(msg), getValue(msg));
        break;
      case TO_TR_TOG:
        // SC.TR.TOG 1-n --> Toggle TR
        commandTRToggle(delay, getPort(msg));
        break;
      case TO_TR_TIME:
        // SC.TR.TIME 1-n �� -->  time for TR.PULSE; �� in milliseconds
        commandTRPulseTime(getPort(msg), getValue(msg));
        break;
      case TO_TR_TIME_S:
        // SC.TR.TIME.S 1-n �� -->  time for TR.PULSE; �� in seconds
        commandTRPulseTime(getPort(msg), getValue(msg) * 1000);
        break;
      case TO_TR_TIME_M:
        // SC.TR.TIME.M 1-n �� --> time for TR.PULSE; �� in minutes
        commandTRPulseTime(getPort(msg), getValue(msg) * 60000);
        break;
      case TO_TR_PULSE:
        // SC.TR.PULSE 1-n --> Pulse TR using TO.TR.TIME/S/M as an interval
        commandTRPulse(delay, getPort(msg));
        break;
      case TO_TR_POL:
        // SC.TR.POL 1-n �� --> polarity for TO.TR.PULSE set to �� (0-1)
        commandTRPolarity(delay, getPort(msg), getValue(msg));
        break;

      case TO_CV:
        // SC.CV 1-n �� --> CV target �� (bipolar)
        commandCV(delay, getPort(msg), getValue(msg));
        break;
      case TO_CV_SET:
        // SC.CV.SET 1-n �� --> set CV to �� (bipolar); ignoring SLEW
        commandCVSet(delay, getPort(msg), getValue(msg));
        break;
      case TO_CV_SLEW:
        // SC.CV.SLEW 1-n �� --> CV slew time; �� in milliseconds
        commandCVSlew(getPort(msg), getValue(msg));
        break;
      case TO_CV_SLEW_S:
        // SC.CV.SLEW.S 1-n �� --> CV slew time; �� in seconds
        commandCVSlew(getPort(msg), getValue(msg) * 1000);
        break;
      case TO_CV_SLEW_M:
        // SC.CV.SLEW.M 1-n �� --> CV slew time; �� in minutes
        commandCVSlew(getPort(msg), getValue(msg) * 60000);
        break;
      case TO_CV_OFF:
        // SC.CV.OFF 1-n �� --> CV offset; �� added at final stage
        commandCVOffset(delay, getPort(msg), getValue(msg));
        break;

      default:
        // unknown command
        break;
      }
    }

    mLastTimestamp = ticks();
  }

  void Dispatcher::fillTRFrame(int port, float *out)
  {
    for (int i = 0; i < mTRDelays[port]; i++)
    {
      out[i] = mTRLevel0[port];
    }
    for (int i = mTRDelays[port]; i < FRAMELENGTH; i++)
    {
      out[i] = mTRLevel1[port];
    }
  }

  float Dispatcher::fillCVFrame(int port, float *out, float lastValue)
  {
    if (mCVSlewEnabled[port])
    {
      float *ramp = od::LookupTables::FrameOfLinearRamp.mValues.data();
      float diff = mCVCurrent[port] - lastValue;

#if LOCAL_USE_NEON
      float32x4_t p = vdupq_n_f32(lastValue);
      float32x4_t d = vdupq_n_f32(diff);
      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        vst1q_f32(out + i, p + d * vld1q_f32(ramp + i));
      }
#else
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = lastValue + diff * ramp[i];
      }
#endif
    }
    else
    {
      for (int i = 0; i < mCVDelays[port]; i++)
      {
        out[i] = lastValue;
      }
      for (int i = mCVDelays[port]; i < FRAMELENGTH; i++)
      {
        out[i] = mCVTarget[port];
      }
    }
    return out[FRAMELENGTH - 1];
  }

  void Dispatcher::enable(uint32_t address)
  {
    if (!mEnabled)
    {
      logInfo("Enabling Teletype I2C on 0x%x. ", address);
#if SUPPRESS_I2C && BUILDOPT_TESTING
      Uart_enable();
#else
      Uart_disable();
      I2c_init();
      I2c_openSlave(address);
#endif
      softReset();
      od::AudioThread::addTask(this, INT_MAX - 1);
      mEnabled = true;
    }
  }

  void Dispatcher::disable()
  {
    if (mEnabled)
    {
      od::AudioThread::removeTask(this);
      mEnabled = false;
      I2c_closeSlave();
      I2c_deinit();
    }
  }

  void Dispatcher::setDefaultSlew(int ms)
  {
    mDefaultSlewTimeMs = ms;
  }

} /* namespace teletype */
