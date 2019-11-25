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
#define ENTER	3

typedef uint8_t enc_flag_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief initializes encoder ports
 * @param callback to call when an event asociated to the movement or switch press occurrs.
 */
void encoderInit(void (*funcallback)(void));

/**
 * @brief getter for the events of the encoder
 * @return RIGHT or LEFT if the encoder was rotated, or ENTER if the switch was pressed.
 */
enc_flag_t encoderMotionGetEvent(void);

#endif /* ENCODER_H_ */
