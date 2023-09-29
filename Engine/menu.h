#ifndef MENU_H_
#define MENU_H_

#define BTNPORT GPIOC
#define JUMPBTN GPIO_PIN_15
#define SAVEBTN GPIO_PIN_14
#define MIDIBTN GPIO_PIN_13  
#define READSTATE(btn) ( !( (uint8_t)HAL_GPIO_ReadPin(GPIOC, (btn) ) )) //считать состояние кнопки

#define ENCODERGET      ( TIM5->CNT >> 1 ) //опрос таймера энкодера
#define ENCODERSET(val) TIM5->CNT = ((val) << 1) //установка значения энкодера
#define ENCODERTOP(val) (val) % 2 ? TIM5->ARR = ((val) << 1) + 1 \
                                  : TIM5->ARR = ((val) << 1) - 1 //установка порога переполнения

#include "stdint.h"
#include "stm32f4xx_hal.h"
                                    
extern uint16_t presetnum;                                    

void Menu_Init(void);
void Menu_Update(void);

void Menue_LoadPreset(uint8_t preset);
void Menue_SavePreset(uint8_t preset);

#endif /* MENU_H_ */