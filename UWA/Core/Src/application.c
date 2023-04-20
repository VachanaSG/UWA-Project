/**
  ******************************************************************************
  * @file    application.c
  * @author
  * @brief   source code for UWA communication application
  */

/* Includes ------------------------------------------------------------------*/
#include "application.h"

/* Private define ------------------------------------------------------------*/
#define AUDIO_BLOCK_SIZE	1024
#define AUDIO_REC_BUF     	AUDIO_REC_START_ADDR
#define NUM_BLOCKS			200
#define REC_BUF_SIZE		(NUM_BLOCKS * AUDIO_BLOCK_SIZE)		// about 7 sec at 8khz and single channel
#define MOD_OUT_BUF   		(AUDIO_REC_START_ADDR + REC_BUF_SIZE)
#define MOD_BUF_SIZE		(REC_BUF_SIZE * 12)
#define DEMOD_OUT_BUF		(MOD_OUT_BUF + MOD_BUF_SIZE)
#define DEMOD_OUT_SIZE		REC_BUF_SIZE
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t  audio_rec_buffer_state, audio_play_buffer_state;
static TS_StateTypeDef  TS_State;
UART_HandleTypeDef UartHandle;
uint32_t UartReady = 0;
/* Private functions ---------------------------------------------------------*/

/* ------------------------------LCD Functions ------------------------------------------------*/
void LCDInit() {
	uint8_t  lcd_status = LCD_OK;
	// Initialize LCD display with dynamically allocated frame buffer
	lcd_status = BSP_LCD_Init();
	if(lcd_status != LCD_OK){
		while(1){
		}
	}
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_DisplayOn();
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"Hello World", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, 30, (uint8_t *)"Display initialized", CENTER_MODE);
}

void TSInit() {
	uint8_t  status = 0;
	status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (status != TS_OK){
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"ERROR", CENTER_MODE);
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 80, (uint8_t *)"Touchscreen cannot be initialized", CENTER_MODE);
	}
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"Touch Screen initialized", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)"Touch OK to continue", CENTER_MODE);
    drawOK();
    while(1){
    	if(isOKTouched()){
    		HAL_Delay(100);
    		return;
    	}
    	HAL_Delay(100);
    }
}

/*--------------- The UWA App ------------------------*/
void TheApp(){
	uint16_t *record_buffer = AUDIO_REC_BUF;
	uint16_t *mod_buffer = MOD_OUT_BUF;
	uint16_t *demod_buffer = DEMOD_OUT_BUF;
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	ModDemodInit();
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Press OK to start the recording", CENTER_MODE);
	drawOK();
	while(!isOKTouched()){
		HAL_Delay(100);
	}
	record(record_buffer, NUM_BLOCKS);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Recording Complete", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, 140, (uint8_t *)"Press OK to play the recorded audio", CENTER_MODE);
	drawOK();
	while(!isOKTouched()){
		HAL_Delay(100);
	}
	playback(record_buffer, NUM_BLOCKS);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Playback Complete", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, 140, (uint8_t *)"Modulating the Audio................", CENTER_MODE);
	modulate((int16_t*)record_buffer, (int16_t*)mod_buffer, NUM_BLOCKS);
	/*
	for(int i=0;i<NUM_BLOCKS;i++){
		HAL_UART_Transmit(&UartHandle, (uint8_t *)(MOD_OUT_BUF + i*1024*12), 1024*12, HAL_MAX_DELAY);
		HAL_Delay(10);
	}
	uint8_t rxbuf[10] = {0};
	for(int i=0;i<NUM_BLOCKS;i++){
		UartReady = 0;
		HAL_UART_Receive_IT(&UartHandle, (uint8_t *)rxbuf, 5);
		while(UartReady!=1){
		}
	}
	*/
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Modulation Complete", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, 140, (uint8_t *)"Demodulating the Audio................", CENTER_MODE);
	demodulate((int16_t*)mod_buffer, (int16_t*)demod_buffer, NUM_BLOCKS);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"DeModulation Complete", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, 140, (uint8_t *)"Press OK to play the Demodulated Audio", CENTER_MODE);
	drawOK();
	while(!isOKTouched()){
		HAL_Delay(100);
	}
	playback(demod_buffer, NUM_BLOCKS);
	return;
}
/*------------------------ Audio Functions   ----------------------------------*/
uint16_t  rec_buf[AUDIO_BLOCK_SIZE] = {0}, play_buf[AUDIO_BLOCK_SIZE]={0};
// audio loop-back test function
void audioTest(){
	//BSP_AUDIO_IN_Init(DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
	//BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 50, DEFAULT_AUDIO_IN_FREQ);
	BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_DIGITAL_MICROPHONE_2, OUTPUT_DEVICE_HEADPHONE, 48000, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
	audio_rec_buffer_state = BUFFER_OFFSET_NONE;
	BSP_AUDIO_IN_Record((uint16_t*)rec_buf, AUDIO_BLOCK_SIZE);
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	BSP_AUDIO_OUT_Play((uint16_t*)play_buf, AUDIO_BLOCK_SIZE*2);
	//BSP_AUDIO_OUT_SetVolume(50);
	while(1){
		while(audio_rec_buffer_state != BUFFER_OFFSET_HALF){
		}
		audio_rec_buffer_state = BUFFER_OFFSET_NONE;
		for(int i=0;i<AUDIO_BLOCK_SIZE/2;i++){
			play_buf[i] = rec_buf[i];
		}
		while(audio_rec_buffer_state != BUFFER_OFFSET_FULL){
		}
		audio_rec_buffer_state = BUFFER_OFFSET_NONE;
		for(int i=AUDIO_BLOCK_SIZE/2;i<AUDIO_BLOCK_SIZE;i++){
			play_buf[i] = rec_buf[i];
		}
	}
}

