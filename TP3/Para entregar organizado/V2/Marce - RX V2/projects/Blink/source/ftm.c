/*
 * ftm.c
 *
 *  Created on: 18 oct. 2019
 *      Author: joa-m
 */
#include "ftm.h"
#include "DMA.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

static SIM_Type* sim_ptr = SIM;
static PORT_Type* portPointers[] = PORT_BASE_PTRS;

uint16_t OC_Delta = 100;

#define PWM_MOD 500-1
#define PWM_DUTY_CYCLE 1000

void (*callback)(void);

static uint16_t dutys = 0;
uint32_t med = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
__ISR__ FTM0_IRQHandler(void);

__ISR__ FTM3_IRQHandler(void);

bool isChannelnInterrupt(FTM_Type* module, uint8_t channel);

void FTM_DisableInterrupts(FTM_Type * module, uint8_t channel);

bool isTimerOverFlowInterrupt(FTM_Type* module);

void updateDuty(FTM_Type* module,uint8_t channel, uint8_t DCpercent);

void PWM_ISR (void);

void OVF_ISR(void);

void IC_ISR(void);

void OC_ISR(void);

void configPinFtm(pin_t pin, uint8_t mux_alt);

void clearTimerOverFlowFlag(FTM_Type* module);

uint32_t FTM_GetCounter(FTM_Type* ftm, uint8_t channel);

void FTM_SetCounter (FTM_Type* ftm, uint8_t channel, uint16_t data);

void FTM_ClearInterruptFlag (FTM_Type * module , uint8_t channel);

void FTM0_DmaMode (uint8_t channel, bool dma_mode);

void FTM3_DmaMode (uint8_t channel, bool dma_mode);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/* FTM0 fault, overflow and channels interrupt handler*/
uint16_t* getDutyAddress(void)
{
	return &dutys;
}

__ISR__ FTM0_IRQHandler(void)
{
	//OVF_ISR(); si tuviera habilitadas las de overflow usaria esto
	//OC_ISR();
	PWM_ISR();
}

/* FTM3 fault, overflow and channels interrupt handler*/

__ISR__ FTM3_IRQHandler(void)
{
	IC_ISR();
}

void PWM_ISR (void)
{
	uint8_t channel = 0;
	FTM_Type * module = FTM0;

	if (isTimerOverFlowInterrupt(module)){ // CNT == MOD
		clearTimerOverFlowFlag(module);
		updateDuty(module, channel, dutys);
		//callback();
	}

	if(isChannelnInterrupt(module,channel)){// CNT = CnV
		FTM_ClearInterruptFlag(module, channel);
	}
}

void setDuty(uint8_t duty_cycle){
	dutys = duty_cycle;
}

bool isTimerOverFlowInterrupt(FTM_Type* module){
	return (module->SC & FTM_SC_TOF_MASK) == FTM_SC_TOF_MASK;
}

void clearTimerOverFlowFlag(FTM_Type* module){
	module->SC &= ~FTM_SC_TOF_MASK; //  clear the TimerOverflowFlag
}


bool isChannelnInterrupt(FTM_Type* module, uint8_t channel){
	return (module->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK) == FTM_CnSC_CHF_MASK;
}

void OC_ISR(void)  //FTM0 CH0 PTC8 as GPIO
{
	uint8_t channel = 0 ;
	FTM_Type * module = FTM0;
	if((module->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK) == FTM_CnSC_CHF_MASK){
		FTM_ClearInterruptFlag(module, channel);
		gpioToggle(PIN_GPIO_TEST);
		FTM_SetCounter(module,channel,FTM_GetCounter (module, channel)+ OC_Delta); // OC + oc_Delta --> oc
	}
	if((module->SC & FTM_SC_TOIE_MASK) == FTM_SC_TOIE_MASK){
		module->SC &= ~FTM_SC_TOF_MASK; //  clear the TimerOverflowFlag
	}
}

void IC_ISR(void) //FTM3 CH5 PTC9 as IC
{
	static uint32_t med1,med2,cnt_ovf;
	static uint8_t  state=0;
	FTM_Type * module = FTM3;
	uint8_t channel = 5;

	if((module->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK) == FTM_CnSC_CHF_MASK){


		FTM_ClearInterruptFlag (module, channel);
		/*
		if(state==0)
		{
			cnt_ovf = 0;
			med1=FTM_GetCounter (module, channel); //
			state=1;
		}
		else if(state==1)
		{
			med2=FTM_GetCounter (module, channel);

			if(med2 > med1){
				med=med2-med1;
			}
			else{
				med=med1-med2;
			}
			//med = med + (cnt_ovf*0xFFFF);

			state=0;					// Set break point here and watch "med" value
			//callback();
		}
		*/
	}

	if((module->SC & FTM_SC_TOIE_MASK) == FTM_SC_TOIE_MASK){
		module->SC &= ~FTM_SC_TOF_MASK; //  clear the TimerOverflowFlag
		cnt_ovf++;
		callback();
	}
}


