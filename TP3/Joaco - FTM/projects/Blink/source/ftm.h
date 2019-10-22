/*
 * ftm.h
 *
 *  Created on: 18 oct. 2019
 *      Author: joa-m
 */

#ifndef FTM_H_
#define FTM_H_

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

enum{FTM_PSC_x1=0,
	FTM_PSC_x2,
	FTM_PSC_x4,
	FTM_PSC_x8,
	FTM_PSC_x16,
	FTM_PSC_x32,
	FTM_PSC_x64,
	FTM_PSC_x128
	};

enum{FTM_IC_CaptureOnRisingEdgeOnly=1,
	FTM_IC_CaptureOnFallingEdgeOnly,
	FTM_IC_CaptureOnEitherEdge
};

enum{FTM_OC_ToggleOutputOnMatch=1,
	FTM_OC_ClearOutputOnMatch,
	FTM_OC_SetOutputOnMatch
};



#define GPIO_ALTERNATIVE 1

__ISR__ FTM0_IRQHandler(void);

__ISR__ FTM3_IRQHandler(void);

void OVF_ISR(void);

void IC_ISR(void);

void OC_ISR(void);

void configPinFtm(pin_t pin, uint8_t mux_alt);

void ftmInit(void);




#endif /* FTM_H_ */
