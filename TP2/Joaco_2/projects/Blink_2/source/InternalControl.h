/*
 * InternalControl.h
 *
 *  Created on: 5 Sep 2019
 *      Author: Marcelo
 */

#ifndef INTERNALCONTROL_H_
#define INTERNALCONTROL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

enum{RED_INDICATOR, BLUE_INDICATOR, GREEN_INDICATOR};

#define NONE			0
#define OK_EVENT		1
#define CANCEL_EVENT	2

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief initializes buttons an RGB from K64F
 * @param callback to call when a button event occurs.
 */
void internalControlInit(void (*funcallback)(void));

/**
 * @brief getter for the events of the buttons
 * @return NONE if nothing happend, OK_EVENT or CANCEL_EVENT from the respective buttons.
 */
int internalControlGetEvent(void);

/**
 * @brief sets color for the RGB led
 * @param the color indicator in the enum braces (RED, BLUE or GREEN).
 */
void RGBIndicator(int led_color);

#endif /* INTERNALCONTROL_H_ */
