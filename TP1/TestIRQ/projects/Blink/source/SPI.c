#include "SPI.h"

/*
 * SPI.c
 *
 *  Created on: 18 sep. 2019
 *      Author: joa-m
 */

static SPI_Type* SPIPtrs[] = SPI_BASE_PTRS;
//static SPI_Type* SPI_IRQSPtrs[] = SPI_IRQS;

// Existe la macro SPI_IRQS

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




