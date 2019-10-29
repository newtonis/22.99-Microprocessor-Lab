/*
 * demodulatorFSK.c
 *
 *  Created on: Oct 18, 2019
 *      Author: martinamaspero
 */

#include "demodulatorFSK.h"
#include "ADC.h"
#include "PIT.h"

#define ADC_CH 0

typedef enum {IDLE, MSG}status_t;

void filterSignal (void);
void updateData (uint16_t newdata);
void FSKdemodulate(void);

int16_t rawData [ DELAY + 1] = {0};
float prefilter[FIR_ORDER+1] = {0};
bool output[10];
bool msg[11];
bool value = true;
bool firstZero = true;

void (*DemCallback)(void);


int outputCount = 0;
int sampleCount = 0;

static status_t status = IDLE;


static float firCoef[ ] =
		{-0.0018693054003477631f,
		-0.0063458560700613868f,
		-0.011803262238256836f,
		-0.011431063027960632f,
		 0.0055142394274450349f,
		 0.047971856405729658f,
		 0.11520334422221702f,
		 0.1924527660178473f,
		 0.2548404235990876f,
		 0.27884873508664521f,
		 0.2548404235990876f,
		 0.1924527660178473f,
		 0.11520334422221702f,
		 0.047971856405729658f,
		 0.0055142394274450349f,
		-0.011431063027960632f,
		-0.011803262238256836f,
		-0.0063458560700613868f,
		-0.0018693054003477631f };

void FSKdem_init(callback_t callback)
{

	DemCallback = callback;
	ADC_init(ADC_CH, FSKdemodulate);
	ADC_enableModule(ADC_CH);


	PIT_configTimer(2, 4167, ADC_startConvertion);//4166 es 12k
	PIT_startTime(2);
}


void FSKdemodulate()
{


	uint8_t sum;
	updateData( ADC_getDataResult() );
	for(int i=1; i < (1+ FIR_ORDER); i++)
	{
		prefilter[i-1] = prefilter[i];
	}
	prefilter[FIR_ORDER] = rawData[DELAY]*rawData[0];
	filterSignal();
	sampleCount++;


	if (status == MSG)
	{
		if (sampleCount == 10)
		{
			sampleCount = 0;
			for(int j = 0; j<10 ; j++)
			{
				sum+= output[j];
			}
			if (sum >= 5)
			{
				msg[outputCount] = true;
			}
			else
			{
				msg[outputCount] = false;
			}

			outputCount++;
			if (outputCount == 11)
			{
				status = IDLE;
				//callback();
			}


			//estado idle, si llega 1 no hago nada, si llega cero paso a estado mensaje,
			//guardo el cero y depues empiezo a guardar hasta llegar a 11 bits.
			//me falta que cada vez que hago una suma resetar el array output
		}
	}
	else
	{
		if (sampleCount == 10)
		{
			for(int j = 0; j<10 ; j++)
			{
				sum+= output[j];
			}
 			if (sum >= 5)
			{
				value = true;
			}
			else
			{
				value = false;
			}

			if ((value == false))
			{
				if((firstZero == false))
				{
					outputCount = 1;
					sampleCount = 0;
					status = MSG;
					msg[0] = false;
				}
				else
				{
					firstZero = false; //el primer
				}

			}
			else
			{
				sampleCount = 9;
				for(int i=1; i < 10; i++)
				{
					output[i-1] = output[i];
				}

			}
		}
	}
}

void updateData (uint16_t newdata)
{
	for(int i=1; i < (1+ DELAY); i++)
	{
		rawData[i-1] = rawData[i];
	}
	rawData[DELAY] = newdata - 2048;
}

void filterSignal (void)
{
	float aux = 0;

	for (int k = 0; k <= FIR_ORDER ; k++ )
	{
		aux += firCoef[k] * prefilter[FIR_ORDER - k];
	}

	if (aux < 0)
	{
		output[sampleCount] = true;
	}
	else
	{
		output[sampleCount] = false;
	}

}

bool * get_Msg (void)
{
	return msg;
}
