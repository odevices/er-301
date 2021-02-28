/*
 * MultiText.h
 *
 *  Created on: 8 May 2016
 *      Author: clarkson
 */

#ifndef ER301_APP_GRAPHICS_MULTITEXT_H_
#define ER301_APP_GRAPHICS_MULTITEXT_H_

#include <od/graphics/Graphic.h>
#include <string>
#include <vector>

namespace od
{

	class MultiText : public Graphic
	{
	public:
		MultiText(int nlines, int left, int bottom, int width, int height);
		virtual ~MultiText();
#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

		void clear();
		void appendLine(const std::string &line);
		const std::string &getLine(int pos);

	private:
		std::vector<std::string> lines;
		std::string empty_line;
		int nlines;
		int ncols;
		int pos;
		int last;
	};

} /* namespace od */

#endif /* ER301_APP_GRAPHICS_MULTITEXT_H_ */
