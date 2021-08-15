#include <MinimalObject.h>
#include <od/config.h>
#include <hal/ops.h>

MinimalObject::MinimalObject()
{
  addInput(mInput);
  addOutput(mOutput);
}

MinimalObject::~MinimalObject()
{
}

void MinimalObject::process()
{
  float *in = mInput.buffer();
  float *out = mOutput.buffer();
  for (int i = 0; i < FRAMELENGTH; i++)
  {
    out[i] = in[i];
  }
}
