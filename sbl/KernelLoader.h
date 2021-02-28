#pragma once

#include <od/graphics/MainFrameBuffer.h>
#include <od/graphics/SubFrameBuffer.h>
#include <od/ui/JobQueue.h>
#include <stdint.h>

namespace od
{

  class KernelLoader : public Job
  {
  public:
    KernelLoader();

    void init();
    void work();
    void draw(uint8_t *mainFrame, uint8_t *subFrame);

    int progress = 0;
    uint32_t EntryPoint = 0;

  private:
    MainFrameBuffer mainFrameBuffer;
    SubFrameBuffer subFrameBuffer;

    // Progress in Bubbles
    int X[100];
    int Y[100];
    int R[100] = {0};
    float t = 0.0f;
    int n = 0;
  };

} // namespace od