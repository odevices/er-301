/*
 * SpottedSection.h
 *
 *  Created on: 30 Jul 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SPOTTEDSECTION_H_
#define APP_GRAPHICS_SPOTTEDSECTION_H_

#include <od/graphics/Graphic.h>
#include <vector>

namespace od
{

	class SpottedStrip;

	class SpottedSection : public Graphic
	{
	public:
		SpottedSection(int width, int height, SectionStyle style, bool markable);
		~SpottedSection();

		int findSpotIdByScreenLocation(int x);
		void setWidth(int width);
		int addSpot(int center, int radius = SECTION_PLY);
		void clearSpots();
		void addVerticalDivider(int x);
		void setVerticalDividerColor(Color c)
		{
			mVerticalDividerColor = c;
		}
		void clear();
		bool mark();
		void unmark();
		bool marked()
		{
			return mMarked;
		}
		void applyStyle(SectionStyle style);

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

	private:
		friend class SpottedStrip;

		struct Spot
		{
			Spot(int c, int r) : center(c), radius(r)
			{
			}
			int center;
			int radius;
		};

		int lastSpot();

		std::vector<Spot> mSpots;
		std::vector<int> mVerticalDividers;
		Color mVerticalDividerColor = GRAY7;
		SectionStyle mStyle;
		int mOffset = 0;
		bool mMarked = false;
		bool mMarkable = false;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_SPOTTEDSECTION_H_ */
