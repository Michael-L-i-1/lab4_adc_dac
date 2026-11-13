#ifndef LAB4_THEREMIN_H
#define LAB4_THEREMIN_H

#include <stdint.h>

// ADC readings with the sensor fully covered and fully uncovered.
// read these off the Part 2 terminal and tune them for the room
#define LIGHT_DARK 1400u
#define LIGHT_BRIGHT 3150u

// range for the smooth (non-snapped) theremin
#define FREQ_LOW 220u
#define FREQ_HIGH 880u

void changeNote(uint32_t frequency);
void run_part4_scale(void);
void run_part4(void);

#endif
