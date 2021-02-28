#pragma once

#include <string>

namespace od
{

	class CardInfo
	{
	public:
		CardInfo(const std::string &path);
		virtual ~CardInfo();

		bool measureSpeed(int nbytes, int ntrials, int chunkSize);
		float getWriteSpeed()
		{
			return mWriteSpeed;
		}
		float getReadSpeed()
		{
			return mReadSpeed;
		}

		bool isHighCapacity();
		int getVersion();
		int getBusWidth();
		int getTransferSpeed();
		bool supportsCMD23();

	private:
		float mWriteSpeed = 0.0f;
		float mReadSpeed = 0.0f;
		std::string mPath;
	};

} /* namespace od */
