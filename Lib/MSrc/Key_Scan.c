#include "main.h"

#define ROW 4
#define COL 4
#define MAX_VALUE 5

uint8_t key_state[ROW][COL] = {0};
uint8_t key_value[ROW][COL] = {0};

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
}GPIO_Pin_Typedef;

typedef struct
{
    GPIO_Pin_Typedef Row[ROW];
    GPIO_Pin_Typedef Col[COL];
}KeyMatrix_Typedef;

const KeyMatrix_Typedef KeyMatrix = {
    .Row = {
        {ROW1_GPIO_Port, ROW1_Pin},
        {ROW2_GPIO_Port, ROW2_Pin},
        {ROW3_GPIO_Port, ROW3_Pin},
        {ROW4_GPIO_Port, ROW4_Pin}
    },
    .Col = {
        {COL1_GPIO_Port, COL1_Pin},
        {COL2_GPIO_Port, COL2_Pin},
        {COL3_GPIO_Port, COL3_Pin},
        {COL4_GPIO_Port, COL4_Pin}
    }
};

void KeyScan()
{
    for (int i = 0; i < ROW; i++)
    {
        HAL_GPIO_WritePin(KeyMatrix.Row[i].GPIOx, KeyMatrix.Row[i].GPIO_Pin, GPIO_PIN_SET);
    }

    for (int i = 0; i < ROW; i++)
    {
        HAL_GPIO_WritePin(KeyMatrix.Row[i].GPIOx, KeyMatrix.Row[i].GPIO_Pin, GPIO_PIN_RESET);
        for (int j = 0; j < COL; j++)
        {
            if (HAL_GPIO_ReadPin(KeyMatrix.Col[j].GPIOx, KeyMatrix.Col[j].GPIO_Pin) == GPIO_PIN_RESET && key_value[i][j] < MAX_VALUE)
            {
                key_value[i][j] += 1;
            }
            else if (HAL_GPIO_ReadPin(KeyMatrix.Col[j].GPIOx, KeyMatrix.Col[j].GPIO_Pin) == GPIO_PIN_SET && key_value[i][j] > 0)
            {
                key_value[i][j] -= 1;
            }

            if (key_value[i][j] >= MAX_VALUE)
            {
                key_state[i][j] = 1;
            }
            else
            {
                key_state[i][j] = 0;
            }
        }
        HAL_GPIO_WritePin(KeyMatrix.Row[i].GPIOx, KeyMatrix.Row[i].GPIO_Pin, GPIO_PIN_SET);
    }
}