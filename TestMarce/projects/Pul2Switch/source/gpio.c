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
	uint32_t port_name = PIN2PORT(pin);
	uint32_t port_num = PIN2NUM(pin);
	uint32_t mask;
	if(value == HIGH){
		mask = 1 << port_num;
	}else{
		mask = 0;
	}
	GPIO_Type *gpio[] = GPIO_BASE_PTRS;
	gpio[port_name]->PDOR = (gpio[port_name]->PDOR) || mask;
}

bool gpioRead(pin_t pin){
	uint32_t port_name = PIN2PORT(pin);
	uint32_t port_num = PIN2NUM(pin);
	GPIO_Type *gpio[] = GPIO_BASE_PTRS;
	uint32_t temp_port = gpio[port_name]->PDIR;
	uint32_t input_value = temp_port >> port_num;
	if(input_value == 1){
		return HIGH;
	}else{
		return LOW;
	}
}