void configPinFtm(pin_t pin, uint8_t mux_alt){
	sim_ptr->SCGC5 |= SIM_SCGC5_PORTD_MASK; // clk gating de los pines puerto D y C
	sim_ptr->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	PORT_Type * port_ftm = portPointers[PIN2PORT(pin)];
	port_ftm-> PCR[PIN2NUM(pin)] = PORT_PCR_DSE(1)| // enable Drive Strength
									PORT_PCR_MUX(mux_alt)| // gpio alternative
									PORT_PCR_IRQC(0)|  // deshabilito interrupciones
									PORT_PCR_PS(0)|
									PORT_PCR_PE(0);
}


uint32_t FTM_GetCounter(FTM_Type* ftm, uint8_t channel){
	return ftm->CONTROLS[channel].CnV & FTM_CnV_VAL_MASK;
}

void FTM_SetCounter (FTM_Type* ftm, uint8_t channel, uint16_t data){
	ftm->CONTROLS[channel].CnV = FTM_CnV_VAL(data);
}

void FTM_ClearInterruptFlag (FTM_Type * module , uint8_t channel){
	module->CONTROLS[channel].CnSC &= ~FTM_CnSC_CHF_MASK; // clear interupt status flag
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

void FTM_DisableInterrupts(FTM_Type * module, uint8_t channel){
	module->CONTROLS[channel].CnSC &= ~(FTM_CnSC_CHIE_MASK);
}

void setChannelnModeForIcOrOC(FTM_Type* module,uint8_t channel, uint8_t mode, bool isOutputCompare){
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

void setChannelnModeForPwm(FTM_Type* module,uint8_t channel, uint8_t mode, uint8_t PWM_type){
	module->QDCTRL = (module->QDCTRL & ~ FTM_QDCTRL_QUADEN_MASK) | FTM_QDCTRL_QUADEN(0);
	if(module == FTM0){
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_DECAPEN0_MASK)|FTM_COMBINE_DECAPEN0(0);
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_COMBINE0_MASK )|FTM_COMBINE_COMBINE0(0);
	}else if(module == FTM3){
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_DECAPEN3_MASK)|FTM_COMBINE_DECAPEN3(0);
		module->COMBINE=(module->COMBINE & ~FTM_COMBINE_COMBINE3_MASK )|FTM_COMBINE_COMBINE3(0);
	}

	if(PWM_type == EDGE_ALIGNED_PWM){
		module->SC=(module->SC & ~FTM_SC_CPWMS_MASK)|FTM_SC_CPWMS(0);
		module->CONTROLS[channel].CnSC = (FTM0->CONTROLS[channel].CnSC & ~ FTM_CnSC_MSB_MASK)| FTM_CnSC_MSB(1);

	}else if(PWM_type == CENTER_ALIGNED_PWM){
		module->SC=(module->SC & ~FTM_SC_CPWMS_MASK)|FTM_SC_CPWMS(1);
	}

	bool elsb = (mode&(1<<1)) == 1<<1;
	bool elsa = (mode&(1<<0)) == 1<<0;
	module->CONTROLS[channel].CnSC = (module->CONTROLS[channel].CnSC & ~ FTM_CnSC_ELSB_MASK)| FTM_CnSC_ELSB(elsb);
	module->CONTROLS[channel].CnSC = (module->CONTROLS[channel].CnSC & ~ FTM_CnSC_ELSA_MASK)| FTM_CnSC_ELSA(elsa);

}


void initSettings(FTM_Type* module,uint8_t channel, uint8_t prescaler,uint16_t initial, uint16_t final){
	module->MODE = (module->MODE  & ~FTM_MODE_WPDIS_MASK)| FTM_MODE_WPDIS(1);// desactivo write protection (poniendo 1)
	module->MODE = (module->MODE  & ~FTM_MODE_FTMEN_MASK)| FTM_MODE_FTMEN_MASK;//pongo el ftmen en 1
	module->SC = (module->SC 	 & ~FTM_SC_PS_MASK) 	| FTM_SC_PS(prescaler); //indico que utilizo el sitem clock 50M
	module->SC = (module->SC 	 & ~FTM_SC_TOIE_MASK) 	| FTM_SC_TOIE_MASK;//habilito la interrupcion de overflow
	module->MOD = (module->MOD 	 & ~FTM_MOD_MOD_MASK)	| FTM_MOD_MOD(final);//seteo el valor al que llega el contador
	module->CNTIN = (module->CNTIN & ~FTM_CNTIN_INIT_MASK)| FTM_CNTIN_INIT(initial);//seteo el valor inicial del contador
	//module->CNT = 0;
}


