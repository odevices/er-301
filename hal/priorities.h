#pragma once

// Task priorities
// higher number has higher priority
// priority 0 is reserved by the idle task
#define TASK_PRIORITY_AUDIO 11
#define TASK_PRIORITY_SPI 13
#define TASK_PRIORITY_ADC 12
#define TASK_PRIORITY_RECORDER 10

// tasks with priority above this have hard deadlines
#define TASK_PRIORITY_REALTIME 7

#define TASK_PRIORITY_USB 6
#define TASK_PRIORITY_PWM 6
#define TASK_PRIORITY_DISPLAY 6

#define TASK_PRIORITY_MAIN 4
#define TASK_PRIORITY_BUSY (TASK_PRIORITY_MAIN)
#define TASK_PRIORITY_APPTHREAD (TASK_PRIORITY_MAIN)
#define TASK_PRIORITY_LOG (TASK_PRIORITY_MAIN+1)

/*  
 * Create clock that calls Task_yield() periodically.
 * Purpose: Prevent tasks from starving other tasks that have the same priority.
 */
void Priorities_enableTaskSlicing(int periodInTicks);

// Hwi (hardware interrupt) priorities
// lower number has higher priority
#define HWI_PRIORITY_GPIO 10
#define HWI_PRIORITY_DISPLAY 9
#define HWI_PRIORITY_AUDIO 6
#define HWI_PRIORITY_SD 8
#define HWI_PRIORITY_USB 8
#define HWI_PRIORITY_I2C 6

// DMA Event Queues
#define DMA_QUEUE_MCASP 0
#define DMA_QUEUE_SPI 1
#define DMA_QUEUE_ADC 0
#define DMA_QUEUE_SD 2
#define DMA_QUEUE_UART 2
