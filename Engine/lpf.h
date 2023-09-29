#ifndef LPF_H_
#define LPF_H_


#include "stdint.h"

#define MAXCTRLVAL 4095 //максимальное значение 4095 (12 бит)
#define COFFCUVERANGE 0.001 //диапазон экспоненциальной кривой 1/1000

#define INSCALE (0.5 / 32768)
#define OUTSCALE (32767 * 1.5)
#define MAXCUTFREQ 10000.0 //Hz


//класс State Variable Filter -12 db/oct
class tLPF24
{
  public:
    tLPF24(float* in, float* out, float* egcv, float* modcv); //создать и соединить с выходом
    //~tLPF24();
    
    void nextSample(void);
    void update(void);
    
    void setMode(uint8_t mode);
    void linkEG(float* egcv);
    void setCutoff(uint16_t cutoff);
    void setResonance(uint16_t q);
    void setAmount(uint16_t amount);

  private:
    float _fcoff;
    float _reso;
    float _amount;
    
    float _f;
    float _fb;
    //float _lp;
    //float _zeroout;
    //float *_collector;
    
    float _in1;
    float _in2;
    float _in3;
    float _in4;
    float _out1;
    float _out2;
    float _out3;
    float _out4;
    
    float* _pin;
    float* _pout;
    
    float* _peg;
    float* _pmod;
};


void LPF24_CoreInit(uint32_t clockfreq);

#endif /* LPF_H_ */