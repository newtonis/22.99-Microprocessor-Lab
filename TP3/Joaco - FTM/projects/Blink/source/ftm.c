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

uint16_t OC_Delta = 200;


/* FTM0 fault, overflow and channels interrupt handler*/

__ISR__ FTM0_IRQHandler(void)
{
	//OVF_ISR(); si tuviera habilitadas las de overflow usaria esto
	OC_ISR();
}

/* FTM3 fault, overflow and channels interrupt handler*/

__ISR__ FTM3_IRQHandler(void)
{
	IC_ISR();
}


void configPinFtm(pin_t pin, uint8_t mux_alt){
	sim_ptr->SCGC5 |= SIM_SCGC5_PORTD_MASK; // clk gating de los pines puerto D y C
	sim_ptr->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	PORT_Type * port_ftm = portPointers[PIN2PORT(pin)];
	port_ftm-> PCR[PIN2NUM(pin)] = PORT_PCR_DSE(1)| // enable Drive Strength
									PORT_PCR_MUX(mux_alt)| // gpio alternative
									PORT_PCR_IRQC(0)|
									PORT_PCR_PS(0)|
									PORT_PCR_PE(0); // deshabilito interrupciones
}


uint32_t FTM_GetCounter (FTM_Type* ftm, uint8_t channel)
{
	return ftm->CONTROLS[channel].CnV & FTM_CnV_VAL_MASK;
}

void FTM_SetCounter (FTM_Type* ftm, uint8_t channel, uint16_t data)
{
	ftm->CONTROLS[channel].CnV = FTM_CnV_VAL(data);
}

void FTM_ClearInterruptFlag (FTM_Type * ftm_module , uint8_t channel){
	ftm_module->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK; // clear interupt status flag
}
void FTM_SetClock(FTM_Type * module){
	module->SC |= FTM_SC_CLKS(0x01); // select clock
}
void FTM_DisableClock(FTM_Type * module){
	module->SC &= ~FTM_SC_CLKS_MASK; // select clock
}

void FTM_EnableInterrupts(FTM_Type * module, uint8_t channel){
	module->CONTROLS[channel].CnSC |= FTM_CnSC_CHIE(1);
}

void OC_ISR(void)  //FTM0 CH0 PTC8 as GPIO
{
	uint8_t channel = 0 ;
	FTM_Type * ftm_module = FTM0;
	if((ftm_module->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK) == FTM_CnSC_CHF_MASK){
		FTM_ClearInterruptFlag(ftm_module, channel);
		gpioToggle(PIN_GPIO_TEST);
		FTM_SetCounter(ftm_module,channel,FTM_GetCounter (ftm_module, channel)+ OC_Delta); // OC + oc_Delta --> oc
	}
	if((ftm_module->SC & FTM_SC_TOIE_MASK) == FTM_SC_TOIE_MASK){
		ftm_module->SC &= ~FTM_SC_TOF_MASK; //  clear the TimerOverflowFlag
	}
}

void IC_ISR(void) //FTM3 CH5 PTC9 as IC
{
	static uint16_t med1,med2,med;
	static uint8_t  state=0;
	FTM_Type * ftm_module = FTM3;
	uint8_t channel = 5;

	if((ftm_module->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK) == FTM_CnSC_CHF_MASK){
		FTM_ClearInterruptFlag (ftm_module, channel);
		if(state==0)
		{
			med1=FTM_GetCounter (ftm_module, channel); //
			state=1;
		}
		else if(state==1)
		{
			med2=FTM_GetCounter (ftm_module, channel);
			med=med2-med1;
			// con prescaler de 32 daba 200 el med y la frecuencia era 3.91k que es 256uS
			// entonces si 256us/2 es hasta que termina de contar FFFF
			// mi tick entonces es de 1,95e-9 o sea 511 megas?? what

			state=0;					// Set break point here and watch "med" value
		}

	}

	if((ftm_module->SC & FTM_SC_TOIE_MASK) == FTM_SC_TOIE_MASK){
		ftm_module->SC &= ~FTM_SC_TOF_MASK; //  clear the TimerOverflowFlag
	}

}
/*
void OVF_ISR(void)
{
	FTM_Type* ftm_mod = FTM0;
	ftm_mod->SC &= ~FTM_SC_TOF_MASK; //  clear the TimerOverflowFlag
	gpioToggle(PIN_FTM_TEST_OVF);
}
*/


