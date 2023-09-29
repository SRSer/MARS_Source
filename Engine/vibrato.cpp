#include "vibrato.h"

#include "cuveconv.h"
#include "wtsine.h"


static tCuveConv *pfreqconv = 0;

//static uint32_t vibratoclock;

//static float constA;
static float constB;


void Vibrato_CoreInit (uint32_t clockfreq)
{
  //vibratoclock = clockfreq;
  
  //constA = MAXDURATION * clockfreq;
  constB = (float)0x10000 / clockfreq;
  
  static tCuveConv freqconv(0, MAXCTRLVAL, MINFREQ, MAXFREQ);
  pfreqconv = &freqconv;
}


tVibrato::tVibrato(float* out)
{
  _pout = out;
  
  //_duration = ; //длительность
  //_inc = 0; //пиращение фазы
  _accum = 0;
  setRate(2048);
}


void tVibrato::update(void)
{
  _accum += _inc;
  *_pout = (float)wtSine[_accum >> ACCUMSHIFT] / 0x7FFF;
}


void tVibrato::setRate(uint16_t rate)
{
  if (rate)
    _freq = pfreqconv->linToExp(rate);
  else
    _freq = MINFREQ;
  
  _inc = (uint16_t)( _freq * constB );
}