#include "presetmngr.h"

#include "stdint.h"

#include "engine.h"
#include "matrix.h"
#include "kbd.h"
#include "dco.h"
#include "hadsr.h"
#include "svf.h"
//#include "lpf.h"
#include "vibrato.h"
#include "LFO.h"

#include "midi.h"
//#include "menu.h"
#include "usbd_midi_if.h" //ѕ≈–≈ƒ≈Ћј“№


#define MIDILOBYTE(x)  ((uint8_t)(x & 0x007F)) //младшие 7 бит
#define MIDIHIBYTE(x)  ((uint8_t)((x & 0x0F80) >> 7)) //старшие 5 бит

// SysEx Start, Vender ID, Device ID / Midi CH,
// Module Type / Submodule ID, Controller ID, MSB 5bit, LSB 7bit, SysEx End
// 0xF0, 0x7D, 0, 0, 0, 0, 0, 0xF7
uint8_t sysexmsg[SYSEXMSGSIZE] = {MIDI_SYSEXSTART, MIDI_SYSEXID, 0,
                                  0, 0, 0, 0, MIDI_SYSEXEND};


struct tFaceMem
{
  uint16_t kbdMode;
  uint16_t kbdGateUp;
  
  uint16_t dco1Semitones;
  uint16_t dco1Detune;
  uint16_t dco1WaveMode;
  uint16_t dco1Retrig;
  uint16_t dco1WaveShape;
  uint16_t dco1Phase;
  uint16_t dco1FMDepts;
  
  uint16_t dco2Semitones;
  uint16_t dco2Detune;
  uint16_t dco2WaveMode;
  uint16_t dco2Retrig;
  uint16_t dco2WaveShape;
  uint16_t dco2Phase;
  uint16_t dco2FMDepts;
  
  uint16_t dco3Semitones;
  uint16_t dco3Detune;
  uint16_t dco3WaveMode;
  uint16_t dco3Retrig;
  uint16_t dco3WaveShape;
  uint16_t dco3Phase;
  uint16_t dco3FMDepts;
  
  uint16_t dco4Semitones;
  uint16_t dco4Detune;
  uint16_t dco4WaveMode;
  uint16_t dco4Retrig;
  uint16_t dco4WaveShape;
  uint16_t dco4Phase;
  uint16_t dco4FMDepts;
  
  uint16_t dcosPortaTime;
  uint16_t dcosLegato;
  uint16_t dcosEnvA;
  uint16_t dcosEnvD;
  uint16_t dcosEnvS;
  uint16_t dcosEnvR;
  uint16_t dcosEnvRetrig;
  uint16_t dcosPBRange;

  uint16_t level1; 
  uint16_t level2;
  uint16_t level3;
  uint16_t level4;
  uint16_t balance;
  
  uint16_t fltACutoff;
  uint16_t fltAResonance;
  uint16_t fltAMode;
  uint16_t fltAEGSource;
  uint16_t fltAAmount;
  
  uint16_t fltBCutoff;
  uint16_t fltBResonance;
  uint16_t fltBMode;
  uint16_t fltBEGSource;
  uint16_t fltBAmount;

  uint16_t eg1Hold;
  uint16_t eg1Attack;
  uint16_t eg1Decay;
  uint16_t eg1Sustain;
  uint16_t eg1Release;
  uint16_t eg1Mode;
  
  uint16_t eg2Hold;
  uint16_t eg2Attack;
  uint16_t eg2Decay;
  uint16_t eg2Sustain;
  uint16_t eg2Release;
  uint16_t eg2Mode;

  uint16_t vibratoRate;

  uint16_t lfoRate;
  uint16_t lfoSlope;
  uint16_t lfoForm;
  uint16_t lfoRetrig;
  uint16_t lfoSync;
  
  uint16_t matrixSrc1;
  uint16_t matrixCtrl1;
  uint16_t matrixDest1;
  uint16_t matrixAmount1;
  
  uint16_t matrixSrc2;
  uint16_t matrixCtrl2;
  uint16_t matrixDest2;
  uint16_t matrixAmount2;
  
  uint16_t matrixSrc3;
  uint16_t matrixCtrl3;
  uint16_t matrixDest3;
  uint16_t matrixAmount3;
  
