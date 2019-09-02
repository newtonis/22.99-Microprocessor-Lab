/*
 * Encoder.c
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */
#include "Encoder.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static int channel_flag = NONE;

/*******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 ******************************************************************************/

void EncSetFlag(int flag){
	channel_flag = flag;
}

void EncClearFlag(void){
	channel_flag = NONE;
}

int EncGetFlag(void){
	return channel_flag;
}
