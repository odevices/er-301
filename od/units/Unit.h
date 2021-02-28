#pragma once

#include <od/objects/Object.h>

namespace od
{

    class UnitChain;
    class CustomUnit;
    class ObjectList;
    class ParamSetMorph;

    // A unit is a DAG of Objects with an enumerated set of inputs and outputs.
    // Their purpose is to make it easy to connect two unrelated DAGs.
    class Unit : public ReferenceCounted
    {
    public:
        Unit(const std::string &name, int channelCount);
        virtual ~Unit();

#ifndef SWIGLUA
        virtual void process();
        void disconnect();
        void connect(Unit *unit);
        static void connect(Unit *u0, Unit *u1);

        ExecutionTimer mExecutionTimer;
        bool mEnabled = true;
        bool mBypass = false;
        std::string mName;

        int getInputCount()
        {
            return mInputs.size();
        }

        int getOutputCount()
        {
            return mOutputs.size();
        }

        std::vector<Inlet *> &getInputs(int i);
#endif

        void addObject(Object *object);
        void removeObject(Object *object);
        bool compile();

        // direct channel i to the given internal port
        // more than one input per channel is possible
        bool addInput(int i, Object *object, const std::string &inletName);
        // only one port per channel
        bool setOutput(int i, Object *object, const std::string &outletName);
        bool setOutput(int i, Object *object, int outletIndex);

        Outlet *getInwardConnection(int i);
        Outlet *getOutput(int i);

        void setBypass(bool value)
        {
            mBypass = value;
        }

        bool getBypass()
        {
            return mBypass;
        }

        void lock()
        {
            mLocked = true;
            mSavedEnabled = mEnabled;
            mEnabled = false;
        }

        void unlock()
        {
            mEnabled = mSavedEnabled;
            mLocked = false;
        }

        void enable()
        {
            if (mLocked)
            {
                mSavedEnabled = true;
            }
            else
            {
                mEnabled = true;
            }
        }

        void disable()
        {
            if (mLocked)
            {
                mSavedEnabled = false;
            }
            else
            {
                mEnabled = false;
            }
        }

        bool isEnabled()
        {
            return mEnabled;
        }

        bool isSource();

        const std::string &name()
        {
            return mName;
        }

        // hold/unhold all objects
        void hold();
        void unhold();

    protected:
        std::vector<Outlet *> mOutputs;
        std::vector<std::vector<Inlet *>> mInputs;

        std::vector<Object *> mObjects;
        std::vector<Object *> mProcessingOrder;

        bool mLocked = false;
        bool mSavedEnabled = false;

        friend UnitChain;
        friend CustomUnit;
        friend ObjectList;
    };

} /* namespace od */
