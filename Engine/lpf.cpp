#include "lpf.h"


#include "math.h"
#include "fastmath.h" //функция быстрого приближенного рассчета экспоненты
#include "cuveconv.h"



uint32_t lpfclock;

static tCuveConv *pcoffcuve = 0;


void LPF24_CoreInit (uint32_t clockfreq)
{
  lpfclock = clockfreq;
  static tCuveConv coffcuve(0, 1, COFFCUVERANGE, 1);
  pcoffcuve = &coffcuve;
}



tLPF24::tLPF24 (float* in, float* out, float* egcv, float* modcv)
{
  _pin = in;
  _pout = out;
  _peg = egcv;
  _pmod = modcv;
  _fcoff = 0;
  _amount = 0;
  
  _in1 = _in2 = _in3 = _in4 = 
  _out1 = _out2 = _out3 = _out4 = 0;
  
  setCutoff(MAXCTRLVAL / 2);
  setResonance(0);
  //setMode(1);
}


void tLPF24::setCutoff (uint16_t cutoff)
{
  _fcoff = (float)cutoff / MAXCTRLVAL;
}


void tLPF24::setResonance (uint16_t resonance)
{
  _reso = (float)resonance / MAXCTRLVAL;
}


void tLPF24::update(void)
{
  float f;

  f = _fcoff + *_peg * _amount + *_pmod;
    
  _f = (1.16 * MAXCUTFREQ) * pcoffcuve->linToExp(f) / lpfclock;
  
  _fb = 4.0 * _reso * (1.0 - 0.15 * _f * _f);
}


void tLPF24::setAmount(uint16_t amount)
{
  _amount =( (float)amount - (MAXCTRLVAL>>1) ) / (MAXCTRLVAL>>1);
  //_amount = (float)amount / MAXCTRLVAL;
}


/*void tLPF24::setMode (uint8_t mode)
{
  switch (mode)
  {
    case 0:
      _collector = &_zeroout;
      break;
    case 1:
      _collector = &_bp;
      break;
    case 2:
      _collector = &_hp;
      break;
    default:
      _collector = &_lp;
  }
}*/


void tLPF24::linkEG(float* egcv)
{
  _peg = egcv;
}


void tLPF24::nextSample (void)
{ 
  float input = *_pin;
    
  input -= _out4 * _fb;
  input *= 0.35013 * (_f*_f)*(_f*_f);
  _out1 = input + 0.3 * _in1 + (1 - _f) * _out1; // Pole 1
  _in1  = input;
  _out2 = _out1 + 0.3 * _in2 + (1 - _f) * _out2;  // Pole 2
  _in2  = _out1;
  _out3 = _out2 + 0.3 * _in3 + (1 - _f) * _out3;  // Pole 3
  _in3  = _out2;
  _out4 = _out3 + 0.3 * _in4 + (1 - _f) * _out4;  // Pole 4
  _in4  = _out3;
  
  *_pout = _out4;
  
  //*_pout = *_collector * OUTSCALE;
}