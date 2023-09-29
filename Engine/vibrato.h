#ifndef VIBRATO_H_
#define VIBRATO_H_

#include "stdint.h"

#define TABLESTEP 2048 //количетво отсчетов в каждой таблице
#define ACCUMSHIFT 5 //сдвиг аккумул€тора фазы дл€ получени€ адреса в волновой таблице

#define MAXCTRLVAL 4095 //максимальное значение 4095 (12 бит)
//#define EXPCUVERANGE 0.0001 //диапазон экспоненциальной кривой 1/10000

#define MAXFREQ 25 //√ц
#define MINFREQ 0.1 //√ц

class tVibrato
{
  public:
    tVibrato(float* out); //создать
    void update(void);
    void setRate(uint16_t rate);
      
  private:
    float* _pout; //указатель на выход сигнала
    float _freq; //частота
    uint16_t _inc; //пиращение фазы
    uint16_t _accum; //фазовый аккумултор
};


void Vibrato_CoreInit (uint32_t clockfreq);

#endif /* VIBRATO_H_ */