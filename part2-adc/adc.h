#ifndef LAB4_ADC_H
#define LAB4_ADC_H

#include "stm32l4xx_hal.h"

#define ADC_MAX 4095    // 12-bit ADC and DAC both top out here
#define VREF 3.3f       // VREF+ is tied to the 3.3 V supply on the Nucleo

extern UART_HandleTypeDef hlpuart1;
extern ADC_HandleTypeDef hadc1;

void uart_init(void);
void adc_init(void);
uint32_t adc_read(void);
float counts_to_volts(uint32_t counts);
void run_part2(void);

#endif
