#ifndef ENGINE_H_
#define ENGINE_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

#include "matrix.h"
#include "kbd.h"
#include "dco.h"
#include "hadsr.h"
#include "svf.h"
#include "vibrato.h"
#include "lfo.h"
//#include "lpf.h"
#include "bbd.h"
#include "midi.h"
//#include "PresetManager.h"

//#define LED_PORT GPIOD
//#define LED_GREEN GPIO_PIN_12
//#define LED_ORANGE GPIO_PIN_13
//#define LED_RED GPIO_PIN_14
//#define LED_BLUE GPIO_PIN_15

#define AUBUFSIZE 256 //размер аудио буффера
#define MAXCTRLVAL 4095 //максимальное значение 4095 (12 бит)
//#define EXPCUVERANGE 0.0001 //диапазон экспоненциальной кривой 1/10000
#define MODWHFLTRFREQ 5.0 //частота фильтрации колеса модул€ции

extern uint16_t aubuf [AUBUFSIZE]; //аудио буффер

//extern float cv[4];
extern uint8_t gate[4];
//extern uint8_t comgate;

extern float lavelA[4];
extern float lavelB[4];

extern tMatrix *pmatrix;

extern tKBD *pkbd;

extern tDCO *pdco1;
extern tDCO *pdco2;
extern tDCO *pdco3;
extern tDCO *pdco4;

extern tHADSR *peg1;
extern tHADSR *peg2;

extern tVibrato *pvibrato;

extern tLFO *plfo;

extern tSVF12 *psvfA;
extern tSVF12 *psvfB;



void Engine_Init(uint32_t clockfreq, float tune, RNG_HandleTypeDef* prng); //иництализаци€ аудио движка

void Engine_FillBuf (uint8_t offset); //заполнение буффера данными

//void Engine_SetParam (uint8_t module, uint8_t controller, uint16_t value); //установить значение параметра по SysEx

void Engine_SetNote (uint8_t note, uint8_t state);

void Engine_SetVelocity (uint8_t value);

void Engine_SetAftertouch (uint8_t value);

void Engine_SetModWheel (uint8_t value);


void Engine_SetMixLevel(uint8_t controller, uint16_t value);

void Engine_SetMixBalance(uint16_t value);


void Engine_LinkVCFAEG(uint8_t source);

void Engine_LinkVCFBEG(uint8_t source);


#endif /* ENGINE_H_ */
