#include "KernelLoader.h"
#include <od/extras/FileReader.h>
#include <od/extras/Random.h>
#include <hal/rng.h>
#include <hal/log.h>
#include <hal/constants.h>

#include <math.h>
#include <string.h>
#include <stdio.h>

typedef struct _ti_header_
{
  uint32_t image_size;
  uint32_t load_addr;
} ti_header;

#define MAX_READ_SIZE (1024 * 16)

namespace od
{

  KernelLoader::KernelLoader()
  {
  }

  void KernelLoader::init()
  {
    Rng_init();
    od::Random::init();
  }

  void KernelLoader::work()
  {
    FileReader reader;
    ti_header imageHdr;
    uint8_t *pDestAddr;
    int size;

    if (!reader.open("0:/kernel.bin"))
    {
      logError("Kernel not found.");
      return;
    }

    if (reader.readBytes(&imageHdr, sizeof(ti_header)) != sizeof(ti_header))
    {
      logError("Kernel header is corrupted.");
      return;
    }
    else
    {
      size = (int)imageHdr.image_size;
      size -= sizeof(ti_header);
      logInfo("Loading %d KB.", size / 1024);
    }

    pDestAddr = (uint8_t *)imageHdr.load_addr;
    EntryPoint = imageHdr.load_addr;

    int br = 0;
    int chunk_size;

    do
    {
      chunk_size = reader.readBytes(pDestAddr, MAX_READ_SIZE);
      br += chunk_size;
      pDestAddr += chunk_size;
      progress = (int)(100 * br / (float)size);
    } while (chunk_size > 0);

    if (br == size)
    {
      progress = 100;
    }
  }

  void KernelLoader::draw(uint8_t *mainFrame, uint8_t *subFrame)
  {
    mainFrameBuffer.setBuffer((uint16_t *)mainFrame);
    mainFrameBuffer.clear();

    int N = progress;

    if (N == 100)
    {
      int i = Random::generateFloat(0.0f, 1.0f) * (N - 1);
      R[i] = 0;
      i = Random::generateFloat(0.0f, 1.0f) * (N - 1);
      R[i] = 0;
      i = Random::generateFloat(0.0f, 1.0f) * (N - 1);
      R[i] = 0;
      i = Random::generateFloat(0.0f, 1.0f) * (N - 1);
      R[i] = 0;
    }
    else if (N > 0)
    {
      for (int i = n; i < N + 1; i++)
      {
        Y[i] = Random::generateFloat(0.0f, 1.0f) * 64;
        X[i] = ((Random::generateFloat(0.0f, 1.0f) + i) * 0.01f) * 256;
        R[i] = Random::generateFloat(0.0f, 1.0f) * 8 + 2;
      }
    }

    t += GRAPHICS_REFRESH_PERIOD;
    for (int i = 0; i < n; i++)
    {
      if (R[i] > 0)
      {
        int x = X[i];
        int y = Y[i];
        int r = R[i] + 3.0f * sinf(x + t * 3.14f);
        mainFrameBuffer.circle(WHITE, x, y, r);
      }
    }

    n = N;

    subFrameBuffer.setBuffer((uint16_t *)subFrame);
    subFrameBuffer.clear();

    char text[16];
    snprintf(text, sizeof(text), "%3d%%", progress);
    subFrameBuffer.text(WHITE, 16, 32, text, 48, ALIGN_MIDDLE);
  }

} // namespace od