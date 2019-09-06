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

typedef struct {
    int	id[8];
    int pin[5];
} user_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void PortInit(void);
bool PortGetOK_Status(void);
void PortClearOK_Status(void);
bool PortGetCANCEL_Status(void);
void PortClearCANCEL_Status(void);
void RGBIndicator(int led_color);

#endif /* INTERNALCONTROL_H_ */
