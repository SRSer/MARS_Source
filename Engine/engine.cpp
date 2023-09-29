#include "engine.h"

#include "math.h"
//#include "cuveconv.h"
#include "onepole.h"

uint16_t aubuf [AUBUFSIZE];

//volatile uint16_t adcdata[4];  ///!!!!!!!!!!!!!!!!!

float cv[4] = {0};
uint8_t gate[4] = {0};
uint8_t trig[4] = {0};
uint8_t comtrig = 0;
//uint8_t prevtrig = 0;
uint8_t comgate = 0;
uint8_t gateup = 0;
uint32_t newtrig = 0;

float ModWheel = 0;
tOnePole *pmodwhfltr = 0;

tMatrix *pmatrix = 0;

tKBD *pkbd = 0;

tDCO *pdco1 = 0;
tDCO *pdco2 = 0;
tDCO *pdco3 = 0;
tDCO *pdco4 = 0;

tHADSR *peg1 = 0;
tHADSR *peg2 = 0;

tVibrato *pvibrato = 0;

tLFO *plfo = 0;

tSVF12 *psvfA = 0;
tSVF12 *psvfB = 0;

//tBBD *pbbd = 0;

float level[4] = {0.25, 0.25, 0.25, 0.25};
float balanceA = 0.5;
float balanceB = 0.5;
float mixout;

float dcoout[4];

float Aout, Bout;

//float filterAout;
//ADC_HandleTypeDef* hadc;


uint32_t cycles_count=0; //переменная для счета циклов (для статистики)


//инициализация движка
void Engine_Init(uint32_t clockfreq, float tune, RNG_HandleTypeDef* prng)
{
  //I2C_HandleTypeDef* hi2c1;
  //I2C_HandleTypeDef* hi2c3;
  //I2S_HandleTypeDef* hi2s3;
  //DMA_HandleTypeDef* hdma_spi3_tx; 
  //hadc = padc;
  
  uint32_t modfreq = clockfreq / (AUBUFSIZE >> 1); 
  
  //Matrix_CoreInit;
  
  static tOnePole modwhfltr(MODWHFLTRFREQ / modfreq);
  pmodwhfltr = &modwhfltr;
    
  static tMatrix matrix;
  pmatrix = &matrix;
  
  static tKBD kbd(&cv[0], &gate[0], &trig[0], &comgate, &comtrig, &pmatrix->src.Keytrack);
  pkbd = &kbd;
  
  DCO_CoreInit(clockfreq, modfreq, tune, prng); //инициализируем константы осцилляторов  
  static tDCO dco1(&dcoout[0], &cv[0], &gate[0], &trig[0], &pmatrix->dest.VCOSFreq, &pmatrix->dest.VCOSPWM, &dcoout[1], &pmatrix->dest.VCO21FM);
  pdco1 = &dco1;
  static tDCO dco2(&dcoout[1], &cv[1], &gate[1], &trig[1], &pmatrix->dest.VCOSFreq, &pmatrix->dest.VCOSPWM, NULL, NULL);
  pdco2 = &dco2;
  static tDCO dco3(&dcoout[2], &cv[2], &gate[2], &trig[2], &pmatrix->dest.VCOSFreq, &pmatrix->dest.VCOSPWM, &dcoout[3], &pmatrix->dest.VCO43FM);
  pdco3 = &dco3;
  static tDCO dco4(&dcoout[3], &cv[3], &gate[3], &trig[3], &pmatrix->dest.VCOSFreq, &pmatrix->dest.VCOSPWM, NULL, NULL);
  pdco4 = &dco4;
  
  HADSR_CoreInit( modfreq ); //256  
  static tHADSR eg1(&pmatrix->src.EG1, &comgate, &comtrig);
  peg1 = &eg1;
  static tHADSR eg2(&pmatrix->src.EG2, &comgate, &comtrig);
  peg2 = &eg2;
  
  Vibrato_CoreInit( modfreq ); 
  static tVibrato vibrato(&pmatrix->src.Vibrato);
  pvibrato = &vibrato;
  
  LFO_CoreInit(modfreq, prng);  
  static tLFO lfo(&pmatrix->src.LFO, &comgate, &pmatrix->dest.LFORate);
  plfo = &lfo;

  SVF12_CoreInit(clockfreq);  
  static tSVF12 svfA(&mixout, &Aout, &pmatrix->src.EG1, &pmatrix->dest.VCFACutoff);
  psvfA = &svfA;
  static tSVF12 svfB(&mixout, &Bout, &pmatrix->src.EG2, &pmatrix->dest.VCFBCutoff);
  psvfB = &svfB;
  
  //static tBBD bbd(&pmatrix->dest.BDDMod);
  //pbbd = &bbd;
  
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Разрешаем TRACE
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // Разрешаем счетчик тактов
}

