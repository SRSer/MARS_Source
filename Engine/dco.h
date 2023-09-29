#ifndef DCO_CORE_H_
#define DCO_CORE_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"


#define TUNEMUL 4 //��������� ������� ������� �������� ����������� ���� �� ������ ������
#define TABLECNT 10 //���������� ������ �� ����� �� ������
#define TABLESTEP 2048 //��������� �������� � ������ �������
#define ACCUMSHIFT 21 //����� ������������ ���� ��� ��������� ������ � �������� �������

#define WTSINE 0
#define WTTRI 1
#define WTSAW 2
#define WTPULSE 3
#define WTNOISE 4

#define MAXCTRLVAL 4095 //������������ �������� 4095 (12 ���)
#define EGCUVERANGE 0.0001 //�������� ���������������� ������ 1/10000
#define PORTACUVERANGE 0.00033 //�������� ���������������� ������ 1/1000

#define PBFLTRFREQ 5.0 //������� ��� pitch bend

//#define MAXREGISTRS 5 //���������� ���������

#define DECAYTARGETLEVEL 0.33 // 33%
#define AMAXTIME 10.0
#define DMAXTIME 30.0
#define RMAXTIME 30.0
#define PORTARANGE 0.1 // 10 ������
#define PORTAMAXTIME 60.0 // ���

#define FREQMODRANGE 64

#define FMDEPTHMULT 3 

//����� Digital Controlled Oscillator
class tDCO
{
  public:
    tDCO(float* out, float* cv, uint8_t* gate, uint8_t* trig, float* freqmod, float* pwm, float* fm, float* fmmod); //������� � ��������� � �������
    //~tDCO();
    
    void update(); //�������� ��������� �����������
    //void egTrig(uint8_t gate); //������ ��������� ���������� ���������   
    void nextSample(void); //������� ���� ������ �������������
    
    //void linkCV(float* cv); //��������� cv
    //void linkGate(uint8_t* gate);
    //void linkPitchBend(uint16_t* pb); //��������� ���� ����
    
    void setSemitones(uint8_t semitones);
    void setDetune(uint16_t detune);
    void setWaveMode(uint8_t mode); //����������� ���������� � ������ �������
    void setWaveForm(uint8_t form); //�������� ���������� (0-�����, 1-�����������, 2-����, 3-�����, 4-���)
    void setWaveShape(uint16_t shape);
    void setPhase(uint32_t phase);
    void setRetrig(uint8_t enable);
    void setFMDepts(uint16_t depts);
    //void setEnvA(uint16_t time);
    //void setEnvDR(uint16_t time);
    //void setEnvMode(uint8_t mode);
    //uint32_t f; //��� ���������
    
  private:
    //uint32_t _clockfreq; //������� �������������
    float _freq; //�������
    float _portacv; //����� cv ����������
    float* _pcv; //��������� �� ����������� ���������� 12 �����/������
    float* _pout; //��������� �� �����
    float* _pfreqmod; //��������� �� ��������� ���������
    float* _ppwm; //PW Modulation CV
    float* _pfm; //FM CV
    float* _pfmmod; //FM Modulation CV
    uint8_t* _pgate; //��������� �� ����
    uint8_t* _ptrig; //��������� �� ������� �������
    uint8_t _prevgate; //���������� ��������� �����
    uint8_t _prevtrig; //���������� ��������� ��������
    float _detune; //��������� �����������
    float _semitones; //���������� � ���������
    uint8_t _registr; //������� 0-������, 1-������, 2-������ � �.�.
    uint8_t _wavemode; //����� ���������
    uint8_t _waveform; //����� �����
    uint32_t _pulsewidth; //Pulse Width
    uint32_t _fmdepth; //������� ��
    int32_t _fmdepthmod; //������� ��������� ��
    int32_t _pwm; //PW Modulation
    //int32_t _fm; //FM
    uint16_t _shape; 
    uint32_t _inc; //��������� ����
    uint32_t _accum; //������� ����������
    uint32_t _phaseshift; //����� ����
    uint32_t _phase; //����
    uint16_t _shift; //��������� ����� ������� ��� ������� ������
    int16_t _sample; //������ �������������
    uint8_t _retrig; //����� ���� ��������

    float _egphase; //���� ���������� ���������
    //float _coefattack; //����������� ������ 
    //float _coefdecay; //����������� ����� / �����������
    //uint8_t _egmode; //����� AD/AR
    uint8_t _egstage; //������ ���������� ��������� A ��� D/R
    
    const int16_t* _table[TABLECNT]; //��������� �� �������� �������
    
    int16_t getSampleUni(void); //������� ������ ������������� (����, �����������, ��� ������������ ����� �����)
    int16_t getSamplePulse(void); //������� ������ �����
    int16_t getSampleSine(void); //������� ������ �����
    int16_t getSampleNoise(void); //������� ������ ���
    
    typedef int16_t(tDCO::*pFunc)(void);
    pFunc _getSampleFunc; //��������� �� ��������� ������� �������
};


void DCO_CoreInit(uint32_t clockfreq, uint32_t modfreq, float tune, RNG_HandleTypeDef* rng); //������������� ��������� ��������
void DCO_UpdatePBFltr(void); //��� pitch bend � ������� ���������

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