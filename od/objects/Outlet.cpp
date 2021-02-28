#include <od/objects/Outlet.h>
#include <od/objects/Inlet.h>
#include <od/AudioThread.h>
#include <od/config.h>
#include <algorithm>
#include <string.h>

namespace od
{

  Outlet ZeroOutput{"Zero"};
  Outlet OneOutput{"One"};

  void Outlet::initializeGlobalOutlets()
  {
    float *zeros = ZeroOutput.buffer();
    float *ones = OneOutput.buffer();
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      zeros[i] = 0.0f;
      ones[i] = 1.0f;
    }
    ZeroOutput.mIsConstant = true;
    OneOutput.mIsConstant = true;
  }

  Outlet::Outlet()
  {
  }

  Outlet::Outlet(const std::string &name) : Port(name)
  {
  }

  Outlet::Outlet(const std::string &name, uint32_t index) : Port(name, index)
  {
  }

  Outlet::~Outlet()
  {
    // If this outlet is a member variable of another class
    // then this might get called before all inlets have
    // disconnected.  So disconnect here for good measure.
    // However, we cannot use the disconnect() method because
    // that might cause a double delete since it calls release
    // on this object.  Instead, just clear the inlet's pointer
    // to this outlet.
    for (Inlet *inlet : mOutwardConnections)
    {
      inlet->mInwardConnection = 0;
    }
    mOutwardConnections.clear();

    if (mBuffer)
    {
      AudioThread::releaseFrame(mBuffer);
      mBuffer = 0;
    }
  }

  float testBuffer[1024];

  float *__restrict__ Outlet::buffer()
  {
    if (mIsMuted)
    {
      return ZeroOutput.buffer();
    }
    if (mBuffer == 0)
    {
      mBuffer = AudioThread::getFrame();
      if (mBuffer)
      {
        memset(mBuffer, 0, FRAMELENGTH * sizeof(float));
      }
    }
    return mBuffer;
  }

  void Outlet::addInlet(Inlet *inlet)
  {
    auto i = std::find(mOutwardConnections.begin(), mOutwardConnections.end(),
                       inlet);
    if (i == mOutwardConnections.end())
    {
      mOutwardConnections.push_back(inlet);
    }
  }

  void Outlet::removeInlet(Inlet *inlet)
  {
    auto i = std::find(mOutwardConnections.begin(), mOutwardConnections.end(),
                       inlet);
    if (i != mOutwardConnections.end())
    {
      mOutwardConnections.erase(i);
    }
  }

  // disconnect all inlets
  void Outlet::disconnect()
  {
    std::vector<Inlet *> copy = mOutwardConnections;

    for (Inlet *inlet : copy)
    {
      inlet->disconnect();
    }
  }

  bool Outlet::isConnected()
  {
    return mOutwardConnections.size() > 0;
  }

  bool Outlet::isConstant()
  {
    return mIsConstant;
  }

  void Outlet::mute()
  {
    mIsMuted = true;
  }

  void Outlet::unmute()
  {
    mIsMuted = false;
  }

} /* namespace od */
