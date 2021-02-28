/*
 * ADSR.h
 *
 *  Created on: 9 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECT_ADSR_H_
#define APP_OBJECT_ADSR_H_

#include <od/extras/LookupTable.h>
#include <od/objects/Object.h>

namespace od
{

  class ADSR : public Object
  {
  public:
    ADSR();
    virtual ~ADSR();

#ifndef SWIGLUA
    virtual void process();
    Inlet mGateInput{"Gate"};
    Inlet mAttack{"Attack"};
    Inlet mDecay{"Decay"};
    Inlet mSustain{"Sustain"};
    Inlet mRelease{"Release"};
    Outlet mOutput{"Out"};
#endif

  private:
    float next(float sustain);
    float nextLUT(float sustain);

    int mStage = 0;

    float mPhase = 0.0f;
    float mCapture = 0.0f;
    float mAttackPhaseChange = 0.0f;
    float mDecayPhaseChange = 0.0f;
    float mReleasePhaseChange = 0.0f;

    LookupTable *mAttackStage = NULL;
    LookupTable *mDecayStage = NULL;
    LookupTable *mReleaseStage = NULL;

    float mCurrentValue = 0.0f;
  };

} /* namespace od */

#endif /* APP_AUDIO_ADSR_H_ */
