#ifndef HADSR_H_
#define HADSR_H_

#include "stdint.h"


#define DECAYTARGETLEVEL 0.33 // 33%
#define HMAXTIME 60
#define AMAXTIME 60
#define DMAXTIME 60 //60
#define RMAXTIME 60
#define RETRIGLEVEL 0.01

#define MAXCTRLVAL 4095 //������������ �������� 4095 (12 ���)
#define EXPCUVERANGE 0.0001 //�������� ���������������� ������ 1/10000


class tHADSR
{
  public:
    tHADSR(float* out, uint8_t* gate, uint8_t* trig); //������� � ��������� � �������
    //~tHADSR();
    //void linkCV(float* cv); //��������� cv
    //void linkGate(uint8_t* gate);
    
    void update(void);
    
    void setHold(uint16_t time);
    void setAttack(uint16_t time);
    void setDecay(uint16_t time);
    void setSustain(uint16_t level);
    void setRelease(uint16_t time);
    void setMode(uint8_t mode);
    
    //float _out;

  private:
    float* _pout;
    uint8_t* _pgate;
    uint8_t* _ptrig;
    
    uint8_t _trig;
    uint8_t _gate;
    float _hold;
    float _phase; //���� ���������� ���������
    uint8_t _mode; //����� ��������
    uint8_t _stage; //������ ���������� ���������
    float _sustain;
    float _coefhold;
    float _coefattack;
    float _coefdecaya0;
    float _coefdecayb1;
    float _coefrelease;
    uint16_t _decaytime;
};


void HADSR_CoreInit(uint32_t clockfreq);

#endif /* HADSR_H_ */