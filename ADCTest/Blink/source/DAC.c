/*
 * DAC.c
 *
 *  Created on: 18 Oct 2019
 *      Author: Marcelo
 */

#include "DAC.h"
#include "hardware.h"

#include "timer.h"
#include "math.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DACData_LOW_LEN		8
#define SIN_VALUES			500

static DAC_Type* DAC0_ptr = DAC0;
static SIM_Type* sim_ptr = SIM;

static tim_id_t timerSin;
static uint16_t sinValues[SIN_VALUES];
static uint16_t cont = 0;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void DAC_setData(DACDATA_t data);

void testSin(void);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void DAC_init(void)
{
	// Clock Gating
	sim_ptr->SCGC2 |= SIM_SCGC2_DAC0_MASK;

	DAC0_ptr->C0 &= ~(DAC_C0_DACEN_MASK |
					 DAC_C0_DACRFS_MASK |
					 DAC_C0_DACTRGSEL_MASK |
					 DAC_C0_LPEN_MASK);
	DAC0_ptr->C0 |= (DAC_C0_DACEN(1) |			// Enable module
					 DAC_C0_DACRFS(1) |			// Reference voltage DACREF_2
					 DAC_C0_DACTRGSEL(1) |		// Software triggered
					 DAC_C0_LPEN(1));			// Low-power mode

	DAC0_ptr->C1 &= ~(DAC_C1_DMAEN_MASK |
					 DAC_C1_DACBFMD_MASK |
					 DAC_C1_DACBFEN_MASK);
	DAC0_ptr->C1 |= (DAC_C1_DMAEN(0) |			// DMA disabled
					 DAC_C1_DACBFMD(0) |		// Normal Mode (circular buffer)
					 DAC_C1_DACBFEN(0));		// Buffer PROVISORIAMENTE DESHABILITADO

	timerInit();
	timerSin = timerGetId();
	timerStart(timerSin, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, testSin);

	for(int i = 0; i < SIN_VALUES; i++)
	{
		sinValues[i] = 2048*(1 + sin(2*3.141592*i/SIN_VALUES));
	}
}

void DAC_setData(DACDATA_t data)
{
	DAC0_ptr->DAT->DATL = DAC_DATL_DATA0(data);
	DAC0_ptr->DAT->DATH = DAC_DATH_DATA1(data >> DACData_LOW_LEN);
}

void testSin(void)
{
	DAC_setData(sinValues[cont]);
	cont++;
	if(cont == SIN_VALUES)
	{
		cont = 0;
	}
}

/*******************************************************************************
 ******************************************************************************/
