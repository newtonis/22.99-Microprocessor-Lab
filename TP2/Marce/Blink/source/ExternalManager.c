/*
 * ExternalManager.c
 *
 *  Created on: 5 Oct 2019
 *      Author: Marcelo
 */

#include "ExternalManager.h"
#include "SPI.h"
#include "CAN.h"
#include "Posicionamiento.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
#define R_CHAR		0x82
#define C_CHAR		0x67
#define O_CHAR		0x79
#define PLUS_CHAR	0x43
#define MINUS_CHAR	0x45

typedef struct
{
	roll_t roll;
	pitching_t pitching;
	orientation_t orientation;
}BoardParams_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
BoardParams_t Boards[GROPUS];
uint8_t myGroup;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void ExternManager_EventHandler(void);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void ExternManager_init(uint8_t group_num)
{
	myGroup = group_num;
	SPI_driver_init();
	init_CAN(myGroup, ExternManager_EventHandler);
}

void ExternManager_EventHandler(void)
{
	RXB_RAWDATA_t bufferRXB = getRXB_Data_CAN(); // Hago un get de la data recibida

	uint8_t group_index = ((bufferRXB.SID) & 0x001) - 1;
	uint8_t digitos = bufferRXB.DLC - 2;
	uint8_t angle;

	if(digitos == 1)
	{
		angle = bufferRXB.Dn[2];
	}
	else if(digitos == 2)
	{
		angle = bufferRXB.Dn[2]*10 + bufferRXB.Dn[3];
	}
	else if(digitos == 3)
	{
		angle = bufferRXB.Dn[2]*100 + bufferRXB.Dn[3]*10 + bufferRXB.Dn[4];
	}

	switch (bufferRXB.Dn[1]) {
		case PLUS_CHAR:
			break;
		case MINUS_CHAR:
			angle = -angle;
			break;
		default:
			// Si no es + ni - seguro es un 0
			angle = 0;
			break;
	}

	switch (bufferRXB.Dn[0]) {
		case R_CHAR:
			Boards[group_index].roll = angle;
			break;
		case C_CHAR:
			Boards[group_index].pitching = angle;
			break;
		case O_CHAR:
			Boards[group_index].orientation = angle;
			break;
	}
}

/*******************************************************************************
 ******************************************************************************/
