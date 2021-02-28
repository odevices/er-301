#include <hal/priorities.h>
#include <hal/log.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Task.h>

typedef struct _Local
{
  Clock_Handle handle;
} Local;

static Local local;

Void clockHandler(UArg arg)
{
  Task_yield();
}

void Priorities_enableTaskSlicing(int periodInTicks)
{
  Clock_Params clockParams;
  Error_Block eb;
  Clock_Params_init(&clockParams);
  clockParams.period = periodInTicks;
  clockParams.startFlag = TRUE; /* start immediately */
  local.handle = Clock_create((Clock_FuncPtr)clockHandler,
                              periodInTicks,
                              &clockParams, &eb);
  if (local.handle == NULL)
  {
    logError("Task Slicing Clock create failed");
  }
}