#include "lfo.h"

#include "cuveconv.h"


static tCuveConv *pfreqconv = 0;

static float constB;

static RNG_HandleTypeDef* prng; //хендлер генератора случайных чисел

static uint32_t tickcounter = 0;

static uint32_t syncduration;

static uint8_t syncstart = 0;


const uint16_t MIDI_Sync_Size[MIDISYNCSTEPS] = 
{
  3,            // 1/32
  6,            // 1/26
  12,           // 1/8
  18,           // 1/8D
  24,           // 1/4
  36,           // 1/4D
  48,           // 1/2
  72,           // 3/4
  96,           // 4/4
  144,          // 6/4
  192,          // 2 Bar
  288,          // 3 Bar
  384,          // 4 Bar
  768,          // 8 Bar
  1536,         // 16 Bar
  3072          // 32 Bar
};


void LFO_SyncStart( void )
{
  syncstart = !syncstart;
}


void LFO_SyncTick ( uint16_t duration )
{
  syncduration = (syncduration + duration) >> 1 ;
  //syncduration = duration;
  ++tickcounter;
}


void LFO_CoreInit (uint32_t clockfreq, RNG_HandleTypeDef* rng)
{
  //инициализируем генератор случайных чисел
  prng = rng;
  
  constB = (float)0x100000000 / clockfreq;
  
  //syncFreqMult = 1.0 / clockfreq;
  
  static tCuveConv freqconv(0, MAXCTRLVAL, MINFREQ, MAXFREQ);
  pfreqconv = &freqconv;
}


tLFO::tLFO(float* out, uint8_t* gate, float* mod)
{
  _pout = out;
  _pgate = gate;
  _pmod = mod;
  
  //_duration = ; //длительность
  _accum = 0;
  _snhmem = 0;
  _snhprevaccum = 0;
  _snhtrigged = 0;
  _gate = 0;
  _retrig = 0;
  _sync = 0;
  _syncsize = 5;
  _syncstart = 0;
  _tickcounter = 0;
  _ticks = 0;
  setRate(2048);
  setSlope(2048);
  setForm(0);
}



void tLFO::update(void)
{
  float freq;
  
  if (_sync)
  {    
    _ticks += tickcounter - _tickcounter;
    _tickcounter = tickcounter;
    if ( (_ticks) >= MIDI_Sync_Size[_syncsize] )
    {
      _ticks = 0;
      _accum = 0;
    }
      
    if (syncstart != _syncstart)
    {
      _syncstart = syncstart;
      _accum = 0;
    }
    
    freq = 1000000.0 / ( syncduration * (MIDI_Sync_Size[_syncsize]>>1) );   
  }
  else
  {   
    freq = _freq + (*_pmod * MAXFREQ);   
  }
  
  
  if ( (_retrig) && (*_pgate != _gate) )
  {
    _gate = *_pgate;
    if (_gate) _accum = 0;
  }
  else
  {
    if (freq > 0)
    {
      _inc = freq * constB;
      _accum += _inc;
    }
  }
       
  *_pout = (this->*_getUpdateFunc)();
}


float tLFO::getTriRamp(void)
{
  if (_accum < _slope)
   return (_multramp * _accum) / 0xFFFFFFFF;
  else
   return (_multsaw * (0xFFFFFFFF - _accum)) / 0xFFFFFFFF;
}


float tLFO::getPulse(void)
{
  return (_accum < _slope);
}


float tLFO::getSnH(void)
{
  if (!_snhtrigged)
  {
    if (_accum < _snhprevaccum)
    {
      _snhmem = (float)((uint16_t)(*prng).Instance->DR) / 0xFFFF;
      _snhtrigged = 1;
    }
  }
  else
  {
    _snhtrigged = 0;
  }
  
  _snhprevaccum = _accum;
  
  return _snhmem;
}


float tLFO::getNoise(void)
{
  return (float)((uint16_t)(*prng).Instance->DR) / 0xFFFF;
}


void tLFO::setForm(uint8_t form)
{
  switch (form) {
  case 0: //пила-треугольнк
    _getUpdateFunc = &tLFO::getTriRamp;
    break;
  case 1: //пульс
    _getUpdateFunc = &tLFO::getPulse;
    break;
  case 2: //sample & hold
    _getUpdateFunc = &tLFO::getSnH;
    break;
  case 3: //шум
    _getUpdateFunc = &tLFO::getNoise;
    break; 
  } 
  //setShape(_slope >> 4);
}


void tLFO::setRate(uint16_t rate)
{
  uint8_t syncsize;
  
  syncsize = (MAXCTRLVAL - rate) / ((MAXCTRLVAL+1) / MIDISYNCSTEPS);
  if (syncsize != _syncsize)
  {
    _syncsize = syncsize;
    if (_sync)
    {
      _ticks = 0;
      _accum = 0;
    }
  }
  
  if (rate)
    _freq = pfreqconv->linToExp(rate);
  else
    _freq = MINFREQ;
}


void tLFO::setSlope(uint16_t slope)
{
  if (slope)
  {
    if (slope != MAXCTRLVAL)
      _slope = slope << 20;
    else
      _slope = 0xFFFFFFFF;
  }
  else
  {
    _slope = 1;
  }
  
  _multramp = 4294967295.0 / _slope;
  _multsaw = 4294967295.0 / (0xFFFFFFFF - _slope);
}


void tLFO::setRetrig(uint8_t retrig)
{
  _retrig = retrig;
}


void tLFO::setMode(uint8_t sync)
{
  _sync = sync;
}