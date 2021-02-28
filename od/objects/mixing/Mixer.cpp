#include <od/objects/mixing/Mixer.h>
#include <od/config.h>
#include <sstream>

namespace od
{

	Mixer::Mixer(int n)
	{
		if (n < 1)
			n = 1;
		// add inlets
		for (int i = 0; i < n; i++)
		{
			std::ostringstream ss;
			ss << "In" << i + 1;
			Inlet *port = new Inlet(ss.str());
			addInputFromHeap(port);
		}
		// add params
		for (int i = 0; i < n; i++)
		{
			std::ostringstream ss;
			ss << "Gain" << i + 1;
			Parameter *param = new Parameter(ss.str());
			addParameterFromHeap(param);
		}
		addOutput(mOutput);
	}

	Mixer::~Mixer()
	{
	}

	void Mixer::process()
	{
		float *out = mOutput.buffer();
		float *in;
		uint32_t i, n = mInputs.size();
		float gain;

		in = mInputs[0]->buffer();
		gain = mParameters[0]->value();
		for (int j = 0; j < FRAMELENGTH; j++)
		{
			out[j] = in[j] * gain;
		}

		for (i = 1; i < n; i++)
		{
			in = mInputs[i]->buffer();
			gain = mParameters[i]->value();
			for (int j = 0; j < FRAMELENGTH; j++)
			{
				out[j] += in[j] * gain;
			}
		}
	}

} /* namespace od */
