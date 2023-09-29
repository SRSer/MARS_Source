#include "dco.h"

#include "math.h"
#include "intrinsics.h"

#include "fastmath.h" //функция быстрого приближенного рассчета экспоненты
#include "cuveconv.h"
#include "onepole.h"

#include "wtsawtooth.h" //мульти-таблица пилы
#include "wttriangle.h" //мульти-таблица треугольника
#include "wtsine.h" //таблица синуса


const float wtTopFreq[TABLECNT] = {20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240}; //верхние частоты для волновых таблиц

uint32_t inctable[TABLECNT]; //рассчитанные приращения фазы по таблице частот

float initfreq; //нижняя частота первого регистра

//некоторые предрассчетные данные
float constA;
float constB;

uint32_t dcoclock;
uint32_t modclock;

float eglog;
float coefattack;
float coefdecaya0;
float coefdecayb1;
float sustain = 0.5;
float coefrelease;
uint16_t decaytime;
uint8_t egretrig = 0;

float portalog;
float portab1 = 0;
float portaa0 = 1;
float pitchbendin = 0;
float pitchbendout = 0;
uint8_t pbrange = 2;
uint8_t legato = 0;

tOnePole *ppbfltr = 0;

//uint8_t gateup = 0;

static tCuveConv *pegcuve = 0;
static tCuveConv *pportacuve = 0;

static RNG_HandleTypeDef* prng; //хендлер генератора случайных чисел

float zero = 0;



void DCO_CoreInit(uint32_t clockfreq, uint32_t modfreq, float tune, RNG_HandleTypeDef* rng)
{
  uint8_t i;
  
  dcoclock = clockfreq; 
  modclock = modfreq;
  //рассчитываем константы
  constA = logf(2) / 12 ;
  constB = 0x100000000 / clockfreq; //dcoclock;  
  //рассчитываем таблицу инкрементов
  for (i=0; i<TABLECNT; i++)
    inctable[i] = (uint32_t)(constB * wtTopFreq[i]) * 2; 
  //рассчитываем начальную частоту
  initfreq = tune * TUNEMUL;
  //инициализируем генератор случайных чисел
  prng = rng;
  
  eglog = logf(DECAYTARGETLEVEL);
  portalog = logf(PORTARANGE);
  
  static tOnePole pbfltr(PBFLTRFREQ / modfreq);
  ppbfltr = &pbfltr;
  
  static tCuveConv egcuve(1, MAXCTRLVAL, EGCUVERANGE, 1);
  pegcuve = &egcuve;
  static tCuveConv portacuve(1, MAXCTRLVAL, PORTACUVERANGE, 1);
  pportacuve = &portacuve;
  
  DCO_SetEnvA(0);
  DCO_SetEnvD(2048);
  DCO_SetEnvR(2048);
}



void DCO_UpdatePBFltr(void)
{
  pitchbendout = ppbfltr->update( pitchbendin );
}



tDCO::tDCO(float* out, float* cv, uint8_t* gate, uint8_t* trig, float* freqmod, float* pwm, float* fm, float* fmmod)
{  
  _pcv = cv;
  _pgate = gate;
  _ptrig = trig;
  _pout = out;
  _pfreqmod = freqmod;
  _ppwm = pwm;
  _pfm = fm;
  _pfmmod = fmmod;
  //if (fm!=NULL) _pfm = fm; else _pfm = &zero;
  //if (fmmod!=NULL) _pfmmod = fmmod; else _pfmmod = &zero;
  
  _portacv = 0;
  _detune = 0;
  _registr = 2;
  _semitones = 0;
  _wavemode = 0;
  _shape = 2048;
  _phaseshift = 0;
  _prevgate = 0;
  _prevtrig = 0;
  _retrig = 0; 
  _waveform = 0;
  _pulsewidth = 0x7FFFFFFF; //пульс 50%
  _pwm = 0x7FFFFFFF;
  _fmdepth = 0;
  _fmdepthmod = 0;
  //setWaveMode(0);
  setWaveForm(WTSAW); //форма - пила
  
  _egphase = 0;
  _egstage = 2;
  //_fm = 0;
}


