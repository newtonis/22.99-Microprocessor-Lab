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
	setMasterMode(SPI_0);
	HALTStartTransfers(SPI_0);
	SPIClockGatingEnable(SPI_0);
	setClockPhase(SPI_0, DISABLE);
	setClockPolarity(SPI_0, DISABLE);
}
