# EE186 Lab 4: Bit by Bit, Hertz by Hertz

Due: November 12, 2026 (handout).

Board: NUCLEO-L4R5ZI-P, STM32L4R5ZITxP.

## Project setup

Import this folder with **File > Import > General > Existing Projects into Workspace** in STM32CubeIDE. Select `lab4_adc_dac`, build the Debug configuration, and use `lab4_adc_dac Debug` when ready to debug on the board.

This is the starter project scaffold. `Src/main.c` only initializes HAL and idles; ADC, DAC, timer, serial, and theremin behavior remain to be implemented and tested on hardware. ADC, DAC, TIM, and UART HAL modules are enabled. The CMSIS/HAL drivers, startup code, linker scripts, and build settings follow the previous lab's STM32L4R5 project.

## Lab parts

- [Part 2: ADC](part2-adc/README.md)
- [Part 3: DAC](part3-dac/README.md)
- [Part 4: Musical Instrument](part4-musical-instrument/README.md)

Place part-specific source in its corresponding folder; those folders are included in both Debug and Release builds. Keep shared support code in `Src/` and headers in `Inc/`. Screenshots and videos go in each part's matching folders. Add answers and evidence as the lab is completed.

The assignment handout is kept in the course folder at `EE186/Lab 4/EE186_Lab4.pdf`. The handout's opening board specification is STM32L4R5; later STM32L5 mentions do not change this project's target.
