#ifndef LAB4_DAC_H
#define LAB4_DAC_H

#include "stm32l4xx_hal.h"

#define SINE_SAMPLES 64         // table entries in one period of the sine
#define TIMER_HZ 4000000u       // TIM6 runs off the default 4 MHz clock
#define MIDDLE_C 262u

// TIM6 reload value that plays the table at this frequency:
// one sample per timer update, so update rate = frequency * SINE_SAMPLES
#define SINE_PERIOD(frequency) (TIMER_HZ / ((frequency) * SINE_SAMPLES) - 1u)

extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim6;

void dac_init(void);
void sine_start(uint32_t frequency);
void run_part3_loopback(void);
void run_part3_sine(void);

#endif
