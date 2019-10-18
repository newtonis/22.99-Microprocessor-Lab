/*
 * ftm.c
 *
 *  Created on: 18 oct. 2019
 *      Author: joa-m
 */
#include "ftm.h"

FTM_Type * ftm_pointers[] = FTM_BASE_PTRS;
static SIM_Type* sim_ptr = SIM;
static PORT_Type* portPointers[] = PORT_BASE_PTRS;

__ISR__ FTM0_IRQHandler(void)
{
	OVF_ISR();
}

void OVF_ISR(void)
{
	FTM_Type* ftm_mod = ftm_pointers[FTM_MODULE_NUMBER];
	ftm_mod->SC &= ~FTM_SC_TOF_MASK;
	gpioToggle(PIN_FTM);
}

void ftmInit(void){
	// primero hacemos clockgating
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	sim_ptr->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	sim_ptr->SCGC3 |= SIM_SCGC3_FTM3_MASK;

	sim_ptr->SCGC5 |= SIM_SCGC5_PORTD_MASK; // activo el puerto

	NVIC_EnableIRQ(FTM0_IRQn);
	//NVIC_EnableIRQ(FTM1_IRQn);
	//NVIC_EnableIRQ(FTM2_IRQn);
	//NVIC_EnableIRQ(FTM3_IRQn);

	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	//FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	//FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	//FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;



	PORT_Type * ftm_pin = portPointers[PIN2PORT(PIN_FTM)];
	uint32_t num = PIN2NUM(PIN_FTM);
	ftm_pin->PCR[num] = 0;
	ftm_pin-> PCR[num] |=   PORT_PCR_DSE(1)| // enable Drive Strength
							PORT_PCR_MUX(1)| // gpio alternative
							PORT_PCR_IRQC(GPIO_IRQ_MODE_FALLING_EDGE); // interrupciones deshabilitadas
	gpioMode(PIN_FTM, OUTPUT);




	FTM_Type* ftm_mod = ftm_pointers[FTM_MODULE_NUMBER];

	ftm_mod->MODE=(ftm_mod->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(1); ///  Enable Timer advanced modes
	ftm_mod->SC = (ftm_mod->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(0); // seteamos el prescaler
	ftm_mod->CNTIN=0x00;
	ftm_mod->MOD=50-1;
	ftm_mod->SC = (FTM0->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(1);
	ftm_mod->SC |= FTM_SC_CLKS(0x01);


}
