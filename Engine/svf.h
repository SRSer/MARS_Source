#ifndef SVF_H_
#define SVF_H_


#include "stdint.h"


#define MAXCTRLVAL 4095 //максимальное значение 4095 (12 бит)
#define COFFCUVERANGE 0.001 //диапазон экспоненциальной кривой 1/1000

#define INSCALE (0.5 / 32768) //(1.525878906e-05)
#define OUTSCALE (1.5 * 32768) //(49150)
#define MAXCUTFREQ 10000.0 //Hz


//класс State Variable Filter -12 db/oct
class tSVF12
{
  public:
    tSVF12(float* in, float* out, float* egcv, float* modcv); //создать и соединить с выходом
    //~tSVF12();
    
    void nextSample(void);
    void update(void);
    
    void setMode(uint8_t mode);
    void linkEG(float* egcv);
    void setCutoff(uint16_t cutoff);
    void setResonance(uint16_t q);
    void setAmount(uint16_t amount);

  private:
    float _fcoff;
    float _amount;
    
    float _f;
    float _fb;
    float _lim;
    float _lp;
    float _bp;
    float _hp;
    float _zeroout;
    float *_collector;
    
    float* _pin;
    float* _pout;
    
    float* _peg;
    float* _pmod;
};


void SVF12_CoreInit(uint32_t clockfreq);

#endif /* SVF_H_ */