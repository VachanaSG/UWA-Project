/**
  ******************************************************************************
  * @file    application.h
  * @author
  * @brief   header file for application.c
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_eeprom.h"
#include "stm32746g_discovery_camera.h"
#include "stm32746g_discovery_audio.h"
#include "stm32746g_discovery_qspi.h"
#include "arm_math.h"


#define LCD_FRAME_BUFFER          SDRAM_DEVICE_ADDR
#define SDRAM_WRITE_READ_ADDR        ((uint32_t)(LCD_FRAME_BUFFER + (RK043FN48H_WIDTH * RK043FN48H_HEIGHT * 4)))
#define AUDIO_REC_START_ADDR         SDRAM_WRITE_READ_ADDR

extern UART_HandleTypeDef UartHandle;

typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF = 1,
  BUFFER_OFFSET_FULL = 2,
}BUFFER_StateTypeDef;

void LCDInit(void);
void audio_loopback(void);
void TSInit(void);
void rec_and_play(void);
void testModDeMod(void);
void test(void);
void record(uint16_t *rec_buff, int num_blocks);
void playback(uint16_t *play_buff, int num_blocks);
void audioTest(void);
void ModDemodInit(void);
void modulate(int16_t *input_buf, int16_t *output_buf, uint32_t input_buf_size);
void demodulate(int16_t *input_buf, int16_t *output_buf, uint32_t input_buf_size);
uint8_t isTouched(void);
void drawOK(void);
uint8_t isOKTouched(void);
void TheApp(void);
void UART1_Init(void);
void UART6_Init(void);
