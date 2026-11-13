#include <stdio.h>
#include "adc.h"

UART_HandleTypeDef hlpuart1;
ADC_HandleTypeDef hadc1;

// same UART setup as Lab 3: prints to the Mac's serial terminal
void uart_init(void)
{
    // PG7 = TX, PG8 = RX: LPUART1, wired on the board to the ST-Link USB port
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2(); // port G needs its own power supply turned on
    __HAL_RCC_GPIOG_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_7 | GPIO_PIN_8;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Alternate = GPIO_AF8_LPUART1;
    HAL_GPIO_Init(GPIOG, &gpio);

    // 115200 baud, 8 data bits, no parity, 1 stop bit
    __HAL_RCC_LPUART1_CLK_ENABLE();
    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 115200;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&hlpuart1);
}

// printf sends each character here, and it goes out over the UART to the Mac
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, 10);
    return ch;
}

void adc_init(void)
{
    // PA3 = A0 on the Arduino header = ADC1 channel 8
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    // the ADC has no clock by default, so run it off the 4 MHz system clock
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_SYSCLK);
    __HAL_RCC_ADC_CLK_ENABLE();

    // 12-bit, one conversion each time we ask for it
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    HAL_ADC_Init(&hadc1);

    // longest sampling time, since the 10k divider charges the ADC slowly
    ADC_ChannelConfTypeDef channel = {0};
    channel.Channel = ADC_CHANNEL_8;
    channel.Rank = ADC_REGULAR_RANK_1;
    channel.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
    channel.SingleDiff = ADC_SINGLE_ENDED;
    channel.OffsetNumber = ADC_OFFSET_NONE;
    HAL_ADC_ConfigChannel(&hadc1, &channel);

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
}

// one conversion, returns 0 to 4095
uint32_t adc_read(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    return HAL_ADC_GetValue(&hadc1);
}

// V = D / 2^n * Vref
float counts_to_volts(uint32_t counts)
{
    return counts * VREF / (ADC_MAX + 1);
}

// potentiometer wiper (or the photodiode divider midpoint) goes to A0
void run_part2(void)
{
    uart_init();
    adc_init();

    while (1)
    {
        uint32_t counts = adc_read();

        printf("ADC: %4lu (%.3f V)\r\n", counts, counts_to_volts(counts));
        HAL_Delay(200);
    }
}
