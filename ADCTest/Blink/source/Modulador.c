/*
 * Modulador.c
 *
 *  Created on: 19 Oct 2019
 *      Author: Marcelo
 */
#include "Modulador.h"
#include "ftm.h"
#include "math.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SIN_VALUES	83

#define NEXT_SYM	2
#define START		1
#define STOP		0

#define V_LEVEL		50

#define SIN0		0
#define SIN1		1


static uint16_t sinValues[SIN_VALUES];
static uint16_t sinValues1[SIN_VALUES];
static uint16_t cont = 0;

static bool bitStream[STAND_LEN];
static uint8_t msg_ptr = 0;
static bool is_Tx = false;

void (*MsgSendedCallback)(void);
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void sendSignal(void);

void procesBitStream(uint8_t command); // El comando puede ser START o NEXT_SYM

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Modulador_init(void(*funcallback)(void))
{
	MsgSendedCallback = funcallback;

	// Generacion de valores de señal de salida
	for(int i = 0; i < SIN_VALUES; i++) // Mark 0
	{
		sinValues[i] = 2048*(1 + sin(2*2*3.141592*i/SIN_VALUES));
		sinValues1[i] = 2048*(1 + sin(2*3.141592*i/SIN_VALUES));
	}

	ftmInit(sendSignal);
	setDuty(V_LEVEL); // Valor medio por default
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
			is_Tx = true;
			break;
		case NEXT_SYM:
			msg_ptr++;
			if(msg_ptr == STAND_LEN)
			{
				msg_ptr = 0;
				is_Tx = false;
				setDuty(V_LEVEL);
				MsgSendedCallback(); // Avisa que termino de mandar la señal modulada
			}
			else
			{
				msg_ptr;
				// NO TERMINO DE ENVIARSE EL DATO
			}
			break;
	}
}

void sendSignal(void)
{
	if(is_Tx)
	{
		switch (bitStream[msg_ptr]) {
			case 0:
				setDuty(sinValues[cont]/41);
				break;
			case 1:
				setDuty(sinValues1[cont]/41);
				break;
		}

		cont++;
		if(cont == SIN_VALUES)
		{
			cont = 0;
			procesBitStream(NEXT_SYM);
		}
		else
		{
			// NO TERMINO DE TRANSMITIR
		}
	}
}

/*******************************************************************************
 ******************************************************************************/
