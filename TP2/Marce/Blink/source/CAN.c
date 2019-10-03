/*
 * CAN.c
 *
 *  Created on: 2 Oct 2019
 *      Author: Marcelo
 */

#include "CAN.h"
#include "SPI.h"
#include <stdbool.h>

//Defines de comandos de SPI
#define RESET 0xC0
#define READ 0x03
#define READ_RX_BUFFER 0x90
#define WRITE 0x02
#define LOAD_RX_BUFFER 0x40
#define RTS_TX0 0x81 // Por ahora configure solo para el buffer 0
#define READ_STATUS 0xA0
#define RX_STATUS 0xB0
#define BIT_MODIFY 0x05

//Defines de direcciones de registros
#define CNF1_REG 0x2A
#define CNF2_REG 0x29
#define CNF3_REG 0x28

#define RXM0SIDH_REG 0x20
#define RXM0SIDL_REG 0x21

#define RXF0SIDH_REG 0x00
#define RXF0SIDL_REG 0x01

#define CANCTRL_REG 0x0F
#define CANINTE_REG 0x2B

#define TXB0CTRL_REG 0x30
#define TXB0SIDH_REG 0x31
#define TXB0SIDL_REG 0x32
#define TXB0DLC_REG 0x35
#define TXB0Dn_REG 0x36

#define TXRTSCTRL_REG 0x0D

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void CAN_BIT_MODIFY(char address, char mask, char data);

void CAN_WRITE(char address, char data);

void CAN_RESET(void);

void CAN_RTS_TXB0(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void CAN_RESET(void)
{
	SPI_ByteWrite(RESET, false);
}


void CAN_BIT_MODIFY(char address, char mask, char data)
{
	int i = 0;
	char buffer[4] = {BIT_MODIFY, address, mask, data};
	for(i = 0; i < 4; i++)
	{
		if(i < 4 - 1)
		{
			SPI_ByteWrite(buffer[i], true);
		}
		else
		{
			SPI_ByteWrite(buffer[i], false);
		}
	}
}

void CAN_WRITE(char address, char data)
{
	int i = 0;
	char buffer[3] = {WRITE, address, data};
	for(i = 0; i < 3; i++)
	{
		if(i < 3 - 1)
		{
			SPI_ByteWrite(buffer[i], true);
		}
		else
		{
			SPI_ByteWrite(buffer[i], false);
		}
	}
}

void CAN_RTS_TXB0(void)
{
	SPI_ByteWrite(RTS_TX0, false);
}


//Oscilador de 16MHz
//Tengo 8 time quantas por bit
//BRP tiene que valer 7
void init_CAN(int ID)
{
	CAN_RESET(); // Reseteo el controlador y lo pongo en modo configuración

	// Seteo el bitrate y los time quantas
	CAN_BIT_MODIFY(CNF1_REG, 0xFF ,7);//aca seteo el time quanta como 1us
	CAN_BIT_MODIFY(CNF2_REG, 0xFF ,0x98); // Aca seteo cuantos time quantas tiene el PHSEG1 y el PRSEG
	CAN_BIT_MODIFY(CNF3_REG, 0x47 ,0x41); // Seteo time quantas asociados a PHSEG2 y WAKFIL

	CAN_BIT_MODIFY(TXRTSCTRL_REG, 0x07, 0x01); // Configuro para utilizar uno solo de los 3 buffers de TX

	CAN_WRITE(RXF0SIDH_REG, 0x20); // Seteo el filtro (H)
	CAN_WRITE(RXF0SIDL_REG, 0x00); // Seteo el filtro (L)
	CAN_WRITE(RXM0SIDH_REG, 0xFF); // Seteo la mask (H)
	CAN_WRITE(RXM0SIDL_REG, 0x00); // Seteo la mask (L)

	CAN_BIT_MODIFY(CANCTRL_REG, 0xEF, 0x0C); // Modo Normal, One-Shot, Clock Enable y Preescaler en 1
}

void send_CAN(int ID, char * buffer, int bufflen)
{
	CAN_BIT_MODIFY(TXB0CTRL_REG, 0x08, 0x00); // Clear de TXREQ

	//CAN_WRITE(CANINTE_REG, 0x00); // Disable Interrupt

	// Cargo ID de destino
	char ID_H = ID >> 3;
	CAN_WRITE(TXB0SIDH_REG, ID_H);
	char ID_L = (ID & 0x07) << 5;
	CAN_WRITE(TXB0SIDL_REG, ID_L);

	// Cuantos bytes voy a enviar
	char DATA_LEN = 0x00 + bufflen;
	CAN_WRITE(TXB0DLC_REG, DATA_LEN);

	// Escribo la data a enviar en el buffer
	CAN_WRITE(TXB0Dn_REG, 0x69);

	CAN_RTS_TXB0(); // Aviso que ya esta para enviar. Cuando detecte libre el bus se manda


}


/*******************************************************************************
 ******************************************************************************/
