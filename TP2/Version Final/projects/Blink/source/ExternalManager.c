/*
 * ExternalManager.c
 *
 *  Created on: 5 Oct 2019
 *      Author: Marcelo
 */

#include "ExternalManager.h"
#include <stdio.h>
#include "SPI.h"
#include "CAN.h"
#include "UART.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
#define R_CHAR		82
#define C_CHAR		67
#define O_CHAR		79
#define PLUS_CHAR	43
#define MINUS_CHAR	45


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
static BoardParams_t Boards[GROUPS];
static uint8_t myGroup;
static char bufferPC[7] = {'2', 'R', '+', '1', '2', '3', '\0'};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void ExternManager_EventHandler(void);

int16_t ExternManager_MakeCANMsj(char* buf, int16_t* boardDATA, uint8_t typeUPD, BoardParams_t myBoard);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void ExternManager_init(uint8_t group_num)
{
	myGroup = group_num;
	SPI_Initialize(NULL);
	init_CAN(myGroup, ExternManager_EventHandler);

	uart_cfg_t config;
	config.baudrate = 4800;
	uartInit(config);
}

void ExternManager_EventHandler(void)
{
	RXB_RAWDATA_t bufferRXB = getRXB_Data_CAN(); // Hago un get de la data recibida

	uint8_t group_index = ((bufferRXB.SID) - 256) - 1;
	uint8_t digitos = bufferRXB.DLC - 2;
	int16_t angle;

	if(digitos == 1)
	{
		angle = (bufferRXB.Dn[2]) - '0';
	}
	else if(digitos == 2)
	{
		angle = (bufferRXB.Dn[2] - '0')*10 + (bufferRXB.Dn[3] - '0');
	}
	else if(digitos == 3)
	{
		angle = (bufferRXB.Dn[2] - '0')*100 + (bufferRXB.Dn[3] - '0')*10 + (bufferRXB.Dn[4] - '0');
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

	// Refresco de PC
	bufferPC[0] = (group_index + 1) + '0';
	bufferPC[1] = bufferRXB.Dn[0];
	if(bufferRXB.Dn[1] == 0)
	{
		bufferPC[2] = PLUS_CHAR;
		bufferPC[3] = '0';
		bufferPC[4] = '0';
		bufferPC[5] = '0';
	}
	else
	{
		if(bufferRXB.Dn[1] == PLUS_CHAR)
		{
			bufferPC[2] = PLUS_CHAR;
		}
		else if(bufferRXB.Dn[1] == MINUS_CHAR)
		{
			bufferPC[2] = MINUS_CHAR;
		}
		bufferPC[3] = (angle / 100) + '0';
		if(angle >= 100)
		{
			bufferPC[4] = ((angle / 10) - 10) + '0';
		}
		else
		{
			bufferPC[4] = (angle / 10) + '0';
		}
		bufferPC[5] = (angle % 10) + '0';
	}

	sendWord(bufferPC);
}

int16_t ExternManager_MakeCANMsj(char* buf, int16_t* boardDATA, uint8_t typeUPD, BoardParams_t myBoard)
{
	int16_t nBytes;

	switch (typeUPD) {
		case ROLL_UPD:
			*boardDATA = myBoard.roll;
			buf[0] = 'R';
			break;
		case PITCHING_UPD:
			*boardDATA = myBoard.pitching;
			buf[0] = 'C';
			break;
		case ORIENTATION_UPD:
			*boardDATA = myBoard.orientation;
			buf[0] = 'O';
			break;
	}

	if(boardDATA == 0)
	{
		buf[1] = 0;
		nBytes = 2;
	}
	else
	{
		// Defino signo
		if(*boardDATA > 0)
		{
			buf[1] = '+';
		}
		else
		{
			buf[1] = '-';
			*boardDATA = -(*boardDATA);
		}

		// Defino digitos
		uint8_t c,d,u;
		c = *boardDATA / 100;
		d = *boardDATA / 10;
		if(d >= 10)
		{
			d = d - 10;
		}
		u = *boardDATA % 10;

		if(c != 0)
		{
			buf[2] = c;
			buf[3] = d;
			buf[4] = u;
			nBytes = 5;
		}
		else
		{
			if(d != 0)
			{
				buf[2] = d;
				buf[3] = u;
				nBytes = 4;
			}
			else
			{
				buf[2] = u;
				nBytes = 3;

			}
		}
	}

	return nBytes;
}

void ExternManager_send2Ext(BoardParams_t myBoard, uint8_t typeUPD)
{
	int16_t boardDATA, nBytes;
	char buffer[5]; // Considero peor caso, pero cuando mando indico cuantos bytes son

	nBytes = ExternManager_MakeCANMsj(buffer, &boardDATA, typeUPD, myBoard);

	buffer[2] = buffer[2] + '0';
	buffer[3] = buffer[3] + '0';
	buffer[4] = buffer[4] + '0';

	disableRXB0_CAN();

	send_CAN(0x102, buffer, nBytes); // Para que no se interrumpa una operacion de envio por SPI

	bufferPC[0] = myGroup + '0';
	bufferPC[1] = buffer[0];
	if(boardDATA != 0)
	{
		bufferPC[2] = buffer[1];
	}
	else
	{
		bufferPC[2] = '+';
	}

	if(nBytes == 2) // Es 0
	{
		bufferPC[3] = '0';
		bufferPC[4] = '0';
		bufferPC[5] = '0';
	}
	else if(nBytes == 3)
	{
		bufferPC[3] = '0';
		bufferPC[4] = '0';
		bufferPC[5] = buffer[2];
	}
	else if(nBytes == 4)
	{
		bufferPC[3] = '0';
		bufferPC[4] = buffer[2];
		bufferPC[5] = buffer[3];
	}
	else if(nBytes == 5)
	{
		bufferPC[3] = buffer[2];
		bufferPC[4] = buffer[3];
		bufferPC[5] = buffer[4];
	}

	sendWord(bufferPC);

	enableRXB0_CAN();

	Boards[myGroup].roll = myBoard.roll;
	Boards[myGroup].pitching = myBoard.pitching;
	Boards[myGroup].orientation = myBoard.orientation;
}

/*******************************************************************************
 ******************************************************************************/
