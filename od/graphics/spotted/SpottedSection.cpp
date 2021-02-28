/*
 * SpottedSection.cpp
 *
 *  Created on: 30 Jul 2016
 *      Author: clarkson
 */

#include <od/graphics/spotted/SpottedSection.h>
#include <od/graphics/spotted/SpottedStrip.h>
#include <algorithm>

#define CORNER_RADIUS 5

namespace od
{

	SpottedSection::SpottedSection(int width, int height, SectionStyle style,
																 bool markable) : Graphic(0, 0, width, height), mMarkable(markable)
	{
		applyStyle(style);
	}

	SpottedSection::~SpottedSection()
	{
	}

	void SpottedSection::applyStyle(SectionStyle style)
	{
		switch (style)
		{
		case sectionNoBorder:
			setBorder(0);
			setCornerRadius(0, 0, 0, 0);
			break;
		case sectionSimple:
			setBorder(1);
			setCornerRadius(0, 0, 0, 0);
			mBorderColor = WHITE;
			break;
		case sectionBegin:
			setBorder(1);
			setCornerRadius(2 * CORNER_RADIUS, 2 * CORNER_RADIUS, 0, 0);
			mBorderColor = GRAY11;
			break;
		case sectionMiddle:
			setBorder(1);
			setCornerRadius(CORNER_RADIUS, CORNER_RADIUS, CORNER_RADIUS,
											CORNER_RADIUS);
			mBorderColor = GRAY7;
			break;
		case sectionEnd:
			setBorder(1);
			setCornerRadius(0, 0, 2 * CORNER_RADIUS, 2 * CORNER_RADIUS);
			mBorderColor = GRAY11;
			break;
		}
		mStyle = style;
	}

	int SpottedSection::findSpotIdByScreenLocation(int target)
	{
		int distance;
		int closestSpotId = -1;
		int minDistance = 10000;
		int i;
		int n = (int)mSpots.size();

		target -= mWorldLeft;

		for (i = 0; i < n; i++)
		{
			distance = abs(target - mSpots[i].center);
			if (distance < minDistance)
			{
				minDistance = distance;
				closestSpotId = i;
			}
		}

		return closestSpotId;
	}

	bool SpottedSection::mark()
	{
		if (mMarkable && !mMarked)
		{
			mMarked = true;
			setBorder(2);
			setBorderColor(WHITE);
			return true;
		}
		return false;
	}
	void SpottedSection::unmark()
	{
		if (mMarked)
		{
			mMarked = false;
			applyStyle(mStyle);
		}
	}

	void SpottedSection::setWidth(int width)
	{
		setSize(width, mHeight);
		if (mpParent)
		{
			mpParent->notifyContentsChanged();
		}
	}

	int SpottedSection::addSpot(int center, int radius)
	{
		mSpots.emplace_back(center, radius);
		return mSpots.size() - 1;
	}

	void SpottedSection::clearSpots()
	{
		mSpots.clear();
	}

	void SpottedSection::addVerticalDivider(int x)
	{
		mVerticalDividers.push_back(x);
	}

	void SpottedSection::draw(FrameBuffer &fb)
	{
		if (visible(fb))
		{
			if (mVisibility == hiddenState)
			{
				mVisibility = visibleState;
				notifyVisible();
			}
#if 0
		// draw vertical lines at each spot
		for (const Spot & spot : mSpots) {
			fb.line(GRAY1, mWorldLeft + spot.center, mWorldBottom, mWorldLeft + spot.center,
					mWorldBottom + mHeight);
		}
#endif
			// draw dividers
			for (int x : mVerticalDividers)
			{
				fb.vline(mVerticalDividerColor, mWorldLeft + x, mWorldBottom,
								 mWorldBottom + mHeight);
			}

			Graphic::draw(fb);
		}
		else
		{
			if (mVisibility == visibleState)
			{
				mVisibility = hiddenState;
				notifyHidden();
			}
		}
	}

	int SpottedSection::lastSpot()
	{
		if (mSpots.size() > 0)
		{
			return mSpots.size() - 1;
		}
		else
		{
			return 0;
		}
	}

	void SpottedSection::clear()
	{
		mSpots.clear();
		mVerticalDividers.clear();
		Graphic::clear();
		mOffset = 0;
		setSize(0, mHeight);
		if (mpParent)
		{
			mpParent->notifyContentsChanged();
		}
	}

} /* namespace od */
