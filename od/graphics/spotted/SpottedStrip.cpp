/*
 * SpottedStrip.cpp
 *
 *  Created on: 26 Jul 2016
 *      Author: clarkson
 */

#include <od/graphics/spotted/SpottedStrip.h>
#include <algorithm>
#include <math.h>

namespace od
{

    static const int C[6] = {
        21,
        63,
        106,
        149,
        192,
        234,
    };

    SpottedStrip::SpottedStrip(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
    {
        mTotalWidth = width;
    }

    SpottedStrip::~SpottedStrip()
    {
    }

    ///// Graphic overloads

    void SpottedStrip::draw(FrameBuffer &fb)
    {
        mOriginState = mOriginState * 0.8f + 0.2f * mTargetOrigin;
        if (fabs(mOriginState - mTargetOrigin) < 2)
        {
            mOriginState = mTargetOrigin;
        }
        mOrigin = mOriginState;
        if (mOrigin != mBuiltOrigin || mContentsChanged)
        {
            mContentsChanged = false;
            rebuild();
        }

        // draw children (i.e. sections)
        Graphic::draw(fb);

        if (mCursorState.active)
        {
            // scroll hints
            if (mBuiltOrigin > 0)
            {
                // can scroll to the left
                int x = mLeft;
                int y = mBottom + mHeight - 8;
                Color color = sTween * WHITE;
                Color color2 = WHITE - color;
                fb.pixel(color, x, y);
                fb.pixel(color, x + 1, y + 1);
                fb.pixel(color, x + 2, y + 2);
                fb.pixel(color, x + 3, y + 3);

                fb.pixel(color, x, y - 1);
                fb.pixel(color, x + 1, y - 2);
                fb.pixel(color, x + 2, y - 3);
                fb.pixel(color, x + 3, y - 4);

                x++;
                fb.pixel(BLACK, x, y);
                fb.pixel(BLACK, x + 1, y + 1);
                fb.pixel(BLACK, x + 2, y + 2);
                fb.pixel(BLACK, x + 3, y + 3);

                fb.pixel(BLACK, x, y - 1);
                fb.pixel(BLACK, x + 1, y - 2);
                fb.pixel(BLACK, x + 2, y - 3);
                fb.pixel(BLACK, x + 3, y - 4);

                x++;
                fb.pixel(color2, x, y);
                fb.pixel(color2, x + 1, y + 1);
                fb.pixel(color2, x + 2, y + 2);
                fb.pixel(color2, x + 3, y + 3);

                fb.pixel(color2, x, y - 1);
                fb.pixel(color2, x + 1, y - 2);
                fb.pixel(color2, x + 2, y - 3);
                fb.pixel(color2, x + 3, y - 4);

                x++;
                fb.pixel(BLACK, x, y);
                fb.pixel(BLACK, x + 1, y + 1);
                fb.pixel(BLACK, x + 2, y + 2);
                fb.pixel(BLACK, x + 3, y + 3);

                fb.pixel(BLACK, x, y - 1);
                fb.pixel(BLACK, x + 1, y - 2);
                fb.pixel(BLACK, x + 2, y - 3);
                fb.pixel(BLACK, x + 3, y - 4);
            }

            if (mBuiltOrigin + mWidth < mTotalWidth - 1)
            {
                // can scroll to the right
                int x = mLeft + mWidth - 3;
                int y = mBottom + mHeight - 8;
                Color color2 = sTween * WHITE;
                Color color = WHITE - color2;
                fb.pixel(color, x, y);
                fb.pixel(color, x - 1, y + 1);
                fb.pixel(color, x - 2, y + 2);
                fb.pixel(color, x - 3, y + 3);

                fb.pixel(color, x, y - 1);
                fb.pixel(color, x - 1, y - 2);
                fb.pixel(color, x - 2, y - 3);
                fb.pixel(color, x - 3, y - 4);

                x++;
                fb.pixel(BLACK, x, y);
                fb.pixel(BLACK, x - 1, y + 1);
                fb.pixel(BLACK, x - 2, y + 2);
                fb.pixel(BLACK, x - 3, y + 3);

                fb.pixel(BLACK, x, y - 1);
                fb.pixel(BLACK, x - 1, y - 2);
                fb.pixel(BLACK, x - 2, y - 3);
                fb.pixel(BLACK, x - 3, y - 4);

                x++;
                fb.pixel(color2, x, y);
                fb.pixel(color2, x - 1, y + 1);
                fb.pixel(color2, x - 2, y + 2);
                fb.pixel(color2, x - 3, y + 3);

                fb.pixel(color2, x, y - 1);
                fb.pixel(color2, x - 1, y - 2);
                fb.pixel(color2, x - 2, y - 3);
                fb.pixel(color2, x - 3, y - 4);

                x++;
                fb.pixel(BLACK, x, y);
                fb.pixel(BLACK, x - 1, y + 1);
                fb.pixel(BLACK, x - 2, y + 2);
                fb.pixel(BLACK, x - 3, y + 3);

                fb.pixel(BLACK, x, y - 1);
                fb.pixel(BLACK, x - 1, y - 2);
                fb.pixel(BLACK, x - 2, y - 3);
                fb.pixel(BLACK, x - 3, y - 4);
            }
        }

        // set the cursor
        mCursorState.x = mWorldSpot;
        mCursorState.y = mWorldBottom + mHeight - 1;
    }

    /////////////////////////////////////////////////

    SpottedSection *SpottedStrip::findSectionByScreenLocation(int target)
    {
        bool takeNext = false;
        for (SpottedSection *section : mSections)
        {
            if (takeNext || (target >= section->mWorldLeft && target < section->mWorldLeft + section->mWidth))
            {
                if (section->mSpots.size() == 0)
                {
                    // skip ahead to the next section with a spot
                    takeNext = true;
                }
                else
                {
                    return section;
                }
            }
        }
        return 0;
    }

    // make sure the current spot is a valid spot for the current view
    void SpottedStrip::clampSpot()
    {
        SpottedSection *section = mSections[mSelectedLocation.section];
        if (section == 0)
            return;

        if (mSelectedLocation.spot < 0)
        {
            mSelectedLocation.spot = 0;
        }
        else if (mSelectedLocation.spot > section->lastSpot())
        {
            mSelectedLocation.spot = section->lastSpot();
        }
    }

    static int closestDetent(int x)
    {
        int D = abs(C[0] - x);
        int c = C[0];
        for (int i = 1; i < 6; i++)
        {
            int d = abs(C[i] - x);
            if (d < D)
            {
                D = d;
                c = C[i];
            }
        }
        return c;
    }

    static int roundUp(int numToRound, int multiple)
    {
        if (multiple == 0)
            return numToRound;

        int remainder = numToRound % multiple;
        if (remainder == 0)
            return numToRound;

        return numToRound + multiple - remainder;
    }

    void SpottedStrip::makeVisible(SpottedSection *section)
    {
        if (section == 0 || mSections.size() == 0)
            return;

        rebuild();

        int center = section->mWidth / 2;
        int radius = center;

        int virtualSpot, virtualCenter, virtualRadius;
        virtualCenter = roundUp(center, SECTION_PLY) - SECTION_PLY / 2;
        virtualRadius = radius + virtualCenter - center;
        virtualSpot = section->mWorldLeft + virtualCenter;

        // make sure the spot is visible
        mTargetOrigin = section->mOffset + virtualCenter - virtualRadius;

        int change = mTargetOrigin - mBuiltOrigin;
        virtualSpot -= change;
        int detent = closestDetent(virtualSpot);
        // apply detents
        mTargetOrigin += virtualSpot - detent;

        if (mTargetOrigin < 0)
        {
            mTargetOrigin = 0;
        }
        else if (mTargetOrigin > mTotalWidth - mWidth)
        {
            mTargetOrigin = mTotalWidth - mWidth;
        }
    }

    void SpottedStrip::leftJustify(SpottedSection *section, int spotId)
    {
        if (section == 0 || mSections.size() == 0 || section->mSpots.size() == 0)
            return;

        if (spotId < 0)
            spotId = 0;

        if (spotId >= (int)section->mSpots.size())
        {
            spotId = section->mSpots.size() - 1;
        }

        const SpottedSection::Spot &spot = section->mSpots[spotId];

        int virtualSpot, virtualCenter, virtualRadius;
        virtualCenter = roundUp(spot.center, SECTION_PLY) - SECTION_PLY / 2;
        virtualRadius = spot.radius + virtualCenter - spot.center;
        virtualSpot = section->mWorldLeft + virtualCenter;

        // put the spot at the left of the display
        mTargetOrigin = section->mOffset + virtualCenter - virtualRadius;

        int change = mTargetOrigin - mBuiltOrigin;
        virtualSpot -= change;
        int detent = closestDetent(virtualSpot);
        // apply detents
        mTargetOrigin += virtualSpot - detent;

        if (mTargetOrigin < 0)
        {
            mTargetOrigin = 0;
        }
        else if (mTargetOrigin > mTotalWidth - mWidth)
        {
            mTargetOrigin = mTotalWidth - mWidth;
        }
    }

    void SpottedStrip::makeSelectionVisible()
    {
        SpottedSection *section = mSections[mSelectedLocation.section];
        if (section->mSpots.size() == 0)
            return;
        const SpottedSection::Spot &spot = section->mSpots[mSelectedLocation.spot];

        rebuild();

        mWorldSpot = section->mWorldLeft + spot.center;

        int virtualSpot, virtualCenter, virtualRadius;
        virtualCenter = roundUp(spot.center, SECTION_PLY) - SECTION_PLY / 2;
        virtualRadius = spot.radius + virtualCenter - spot.center;
        virtualSpot = section->mWorldLeft + virtualCenter;

        if (virtualSpot + virtualRadius > mWorldLeft + mWidth)
        {
            // subtract mWidth so the spot is at the right of the display
            mTargetOrigin = section->mOffset + virtualCenter + virtualRadius - mWidth;
        }
        else if (virtualSpot - virtualRadius < mWorldLeft)
        {
            // put the spot at the left of the display
            mTargetOrigin = section->mOffset + virtualCenter - virtualRadius;
        }

        int change = mTargetOrigin - mBuiltOrigin;
        virtualSpot -= change;
        int detent = closestDetent(virtualSpot);
        // apply detents
        mTargetOrigin += virtualSpot - detent;

        if (mTargetOrigin < 0)
        {
            mTargetOrigin = 0;
        }
        else if (mTargetOrigin > mTotalWidth - mWidth)
        {
            mTargetOrigin = mTotalWidth - mWidth;
        }
    }

    int SpottedStrip::calculateWorldLeft(Location &location)
    {
        if (mSections.size() > 0)
        {
            SpottedSection *section = mSections[location.section];
            if (section->mSpots.size() > 0)
            {
                return section->mWorldLeft + section->mSpots[location.spot].center;
            }
        }
        return 0;
    }

    void SpottedStrip::rebuild()
    {
        int x = 0;

        mMarkCount = 0;
        for (SpottedSection *section : mSections)
        {
            section->mOffset = x;
            section->setPosition(x - mOrigin, section->mBottom);
            x += section->mWidth + 1;
            section->unmark();
        }

        // update the world coordinates of the current selected spot
        mWorldSpot = calculateWorldLeft(mSelectedLocation);

        // updated marked sections
        if (mMarking)
        {
            int markStart = calculateWorldLeft(mMarkStart);
            int markEnd = calculateWorldLeft(mMarkEnd);
            int left = MIN(markStart, markEnd) + 1;
            int right = MAX(markStart, markEnd);
            if (left < right)
            {
                for (SpottedSection *section : mSections)
                {
                    if (section->mWorldLeft > right || section->mWorldLeft + section->mWidth < left)
                    {
                        // not intersecting
                    }
                    else
                    {
                        if (section->mark())
                        {
                            mMarkCount++;
                        }
                    }
                }
            }
        }

        mTotalWidth = MAX(mWidth, x);
        mBuiltOrigin = mOrigin;
    }

    int SpottedStrip::findSectionPosition(SpottedSection *section)
    {
        for (int i = 0; i < (int)mSections.size(); i++)
        {
            if (mSections[i] == section)
                return i;
        }
        return -1;
    }

    SpottedSection *SpottedStrip::getSelectedSection(int shift)
    {
        if (mSections.size() == 0)
            return 0;
        if (mSelectedLocation.section + shift > -1 && mSelectedLocation.section + shift < (int)mSections.size())
            return mSections[mSelectedLocation.section + shift];
        else
            return 0;
    }

    void SpottedStrip::selectLast()
    {
        if (mSections.size() == 0)
            return;

        SpottedSection *section = mSections.back();
        if (section->mSpots.size() > 0)
        {
            int spotId = section->mSpots.size() - 1;
            select(section, spotId);
        }
    }

    void SpottedStrip::select(SpottedSection *section, int spotId)
    {
        if (section == 0 || mSections.size() == 0)
            return;

        if (section->mSpots.size() > 0)
        {
            if (spotId < 0)
            {
                mSelectedLocation.spot = 0;
            }
            else if (spotId >= (int)section->mSpots.size())
            {
                mSelectedLocation.spot = section->mSpots.size() - 1;
            }
            else
            {
                mSelectedLocation.spot = spotId;
            }

            mWorldSpot = section->mWorldLeft + section->mSpots[mSelectedLocation.spot].center;
        }
        else
        {
            if (!searchForwardForSectionWithSpots(false))
            {
                return;
            }
        }

        mSelectedLocation.section = findSectionPosition(section);
        makeSelectionVisible();
    }

    bool SpottedStrip::searchForwardForSectionWithSpots(bool inclusive)
    {
        int i, n = (int)mSections.size();
        bool found = false;

        if (inclusive)
        {
            i = mSelectedLocation.section;
        }
        else
        {
            i = mSelectedLocation.section + 1;
        }

        for (; i < n; i++)
        {
            SpottedSection *section = mSections[i];
            if (section->mSpots.size() > 0)
            {
                mSelectedLocation.spot = 0;
                mSelectedLocation.section = i;
                found = true;
                break;
            }
        }

        return found;
    }

    bool SpottedStrip::searchBackwardForSectionWithSpots(bool inclusive)
    {
        int i;
        bool found = false;

        if (inclusive)
        {
            i = mSelectedLocation.section;
        }
        else
        {
            i = mSelectedLocation.section - 1;
        }

        for (; i >= 0; i--)
        {
            SpottedSection *section = mSections[i];
            if (section->mSpots.size() > 0)
            {
                mSelectedLocation.section = i;
                mSelectedLocation.spot = ((int)section->mSpots.size()) - 1;
                found = true;
                break;
            }
        }

        return found;
    }

    int SpottedStrip::selectNextSpot()
    {
        if (mSections.size() == 0)
        {
            mSelectedLocation.spot = 0;
            return 0;
        }

        SpottedSection *section = mSections[mSelectedLocation.section];

        if (mSelectedLocation.spot + 1 < (int)section->mSpots.size())
        {
            mSelectedLocation.spot++;
        }
        else if (mSelectedLocation.section + 1 < (int)mSections.size())
        {
            if (!searchForwardForSectionWithSpots(false))
            {
                mSelectedLocation.section++;
                mSelectedLocation.spot = 0;
                return 0;
            }
        }

        makeSelectionVisible();
        return mSelectedLocation.spot;
    }

    int SpottedStrip::selectPreviousSpot()
    {
        if (mSections.size() == 0)
        {
            mSelectedLocation.spot = 0;
            return 0;
        }

        if (mSelectedLocation.spot > 0)
        {
            mSelectedLocation.spot--;
        }
        else if (mSelectedLocation.section > 0)
        {
            if (!searchBackwardForSectionWithSpots(false))
            {
                mSelectedLocation.section--;
                mSelectedLocation.spot = 0;
                return 0;
            }
        }

        makeSelectionVisible();
        return mSelectedLocation.spot;
    }

    bool SpottedStrip::encoder(int change, bool shifted, int threshold)
    {
        bool changed = false;
        Location savedLocation = mSelectedLocation;

        mEncoderSum += change;
        if (mEncoderSum > threshold)
        {
            mEncoderSum = 0;
            if (shifted)
            {
                while (selectNextSpot() > 0 && mSelectedLocation.section + 1 < (int)mSections.size())
                {
                    // do nothing
                };
            }
            else
            {
                selectNextSpot();
            }
            changed = true;
        }
        else if (mEncoderSum < -threshold)
        {
            mEncoderSum = 0;
            if (shifted)
            {
                while (selectPreviousSpot() > 0 && mSelectedLocation.section > 0)
                {
                    // do nothing
                };
            }
            else
            {
                selectPreviousSpot();
            }
            changed = true;
        }

        if (changed)
        {
            if (shifted && !mMarking)
            {
                mMarking = true;
                mMarkStart = savedLocation;
                mMarkEnd = mSelectedLocation;
                mContentsChanged = true;
            }
            else if (!shifted && mMarking)
            {
                mMarking = false;
                mMarkStart.zero();
                mMarkEnd.zero();
                mContentsChanged = true;
            }
            else if (mMarking)
            {
                mMarkEnd = mSelectedLocation;
                mContentsChanged = true;
            }
        }

        return changed;
    }

    void SpottedStrip::clearMark()
    {
        mMarking = false;
        mMarkStart.zero();
        mMarkEnd.zero();
        mContentsChanged = true;
    }

    void SpottedStrip::insertSection(SpottedSection *section, int index)
    {
        if (section == 0)
            return;

        if (index < 0)
        {
            index = 0;
        }

        if (index >= (int)mSections.size())
        {
            appendSection(section);
            return;
        }

        mSections.insert(mSections.begin() + index, section);
        addChild(section);
        mContentsChanged = true;
    }

    void SpottedStrip::appendSection(SpottedSection *section)
    {
        if (section == 0)
            return;
        mSections.push_back(section);
        addChild(section);
        mContentsChanged = true;
    }

    SpottedSection *SpottedStrip::getSectionByIndex(int index)
    {
        if (mSections.size() == 0)
            return 0;

        if (index < 0)
        {
            return mSections.at(0);
        }

        if (index >= (int)mSections.size())
        {
            return mSections.back();
        }

        return mSections.at(index);
    }

    void SpottedStrip::clear()
    {
        mSections.clear();
        mOriginState = 0.0f;
        mOrigin = 0;
        mBuiltOrigin = 0;
        mTargetOrigin = 0;
        mWorldSpot = 0;
        mTotalWidth = 0;
        mSelectedLocation.section = 0;
        mSelectedLocation.spot = 0;
        mEncoderSum = 0;
        mContentsChanged = true;
        Graphic::clear();
    }

    void SpottedStrip::removeSection(SpottedSection *section)
    {
        if (section == 0)
            return;
        std::vector<SpottedSection *>::iterator i = std::find(mSections.begin(),
                                                              mSections.end(),
                                                              section);
        if (i != mSections.end())
        {
            // update selection
            int index = (int)(i - mSections.begin());
            if (mSelectedLocation.section > index)
            {
                // selection is after the deleted section
                mSelectedLocation.section--;
            }
            else if (mSelectedLocation.section == index)
            {
                // selected section is being removed
                int n = size();
                mSelectedLocation.spot = 0;
                if (n > 1)
                {
                    mSelectedLocation.section = MIN(mSelectedLocation.section,
                                                    n - 2);
                }
                else
                {
                    mSelectedLocation.section = 0;
                }
            }
            mSections.erase(i);
            removeChild(section);
            mContentsChanged = true;
        }
    }

    void SpottedStrip::moveSection(SpottedSection *section, int index)
    {
        if (section == 0)
            return;
        std::vector<SpottedSection *>::iterator i = std::find(mSections.begin(),
                                                              mSections.end(),
                                                              section),
                                                j;

        if (i != mSections.end() && i != j)
        {
            SpottedSection *section = *i;
            if (i > j)
            {
                mSections.erase(i);
                j = mSections.begin() + index;
            }
            else
            {
                mSections.erase(i);
                j = mSections.begin() + (index - 1);
            }
            mSections.insert(j, section) - mSections.begin();
            mContentsChanged = true;
        }
    }

    int SpottedStrip::getScrollPosition()
    {
        return mTargetOrigin;
    }

    void SpottedStrip::setScrollPosition(int pos, bool smooth)
    {
        mTargetOrigin = pos;
        if (!smooth)
        {
            mOriginState = mTargetOrigin;
        }
    }

} // namespace od
/* namespace od */
