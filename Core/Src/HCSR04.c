//
// Created by 彭世辰 on 2021/5/31.
//

#include "stm32h7xx_hal.h"
#include "HCSR04.h"
#include "stdio.h"

void HCSR04_Init(HCSR04* hcsr04)
{
    HAL_TIM_IC_Start_IT(hcsr04->htim, hcsr04->channel);
}

void HCSR04_Trig(HCSR04* hcsr04)
{
    HAL_GPIO_WritePin(hcsr04->connector->HCSR04_TRIG_Port, hcsr04->connector->HCSR04_TRIG_Pin, GPIO_PIN_SET);
    delay_us(20);
    HAL_GPIO_WritePin(hcsr04->connector->HCSR04_TRIG_Port, hcsr04->connector->HCSR04_TRIG_Pin, GPIO_PIN_RESET);
}

void HCSR04_Capture_Interrupt_Handler(HCSR04* hcsr04)
{
    if(HAL_GPIO_ReadPin(hcsr04->connector->HCSR04_ECHO_Port, hcsr04->connector->HCSR04_ECHO_Pin))
    {
        HAL_TIM_Base_Start_IT(hcsr04->htim);
        printf("cap\r\n");

    }
    else
    {
        HAL_TIM_Base_Stop_IT(hcsr04->htim);
        hcsr04->echo_time->time_us = HAL_TIM_ReadCapturedValue(hcsr04->htim, hcsr04->channel);
        hcsr04->distance = (hcsr04->echo_time->time_ms/1000.0 + hcsr04->echo_time->time_us/1000000.0)*170.0;
        hcsr04->echo_time->time_us = 0;
        hcsr04->echo_time->time_ms = 0;
        __HAL_TIM_SET_COUNTER(hcsr04->htim,0);
    }
}

void HCSR04_Period_Interrupt_Handler(HCSR04* hcsr04)
{
    hcsr04->echo_time->time_ms++;
}
