#pragma once

namespace od
{

  class EncoderHysteresis
  {
  public:
    EncoderHysteresis();
    virtual ~EncoderHysteresis();

    void setThreshold(int threshold);
    void setReset(int reset);
    int onChange(int change);

  private:
    int mSum = 0;
    int mThreshold = 1;
    int mReset = 1;
  };

} /* namespace od */
