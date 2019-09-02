/*
 * Encoder.h
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */

#ifndef ENCODER_H_
#define ENCODER_H_

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define NONE	0

#define RIGHT	1
#define LEFT	-1
#define ENTER	0

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void EncSetFlag(int flag);

void EncClearFlag(void);

int EncGetFlag(void);

#endif /* ENCODER_H_ */
