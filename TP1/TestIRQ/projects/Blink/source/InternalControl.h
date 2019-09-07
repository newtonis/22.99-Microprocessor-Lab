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

void internalControlInit(void (*funcallback)(void));
int internalControlGetEvent(void);
void RGBIndicator(int led_color);

#endif /* INTERNALCONTROL_H_ */