  uint16_t matrixSrc4;
  uint16_t matrixCtrl4;
  uint16_t matrixDest4;
  uint16_t matrixAmount4;
  
} faceMem; //81+47 = 128


//uint16_t com_midichannel;
//const uint16_t midichannel_eepaddr = 0x0000;
//uint16_t com_lastpreset;
//const uint16_t com_lastpreset_eepaddr = 0x0002;


//static I2C_HandleTypeDef* hi2c;



//инициализируем настройки по умолчанию и читаем 0-й пресет
/*void PM_Init(I2C_HandleTypeDef* pi2c)
{
  //hi2c = pi2c;
  
  PM_ReadWriteCommon(&midi_channel, MIDICH_EEPADDR, 1, EEP_RD);
  //PM_ReadWriteCommon((uint8_t*)&presetnum, LASTPRESET_EEPADDR, 2, EEP_RD);
  //Menue_LoadPreset(presetnum);
  //PM_ReadWrite (0, EEP_RD);
  //PM_ApplyPreset();
}*/



/*void PM_SwitchPreset(uint8_t number)
{
  PM_ReadWrite(number, EEP_RD);  //загружаем пресет
  PM_ApplyPreset();
}*/



/*void PM_ReadWriteCommon(uint8_t* var, uint16_t eepaddr, uint8_t size, EEP_Operation eepOperation)
{
  if (eepOperation)
  {
    HAL_I2C_Mem_Write(hi2c, (uint16_t)EEPROM_I2C_ADDRESS<<1, eepaddr, I2C_MEMADD_SIZE_16BIT,
                      var, size, EEPROM_TIMEOUT);
    //HAL_Delay(EEPROM_WRDELAY);
  }
  else
  {
    HAL_I2C_Mem_Read (hi2c, (uint16_t)EEPROM_I2C_ADDRESS<<1, eepaddr, I2C_MEMADD_SIZE_16BIT,
                      var, size, EEPROM_TIMEOUT);
  }
}*/
                      


//чтение / запись EEP
/*void PM_ReadWrite(uint8_t presetNumber, EEP_Operation eepOperation)
{

  presetNumber++; //резервируем первый пресет дл€ общих настроек
  
  uint8_t* p = (uint8_t*)&faceMem;

  for(uint16_t i = 0, len = sizeof(faceMem); i < sizeof(faceMem); i += EEPROM_BLOCK_SIZE, len -= EEPROM_BLOCK_SIZE)
  {
    if (eepOperation)
    {
      HAL_I2C_Mem_Write(hi2c, (uint16_t)EEPROM_I2C_ADDRESS<<1, i + (uint16_t)presetNumber * PRESET_SIZE, I2C_MEMADD_SIZE_16BIT,
                        p + i, (len > EEPROM_BLOCK_SIZE) ? EEPROM_BLOCK_SIZE : len, EEPROM_TIMEOUT);
      HAL_Delay(EEPROM_WRDELAY);
    }
    else
    {
      HAL_I2C_Mem_Read(hi2c, (uint16_t)EEPROM_I2C_ADDRESS<<1, i + (uint16_t)presetNumber * PRESET_SIZE, I2C_MEMADD_SIZE_16BIT,
                       p + i, (len > EEPROM_BLOCK_SIZE) ? EEPROM_BLOCK_SIZE : len, EEPROM_TIMEOUT);
    }
  }
}*/



void AddSysEx(const uint8_t mdlid, const uint8_t ctrlid, uint16_t ctrlval)
{  
  sysexmsg[3] = mdlid;
  sysexmsg[4] = ctrlid;
  sysexmsg[5] = MIDIHIBYTE(ctrlval);
  sysexmsg[6] = MIDILOBYTE(ctrlval);
  USBD_AddSysExMessage(0, sysexmsg, SYSEXMSGSIZE);
}


