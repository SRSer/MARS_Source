#ifndef MATRIX_H_
#define MATRIX_H_

#include "stdint.h"

#define MAXCTRLVAL 4095 //максимальное значение 4095 (12 бит)
//#define EXPCUVERANGE 0.01 //диапазон экспоненциальной кривой 1/100

//#define SRCCOUNT 4
//#define DESTCOUNT 4
#define SLOTSCOUNT 4

class tMatrix
{
  public:
    tMatrix(); //создать
    void update(void); 
    void connectSrc(uint8_t slotNum, uint8_t srcNum);
    void connectCtrl(uint8_t slotNum, uint8_t srcNum);
    void connectDest(uint8_t slotNum, uint8_t destNum);
    void setSrcAmount(uint8_t slotNum, uint16_t amount);
    
    struct
    {
      float EG1;
      float EG2;
      float Vibrato;
      float LFO;
      float Velocity;
      float Wheel;
      float Aftertoch;
      float Keytrack;
    } src;
      
    struct
    {
      float VCFACutoff;
      float VCFBCutoff;
      float VCOSFreq;
      float VCOSPWM;
      float VCO21FM;
      float VCO43FM;
      float LFORate;
      float BDDMod;
    } dest;

  private:
    uint8_t _srcnum[SLOTSCOUNT];
    uint8_t _ctrlnum[SLOTSCOUNT];
    uint8_t _destnum[SLOTSCOUNT];
    float _srcamnt[SLOTSCOUNT];
    float _destval[SLOTSCOUNT];
      
    float* _psrc;
    float* _pdest;
    uint8_t _destcount;

};


//void Matrix_CoreInit(void);

#endif /* MATRIX_H_ */