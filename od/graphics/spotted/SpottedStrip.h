/*
 * SpottedStrip.h
 *
 *  Created on: 26 Jul 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SPOTTEDSTRIP_H_
#define APP_GRAPHICS_SPOTTEDSTRIP_H_

#include <od/graphics/Graphic.h>
#include <od/graphics/spotted/SpottedSection.h>
#include <vector>

namespace od
{

	class SpottedStrip : public Graphic
	{
	public:
		SpottedStrip(int left, int bottom, int width, int height);
		virtual ~SpottedStrip();

		void appendSection(SpottedSection *section);
		void insertSection(SpottedSection *section, int index);
		void removeSection(SpottedSection *section);
		void moveSection(SpottedSection *section, int index);

		bool encoder(int change, bool shifted, int threshold);

		void select(SpottedSection *section, int spot);
		void selectLast();
		void leftJustify(SpottedSection *section, int spot);
		void makeVisible(SpottedSection *section);
		void makeSelectionVisible();

		int getScrollPosition();
		void setScrollPosition(int pos, bool smooth = false);

		void clearMark();
		int getMarkCount()
		{
			return mMarkCount;
		}

		SpottedSection *getSectionByIndex(int index);
		SpottedSection *findSectionByScreenLocation(int x);
		SpottedSection *getSelectedSection(int shift = 0);

		int getSelectedSpotIndex()
		{
			return mSelectedLocation.spot;
		}

		int getSelectedSectionIndex()
		{
			return mSelectedLocation.section;
		}

		int size()
		{
			return (int)mSections.size();
		}

		void clear();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
		virtual void notifyContentsChanged()
		{
			mContentsChanged = true;
		}
#endif

	private:
		typedef struct
		{
			int section = 0;
			int spot = 0;

			void zero()
			{
				section = 0;
				spot = 0;
			}
		} Location;

		std::vector<SpottedSection *> mSections;
		float mOriginState = 0.0f;
		int mOrigin = 0;
		int mBuiltOrigin = 0;
		int mTargetOrigin = 0;
		int mWorldSpot = 0;
		int mTotalWidth = 0;
		Location mSelectedLocation;
		int mEncoderSum = 0;
		Location mMarkStart;
		Location mMarkEnd;
		int mMarkCount = 0;
		bool mMarking = false;
		bool mContentsChanged = false;

		int findSectionPosition(SpottedSection *section);
		int selectNextSpot();
		int selectPreviousSpot();
		bool searchForwardForSectionWithSpots(bool inclusive);
		bool searchBackwardForSectionWithSpots(bool inclusive);
		void clampSpot();
		void rebuild();
		int calculateWorldLeft(Location &location);
	};

} /* namespace od */

#endif /* APP_GRAPHICS_SPOTTEDSTRIP_H_ */
