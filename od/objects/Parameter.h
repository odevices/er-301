#pragma once

#include <od/objects/Followable.h>
#include <od/objects/Settable.h>
#include <vector>
#include <string>
#include <limits>

namespace od
{

    class ParamSetMorph;
    class Parameter : public Followable, public Settable
    {
    public:
        Parameter(const std::string &name, float initialValue = 0.0f);
        virtual ~Parameter();

        // WARNING
        // It is possible to create an infinite recursion
        // by tying Followables together in a loop or cycle.

        void tie(Followable &leader);
        void untie();
        bool isTied();

        virtual float value();
        virtual float target();
        virtual int roundValue();
        virtual int roundTarget();

        virtual void hardSet(float x);
        virtual void softSet(float x);

        void hold();
        void unhold();

        const std::string &name();
        void setName(const std::string &name);

        bool isSerializationNeeded();
        void enableSerialization();
        void disableSerialization();
        void deserializeWithHardSet();
        void deserializeWithSoftSet();
        void deserialize(float x);

        void enableDecibelMorph();

#ifndef SWIGLUA
        Parameter &operator=(Parameter &param);
        void update();
        void forcedUpdate();
        bool offTarget();

        std::string mName;
#endif

    protected:
        friend ParamSetMorph;
        Followable *mpLeader = 0;

        float mTarget;
        float mValue;
        float mStep = 0.0f;

        int mCount = 0;
        bool mHeld = false;

        bool mDeserializeWithHardSet = false;
        bool mIsSerializationEnabled = false;
        bool mEnableDecibelMorph = false;

        void rampTo(float x);
    };

} // namespace od
