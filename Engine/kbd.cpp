#include "kbd.h"
#include "midi.h"

//******************* ВЕСЬ МОДУЛЬ ДИКИЙ БЫДЛОКОД С ПРЕДЪЯВОЙ НА ОПТИМИЗАЦИЮ ***********************//

tKBD::tKBD(float* cv, uint8_t* gate, uint8_t* trig, uint8_t* comgate, uint8_t* comtrig, float* keytrack)
{ 
  _pcv = cv;
  _pkeytrack = keytrack;
  _pgate = gate;
  _ptrig = trig;
  _pcomgate = comgate;
  _pcomtrig = comtrig;
  *_pkeytrack = 0;
  //*_notesout = new uint8_t(4);
  resetNotesMem();
  setMode(KBDMONOLAST);
}


void tKBD::resetNotesMem(void)
{
  uint8_t i;
  for (i=0; i<MAXNOTESMEM; i++) _notesmem[i] = 0xFF;
  *(uint32_t*)_notesout = 0xFFFFFFFF;
  _mempointer = 0;
}


void tKBD::setMode(uint8_t mode)
{
  
  switch (mode)
  {
    case KBDMONOLOW:
    {
      _setNotesMemFunc = &tKBD::setNotesFree;
      _getCvGateFunc = &tKBD::getCvMonoLo;
      break;
    }
    case KBDMONOHI:
    {
      _setNotesMemFunc = &tKBD::setNotesFree;
      _getCvGateFunc = &tKBD::getCvMonoHi;
      break;
    }
    case KBDMONOLAST:
    {
      _setNotesMemFunc = &tKBD::setNotesCircular;
      _getCvGateFunc = &tKBD::getCvMonoLast;
      break;
    } 
    case KBDPOLY1:
    {
      _setNotesMemFunc = &tKBD::setNotesFree;
      _getCvGateFunc = &tKBD::getCvPoly1;
      break;
    }
    case KBDPOLY2:
    {
      _setNotesMemFunc = &tKBD::setNotes4Voice;
      _getCvGateFunc = &tKBD::getCvPoly2;
      break;
    } 
  }
  
  //resetNotesMem();
  (this->*_getCvGateFunc)();
}


void tKBD::setGateUp(uint8_t enable)
{
  _gateup = enable;
  if (_gateup)
  {
    *(uint32_t*)_pgate = 0x01010101;
    *(uint32_t*)_ptrig ^= 0x01010101;
    *_pcomtrig ^= 1;
    *_pcomgate = 1;
  }
  else
  {
    *(uint32_t*)_pgate = 0;
    *_pcomgate = 0;
  }
  //(this->*_getCvGateFunc)();
}


void tKBD::setNote(uint8_t note, uint8_t state)
{
  _note = note;
  _state = state;
  (this->*_setNotesMemFunc)(); 
  (this->*_getCvGateFunc)(); 
  *_pcomgate = _pgate[0] | _pgate[1] | _pgate[2] | _pgate[3];
}



void tKBD::setNotesFree(void)
{
  uint8_t i = 0;
  
  if (_state)
  {       
    do
    {
      if ( _notesmem[i] == 0xFF) break;
      ++i;
    }
    while ( i <= MAXNOTESMEM );
    
    if ( i == MAXNOTESMEM ) return; 
    
    _notesmem[i] = _note;
    //_noteson++;
  }
  else
  {  
    do
    {
      if ( _notesmem[i] == _note)
      {
        _notesmem[i] = 0xFF;
        //_noteson--;
      }
      ++i;
    }
    while ( i <= MAXNOTESMEM );
    
    //if ( i == MAXNOTESMEM ) return;
  }
}


void tKBD::setNotes4Voice(void)
{  
  static uint8_t noteson[4] = {0xFF,0xFF,0xFF,0xFF};
  uint8_t i;
  
  if (_state) //если нота нажата
  {   
    //если есть голоса настроенные на этой ноте перезапускаем
    for (i=0; i<4; i++)
    {
      if (noteson[i] == _note)
      {
        _notesmem[i] = _note;
        return;
      }
    }
    
    //если есть свободные голоса назначаем на свободный
    for (i=0; i<4; i++)
    {
      if (_notesmem[_mempointer] == 0xFF)
      {
        _notesmem[_mempointer] = noteson[_mempointer] = _note;
        ++_mempointer &= 0x03;
        return;
      }
      ++_mempointer &= 0x03;
    }
    
    //если нет свободных голосов заменяем по кругу
    _notesmem[_mempointer] = noteson[_mempointer] = _note;
    ++_mempointer &= 0x03;
  }
  else //если нота отпущена
  {
    //ищем голос настроенный на ноту и отключаем
    for (i=0; i<4; i++) 
      if (_notesmem[i] == _note) _notesmem[i] = 0xFF;
  }
}


void tKBD::setNotesCircular(void)
{
  uint8_t i;
  
  if (_state)
  {
    i = _mempointer;
    while ( (_notesmem[i] == 0xFF) )
    {
      --i &= (MAXNOTESMEM - 1);
      if (i == _mempointer)
      {
        _mempointer = 0;
        _notesmem[_mempointer] = _note;
        return;
      }
    }
    
   _mempointer = ++i & (MAXNOTESMEM - 1);
    _notesmem[_mempointer] = _note;
  }
  else
  {
    for (i=0; i<MAXNOTESMEM; i++)
    {
      if (_notesmem[i] == _note) _notesmem[i] = 0xFF;
    }
  }
  
}


