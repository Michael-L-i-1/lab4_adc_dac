#include <stdio.h>
#include "theremin.h"
#include "../part2-adc/adc.h"
#include "../part3-dac/dac.h"

#define NUM_NOTES 11
#define LIGHT_SAMPLES 16

// C major scale, C4 to C5, for checking changeNote by ear
static const uint32_t scale[] = {262, 294, 330, 349, 392, 440, 494, 523};

// C major pentatonic, C4 to C6. any two of these sound fine together
static const uint32_t notes[NUM_NOTES] = {262, 294, 330, 392, 440, 523, 587, 659, 784, 880, 1047};

// same sine table, played faster or slower by changing the TIM6 period
void changeNote(uint32_t frequency)
{
    if (frequency == 0)
        return;

    __HAL_TIM_SET_AUTORELOAD(&htim6, SINE_PERIOD(frequency));
}

// average a few readings so the pitch doesn't jitter
static uint32_t read_light(void)
{
    uint32_t sum = 0;

    for (int i = 0; i < LIGHT_SAMPLES; i++)
        sum += adc_read();

    return sum / LIGHT_SAMPLES;
}

// keep the reading inside the calibrated range
static uint32_t clamp_light(uint32_t light)
{
    if (light < LIGHT_DARK)
        light = LIGHT_DARK;
    if (light > LIGHT_BRIGHT)
        light = LIGHT_BRIGHT;

    return light;
}

// smooth slide between FREQ_LOW and FREQ_HIGH
uint32_t light_to_frequency(uint32_t light)
{
    return FREQ_LOW + (light - LIGHT_DARK) * (FREQ_HIGH - FREQ_LOW) / (LIGHT_BRIGHT - LIGHT_DARK);
}

// snap to the nearest note of the scale (extra credit)
uint32_t light_to_note(uint32_t light)
{
    uint32_t index = (light - LIGHT_DARK) * NUM_NOTES / (LIGHT_BRIGHT - LIGHT_DARK + 1u);

    return notes[index];
}

// play up the scale over and over
void run_part4_scale(void)
{
    sine_start(MIDDLE_C);

    while (1)
    {
        for (int i = 0; i < 8; i++)
        {
            changeNote(scale[i]);
            HAL_Delay(400);
        }
    }
}

// photodiode divider on A0, earphones on D13 through the capacitor
void run_part4(void)
{
    uart_init();
    adc_init();
    sine_start(MIDDLE_C);

    while (1)
    {
        uint32_t raw = read_light();
        uint32_t light = clamp_light(raw);

//        uint32_t frequency = light_to_frequency(light);
        uint32_t frequency = light_to_note(light);

        changeNote(frequency);
        printf("light: %4lu  note: %4lu Hz\r\n", raw, frequency);
        HAL_Delay(20);
    }
}
