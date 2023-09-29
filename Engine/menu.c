#include "menu.h"

#include "display.h"
//#include "usb_device.h"
//#include "usbd_midi.h"
#include "usbd_midi_if.h"
#include "presetmngr.h"
#include "midi.h"

//#define PRESSED_PERIOD 150
#define PRESETSWITCH_DELAY 0
#define PRESETDUMP_DELAY 16 //������ INDICATE_PERIOD

#define PRESETSCNT 120


struct
{
  uint8_t jump;
  uint8_t save;
  uint8_t midi;
  uint8_t savenew;
  uint8_t jumpnew;
  uint8_t midinew;
  //uint32_t jumptime;
  //uint32_t savetime;
  //uint32_t miditime;
} btnstate = {0,0,0,0,0,0};//,0,0,0};
  

struct
{
  uint16_t val;
  uint16_t newval;
  uint32_t time;
} encoder = {0,0,0};

enum MenuStates 
{ 
  PresetSwitch = 0,
  PresetSave,
  MidiSwitch,
  MidiSave
};

  
uint16_t dispval = 0;
uint16_t presetnum = 0;
//uint16_t prevpresetnum = 0;
MenuStates state = PresetSwitch;
MenuStates prevstate = PresetSwitch;
//uint8_t savemode = 0;
//uint8_t midimode = 0;

  //ENCODERTOP (PRESETSCNT);
  //ENCODERSET (presetnum);
  //Menu_SwitchPreset (presetnum);


void EncoderSwitched(void)
{
  switch (state)
  {
    case PresetSwitch:
      presetnum = encoder.val;
      dispval = presetnum;
      PM_ReadWrite(presetnum, EEP_RD);  //��������� ������
      PM_ApplyPreset();
      break;
      
    case MidiSwitch:
      dispval = encoder.val + 1;
      midi_channel = encoder.val;
      break;
      
    case PresetSave:
      dispval = encoder.val;
      break;
      
    case MidiSave:
      dispval = encoder.val + 1;
      break;
  }
}


void EncoderStopped(void)
{
  switch (state)
  {
    case PresetSwitch:
      PM_ReadWriteCommon((uint8_t*)&presetnum, LASTPRESET_EEPADDR, 2, EEP_WR);
      PM_DumpPreset();              //��������� ����� SysEx ������� 
      USBD_SendMidiMessages();      //�������� ������ ����� (64 �����) � USB
      break;
  }
}


void SavePressed(void)
{
  if (btnstate.midi)
  {
    PM_DumpPreset();              //��������� ����� SysEx ������� 
    USBD_SendMidiMessages();      //�������� ������ ����� (64 �����) � USB
  }
  
  switch (state)
  {
    case PresetSwitch:
      Display_Blink(1);
      state = PresetSave;
      break;
      
    case PresetSave:
      presetnum = encoder.val;
      PM_ReadWrite(presetnum, EEP_WR);    //��������� ������ � EEP
      PM_ReadWriteCommon((uint8_t*)&presetnum, LASTPRESET_EEPADDR, 2, EEP_WR);
      Display_Blink(0);                 //������� ������� �� ������ �������
      state = PresetSwitch;
      break;
      
    case MidiSwitch:
      encoder.val = encoder.newval = presetnum;
      ENCODERTOP(PRESETSCNT);         //������ ������� ������ ��������
      ENCODERSET(presetnum);          //������������� �������� �������� �������
      dispval = presetnum;
      Display_Mode(DISPALPHNUM);      //��������� ������� �� ����������� �����+�����
      state = PresetSwitch;
      break;
      
    case MidiSave:
      encoder.val = encoder.newval = midi_channel;
      ENCODERSET(midi_channel);
      dispval = midi_channel + 1;
      Display_Blink(0);
      state = MidiSwitch;
      break;
  }
}


void MidiPressed(void)
{
  switch (state)
  {  
    case PresetSwitch:
      presetnum = encoder.val;
      encoder.val = encoder.newval = midi_channel;
      ENCODERTOP(15);                   //������ ������� ������ ��������
      ENCODERSET(midi_channel);         //������������� �������� �������� �������
      dispval = midi_channel + 1;
      Display_Mode(DISPNUM);
      state = MidiSwitch;
      break;
      
    case PresetSave:
      encoder.val = encoder.newval = presetnum;
      ENCODERSET(presetnum);
      dispval = presetnum;
      Display_Blink(0);                 //������� ������� �� ������ �������
      state = PresetSwitch;
      break;
    
    case MidiSwitch:
      midi_channel = encoder.val;
      Display_Blink(1);                 //������� ������� �� ������ �������
      state = MidiSave;
      break;
      
    case MidiSave:
      midi_channel = encoder.val;
      ENCODERSET(midi_channel);         //������������� �������� �������� �������
      PM_ReadWriteCommon(&midi_channel, MIDICH_EEPADDR, 1, EEP_WR);
      dispval = midi_channel + 1;
      Display_Blink(0);
      state = MidiSwitch;
      break;
  }
}