/* Following function is going to store single channel audio
 * in the SDRAM for specified number of blocks.
 * For audio playback using this data, make sure to make two copies
 * of the data in  the playback buffer with each sample duplicated
 * and stored together, as the playback function supports only stereo audio
 */
uint16_t internal_buffer[AUDIO_BLOCK_SIZE];
void record(uint16_t *rec_buff, int num_blocks){
	//uint16_t *rec_buffer = AUDIO_REC_BUF;
	uint16_t *rec_buffer = rec_buff;
	BSP_AUDIO_IN_Init(8000, 16, 2);
	//BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_DIGITAL_MICROPHONE_2, OUTPUT_DEVICE_HEADPHONE, 8000, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
	audio_rec_buffer_state = BUFFER_OFFSET_NONE;
	BSP_AUDIO_IN_Record((uint16_t*)internal_buffer, AUDIO_BLOCK_SIZE);
	int num = 0;
	while(1){
		BSP_LCD_SetTextColor(((num%2)?LCD_COLOR_WHITE:LCD_COLOR_RED));
		BSP_LCD_FillCircle(200,200,10);
		while(audio_rec_buffer_state != BUFFER_OFFSET_HALF){
		}
		audio_rec_buffer_state = BUFFER_OFFSET_NONE;
		for(int i=0;i<AUDIO_BLOCK_SIZE/2;i+=2){
			*rec_buffer = internal_buffer[i];
			rec_buffer++;
		}
		while(audio_rec_buffer_state != BUFFER_OFFSET_FULL){
		}
		audio_rec_buffer_state = BUFFER_OFFSET_NONE;
		for(int i=AUDIO_BLOCK_SIZE/2;i<AUDIO_BLOCK_SIZE;i+=2){
			*rec_buffer = internal_buffer[i];
			rec_buffer++;
		}
		if(num == num_blocks)
			break;
		else
			num++;
	}
	BSP_AUDIO_IN_Stop(CODEC_PDWN_SW);
}

void playback(uint16_t *play_buff, int num_blocks){
	//uint16_t *play_buffer = AUDIO_REC_BUF;
	uint16_t *play_buffer = play_buff;
	BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 100, 8000);
	//BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_DIGITAL_MICROPHONE_2, OUTPUT_DEVICE_HEADPHONE, 8000, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
	audio_play_buffer_state = BUFFER_OFFSET_NONE;
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	BSP_AUDIO_OUT_Play((uint16_t*)internal_buffer, AUDIO_BLOCK_SIZE*2);
	int num = 0;
	while(1){
		BSP_LCD_SetTextColor(((num%2)?LCD_COLOR_WHITE:LCD_COLOR_RED));
		BSP_LCD_FillCircle(200,200,10);
		while(audio_play_buffer_state != BUFFER_OFFSET_HALF){
		}
		audio_play_buffer_state = BUFFER_OFFSET_NONE;
		for(int i=0;i<AUDIO_BLOCK_SIZE/2;i+=2){
			internal_buffer[i] = (*play_buffer);
			internal_buffer[i+1] = (*play_buffer);
			play_buffer++;
		}
		while(audio_play_buffer_state != BUFFER_OFFSET_FULL){
		}
		audio_play_buffer_state = BUFFER_OFFSET_NONE;
		for(int i=AUDIO_BLOCK_SIZE/2;i<AUDIO_BLOCK_SIZE;i+=2){
			internal_buffer[i] = (*play_buffer);
			internal_buffer[i+1] = (*play_buffer);
			play_buffer++;
		}
		if(num == num_blocks)
			break;
		else
			num++;
	}
	BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
	BSP_AUDIO_OUT_DeInit();
}

