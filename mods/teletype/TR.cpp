#include <teletype/TR.h>
#include <hal/ops.h>

namespace teletype
{

  TR::TR(Dispatcher *pDispatcher) : mpDispatcher(pDispatcher)
  {
    addOutput(mOutput);
    addParameter(mPort);
  }

  TR::~TR()
  {
  }

  void TR::process()
  {
    int port = CLAMP(0, Dispatcher::PortCount - 1, mPort.roundValue());
    float *out = mOutput.buffer();
    mpDispatcher->fillTRFrame(port, out);
  }

} // namespace teletype
