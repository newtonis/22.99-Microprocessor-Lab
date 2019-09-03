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

    // El LED es de prueba
    gpioMode(PIN_LED_BLUE, OUTPUT);
}

void encoderReadMotion(int pin_array[], encCH_callback_t callback){
	if(EncGetFlag() == NONE){
		if(!gpioRead(RCHA)){
			EncSetFlag(LEFT);
			callback(pin_array, LEFT);
		}else if(!(gpioRead(RCHB))){
			EncSetFlag(RIGHT);
			callback(pin_array, RIGHT);
		}
	}else{
		if(gpioRead(RCHA) && gpioRead(RCHB)){
			EncSetFlag(NONE);
		}
	}
}

void encoderReadSwitch(int menu_type, encRSw_callback_t callback){
	if(!gpioRead(RSwitch)){
		gpioToggle(PIN_LED_BLUE);
		callback(menu_type);
	}
}

/*******************************************************************************
 ******************************************************************************/
