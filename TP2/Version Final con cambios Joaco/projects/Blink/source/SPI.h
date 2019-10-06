/*
 * SPI_driver.h
 *
 *  Created on: Oct 2, 2019
 *      Author: joa-m
 */

#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include<stdint.h>
#include<stdbool.h>


#define SPI_DRIVER_MODE		2 //alternativa
#define SPI_DRIVER_INPUT	1
#define SPI_DRIVER_BR 5
#define SPI_DRIVER_PBR 1


enum{SPI_0,SPI_1,SPI_2};

typedef uint8_t pin_t;
typedef struct{
	uint32_t CTAR_CPOL;
	uint32_t CTAR_CPHA;
	uint32_t CTAR_PBR;
	uint32_t CTAR_BR;
	uint32_t CTAR_FMSZ;
	uint32_t CTAR_ASC;
	uint32_t CTAR_PASC;
	uint32_t CTAR_CSSCK;
	uint32_t CTAR_PCSSCK;
	uint32_t CTAR_PDT;
	uint32_t CTAR_DT;

}spi_cfg_t;

void SPIClockGatingEnable(uint8_t SPI_n);
void setup_pin (pin_t pin);
void SPI_driver_init (void);
uint8_t SPI_driver_sendRecive(uint8_t * data2end, uint8_t size,uint8_t * recivedData);


#endif /* SPI_DRIVER_H_ */

