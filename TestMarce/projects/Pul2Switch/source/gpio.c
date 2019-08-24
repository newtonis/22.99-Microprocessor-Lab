/*
 * gpio.c
 *
 *  Created on: 23 Aug 2019
 *      Author: Marcelo
 */

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <gpio.h>
#include <hardware.h>

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/

void gpioWrite (pin_t pin, bool value){
	port_name = PIN2PORT(pin);
	port_num = PIN2NUM(pin);
	uint32_t mask = 1 << port_num;
	GPIO_Type *gpio[] = GPIO_BASE_PTRS;
	temp = gpio[port_name]->PDOR;

}
