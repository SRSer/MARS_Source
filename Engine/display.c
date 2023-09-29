#include "display.h"

#define SEGMETN_PORT GPIOE //порт сегментов
#define SEGMENT_INIT_PIN_NUMBER 0 //первая задействованная ножка порта
#define SEGMENT_MASK 0x7F
#define DIGIT_PORT GPIOB //порт разрядов
#define DIGIT_0_PIN GPIO_PIN_7 //норка первого разряда
#define DIGIT_1_PIN GPIO_PIN_8 //ножка второго разряда

#define BLINK_DURAYION 48 //длительность мигания тактов
#define BLINK_TRESHOLD (BLINK_DURAYION >> 1) //порог 50%

#define CLEAR_DIGIT SEGMETN_PORT->ODR &= ~( 0x7F << SEGMENT_INIT_PIN_NUMBER )
#define SET_DIGIT(d) SEGMETN_PORT->ODR |= ( (d) << SEGMENT_INIT_PIN_NUMBER )

#define ALPHCNT 20

// A b C d E F G H I J
// L n o P Q r S t U Y
const uint8_t aplhabet[ALPHCNT] = {0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x30, 0x1E,
                                   0x38, 0x54, 0x5C, 0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x6E};

//графическое отображение символов 0-9
const uint8_t digits[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};


uint16_t* var;
uint8_t digit = 0;
uint8_t blink_enabled = 0;
uint8_t blink_cnt = 0;
uint8_t disp_mode = DISPALPHNUM;



void Display_Reset()
{
  CLEAR_DIGIT;  
  HAL_GPIO_WritePin(DIGIT_PORT, DIGIT_0_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DIGIT_PORT, DIGIT_1_PIN, GPIO_PIN_SET);
  digit = 1;
  Display_Refrash();
}


void Display_Init(uint16_t* variable)
{
  var = variable;
  Display_Reset();
}


void Display_Blink(const uint8_t enable)
{
  if (!blink_enabled) blink_cnt = 0;
  blink_enabled = enable;
  //Display_Reset();
}


void Display_Mode(const uint8_t mode)
{
  disp_mode = mode;
  //Display_Reset();
}


void Display_Num()
{
  uint8_t decades;
  
  if (digit)
  {
    decades = *var / 10;
    if (decades) SET_DIGIT( digits[decades] );
  }
  else
  {
    SET_DIGIT( digits[*var % 10] );
  }
}


void Display_AlphNum()
{
  //static uint8_t decades;
  //decades =  ((uint16_t)*var) / 10;
  if (digit)
  {
    SET_DIGIT( aplhabet[*var / 10] ); //Letter
  }
  else
  {
    SET_DIGIT( digits[*var % 10] ); //Digit
  }
}


void Display_Mode()
{
  if (digit)
  {
    SET_DIGIT( aplhabet[13] ); //o
  }
  else
  {
    if (*var)
    {
      SET_DIGIT( aplhabet[12] ); //n
    }
    else
    {
      SET_DIGIT( aplhabet[6] ); //F
    }
  }
}


void Display_Refrash()
{ 
  CLEAR_DIGIT; 
  HAL_GPIO_TogglePin(DIGIT_PORT, DIGIT_0_PIN);
  HAL_GPIO_TogglePin(DIGIT_PORT, DIGIT_1_PIN);
  digit = !digit;
  
  if (blink_enabled)
  { 
    if (blink_cnt < BLINK_DURAYION) blink_cnt++; else blink_cnt = 0;
    if (blink_cnt < BLINK_TRESHOLD) return; 
  }
  
  switch (disp_mode) {
  case DISPNUM:
    Display_Num();
    break;
  case DISPALPHNUM:
    Display_AlphNum();
    break;
  case DISPSTATE:
    Display_Mode();
    break;
  }
}
