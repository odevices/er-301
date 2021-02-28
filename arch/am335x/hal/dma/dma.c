
#include <hal/dma.h>
#include <hal/log.h>

typedef struct
{
    EDMA3_DRV_Handle hEdma;
} Local;

static Local self;

void DMA_init()
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    self.hEdma = edma3init(0, &result);
    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_setTcErrorInt(0, 0, EDMA3_DRV_TC_ERR_EN);
    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_setTcErrorInt(0, 1, EDMA3_DRV_TC_ERR_EN);
    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_setTcErrorInt(0, 2, EDMA3_DRV_TC_ERR_EN);
    logAssert(result == EDMA3_DRV_SOK);
}

void DMA_deinit()
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    result = edma3deinit(0, self.hEdma);
    logAssert(result == EDMA3_DRV_SOK);
}

EDMA3_DRV_Handle DMA_getHandle()
{
    return self.hEdma;
}