/*---------------- Mod Demod --------------------------  */
#define buf_size 			512
#define block_size  		128
#define numTapsLP 			29
#define numTaps_upsmp 		120
#define upsmp_factor 		12
#define upsmp_buf_size 		(buf_size*upsmp_factor)
#define upsmp_block_size 	(block_size*upsmp_factor)
#define fs_ms 				8000
#define fs_mod 				96000
#define B					3300
#define wo					1650
#define fc 					32000
#define f1					(fc - wo)

float32_t FIRcoef_lp[numTapsLP] = {-0.001184f, -0.002041f, 0.000522f, 0.005196f,
			0.003092f, -0.009479f, -0.014124f, 0.008521f, 0.034061f, 0.009209f,
			-0.058775f, -0.064962f, 0.079575f, 0.303607f, 0.413564f, 0.303607f,
			0.079575f, -0.064962f, -0.058775f, 0.009209f, 0.034061f, 0.008521f,
			-0.014124f, -0.009479f, 0.003092f, 0.005196f, 0.000522f, -0.002041f, -0.001184f
			};
float32_t FIRcoef_lp_upsmp[numTaps_upsmp] = {0.000056f, 0.000167f, 0.000278f, 0.000382f,
		0.000477f, 0.000554f, 0.000605f, 0.000621f, 0.000589f, 0.000501f, 0.000350f, 0.000133f,
		-0.000148f, -0.000481f, -0.000850f, -0.001228f, -0.001582f, -0.001875f, -0.002068f,
		-0.002120f, -0.001999f, -0.001682f, -0.001157f, -0.000431f, 0.000470f, 0.001500f, 0.002594f,
		0.003670f, 0.004634f, 0.005386f, 0.005827f, 0.005870f, 0.005446f, 0.004512f, 0.003061f,
		0.001126f, -0.001215f, -0.003844f, -0.006600f, -0.009288f, -0.011688f, -0.013570f, -0.014701f,
		-0.014867f, -0.013885f, -0.011620f, -0.007993f, -0.002995f, 0.003308f, 0.010775f, 0.019194f,
		0.028286f, 0.037721f, 0.047132f, 0.056134f, 0.064346f, 0.071409f, 0.077008f, 0.080889f, 0.082876f,
		0.082876f, 0.080889f, 0.077008f, 0.071409f, 0.064346f, 0.056134f, 0.047132f, 0.037721f, 0.028286f,
		0.019194f, 0.010775f, 0.003308f, -0.002995f, -0.007993f, -0.011620f, -0.013885f, -0.014867f, -0.014701f,
		-0.013570f, -0.011688f, -0.009288f, -0.006600f, -0.003844f, -0.001215f, 0.001126f, 0.003061f, 0.004512f,
		0.005446f, 0.005870f, 0.005827f, 0.005386f, 0.004634f, 0.003670f, 0.002594f, 0.001500f, 0.000470f,
		-0.000431f, -0.001157f, -0.001682f, -0.001999f, -0.002120f, -0.002068f, -0.001875f, -0.001582f,
		-0.001228f, -0.000850f, -0.000481f, -0.000148f, 0.000133f, 0.000350f, 0.000501f, 0.000589f, 0.000621f,
		0.000605f, 0.000554f, 0.000477f, 0.000382f, 0.000278f, 0.000167f, 0.000056f
			};
arm_fir_instance_f32 FIR_lp_mod_sin, FIR_lp_mod_cos;
arm_fir_instance_f32 FIR_lp_demod_sin, FIR_lp_demod_cos;
float32_t FIRState_lp_mod_sin[numTapsLP + block_size -1] = {0}, FIRState_lp_mod_cos[numTapsLP + block_size -1] = {0};
float32_t FIRState_lp_demod_sin[numTapsLP + block_size -1] = {0}, FIRState_lp_demod_cos[numTapsLP + block_size -1] = {0};