//заполнение аудио буффера
void Engine_FillBuf(uint8_t offset)
{
  uint16_t i=0;
  uint16_t begin=0;
  uint16_t end=AUBUFSIZE / 2; 
  
  if (offset) //если пишем во вторую половину буффера
  {
    begin=AUBUFSIZE / 2;
    end=AUBUFSIZE;
  }
  
  pmatrix->src.Wheel = pmodwhfltr->update( ModWheel );
  
  DCO_UpdatePBFltr();
  pdco1->update(); //обновляем параметры осциллятора
  pdco2->update();
  pdco3->update();
  pdco4->update();
 
  peg1->update();
  peg2->update();
  
  pvibrato->update();
  
  //DWT->CYCCNT = 0; //обнуляем счетчик циклов
  
  plfo->update();
  
  //cycles_count = DWT->CYCCNT; //узнаем количество затраченных циклов
  
  psvfA->update();
  psvfB->update();
  
  //pbbd->update();
  
  pmatrix->update();
  
  for(i=begin; i<end; i+=2)
  {
    DWT->CYCCNT = 0; //обнуляем счетчик циклов
    
    pdco1->nextSample();
    pdco2->nextSample();
    pdco3->nextSample();
    pdco4->nextSample();
    
    mixout = dcoout[0]*level[0] + dcoout[1]*level[1] + 
             dcoout[2]*level[2] + dcoout[3]*level[3];
    
    psvfA->nextSample();
    psvfB->nextSample();
    //plpf->nextSample();
    
    aubuf[i] = (uint16_t) ( Aout * balanceA + Bout * balanceB ); //расчитываем новый семпл для левого канала
    aubuf[i+1] = aubuf[i]; //расчитываем новый семпл для правого канала
    //aubuf[i] =   (uint16_t) (Aout * balanceA); //расчитываем новый семпл для левого канала
    //aubuf[i+1] = (uint16_t) (Bout * balanceB); //расчитываем новый семпл для правого канала
    
    cycles_count = DWT->CYCCNT; //узнаем количество затраченных циклов
  };
}


void Engine_SetNote (uint8_t note, uint8_t state)
{  
  pkbd->setNote(note, state);
}

void Engine_SetVelocity (uint8_t value)
{  
  pmatrix->src.Velocity = (float)value / 127;
}

void Engine_SetModWheel (uint8_t value)
{
  ModWheel = (float)value / 127;
}

void Engine_SetAftertouch (uint8_t value)
{
  pmatrix->src.Aftertoch = (float)value / 127;
}

/*void Engine_SetKeyTrack (void)
{
  float max = 0;
  for (uint8_t i = 0; i < 4; i++)
    if (max < cv[i]) max = cv[i];
  pmatrix->src.Keytrack = max / (HNOTE - LNOTE);
}*/


void Engine_SetMixLevel(uint8_t controller, uint16_t value)
{   
  level[controller] = 0.25 * (float)value / MAXCTRLVAL;
}


void Engine_SetMixBalance(uint16_t value)
{
  balanceA = (float)(MAXCTRLVAL - value) / (MAXCTRLVAL);
  balanceB = (float)value / (MAXCTRLVAL);
}


void Engine_LinkVCFAEG(uint8_t source)
{ 
  if (!source)
    psvfA->linkEG( &pmatrix->src.EG1 );
  else
    psvfA->linkEG( &pmatrix->src.EG2 );
}


void Engine_LinkVCFBEG(uint8_t source)
{ 
  if (!source)
    psvfB->linkEG( &pmatrix->src.EG1 );
  else
    psvfB->linkEG( &pmatrix->src.EG2 );
}