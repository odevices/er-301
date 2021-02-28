#include <od/objects/Parameter.h>
#include <od/extras/Conversions.h>
#include <od/extras/Utils.h>

namespace od
{

    Parameter::Parameter(const std::string &name, float initialValue) : mName(name), mTarget(initialValue), mValue(initialValue)
    {
    }

    Parameter::~Parameter()
    {
        untie();
    }

    void Parameter::tie(Followable &leader)
    {
        untie();
        mpLeader = &leader;
        mpLeader->attach();
    }

    void Parameter::untie()
    {
        if (mpLeader)
        {
            mpLeader->release();
            mpLeader = 0;
        }
    }

    bool Parameter::isTied()
    {
        return mpLeader;
    }

    float Parameter::value()
    {
        if (mpLeader)
        {
            return mpLeader->value();
        }
        else
        {
            return mValue;
        }
    }

    float Parameter::target()
    {
        if (mpLeader)
        {
            return mpLeader->target();
        }
        else
        {
            return mTarget;
        }
    }

    void Parameter::rampTo(float x)
    {
        float diff = x - mValue;
        if (fabs(diff) < 1e-10f)
        {
            mValue = x;
            mCount = 0;
        }
        else
        {
            mCount = 50;
            mStep = diff * 0.02f;
        }
    }

    void Parameter::hardSet(float x)
    {
        if (mpLeader)
        {
            // do nothing
        }
        else
        {
            mTarget = mValue = x;
            mCount = 0;
        }
    }

    void Parameter::softSet(float x)
    {
        if (mpLeader)
        {
            // do nothing
        }
        else
        {
            mTarget = x;
            rampTo(x);
        }
    }

    void Parameter::deserialize(float x)
    {
        if (mDeserializeWithHardSet)
        {
            hardSet(x);
        }
        else
        {
            softSet(x);
        }
    }

    void Parameter::hold()
    {
        mHeld = true;
    }

    void Parameter::unhold()
    {
        mTarget = mValue;
        mCount = 0;
        mHeld = false;
    }

    int Parameter::roundValue()
    {
        if (mpLeader)
        {
            return mpLeader->roundValue();
        }
        else
        {
            return fastRound(mValue);
        }
    }

    int Parameter::roundTarget()
    {
        if (mpLeader)
        {
            return mpLeader->roundTarget();
        }
        else
        {
            return fastRound(mTarget);
        }
    }

    const std::string &Parameter::name()
    {
        return mName;
    }

    void Parameter::setName(const std::string &name)
    {
        mName = name;
    }

    Parameter &Parameter::operator=(Parameter &param)
    {
        hardSet(param.target());
        return *this;
    }

    void Parameter::forcedUpdate()
    {
        if (mpLeader == NULL)
        {
            if (mCount > 0)
            {
                mValue += mStep;
                mCount--;
            }
            else
            {
                mValue = mTarget;
            }
        }
    }

    void Parameter::update()
    {
        if (mpLeader == NULL)
        {
            if (mHeld)
            {
                // do nothing
            }
            else if (mCount > 0)
            {
                mValue += mStep;
                mCount--;
            }
            else
            {
                mValue = mTarget;
            }
        }
    }

    bool Parameter::isSerializationNeeded()
    {
        return mIsSerializationEnabled;
    }

    void Parameter::enableSerialization()
    {
        mIsSerializationEnabled = true;
    }

    void Parameter::disableSerialization()
    {
        mIsSerializationEnabled = false;
    }

    void Parameter::deserializeWithHardSet()
    {
        mDeserializeWithHardSet = true;
    }

    void Parameter::deserializeWithSoftSet()
    {
        mDeserializeWithHardSet = false;
    }

    bool Parameter::offTarget()
    {
        return mCount > 0;
    }

    void Parameter::enableDecibelMorph()
    {
        mEnableDecibelMorph = true;
    }

} /* namespace od */