//arm_fir_interpolate_instance_f32 FIR_mod_upsmp_sin, FIR_mod_upsmp_cos;
//arm_fir_decimate_instance_f32 FIR_demod_upsmp_sin, FIR_demod_upsmp_cos;
//float32_t FIRState_mod_upsmp_sin[(numTaps_upsmp/upsmp_factor) + block_size - 1] = {0}, FIRState_mod_upsmp_cos[(numTaps_upsmp/upsmp_factor) + block_size - 1] = {0};
//float32_t FIRState_demod_upsmp_sin[numTaps_upsmp + upsmp_block_size - 1] = {0}, FIRState_demod_upsmp_cos[numTaps_upsmp + upsmp_block_size - 1] = {0};


arm_fir_instance_f32 FIR_mod_upsmp_sin, FIR_mod_upsmp_cos;
arm_fir_instance_f32 FIR_demod_upsmp_sin, FIR_demod_upsmp_cos;
float32_t FIRState_mod_upsmp_sin[numTaps_upsmp + upsmp_block_size - 1] = {0}, FIRState_mod_upsmp_cos[numTaps_upsmp + upsmp_block_size - 1] = {0};
float32_t FIRState_demod_upsmp_sin[numTaps_upsmp + upsmp_block_size - 1] = {0}, FIRState_demod_upsmp_cos[numTaps_upsmp + upsmp_block_size - 1] = {0};