void Jump10(void)
{
  if (encoder.newval < PRESETSCNT - 10)
    encoder.newval += 10;
  else
    encoder.newval = encoder.newval - (PRESETSCNT - 10);
  ENCODERSET(encoder.newval);
}


void JumpPressed(void)
{  
  switch (state)
  {
    case PresetSwitch:
      Jump10();
      presetnum = encoder.val;
      dispval = presetnum;
      PM_ReadWrite(presetnum, EEP_RD);  //��������� ������
      PM_ApplyPreset();
      break;
      
    case PresetSave:
      Jump10();
      dispval = encoder.val;
      break;
  }
}


void Menue_LoadPreset(uint8_t preset)
{
  encoder.val = encoder.newval = presetnum = preset;
  ENCODERTOP(PRESETSCNT);         //������ ������� ������ ��������
  ENCODERSET(presetnum);          //������������� �������� �������� �������
  
  PM_ReadWrite(presetnum, EEP_RD);  //��������� ������
  PM_ApplyPreset();
  
  dispval = presetnum;
  if (state != PresetSwitch)
  {
    Display_Mode(DISPALPHNUM);      //��������� ������� �� ����������� �����+�����
    Display_Blink(0);
    state = PresetSwitch;
  }
}


void Menue_SavePreset(uint8_t preset)
{
  encoder.val = encoder.newval = presetnum = preset;
  ENCODERTOP(PRESETSCNT);         //������ ������� ������ ��������
  ENCODERSET(presetnum);          //������������� �������� �������� �������
  
  PM_ReadWrite(presetnum, EEP_WR);    //��������� ������ � EEP
  
  dispval = presetnum; 
  if (state != PresetSwitch)
  {
    Display_Mode(DISPALPHNUM);      //��������� ������� �� ����������� �����+�����
    Display_Blink(0);
    state = PresetSwitch;
  }
}



void Menu_Init(void)
{
  //�������������� �������
  ENCODERTOP (PRESETSCNT);
  ENCODERSET (dispval);
  //prevpresetnum = dispval;
  Display_Init(&dispval);
}


void Menu_Update(void)
{
  //���������� �������
  encoder.newval = ENCODERGET;
 
  //���������� ������ 
  btnstate.jumpnew = READSTATE( JUMPBTN );
  btnstate.savenew = READSTATE( SAVEBTN );
  btnstate.midinew = READSTATE( MIDIBTN );
  
 
  //���� �������� �������� ���������� -----------------------------------------
  if (encoder.val != encoder.newval)
  {
    encoder.val = encoder.newval;
    encoder.time = 0;                   //���������� ������� ������� ���������
    EncoderSwitched();
  }
  else                                  //���� �������� �������� �� ��������
  {
    if (encoder.time == PRESETDUMP_DELAY)
    {
      EncoderStopped();
    }     
    encoder.time++;                     //�������������� ������� ���������
  }
  
  
  //���������� ������ JUMP ----------------------------------------------------
  if (btnstate.jump != btnstate.jumpnew)
  {
    btnstate.jump = btnstate.jumpnew;        
    if (btnstate.jump)      //���� ������
    {  
      JumpPressed();
    }       
  }
  
  
  //���������� ������ SAVE ----------------------------------------------------
  if (btnstate.save != btnstate.savenew)    //���� ������ ������ ���������
  {
    btnstate.save = btnstate.savenew;       //��������� ������
    if (btnstate.save)                      //������� ������ save
    {
      SavePressed();
    }       
  }
  
  
  //���������� ������ MIDI ----------------------------------------------------
  if (btnstate.midi != btnstate.midinew)    //���� ������ ������ ���������
  {   
    btnstate.midi = btnstate.midinew;       //��������� ������
    if (btnstate.midi)                      //������� ������ midi
    {
      MidiPressed();
    } 
  }
  
}
