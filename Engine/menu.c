#include "menu.h"

#include "display.h"
//#include "usb_device.h"
//#include "usbd_midi.h"
#include "usbd_midi_if.h"
#include "presetmngr.h"
#include "midi.h"

//#define PRESSED_PERIOD 150
#define PRESETSWITCH_DELAY 0
#define PRESETDUMP_DELAY 16 //циклов INDICATE_PERIOD

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
      PM_ReadWrite(presetnum, EEP_RD);  //загружаем пресет
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
      PM_DumpPreset();              //формируем буфер SysEx пресета 
      USBD_SendMidiMessages();      //передаем первый пакет (64 байта) в USB
      break;
  }
}


void SavePressed(void)
{
  if (btnstate.midi)
  {
    PM_DumpPreset();              //формируем буфер SysEx пресета 
    USBD_SendMidiMessages();      //передаем первый пакет (64 байта) в USB
  }
  
  switch (state)
  {
    case PresetSwitch:
      Display_Blink(1);
      state = PresetSave;
      break;
      
    case PresetSave:
      presetnum = encoder.val;
      PM_ReadWrite(presetnum, EEP_WR);    //сохраняем пресет в EEP
      PM_ReadWriteCommon((uint8_t*)&presetnum, LASTPRESET_EEPADDR, 2, EEP_WR);
      Display_Blink(0);                 //выводим дисплей из режима мигания
      state = PresetSwitch;
      break;
      
    case MidiSwitch:
      encoder.val = encoder.newval = presetnum;
      ENCODERTOP(PRESETSCNT);         //меняем верхний предел энкодера
      ENCODERSET(presetnum);          //устанавливаем энкодеру значение пресета
      dispval = presetnum;
      Display_Mode(DISPALPHNUM);      //переводим дисплей на отображение буква+цифра
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
      ENCODERTOP(15);                   //меняем верхний предел энкодера
      ENCODERSET(midi_channel);         //устанавливаем энкодеру значение пресета
      dispval = midi_channel + 1;
      Display_Mode(DISPNUM);
      state = MidiSwitch;
      break;
      
    case PresetSave:
      encoder.val = encoder.newval = presetnum;
      ENCODERSET(presetnum);
      dispval = presetnum;
      Display_Blink(0);                 //выводим дисплей из режима мигания
      state = PresetSwitch;
      break;
    
    case MidiSwitch:
      midi_channel = encoder.val;
      Display_Blink(1);                 //выводим дисплей из режима мигания
      state = MidiSave;
      break;
      
    case MidiSave:
      midi_channel = encoder.val;
      ENCODERSET(midi_channel);         //устанавливаем энкодеру значение пресета
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
      PM_ReadWrite(presetnum, EEP_RD);  //загружаем пресет
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
  ENCODERTOP(PRESETSCNT);         //меняем верхний предел энкодера
  ENCODERSET(presetnum);          //устанавливаем энкодеру значение пресета
  
  PM_ReadWrite(presetnum, EEP_RD);  //загружаем пресет
  PM_ApplyPreset();
  
  dispval = presetnum;
  if (state != PresetSwitch)
  {
    Display_Mode(DISPALPHNUM);      //переводим дисплей на отображение буква+цифра
    Display_Blink(0);
    state = PresetSwitch;
  }
}


void Menue_SavePreset(uint8_t preset)
{
  encoder.val = encoder.newval = presetnum = preset;
  ENCODERTOP(PRESETSCNT);         //меняем верхний предел энкодера
  ENCODERSET(presetnum);          //устанавливаем энкодеру значение пресета
  
  PM_ReadWrite(presetnum, EEP_WR);    //сохраняем пресет в EEP
  
  dispval = presetnum; 
  if (state != PresetSwitch)
  {
    Display_Mode(DISPALPHNUM);      //переводим дисплей на отображение буква+цифра
    Display_Blink(0);
    state = PresetSwitch;
  }
}



void Menu_Init(void)
{
  //инициализируем дисплей
  ENCODERTOP (PRESETSCNT);
  ENCODERSET (dispval);
  //prevpresetnum = dispval;
  Display_Init(&dispval);
}


void Menu_Update(void)
{
  //опрашиваем энкодер
  encoder.newval = ENCODERGET;
 
  //опрашиваем кнопки 
  btnstate.jumpnew = READSTATE( JUMPBTN );
  btnstate.savenew = READSTATE( SAVEBTN );
  btnstate.midinew = READSTATE( MIDIBTN );
  
 
  //если значение энкодера изменилось -----------------------------------------
  if (encoder.val != encoder.newval)
  {
    encoder.val = encoder.newval;
    encoder.time = 0;                   //сбрасываем счетчик времени удержания
    EncoderSwitched();
  }
  else                                  //если значение энкодера не менялось
  {
    if (encoder.time == PRESETDUMP_DELAY)
    {
      EncoderStopped();
    }     
    encoder.time++;                     //инкрементируем счетчик удержания
  }
  
  
  //опрашиваем кнопку JUMP ----------------------------------------------------
  if (btnstate.jump != btnstate.jumpnew)
  {
    btnstate.jump = btnstate.jumpnew;        
    if (btnstate.jump)      //если нажата
    {  
      JumpPressed();
    }       
  }
  
  
  //опрашиваем кнопку SAVE ----------------------------------------------------
  if (btnstate.save != btnstate.savenew)    //если статус кнопки изменился
  {
    btnstate.save = btnstate.savenew;       //обновляем статус
    if (btnstate.save)                      //событие нажата save
    {
      SavePressed();
    }       
  }
  
  
  //опрашиваем кнопку MIDI ----------------------------------------------------
  if (btnstate.midi != btnstate.midinew)    //если статус кнопки изменился
  {   
    btnstate.midi = btnstate.midinew;       //обновляем статус
    if (btnstate.midi)                      //событие нажата midi
    {
      MidiPressed();
    } 
  }
  
}
