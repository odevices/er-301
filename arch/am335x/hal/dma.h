#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <hal/dma/edma3_drv.h>

  void DMA_init();
  void DMA_deinit();
  EDMA3_DRV_Handle DMA_getHandle();

#ifdef __cplusplus
}
#endif