float32_t sin_wo[buf_size], cos_wo[buf_size], sin_fc[upsmp_buf_size], cos_fc[upsmp_buf_size];
void testModDeMod(){
	/* -------------------------------*/
	/* ----- intialisations ---------*/
	/*-------------------------------*/
	/* -------- sine and cos wave ------------*/
	float32_t tm = (float32_t)1/(float32_t)fs_ms;
	for(int i=0;i<block_size;i++){
		sin_wo[i] = arm_sin_f32((float32_t)2 * (float32_t)3.14 * (float32_t)wo * (float32_t)(tm*(float32_t)i));
		cos_wo[i] = arm_cos_f32((float32_t)2 * (float32_t)3.14 * (float32_t)wo * (float32_t)(tm*(float32_t)i));
	}
	/* -------------- Carrier wave ------- */
	tm = (float32_t)1/(float32_t)fs_mod;
	for(int i=0;i<upsmp_block_size;i++){
		sin_fc[i] = arm_sin_f32((float32_t)2 * (float32_t)3.14 * (float32_t)f1 * (float32_t)(tm*(float32_t)i));
		cos_fc[i] = arm_cos_f32((float32_t)2 * (float32_t)3.14 * (float32_t)f1 * (float32_t)(tm*(float32_t)i));
	}
	/* -------- low pass filter for mod ------------- */
	arm_fir_init_f32(&FIR_lp_mod_sin, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_mod_sin[0], block_size);
	arm_fir_init_f32(&FIR_lp_mod_cos, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_mod_cos[0], block_size);
	/* -------- low pass filter for demod ------------- */
	arm_fir_init_f32(&FIR_lp_demod_sin, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_demod_sin[0], block_size);
	arm_fir_init_f32(&FIR_lp_demod_cos, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_demod_cos[0], block_size);
	/* -------- low pass filter for upsampling ------------- */
	arm_fir_init_f32(&FIR_mod_upsmp_sin, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_sin[0], upsmp_block_size);
	arm_fir_init_f32(&FIR_mod_upsmp_cos, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_cos[0], upsmp_block_size);
	/* -------- low pass filter for downsampling ------------- */
	arm_fir_init_f32(&FIR_demod_upsmp_sin, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_sin[0], upsmp_block_size);
	arm_fir_init_f32(&FIR_demod_upsmp_cos, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_cos[0], upsmp_block_size);
	/* -------- upsampling using interpolator ------------- */
	//arm_fir_interpolate_init_f32(&FIR_mod_upsmp_sin, upsmp_factor, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_sin[0], block_size);
	//arm_fir_interpolate_init_f32(&FIR_mod_upsmp_cos, upsmp_factor, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_cos[0], block_size);
	/* -------- downsampling using decimator ------------- */
	//arm_fir_decimate_init_f32(&FIR_demod_upsmp_sin, numTaps_upsmp, upsmp_factor, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_sin[0], upsmp_block_size);
	//arm_fir_decimate_init_f32(&FIR_demod_upsmp_cos, numTaps_upsmp, upsmp_factor, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_cos[0], upsmp_block_size);
	/* ----------- sample input ------------ */
	int16_t buf[buf_size], mod_out[buf_size*upsmp_factor], demod_out[buf_size];
	tm = (float32_t)1/(float32_t)fs_ms;
	for(int i=0;i<buf_size;i++){
		buf[i] = (int16_t)((arm_sin_f32((float32_t)2 * (float32_t)3.14 * (float32_t)3000 * (float32_t)(tm*(float32_t)i))
				+ arm_sin_f32((float32_t)2 * (float32_t)3.14 * (float32_t)2000 * (float32_t)(tm*(float32_t)i))
				+ arm_sin_f32((float32_t)2 * (float32_t)3.14 * (float32_t)1000 * (float32_t)(tm*(float32_t)i))))*100;
	}
	/* ----------------- modulator ----------------- */
	float32_t input[block_size] , output[upsmp_block_size];
	for(int j=0;j<(buf_size/block_size);j++){
		for(int i=0;i<block_size;i++)
			input[i] = buf[(j*block_size) + i];
		float32_t sin_buf1[block_size], cos_buf1[block_size];
		float32_t sin_buf2[block_size], cos_buf2[block_size];
		float32_t sin_buf3[upsmp_block_size]={0}, cos_buf3[upsmp_block_size]={0};
		float32_t sin_buf4[upsmp_block_size], cos_buf4[upsmp_block_size];
		arm_mult_f32(&input[0], &sin_wo[(j*block_size)], &sin_buf1[0], block_size);
		arm_mult_f32(&input[0], &cos_wo[(j*block_size)], &cos_buf1[0], block_size);
		arm_fir_f32(&FIR_lp_mod_sin, &sin_buf1[0], &sin_buf2[0], block_size);
		arm_fir_f32(&FIR_lp_mod_cos, &cos_buf1[0], &cos_buf2[0], block_size);
		for(int k=0;k<block_size;k++){
			sin_buf3[k*upsmp_factor] = sin_buf2[k];
			cos_buf3[k*upsmp_factor] = cos_buf2[k];
		}
		arm_fir_f32(&FIR_mod_upsmp_sin, &sin_buf3[0], &sin_buf4[0], upsmp_block_size);
		arm_fir_f32(&FIR_mod_upsmp_cos, &cos_buf3[0], &cos_buf4[0], upsmp_block_size);
		//arm_fir_interpolate_f32(&FIR_mod_upsmp_sin, &sin_buf2[0], &sin_buf4[0], block_size);
		//arm_fir_interpolate_f32(&FIR_mod_upsmp_cos, &cos_buf2[0], &cos_buf4[0], block_size);
		arm_mult_f32(&sin_buf4[0], &sin_fc[(j*upsmp_block_size)], &sin_buf4[0], upsmp_block_size);
		arm_mult_f32(&cos_buf4[0], &cos_fc[(j*upsmp_block_size)], &cos_buf4[0], upsmp_block_size);
		arm_add_f32(&sin_buf4[0], &cos_buf4[0], &output[0], upsmp_block_size);
		for(int i=0;i<upsmp_block_size;i++){
			output[i]*=10;
			mod_out[(j*upsmp_block_size)+i] = (int16_t)output[i];
		}
	}
	/* ----------------- demodulator ----------------- */
	float32_t demod_input[upsmp_block_size] , demod_output[block_size];
	for(int j=0;j<((buf_size*12)/upsmp_block_size);j++){
		for(int i=0;i<upsmp_block_size;i++)
			demod_input[i] = ((float32_t)mod_out[(j*upsmp_block_size) + i]);
		float32_t sin_buf1[upsmp_block_size], cos_buf1[upsmp_block_size];
		float32_t sin_buf2[upsmp_block_size], cos_buf2[upsmp_block_size];
		float32_t sin_buf3[block_size], cos_buf3[block_size];
		float32_t sin_buf4[block_size], cos_buf4[block_size];
		arm_mult_f32(&demod_input[0], &sin_fc[(j*upsmp_block_size)], &sin_buf1[0], upsmp_block_size);
		arm_mult_f32(&demod_input[0], &cos_fc[(j*upsmp_block_size)], &cos_buf1[0], upsmp_block_size);
		arm_fir_f32(&FIR_demod_upsmp_sin, &sin_buf1[0], &sin_buf2[0], upsmp_block_size);
		arm_fir_f32(&FIR_demod_upsmp_cos, &cos_buf1[0], &cos_buf2[0], upsmp_block_size);
		for(int k=0;k<block_size;k++){
			sin_buf3[k] = sin_buf2[k*12];
			cos_buf3[k] = cos_buf2[k*12];
		}
		//arm_fir_decimate_f32(&FIR_demod_upsmp_sin, &sin_buf1[0], &sin_buf3[0], upsmp_block_size);
		//arm_fir_decimate_f32(&FIR_demod_upsmp_cos, &cos_buf1[0], &cos_buf3[0], upsmp_block_size);
		arm_fir_f32(&FIR_lp_demod_sin, &sin_buf3[0], &sin_buf4[0], block_size);
		arm_fir_f32(&FIR_lp_demod_cos, &cos_buf3[0], &cos_buf4[0], block_size);
		arm_mult_f32(&sin_buf4[0], &sin_wo[(j*block_size)], &sin_buf4[0], block_size);
		arm_mult_f32(&cos_buf4[0], &cos_wo[(j*block_size)], &cos_buf4[0], block_size);
		arm_add_f32(&sin_buf4[0], &cos_buf4[0], &demod_output[0], block_size);
		for(int i=0;i<block_size;i++){
			demod_output[i]*=5;
			demod_out[(j*block_size)+i] = (int16_t)demod_output[i];
		}
	}
}

