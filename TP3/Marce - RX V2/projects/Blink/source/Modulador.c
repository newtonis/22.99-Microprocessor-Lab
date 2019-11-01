/*
 * Modulador.c
 *
 *  Created on: 19 Oct 2019
 *      Author: Marcelo
 */
#include "Modulador.h"
#include "DMA.h"
#include "DecodeBits.h" // PROVISORIO
#include "math.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SIN_VALUES	83

#define IDLE		3
#define NEXT_SYM	2
#define START		1
#define STOP		0

#define SIN0		0
#define SIN1		1

#define TOBITMASK 		10000000

static uint16_t sinValues[SIN_VALUES];
static uint16_t sinValues1[SIN_VALUES];
static bool rts = false;
static bool llegoAlgo = false;

static bool bitStream[STAND_LEN];
static uint8_t msg_ptr = 0;
static bool is_Tx = false;

void (*MsgSendedCallback)(void);
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void procesBitStream(uint8_t command); // El comando puede ser START o NEXT_SYM

void Modulador_sendStream(bool *stream);

bool parity (char num);

void setNextBit(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Modulador_init(uint16_t* dutyAddress, void(*funcallback)(void))
{
	MsgSendedCallback = funcallback;

	// Generacion de valores de señal de salida
	for(int i = 0; i < SIN_VALUES; i++) // Mark 0
	{
		sinValues[i] = 2048*(1 + sin(2*2*3.141592*i/SIN_VALUES))/41;
		if(sinValues[i] == 0)
		{
			sinValues[i] = sinValues[i] + 1;
		}
		sinValues1[i] = 2048*(1 + sin(2*3.141592*i/SIN_VALUES))/41;
		if(sinValues1[i] == 0)
		{
			sinValues1[i] = sinValues1[i] + 1;
		}
	}

	DMA0_Config(sinValues1, dutyAddress, setNextBit);

	DMA0_ConfigCounters(0, sizeof(sinValues), sizeof(sinValues[0]));

	DMA0_EnableRequest(0);
}

void Modulador_sendChar(char my_char)
{
	char msg_t = my_char;

	bitStream[0] = false; //START
	bitStream[STAND_LEN - 1] = true; //STOP
	bitStream[STAND_LEN - 2] = parity(msg_t);

	for(int i = 1; i < 9; i++)
	{
		bitStream[i] = (msg_t & TOBITMASK);
		msg_t = msg_t<<1;
	}

	llegoAlgo = true;

	while(!rts)
	{

	}

	llegoAlgo = false;
	rts = false;

	procesBitStream(START);
}


void Modulador_sendStream(bool *stream)
{
	for(int i = 0; i < STAND_LEN; i++)
	{
		bitStream[i] = stream[i];
	}

	procesBitStream(START);
}

void procesBitStream(uint8_t command)
{
	switch (command) {
		case START:
			if(!is_Tx)
			{
				if(bitStream[msg_ptr] == SIN0)
				{
					DMA0_ConfigSourceAddress(0, sinValues);
				}
				else
				{
					DMA0_ConfigSourceAddress(0, sinValues1);
				}
				DMA0_EnableRequest(0);
				is_Tx = true;
			}
			break;
		case NEXT_SYM:
			msg_ptr++;
			if(msg_ptr == STAND_LEN)
			{
				msg_ptr = 0;
				is_Tx = false;
				DMA0_ConfigSourceAddress(0, sinValues1); // Estado de IDLE
				DMA0_EnableRequest(0);
				MsgSendedCallback(); // Avisa que termino de mandar la señal modulada
			}
			else
			{
				if(bitStream[msg_ptr] == SIN0)
				{
					DMA0_ConfigSourceAddress(0, sinValues);
				}
				else
				{
					DMA0_ConfigSourceAddress(0, sinValues1);
				}
				DMA0_EnableRequest(0);
				// NO TERMINO DE ENVIARSE EL DATO
			}
			break;
		case IDLE:
			// Estado de IDLE es un 1
			break;
	}
}

void setNextBit(void)
{
	if(is_Tx)
	{
		DMA0_DisableRequest(0);
		procesBitStream(NEXT_SYM);
	}
	else
	{
		if(llegoAlgo)
		{
			rts = true;
		}
		procesBitStream(IDLE);
	}
}

bool parity (char num)
{
	bool par = 1;
	for (int i = 0 ; i < 7; i++)
	{
		par ^= ((num & (1<<i))>>i);
	}
	return par;
}

/*******************************************************************************
 ******************************************************************************/
