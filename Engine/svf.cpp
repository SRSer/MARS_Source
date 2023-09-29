#include "svf.h"


#include "math.h"
#include "fastmath.h" //функция быстрого приближенного рассчета экспоненты
#include "cuveconv.h"



uint32_t svfclock;

static tCuveConv *pcoffcuve = 0;


void SVF12_CoreInit (uint32_t clockfreq)
{
  svfclock = clockfreq;
  static tCuveConv coffcuve(0, 1, COFFCUVERANGE, 1);
  pcoffcuve = &coffcuve;
}



tSVF12::tSVF12 (float* in, float* out, float* egcv, float* modcv)
{
  _pin = in;
  _pout = out;
  _peg = egcv;
  _pmod = modcv;
  _fcoff = 0;
  _amount = 0;
  
  _hp = _bp = _lp = _zeroout = 0;
  setCutoff(MAXCTRLVAL);
  setResonance(0);
  setMode(1);
}


void tSVF12::setCutoff (uint16_t cutoff)
{
  _fcoff = (float)cutoff / MAXCTRLVAL;
}


void tSVF12::update(void)
{
  float f;

  f = _fcoff + *_peg * _amount + *_pmod;
  
  if (f > 0.98) f = 0.98;
    else
      if (f < 0) f = 0;
    
  //_f = (2.0 * 3.1415 * MAXCUTFREQ) * _feg * 0.5 / svfclock;
  _f = (2.0 * 3.1415 * MAXCUTFREQ) * pcoffcuve->linToExp(f) / svfclock;
}


void tSVF12::setAmount(uint16_t amount)
{
  _amount =( (float)amount - (MAXCTRLVAL>>1) ) / (MAXCTRLVAL>>1);
  //_amount = (float)amount / MAXCTRLVAL;
}


void tSVF12::setResonance (uint16_t resonance)
{
  _fb = 1.0 - (float)resonance / (MAXCTRLVAL + 200);
  _lim = 0.1 + _fb * 0.4;
}


void tSVF12::setMode (uint8_t mode)
{
  switch (mode)
  {
    case 0:
      _collector = &_zeroout;
      break;
    case 2:
      _collector = &_bp;
      break;
    case 3:
      _collector = &_hp;
      break;
    default:
      _collector = &_lp;
  }
  //_pout = _collector;
}


void tSVF12::linkEG(float* egcv)
{
  _peg = egcv;
}


void tSVF12::nextSample (void)
{ 
  _lp = _lp + _f * _bp;
  //_hp = *_pin * INSCALE - _lp - _fb * _bp;
  _hp = *_pin - _lp - _fb * _bp;
  _bp = (_f * _hp + _bp);
  *_pout = *_collector * _lim;
  
  //_bp -= (_bp * _bp * _bp) / 6; //Clipper band limited sigmoid
  //notch = _hp + _lp;
  //*_pout = *_collector * OUTSCALE;
}
