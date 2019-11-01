/*
 * DecodeBits.h
 *
 *  Created on: 27 Oct 2019
 *      Author: Marcelo
 */

#ifndef DECODEBITS_H_
#define DECODEBITS_H_

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void Decoder_init(uint32_t* medAddress, void(*rtsFunc)(char c));


#endif /* DECODEBITS_H_ */
