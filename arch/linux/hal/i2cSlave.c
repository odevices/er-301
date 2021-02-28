#include <hal/i2c.h>
#include <hal/priorities.h>
#include <hal/log.h>

static bool initialized = false;

bool I2c_popMessage(I2cMessage *msg)
{
  return false;
}

void I2c_init()
{
  initialized = true;
}

void I2c_deinit()
{
  initialized = false;
}

bool I2c_openSlave(uint32_t ownAddress)
{
  if (initialized)
  {
    logWarn("I2c_openSlave: not implemented.");
  }
  else
  {
    logError("I2c_openSlave: i2c not initialized.");
  }
  return false;
}

void I2c_closeSlave()
{
  if (initialized)
  {
    logWarn("I2c_closeSlave: not implemented.");
  }
  else
  {
    logError("I2c_closeSlave: i2c not initialized.");
  }
}
