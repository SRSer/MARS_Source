#ifndef VIBRATO_H_
#define VIBRATO_H_

#include "stdint.h"

#define TABLESTEP 2048 //��������� �������� � ������ �������
#define ACCUMSHIFT 5 //����� ������������ ���� ��� ��������� ������ � �������� �������

#define MAXCTRLVAL 4095 //������������ �������� 4095 (12 ���)
//#define EXPCUVERANGE 0.0001 //�������� ���������������� ������ 1/10000

#define MAXFREQ 25 //��
#define MINFREQ 0.1 //��

class tVibrato
{
  public:
    tVibrato(float* out); //�������
    void update(void);
    void setRate(uint16_t rate);
      
  private:
    float* _pout; //��������� �� ����� �������
    float _freq; //�������
    uint16_t _inc; //��������� ����
    uint16_t _accum; //������� ����������
};


void Vibrato_CoreInit (uint32_t clockfreq);

#endif /* VIBRATO_H_ */