#include <stdio.h>
#include "dac.h"
#include "../part2-adc/adc.h"

#define DAC_STEP 64u // loopback ramp step, so one sweep is about 64 lines

DAC_HandleTypeDef hdac1;
TIM_HandleTypeDef htim6;
static DMA_HandleTypeDef hdma_dac1;

// one period of a sine, centered at 2048 with amplitude 1800.
// kept off 0 and 4095 because the output buffer can't quite reach the rails
static const uint32_t sine_table[SINE_SAMPLES] = {
    2048, 2224, 2399, 2571, 2737, 2897, 3048, 3190,
    3321, 3439, 3545, 3635, 3711, 3770, 3813, 3839,
    3848, 3839, 3813, 3770, 3711, 3635, 3545, 3439,
    3321, 3190, 3048, 2897, 2737, 2571, 2399, 2224,
    2048, 1872, 1697, 1525, 1359, 1199, 1048,  906,
     775,  657,  551,  461,  385,  326,  283,  257,
     248,  257,  283,  326,  385,  461,  551,  657,
     775,  906, 1048, 1199, 1359, 1525, 1697, 1872,
};

void dac_init(void)
{
    // PA5 = D13 on the Arduino header = DAC1 channel 2 output
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_5;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    __HAL_RCC_DAC1_CLK_ENABLE();
    hdac1.Instance = DAC1;
    HAL_DAC_Init(&hdac1);
}

// trigger decides when a new value shows up on the pin:
// DAC_TRIGGER_NONE = as soon as we write it, DAC_TRIGGER_T6_TRGO = on each TIM6 update
static void dac_config(uint32_t trigger)
{
    DAC_ChannelConfTypeDef channel = {0};
    channel.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    channel.DAC_Trigger = trigger;
    channel.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    channel.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    channel.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    HAL_DAC_ConfigChannel(&hdac1, &channel, DAC_CHANNEL_2);
}

// jumper D13 (DAC out) to A0 (ADC in)
void run_part3_loopback(void)
{
    uint32_t value = 0;
    int rising = 1;

    uart_init();
    adc_init();
    dac_init();
    dac_config(DAC_TRIGGER_NONE);
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

    while (1)
    {
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, value);
        HAL_Delay(10); // let the output settle before reading it back

        float dac = counts_to_volts(value);
        float adc = counts_to_volts(adc_read());

        printf("DAC: %.3f V  ADC: %.3f V  diff: %+.3f V\r\n", dac, adc, adc - dac);

        // ramp up to the max, then back down to 0, and repeat
        if (rising)
        {
            value += DAC_STEP;
            if (value >= ADC_MAX)
            {
                value = ADC_MAX;
                rising = 0;
            }
        }
        else if (value > DAC_STEP)
        {
            value -= DAC_STEP;
        }
        else
        {
            value = 0;
            rising = 1;
        }

        HAL_Delay(50);
    }
}

// TIM6 ticks at frequency * SINE_SAMPLES. Each tick triggers the DAC, and the
// DMA copies the next table entry in, so the sine runs with no code in the loop
void sine_start(uint32_t frequency)
{
    dac_init();
    dac_config(DAC_TRIGGER_T6_TRGO);

    // DMA1 channel 1 feeds the table to the DAC, wrapping around forever
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    hdma_dac1.Instance = DMA1_Channel1;
    hdma_dac1.Init.Request = DMA_REQUEST_DAC1_CH2;
    hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_dac1.Init.Mode = DMA_CIRCULAR;
    hdma_dac1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_dac1);
    __HAL_LINKDMA(&hdac1, DMA_Handle2, hdma_dac1);

    // preload on, so a new period only takes effect at the next update
    __HAL_RCC_TIM6_CLK_ENABLE();
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 0;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = SINE_PERIOD(frequency);
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim6);

    // send the update event out as the trigger the DAC is waiting for
    TIM_MasterConfigTypeDef master = {0};
    master.MasterOutputTrigger = TIM_TRGO_UPDATE;
    master.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim6, &master);

    HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t *)sine_table, SINE_SAMPLES, DAC_ALIGN_12B_R);
    HAL_TIM_Base_Start(&htim6);
}

// D13 -> 10 uF capacitor -> TIP and RING1 of the audio jack, RING2 -> GND
void run_part3_sine(void)
{
    sine_start(MIDDLE_C);
}
