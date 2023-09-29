#include "midi.h"

#include "engine.h"
#include "lfo.h"
#include "presetmngr.h"


#define MAXLENGTH 4
//#define KBDLENGTH HNOTE-LONOTE
 
/*
#define MIDI_NOTEON     0x90
#define MIDI_NOTEOFF    0x80
#define MIDI_CC         0xB0
#define MIDI_PITCHBEND  0xE0
#define MIDI_SYSEXSTART 0xF0
#define MIDI_CLOCK      0xF8
#define MIDI_PLAY       0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC

#define MIDI_SYSEXEND   0xF7
#define MIDI_SYSEXID    0x7D
*/

// go to next byte
#define NEXTBYTE(idx, mask) (mask & (idx + 1))

uint8_t midi_channel = 0;
uint8_t midi_devicenum = (DEVICEID << 4) | midi_channel;
uint8_t midi_pbrange = 2;


TIM_HandleTypeDef* psynctimer;


void midi_Sync_Init(TIM_HandleTypeDef* htim)
{
  psynctimer = htim;
}


void midi_SetChannel(uint8_t channel)
{
  midi_channel = 0x0F & channel;
  midi_devicenum = (DEVICEID << 4) | midi_channel;
}


void midi_Parse(tMidiStore* pstore, uint8_t* buf, uint16_t* pread, uint16_t* pcurrent, const uint16_t mask)
{
  //static uint16_t 	idx = *pread;
  //static uint8_t	state = 0; //uint8_t* statestore !!!!!!!!!!!!!!
  uint8_t		MIDI_byte;

  // Process message
  while (*pread != *pcurrent)
  {
    // Read a new byte from the MIDI buffer
    MIDI_byte = buf[*pread];
    
    if (MIDI_byte > 0xF7) //System Real Time Messages
    {
      
      switch (MIDI_byte)
      {
        
        case MIDI_CLOCK:
        {
          //здесь вызвать функцию миди клок
          LFO_SyncTick( psynctimer->Instance->CNT );
          psynctimer->Instance->CNT = 0;
          //здесь вызвать функцию перезапуска лфо
          //HAL_GPIO_TogglePin(LED_PORT, GPIO_PIN_12);
          break;
        }
        
        case MIDI_PLAY:
        case MIDI_CONTINUE:
        {
          //здесь вызвать функцию перезапуска лфо
          LFO_SyncStart();
          //HAL_GPIO_WritePin(LED_PORT, LED_GREEN, (GPIO_PinState)1);
          break;
        }
      }
      
      *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
    }
    else
    {
    
      // Move to next byte
      switch (pstore->MIDI_state) 
      {
        
        // State 0 = Starting point for a new MIDI message
        case 0 :
        {
          
          switch (MIDI_byte & 0xF0)
          {
            
            // Note ON message
            case MIDI_NOTEON :
            {
              if ( (MIDI_byte & 0x0F) == midi_channel) //if this device channel
                pstore->MIDI_state = 10;  // Next state is 10
              break;
            }

            // Note OFF message
            case MIDI_NOTEOFF :
            {
              if ( (MIDI_byte & 0x0F) == midi_channel) //if this device channel
                pstore->MIDI_state = 20;  // Next state is 20
              break;
            }

            // CC message
            case MIDI_CTRLCHANGE:
            {
              if ( (MIDI_byte & 0x0F) == midi_channel) //if this device channel
                pstore->MIDI_state = 30;  // Next state is 30
              break;
            }

            // Pitch Bend message
            case MIDI_PITCHBEND :
            {
              if ( (MIDI_byte & 0x0F) == midi_channel) //if this device channel
                pstore->MIDI_state = 40;  // Next state is 40
              break;
            }
            
            // Aftertouch message
            case MIDI_AFTERTOUCH :
            {
              if ( (MIDI_byte & 0x0F) == midi_channel) //if this device channel
                pstore->MIDI_state = 50;  // Next state is 50
              break;
            }
            
            // Programm Change message
            case MIDI_PROGCHANGE :
            {
              if ( (MIDI_byte & 0x0F) == midi_channel) //if this device channel
                pstore->MIDI_state = 60;  // Next state is 60
              break;
            }
            
            // SysEx Start message
            case MIDI_SYSEXSTART :
            {
              if (MIDI_byte != MIDI_SYSEXEND)
                pstore->MIDI_state = 100;  // Next state is 100
              break;
            }
          }
          
          *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
          break;
        };


        // State 10 & 11 : Note ON command
        case 10 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_note = MIDI_byte;  // Save MIDI note
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 11;  // Next state is 11
          }
          break;
        }

        case 11:
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_velocity = MIDI_byte;  // Save MIDI velocity
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 10;  // Next state is 10
            if (pstore->MIDI_velocity > 0)
            {
              
              // ******************************************** NOTE ON FUNC *****************************
              if ((pstore->MIDI_note<=HNOTE) && (pstore->MIDI_note>=LNOTE))
                Engine_SetNote(pstore->MIDI_note, 1);
                Engine_SetVelocity(pstore->MIDI_velocity);
                //Engine_SetKeyTrack();
            }
            else
            {
              
              // ******************************************** NOTE OFF FUNC ****************************        
              if ((pstore->MIDI_note<=HNOTE) && (pstore->MIDI_note>=LNOTE))
                Engine_SetNote(pstore->MIDI_note, 0);
              
            }
          }
          break;
        }

        
        // State 20 & 21 : Note OFF command
        case 20 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_note = MIDI_byte;  // Save MIDI note
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 21;  // Next state is 21
          }
          break;
        }

        case 21 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_velocity = MIDI_byte;  // Save MIDI velocity
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 20;  // Next state is 20
          
            // ******************************************** NOTE OFF FUNC ****************************  
            if ((pstore->MIDI_note<=HNOTE) && (pstore->MIDI_note>=LNOTE))
              Engine_SetNote(pstore->MIDI_note, 0);
            
          }
          break;
        }


        // State 30 & 31 : CC command
        case 30 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_CC_number = MIDI_byte;  // Save MIDI CC number
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 31;  // Next state is 31
          }
          break;
        }

        case 31 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_CC_value = MIDI_byte;  // // Save MIDI CC value
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 30;  // Next state is 30
          }
          
          // ******************************************** CC CHANGED FUNC ****************************
          if (pstore->MIDI_CC_number == MIDI_WHEELMSB) Engine_SetModWheel(pstore->MIDI_CC_value);
          //ChangeParam(MIDI_CC_number, MIDI_CC_value);
          break;
        }
        


        // State 40 & 41 : Pitch Bend message
        case 40 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_PB_byte1 = MIDI_byte;  // Save MIDI Pitch Bend LSB
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 41;  // Next state is 41
          }
          break;
        }

        case 41 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_PB_byte2 = MIDI_byte;  // Save MIDI Pitch Bend MSB
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 40;  // Next state is 40
            
            // ******************************************** PITCH BEND CHANGED FUNC ****************************
            DCO_SetPitchBend( ((uint16_t)pstore->MIDI_PB_byte2 << 7 ) + pstore->MIDI_PB_byte1 );
          }
          break;
        }
        
        
        
        // State 50: Aftertouch command
        case 50 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_Aftertouch = MIDI_byte;  // Save MIDI note
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 50;  // Next state is 50
            //************************************************ AFTERTOUCH *****************************
            Engine_SetAftertouch(pstore->MIDI_Aftertouch);
          }
          break;
        }
        
        
        
        // State 60: Aftertouch command
        case 60 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_ProgNum = MIDI_byte;  // Save MIDI note
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 60;  // Next state is 60
            //************************************************ PROGRAMM CHANGE ************************
            //(pstore->MIDI_ProgNum);
          }
          break;
        }
        
        
        
        // State 100 & 10x : SysEx message
        case 100 :
        {
          if (MIDI_byte != MIDI_SYSEXID)  // If the following byte is not a this device number
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 101;
          }
          break;
        }
        
        case 101 :
        {
          if (MIDI_byte != midi_devicenum)  // If the following byte is not a this device number
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 102;
          }
          break;
        }
        
        case 102 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_SysEx_module = MIDI_byte;
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 103;
          }
          break;
        }

        case 103 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_SysEx_ctrl = MIDI_byte;
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 104;
          }
          break;
        }
        
        case 104 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_SysEx_byte1 = MIDI_byte;
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 105;
          }
          break;
        }
        
        case 105 :
        {
          if (MIDI_byte > 0x7F)  // If the following byte is not a data
          {
            pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          }
          else
          {
            pstore->MIDI_SysEx_byte2 = MIDI_byte;
            *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
            pstore->MIDI_state = 106;
          }
          break;
        }
        
        // State 106 : SysEx End
        case 106 :
        {
          if (MIDI_byte == MIDI_SYSEXEND)  // If the following byte is not a data
          {          
            // ******************************************** SysEx CHANGED FUNC ****************************
            // MIDI_SysEx_module, MIDI_SysEx_ctrl, MIDI_SysEx_byte1, MIDI_SysEx_byte2
            //Engine_SetParam(pstore->MIDI_SysEx_module, pstore->MIDI_SysEx_ctrl, (pstore->MIDI_SysEx_byte1 << 7) + pstore->MIDI_SysEx_byte2 );
            PM_SetParam(pstore->MIDI_SysEx_module, pstore->MIDI_SysEx_ctrl, (pstore->MIDI_SysEx_byte1 << 7) + pstore->MIDI_SysEx_byte2 );
          }
          *pread = NEXTBYTE(*pread, mask);  // Move to next MIDI byte
          pstore->MIDI_state = 0;  // Return to state 0 without moving to next byte
          break;
        }
        
      }
      
    }
  }
  
  //return idx;
};

//ctrlvoltage=(float)note+(float)(pbrange*(pitchbend-8192)) / 8192.0;