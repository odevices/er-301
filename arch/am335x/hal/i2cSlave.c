#include <hal/i2c.h>
#include <hal/priorities.h>
#include <hal/board.h>
#include <hal/log.h>
#include <ti/am335x/csl_i2c.h>
#include <ti/am335x/soc.h>

#include <string.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>

#define I2C_BASE_ADDRESS (SOC_I2C_2_REGS)
#define I2C_INTERRUPT (I2C2_INT)
#define I2C_INPUT_FUNCTIONAL_CLK (48000000U)
#define I2C_MODULE_INTERNAL_CLK_4MHZ (4000000U)
#define I2C_MODULE_INTERNAL_CLK_12MHZ (12000000U)

// Must be a power of 2
#define I2C_QUEUE_SIZE (64)

#define USE_SBLOCK 0
#define USE_BUS_RATE_100KHZ 1
#define USE_BUS_RATE_400KHZ 0

typedef struct
{
  Hwi_Handle hwiHandle;
  bool isInitialized;
  bool isOpen;
  I2cMessage queue[I2C_QUEUE_SIZE];
  I2cMessage workingMessage;
  size_t front, pfront;
  size_t back, cback;
} Local;

static Local self;

//// WeakRB bounded FIFO queue
// Ref: Correct and Efficient Bounded FIFO Queues
// https://www.irif.fr/~guatto/papers/sbac13.pdf
// Ported from C11 atomics to GCC atomics.

static void initQ()
{
  self.front = self.pfront = 0;
  self.back = self.cback = 0;
}

static bool pushQ(I2cMessage *msg)
{
  size_t b;
  b = __atomic_load_n(&self.back, __ATOMIC_RELAXED);
  if (self.pfront + I2C_QUEUE_SIZE - b < 1)
  {
    self.pfront = __atomic_load_n(&self.front, __ATOMIC_ACQUIRE);
    if (self.pfront + I2C_QUEUE_SIZE - b < 1)
    {
      return false;
    }
  }
  self.queue[b % I2C_QUEUE_SIZE] = *msg;
  __atomic_store_n(&self.back, b + 1, __ATOMIC_RELEASE);
  return true;
}

static bool popQ(I2cMessage *msg)
{
  size_t f;
  f = __atomic_load_n(&self.front, __ATOMIC_RELAXED);
  if (self.cback - f < 1)
  {
    self.cback = __atomic_load_n(&self.back, __ATOMIC_ACQUIRE);
    if (self.cback - f < 1)
    {
      return false;
    }
  }
  *msg = self.queue[f % I2C_QUEUE_SIZE];
  __atomic_store_n(&self.front, f + 1, __ATOMIC_RELEASE);
  return true;
}

/////////////////////////////////////

static inline void appendByte(uint8_t x)
{
  if (self.workingMessage.length < I2C_MAX_MSG_SIZE)
  {
    self.workingMessage.data[self.workingMessage.length] = x;
    self.workingMessage.length++;

    if (self.workingMessage.length == 1)
    {
      self.workingMessage.timestamp = ticks();
    }
  }
}

static inline void endMessage()
{
#if 1
  if (self.workingMessage.length > 0)
#else
  if (self.queueCount < I2C_QUEUE_SIZE)
#endif
  {
    pushQ(&self.workingMessage);
  }
  else
  {
    //Breakpoint();
  }

  // Always clear out the working message.
  self.workingMessage.length = 0;
}

bool I2c_popMessage(I2cMessage *msg)
{
  return popQ(msg);
}

static void hwiOnInterrupt(UArg arg)
{
  uint32_t rawStat = I2CSlaveIntRawStatus(I2C_BASE_ADDRESS);

#if USE_SBLOCK
  if ((rawStat & I2C_INT_ADRR_SLAVE) != 0)
  {
    //Breakpoint();
    I2CClockBlockingControl(I2C_BASE_ADDRESS, 0, 0, 0, 0);
  }
#endif

  if ((rawStat & I2C_INT_RECV_READY) != 0U)
  {
    //Breakpoint();
    appendByte(I2CSlaveDataGet(I2C_BASE_ADDRESS));
  }

  //if ((rawStat & I2C_INT_STOP_CONDITION) != 0)
  if ((rawStat & I2C_INT_ADRR_READY_ACESS) != 0)
  {
    endMessage();
#if USE_SBLOCK
    I2CClockBlockingControl(I2C_BASE_ADDRESS, 1, 0, 0, 0);
#endif
  }

  logAssert((rawStat & I2C_INT_RECV_OVER_RUN) == 0);
  I2CSlaveIntClearEx(I2C_BASE_ADDRESS, I2C_INT_ALL);
}