void ModDemodInit(){
	/* -------------------------------*/
	/* ----- intialisations ---------*/
	/*-------------------------------*/
	/* -------- sine and cos wave ------------*/
	float32_t tm = (float32_t)1/(float32_t)fs_ms;
	for(int i=0;i<block_size;i++){
		sin_wo[i] = arm_sin_f32((float32_t)2 * (float32_t)3.14 * (float32_t)wo * (float32_t)(tm*(float32_t)i));
		cos_wo[i] = arm_cos_f32((float32_t)2 * (float32_t)3.14 * (float32_t)wo * (float32_t)(tm*(float32_t)i));
	}
	/* -------------- Carrier wave ------- */
	tm = (float32_t)1/(float32_t)fs_mod;
	for(int i=0;i<upsmp_block_size;i++){
		sin_fc[i] = arm_sin_f32((float32_t)2 * (float32_t)3.14 * (float32_t)f1 * (float32_t)(tm*(float32_t)i));
		cos_fc[i] = arm_cos_f32((float32_t)2 * (float32_t)3.14 * (float32_t)f1 * (float32_t)(tm*(float32_t)i));
	}
	/* -------- low pass filter for mod ------------- */
	arm_fir_init_f32(&FIR_lp_mod_sin, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_mod_sin[0], block_size);
	arm_fir_init_f32(&FIR_lp_mod_cos, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_mod_cos[0], block_size);
	/* -------- low pass filter for demod ------------- */
	arm_fir_init_f32(&FIR_lp_demod_sin, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_demod_sin[0], block_size);
	arm_fir_init_f32(&FIR_lp_demod_cos, numTapsLP, &FIRcoef_lp[0], &FIRState_lp_demod_cos[0], block_size);
	/* -------- low pass filter for upsampling ------------- */
	arm_fir_init_f32(&FIR_mod_upsmp_sin, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_sin[0], upsmp_block_size);
	arm_fir_init_f32(&FIR_mod_upsmp_cos, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_cos[0], upsmp_block_size);
	/* -------- low pass filter for downsampling ------------- */
	arm_fir_init_f32(&FIR_demod_upsmp_sin, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_sin[0], upsmp_block_size);
	arm_fir_init_f32(&FIR_demod_upsmp_cos, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_cos[0], upsmp_block_size);
	/* -------- upsampling using interpolator ------------- */
	//arm_fir_interpolate_init_f32(&FIR_mod_upsmp_sin, upsmp_factor, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_sin[0], block_size);
	//arm_fir_interpolate_init_f32(&FIR_mod_upsmp_cos, upsmp_factor, numTaps_upsmp, &FIRcoef_lp_upsmp[0], &FIRState_mod_upsmp_cos[0], block_size);
	/* -------- downsampling using decimator ------------- */
	//arm_fir_decimate_init_f32(&FIR_demod_upsmp_sin, numTaps_upsmp, upsmp_factor, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_sin[0], upsmp_block_size);
	//arm_fir_decimate_init_f32(&FIR_demod_upsmp_cos, numTaps_upsmp, upsmp_factor, &FIRcoef_lp_upsmp[0], &FIRState_demod_upsmp_cos[0], upsmp_block_size);
	return;
}

