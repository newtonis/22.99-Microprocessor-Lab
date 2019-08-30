/*
 * SysTick.c
 *
 *  Created on: 29 Aug 2019
 *      Author: Marcelo
 */
#include <SysTick.h>

static SysTick_Type* SysTick_ptr = SysTick;

/**
 * @brief Initialize SysTic driver
 * @param funcallback Function to be call every SysTick
 * @return Initialization and registration succeed
 */
bool SysTick_Init (void){
	SysTick_ptr->CTRL = 0x00;
	SysTick_ptr->LOAD = 10000000L - 1; // 500ms
	SysTick_ptr->VAL = 0x00;
	SysTick_ptr->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	return 1;
}

