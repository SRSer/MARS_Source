#ifndef ONEPOLE_H 
#define ONEPOLE_H

#include "math.h"
#include "fastmath.h" //функция быстрого приближенного рассчета экспоненты

class tOnePole 
{ 
  public:
    tOnePole() {a0 = 1.0; b1 = 0.0; z1 = 0.0;}; 
    tOnePole(float Fc) {z1 = 0.0; setFc(Fc);}; //Fc = cutoff / clockfreq
    //~tOnePole();
    void setFc(float Fc);
    float update(float in);
  protected:
    float a0, b1, z1;
};

inline void tOnePole::setFc(float Fc) 
{ 
  b1 = fastexp(-2.0 * M_PI * Fc);
  a0 = 1.0 - b1;
}

inline float tOnePole::update(float in) 
{ 
  return z1 = in * a0 + z1 * b1;
} 

#endif