void tDCO::update()
{    
  if (*_ptrig != _prevtrig)
  {
    _prevtrig = *_ptrig;  
    _egstage = 0; //запускаем огибающую  
    if (egretrig) _egphase = 0;
    if (_retrig) _accum = 0; //если включен перезапуск фазы, сбрасываем фазу     
    if (!legato) _portacv = *_pcv; //если режим лигато выключен устанавливаем управляющее напряжение
  }
  
  if (*_pgate != _prevgate) //если было изменено состояние gate
  {
    _prevgate = *_pgate; //запоминаем новое состояние gate
    if (!_prevgate) //если gate нажат
    {
      _egstage = 2; // если гейт отпущен, устанавливаем фазу огибающей в 0
    }
  }
  
  if (_wavemode == 1) //если выбран режим осциллятора пульс
    _pulsewidth = __QADD(_pwm, (int32_t)(*_ppwm * 0x7FFFFFFF)); //сложение с насыщением
  
  if (_prevgate) //если клавиша нажата
  {
    _portacv = *_pcv * portaa0 + _portacv * portab1; //рассчитываем портаменто (ФНЧ первого порядка)
  }
  
  //рассчитываем модуляцию глубины
  if (_pfmmod != NULL)
    _fmdepthmod = (int32_t)(*_pfmmod * (MAXCTRLVAL * FMDEPTHMULT));
  
  _freq = initfreq * fastexp( (_portacv + pitchbendout + _semitones + _detune + *_pfreqmod * FREQMODRANGE) * constA ); //рассчитываем частоту
  _inc = (uint32_t)(constB * _freq) >> _registr; //получаем приращение
  //f=_freq; //для отладчика
  
  //получаем начальный адрес в таблице
  if      (_inc < inctable[0]) _shift = 0;
  else if (_inc < inctable[1]) _shift = 1;// * TABLESTEP;
  else if (_inc < inctable[2]) _shift = 2;// * TABLESTEP;
  else if (_inc < inctable[3]) _shift = 3;// * TABLESTEP;
  else if (_inc < inctable[4]) _shift = 4;// * TABLESTEP;
  else if (_inc < inctable[5]) _shift = 5;// * TABLESTEP;
  else if (_inc < inctable[6]) _shift = 6;// * TABLESTEP;
  else if (_inc < inctable[7]) _shift = 7;// * TABLESTEP;
  else if (_inc < inctable[8]) _shift = 8;// * TABLESTEP;
  else                         _shift = 9;// * TABLESTEP;
}


void tDCO::nextSample(void)
{
  switch (_egstage)
  {
    case 0:
      _egphase += coefattack;
      if (_egphase > 1.0)
      {
        _egphase = 1.0;
        _egstage = 1;
      }   
      break;
    
    case 1:
      //_egphase *= coefdecay;
      //if (_egphase < sustain)
      //  _egphase = sustain;  
      _egphase = sustain * coefdecaya0 + _egphase * coefdecayb1;
      break;
    
    case 2:
      _egphase *= coefrelease;
  }
  
  if (_pfm != NULL)
    _accum += _inc + ((int32_t)(*_pfm) * (_fmdepth + _fmdepthmod));
  else
    _accum += _inc;
  
  *_pout = _egphase * (float)(this->*_getSampleFunc)(); //возвращаем отсчет дискретизации
  //*_pout = (float)(this->*_getSampleFunc)();
}



int16_t tDCO::getSampleUni(void)
{ 
  //_accum += _inc; //инкрементируем фазу
  
  _phase = _accum + _phaseshift;
  _sample = _table[ _shift ][ _phase >> ACCUMSHIFT ]; //читаем значение из таблицы
  return _sample;
}


int16_t tDCO::getSampleSine(void)
{   
  //_accum += _inc;
  
  _phase = _accum + _phaseshift;
  _sample = _table[0][ _phase >> ACCUMSHIFT ]; //у синуса одна таблица для всех октав
  return _sample;
}


int16_t tDCO::getSamplePulse(void)
{  
  //_accum += _inc;
  
  _phase = _accum + _phaseshift;
  
  //получаем пульс произвольной скважности из разности двух пилообразных форм сдвинутых по фазе
  //для предотвращения переполнения используется вычитание с переполнением
  _sample = __QSUB16( _table[ _shift ][ (_phase + _pulsewidth) >> ACCUMSHIFT ],
                      _table[ _shift ][ _phase >> ACCUMSHIFT ]); 
    
  return _sample;
}


int16_t tDCO::getSampleNoise(void)
{   
  //_accum += _inc;
  
   //получаем случайное число и делим на 2 для баланса громкости с остальными формами
  _sample = (int16_t)((*prng).Instance->DR)>>1;
  return _sample;
}


/*
void tDCO::linkCV(float *cv)
{
  _pcv = cv;
}


void tDCO::linkGate(uint8_t* gate)
{
  _pgate = gate;
}
*/



