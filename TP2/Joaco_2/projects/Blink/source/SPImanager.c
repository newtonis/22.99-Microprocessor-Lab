/*
 * SPImanager.c
 *
 *  Created on: 20 sep. 2019
 *      Author: joa-m
 */

#include "SPImanager.h"

static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static SPI_Type* SPIPtrs[] = SPI_BASE_PTRS;
static SIM_Type* sim_ptr = SIM;

void masterInitiliaze(uint8_t SPI_n){
	bool whichCTAR = 1;
	uint8_t SPI_alternative = 2;

	SPIMode(PIN_PCS0, DISABLE_PULL, SPI_alternative );
	SPIMode(PIN_SCK_MASTER, DISABLE_PULL, SPI_alternative );
	SPIMode(PIN_MOSI_MASTER, DISABLE_PULL, SPI_alternative );
	SPIMode(PIN_MISO_MASTER, DISABLE_PULL, SPI_alternative );

	SPIClockGatingEnable(SPI_n);

	setSPIConfig(SPI_n, 0);

	setMode(SPI_n,MASTER);

	SPIPtrs[SPI_n]->PUSHR &= ~SPI_PUSHR_CONT_SHIFT; // isChipSelectContinuous = false;
	disableContinousSCK(SPI_n); // isSckContinuous = false;
	setPCSActiveLow(SPI_n); // .pcsPolarity = kDspiPcs_ActiveLow;
	SPIPtrs[SPI_n]->PUSHR |= SPI_PUSHR_PCS(1); // activo el PCS0



	disableRxFIFOOverflowOverwrite(SPI_n);
	disableModifyTransferFormat(SPI_n);


	setMasterClockPhase(SPI_n, DISABLE, whichCTAR);
	setMasterClockPolarity(SPI_n, DISABLE, whichCTAR);
	setMasterBaudRate(SPI_n, 0b1111, whichCTAR);
	setMasterBaudRatePrescaler(SPI_n, 0b11, whichCTAR);


	//moduleEnable(SPI_n);
	//disableFreeze(SPI_n);
	//HALTStartTransfers(SPI_n);
}



void SPI_Initialize(void){

	masterInitiliaze(SPI_0);

	// variable para debuggear
	SPI_Type * pointer = SPIPtrs[SPI_0];
	pointer->PUSHR &= ~SPI_PUSHR_CTAS_MASK;
	pointer->PUSHR &= SPI_PUSHR_CTAS(0);
	pointer->SR |= SPI_SR_TXRXS_SHIFT;

	bool i = 0;

	while(1){
		if(i == 0){
			pointer->MCR |= SPI_MCR_PCSIS(0b1);

			pointer->PUSHR &= ~SPI_PUSHR_PCS_MASK;
			pointer->PUSHR |= SPI_PUSHR_PCS(1); // activo el PCS0

			pointer->PUSHR &= ~SPI_PUSHR_TXDATA_MASK;
			uint16_t TXcommand = 0x00;
			TXcommand = TXcommand << 8;
			uint16_t TXdata = 0xAA;
			pointer->PUSHR |= TXcommand;
			pointer->PUSHR |= TXdata;


			pointer->SR |= SPI_SR_TCF_SHIFT;


			i = 1;
		}else{

			pointer->MCR &= ~SPI_MCR_PCSIS(0b111111);

			pointer->PUSHR &= ~SPI_PUSHR_PCS_MASK;

			pointer->PUSHR &= ~SPI_PUSHR_TXDATA_MASK;
			uint16_t TXcommand = 0x00;
			TXcommand = TXcommand << 8;
			uint16_t TXdata = 0x00;
			pointer->PUSHR |= TXcommand;
			pointer->PUSHR |= TXdata;

			pointer->SR |= SPI_SR_TCF_SHIFT;


			i = 0;
		}
	}

	int aux3 = 1;
}