/* modulator function
 * input_buuf = pointer to the input buffer
 * output_buf = pointer to the output buffer
 * input_buf_size = size of the input buffer (should be multiple of the block_size)
 * output_buf should be 12 times larger than input_buf
 */
void modulate(int16_t *input_buf, int16_t *output_buf, uint32_t num_buf){
	float32_t mod_input[block_size] , mod_output[upsmp_block_size];
	//for(int k = 0;k<num_buf;k++){
		for(int j=0;j<((buf_size*num_buf)/block_size);j++){
			int offset = (j*block_size);
			for(int i=0;i<block_size;i++)
				mod_input[i] = input_buf[offset + i];
			float32_t sin_buf1[block_size], cos_buf1[block_size];
			float32_t sin_buf2[block_size], cos_buf2[block_size];
			float32_t sin_buf3[upsmp_block_size]={0}, cos_buf3[upsmp_block_size]={0};
			float32_t sin_buf4[upsmp_block_size], cos_buf4[upsmp_block_size];
			arm_mult_f32(&mod_input[0], &sin_wo[0], &sin_buf1[0], block_size);
			arm_mult_f32(&mod_input[0], &cos_wo[0], &cos_buf1[0], block_size);
			arm_fir_f32(&FIR_lp_mod_sin, &sin_buf1[0], &sin_buf2[0], block_size);
			arm_fir_f32(&FIR_lp_mod_cos, &cos_buf1[0], &cos_buf2[0], block_size);
			for(int k=0;k<block_size;k++){
				sin_buf3[k*upsmp_factor] = sin_buf2[k];
				cos_buf3[k*upsmp_factor] = cos_buf2[k];
			}
			arm_fir_f32(&FIR_mod_upsmp_sin, &sin_buf3[0], &sin_buf4[0], upsmp_block_size);
			arm_fir_f32(&FIR_mod_upsmp_cos, &cos_buf3[0], &cos_buf4[0], upsmp_block_size);
			//arm_fir_interpolate_f32(&FIR_mod_upsmp_sin, &sin_buf2[0], &sin_buf4[0], block_size);
			//arm_fir_interpolate_f32(&FIR_mod_upsmp_cos, &cos_buf2[0], &cos_buf4[0], block_size);
			offset = (j*upsmp_block_size);
			arm_mult_f32(&sin_buf4[0], &sin_fc[0], &sin_buf4[0], upsmp_block_size);
			arm_mult_f32(&cos_buf4[0], &cos_fc[0], &cos_buf4[0], upsmp_block_size);
			arm_add_f32(&sin_buf4[0], &cos_buf4[0], &mod_output[0], upsmp_block_size);
			for(int i=0;i<upsmp_block_size;i++){
				mod_output[i]*=5;
				output_buf[offset+i] = (int16_t)mod_output[i];
			}
		}
	//}
	return;
}
/* demodulator function
 * input_buuf = pointer to the input buffer
 * output_buf = pointer to the output buffer
 * input_buf_size = size of the input buffer (should be multiple of the upsmp_block_size)
 * output_buf should be 12 times smaller than input_buf
 */
