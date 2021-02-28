#include <od/tasks/InputTask.h>
#include <od/config.h>
#include <hal/channels.h>
#include <string.h>

namespace od
{

  InputTask::InputTask() : Task("InputTask")
  {

    own(mA1);
    own(mA2);
    own(mA3);

    own(mB1);
    own(mB2);
    own(mB3);

    own(mC1);
    own(mC2);
    own(mC3);

    own(mD1);
    own(mD2);
    own(mD3);

    own(mG1);
    own(mG2);
    own(mG3);
    own(mG4);

    own(mIN1);
    own(mIN2);
    own(mIN3);
    own(mIN4);

    // and don't let go
  }

  InputTask::~InputTask()
  {
  }

  static inline void copyFromInputs(float *__restrict__ dst,
                                    float *__restrict__ src, uint32_t channel)
  {
    float *end = dst + FRAMELENGTH;
    src += channel;
    for (; dst < end; src += NUM_INPUT_CHANNELS, dst++)
    {
      *dst = *src;
    }
  }

#define INPUTCOPY(x)                                  \
  if (m##x.isConnected())                             \
  {                                                   \
    copyFromInputs(m##x.buffer(), inputs, INPUT_##x); \
  }

  void InputTask::process(float *inputs, float *outputs)
  {
    // cache the last value of input frame for display purposes
    memcpy(mLastInput,
           &inputs[(NUM_INPUT_CHANNELS) * (globalConfig.frameLength - 1)],
           NUM_INPUT_CHANNELS * sizeof(float));

    INPUTCOPY(A1);
    INPUTCOPY(A2);
    INPUTCOPY(A3);

    INPUTCOPY(B1);
    INPUTCOPY(B2);
    INPUTCOPY(B3);

    INPUTCOPY(C1);
    INPUTCOPY(C2);
    INPUTCOPY(C3);

    INPUTCOPY(D1);
    INPUTCOPY(D2);
    INPUTCOPY(D3);

    INPUTCOPY(G1);
    INPUTCOPY(G2);
    INPUTCOPY(G3);
    INPUTCOPY(G4);

    INPUTCOPY(IN1);
    INPUTCOPY(IN2);
    INPUTCOPY(IN3);
    INPUTCOPY(IN4);
  }

} /* namespace od */
