/*
 * LinkDisplay.h
 *
 *  Created on: 12 Aug 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_LINKDISPLAY_H_
#define APP_GRAPHICS_LINKDISPLAY_H_

#include <od/graphics/Graphic.h>

namespace od
{

	class LinkDisplay : public Graphic
	{
	public:
		LinkDisplay();
		virtual ~LinkDisplay();
#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif
		void link(int a, int b);
		void unlink(int a, int b);

		void select(int x)
		{
			mSelection = x;
		}

	private:
		uint32_t mLinks = 0;
		int mSelection = 0;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_LINKDISPLAY_H_ */
