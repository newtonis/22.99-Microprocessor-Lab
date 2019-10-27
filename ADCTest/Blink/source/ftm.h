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

enum{EDGE_ALIGNED_PWM,
	CENTER_ALIGNED_PWM
};

enum{FTM_CA_PWM_ClearOutputOnMatch = 2,
	FTM_CA_PWM_SetOutputOnMatch
};

enum{FTM_EA_PWM_ClearOutputOnMatchUp = 2,
	FTM_EA_PWM_SetOutputOnMatchUp
};


#define GPIO_ALTERNATIVE 1

__ISR__ FTM0_IRQHandler(void);

__ISR__ FTM3_IRQHandler(void);

bool isChannelnInterrupt(FTM_Type* module, uint8_t channel);

bool isTimerOverFlowInterrupt(FTM_Type* module);


void OVF_ISR(void);

void IC_ISR(void);

void OC_ISR(void);

void configPinFtm(pin_t pin, uint8_t mux_alt);



uint32_t FTM_GetCounter(FTM_Type* ftm, uint8_t channel);

void ftmInit(void (*funcallback)(void));

void setDuty(uint8_t duty_cycle);

void FTM_DmaMode (FTM_Type* ftm, uint8_t channel, bool dma_mode);

uint16_t* getDutyAddress(void);

#endif /* FTM_H_ */
