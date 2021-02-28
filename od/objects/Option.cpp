#include <od/objects/Option.h>

namespace od
{

	Option::Option(const std::string &name, int option) : mName(name), mValue(option)
	{
	}

	Option::Option(const std::string &name) : mName(name)
	{
	}

	Option::~Option()
	{
	}

	void Option::hold()
	{
	}

	void Option::unhold()
	{
	}

	void Option::tie(Option &option)
	{
		if (mpTiedOption)
		{
			mpTiedOption->release();
		}
		mpTiedOption = &option;
		mpTiedOption->attach();
	}

	void Option::untie()
	{
		if (mpTiedOption)
		{
			mpTiedOption->release();
		}
		mpTiedOption = 0;
	}

	bool Option::isTied()
	{
		return mpTiedOption != 0;
	}

	void Option::set(int value)
	{
		if (mpTiedOption)
		{
			mpTiedOption->set(value);
		}
		else
		{
			mValue = value;
		}
	}

	int Option::value()
	{
		if (mpTiedOption)
		{
			return mpTiedOption->value();
		}
		else
		{
			return mValue;
		}
	}

	void Option::setFlag(int flag)
	{
		if (mpTiedOption)
		{
			mpTiedOption->setFlag(flag);
		}
		else
		{
			mValue |= 1U << flag;
		}
	}

	void Option::clearFlag(int flag)
	{
		if (mpTiedOption)
		{
			mpTiedOption->clearFlag(flag);
		}
		else
		{
			mValue &= ~(1U << flag);
		}
	}

	bool Option::toggleFlag(int flag)
	{
		if (mpTiedOption)
		{
			return mpTiedOption->toggleFlag(flag);
		}
		else
		{
			mValue ^= 1U << flag;
			return (mValue & (1U << flag)) != 0;
		}
	}

	bool Option::getFlag(int flag)
	{
		if (mpTiedOption)
		{
			return mpTiedOption->getFlag(flag);
		}
		else
		{
			return (mValue & (1U << flag)) != 0;
		}
	}

	const std::string &Option::name()
	{
		return mName;
	}

	bool Option::isSerializationNeeded()
	{
		return mIsSerializationNeeded;
	}

	void Option::enableSerialization()
	{
		mIsSerializationNeeded = true;
	}

	void Option::disableSerialization()
	{
		mIsSerializationNeeded = false;
	}

	void Option::checkWeakTie()
	{
		if (mpTiedOption && mpTiedOption->refCount() == 1)
		{
			untie();
		}
	}

} /* namespace od */
