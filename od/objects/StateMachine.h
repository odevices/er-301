/*
 * StateMachine.h
 *
 *  Created on: 18 Jan 2018
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_STATEMACHINE_H_
#define APP_OBJECTS_STATEMACHINE_H_

#include <od/extras/ReferenceCounted.h>
#include <string>
#include <vector>

namespace od
{

    class StateMachine : public ReferenceCounted
    {
    public:
        StateMachine(const std::string &name);
        virtual ~StateMachine();

#ifndef SWIGLUA
        void setCurrentState(int index);
        void setNextState(int index);
        int getCurrentStateIndex();
        int getNextStateIndex();
        int getPrevStateIndex();

        const std::string &getStateName(int index);
        int getValue(int index);
        void setStateName(int i, const std::string &name);
        void setStateValue(int i, int value);
        void add(const std::string &name);
        void add(const std::string &name, int value);
        void clear();
        int getCount();

        std::string mName;
        std::vector<std::string> mStateNames;
        std::vector<int> mValues;
        int mCurrentIndex = 0;
        int mNextIndex = 0;
        int mPreviousIndex = 0;
#endif
    };

} /* namespace od */

#endif /* APP_OBJECTS_STATEMACHINE_H_ */
