/*
 * BinaryIndicator.h
 *
 *  Created on: 26 Oct 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_BINARYINDICATOR_H_
#define APP_GRAPHICS_BINARYINDICATOR_H_

#include <od/graphics/Graphic.h>

namespace od
{

	class BinaryIndicator : public Graphic
	{
	public:
		BinaryIndicator(int left, int bottom, int width, int height);
		virtual ~BinaryIndicator();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

		void on()
		{
			mState = true;
		}

		void off()
		{
			mState = false;
		}

		bool value()
		{
			return mState;
		}

	private:
		bool mState = false;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_BINARYINDICATOR_H_ */
