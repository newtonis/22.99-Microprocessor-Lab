/*
 * ADC.h
 *
 *  Created on: 17 Oct 2019
 *      Author: Marcelo
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define ADC0_DP0	0b00000
#define DISABLE		0b11111

typedef uint32_t ADCResultData_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void ADC_init(uint32_t adc_ch, void (*funcallback)(void));

void ADC_enableModule(uint32_t adc_ch);

void ADC_disableModule(void);

ADCResultData_t ADC_getDataResult(void);

#endif /* ADC_H_ */
