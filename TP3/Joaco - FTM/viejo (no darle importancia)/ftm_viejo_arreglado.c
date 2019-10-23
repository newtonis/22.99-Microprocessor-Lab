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



void OVF_ISR(void)
{
	FTM_Type* ftm_mod = FTM0;
	ftm_mod->SC &= ~FTM_SC_TOF_MASK; //  clear the TimerOverflowFlag
	gpioToggle(PIN_FTM_TEST_OVF);
}





void configPinFtm(pin_t pin, uint8_t mux_alt){
	PORT_Type * port_ftm = portPointers[PIN2PORT(pin)];
	port_ftm->PCR[PIN2NUM(pin)] = 0;
	port_ftm-> PCR[PIN2NUM(pin)] |=   PORT_PCR_DSE(1)| // enable Drive Strength
							PORT_PCR_MUX(mux_alt)| // gpio alternative
							PORT_PCR_IRQC(0); // deshabilito interrupciones

}

void configFtmModuleForOverFlowInterrupts(FTM_Type * ftm_mod){
	ftm_mod->MODE=(ftm_mod->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(1); ///  Enable Timer advanced modes
	ftm_mod->SC = (ftm_mod->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(0); // seteamos el prescaler (divide por 2**(n)) lo que sea que este en CLKS
	ftm_mod->CNTIN=0x00; // es importante inicializarlo antes de setear el modulo
	ftm_mod->MOD=50-1;
	ftm_mod->SC = (FTM0->SC & ~FTM_SC_TOIE_MASK) | FTM_SC_TOIE(1); // habilitamos la interrupcion del timeroverflow
	ftm_mod->SC |= FTM_SC_CLKS(0x01); // seleccionamos system clock , pdria haber sido un frec fija de clock o clk ext
}


void SetInputCaptureOrOutputCompare(FTM_Type * module_pnt ,uint8_t ch_num,bool isOutputCompare){
	// DECAPEN =0 , COMBINE = 0; CPWMS = 0; Msnb 0 MsnA isOutputCompare
	if(module_pnt == FTM0){
		module_pnt->COMBINE &= ~FTM_COMBINE_DECAP0_MASK;
		module_pnt->COMBINE &= ~FTM_COMBINE_COMBINE0_MASK; // COMBINE = 0;
	}else if(module_pnt==FTM3){
		module_pnt->COMBINE &= ~FTM_COMBINE_DECAP3_MASK;
		module_pnt->COMBINE &= ~FTM_COMBINE_COMBINE3_MASK; // COMBINE = 0;
	}

	module_pnt->SC &= ~FTM_SC_CPWMS_MASK; // CPWMS = 0;
	module_pnt->CONTROLS[ch_num].CnSC &= ~FTM_CnSC_MSB_MASK;
	module_pnt->CONTROLS[ch_num].CnSC = (module_pnt->CONTROLS[ch_num].CnSC & ~ FTM_CnSC_MSA_MASK) | FTM_CnSC_MSA(isOutputCompare);
}

void setModeConfig(FTM_Type * module_pnt, uint8_t ch_num, uint8_t mode){
	module_pnt->CONTROLS[ch_num].CnSC &= ~ (FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSA_MASK);
	bool elsb = (mode&(1<<1)) == 1<<1;
	bool elsa = (mode&(1<<0)) == 1<<0;
	module_pnt->CONTROLS[ch_num].CnSC |= FTM_CnSC_ELSB(elsb) | FTM_CnSC_ELSA(elsa); // capture on rising or falling
}

void OVF_Init(){
	configPinFtm(PIN_FTM_TEST_OVF,GPIO_ALTERNATIVE); // configuramos pin a togglear cuando hace overflow el contador
	gpioMode(PIN_FTM_TEST_OVF, OUTPUT);
	configFtmModuleForOverFlowInterrupts(FTM0);
}


void IC_Init (void)
{
	// PTC1 as OC (FTM0-CH0)
	configPinFtm(PIN_OUTPUT_CAPTURE, 4); //alternativa 4

	// PTC8 as GPIO
	configPinFtm(PIN_GPIO_TEST,1);
	gpioMode(PIN_GPIO_TEST, OUTPUT);


	// PTC9 as IC (FTM3-CH5)
	configPinFtm(PIN_INPUT_CAPTURE, 3);


	//  Enable Timer advanced modes (FTMEN=1)

	FTM0->MODE=(FTM0->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(1);

	/// BusClock=sysclk/2= 50MHz
	/// Set prescaler = divx32 => Timer Clock = 32 x (1/BusClock)= 32x1/50MHz= 0.64 useg
	/// =>Timer Clock x OC_Delta= 0.64 x 100=64 useg

	/// ---- Generador


	FTM0->SC = (FTM0->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(FTM_PSC_x32);
	FTM0->CNTIN=0x0000;				  		                 // Free running
	FTM0->MOD=0xFFFF;



	uint8_t ch_num = 0;
	SetInputCaptureOrOutputCompare(FTM0,ch_num,true);
	setModeConfig(FTM0, ch_num, FTM_OC_ToggleOutputOnMatch ); // hay que ver si esto esta bien
	FTM0->CONTROLS[ch_num].CnSC |= FTM_CnSC_CHIE(1); // habilitamos las interrupciones del canal
	FTM0->SC |= FTM_SC_CLKS(0x01); // select clock


	//--- medidor
	FTM3->SC = (FTM3->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(FTM_PSC_x32);
	FTM3->CNTIN=0x0000;				  		  				// Free running
	FTM3->MOD=0xFFFF;
	ch_num = 5;
	SetInputCaptureOrOutputCompare(FTM3,ch_num, false);   // Select IC Function
	setModeConfig(FTM3, ch_num, FTM_IC_CaptureOnEitherEdge);  // Capture on both edges

	FTM3->CONTROLS[ch_num].CnSC |= FTM_CnSC_CHIE(1); // habilitamos las interrupciones del canal
	FTM3->SC |= FTM_SC_CLKS(0x01); // select clock


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


	NVIC_EnableIRQ(FTM0_IRQn);
	NVIC_EnableIRQ(FTM1_IRQn);
	NVIC_EnableIRQ(FTM2_IRQn);
	NVIC_EnableIRQ(FTM3_IRQn);

	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;


	IC_Init();

}
