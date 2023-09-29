#ifndef DCO_CORE_H_
#define DCO_CORE_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"


#define TUNEMUL 4 //наяальная частота высшего регистра осциллятора ноты Ля первой октавы
#define TABLECNT 10 //количество таблиц по одной на октаву
#define TABLESTEP 2048 //количетво отсчетов в каждой таблице
#define ACCUMSHIFT 21 //сдвиг аккумулятора фазы для получения адреса в волновой таблице

#define WTSINE 0
#define WTTRI 1
#define WTSAW 2
#define WTPULSE 3
#define WTNOISE 4

#define MAXCTRLVAL 4095 //максимальное значение 4095 (12 бит)
#define EGCUVERANGE 0.0001 //диапазон экспоненциальной кривой 1/10000
#define PORTACUVERANGE 0.00033 //диапазон экспоненциальной кривой 1/1000

#define PBFLTRFREQ 5.0 //частота ФНЧ pitch bend

//#define MAXREGISTRS 5 //количество регистров

#define DECAYTARGETLEVEL 0.33 // 33%
#define AMAXTIME 10.0
#define DMAXTIME 30.0
#define RMAXTIME 30.0
#define PORTARANGE 0.1 // 10 центов
#define PORTAMAXTIME 60.0 // сек

#define FREQMODRANGE 64

#define FMDEPTHMULT 3 

//класс Digital Controlled Oscillator
class tDCO
{
  public:
    tDCO(float* out, float* cv, uint8_t* gate, uint8_t* trig, float* freqmod, float* pwm, float* fm, float* fmmod); //создать и соединить с выходом
    //~tDCO();
    
    void update(); //обновить параметры осциллятора
    //void egTrig(uint8_t gate); //запуск остановка генератора огибающей   
    void nextSample(void); //сделать один отсчет дискретизации
    
    //void linkCV(float* cv); //соединить cv
    //void linkGate(uint8_t* gate);
    //void linkPitchBend(uint16_t* pb); //соединить питч бенд
    
    void setSemitones(uint8_t semitones);
    void setDetune(uint16_t detune);
    void setWaveMode(uint8_t mode); //переключает волноформы в разных режимах
    void setWaveForm(uint8_t form); //изменить волноформу (0-синус, 1-треугольник, 2-пила, 3-пульс, 4-шум)
    void setWaveShape(uint16_t shape);
    void setPhase(uint32_t phase);
    void setRetrig(uint8_t enable);
    void setFMDepts(uint16_t depts);
    //void setEnvA(uint16_t time);
    //void setEnvDR(uint16_t time);
    //void setEnvMode(uint8_t mode);
    //uint32_t f; //для отладчика
    
  private:
    //uint32_t _clockfreq; //частота дискретизации
    float _freq; //частота
    float _portacv; //выход cv портаменто
    float* _pcv; //указатель на управляющее напряжение 12 вольт/октава
    float* _pout; //указатель на выход
    float* _pfreqmod; //указатель на частотную модуляцию
    float* _ppwm; //PW Modulation CV
    float* _pfm; //FM CV
    float* _pfmmod; //FM Modulation CV
    uint8_t* _pgate; //указатель на гейт
    uint8_t* _ptrig; //указатель на триггер запуска
    uint8_t _prevgate; //предидущее состояние гейта
    uint8_t _prevtrig; //предидущее состояние триггера
    float _detune; //расстойка осциллятора
    float _semitones; //расстройка в полутонах
    uint8_t _registr; //регистр 0-первый, 1-второй, 2-третий и т.д.
    uint8_t _wavemode; //режим волноформ
    uint8_t _waveform; //форма волны
    uint32_t _pulsewidth; //Pulse Width
    uint32_t _fmdepth; //глубина фм
    int32_t _fmdepthmod; //глубина модуляции фм
    int32_t _pwm; //PW Modulation
    //int32_t _fm; //FM
    uint16_t _shape; 
    uint32_t _inc; //пиращение фазы
    uint32_t _accum; //фазовый аккумултор
    uint32_t _phaseshift; //сдвиг фазы
    uint32_t _phase; //фаза
    uint16_t _shift; //начальный адрес таблицы для текущей октавы
    int16_t _sample; //отсчет дискретизации
    uint8_t _retrig; //сброс фазы разрешен

    float _egphase; //фаза генератора огибающей
    //float _coefattack; //коэффициент аттаки 
    //float _coefdecay; //коэффициент спада / послезвучия
    //uint8_t _egmode; //режим AD/AR
    uint8_t _egstage; //стадия генератора огибающей A или D/R
    
    const int16_t* _table[TABLECNT]; //указытель на волновую таблицу
    
    int16_t getSampleUni(void); //сделать отсчет универсальный (пила, треугольник, или произвольная форма волны)
    int16_t getSamplePulse(void); //сделать отсчет пульс
    int16_t getSampleSine(void); //сделать отсчет синус
    int16_t getSampleNoise(void); //сделать отсчет шум
    
    typedef int16_t(tDCO::*pFunc)(void);
    pFunc _getSampleFunc; //указатель на выбранную функцию отсчета
};


void DCO_CoreInit(uint32_t clockfreq, uint32_t modfreq, float tune, RNG_HandleTypeDef* rng); //инициализация начальных значений
void DCO_UpdatePBFltr(void); //ФНЧ pitch bend в секциию модуляции

void DCO_SetEnvA(uint16_t time);
void DCO_SetEnvD(uint16_t time);
void DCO_SetEnvR(uint16_t time);
void DCO_SetEnvS(uint16_t level);
void DCO_SetEnvRetrig(uint8_t enable);
void DCO_SetPortaTime(uint16_t time);
void DCO_SetLegato(uint8_t enable);
void DCO_SetPitchBend (uint16_t value);
void DCO_SetPBRange (uint8_t value);


#endif /* DCO_CORE_H_ */