void PM_DumpPreset(void)
{
  //sysexmsg[2] = midi_channel;
  
  AddSysEx(0x01, 0x00, faceMem.kbdMode);
  AddSysEx(0x01, 0x01, faceMem.kbdGateUp);

  AddSysEx(0x10, 0x00, faceMem.dco1Semitones);
  AddSysEx(0x10, 0x01, faceMem.dco1Detune);
  AddSysEx(0x10, 0x02, faceMem.dco1WaveMode);
  AddSysEx(0x10, 0x03, faceMem.dco1Retrig);
  AddSysEx(0x10, 0x04, faceMem.dco1WaveShape);
  AddSysEx(0x10, 0x05, faceMem.dco1Phase);
  AddSysEx(0x10, 0x06, faceMem.dco1FMDepts);
  
  AddSysEx(0x11, 0x00, faceMem.dco2Semitones);
  AddSysEx(0x11, 0x01, faceMem.dco2Detune);
  AddSysEx(0x11, 0x02, faceMem.dco2WaveMode);
  AddSysEx(0x11, 0x03, faceMem.dco2Retrig);
  AddSysEx(0x11, 0x04, faceMem.dco2WaveShape);
  AddSysEx(0x11, 0x05, faceMem.dco2Phase);
  AddSysEx(0x11, 0x06, faceMem.dco2FMDepts);
  
  AddSysEx(0x12, 0x00, faceMem.dco3Semitones);
  AddSysEx(0x12, 0x01, faceMem.dco3Detune);
  AddSysEx(0x12, 0x02, faceMem.dco3WaveMode);
  AddSysEx(0x12, 0x03, faceMem.dco3Retrig);
  AddSysEx(0x12, 0x04, faceMem.dco3WaveShape);
  AddSysEx(0x12, 0x05, faceMem.dco3Phase);
  AddSysEx(0x12, 0x06, faceMem.dco3FMDepts);
  
  AddSysEx(0x13, 0x00, faceMem.dco4Semitones);
  AddSysEx(0x13, 0x01, faceMem.dco4Detune);
  AddSysEx(0x13, 0x02, faceMem.dco4WaveMode);
  AddSysEx(0x13, 0x03, faceMem.dco4Retrig);
  AddSysEx(0x13, 0x04, faceMem.dco4WaveShape);
  AddSysEx(0x13, 0x05, faceMem.dco4Phase);
  AddSysEx(0x13, 0x06, faceMem.dco4FMDepts);

  AddSysEx(0x1F, 0x07, faceMem.dcosPortaTime);
  AddSysEx(0x1F, 0x08, faceMem.dcosLegato);
  AddSysEx(0x1F, 0x09, faceMem.dcosEnvA);
  AddSysEx(0x1F, 0x0A, faceMem.dcosEnvD);
  AddSysEx(0x1F, 0x0B, faceMem.dcosEnvS);
  AddSysEx(0x1F, 0x0C, faceMem.dcosEnvR);
  AddSysEx(0x1F, 0x0E, faceMem.dcosEnvRetrig);  ///!!!!!!!!!!!!!
  AddSysEx(0x1F, 0x0D, faceMem.dcosPBRange);

  AddSysEx(0x20, 0x00, faceMem.level1); 
  AddSysEx(0x20, 0x01, faceMem.level2); 
  AddSysEx(0x20, 0x02, faceMem.level3); 
  AddSysEx(0x20, 0x03, faceMem.level4); 
  AddSysEx(0x20, 0x04, faceMem.balance);
  
  AddSysEx(0x30, 0x00, faceMem.fltACutoff);
  AddSysEx(0x30, 0x01, faceMem.fltAResonance);
  AddSysEx(0x30, 0x02, faceMem.fltAMode); 
  AddSysEx(0x30, 0x03, faceMem.fltAEGSource);
  AddSysEx(0x30, 0x04, faceMem.fltAAmount);
  
  AddSysEx(0x31, 0x00, faceMem.fltBCutoff);
  AddSysEx(0x31, 0x01, faceMem.fltBResonance);
  AddSysEx(0x31, 0x02, faceMem.fltBMode);
  AddSysEx(0x31, 0x03, faceMem.fltBEGSource);
  AddSysEx(0x31, 0x04, faceMem.fltBAmount);
  
  AddSysEx(0x40, 0x00, faceMem.eg1Hold);
  AddSysEx(0x40, 0x01, faceMem.eg1Attack);
  AddSysEx(0x40, 0x02, faceMem.eg1Decay);
  AddSysEx(0x40, 0x03, faceMem.eg1Sustain);
  AddSysEx(0x40, 0x04, faceMem.eg1Release);
  AddSysEx(0x40, 0x05, faceMem.eg1Mode);
  
  AddSysEx(0x41, 0x00, faceMem.eg2Hold);
  AddSysEx(0x41, 0x01, faceMem.eg2Attack);
  AddSysEx(0x41, 0x02, faceMem.eg2Decay);
  AddSysEx(0x41, 0x03, faceMem.eg2Sustain);
  AddSysEx(0x41, 0x04, faceMem.eg2Release);
  AddSysEx(0x41, 0x05, faceMem.eg2Mode);
  
  AddSysEx(0x50, 0x00, faceMem.vibratoRate);

  AddSysEx(0x51, 0x00, faceMem.lfoRate);
  AddSysEx(0x51, 0x01, faceMem.lfoSlope);
  AddSysEx(0x51, 0x02, faceMem.lfoForm);
  AddSysEx(0x51, 0x03, faceMem.lfoRetrig);
  AddSysEx(0x51, 0x04, faceMem.lfoSync);

  AddSysEx(0x70, 0x00, faceMem.matrixSrc1);
  AddSysEx(0x70, 0x01, faceMem.matrixCtrl1);
  AddSysEx(0x70, 0x02, faceMem.matrixDest1);
  AddSysEx(0x70, 0x03, faceMem.matrixAmount1);
  
  AddSysEx(0x71, 0x00, faceMem.matrixSrc2);
  AddSysEx(0x71, 0x01, faceMem.matrixCtrl2);
  AddSysEx(0x71, 0x02, faceMem.matrixDest2);
  AddSysEx(0x71, 0x03, faceMem.matrixAmount2);
  
  AddSysEx(0x72, 0x00, faceMem.matrixSrc3);
  AddSysEx(0x72, 0x01, faceMem.matrixCtrl3);
  AddSysEx(0x72, 0x02, faceMem.matrixDest3);
  AddSysEx(0x72, 0x03, faceMem.matrixAmount3);
  
  AddSysEx(0x73, 0x00, faceMem.matrixSrc4);
  AddSysEx(0x73, 0x01, faceMem.matrixCtrl4);
  AddSysEx(0x73, 0x02, faceMem.matrixDest4);
  AddSysEx(0x73, 0x03, faceMem.matrixAmount4);
  
  //USBD_SendMidiMessages();
}



