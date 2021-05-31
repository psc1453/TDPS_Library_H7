//
// Created by 彭世辰 on 2021/5/31.
//

#ifndef TDPS_LIBRARY_H7_HCSR04_H
#define TDPS_LIBRARY_H7_HCSR04_H

//
//
// Requirement: Enable update and capture compare interrupt of timer with input capture direct mode
// Set PSC to make 1MHz CLK and counter period to 999 to make overflow every 1ms and the counter indicates 1us
//
//

// Modify to target chip
#include "stm32h7xx_hal.h"
#include "main.h"
#include "delay.h"

typedef struct
{
    GPIO_TypeDef* HCSR04_TRIG_Port; uint16_t HCSR04_TRIG_Pin;
    GPIO_TypeDef* HCSR04_ECHO_Port; uint16_t HCSR04_ECHO_Pin;
}HCSR04_Connector;

typedef struct
{
    uint16_t time_ms;
    uint16_t time_us;
}HCSR04_Time;

// Declare in main.c and extern in it.c
typedef struct
{
    HCSR04_Connector* connector;
    HCSR04_Time* echo_time;
    TIM_HandleTypeDef* htim;
    uint32_t channel;
    double distance;
}HCSR04;

void HCSR04_Init(HCSR04* hcsr04);
void HCSR04_Trig(HCSR04* hcsr04);
// Put into void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
void HCSR04_Capture_Interrupt_Handler(HCSR04* hcsr04);
// Put into void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
void HCSR04_Period_Interrupt_Handler(HCSR04* hcsr04);

#endif //TDPS_LIBRARY_H7_HCSR04_H
