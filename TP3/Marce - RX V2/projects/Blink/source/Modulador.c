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


static uint16_t sinValues[SIN_VALUES];
static uint16_t sinValues1[SIN_VALUES];

static bool bitStream[STAND_LEN];
static uint8_t msg_ptr = 0;
static bool is_Tx = false;

void (*MsgSendedCallback)(void);
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void procesBitStream(uint8_t command); // El comando puede ser START o NEXT_SYM

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

	DMA_Config(sinValues1, dutyAddress, setNextBit);

	DMA_ConfigCounters(sizeof(sinValues), sizeof(sinValues[0]));

	DMA_DisableRequest();
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
			if(bitStream[msg_ptr] == SIN0)
			{
				DMA_ConfigSourceAddress(sinValues);
			}
			else
			{
				DMA_ConfigSourceAddress(sinValues1);
			}
			DMA_EnableRequest();
			is_Tx = true;
			break;
		case NEXT_SYM:
			msg_ptr++;
			if(msg_ptr == STAND_LEN)
			{
				msg_ptr = 0;
				is_Tx = false;
				DMA_ConfigSourceAddress(sinValues1); // Estado de IDLE
				DMA_EnableRequest();
				MsgSendedCallback(); // Avisa que termino de mandar la señal modulada
			}
			else
			{
				if(bitStream[msg_ptr] == SIN0)
				{
					DMA_ConfigSourceAddress(sinValues);
				}
				else
				{
					DMA_ConfigSourceAddress(sinValues1);
				}
				DMA_EnableRequest();
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
		DMA_DisableRequest();
		procesBitStream(NEXT_SYM);
	}
	else
	{
		procesBitStream(IDLE);
	}
}

/*******************************************************************************
 ******************************************************************************/
