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
#include "defineslocos.h"

#define SPI_DRIVER_MODE		2 //alternativa
#define SPI_DRIVER_INPUT	1

#define SPI_DRIVER_BR 5
#define SPI_DRIVER_PBR 1

typedef uint8_t pin_t;


void setup_pin (pin_t pin);
void SPI_driver_init (void);
uint8_t SPI_driver_sendRecive(uint8_t * data2end, uint8_t size,uint8_t * recivedData);


#endif /* SPI_DRIVER_H_ */

