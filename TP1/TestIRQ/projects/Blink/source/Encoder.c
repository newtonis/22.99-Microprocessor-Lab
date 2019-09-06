/*
 * Encoder.c
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#include "Encoder.h"
#include "board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int channel_flag = NONE;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void EncSetFlag(int flag);

void EncClearFlag(void);

enc_flag_t EncGetFlag(void);

/*******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

void EncSetFlag(int flag){
	channel_flag = flag;
}

void EncClearFlag(void){
	channel_flag = NONE;
}

enc_flag_t EncGetFlag(void){
	return channel_flag;
}

void encoderInit(void){
    gpioMode(RCHA, INPUT);
    gpioMode(RCHB, INPUT);
    gpioMode(RSwitch, INPUT);
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
}

bool encoderReadSwitch(void){
	if(!gpioRead(RSwitch)){
		return 1;
	}else{
		return 0;
	}
}

/*******************************************************************************
 ******************************************************************************/
