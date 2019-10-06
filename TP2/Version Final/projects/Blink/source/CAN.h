/*
 * CAN.h
 *
 *  Created on: 2 Oct 2019
 *      Author: Marcelo
 */

#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
#define BUFFER_SIZE	8

typedef struct
{
	uint16_t SID;
	uint8_t DLC;
	uint8_t Dn[BUFFER_SIZE];
}RXB_RAWDATA_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void init_CAN(int ID, void (*funcallback)(void));

void send_CAN(int ID, char * buffer, int bufflen);

RXB_RAWDATA_t getRXB_Data_CAN(void);

bool getTXFlag_CAN(void);

void enableRXB0_CAN(void);

void disableRXB0_CAN(void);

#endif /* CAN_H_ */
