/*
 * Modulador.h
 *
 *  Created on: 19 Oct 2019
 *      Author: Marcelo
 */

#ifndef MODULADOR_H_
#define MODULADOR_H_

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define STAND_LEN	11

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void Modulador_init(uint16_t* dutyAddress, void(*funcallback)(void));

void Modulador_sendChar(char my_char);

void Modulador_sendStream(bool *stream);


#endif /* MODULADOR_H_ */
