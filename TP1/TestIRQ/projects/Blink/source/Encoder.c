/*
 * Encoder.c
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#include "Encoder.h"
#include "timer.h"
#include "board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int channel_flag = NONE;

static tim_id_t timerEncCH;
static tim_id_t timerEncSwitch;

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
/*******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

void encoderInit(void (*funcallback)(void)){
    gpioMode(RCHA, INPUT);
    gpioMode(RCHB, INPUT);
    gpioMode(RSwitch, INPUT);

    callback = funcallback;

    timerEncCH = timerGetId();
    timerStart(timerEncCH, TIMER_MS2TICKS(40), TIM_MODE_PERIODIC, encoderCH_Handler);

    timerEncSwitch = timerGetId();
    timerStart(timerEncSwitch, TIMER_MS2TICKS(500), TIM_MODE_PERIODIC, enconderSwitch_Handler);
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
