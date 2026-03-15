#include <stdio.h>

#include "adc.h"
#include "usart.h"
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "time.h"
#include "font.h"
#include "oled.h"
#include "i2c.h"

#define sample_frq 1e3
#define sample_len 128
#define PRC 84

extern uint32_t LUT_test [4];
extern uint8_t key_state [4][4];

//PV for button scan
uint16_t Curr_States = 0;
uint8_t BtnState = 0;
uint32_t last_time = 0;

//PV for ADC
uint16_t adc_buffer[128];
uint8_t show_buffer[128];
uint8_t adc_done = 0;

void KeyScan();

void app_main()
{
    HAL_TIM_Base_Start_IT(&htim1);

    //start ADC
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 128);
    TIM3->ARR = 500;
    HAL_TIM_Base_Start(&htim3);

    OLED_Init();
    OLED_NewFrame();
    OLED_PrintString(0, 0, "oled on", &font13x13, OLED_COLOR_NORMAL);
    OLED_ShowFrame();

    while (1)
    {
        if (BtnState == 1)
        {
            HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
        }

        if (HAL_GetTick() - last_time > 300)
        {
            //printf("%d\n", key_state[3][3]);
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (key_state[i][j])
                    {
                        printf("(%d, %d)\n", i, j);
                    }
                }
            }
            last_time = HAL_GetTick();
        }

        if (adc_done)
        {
            OLED_NewFrame();
            for (int i=0; i<sample_len; i++)
            {
                show_buffer[i] = (uint8_t)(63 - (uint32_t)adc_buffer[i] * 63 / 0xfff);
                OLED_SetPixel(i, show_buffer[i], OLED_COLOR_NORMAL);
            }
            OLED_ShowFrame();

            HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 128);
            adc_done = 0;
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        KeyScan();

        int SumStates = 0;
        BtnState = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
        Curr_States <<= 1;
        Curr_States |= BtnState;
        SumStates = __builtin_popcount(Curr_States);
        if (SumStates >= 14)
        {
            BtnState = 1;
        }
        else if (SumStates <= 2)
        {
            BtnState = 0;
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_done = 1;
    }
}

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart1, ptr, len, HAL_MAX_DELAY);
    return len;
}