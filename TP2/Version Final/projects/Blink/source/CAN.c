/*
 * CAN.c
 *
 *  Created on: 2 Oct 2019
 *      Author: Marcelo
 */

#include "CAN.h"
#include "SPI.h"
#include "board.h"
#include <stdio.h>

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
#define RXB0SIDH_REG 0x61
#define RXB0SIDL_REG 0x62
#define RXB0DLC_REG	0x65

#define CANCTRL_REG 0x0F
#define CANINTE_REG 0x2B
#define CANINTF_REG 0x2C

#define TXB0CTRL_REG 0x30
#define TXB0SIDH_REG 0x31
#define TXB0SIDL_REG 0x32
#define TXB0DLC_REG 0x35
#define TXB0D0_REG 0x36

#define TXRTSCTRL_REG 0x0D

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
static RXB_RAWDATA_t RXbufferData;
static uint16_t myID;

static void (*callback)(void);
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void CAN_BIT_MODIFY(char address, char mask, char data);

void CAN_WRITE(char address, char data);

void CAN_RESET(void);

void CAN_RTS_TXB0(void);

void CAN_LOAD_TX_BUFFER(char address, char *data, char nBytes);

void CAN_READ_RX_BUFFER(void);

void CAN_ClearTxFlag(void);

void CAN_ClearRxFlag(void);

uint8_t CAN_READ(char address);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void CAN_RESET(void)
{
	CAN_BIT_MODIFY(CANCTRL_REG, 0xE0, 0x80); // Modo Configuracion
}

void CAN_ClearTxFlag(void)
{
	CAN_BIT_MODIFY(CANINTF_REG, 0x04, 0x00);
}

void CAN_ClearRxFlag(void)
{
	CAN_BIT_MODIFY(CANINTF_REG, 0X01, 0x00);
}

bool getTXFlag_CAN(void)
{
	return (CAN_READ(CANINTF_REG) & 0x04);
}

void CAN_BIT_MODIFY(char address, char mask, char data)
{
	unsigned char buffer[4] = {BIT_MODIFY, address, mask, data};
	SPI_driver_sendRecive(buffer, 4, NULL);
}

void CAN_WRITE(char address, char data)
{
	unsigned char buffer[3] = {WRITE, address, data};
	SPI_driver_sendRecive(buffer, 3, NULL);
}

void CAN_RTS_TXB0(void)
{
	unsigned char buffer[1] = {RTS_TX0};
	SPI_driver_sendRecive(buffer, 1, NULL);
}

void CAN_LOAD_TX_BUFFER(char address, char *data, char nBytes)
{
	int i = 0;
	unsigned char buffer[nBytes + 1]; // Ojo que nBytes solo puede ir hasta 8

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

	SPI_driver_sendRecive(buffer, nBytes+1, NULL);
}

uint8_t CAN_READ(char address){
	unsigned char buffer[3] = {READ, address, 0x00};
	unsigned char recBuffer[3] = {0, 0, 0};

	SPI_driver_sendRecive(buffer, 3, recBuffer);

	return recBuffer[2];
}

void CAN_READ_RX_BUFFER(void)
{
	uint8_t IE_Flags = CAN_READ(CANINTF_REG);
	bool TX_Flag = IE_Flags & 0x04;
	bool RX_Flag = IE_Flags & 0x01;

	if(TX_Flag)
	{
		CAN_ClearTxFlag();
	}
	if(RX_Flag)
	{
		uint8_t bytesRX = 0;
		uint16_t idRX = 0;

		unsigned char bufferSend[4] = {READ, RXB0SIDH_REG, 0x00, 0x00};
		uint8_t bufferID[4]; // Me interesan los ultimos 2
		SPI_driver_sendRecive(bufferSend, 4, bufferID);

		idRX = (bufferID[2] << 3) | ((bufferID[3] & 0xE0) >> 5);

		bytesRX = 0x0F & CAN_READ(RXB0DLC_REG); // Numero de bytes recibidos

		unsigned char bufferSend2[9] = {READ_RX_BUFFER, 0, 0, 0, 0, 0, 0, 0, 0};
		unsigned char bufferDATA[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // Buffer data
		SPI_driver_sendRecive(bufferSend2, 9, bufferDATA);

		RXbufferData.SID = idRX;
		RXbufferData.DLC = bytesRX;

		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			RXbufferData.Dn[i] = bufferDATA[i+1];
		}

		callback();

		CAN_ClearRxFlag();
	}
}

RXB_RAWDATA_t getRXB_Data_CAN(void)
{
	return RXbufferData;
}

//Oscilador de 16MHz
//Tengo 8 time quantas por bit
//BRP tiene que valer 7
void init_CAN(int ID, void (*funcallback)(void))
{
	myID = 0x100 + ID;
	callback = funcallback;

	gpioMode(IRQ_CAN, INPUT);
	gpioIRQ(IRQ_CAN, GPIO_IRQ_MODE_FALLING_EDGE, CAN_READ_RX_BUFFER);
	NVIC_EnableIRQ(PORTB_IRQn);

	unsigned char res = RESET;
	SPI_driver_sendRecive(&res, 1, NULL);
	CAN_RESET(); // Reseteo el controlador y lo pongo en modo configuración

	// Seteo el bitrate y los time quantas
	CAN_BIT_MODIFY(CNF1_REG, 0xFF, 0x07); //aca seteo el time quanta como 1us

	CAN_BIT_MODIFY(CNF2_REG, 0xFF ,0x98); // Aca seteo cuantos time quantas tiene el PHSEG1 y el PRSEG
	CAN_BIT_MODIFY(CNF3_REG, 0x47 ,0x41); // Seteo time quantas asociados a PHSEG2 y WAKFIL

	CAN_BIT_MODIFY(TXRTSCTRL_REG, 0x07, 0x01); // Configuro para utilizar uno solo de los 3 buffers de TX

	CAN_WRITE(RXF0SIDH_REG, 0x20); // Seteo el filtro (H)
	CAN_WRITE(RXF0SIDL_REG, 0x00); // Seteo el filtro (L)
	CAN_WRITE(RXM0SIDH_REG, 0xFF); // Seteo la mask (H)
	CAN_WRITE(RXM0SIDL_REG, 0x00); // Seteo la mask (L)

	CAN_BIT_MODIFY(RXB0CTRL_REG, 0x64, 0x00); // Habilito el filtro y desactivo rollover

	CAN_BIT_MODIFY(CANCTRL_REG, 0xEF, 0x04); // Modo Normal, One-Shot, Clock Enable y Preescaler en 1
	//CAN_BIT_MODIFY(CANCTRL_REG, 0xEF, 0x44); // Modo Loopback, One-Shot, Clock Enable y Preescaler en 1

	CAN_BIT_MODIFY(CANINTE_REG, 0x05, 0x05); // Habilito interrupciones por TX y RX
}

void send_CAN(int ID, char * buffer, int bufflen)
{
	CAN_BIT_MODIFY(TXB0CTRL_REG, 0x08, 0x00); // Clear de TXREQ

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
}


/*******************************************************************************
 ******************************************************************************/
