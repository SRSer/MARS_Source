#ifndef PRESETMNGR_H_
#define PRESETMNGR_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

#define EEPROM_I2C_ADDRESS  0x50   // A0 = A1 = A2 = 0
#define EEPROM_TIMEOUT 100
#define EEPROM_BLOCK_SIZE 128
#define EEPROM_WRDELAY 5
#define PRESET_SIZE 256

#define MIDICH_EEPADDR 0x0000
#define LASTPRESET_EEPADDR 0x0002

#define SYSEXMSGSIZE 8
//#define SYSEXDELAY 1


/*external uint16_t com_lastpreset;
external uint16_t com_midichannel;*/

enum EEP_Operation {EEP_RD = 0, EEP_WR};

void PM_SetParam (uint8_t module, uint8_t controller, uint16_t value); //установить значение параметра по SysEx

void PM_Init (I2C_HandleTypeDef* pi2c);

void PM_ReadWrite (uint8_t presetNumber, const EEP_Operation eepOperation);

void PM_ReadWriteCommon(uint8_t* var, uint16_t eepaddr, uint8_t size, EEP_Operation eepOperation);

void PM_ApplyPreset(void);

void PM_DumpPreset(void);

//void PM_SwitchPreset(uint8_t number);


#endif /* PRESETMNGR_H_ */