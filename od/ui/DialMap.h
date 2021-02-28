/*
 * DialMap.h
 *
 *  Created on: Oct 9, 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_DIALMAP_H_
#define APP_UTILS_DIALMAP_H_

#include <od/extras/ReferenceCounted.h>

namespace od
{

#ifndef SWIGLUA
    struct DialPosition
    {
        int mCoarse = 0;
        int mFine = 0;
        int mSuperFine = 0;
    };

    inline bool operator==(const DialPosition &lhs, const DialPosition &rhs)
    {
        return lhs.mCoarse == rhs.mCoarse && lhs.mFine == rhs.mFine && lhs.mSuperFine == rhs.mSuperFine;
    }

    inline bool operator<(const DialPosition &lhs, const DialPosition &rhs)
    {
        if (lhs.mCoarse < rhs.mCoarse)
        {
            return true;
        }
        else if (lhs.mCoarse > rhs.mCoarse)
        {
            return false;
        }
        else
        {
            if (lhs.mFine < rhs.mFine)
            {
                return true;
            }
            else if (lhs.mFine > rhs.mFine)
            {
                return false;
            }
            else
            {
                return lhs.mSuperFine < rhs.mSuperFine;
            }
        }
    }

    inline bool operator>=(const DialPosition &lhs, const DialPosition &rhs)
    {
        return !operator<(lhs, rhs);
    }
#endif

    class DialMap : public ReferenceCounted
    {
    public:
        DialMap();
        DialMap(const DialMap &map) = default;
        virtual ~DialMap();

        virtual float min(bool skipZero = false) = 0;
        virtual float max() = 0;

        void setZero(float value)
        {
            mZeroValue = value;
        }
        void setRounding(float value)
        {
            mRounding = value;
            mInverseRounding = 1.0f / value;
        }
        float getRounding()
        {
            return mRounding;
        }
        void setCoarseRadix(int radix);
        void setFineRadix(int radix);
        void setSuperFineRadix(int radix);
        void setRadix(int coarse, int fine, int superFine);
        void setSuperCoarseMultiplier(int multiplier)
        {
            mSuperCoarseMultiplier = multiplier;
        }

        int coarseRadix()
        {
            return mCoarseRadix;
        }

        int fineRadix()
        {
            return mFineRadix;
        }

        int superFineRadix()
        {
            return mSuperFineRadix;
        }

        int superCoarseMultiplier()
        {
            return mSuperCoarseMultiplier;
        }

#ifndef SWIGLUA
        virtual float getValue(const DialPosition &dp) = 0;
        virtual DialPosition getPosition(float value) = 0;
        DialPosition getZeroPosition()
        {
            return getPosition(mZeroValue);
        }

        bool updateSuperFine(DialPosition &dp, int change);
        bool updateFine(DialPosition &dp, int change);
        bool updateCoarse(DialPosition &dp, int change);
        DialPosition validatePosition(const DialPosition &dp);
#endif

    protected:
        float mRounding = 0.001f;
        float mInverseRounding = 1000.0f;
        float mZeroValue = 0.0f;
        int mCoarseRadix = 10;
        int mFineRadix = 10;
        int mSuperFineRadix = 10;
        int mSuperCoarseMultiplier = 10;
        bool mChanged = false;

        void getRadixWeights(float &coarseWeight, float &fineWeight,
                             float &superFineWeight, float amplitude);

        void markChanged()
        {
            mChanged = true;
        }
    };

} // namespace od
/* namespace od */

#endif /* APP_UTILS_DIALMAP_H_ */
