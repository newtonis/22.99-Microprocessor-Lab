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
#define READ_RX_BUFFER 0x92
#define WRITE 0x02
#define LOAD_TX_BUFFER 0x40
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

#define RXB0CTRL_REG 0x60
#define BFPCTRL_REG 0x0C
#define RXBxSIDH_REG 0x61

#define CANCTRL_REG 0x0F
#define CANINTE_REG 0x2B

#define TXB0CTRL_REG 0x30
#define TXB0SIDH_REG 0x31
#define TXB0SIDL_REG 0x32
#define TXB0DLC_REG 0x35
#define TXB0D0_REG 0x36

#define TXRTSCTRL_REG 0x0D

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void CAN_BIT_MODIFY(char address, char mask, char data);

void CAN_WRITE(char address, char data);

void CAN_RESET(void);

void CAN_RTS_TXB0(void);

void CAN_LOAD_TX_BUFFER(char address, char *data, char nBytes);

void CAN_READ_RX_BUFFER(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void CAN_RESET(void)
{
	CAN_BIT_MODIFY(CANCTRL_REG, 0xE0, 0x80); // Modo Configuracion
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

void CAN_LOAD_TX_BUFFER(char address, char *data, char nBytes)
{
	int i = 0;
	char buffer[nBytes + 1]; // Ojo que nBytes solo puede ir hasta 8

	for(i = 0; i < nBytes; i++)
	{
		buffer[i+1] = data[i];
	}

	switch (address) { // Define el comando
		case TXB0SIDH_REG:
			buffer[0] = 0x40;
			break;
		case TXB0D0_REG:
			buffer[0] = 0x41;
			break;
		default:
			break;
	}

	for(i = 0; i < nBytes + 1; i++)
	{
		if(i < nBytes)
		{
			SPI_ByteWrite(buffer[i], true);
		}
		else
		{
			SPI_ByteWrite(buffer[i], false);
		}
	}
}

void CAN_READ_RX_BUFFER(void)
{
	spi_command command;
	command.keepAssertedPCSnBetweenTransfers = true; // 1 si mantener o 0 para bajarlo
	command.isEndOfQueue = false;
	command.whichPcs = Pcs0;
	command.whichCtar = 0;
	command.clearTransferCount = 0;

	uint16_t data;
	SPI_ByteWrite(READ_RX_BUFFER, true);
	data = SPI_BufferReadCommand(0x00, false, 0, &command);
	data = 1;
}

//Oscilador de 16MHz
//Tengo 8 time quantas por bit
//BRP tiene que valer 7
void init_CAN(int ID)
{
	CAN_RESET(); // Reseteo el controlador y lo pongo en modo configuración

	// Seteo el bitrate y los time quantas
	CAN_BIT_MODIFY(CNF1_REG, 0xFF ,7); //aca seteo el time quanta como 1us
// TEST
	SPI_ByteWrite(READ, true);
	SPI_ByteWrite(CNF1_REG, true);
	spi_command command;
	command.keepAssertedPCSnBetweenTransfers = true; // 1 si mantener o 0 para bajarlo
	command.isEndOfQueue = false;
	command.whichPcs = Pcs0;
	command.whichCtar = 0;
	command.clearTransferCount = 0;

	uint16_t data;
	SPI_ByteWrite(READ_RX_BUFFER, true);
	data = SPI_BufferReadCommand(0x00, false, 0, &command);
	data = 1;
// FIN DE TEST
	CAN_BIT_MODIFY(CNF2_REG, 0xFF ,0x98); // Aca seteo cuantos time quantas tiene el PHSEG1 y el PRSEG
	CAN_BIT_MODIFY(CNF3_REG, 0x47 ,0x41); // Seteo time quantas asociados a PHSEG2 y WAKFIL

	CAN_BIT_MODIFY(TXRTSCTRL_REG, 0x07, 0x01); // Configuro para utilizar uno solo de los 3 buffers de TX

	CAN_WRITE(RXF0SIDH_REG, 0x20); // Seteo el filtro (H)
	CAN_WRITE(RXF0SIDL_REG, 0x00); // Seteo el filtro (L)
	CAN_WRITE(RXM0SIDH_REG, 0xFF); // Seteo la mask (H)
	CAN_WRITE(RXM0SIDL_REG, 0x00); // Seteo la mask (L)

	CAN_BIT_MODIFY(RXB0CTRL_REG, 0x64, 0x00); // Habilito el filtro y desactivo rollover

	CAN_BIT_MODIFY(CANCTRL_REG, 0xEF, 0x0C); // Modo Normal, One-Shot, Clock Enable y Preescaler en 1
	//CAN_BIT_MODIFY(CANCTRL_REG, 0xEF, 0x4C); // Modo Loopback, One-Shot, Clock Enable y Preescaler en 1
	//CAN_BIT_MODIFY(BFPCTRL_REG, 0x0F, 0x05); // Habilito interrupcion de RX0BF
}

void send_CAN(int ID, char * buffer, int bufflen)
{
	CAN_BIT_MODIFY(TXB0CTRL_REG, 0x08, 0x00); // Clear de TXREQ

	//CAN_WRITE(CANINTE_REG, 0x00); // Disable Interrupt

	// Cargo ID de destino
	char ID_H = ID >> 3;
	char ID_L = (ID & 0x07) << 5;
	char id_buffer[2] = {ID_H, ID_L};
	CAN_LOAD_TX_BUFFER(TXB0SIDH_REG, id_buffer, 2);

	// Cuantos bytes voy a enviar
	char DATA_LEN = 0x00 + bufflen;
	CAN_WRITE(TXB0DLC_REG, DATA_LEN);

	// Escribo la data a enviar en el buffer
	CAN_LOAD_TX_BUFFER(TXB0D0_REG, buffer, bufflen);

	CAN_RTS_TXB0(); // Aviso que ya esta para enviar. Cuando detecte libre el bus se manda

	//CAN_READ_RX_BUFFER(); // TESTEO DE RX
}


/*******************************************************************************
 ******************************************************************************/
