/*
 * Inlet.cpp
 *
 *  Created on: 10 Jun 2016
 *      Author: clarkson
 */

#include <od/objects/Inlet.h>

namespace od
{

	Inlet::Inlet() : mInwardConnection(NULL)
	{
	}

	Inlet::Inlet(const std::string &name) : Port(name)
	{
	}

	Inlet::Inlet(const std::string &name, uint32_t index) : Port(name, index)
	{
	}

	Inlet::~Inlet()
	{
		disconnect();
	}

	float *Inlet::buffer()
	{
		if (mInwardConnection)
		{
			return mInwardConnection->buffer();
		}
		else
		{
			return ZeroOutput.buffer();
		}
	}

	void Inlet::connect(Outlet *outlet)
	{
		disconnect();
		outlet->attach();
		outlet->addInlet(this);
		mInwardConnection = outlet;
	}

	void Inlet::disconnect()
	{
		if (mInwardConnection)
		{
			mInwardConnection->removeInlet(this);
			mInwardConnection->release();
			mInwardConnection = 0;
		}
	}

	bool Inlet::isConnected()
	{
		return mInwardConnection != 0;
	}

	bool Inlet::isConstant()
	{
		if (mInwardConnection)
		{
			return mInwardConnection->isConstant();
		}
		else
		{
			return true;
		}
	}

} /* namespace od */