void I2c_init()
{
  if (self.isInitialized)
  {
    return;
  }

  memset(&self, 0, sizeof(self));

  Hwi_Params params;
  Hwi_Params_init(&params);
  params.priority = HWI_PRIORITY_I2C;
  self.hwiHandle = Hwi_create(I2C_INTERRUPT, hwiOnInterrupt, &params, NULL);

  Board_pinmuxI2C2();
  Board_enableI2C2();

  self.isInitialized = true;
}

void I2c_deinit()
{
  if (self.isInitialized)
  {
    if (self.isOpen)
    {
      I2c_closeSlave();
    }

    Hwi_delete(&self.hwiHandle);
    self.hwiHandle = 0;
    Board_pinmuxUART0();

    self.isInitialized = false;
  }
}

bool I2c_openSlave(uint32_t ownAddress)
{
  if (!self.isInitialized)
  {
    return false;
  }

  if (self.isOpen)
  {
    I2c_closeSlave();
  }

  initQ();

  // I2CSoftReset(I2C_BASE_ADDRESS);

  /* Put i2c in reset/disabled state */
  I2CMasterDisable(I2C_BASE_ADDRESS);

  /* Disable Auto Idle functionality */
  I2CAutoIdleDisable(I2C_BASE_ADDRESS);

#if USE_BUS_RATE_100KHZ
  /* Set up clock for 100kHz bit-rate */
  I2CMasterInitExpClk(I2C_BASE_ADDRESS, I2C_INPUT_FUNCTIONAL_CLK,
                      I2C_MODULE_INTERNAL_CLK_4MHZ,
                      100000);
#elif USE_BUS_RATE_400KHZ
  /* Set up clock for 400kHz bit-rate */
  I2CMasterInitExpClk(I2C_BASE_ADDRESS, I2C_INPUT_FUNCTIONAL_CLK,
                      I2C_MODULE_INTERNAL_CLK_12MHZ,
                      400000);
#endif
  /* Clear any pending interrupts */
  I2CMasterIntClearEx(I2C_BASE_ADDRESS, I2C_INT_ALL);

  /* Mask off all interrupts */
  I2CMasterIntDisableEx(I2C_BASE_ADDRESS, I2C_INT_ALL);

  /* In slave mode, set the I2C own address */
  I2COwnAddressSet(I2C_BASE_ADDRESS, ownAddress, I2C_OWN_ADDR_0);

  /* Enable the I2C for operation (not just master) */
  I2CMasterEnable(I2C_BASE_ADDRESS);

  /* Enable free run mode (i.e. do not suspend I2C hardware during debugging) */
  I2CMasterEnableFreeRun(I2C_BASE_ADDRESS);

  /* Clear fifo and configure threshold */
  I2CFIFOClear(I2C_BASE_ADDRESS, I2C_RX_MODE);
  I2CFIFOThresholdConfig(I2C_BASE_ADDRESS, 0, I2C_RX_MODE); // 0 is 1 byte

  ///// Slave-specific configuration

  /* Configure data buffer length to 0 as the actual number of bytes to
     transmit/receive is dependant on external master. */
  I2CSetDataCount(I2C_BASE_ADDRESS, 0U);

  /* Enable interrupts in slave mode */
  I2CSlaveIntEnableEx(
      I2C_BASE_ADDRESS,
      I2C_INT_ADRR_READY_ACESS | I2C_INT_RECV_READY | I2C_INT_ADRR_SLAVE | I2C_INT_RECV_OVER_RUN);

  /* Start the I2C transfer in slave mode with 7-bit address */
  I2CMasterControl(I2C_BASE_ADDRESS,
                   I2C_CFG_MST_ENABLE | I2C_CFG_7BIT_OWN_ADDR_0);

#if USE_SBLOCK
  I2CClockBlockingControl(I2C_BASE_ADDRESS, 1, 0, 0, 0);
#endif

  self.isOpen = true;
  return true;
}

void I2c_closeSlave()
{
  if (self.isOpen)
  {
    self.isOpen = false;

    /* Clear the RX data fifo */
    I2CSlaveDataGet(I2C_BASE_ADDRESS);

    /* Clear all interrupts */
    I2CSlaveIntClearEx(I2C_BASE_ADDRESS, I2C_INT_ALL);

    /* Disable STOP condition interrupt */
    I2CSlaveIntDisableEx(I2C_BASE_ADDRESS, I2C_INT_ALL);

    /* Mask I2C interrupts */
    I2CMasterIntDisableEx(I2C_BASE_ADDRESS, I2C_INT_ALL);

    /* Disable the I2C Master */
    I2CMasterDisable(I2C_BASE_ADDRESS);
  }
}
