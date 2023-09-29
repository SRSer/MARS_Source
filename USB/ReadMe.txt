/*** main.h ***/

#define HAL_PCD_MODULE_ENABLED

/*** main.c ***/

/* USER CODE BEGIN Includes */

#include "usb_device.h"#include "usbd_midi_if.h"

/* USER CODE BEGIN 2 */    MX_USB_DEVICE_Init();

/* usbd_midi_if */

// basic midi rx/tx functions
static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);
static uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);