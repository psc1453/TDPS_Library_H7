//
// Created by 彭世辰 on 2021/5/30.
//

// Modified to target chip
#include "stm32h7xx_hal.h"

// Modified to target debug pins
extern UART_HandleTypeDef hlpuart1;

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, 50); // Modified to target debug pins

    return ch;
}
