/*
 * Encoder.h
 *
 *  Created on: 30 Aug 2019
 *      Author: Marcelo
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define NONE	0

#define RIGHT	1
#define LEFT	2
#define ENTER	0

typedef uint8_t enc_flag_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void encoderInit(void);

enc_flag_t encoderReadMotion(void);

bool encoderReadSwitch(void);

#endif /* ENCODER_H_ */
