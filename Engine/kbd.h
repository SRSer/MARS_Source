#ifndef KBD_H_
#define KBD_H_

#include "stdint.h"


#define MAXNOTESMEM 16 //максимальное количество запоминаемых нот
#define ZEROKEYNUM 69 //центральна€ Ћ€

#define KBDMONOHI 0
#define KBDMONOLOW 1
#define KBDMONOLAST 2
#define KBDPOLY1 3
#define KBDPOLY2 4


//класс Digital Controlled Oscillator
class tKBD
{
  public:
    tKBD(float* cv, uint8_t* gate, uint8_t* trig, uint8_t* comgate, uint8_t* comtrig, float* keytrack); //создать и соединить с выходом
    //~tKBD();
    void setNote(uint8_t note, uint8_t state);
    void setMode(uint8_t mode);
    void setGateUp(uint8_t enable);

  private:
    void resetNotesMem(void);
    void setNotesFree(void);
    void setNotes4Voice(void);
    void setNotesCircular(void);
    void getCvPoly1(void);
    void getCvPoly2(void);
    void getCvMonoHi(void);
    void getCvMonoLo(void);
    void getCvMonoLast(void);
    float* _pcv;
    float* _pkeytrack;
    uint8_t* _pgate;
    uint8_t* _ptrig;
    uint8_t* _pcomgate;
    uint8_t* _pcomtrig;
    uint8_t _gateup;
    uint8_t _notesmem [MAXNOTESMEM]; //памть нот
    uint8_t _mempointer;
    uint8_t _notesout [4];
    uint8_t _note;
    uint8_t _state;
    
    typedef void(tKBD::*pFunc)(void);
    pFunc _getCvGateFunc; //указатель на выбранную функцию
    pFunc _setNotesMemFunc; //указатель на выбранную функцию

};

#endif /* KBD_H_ */