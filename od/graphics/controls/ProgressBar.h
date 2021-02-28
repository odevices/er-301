/*
 * ProgressBar.h
 *
 *  Created on: 13 Oct 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_PROGRESSBAR_H_
#define APP_GRAPHICS_PROGRESSBAR_H_

#include <od/graphics/Graphic.h>
#include <od/objects/Parameter.h>

namespace od
{

	class ProgressBar : public Graphic
	{
	public:
		ProgressBar(int left, int bottom, int width, int height);
		virtual ~ProgressBar();

		void setTarget(Parameter &param);
		void clearTarget();
#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

	private:
		Parameter mParameter{"Progress"};
	};

} /* namespace od */

#endif /* APP_GRAPHICS_PROGRESSBAR_H_ */
