#include "bbd.h"
#include "stm32f4xx_hal.h"

/*void BBD_CoreInit (uint32_t clockfreq)
{
 
}*/


tBBD::tBBD(float* mod)
{
  _pmod = mod;
  
  TIM1->CCR1 = 2047>>1;
  TIM1->ARR = 2047;
  
  TIM8->CCR1 = 2047>>1;
  TIM8->ARR = 2047;
}


void tBBD::update(void)
{
  int16_t modulation = *_pmod * 1023;
  uint16_t period_l = 1024 + modulation;;
  uint16_t period_r = 2047 - modulation;

  TIM1->CCR1 = period_l >> 1;
  TIM1->ARR = period_l;

  TIM8->CCR1 = period_r >> 1;
  TIM8->ARR = period_r;
}