#include "stm32l4xx_hal.h"

int main(void)
{
    HAL_Init();

    /* Add the lab part initialization and main loop here. */
    while (1)
    {
    }
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}
