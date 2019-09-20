/*
 * SPImanager.c
 *
 *  Created on: 20 sep. 2019
 *      Author: joa-m
 */

#include "SPImanager.h"

void SPI_Initialize(void){
	SPIMode(PIN_SCK, DISABLE_PULL);
	SPIMode(PIN_MOSI, DISABLE_PULL);
	SPIMode(PIN_MISO, DISABLE_PULL);
	SPIClockGatingEnable(SPI_0);
	setMasterMode(SPI_0);
	setMasterClockPhase(SPI_0, DISABLE);
	setMasterClockPolarity(SPI_0, DISABLE);
	setMasterBaudRate(SPI_0, 0);
	setMasterBaudRatePrescaler(SPI_0, 2);
	enableContinousSCK(SPI_0);
	setSPIConfig(SPI_0, 0);
	disableFreeze(SPI_0);
	disableModifyTransferFormat(SPI_0);
	moduleEnable(SPI_0);
	HALTStartTransfers(SPI_0);
}
