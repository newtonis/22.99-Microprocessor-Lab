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


void ftmInit(void (*funcallback)(void));

void setDuty(uint8_t duty_cycle);

uint16_t* getDutyAddress(void);

uint32_t* getMedAddress(void);

uint32_t getPulseMeasure(void);

void setCntOvf(void);

void FTM_ClearInterruptFlag (FTM_Type * module , uint8_t channel);

#endif /* FTM_H_ */
