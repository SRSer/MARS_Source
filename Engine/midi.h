#ifndef MIDI_H_
#define MIDI_H_

#ifdef __cplusplus
 extern "C" {
#endif
   

#include "stdint.h"

#include "stm32f4xx_hal.h"
   
   
#define DEVICEID 0x0
   
#define HNOTE 108
#define LNOTE 21

//Header
#define MIDI_NOTEON     0x90
#define MIDI_NOTEOFF    0x80
#define MIDI_CTRLCHANGE 0xB0
#define MIDI_PITCHBEND  0xE0
//#define MIDI_POLYAFTER  0xA0
#define MIDI_AFTERTOUCH 0xD0
#define MIDI_PROGCHANGE 0xC0

//Control Change
#define MIDI_WHEELMSB   0x01
//#define MIDI_WHEELLSB   0x21

//SysEx
#define MIDI_SYSEXSTART 0xF0
#define MIDI_CLOCK      0xF8
#define MIDI_PLAY       0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC
#define MIDI_SYSEXEND   0xF7
#define MIDI_SYSEXID    0x7D


struct tMidiStore
{
   uint8_t  MIDI_state;
   uint8_t  MIDI_note, MIDI_velocity;
   uint8_t  MIDI_CC_number, MIDI_CC_value;
   uint8_t  MIDI_PB_byte1, MIDI_PB_byte2;
   uint8_t  MIDI_Aftertouch;
   uint8_t  MIDI_ProgNum;
   //uint8_t  MIDI_PolyAfter_note, MIDI_PolyAfter_value;
   uint8_t  MIDI_SysEx_module, MIDI_SysEx_ctrl, MIDI_SysEx_byte1, MIDI_SysEx_byte2;
};
   

extern uint8_t midi_channel;
extern uint8_t midi_devicenum;
extern uint8_t midi_pbrange;

//extern uint8_t note;
//extern int16_t pitchbend;
//extern int16_t wheel;
//extern uint8_t aftertouch;

//extern uint8_t gate;
//extern float ctrlvoltage;


void midi_Sync_Init(TIM_HandleTypeDef* htim);

void midi_SetChannel(uint8_t channel);

void midi_Parse(tMidiStore* pstore, uint8_t* buf, uint16_t* pread, uint16_t* pcurrent, const uint16_t mask);

#ifdef __cplusplus
}
#endif
#endif /* MIDI_H_ */
