#pragma once

#include <od/extras/ReferenceCounted.h>
#include <od/constants.h>
#include <string>

namespace od
{

    class Option : public ReferenceCounted
    {
    public:
        Option(const std::string &name, int mode);
        Option(const std::string &name);
        virtual ~Option();

        // WARNING
        // It is possible to create an infinite recursion
        // by tying options together in a loop or cycle.

        void tie(Option &option);
        void untie();
        bool isTied();
        void set(int value);
        int value();
        void setFlag(int flag);
        void clearFlag(int flag);
        bool toggleFlag(int flag);
        bool getFlag(int flag);
        const std::string &name();
        bool isSerializationNeeded();
        void enableSerialization();
        void disableSerialization();
        void hold();
        void unhold();

#ifndef SWIGLUA

        void checkWeakTie();

        std::string mName;
        int mValue = CHOICE_UNKNOWN;
#endif

    private:
        Option *mpTiedOption = 0;
        bool mIsSerializationNeeded = false;
    };

} /* namespace od */
