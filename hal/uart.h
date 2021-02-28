#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void Uart_init();
void Uart_write(const char * buffer, int size);
void Uart_puts(const char * text);
void Uart_enable();
void Uart_disable();
bool Uart_isEnabled();

#ifdef __cplusplus
}
#endif

