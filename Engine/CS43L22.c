#include "CS43L22.h"

//------------------------------------------------------
#define I2S_STANDARD                  I2S_STANDARD_PHILIPS

#define AUDIO_RESET_GPIO_CLK_ENABLE()         __GPIOD_CLK_ENABLE()
#define AUDIO_RESET_PIN                       GPIO_PIN_4
#define AUDIO_RESET_GPIO                      GPIOD
#define VOLUME_CONVERT(Volume)   (((Volume) > 100)? 100:((uint8_t)(((Volume) * 255) / 100)))

#define CODEC_STANDARD                0x04

static uint8_t Is_cs43l22_Stop = 1;

#define   CS43L22_REG_MISC_CTL        0x0E

#define AUDIO_I2C_ADDRESS     0x94
#define CS43L22_CHIPID_ADDR   0x01
#define CS43L22_ID            0xE0
#define CS43L22_ID_MASK       0xF8

volatile uint8_t OutputDev = 0;

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;


//------------------------------------------------------
/*void Error(void)
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
}*/
//------------------------------------------------------
/*static void I2S3_Init(uint32_t AudioFreq)
{
	hi2s3.Instance=I2S3;
	//Disable I2S Block
	__HAL_I2S_DISABLE(&hi2s3);
	hi2s3.Init.AudioFreq = AudioFreq;
	hi2s3.Init.Standard = I2S_STANDARD;
	HAL_I2S_DeInit(&hi2s3);
	if(HAL_I2S_Init(&hi2s3)!=HAL_OK)
	{
		//Error();
	}
}*/
//------------------------------------------------------
uint8_t CODEC_IO_Write(uint8_t Reg, uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t result=0;
	status = HAL_I2C_Mem_Write(&hi2c1, AUDIO_I2C_ADDRESS,(uint16_t)Reg,
                                   I2C_MEMADD_SIZE_8BIT,&Value,1,0x1000);
	if(status!=HAL_OK)
	{
		//Error();
		return 1;
	}
	return result;
}
//------------------------------------------------------
uint32_t cs43l22_SetVolume(uint8_t Volume)
{

	uint32_t counter=0;
	uint8_t convertedvol=VOLUME_CONVERT(Volume);
	if(Volume>0xE6)
	{
		/*Set the Master volume*/
		counter+=CODEC_IO_Write(0x20, convertedvol-0x07);
		counter+=CODEC_IO_Write(0x21, convertedvol-0x07);
	}
	else
	{
		/*Set the Master volume*/
		counter+=CODEC_IO_Write(0x20, convertedvol+0x19);
		counter+=CODEC_IO_Write(0x21, convertedvol+0x19);
	}
	return counter;
}
//------------------------------------------------------
uint32_t cs43l22_SetMute(uint32_t Cmd)
{
	uint32_t counter=0;
	/*Set the Mute mode*/
	if(Cmd==AUDIO_MUTE_ON)
	{
		counter+=CODEC_IO_Write(0x04, 0xFF);
	}
	else /*AUDIO_MUTE_OFF Disable the Mute*/
	{
		counter+=CODEC_IO_Write(0x04, OutputDev);
	}
	return counter;
}
//------------------------------------------------------
uint32_t cs43l22_Init(uint16_t OutputDevice, uint8_t Volume)
{
	uint32_t counter=0;
	//reset on the codec
        HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_RESET);
	HAL_Delay(5);
        HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_SET);
	HAL_Delay(5);	
	counter+=CODEC_IO_Write(0x02, 0x01);
	/*Save Output device for mute ON/OFF procedure*/
	switch(OutputDevice)
	{
		case OUTPUT_DEVICE_SPEAKER:
			OutputDev=0xFA;
			break;
		case OUTPUT_DEVICE_HEADPHONE:
			OutputDev=0xAF;
			break;
		case OUTPUT_DEVICE_BOTH:
			OutputDev=0xAA;
			break;
		case OUTPUT_DEVICE_AUTO:
			OutputDev=0x05;
			break;
		default :
			OutputDev=0x05;
			break;
	}
	counter+=CODEC_IO_Write(0x04, OutputDev);
	/*Clock configuration: Auto detection*/
        counter+=CODEC_IO_Write(0x05, 0x81);
        /*Set the Slave Mode and the audio Standard*/
	counter+=CODEC_IO_Write(0x06, CODEC_STANDARD);
	/*Set the Master volume*/
	counter+=cs43l22_SetVolume(Volume);
	/*If the Speaker is enabled, set the Mono mode and Volume attenuation level*/
	if(OutputDevice!=OUTPUT_DEVICE_HEADPHONE)
	{
		/*Set the Speaker Mono mode*/
		counter+=CODEC_IO_Write(0x0F, 0x06);
		/*Set the Speaker attenuation level*/
		counter+=CODEC_IO_Write(0x24, 0x00);
		counter+=CODEC_IO_Write(0x25, 0x00);
	}
	/*Disable the analog soft ramp*/
	counter+=CODEC_IO_Write(0x0A, 0x00);
	/*Disable the digital soft ramp*/
	counter+=CODEC_IO_Write(0x0E, 0x04);
	/*Disable the limiter attack level*/
	counter+=CODEC_IO_Write(0x27, 0x00);
	/*Adjust Bass and Treble level*/
	counter+=CODEC_IO_Write(0x1F, 0x0F);
	/*Adjust PCM volume level*/
	counter+=CODEC_IO_Write(0x1A, 0x0A);
	counter+=CODEC_IO_Write(0x1B, 0x0A);
	//I2S3_Init(AudioFreq);
	return counter;
}
//------------------------------------------------------
uint32_t cs43l22_Stop(void)
{
	uint32_t counter=0;
	/*Mute the output first*/
	counter+=cs43l22_SetMute(AUDIO_MUTE_ON);
	/*Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
		counter+=CODEC_IO_Write(0x02, 0x9F);
	Is_cs43l22_Stop=1;
	return counter;
}
//------------------------------------------------------
uint32_t cs43l22_Play(void)
{
	uint32_t counter=0;
	
	if(Is_cs43l22_Stop==1)
	{
		/*Enable the digital soft ramp*/
		counter+=CODEC_IO_Write(CS43L22_REG_MISC_CTL, 0x06);
		/*Enable Output Device*/		
		counter+= cs43l22_SetMute(AUDIO_MUTE_OFF);
		/*Power on the Codec*/
		counter+=CODEC_IO_Write(0x02, 0x9E);
		Is_cs43l22_Stop=0;
	}
	return counter;
}
//------------------------------------------------------
uint32_t cs43l22_ReadID(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value=0;
	status = HAL_I2C_Mem_Read(&hi2c1, AUDIO_I2C_ADDRESS, (uint16_t)CS43L22_CHIPID_ADDR, 
                                  I2C_MEMADD_SIZE_8BIT, &value, 1, 0x1000);
	if(status==HAL_OK)
	{
		value=(value&CS43L22_ID_MASK);
		return ((uint32_t)value);
	}
	return 0;
}
//------------------------------------------------------