void configFtmModuleForOverFlowInterrupts(FTM_Type * ftm_mod){
	ftm_mod->MODE=(ftm_mod->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(1); ///  Enable Timer advanced modes
	ftm_mod->SC = (ftm_mod->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(0); // seteamos el prescaler (divide por 2**(n)) lo que sea que este en CLKS
	ftm_mod->CNTIN=0x00; // es importante inicializarlo antes de setear el modulo
	ftm_mod->MOD=50-1;
	ftm_mod->SC = (FTM0->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(1); // habilitamos la interrupcion del timeroverflow
	ftm_mod->SC |= FTM_SC_CLKS(0x01); // seleccionamos system clock , pdria haber sido un frec fija de clock o clk ext
}
/*
void OVF_Init(){
	configPinFtm(PIN_FTM_TEST_OVF,GPIO_ALTERNATIVE); // configuramos pin a togglear cuando hace overflow el contador
	gpioMode(PIN_FTM_TEST_OVF, OUTPUT);
	configFtmModuleForOverFlowInterrupts(FTM0);
}
*/



void setChannelnMode(FTM_Type* module,uint8_t channel, uint8_t mode, bool isOutputCompare){
	if(module == FTM0){
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_DECAPEN0_MASK)|FTM_COMBINE_DECAPEN0(0);
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_COMBINE0_MASK )|FTM_COMBINE_COMBINE0(0);
	}else if(module == FTM3){
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_DECAPEN3_MASK)|FTM_COMBINE_DECAPEN3(0);
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_COMBINE3_MASK )|FTM_COMBINE_COMBINE3(0);
	}

	module->SC=(module->SC & ~FTM_SC_CPWMS_MASK)|FTM_SC_CPWMS(0);

	module->CONTROLS[channel].CnSC = (FTM0->CONTROLS[channel].CnSC & ~ FTM_CnSC_MSB_MASK)| FTM_CnSC_MSB(0);
	module->CONTROLS[channel].CnSC = (FTM0->CONTROLS[channel].CnSC & ~ FTM_CnSC_MSA_MASK)| FTM_CnSC_MSA(isOutputCompare);

	bool elsb = (mode&(1<<1)) == 1<<1;
	bool elsa = (mode&(1<<0)) == 1<<0;
	module->CONTROLS[channel].CnSC = (module->CONTROLS[channel].CnSC & ~ FTM_CnSC_ELSB_MASK)| FTM_CnSC_ELSB(elsb);
	module->CONTROLS[channel].CnSC = (module->CONTROLS[channel].CnSC & ~ FTM_CnSC_ELSA_MASK)| FTM_CnSC_ELSA(elsa);
}


void initSettings(FTM_Type* module,uint8_t channel, uint8_t prescaler,uint16_t initial, uint16_t final){
	module->MODE = (module->MODE  & ~FTM_MODE_FTMEN_MASK)| FTM_MODE_FTMEN_MASK;//pongo el ftmen en 1
	module->SC = (module->SC 	 & ~FTM_SC_PS_MASK) 	| FTM_SC_PS(prescaler); //indico que utilizo el sitem clock 50M
	module->SC = (module->SC 	 & ~FTM_SC_TOIE_MASK) 	| FTM_SC_TOIE_MASK;//habilito la interrupcion de overflow
	module->MOD = (module->MOD 	 & ~FTM_MOD_MOD_MASK)	| FTM_MOD_MOD(final);//seteo el valor al que llega el contador
	module->CNTIN = (module->CNTIN & ~FTM_CNTIN_INIT_MASK)| FTM_CNTIN_INIT(initial);//seteo el valor inicial del contador
}




void setOC(FTM_Type* module,uint8_t channel){
	bool isOutputCompare = true;
	FTM_DisableClock(module);

	initSettings(module, channel,FTM_PSC_x32,0,0xFFFF);

	setChannelnMode(module,channel, FTM_OC_ToggleOutputOnMatch,isOutputCompare);

	FTM_EnableInterrupts(module, channel);

	FTM_SetClock(module);

	NVIC_EnableIRQ(FTM0_IRQn);

	configPinFtm(PIN_OUTPUT_CAPTURE,4);
	configPinFtm(PIN_GPIO_TEST,1);
	gpioMode(PIN_GPIO_TEST, OUTPUT);
}


void setIC(FTM_Type* module,uint8_t channel){
	bool isOutputCompare = false;
	FTM_DisableClock(module);

	initSettings(module, channel,FTM_PSC_x32,0,0xFFFF);

	setChannelnMode(module,channel, FTM_IC_CaptureOnEitherEdge,isOutputCompare);

	FTM_EnableInterrupts(module, channel);

	FTM_SetClock(module);

	// PTC9 as IC (FTM3-CH5)
	NVIC_EnableIRQ(FTM3_IRQn);

	configPinFtm(PIN_INPUT_CAPTURE, 3);
}


void ftmInit(void){
	// primero hacemos clockgating
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	sim_ptr->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	sim_ptr->SCGC3 |= SIM_SCGC3_FTM3_MASK;


	sim_ptr->SCGC5 |= SIM_SCGC5_PORTD_MASK; // clk gating de los pines puerto D y C
	sim_ptr->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	setIC(FTM3,5); // module = FTM3 , channel = 5
	setOC(FTM0,0); // module = FTM0, channel = 0

}
