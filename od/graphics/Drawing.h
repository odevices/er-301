#pragma once

#include <od/graphics/Graphic.h>
#include <od/graphics/DrawingInstructions.h>

namespace od
{

	class Drawing : public Graphic
	{
	public:
		Drawing(int left, int bottom, int width, int height);
		virtual ~Drawing();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

		void clear();
		void add(DrawingInstructions *sequence);

	private:
		std::vector<DrawingInstructions *> mSequences;
	};

} /* namespace od */
