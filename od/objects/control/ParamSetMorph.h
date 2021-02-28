#pragma once

#include <od/objects/Object.h>
#include <vector>

namespace od
{

    class ParamSetMorph : public Object
    {
    public:
        ParamSetMorph();
        virtual ~ParamSetMorph();

#ifndef SWIGLUA
        virtual void process();
        void apply();
        Parameter mWeight{"Weight", 0.0f};
#endif

        void add(Parameter *param, float endValue);
        void remove(Parameter *param);
        void clear();
        int size();

        void softSet(float x);
        void hardSet(float x);
        void reset();

    protected:
        struct Item
        {
            Item(Parameter *param, float endValue);
            Item(Item &&other); // move ctor
            virtual ~Item();
            bool operator==(const Parameter *param);
            Item &operator=(Item &&other); // move assignment

            Parameter *param;
            float startValue;
            float endValue;
        };
        std::vector<Item> mItems;
        float mPreviousWeight = 0.0f;
        bool mUpdateNeeded = false;
    };

} /* namespace od */
