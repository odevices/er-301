/*
 * SkewedSineEnvelope.h
 *
 *  Created on: 7 Jun 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_ENV_SKEWEDSINEENVELOPE_H_
#define APP_OBJECTS_ENV_SKEWEDSINEENVELOPE_H_

#include <od/objects/Object.h>

namespace od
{

  class SkewedSineEnvelope : public Object
  {
  public:
    SkewedSineEnvelope();
    virtual ~SkewedSineEnvelope();

#ifndef SWIGLUA
    virtual void process();
    Inlet mTrigger{"Trigger"};
    Parameter mSkew{"Skew", 0.0f};
    Parameter mDuration{"Duration", 0.1f};
    Inlet mLevel{"Level"};
    Outlet mOutput{"Out"};
#endif

  protected:
    float mPhases[4] = {0, 0, 0, 0};
    float mLasts[4] = {0, 0, 0, 0};
    float mFades[4] = {0, 0, 0, 0};
    float mPhaseDelta = 0.0f;
    float mFadeDelta = 0.0f;
    int mCount = 0;
    float mK1 = 0.0f;
    float mK2 = 0.0f;
    float mC2 = 0.0f;

    static const int waiting = 0;
    static const int triggered = 1;
    static const int retriggered = 2;
    int mState = waiting;
    bool mGateLevel = false;

    float startEnvelope();
    void startFade();
  };

} /* namespace od */

#endif /* APP_OBJECTS_ENV_SKEWEDSINEENVELOPE_H_ */
