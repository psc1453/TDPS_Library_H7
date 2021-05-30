//
// Created by 彭世辰 on 2021/5/28.
//

#ifndef CAMERA_H7_DELAY_H
#define CAMERA_H7_DELAY_H

#include "sys.h"
// Modify to target chip
#include "stm32h7xx_hal.h"

// Modify to system clock (MHz)
#define SystemCoreClockMHz 480
#define SystemCoreClock (SystemCoreClockMHz*1000)

void delay_us(uint32_t udelay);

#endif //CAMERA_H7_DELAY_H
