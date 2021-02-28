#include <stdint.h>
#include <stdbool.h>
#include <ti/am335x/uart.h>
#include <ti/am335x/soc.h>
#include <ti/sysbios/gates/GateMutex.h>

#define MAX_FIFO_DEPTH 56

typedef struct
{
    GateMutex_Handle mutex;
    bool enabled;
    bool initialized;
    int fifoDepth;
} Local;

static Local self = {
    .mutex = 0,
    .enabled = false,
    .initialized = false,
    .fifoDepth = MAX_FIFO_DEPTH};

void Uart_init(void)
{
#if 1
    uint32_t divisorValue = 0U, fifoConfig = 0U;

    /* Performing a module reset. */
    UARTModuleReset(SOC_UART_0_REGS);

    /* Performing FIFO configurations. */
    /*
    ** - Transmit Trigger Level Granularity is 4
    ** - Receiver Trigger Level Granularity is 1
    ** - Transmit FIFO Space Setting is 56. Hence TX Trigger level
    **   is 8 (64 - 56). The TX FIFO size is 64 bytes.
    ** - The Receiver Trigger Level is 1.
    ** - Clear the Transmit FIFO.
    ** - Clear the Receiver FIFO.
    ** - DMA Mode enabling shall happen through SCR register.
    ** - DMA Mode 0 is enabled. DMA Mode 0 corresponds to No
    **   DMA Mode. Effectively DMA Mode is disabled.
    */
    fifoConfig = UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_1,
                                  UART_TRIG_LVL_GRANULARITY_1,
                                  UART_FCR_TX_TRIG_LVL_56,
                                  1,
                                  1,
                                  1,
                                  UART_DMA_EN_PATH_SCR,
                                  UART_DMA_MODE_0_ENABLE);

    /* Configuring the FIFO settings. */
    UARTFIFOConfig(SOC_UART_0_REGS, fifoConfig);

    /* Performing Baud Rate settings. */
    /* Computing the Divisor Value. */
    divisorValue = UARTDivisorValCompute(48000000U,
                                         115200U,
                                         UART16x_OPER_MODE,
                                         UART_MIR_OVERSAMPLING_RATE_42);
    /* Programming the Divisor Latches. */
    UARTDivisorLatchWrite(SOC_UART_0_REGS, divisorValue);

    /* Switching to Configuration Mode B. */
    UARTRegConfigModeEnable(SOC_UART_0_REGS, UART_REG_CONFIG_MODE_B);

    /* Programming the Line Characteristics. */
    UARTLineCharacConfig(SOC_UART_0_REGS, (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1), UART_PARITY_NONE);

    /* Disabling write access to Divisor Latches. */
    UARTDivisorLatchDisable(SOC_UART_0_REGS);

    /* Disabling Break Control. */
    UARTBreakCtl(SOC_UART_0_REGS, UART_BREAK_COND_DISABLE);

    /* Uart enable */
    UARTOperatingModeSelect(SOC_UART_0_REGS, UART16x_OPER_MODE);
#endif

    self.mutex = GateMutex_create(NULL, NULL);
    self.initialized = true;
    self.fifoDepth = MAX_FIFO_DEPTH;
}

void Uart_write(const char *buffer, int size)
{
    if (!self.enabled)
        return;
    IArg key = GateMutex_enter(self.mutex);
    for (int i = 0; i < size; i++)
    {
        if (buffer[i])
        {
            if (buffer[i] == '\n')
            {
                UARTCharPut(SOC_UART_0_REGS, '\r');
            }
            UARTCharPut(SOC_UART_0_REGS, buffer[i]);
        }
    }
    GateMutex_leave(self.mutex, key);
}

void Uart_puts(const char *buffer)
{
    if (!self.enabled)
        return;
    IArg key = GateMutex_enter(self.mutex);
    while (*buffer)
    {
        if (*buffer == '\n')
        {
            UARTCharPut(SOC_UART_0_REGS, '\r');
        }
        UARTCharPut(SOC_UART_0_REGS, *buffer);
        buffer++;
    }
    GateMutex_leave(self.mutex, key);
}

void Uart_putc(char c)
{
    UARTCharPut(SOC_UART_0_REGS, c);
}

void Uart_enable()
{
    self.enabled = self.initialized;
}

void Uart_disable()
{
    self.enabled = false;
}

bool Uart_isEnabled()
{
    return self.enabled;
}
