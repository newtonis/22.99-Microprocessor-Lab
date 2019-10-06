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


#define SPI_ALTERNATIVE		2
#define BAUD_RATE 5
#define BAUD_RATE_PRESCALER 1

#define INITIAL_DATA2SEND_MASK SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(1)
#define FINAL_DATA2SEND_MASK SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(0)|SPI_PUSHR_EOQ(1)


typedef uint8_t pin_t;

void setup_pin (pin_t pin);
void SPI_Init (void);
uint8_t SPI_MasterReadWrite(uint8_t * data2end, uint8_t size,uint8_t * recivedData);


#endif /* SPI_DRIVER_H_ */

