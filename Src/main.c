#include "stm32l4xx_hal.h"
#include "../part2-adc/adc.h"
#include "../part3-dac/dac.h"
#include "../part4-musical-instrument/theremin.h"

int main(void)
{
    HAL_Init();

//    run_part2();
//    run_part3_loopback();
//    run_part3_sine();
//    run_part4_scale();
    run_part4();

    while (1)
    {
    }
}

// HAL_Delay counts these 1 ms ticks
void SysTick_Handler(void)
{
    HAL_IncTick();
}
