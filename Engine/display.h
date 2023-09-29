#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

#define DISPNUM 0
#define DISPALPHNUM 1
#define DISPSTATE 2

void Display_Init(uint16_t* variable);
void Display_Refrash();
void Display_Blink(const uint8_t enable);
void Display_Mode(const uint8_t mode);

#endif /* DISPLAY_H_ */