void updateDuty(FTM_Type* module,uint8_t channel, uint8_t DCpercent){
	uint32_t period_ticks = 0 ;
	module->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1); // voy a cargar datos! (habilito a cargar en los buffers)
	// habilita loading de MOD CNT y CV con valores de los buffers de escritura
	// además el channel 0 esta incluido en el proceso de matcheo (cuando FTM_Counter = CnV)
	period_ticks = (module->MOD & FTM_MOD_MOD_MASK) - (module->CNTIN & FTM_CNTIN_INIT_MASK);
	module->CONTROLS[channel].CnV = (module->CNTIN & FTM_CNTIN_INIT_MASK) + (uint32_t)(period_ticks*DCpercent/100);
	//module->SYNC |= FTM_SYNCONF_SWINVC(1);
	//module->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | FTM_PWMLOAD_CH0SEL(1);
}


void setAlignedPWM(FTM_Type* module,uint8_t channel){
	FTM_DisableClock(module);

	initSettings(module, channel,FTM_PSC_x1,0,PWM_MOD);
	setChannelnModeForPwm(module, channel, FTM_EA_PWM_ClearOutputOnMatchUp, EDGE_ALIGNED_PWM);
	updateDuty(module, channel, 1);

	//module->MODE |= FTM_MODE_PWMSYNC(1); // software trigger solo puede ser usado por MOD yCnv synchronization
											// hardware triggers pueden ser usados por OUTMASK y FTM counter synchronization

	// Pagina 1059 del manual de 1835 paginas!!!!!
	module->SYNCONF |= FTM_SYNCONF_SYNCMODE(1); // enhanced synch enabled
	module->SYNCONF |= FTM_SYNCONF_CNTINC(1);  // CNTIN register is updated with its buffer value by the PWM synchronization.
	module->SYNCONF |= FTM_SYNCONF_SWWRBUF(1); // The software trigger activates MOD, CNTIN, and CV registers synchronization.
	module->SYNCONF |= FTM_SYNCONF_SWRSTCNT(1); // The software trigger activates the FTM counter synchronization.

	module->COMBINE |= FTM_COMBINE_SYNCEN0(1);  // The PWM synchronization in this pair of channels is enabled. (enrte 0 y 1)

	FTM_EnableInterrupts(module, channel);


	FTM_SetClock(module);

	NVIC_EnableIRQ(FTM0_IRQn);

	configPinFtm(PIN_OUTPUT_CAPTURE,4);

}


void setOC(FTM_Type* module,uint8_t channel){
	bool isOutputCompare = true;
	FTM_DisableClock(module);

	initSettings(module, channel,FTM_PSC_x32,0,0xFFFF);

	setChannelnModeForIcOrOC(module,channel, FTM_OC_ToggleOutputOnMatch,isOutputCompare);

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

	initSettings(module, channel,FTM_PSC_x4,0,0xFFFF);

	setChannelnModeForIcOrOC(module,channel, FTM_IC_CaptureOnEitherEdge,isOutputCompare);

	FTM_EnableInterrupts(module, channel);


	FTM_SetClock(module);

	//module->SC = (module->SC 	 & ~FTM_SC_TOIE_MASK) 	| FTM_SC_TOIE(0);//habilito la interrupcion de overflow
	// PTC9 as IC (FTM3-CH5)
	NVIC_EnableIRQ(FTM3_IRQn);

	configPinFtm(PIN_INPUT_CAPTURE, 3);
	configPinFtm(PIN_GPIO_TEST,1);
	gpioMode(PIN_GPIO_TEST, OUTPUT);
}

void FTM0_DmaMode (uint8_t channel, bool dma_mode)
{
	FTM_Type* ftm = FTM0;
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_DMA_MASK)) |
			                      (FTM_CnSC_DMA(dma_mode));
}

void FTM3_DmaMode (uint8_t channel, bool dma_mode)
{
	FTM_Type* ftm = FTM3;
	ftm->CONTROLS[channel].CnSC = (ftm->CONTROLS[channel].CnSC & ~(FTM_CnSC_DMA_MASK)) |
			                      (FTM_CnSC_DMA(dma_mode));
}

void ftmInit(void (*funcallback)(void) ){
	callback = funcallback;
	// primero hacemos clockgating
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	sim_ptr->SCGC6 |= SIM_SCGC6_FTM2_MASK;
	sim_ptr->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	sim_ptr->SCGC3 |= SIM_SCGC3_FTM3_MASK;


	sim_ptr->SCGC5 |= SIM_SCGC5_PORTD_MASK; // clock gating de los pines puerto D y C
	sim_ptr->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
	FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;

	setIC(FTM3,5); // module = FTM3 , channel = 5
//	setOC(FTM0,0); // module = FTM0, channel = 0
	setAlignedPWM(FTM0, 0);

	setDuty(50); // Valor medio por default

	FTM0_DmaMode(0, 1); // DMA ON
	FTM3_DmaMode(5, 1);
}

uint32_t getPulseMeasure(void)
{
	return med;
}

uint32_t* getMedAddress(void)
{
	return &med;
}
/*******************************************************************************
 ******************************************************************************/
