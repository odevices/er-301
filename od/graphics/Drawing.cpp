#include <od/graphics/Drawing.h>

namespace od
{

	Drawing::Drawing(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
	{
	}

	Drawing::~Drawing()
	{
		clear();
	}

	void Drawing::clear()
	{
		for (DrawingInstructions *sequence : mSequences)
		{
			sequence->release();
		}
		mSequences.clear();
	}

	void Drawing::add(DrawingInstructions *sequence)
	{
		if (sequence == 0)
			return;
		sequence->attach();
		mSequences.push_back(sequence);
	}

	void Drawing::draw(FrameBuffer &fb)
	{
		Graphic::draw(fb);

		for (DrawingInstructions *sequence : mSequences)
		{
			sequence->draw(fb, mWorldLeft, mWorldBottom);
		}
	}

} /* namespace od */
