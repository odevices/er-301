/*
 * State.cpp
 *
 *  Created on: 18 Jan 2018
 *      Author: clarkson
 */

#include <od/objects/StateMachine.h>

namespace od
{

    StateMachine::StateMachine(const std::string &name) : mName(name)
    {
    }

    StateMachine::~StateMachine()
    {
    }

    void StateMachine::setCurrentState(int index)
    {
        mPreviousIndex = mCurrentIndex;
        mCurrentIndex = index;
    }

    void StateMachine::setNextState(int index)
    {
        mNextIndex = index;
    }

    int StateMachine::getCurrentStateIndex()
    {
        return mCurrentIndex;
    }

    int StateMachine::getNextStateIndex()
    {
        return mNextIndex;
    }

    int StateMachine::getPrevStateIndex()
    {
        return mPreviousIndex;
    }

    const std::string &StateMachine::getStateName(int index)
    {
        if (index < 0 || index >= (int)mStateNames.size())
        {
            static const std::string unknown("unknown");
            return unknown;
        }

        return mStateNames[index];
    }

    int StateMachine::getValue(int index)
    {
        if (index < 0 || index >= (int)mValues.size())
        {
            return 0;
        }

        return mValues[index];
    }

    void StateMachine::setStateName(int index, const std::string &name)
    {
        if (index >= 0 && index < (int)mStateNames.size())
        {
            mStateNames[index] = name;
        }
    }

    void StateMachine::setStateValue(int index, int value)
    {
        if (index >= 0 && index < (int)mValues.size())
        {
            mValues[index] = value;
        }
    }

    void StateMachine::add(const std::string &name)
    {
        mStateNames.push_back(name);
        mValues.push_back(0);
    }

    void StateMachine::add(const std::string &name, int value)
    {
        mStateNames.push_back(name);
        mValues.push_back(value);
    }

    void StateMachine::clear()
    {
        mStateNames.clear();
        mValues.clear();
    }

    int StateMachine::getCount()
    {
        return (int)mStateNames.size();
    }

} /* namespace od */
