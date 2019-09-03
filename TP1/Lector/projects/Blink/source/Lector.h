/*
 * Lector.h
 *
 *  Created on: Sep 1, 2019
 *      Author: martinamaspero
 */

#ifndef LECTOR_H_
#define LECTOR_H_

#include "board.h"
#include <stddef.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ID_LENGTH 8
#define DATA_LENGTH 40
#define BITSXNUM 5
#define SS 0b01011 // Start Sentinel ';'
#define FS 0b01101 //Field separator '='
#define ES 0b11111 //End sentinel '?'
#define PARITY_BIT_MASK_EVEN 0b00010000
#define PARITY_BIT_MASK_ODD 0b00001111


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


int * get_ID (void); //Devuelve NULL si no terminó de leer
void clear_ID (void);
bool Check_LRC (void);
void get_Data (bool my_data);
void set_Enable(bool status);
bool parity (uint8_t lrc);





#endif /* LECTOR_H_ */