void demodulate(int16_t *input_buf, int16_t *output_buf, uint32_t num_buf){
	float32_t demod_input[upsmp_block_size] , demod_output[block_size];
	//for(int k=0;k<num_buf;k++){
		for(int j=0;j<((upsmp_buf_size*num_buf)/upsmp_block_size);j++){
			int offset = (j*upsmp_block_size);
			for(int i=0;i<upsmp_block_size;i++)
				demod_input[i] = ((float32_t)input_buf[offset + i]);
			float32_t sin_buf1[upsmp_block_size], cos_buf1[upsmp_block_size];
			float32_t sin_buf2[upsmp_block_size], cos_buf2[upsmp_block_size];
			float32_t sin_buf3[block_size], cos_buf3[block_size];
			float32_t sin_buf4[block_size], cos_buf4[block_size];
			arm_mult_f32(&demod_input[0], &sin_fc[0], &sin_buf1[0], upsmp_block_size);
			arm_mult_f32(&demod_input[0], &cos_fc[0], &cos_buf1[0], upsmp_block_size);
			arm_fir_f32(&FIR_demod_upsmp_sin, &sin_buf1[0], &sin_buf2[0], upsmp_block_size);
			arm_fir_f32(&FIR_demod_upsmp_cos, &cos_buf1[0], &cos_buf2[0], upsmp_block_size);
			for(int k=0;k<block_size;k++){
				sin_buf3[k] = sin_buf2[k*12];
				cos_buf3[k] = cos_buf2[k*12];
			}
			//arm_fir_decimate_f32(&FIR_demod_upsmp_sin, &sin_buf1[0], &sin_buf3[0], upsmp_block_size);
			//arm_fir_decimate_f32(&FIR_demod_upsmp_cos, &cos_buf1[0], &cos_buf3[0], upsmp_block_size);
			arm_fir_f32(&FIR_lp_demod_sin, &sin_buf3[0], &sin_buf4[0], block_size);
			arm_fir_f32(&FIR_lp_demod_cos, &cos_buf3[0], &cos_buf4[0], block_size);
			offset = (j*block_size);
			arm_mult_f32(&sin_buf4[0], &sin_wo[0], &sin_buf4[0], block_size);
			arm_mult_f32(&cos_buf4[0], &cos_wo[0], &cos_buf4[0], block_size);
			arm_add_f32(&sin_buf4[0], &cos_buf4[0], &demod_output[0], block_size);
			for(int i=0;i<block_size;i++){
				demod_output[i]*=5;
				output_buf[offset+i] = (int16_t)demod_output[i];
			}
		}
	//}
	return;
}
/* --------------Feedback Function ------------------*/
uint8_t isTouched(){
	BSP_TS_GetState(&TS_State);
	if(TS_State.touchDetected){
		HAL_Delay(100);
		BSP_TS_GetState(&TS_State);
		return TS_State.touchDetected;
	}
	else
		return 0;
}

void drawOK(){
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawRect(400,200,60,30);
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_DisplayStringAt(410,205, (uint8_t *)"OK", LEFT_MODE);
}

uint8_t isOKTouched(){
	//drawOK();
	BSP_TS_GetState(&TS_State);
	if(TS_State.touchDetected && TS_State.touchX[0] > 400 && TS_State.touchX[0] < 460 && TS_State.touchY[0] > 200 && TS_State.touchY[0] < 230)
	{
		do{
			HAL_Delay(10);
			BSP_TS_GetState(&TS_State);
		}while(TS_State.touchDetected);
		return 1;
	}
	else
		return 0;
}

/*-------------- UART ------------------- */
// Use uart1 for transmission using usb cable
// use uart6 for transmission using digital pins CN4.D0(Rx) and CN4.D1(Tx)
void UART1_Init(void)
{
	// Configure UART
	UartHandle.Instance = USART1;
	UartHandle.Init.BaudRate = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_NONE;
	UartHandle.Init.Mode = UART_MODE_TX_RX;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		while(1){

		}
	}
	/*
	while (1)
	{
		// Transmit data over UART
		uint8_t data[] = "Hello!\r\n";
		HAL_UART_Transmit(&huart1, data, sizeof(data), HAL_MAX_DELAY);

		// Delay before sending the next message
		HAL_Delay(1000);
	}
	*/
}

/* Callback implementations (use as required) */
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
  audio_rec_buffer_state = BUFFER_OFFSET_FULL;
  return;
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
  audio_rec_buffer_state = BUFFER_OFFSET_HALF;
  return;
}
void BSP_AUDIO_IN_Error_CallBack(void)
{
  BSP_LCD_SetBackColor(LCD_COLOR_RED);
  BSP_LCD_DisplayStringAt(0, LINE(14), (uint8_t *)"       DMA  ERROR     ", CENTER_MODE);
  return;
}
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
  //if(audio_state == AUDIO_STATE_PLAYING)
  {
	  audio_play_buffer_state = BUFFER_OFFSET_FULL;
  }
}
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
  //if(audio_state == AUDIO_STATE_PLAYING)
  {
	  audio_play_buffer_state = BUFFER_OFFSET_HALF;
  }
}
void BSP_AUDIO_OUT_Error_CallBack(void)
{
  BSP_LCD_SetBackColor(LCD_COLOR_RED);
  BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"       DMA  ERROR     ");
  while (BSP_PB_GetState(BUTTON_KEY) != RESET)
  {
    return;
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = 1;
}
