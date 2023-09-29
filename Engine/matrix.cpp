#include "matrix.h"

#include "cuveconv.h"


/*static tCuveConv *pcuveconv = 0;

void Matrix_CoreInit(void)
{
  static tCuveConv cuveconv(0, MAXCTRLVAL, EXPCUVERANGE, 1);
  pcuveconv = &cuveconv;
}*/


tMatrix::tMatrix()
{
  uint8_t i;
  
  for (i=0; i<SLOTSCOUNT; i++)
  {
    _srcnum[i] = 0xFF;
    _ctrlnum[i] = 0xFF;
    _destnum[i] = 0xFF;
    _srcamnt[i] = 0;
  }
  
  _psrc = (float*)&src;
  _pdest = (float*)&dest;

  //находим кол/во элементов структуры
  //длинну в байтах делим на 4 (размер типа float 32)
  _destcount = sizeof(dest) >> 2;
}


void tMatrix::update(void)
{
  uint8_t i;
  
  for (i=0; i<SLOTSCOUNT; i++)
  {
    if (_srcnum[i] == 0xFF)
      _destval[i] = 0; 
    else
      _destval[i] = _srcamnt[i] * *(_psrc + _srcnum[i]);
    
    if (_ctrlnum[i] != 0xFF)
      _destval[i] *= *(_psrc + _ctrlnum[i]);                     
  }
  
  for (i=0; i<_destcount; i++)
  {
    *(_pdest + i) = _destval[0] * (i == _destnum[0]) + 
                    _destval[1] * (i == _destnum[1]) + 
                    _destval[2] * (i == _destnum[2]) + 
                    _destval[3] * (i == _destnum[3]); 
  }
}



void tMatrix::connectSrc(uint8_t slotNum, uint8_t srcNum)
{
  if (srcNum != 0)
    _srcnum[slotNum] = srcNum - 1;
  else
    _srcnum[slotNum] = 0xFF;
}


void tMatrix::connectCtrl(uint8_t slotNum, uint8_t ctrlNum)
{
  if (ctrlNum != 0)
    _ctrlnum[slotNum] = ctrlNum - 1;
  else
    _ctrlnum[slotNum] = 0xFF;
}


void tMatrix::connectDest(uint8_t slotNum, uint8_t destNum)
{
  if (destNum != 0)
    _destnum[slotNum] = destNum - 1;
  else
    _destnum[slotNum] = 0xFF;
}


void tMatrix::setSrcAmount(uint8_t slotNum, uint16_t amount)
{
  _srcamnt[slotNum] =(float)(amount - (MAXCTRLVAL>>1)) / (MAXCTRLVAL>>1);
  //_srcamnt[slotNum] = (float)amount / MAXCTRLVAL;
  //_srcamnt[slotNum] = pcuveconv->linToExp(amount);
}


/*void tMatrix::setCtrlAmount(uint8_t slotNum, uint16_t amount)
{
  _ctrlamnt[slotNum] =(float)(amount - (MAXCTRLVAL>>1)) / (MAXCTRLVAL>>1);
}*/