#include <od/objects/control/ParamSetMorph.h>
#include <od/extras/Conversions.h>
#include <algorithm>

namespace od
{

    ParamSetMorph::ParamSetMorph()
    {
        addParameter(mWeight);
    }

    ParamSetMorph::~ParamSetMorph()
    {
    }

    void ParamSetMorph::process()
    {
    }

    void ParamSetMorph::apply()
    {
        float w2 = mWeight.target();
        if (fabs(w2 - mPreviousWeight) > 1e-8f || mUpdateNeeded)
        {
            float w1 = 1.0f - w2;

            for (Item &item : mItems)
            {
                if (item.param->mEnableDecibelMorph)
                {
                    float x = w1 * item.startValue + w2 * item.endValue;
                    item.param->softSet(fromDecibels(x));
                }
                else
                {
                    float x = w1 * item.startValue + w2 * item.endValue;
                    item.param->softSet(x);
                }
            }

            mUpdateNeeded = false;
            mPreviousWeight = w2;
        }
    }

    void ParamSetMorph::reset()
    {
        mWeight.hardSet(0);
        for (Item &item : mItems)
        {
            if (item.param->mEnableDecibelMorph)
            {
                item.startValue = toDecibels(item.param->target());
            }
            else
            {
                item.startValue = item.param->target();
            }
        }
        mUpdateNeeded = true;
    }

    ParamSetMorph::Item::Item(Parameter *p, float _endValue) : param(p)
    {
        if (param != nullptr)
        {
            param->attach();
            if (param->mEnableDecibelMorph)
            {
                startValue = toDecibels(param->target());
                endValue = toDecibels(_endValue);
            }
            else
            {
                startValue = param->target();
                endValue = _endValue;
            }
        }
    }

    ParamSetMorph::Item::Item(ParamSetMorph::Item &&other) : param(other.param), startValue(other.startValue), endValue(
                                                                                                                   other.endValue)
    {
        other.param = nullptr;
    }

    ParamSetMorph::Item::~Item()
    {
        if (param != nullptr)
        {
            param->release();
        }
    }

    bool ParamSetMorph::Item::operator==(const Parameter *x)
    {
        return param == x;
    }

    ParamSetMorph::Item &ParamSetMorph::Item::operator=(
        ParamSetMorph::Item &&other)
    {
        if (this != &other)
        {
            if (param != nullptr)
            {
                param->release();
            }

            param = other.param;
            startValue = other.startValue;
            endValue = other.endValue;
        }

        return *this;
    }

    void ParamSetMorph::add(Parameter *param, float endValue)
    {
        auto i = std::find(mItems.begin(), mItems.end(), param);
        if (i == mItems.end())
        {
            mItems.emplace_back(param, endValue);
            mUpdateNeeded = true;
        }
    }

    void ParamSetMorph::remove(Parameter *param)
    {
        auto i = std::find(mItems.begin(), mItems.end(), param);
        if (i != mItems.end())
        {
            mItems.erase(i);
            mUpdateNeeded = true;
        }
    }

    void ParamSetMorph::clear()
    {
        mItems.clear();
        hardSet(0.0f);
    }

    int ParamSetMorph::size()
    {
        return mItems.size();
    }

    void ParamSetMorph::softSet(float x)
    {
        mWeight.softSet(x);
        mUpdateNeeded = true;
    }

    void ParamSetMorph::hardSet(float x)
    {
        mWeight.hardSet(x);
        mUpdateNeeded = true;
    }

} /* namespace od */
