/*
 * PIT.h
 *
 *  Created on: 24 Oct 2019
 *      Author: Marcelo
 */

#ifndef PIT_H_
#define PIT_H_

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

void PIT_init(void(*funcallback0)(void), void(*funcallback1)(void));

// Provisorias
void PIT_set0Time(void);

void PIT_startTime(uint8_t timer_id);

void PIT_stopTime(uint8_t timer_id);
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


#endif /* PIT_H_ */