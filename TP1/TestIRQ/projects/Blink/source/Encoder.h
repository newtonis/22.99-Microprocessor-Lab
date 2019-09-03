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
typedef void (*encCH_callback_t)(int array[], int side_motion);
typedef void (*encRSw_callback_t)(int menu);

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void encoderInit(void);

void encoderReadMotion(int pin_array[], encCH_callback_t callback);

void encoderReadSwitch(int menu_type, encRSw_callback_t callback);

#endif /* ENCODER_H_ */
