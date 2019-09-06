/*
 * Lector.h
 *
 *  Created on: Sep 1, 2019
 *      Author: martinamaspero
 */

#ifndef LECTOR_H_
#define LECTOR_H_

#include <stddef.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PAN_MAX_L 19
#define DATA_LENGTH 40
#define BITSXNUM 5
#define SS 0b01011 // Start Sentinel ';'
#define FS 0b01101 //Field separator '='
#define END_SENTINEL 0b11111 //End sentinel '?'
#define PARITY_BIT_MASK_EVEN 0b00010000
#define PARITY_BIT_MASK_ODD 0b00001111


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Reads the first ID_LENGHT numbers of the card.
 * @return pointer to int array if the data to send is ready or NULL if it isn't or there is an error.
 */
int * lector_get_PAN (void);


bool get_Enable(void);
void clear_Chk(void);

/**
 * @brief initialize the lector
 */
void lectorInit (void);

#endif /* LECTOR_H_ */
