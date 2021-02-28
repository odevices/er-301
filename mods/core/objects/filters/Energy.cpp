/*
 * Energy.cpp
 *
 *  Created on: 20 Jun 2016
 *      Author: clarkson
 */

#include <core/objects/filters/Energy.h>

namespace od
{

	Energy::Energy()
	{
		addInput(mInput);
		addOutput(mOutput);
	}

	Energy::~Energy()
	{
		// TODO Auto-generated destructor stub
	}

	void Energy::process()
	{
	}

} /* namespace od */