//усправление параметрами синтеза
void PM_SetParam (uint8_t module, uint8_t controller, uint16_t value)
{
  
  switch(module >> 4) //switch module types
  {
    
    case 0x0: //master controlls
    {  
      switch(module & 0x0F)
      {
        
        /*case 0xF: //Common
        {    
          
          switch(controller)
          {   
            
          }
          
          break;
        }*/
        
        case 0x1: //KBD
        {  
          
          switch(controller)
          {   
            case 0x00:
              pkbd->setMode(value);
              faceMem.kbdMode = value;
              break;
            case 0x01:
              pkbd->setGateUp(value);
              faceMem.kbdGateUp = value;
              break;
          }
          
          break;
        }
        
      }  
    }
    break;
    
    //***
    
    case 0x1: //DCO
    {     
      switch(module & 0x0F)
      {       
        case 0x0: //DC01
        {        
          switch(controller)
          {
            case 0x00:
              pdco1->setSemitones(value);
              faceMem.dco1Semitones = value;
              break;
            case 0x01:
              pdco1->setDetune(value);
              faceMem.dco1Detune = value;
              break;
            case 0x02:
              pdco1->setWaveMode(value);
              faceMem.dco1WaveMode = value;
              break;
            case 0x03:
              pdco1->setRetrig(value);
              faceMem.dco1Retrig = value;
              break;
            case 0x04:
              pdco1->setWaveShape(value);
              faceMem.dco1WaveShape = value;
              break;
            case 0x05:
              pdco1->setPhase(value);
              faceMem.dco1Phase = value;
              break;
            case 0x06:
              pdco1->setFMDepts(value);
              faceMem.dco1FMDepts = value;
              break;
          }     
        }
        break;
        
        case 0x1: //DC02
        {       
          switch(controller)
          {
            case 0x00:
              pdco2->setSemitones(value);
              faceMem.dco2Semitones = value;
              break;
            case 0x01:
              pdco2->setDetune(value);
              faceMem.dco2Detune = value;
              break;
            case 0x02:
              pdco2->setWaveMode(value);
              faceMem.dco2WaveMode = value;
              break;
            case 0x03:
              pdco2->setRetrig(value);
              faceMem.dco2Retrig = value;
              break;
            case 0x04:
              pdco2->setWaveShape(value);
              faceMem.dco2WaveShape = value;
              break;
            case 0x05:
              pdco2->setPhase(value);
              faceMem.dco2Phase = value;
              break;
            case 0x06:
              pdco2->setFMDepts(value);
              faceMem.dco2FMDepts = value;
              break;
          }     
        }
        break;
        
        case 0x2: //DCO3
        {       
          switch(controller)
          {
            case 0x00:
              pdco3->setSemitones(value);
              faceMem.dco3Semitones = value;
              break;
            case 0x01:
              pdco3->setDetune(value);
              faceMem.dco3Detune = value;
              break;
            case 0x02:
              pdco3->setWaveMode(value);
              faceMem.dco3WaveMode = value;
              break;
            case 0x03:
              pdco3->setRetrig(value);
              faceMem.dco3Retrig = value;
              break;
            case 0x04:
              pdco3->setWaveShape(value);
              faceMem.dco3WaveShape = value;
              break;
            case 0x05:
              pdco3->setPhase(value);
              faceMem.dco3Phase = value;
              break;
            case 0x06:
              pdco3->setFMDepts(value);
              faceMem.dco3FMDepts = value;
              break;
          }     
        }
        break;
        
        case 0x3: //DCO4
        {       
          switch(controller)
          {
            case 0x00:
              pdco4->setSemitones(value);
              faceMem.dco4Semitones = value;
              break;
            case 0x01:
              pdco4->setDetune(value);
              faceMem.dco4Detune = value;
              break;
            case 0x02:
              pdco4->setWaveMode(value);
              faceMem.dco4WaveMode = value;
              break;
            case 0x03:
              pdco4->setRetrig(value);
              faceMem.dco4Retrig = value;
              break;
            case 0x04:
              pdco4->setWaveShape(value);
              faceMem.dco4WaveShape = value;
              break;
            case 0x05:
              pdco4->setPhase(value);
              faceMem.dco4Phase = value;
              break;
            case 0x06:
              pdco4->setFMDepts(value);
              faceMem.dco4FMDepts = value;
              break;
          }     
        }
        break;
        
        case 0xF: //DCOS Common Settings
        {
          switch(controller)
          {
            case 0x07:
              DCO_SetPortaTime(value);
              faceMem.dcosPortaTime = value;
              break;
            case 0x08:
              DCO_SetLegato(value);
              faceMem.dcosLegato = value;
              break;
            case 0x09:
              DCO_SetEnvA(value);
              faceMem.dcosEnvA = value;
              break;
            case 0x0A:
              DCO_SetEnvD(value);
              faceMem.dcosEnvD = value;
              break;
            case 0x0B:
              DCO_SetEnvS(value);
              faceMem.dcosEnvS = value;
              break;
            case 0x0C:
              DCO_SetEnvR(value);
              faceMem.dcosEnvR = value;
              break;
            case 0x0E:                          ///!!!!!!!!!!
              DCO_SetEnvRetrig(value);
              faceMem.dcosEnvRetrig = value;
              break;
            case 0x0D:
              DCO_SetPBRange(value);
              faceMem.dcosPBRange = value;
              break;
          }
        }
        break;     
      }
    }
    break;
    
    //***
    
    case 0x2: //Mixer
    {     
      switch(module & 0x0F)
      {       
        case 0x0: //DCO1Mix
        {        
          switch(controller)
          {
            case 0x00:
              Engine_SetMixLevel(0, value);
              faceMem.level1 = value;
            break;
            case 0x01:
              Engine_SetMixLevel(1, value);
              faceMem.level2 = value;
            break;
            case 0x02:
              Engine_SetMixLevel(2, value);
              faceMem.level3 = value;
            break;
            case 0x03:
              Engine_SetMixLevel(3, value);
              faceMem.level4 = value;
            break;
            case 0x04:
              Engine_SetMixBalance(value);
              faceMem.balance = value;
            break;             
          }     
        }
        break;    
      }
    }
    break;
    
    //***
    
    case 0x3: //VCF
    {     
      switch(module & 0x0F)
      {       
        case 0x0: //VCFA
        {        
          switch(controller)
          {
            case 0:
              psvfA->setCutoff(value);
              //plpf->setCutoff(value);
              faceMem.fltACutoff = value;
              break;
            case 1:
              psvfA->setResonance(value);
              //plpf->setResonance(value);
              faceMem.fltAResonance = value;
              break;
            case 2:
              psvfA->setMode(value);
              //plpf->setMode(value);
              faceMem.fltAMode = value;
              break;
            case 3:
              Engine_LinkVCFAEG(value);
              faceMem.fltAEGSource = value;
              break;
            case 4:
              psvfA->setAmount(value);
              //plpf->setAmount(value);
              faceMem.fltAAmount = value;
              break;
          }     
        }
        break;
        
        case 0x1: //VCFB
        {       
          switch(controller)
          {
            case 0:
              psvfB->setCutoff(value);
              //plpf->setCutoff(value);
              faceMem.fltBCutoff = value;
              break;
            case 1:
              psvfB->setResonance(value);
              //plpf->setResonance(value);
              faceMem.fltBResonance = value;
              break;
            case 2:
              psvfB->setMode(value);
              //plpf->setMode(value);
              faceMem.fltBMode = value;
              break;
            case 3:
              Engine_LinkVCFBEG(value);
              faceMem.fltBEGSource = value;
              break;
            case 4:
              psvfB->setAmount(value);
              //plpf->setAmount(value);
              faceMem.fltBAmount = value;
              break;
          }     
        }
        break;     
      }
    }
    break;
    
    //***
    
    case 0x4: //EG
    {     
      switch(module & 0x0F)
      {       
        case 0x0: //EG1
        {        
          switch(controller)
          {
            case 0x00:
              peg1->setHold(value);
              faceMem.eg1Hold = value;
              break;
            case 0x01:
              peg1->setAttack(value);
              faceMem.eg1Attack = value;
              break;
            case 0x02:
              peg1->setDecay(value);
              faceMem.eg1Decay = value;
              break;
            case 0x03:
              peg1->setSustain(value);
              faceMem.eg1Sustain = value;
              break;
            case 0x04:
              peg1->setRelease(value);
              faceMem.eg1Release = value;
              break;
            case 0x05:
              peg1->setMode(value);
              faceMem.eg1Mode = value;
              break;                 
          }     
        }
        break;
        
        case 0x1: //EG2
        {       
          switch(controller)
          {
            case 0x00:
              peg2->setHold(value);
              faceMem.eg2Hold = value;
              break;
            case 0x01:
              peg2->setAttack(value);
              faceMem.eg2Attack = value;
              break;
            case 0x02:
              peg2->setDecay(value);
              faceMem.eg2Decay = value;
              break;
            case 0x03:
              peg2->setSustain(value);
              faceMem.eg2Sustain = value;
              break;
            case 0x04:
              peg2->setRelease(value);
              faceMem.eg2Release = value;
              break;
            case 0x05:
              peg2->setMode(value);
              faceMem.eg2Mode = value;
              break;                 
          }
        }
        break;     
      }
    }
    break;
       
    //***
    
    case 0x5: //Vibrato & LFO
    {     
      switch(module & 0x0F)
      {       
        case 0x0: //Vibrato
        {        
          switch(controller)
          {
            case 0x00:
              pvibrato->setRate(value);
              faceMem.vibratoRate = value;
            break;
          }     
        }
        break;
        
        case 0x1: //LFO
        {       
          switch(controller)
          {
            case 0x00:
              plfo->setRate(value);
              faceMem.lfoRate = value;
            break;
            case 0x01:
              plfo->setSlope(value);
              faceMem.lfoSlope = value;
              break;
            case 0x02:
              plfo->setForm(value);
              faceMem.lfoForm = value;
            break;
            case 0x03:
              plfo->setRetrig(value);
              faceMem.lfoRetrig = value;
            break;
            case 0x04:
              plfo->setMode(value);
              faceMem.lfoSync = value;
            break;
          }
        }
        break;     
      }
    }
    break;
    
    //***
    
    case 0x7: //Modulation Matrix
    {     
      switch(module & 0x0F)
      {       
        case 0x0: //Slot1
        {        
          switch(controller)
          {
            case 0x00:
              pmatrix->connectSrc(0, value);
              faceMem.matrixSrc1 = value;
            break;
            case 0x01:
              pmatrix->connectCtrl(0, value);
              faceMem.matrixCtrl1 = value;
            break;
            case 0x02:
              pmatrix->connectDest(0, value);
              faceMem.matrixDest1 = value;
            break;
            case 0x03:
              pmatrix->setSrcAmount(0, value);
              faceMem.matrixAmount1 = value;
            break;              
          }     
        }
        break;
        
        case 0x1: //Slot2
        {       
          switch(controller)
          {
            case 0x00:
              pmatrix->connectSrc(1, value);
              faceMem.matrixSrc2 = value;
            break;
            case 0x01:
              pmatrix->connectCtrl(1, value);
              faceMem.matrixCtrl2 = value;
            break;
            case 0x02:
              pmatrix->connectDest(1, value);
              faceMem.matrixDest2 = value;
            break;
            case 0x03:
              pmatrix->setSrcAmount(1, value);
              faceMem.matrixAmount2 = value;
            break;               
          }  
        }
        break;
        
        case 0x2: //Slot3
        {       
          switch(controller)
          {
            case 0x00:
              pmatrix->connectSrc(2, value);
              faceMem.matrixSrc3 = value;
            break;
            case 0x01:
              pmatrix->connectCtrl(2, value);
              faceMem.matrixCtrl3 = value;
            break;
            case 0x02:
              pmatrix->connectDest(2, value);
              faceMem.matrixDest3 = value;
            break;
            case 0x03:
              pmatrix->setSrcAmount(2, value);
              faceMem.matrixAmount3 = value;
            break;               
          }  
        }
        break;
        
        case 0x3: //Slot4
        {       
          switch(controller)
          {
            case 0x00:
              pmatrix->connectSrc(3, value);
              faceMem.matrixSrc4 = value;
            break;
            case 0x01:
              pmatrix->connectCtrl(3, value);
              faceMem.matrixCtrl4 = value;
            break;
            case 0x02:
              pmatrix->connectDest(3, value);
              faceMem.matrixDest4 = value;
            break;
            case 0x03:
              pmatrix->setSrcAmount(3, value);
              faceMem.matrixAmount4 = value;
            break;               
          }  
        }
        break;      
      }
    }
    break;
    
    //***
    
  }
}



