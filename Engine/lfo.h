#ifndef LFO_H_
#define LFO_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

#define MAXCTRLVAL 4095 //максимальное значение 4095 (12 бит)

#define MAXFREQ 25 //√ц
#define MINFREQ 0.1 //√ц

#define MIDISYNCSTEPS 16



class tLFO
{
  public:
    tLFO(float* out, uint8_t* gate, float* mod); //создать
    void update(void);
    //void sync(void);
    void setRate(uint16_t rate);
    void setSlope(uint16_t slope);
    void setForm(uint8_t form);
    void setRetrig(uint8_t retrig);
    void setMode(uint8_t sync);
      
  private:
    float* _pout; //указатель на выход сигнала
    float _freq; //частота
    float _syncfreq;
    float _multsaw; //множитель наростани€
    float _multramp; //множитель спада
    uint32_t _inc; //пиращение фазы
    uint32_t _accum; //фазовый аккумултор
    uint32_t _slope; //наклон/скважность
    uint32_t _snhprevaccum;
    uint8_t _snhtrigged;
    uint8_t _gate;
    uint8_t _retrig;
    uint8_t _sync;
    uint8_t _syncsize;
    uint8_t _syncstart;
    uint32_t _tickcounter;
    uint16_t _ticks;
    uint16_t _duration;
    uint8_t* _pgate;
    float* _pmod;
    float _snhmem; //пам€ть a S&H
    
    float getTriRamp(void);
    float getPulse(void);
    float getSnH(void);
    float getNoise(void);
    
    typedef float(tLFO::*pFunc)(void);
    pFunc _getUpdateFunc; //указатель на выбранную функцию отсчета
};


void LFO_CoreInit (uint32_t clockfreq, RNG_HandleTypeDef* rng);

void LFO_SyncTick ( uint16_t duration );

void LFO_SyncStart( void );

#endif /* LFO_H_ */