void tKBD::getCvPoly1(void)
{
  static uint32_t lasttrigstate = 0;
  uint8_t i;  
  uint8_t top = 0;
    
  for (i=0; i<4; i++)
  {
    if (_notesmem[i] != 0xFF)
    {
      _pcv[i] = (float)(_notesmem[i] - ZEROKEYNUM);
      _pgate[i] = 1;
      if (_notesmem[i] != _notesout[i])
        _ptrig[i] = !_ptrig[i];
      
      if (top < _notesmem[i]) top = _notesmem[i];
    }
    else
    {
      _pgate[i] = _gateup;
    }
  }
  
  *(uint32_t*)_notesout = *(uint32_t*)_notesmem;
  
  if ((*(uint32_t*)_ptrig) != lasttrigstate)
  {
    lasttrigstate = (*(uint32_t*)_ptrig);
    *_pcomtrig ^= 1;
  }
  
  if (top) *_pkeytrack = (float)(top - LNOTE) / (HNOTE - LNOTE);
}




void tKBD::getCvPoly2(void)
{
  static uint32_t lasttrigstate = 0;
  uint8_t i;
  for (i=0; i<4; i++)
  {
    if (_notesmem[i] != _notesout[i])
    {
      if (_notesmem[i] != 0xFF)
      {
        _pcv[i] = (float)(_notesmem[i] - ZEROKEYNUM);
        _pgate[i] = 1;
        _ptrig[i] ^= 1;
      }
      else
      {
        _pgate[i] = _gateup;
      }
    }
  }
  
  *(uint32_t*)_notesout = *(uint32_t*)_notesmem;
  
  if ((*(uint32_t*)_ptrig) != lasttrigstate)
  {
    lasttrigstate = (*(uint32_t*)_ptrig);
    *_pcomtrig ^= 1;
  }
}




void tKBD::getCvMonoHi(void)
{
  uint8_t i;
  uint8_t top = 0;
  static uint8_t lastnote = 0;
  
  for (i=0; i<MAXNOTESMEM; i++)
  {
    if ( (_notesmem[i] != 0xFF) && (_notesmem[i] > top) )
      top = _notesmem[i];
  }
  
  if (top)
  {
    _pcv[0] = _pcv[1] = _pcv[2] = _pcv[3] = (float)(top - ZEROKEYNUM);
    if ( !_pgate[0] ) *(uint32_t*)_ptrig ^= 0x01010101;
    *(uint32_t*)_pgate = 0x01010101;   
    if (top != lastnote) *_pcomtrig ^= 1;
    lastnote = top;
    *_pkeytrack = (float)(top - LNOTE) / (HNOTE - LNOTE);
  }
  else
  {
    _pgate[0] = _pgate[1] = _pgate[2] = _pgate[3] = _gateup;
    lastnote = 0;
  }
}



void tKBD::getCvMonoLo(void)
{
  uint8_t i;
  uint8_t top = 0;
  uint8_t bottom = 0xFF;
  static uint8_t lastnote = 0;
  
  for (i=0; i<MAXNOTESMEM; i++)
  {
    if ( (_notesmem[i] != 0xFF) && (_notesmem[i] > top) )
      top = _notesmem[i];
    if (_notesmem[i] < bottom)
      bottom = _notesmem[i];
  }
  
  if (bottom != 0xFF)
  {
    _pcv[0] = _pcv[1] = _pcv[2] = _pcv[3] = (float)(bottom - ZEROKEYNUM);
    if ( !_pgate[0] ) *(uint32_t*)_ptrig ^= 0x01010101;
    *(uint32_t*)_pgate = 0x01010101;   
    if (bottom != lastnote) *_pcomtrig ^= 1;
    lastnote = bottom;
    *_pkeytrack = (float)(top - LNOTE) / (HNOTE - LNOTE);
  }
  else
  {
    _pgate[0] = _pgate[1] = _pgate[2] = _pgate[3] = _gateup;
    lastnote = 0;
  }
}




void tKBD::getCvMonoLast(void)
{
  //uint8_t i = 0;
  uint8_t p;
  //uint8_t last = 0;
  static uint8_t lastnote = 0;
  
  p = _mempointer;
  while ( (_notesmem[p] == 0xFF) )
  {
    --p &= (MAXNOTESMEM - 1);
    //i++;
    if (p == _mempointer)
    {
      _pgate[0] = _pgate[1] = _pgate[2] = _pgate[3] = _gateup;
      lastnote = 0;
      return;
    }
  }
  
  //if (_mempointer)
  //{
    _pcv[0] = _pcv[1] = _pcv[2] = _pcv[3] = (float)(_notesmem[p] - ZEROKEYNUM);
    if ( !_pgate[0] ) *(uint32_t*)_ptrig ^= 0x01010101;
    *(uint32_t*)_pgate = 0x01010101;   
    if (_notesmem[p] != lastnote) *_pcomtrig ^= 1;
    lastnote = _notesmem[p];
    *_pkeytrack = (float)(_notesmem[p] - LNOTE) / (HNOTE - LNOTE);
  //}
}
