/*
 * SysTick.c
 *
 *  Created on: 29 Aug 2019
 *      Author: Marcelo
 */
#include <SysTick.h>
#include "hardware.h"

static SysTick_Type* SysTick_ptr = SysTick;
static void (*callbackTick)();

/**
 * @brief Initialize SysTic driver
 * @param funcallback Function to be call every SysTick
 * @return Initialization and registration succeed
 */
bool SysTick_Init (void (*funcallback)(void)){
	NVIC_EnableIRQ(SysTick_IRQn);
	SysTick_ptr->CTRL = 0x00;
	SysTick_ptr->LOAD = 00001000L - 1; // 1ms
	SysTick_ptr->VAL = 0x00;
	SysTick_ptr->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	callbackTick = funcallback;
	return 1;
}

__ISR__ SysTick_Handler(void){
	callbackTick();
}

