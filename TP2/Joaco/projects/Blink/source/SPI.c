#include "SPI.h"

/*
 * SPI.c
 *
 *  Created on: 18 sep. 2019
 *      Author: joa-m
 */

static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static SPI_Type* SPIPtrs[] = SPI_BASE_PTRS;
static uint32_t simMasks[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK };
static SIM_Type* sim_ptr = SIM;

//static SPI_Type* SPI_IRQSPtrs[] = SPI_IRQS;

// Existe la macro SPI_IRQS

void SPIClockGatingEnable(int SPI_n){
	switch(SPI_n){
		case SPI_0:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI0(ENABLE); // activo clock gating
			break;
		case SPI_1:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI1(ENABLE);
			break;
		case SPI_2:
			sim_ptr->SCGC3 |= SIM_SCGC3_SPI2(ENABLE);
			break;
	}
}


void SPIMode(pin_t pin, uint8_t mode){
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin); // num es el numero de pin
	// connect to gpio (hay un PCR por pin)
	port->PCR[num] = 0x00;
	port->PCR[num] |= PORT_PCR_MUX(2); // ENABLE SPI
	port->PCR[num] |= PORT_PCR_IRQC(0);
	switch(mode){
		case PULL_UP:
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] |= HIGH<<0; //PULL SELECT en 1 (PULLUP)
			break;
		case PULL_DOWN:
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] &= ~(HIGH); //PULL SELECT en 0 (PULLDOWN)
			break;
		case DISABLE_PULL:
			port->PCR[num] &= ~(HIGH<<1); //PULL ENABLE en 0
			break;
	}
}

void setBaudRate(int mode){
	//baudrate
	//y prescaler

}

void setClockPolarity(int SPI_n, bool pol){
	if (pol == ENABLE){
		SPIPtrs[SPI_n]->CTAR |= SPI_CTAR_CPOL(ENABLE);
	}else{
		SPIPtrs[SPI_n]->CTAR &= ~SPI_CTAR_CPOL_SHIFT;
	}
}

void setClockPhase(int SPI_n, bool pha){
	if (pha == ENABLE){
		SPIPtrs[SPI_n]->CTAR |= SPI_CTAR_CPHA(ENABLE);
	}else{
		SPIPtrs[SPI_n]->CTAR &= ~SPI_CTAR_CPHA_SHIFT;
	}
}


void setMasterMode(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_MSTR(MASTER);
}

void setSlaveMode(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~SPI_MCR_MSTR_SHIFT;
}

bool getMode(int SPI_n){
	return (SPIPtrs[SPI_n]->MCR & SPI_MCR_MSTR_SHIFT) == (SPI_MCR_MSTR_SHIFT);
}

void enableContinousSCK(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_CONT_SCKE(ENABLE);
}

void disableContinousSCK(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_CONT_SCKE_SHIFT;
}

void setSPIConfig(int SPI_n, int SPI_config){
	SPIPtrs[SPI_n]->MCR &= ~SPI_MCR_DCONF_MASK;
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DCONF(SPI_config);
}

void enableFreeze(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_FRZ(ENABLE);
}

void disableFreeze(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_FRZ_SHIFT;
}

void enableModifyTransferFormat(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_MTFE(ENABLE);
}

void disableModifyTransferFormat(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_MTFE_SHIFT;
}

void enableChipSelectStrobe(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_PCSSE(ENABLE);
}

void disableChipSelectStrobe(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_PCSSE_SHIFT;
}

void enableRxFIFOOverflowOverwrite(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_ROOE(ENABLE);
}

void disableRxFIFOOverflowOverwrite(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_ROOE_SHIFT;
}

void dozeEnable(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DOZE(ENABLE);
}

void dozeDisable(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_DOZE_SHIFT;
}

void moduleDisable(int SPI_n){ // power saving
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_MDIS(ENABLE);
}

void moduleEnable(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~  SPI_MCR_MDIS_SHIFT;
}

void disableTxFIFO(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DIS_TXF(ENABLE);
}

void enableTxFIFO(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_DIS_TXF_SHIFT;
}

void disableRxFIFO(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DIS_RXF(ENABLE);
}

void enableRxFIFO(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_DIS_RXF_SHIFT;
}

void clearTxFIFO(int SPI_n){ //flush
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_CLR_TXF(ENABLE);
}

void dontClearTxFIFO(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_CLR_TXF_SHIFT;
}

void clearRxFIFO(int SPI_n){  //flush
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_CLR_RXF(ENABLE);
}

void dontClearRxFIFO(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_CLR_RXF_SHIFT;
}

void HALTStopTransfers(int SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_HALT(ENABLE);
}

void HALTStartTransfers(int SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_HALT_SHIFT;
}
