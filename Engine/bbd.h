#ifndef BBD_H_
#define BBD_H_

#include "stdint.h"

#define TABLESTEP 2048 //��������� �������� � ������ �������
#define ACCUMSHIFT 5 //����� ������������ ���� ��� ��������� ������ � �������� �������

#define MAXCTRLVAL 4095 //������������ �������� 4095 (12 ���)
//#define EXPCUVERANGE 0.0001 //�������� ���������������� ������ 1/10000

#define MAXFREQ 25 //��
#define MINFREQ 0.1 //��

class tBBD
{
  public:
    tBBD(float* mod); //�������
    void update(void);
    //void setRate(uint16_t rate);
      
  private:
    float* _pmod;
    float _freq; //�������
    //uint16_t _inc; //��������� ����
    //uint16_t _accum; //������� ����������
};


//void BBD_CoreInit (uint32_t clockfreq);

#endif /* BBD_H_ */