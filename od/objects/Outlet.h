#pragma once

#include <od/objects/Port.h>
#include <vector>

namespace od
{

  class Inlet;

  class Outlet : public Port
  {
  public:
#ifndef SWIGLUA
    Outlet();
    Outlet(const std::string &name);
    Outlet(const std::string &name, uint32_t index);
    virtual ~Outlet();

    float *__restrict__ buffer();

    void addInlet(Inlet *inlet);
    void removeInlet(Inlet *inlet);

    // disconnect all inlets
    void disconnect();

    bool isConnected();
    bool isConstant();
    void mute();
    void unmute();

    static void initializeGlobalOutlets();

    std::vector<Inlet *> mOutwardConnections;
    float *mBuffer = 0;
    bool mIsConstant = false;
    bool mIsMuted = false;
#endif
  };

  // Global Outlets to represent constant 0 and 1.
  extern Outlet ZeroOutput;
  extern Outlet OneOutput;

} /* namespace od */
