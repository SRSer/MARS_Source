#include "hadsr.h"

#include "math.h"
#include "fastmath.h"
#include "cuveconv.h"


uint32_t hadsrclock;

static float eglog;

static tCuveConv *pcuveconv = 0;


void HADSR_CoreInit(uint32_t clockfreq)
{
  hadsrclock = clockfreq;
  eglog = logf(DECAYTARGETLEVEL);
  static tCuveConv cuveconv(1, MAXCTRLVAL, EXPCUVERANGE, 1);
  pcuveconv = &cuveconv;
}



tHADSR::tHADSR(float* out, uint8_t* gate, uint8_t* trig)
{ 
  _pout = out;
  _pgate = gate;
  _ptrig = trig;
  
  _trig = *_ptrig;
  _gate = 0;
  _mode = 0;
  _hold = 0;
  _phase = 0;
  _stage = 4;
  _sustain = 0.5;
  _coefhold = 1;
  _coefattack = 1;
  setDecay(2048);
  //_coefdecay = 0;
  setRelease(2048);
  //_coefrelease = 0;
  _decaytime = 0;
}


void tHADSR::update(void)
{
  
  if (*_ptrig != _trig)
  {
    _trig = *_ptrig;
    //_stage = 0;
    if (_mode & 0x02) _stage = 0;
  }
  
  if (*_pgate != _gate)
  {
    _gate = *_pgate;
    //if (!_gate) _stage = 4;
    if (_gate) _stage = 0; else _stage = 4;
  }
  
  switch (_stage)
  {
    case 0:
    {
      _hold += _coefhold;
      _phase *= _coefrelease;
      if (_hold > 1.0)
      {
        _hold = 0;
        _stage = 1;
      }
      break;
    }
      
    case 1:
    {
      _phase += _coefattack;
      if (_phase > 1.0)
      {
        _phase = 1.0;
        _stage = 2;
      }
      break;
    }
    
    case 2:
    //case 3:
    {
      //_phase -= _coefdecay;
      _phase = _sustain * _coefdecaya0 + _phase * _coefdecayb1;
      if (_phase < _sustain + RETRIGLEVEL )
      {
        //_phase = _sustain;
        if (_mode & 0x01) _stage = 1;// else _stage = 3;
      }
      break;
    }
    
    case 4:
    {
      _phase *= _coefrelease;
      _hold = 0;
      break;
    }
    
  }
  
  *_pout = _phase;
}


void tHADSR::setHold(uint16_t time)
{
  if (time)
    _coefhold = 1.0 / ( HMAXTIME * hadsrclock * pcuveconv->linToExp(time) );
  else
    _coefhold = 1.0;
}


void tHADSR::setAttack(uint16_t time)
{
  if (time)
    _coefattack = 1.0 / ( AMAXTIME * hadsrclock * pcuveconv->linToExp(time) );
  else
    _coefattack = 1.0;
}


void tHADSR::setDecay(uint16_t time)
{
  _decaytime = time;
  if (time)
    //_coefdecay = (1.0 - _sustain) / ( DMAXTIME * hadsrclock * pcuveconv->linToExp(time) );
    _coefdecayb1 = 1.0 + eglog / ( RMAXTIME * hadsrclock * pcuveconv->linToExp(time) );
  else
    //_coefdecay = 1.0;
    _coefdecayb1 = 0;
  
  _coefdecaya0 = 1.0 - _coefdecayb1;
}


void tHADSR::setSustain(uint16_t level)
{
  _sustain = (float)level / MAXCTRLVAL;
  setDecay(_decaytime);
}


void tHADSR::setRelease(uint16_t time)
{
  if (time)
    _coefrelease = 1.0 + eglog / ( RMAXTIME * hadsrclock * pcuveconv->linToExp(time) );
  else
    _coefrelease = 0;
}


void tHADSR::setMode(uint8_t mode)
{
  _mode = mode;
}


/*
void tHADSR::linkGate(uint8_t* gate)
{
  _pgate = gate;
}
*/