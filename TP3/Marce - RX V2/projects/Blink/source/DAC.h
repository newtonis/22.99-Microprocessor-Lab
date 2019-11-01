/*
 * DAC.h
 *
 *  Created on: 18 Oct 2019
 *      Author: Marcelo
 */

#ifndef DAC_H_
#define DAC_H_

#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

typedef uint16_t DACDATA_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void DAC_init(void);

void DAC_setData(DACDATA_t data);

#endif /* DAC_H_ */
