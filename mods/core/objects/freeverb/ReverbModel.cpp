/*
 * ReverbModel.cpp
 *
 *  Created on: 12 Dec 2017
 *      Author: clarkson
 */

#include <core/objects/freeverb/ReverbModel.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  ReverbModel::ReverbModel()
  {
    comb[0].setLeftBuffer(bufcombL1, combtuningL1);
    comb[0].setRightBuffer(bufcombR1, combtuningR1);
    comb[1].setLeftBuffer(bufcombL2, combtuningL2);
    comb[1].setRightBuffer(bufcombR2, combtuningR2);
    comb[2].setLeftBuffer(bufcombL3, combtuningL3);
    comb[2].setRightBuffer(bufcombR3, combtuningR3);
    comb[3].setLeftBuffer(bufcombL4, combtuningL4);
    comb[3].setRightBuffer(bufcombR4, combtuningR4);
    comb[4].setLeftBuffer(bufcombL5, combtuningL5);
    comb[4].setRightBuffer(bufcombR5, combtuningR5);
    comb[5].setLeftBuffer(bufcombL6, combtuningL6);
    comb[5].setRightBuffer(bufcombR6, combtuningR6);
    comb[6].setLeftBuffer(bufcombL7, combtuningL7);
    comb[6].setRightBuffer(bufcombR7, combtuningR7);
    comb[7].setLeftBuffer(bufcombL8, combtuningL8);
    comb[7].setRightBuffer(bufcombR8, combtuningR8);
    allpass[0].setLeftBuffer(bufallpassL1, allpasstuningL1);
    allpass[0].setRightBuffer(bufallpassR1, allpasstuningR1);
    allpass[1].setLeftBuffer(bufallpassL2, allpasstuningL2);
    allpass[1].setRightBuffer(bufallpassR2, allpasstuningR2);
    allpass[2].setLeftBuffer(bufallpassL3, allpasstuningL3);
    allpass[2].setRightBuffer(bufallpassR3, allpasstuningR3);
    allpass[3].setLeftBuffer(bufallpassL4, allpasstuningL4);
    allpass[3].setRightBuffer(bufallpassR4, allpasstuningR4);
    set(initialroom, initialdamp, initialwidth);
    mute();
  }

  ReverbModel::~ReverbModel()
  {
  }

  inline float32x2_t ReverbModel::processCombPair(float32x2_t input,
                                                  ReverbModel::combPair &comb)
  {
    float32x2_t output = vld1_dup_f32(comb.bufferL + comb.bufidxL);
    output = vld1_lane_f32(comb.bufferR + comb.bufidxR, output, 1);
    comb.filterstore = vadd_f32(vmul_f32(output, combDamp2), vmul_f32(comb.filterstore, combDamp1));

    float32x2_t bufin = vadd_f32(input, vmul_f32(comb.filterstore, combFeedback));
    vst1_lane_f32(comb.bufferL + comb.bufidxL, bufin, 0);
    vst1_lane_f32(comb.bufferR + comb.bufidxR, bufin, 1);

    if (++comb.bufidxL >= comb.bufsizeL)
      comb.bufidxL = 0;

    if (++comb.bufidxR >= comb.bufsizeR)
      comb.bufidxR = 0;

    return output;
  }

  inline float32x2_t ReverbModel::processAllPassPair(
      float32x2_t input, ReverbModel::allpassPair &allpass)
  {
    float32x2_t bufout = vld1_dup_f32(allpass.bufferL + allpass.bufidxL);
    bufout = vld1_lane_f32(allpass.bufferR + allpass.bufidxR, bufout, 1);

    float32x2_t output = vsub_f32(bufout, input);
    float32x2_t half = vdup_n_f32(0.5f);
    float32x2_t bufin = vadd_f32(input, vmul_f32(bufout, half));

    vst1_lane_f32(allpass.bufferL + allpass.bufidxL, bufin, 0);
    vst1_lane_f32(allpass.bufferR + allpass.bufidxR, bufin, 1);

    if (++allpass.bufidxL >= allpass.bufsizeL)
      allpass.bufidxL = 0;

    if (++allpass.bufidxR >= allpass.bufsizeR)
      allpass.bufidxR = 0;

    return output;
  }

  void ReverbModel::process(float *inputL, float *inputR, float *outputL,
                            float *outputR)
  {
    float32x2_t zero = vdup_n_f32(0);
    float32x2_t mL = vdup_n_f32(mixL);
    float32x2_t mR = vdup_n_f32(mixR);
    float32x2_t G = vdup_n_f32(gain);

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      float32x2_t input = vld1_dup_f32(inputL + i);
      input = vld1_lane_f32(inputR + i, input, 1);
      input = vmul_f32(input, G);

      float32x2_t output = zero;

      for (int j = 0; j < numcombs; j++)
      {
        output = vadd_f32(output, processCombPair(input, comb[j]));
      }

      for (int j = 0; j < numallpasses; j++)
      {
        output = processAllPassPair(output, allpass[j]);
      }

      float32x2_t swapped = vrev64_f32(output);
      output = vadd_f32(vmul_f32(output, mL), vmul_f32(swapped, mR));

      vst1_lane_f32(outputL + i, output, 0);
      vst1_lane_f32(outputR + i, output, 1);
    }
  }

  void ReverbModel::mute()
  {
    for (int i = 0; i < numcombs; i++)
    {
      comb[i].mute();
      comb[i].filterstore = vdup_n_f32(0);
    }

    for (int i = 0; i < numallpasses; i++)
    {
      allpass[i].mute();
    }
  }

  void ReverbModel::update()
  {
    // Recalculate internal values after parameter change

    mixL = (width / 2 + 0.5f);
    mixR = ((1 - width) / 2);

    if (freeze)
    {
      roomsize1 = 1;
      damp1 = 0;
      gain = muted;
    }
    else
    {
      roomsize1 = roomsize;
      damp1 = damp;
      gain = fixedgain;
    }

    combFeedback = vdup_n_f32(roomsize1);
    combDamp1 = vdup_n_f32(damp1);
    combDamp2 = vdup_n_f32(1 - damp1);
  }

  void ReverbModel::set(float _size, float _damp, float _width)
  {
    roomsize = (_size * scaleroom) + offsetroom;
    damp = _damp * scaledamp;
    width = _width;
    update();
  }

} /* namespace od */
