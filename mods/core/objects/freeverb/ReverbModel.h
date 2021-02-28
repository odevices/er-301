/*
 * ReverbModel.h
 *
 * Neon-optimized adaptation of Freeverb.
 *
 *
 *  Created on: 12 Dec 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_FREEVERB_REVERBMODEL_H_
#define APP_OBJECTS_FREEVERB_REVERBMODEL_H_

#include "tuning.h"
#include <hal/simd.h>

namespace od
{

  class ReverbModel
  {
  public:
    ReverbModel();
    virtual ~ReverbModel();

    void process(float *inputL, float *inputR, float *outputL, float *outputR);
    void set(float size, float damp, float width);
    void mute();

    void freezeOn()
    {
      freeze = true;
    }
    void freezeOff()
    {
      freeze = false;
    }

  private:
    float gain;
    float roomsize, roomsize1;
    float damp, damp1;
    float mixL, mixR;
    float width;
    bool freeze = false;

    // comb parameters
    float32x2_t combFeedback;
    float32x2_t combDamp1;
    float32x2_t combDamp2;

    struct Pair
    {
      float *bufferL;
      int bufsizeL;
      int bufidxL = 0;
      float *bufferR;
      int bufsizeR;
      int bufidxR = 0;

      void setLeftBuffer(float *buf, int size)
      {
        bufferL = buf;
        bufsizeL = size;
      }

      void setRightBuffer(float *buf, int size)
      {
        bufferR = buf;
        bufsizeR = size;
      }

      void mute()
      {
        for (int i = 0; i < bufsizeL; i++)
          bufferL[i] = 0;
        for (int i = 0; i < bufsizeR; i++)
          bufferR[i] = 0;
      }
    };

    struct combPair : public Pair
    {
      float32x2_t filterstore;
    };

    // Comb filters
    combPair comb[numcombs];

    struct allpassPair : public Pair
    {
    };

    // Allpass filters
    allpassPair allpass[numallpasses];

    // Buffers for the combs
    float bufcombL1[combtuningL1];
    float bufcombR1[combtuningR1];
    float bufcombL2[combtuningL2];
    float bufcombR2[combtuningR2];
    float bufcombL3[combtuningL3];
    float bufcombR3[combtuningR3];
    float bufcombL4[combtuningL4];
    float bufcombR4[combtuningR4];
    float bufcombL5[combtuningL5];
    float bufcombR5[combtuningR5];
    float bufcombL6[combtuningL6];
    float bufcombR6[combtuningR6];
    float bufcombL7[combtuningL7];
    float bufcombR7[combtuningR7];
    float bufcombL8[combtuningL8];
    float bufcombR8[combtuningR8];

    // Buffers for the allpasses
    float bufallpassL1[allpasstuningL1];
    float bufallpassR1[allpasstuningR1];
    float bufallpassL2[allpasstuningL2];
    float bufallpassR2[allpasstuningR2];
    float bufallpassL3[allpasstuningL3];
    float bufallpassR3[allpasstuningR3];
    float bufallpassL4[allpasstuningL4];
    float bufallpassR4[allpasstuningR4];

    void setroomsize(float value);
    float getroomsize();
    void setdamp(float value);
    float getdamp();
    void setwet(float value);
    float getwet();
    void setdry(float value);
    float getdry();
    void setwidth(float value);
    float getwidth();
    void setmode(float value);
    float getmode();

    void update();

    inline float32x2_t processCombPair(float32x2_t input, combPair &comb);
    inline float32x2_t processAllPassPair(float32x2_t input,
                                          allpassPair &allpass);
  };

} /* namespace od */

#endif /* APP_OBJECTS_FREEVERB_REVERBMODEL_H_ */
