/*
 * PIT.c
 *
 *  Created on: 24 Oct 2019
 *      Author: Marcelo
 */

#include "PIT.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static PIT_Type* PIT_ptr = PIT;
static SIM_Type* sim_ptr = SIM;

void (*callback0)(void);
void (*callback1)(void);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void PIT_init(void(*funcallback0)(void), void(*funcallback1)(void))
{
	// Clock Gating
	sim_ptr->SCGC6 |= SIM_SCGC6_PIT_MASK;

	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT1_IRQn);

	callback0 = funcallback0;
	callback1 = funcallback1;

	PIT_ptr->MCR &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK);
	PIT_ptr->MCR |= (PIT_MCR_MDIS(0) | PIT_MCR_FRZ(1));

	PIT_ptr->CHANNEL[0].LDVAL = (PIT_LDVAL_TSV(250 - 1));

	PIT_ptr->CHANNEL[0].TCTRL &= ~(PIT_TCTRL_CHN_MASK);
	PIT_ptr->CHANNEL[0].TCTRL |=(PIT_TCTRL_CHN(0));

	PIT_ptr->CHANNEL[0].TCTRL &= ~(PIT_TCTRL_TIE_MASK);
	PIT_ptr->CHANNEL[0].TCTRL |=(PIT_TCTRL_TIE(0));

	PIT_ptr->CHANNEL[0].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
	PIT_ptr->CHANNEL[0].TCTRL |=(PIT_TCTRL_TEN(0));

	PIT_ptr->CHANNEL[1].LDVAL = (PIT_LDVAL_TSV(500 - 1));

	PIT_ptr->CHANNEL[1].TCTRL &= ~(PIT_TCTRL_CHN_MASK);
	PIT_ptr->CHANNEL[1].TCTRL |=(PIT_TCTRL_CHN(0));

	PIT_ptr->CHANNEL[1].TCTRL &= ~(PIT_TCTRL_TIE_MASK);
	PIT_ptr->CHANNEL[1].TCTRL |=(PIT_TCTRL_TIE(0));

	PIT_ptr->CHANNEL[1].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
	PIT_ptr->CHANNEL[1].TCTRL |=(PIT_TCTRL_TEN(0));
}

void PIT0_IRQHandler(void)
{
	//uint32_t flag = PIT_ptr->CHANNEL[0].TFLG;

	callback0();

	PIT_ptr->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
}

void PIT1_IRQHandler(void)
{
	//uint32_t flag = PIT_ptr->CHANNEL[1].TFLG;

	callback1();

	PIT_ptr->CHANNEL[1].TFLG = PIT_TFLG_TIF(1);
}

void PIT_startTime(uint8_t timer_id)
{
	PIT_ptr->CHANNEL[timer_id].TCTRL &= ~(PIT_TCTRL_TIE_MASK);
	PIT_ptr->CHANNEL[timer_id].TCTRL |=(PIT_TCTRL_TIE(1));

	PIT_ptr->CHANNEL[timer_id].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
	PIT_ptr->CHANNEL[timer_id].TCTRL |=(PIT_TCTRL_TEN(1));
}

void PIT_stopTime(uint8_t timer_id)
{
	PIT_ptr->CHANNEL[timer_id].TCTRL &= ~(PIT_TCTRL_TIE_MASK);
	PIT_ptr->CHANNEL[timer_id].TCTRL |=(PIT_TCTRL_TIE(0));

	PIT_ptr->CHANNEL[timer_id].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
	PIT_ptr->CHANNEL[timer_id].TCTRL |=(PIT_TCTRL_TEN(0));
}

/*******************************************************************************
 ******************************************************************************/