void tDCO::setWaveForm(uint8_t form)
{
  uint8_t i;
  _waveform = form;
  
  switch (_waveform) {
  case WTTRI: //треугольник
    for (i=0; i<TABLECNT; i++)
      _table[i] = &wtTriangle[i][0];
    _getSampleFunc = &tDCO::getSampleUni;
    break;
  case WTSAW: //пила

    for (i=0; i<TABLECNT; i++)
      _table[i] = &wtSawtooth[i][0];
    _getSampleFunc = &tDCO::getSampleUni;
    break;
  case WTPULSE: //пульс
    for (i=0; i<TABLECNT; i++)
      _table[i] = &wtSawtooth[i][0];
    _getSampleFunc = &tDCO::getSamplePulse;
    break;
  case WTNOISE: //шум
    _getSampleFunc = &tDCO::getSampleNoise;
    break; 
  default: //синус по умолчанию
    _waveform = WTSINE;
    _table[0] = &wtSine[0];
    _getSampleFunc = &tDCO::getSampleSine;
    break;
  }
}


void tDCO::setWaveMode(uint8_t mode)
{
  _wavemode = mode;
  setWaveShape(_shape);
}


void tDCO::setDetune(uint16_t detune)
{
  _detune = ( (float)detune - (MAXCTRLVAL>>1) ) / (MAXCTRLVAL>>1);
}


void tDCO::setSemitones(uint8_t semitones)
{
  _registr = 4 - (semitones / 12);
  _semitones = (semitones % 12);
}


void tDCO::setRetrig(uint8_t enable)
{
  _retrig = enable; 
}


void tDCO::setPhase(uint32_t phase)
{
  _phaseshift = phase << 20;
}


void tDCO::setWaveShape(uint16_t shape)
{
  uint8_t form;
  
  _shape = shape;
  
  switch (_wavemode)
  {
    case 0: //аналоговые волноформы
    {
      form = _shape / ((MAXCTRLVAL + 1) / 6); //4 положения на 4 основных формы
      if (form < WTPULSE)
      {
        //if (_waveform != form)
        setWaveForm( form );
      }
      else
      {
        setWaveForm( WTPULSE );
        switch (form)
        {
          case WTPULSE:
            _pulsewidth = 0x7FFFFFFF; //50%
            break;
          case WTPULSE+1:
            _pulsewidth = 0x3FFFFFFF; //25%
            break;
          case WTPULSE+2:
            _pulsewidth = 0x1FFFFFFF; //12,5%
        }
      }
      break;
    }
    
    case 1: //прямоугольник с переменной скважностью
    {
      setWaveForm (3);
      _pwm = _shape << 19;
      break;
    }
    
    case 3: //белый шум
    {
      setWaveForm (4);
      break;
    }
  }
}


void tDCO::setFMDepts(uint16_t depth)
{
  _fmdepth = (uint32_t)depth* FMDEPTHMULT;
}



void DCO_SetEnvA(uint16_t time)
{
  //_mulattack = 1.0 + (logegmax - logegmin) / (time * 48);
  if (time)
    coefattack = 1.0 / ( AMAXTIME * dcoclock * pegcuve->linToExp(time) );
  else
    coefattack = 1.0;
}


void DCO_SetEnvD(uint16_t time)
{
  decaytime = time;
  if (time)
    coefdecayb1 = 1.0 + eglog / ( DMAXTIME * dcoclock * pegcuve->linToExp(time) );
  else
    coefdecayb1 = 0;
  
  coefdecaya0 = 1.0 - coefdecayb1;
}


void DCO_SetEnvS(uint16_t level)
{
  sustain = (float)level / MAXCTRLVAL;
  DCO_SetEnvD(decaytime);
}


void DCO_SetEnvR(uint16_t time)
{
  if (time)
    coefrelease = 1.0 + eglog / ( RMAXTIME * dcoclock * pegcuve->linToExp(time) );
  else
    coefrelease = 0;
}


void DCO_SetEnvRetrig(uint8_t enable)
{
  egretrig = enable;
}


void DCO_SetPortaTime(uint16_t time)
{
  if (time)
    portab1 = 1.0 + portalog / ( PORTAMAXTIME * modclock * pportacuve->linToExp(time) );
  else
    portab1 = 0;
  
  portaa0 = 1.0 - portab1;
}


void DCO_SetLegato(uint8_t enable)
{
  legato = enable;
}


void DCO_SetPitchBend (uint16_t value)
{  
  pitchbendin = ((float)(value - 0x2000) / 0x2000) * pbrange;
}


void DCO_SetPBRange (uint8_t value)
{  
  pbrange = value;
}