void PM_ApplyPreset (void)
{
    pkbd->setMode(faceMem.kbdMode);
    pkbd->setGateUp(faceMem.kbdGateUp);
    

    pdco1->setSemitones(faceMem.dco1Semitones);
    pdco1->setDetune(faceMem.dco1Detune);
    pdco1->setWaveMode(faceMem.dco1WaveMode);
    pdco1->setRetrig(faceMem.dco1Retrig);
    pdco1->setWaveShape(faceMem.dco1WaveShape);
    pdco1->setPhase(faceMem.dco1Phase);
    pdco1->setFMDepts(faceMem.dco1FMDepts);
    
    pdco2->setSemitones(faceMem.dco2Semitones);
    pdco2->setDetune(faceMem.dco2Detune);
    pdco2->setWaveMode(faceMem.dco2WaveMode);
    pdco2->setRetrig(faceMem.dco2Retrig);
    pdco2->setWaveShape(faceMem.dco2WaveShape);
    pdco2->setPhase(faceMem.dco2Phase);
    pdco2->setFMDepts(faceMem.dco2FMDepts);
    
    pdco3->setSemitones(faceMem.dco3Semitones);
    pdco3->setDetune(faceMem.dco3Detune);
    pdco3->setWaveMode(faceMem.dco3WaveMode);
    pdco3->setRetrig(faceMem.dco3Retrig);
    pdco3->setWaveShape(faceMem.dco3WaveShape);
    pdco3->setPhase(faceMem.dco3Phase);
    pdco3->setFMDepts(faceMem.dco3FMDepts);
    
    pdco4->setSemitones(faceMem.dco4Semitones);
    pdco4->setDetune(faceMem.dco4Detune);
    pdco4->setWaveMode(faceMem.dco4WaveMode);
    pdco4->setRetrig(faceMem.dco4Retrig);
    pdco4->setWaveShape(faceMem.dco4WaveShape);
    pdco4->setPhase(faceMem.dco4Phase);
    pdco4->setFMDepts(faceMem.dco4FMDepts);

    DCO_SetPortaTime(faceMem.dcosPortaTime);
    DCO_SetLegato(faceMem.dcosLegato);
    DCO_SetEnvA(faceMem.dcosEnvA);
    DCO_SetEnvD(faceMem.dcosEnvD);
    DCO_SetEnvS(faceMem.dcosEnvS);
    DCO_SetEnvR(faceMem.dcosEnvR);
    DCO_SetEnvRetrig(faceMem.dcosEnvRetrig);
    DCO_SetPBRange(faceMem.dcosPBRange);
  
    Engine_SetMixLevel(0,faceMem.level1);
    Engine_SetMixLevel(1,faceMem.level2); 
    Engine_SetMixLevel(2,faceMem.level3); 
    Engine_SetMixLevel(3,faceMem.level4); 
    Engine_SetMixBalance(faceMem.balance);  
    

    psvfA->setCutoff(faceMem.fltACutoff);
    psvfA->setResonance(faceMem.fltAResonance);
    psvfA->setMode(faceMem.fltAMode);
    psvfA->setAmount(faceMem.fltAAmount);
    
    psvfB->setCutoff(faceMem.fltBCutoff);
    psvfB->setResonance(faceMem.fltBResonance);
    psvfB->setMode(faceMem.fltBMode);
    psvfB->setAmount(faceMem.fltBAmount);


    peg1->setHold(faceMem.eg1Hold);
    peg1->setAttack(faceMem.eg1Attack);
    peg1->setDecay(faceMem.eg1Decay);
    peg1->setSustain(faceMem.eg1Sustain);
    peg1->setRelease(faceMem.eg1Release);
    peg1->setMode(faceMem.eg1Mode);
    
    peg2->setHold(faceMem.eg2Hold);
    peg2->setAttack(faceMem.eg2Attack);
    peg2->setDecay(faceMem.eg2Decay);
    peg2->setSustain(faceMem.eg2Sustain);
    peg2->setRelease(faceMem.eg2Release);
    peg2->setMode(faceMem.eg2Mode);

    
    pvibrato->setRate(faceMem.vibratoRate);

    plfo->setRate(faceMem.lfoRate);
    plfo->setSlope(faceMem.lfoSlope);
    plfo->setForm(faceMem.lfoForm);
    plfo->setRetrig(faceMem.lfoRetrig);
    plfo->setMode(faceMem.lfoSync);

    pmatrix->connectSrc(0, faceMem.matrixSrc1);
    pmatrix->connectCtrl(0, faceMem.matrixCtrl1);
    pmatrix->connectDest(0, faceMem.matrixDest1);
    pmatrix->setSrcAmount(0, faceMem.matrixAmount1);
    
    pmatrix->connectSrc(1, faceMem.matrixSrc2);
    pmatrix->connectCtrl(1, faceMem.matrixCtrl2);
    pmatrix->connectDest(1, faceMem.matrixDest2);
    pmatrix->setSrcAmount(1, faceMem.matrixAmount2);
    
    pmatrix->connectSrc(2, faceMem.matrixSrc3);
    pmatrix->connectCtrl(2, faceMem.matrixCtrl3);
    pmatrix->connectDest(2, faceMem.matrixDest3);
    pmatrix->setSrcAmount(2, faceMem.matrixAmount3);
    
    pmatrix->connectSrc(3, faceMem.matrixSrc4);
    pmatrix->connectCtrl(3, faceMem.matrixCtrl4);
    pmatrix->connectDest(3, faceMem.matrixDest4);
    pmatrix->setSrcAmount(3, faceMem.matrixAmount4);
}
