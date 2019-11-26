/*
 * Encoder.c
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#include "Encoder.h"
#include "board.h"
#include "PIT.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int channel_flag = NONE;

static uint8_t time_count = 20;

//static tim_id_t timerEncCH;
//static tim_id_t timerEncSwitch;

static int motion_event = NONE;

static void (*callback)();
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void EncSetFlag(int flag);

void EncClearFlag(void);

enc_flag_t EncGetFlag(void);

void encoderCH_Handler(void);

void enconderSwitch_Handler(void);

enc_flag_t encoderReadMotion(void);

bool encoderReadSwitch(void);

void encoder_switch(void);
/*******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

void encoderInit(void (*funcallback)(void)){
    gpioMode(RCHA, INPUT);
    gpioMode(RCHB, INPUT);
    gpioMode(RSwitch, INPUT);

    callback = funcallback;

    PIT_configTimer(2, 400000,  encoder_switch);

    PIT_startTime(2);
}

void encoder_switch(void)
{
	encoderCH_Handler();
	if (time_count == 0)
	{
		enconderSwitch_Handler();
		time_count = 20;

	}
	else
	{
		time_count = time_count - 1;
	}

}


void encoderCH_Handler(void){

	motion_event = encoderReadMotion();
	if(motion_event){
		callback();
	}
}

void enconderSwitch_Handler(void){

	if(encoderReadSwitch()){
		motion_event = ENTER;
		callback();
	}

}

enc_flag_t encoderMotionGetEvent(void){

	if(motion_event == LEFT){
		motion_event = NONE;
		return LEFT;
	}else if(motion_event == RIGHT){
		motion_event = NONE;
		return RIGHT;
	}else if(motion_event == ENTER){
		motion_event = NONE;
		return ENTER;
	}
	return NONE;
}

enc_flag_t encoderReadMotion(void){
	if(EncGetFlag() == NONE){
		if(!gpioRead(RCHA)){
			EncSetFlag(LEFT);
			return LEFT;
		}else if(!(gpioRead(RCHB))){
			EncSetFlag(RIGHT);
			return RIGHT;
		}
	}else{
		if(gpioRead(RCHA) && gpioRead(RCHB)){
			EncSetFlag(NONE);
			return NONE;
		}
	}
	return NONE;
}

bool encoderReadSwitch(void){
	if(!gpioRead(RSwitch)){
		return 1;
	}else{
		return 0;
	}
}

void EncSetFlag(int flag){
	channel_flag = flag;
}

void EncClearFlag(void){
	channel_flag = NONE;
}

enc_flag_t EncGetFlag(void){
	return channel_flag;
}

/*******************************************************************************
 ******************************************************************************/
