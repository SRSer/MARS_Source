#ifndef AUDIOPLAY_H_
#define AUDIOPLAY_H_

#ifdef __cplusplus
 extern "C" {
#endif
//------------------------------------------------
#include "stm32f4xx_hal.h"
//------------------------------------------------
   
/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0
   
/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

//------------------------------------------------

uint32_t cs43l22_ReadID(void);
uint32_t cs43l22_Init(uint16_t OutputDevice, uint8_t Volume);

uint32_t cs43l22_Play(void);
uint32_t cs43l22_Stop(void);
uint32_t cs43l22_SetMute(uint32_t Cmd);
uint32_t cs43l22_SetVolume(uint8_t Volume);

//------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif /* AUDIOPLAY_H_ */
