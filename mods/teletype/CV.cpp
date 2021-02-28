#include <teletype/CV.h>
#include <hal/ops.h>

namespace teletype
{

  CV::CV(Dispatcher *pDispatcher) : mpDispatcher(pDispatcher)
  {
    addOutput(mOutput);
    addParameter(mPort);
  }

  CV::~CV()
  {
  }

  void CV::process()
  {
    int port = CLAMP(0, Dispatcher::PortCount - 1, mPort.roundValue());
    float *out = mOutput.buffer();
    mLastValue = mpDispatcher->fillCVFrame(port, out, mLastValue);
  }

} // namespace teletype
