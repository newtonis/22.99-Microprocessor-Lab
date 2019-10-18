/*
 * ADC.c
 *
 *  Created on: 17 Oct 2019
 *      Author: Marcelo
 */
#include "ADC.h"
#include "hardware.h"

static ADC_Type* ADC0_ptr = ADC0;
static SIM_Type* sim_ptr = SIM;

static ADCResultData_t ADCdata;	// ADC Data Result

void (*ADCcallback)(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ADC_init(uint32_t adc_ch, void (*funcallback)(void))
{
	ADCcallback = funcallback;
	// Activo CLOCK GATING
	sim_ptr->SCGC6 &= ~SIM_SCGC6_ADC0_MASK;
	sim_ptr->SCGC6 |= SIM_SCGC6_ADC0(1);

	// Enable Interrupt on ADC0
	NVIC_EnableIRQ(ADC0_IRQn);

	// Modo 16-bits
	ADC0_ptr->CFG1 &= ~( ADC_CFG1_ADLPC_MASK |
						 ADC_CFG1_ADIV_MASK |
						 ADC_CFG1_ADLSMP_MASK |
						 ADC_CFG1_MODE_MASK |
						 ADC_CFG1_ADICLK_MASK );
	ADC0_ptr->CFG1 |= ( ADC_CFG1_ADLPC(1) |			// Low-power configuration
						ADC_CFG1_ADIV(1) |			// Divide ratio = 2
						ADC_CFG1_ADLSMP(0) |		// Short sample time
						ADC_CFG1_MODE(0x3) |		// Single-ended 16-bit
						ADC_CFG1_ADICLK(1) );		// Bus Clock % 2

	ADC0_ptr->CFG2 &= ~(ADC_CFG2_MUXSEL_MASK |
					    ADC_CFG2_ADACKEN_MASK |
					    ADC_CFG2_ADHSC_MASK |
					    ADC_CFG2_ADLSTS_MASK);
	ADC0_ptr->CFG2 |=  (ADC_CFG2_MUXSEL(0) |		// ADxxa channels are selected
						ADC_CFG2_ADACKEN(0) |		// Asynchronous clock output disabled
						ADC_CFG2_ADHSC(0) |			// Normal conversion sequence selected
						ADC_CFG2_ADLSTS(0));		// Default longest sample time

	ADC0_ptr->SC2 &= ~( ADC_SC2_ADTRG_MASK |
					   ADC_SC2_ACFE_MASK |
					   ADC_SC2_DMAEN_MASK |
					   ADC_SC2_REFSEL_MASK);
	ADC0_ptr->SC2 |= ( ADC_SC2_ADTRG(0) |			// Software trigger
					   ADC_SC2_ACFE(0) |			// Compare function disabled
					   ADC_SC2_DMAEN(0) |			// DMA Disabled
					   ADC_SC2_REFSEL(0));			// Default voltage reference

	ADC0_ptr->SC3 &= ~ADC_SC3_CAL_MASK;
	ADC0_ptr->SC3 |= ADC_SC3_CAL(1);				// Calibrate ADC

	while((ADC0_ptr->SC3) & (ADC_SC3_CAL_MASK))
	{
		// Wait while calibration
	}

	//uint32_t aux_CALF = (ADC0_ptr->SC3) & ADC_SC3_CALF_MASK; // Ver despues por cheque de init

	ADC0_ptr->SC3 &= ~( ADC_SC3_ADCO_MASK |
						ADC_SC3_AVGE_MASK |
						ADC_SC3_AVGS_MASK);
	ADC0_ptr->SC3 |= ( ADC_SC3_ADCO(1) |			// Continuous conversion
					   ADC_SC3_AVGE(1) |			// Hardware average enable
					   ADC_SC3_AVGS(0));			// 4 samples for average

	ADC0_ptr->SC1[0] &= ~( ADC_SC1_AIEN_MASK |
					       ADC_SC1_DIFF_MASK |
						   ADC_SC1_ADCH_MASK);
	ADC0_ptr->SC1[0] |= ( ADC_SC1_AIEN(1) |			// Conversion complete IE
						  ADC_SC1_DIFF(0) |			// Single-ended conversions
						  ADC_SC1_ADCH(adc_ch));	// ADC channel select
}

void ADC0_IRQHandler(void)
{
	ADCdata = ADC0_ptr->R[0];
	ADCcallback();
}

void ADC_enableModule(uint32_t adc_ch)
{
	ADC0_ptr->SC1[0] &= ~ADC_SC1_ADCH_MASK;
	ADC0_ptr->SC1[0] |= ADC_SC1_ADCH(adc_ch);
}

void ADC_disableModule(void)
{
	ADC0_ptr->SC1[0] &= ~ADC_SC1_ADCH_MASK;
	ADC0_ptr->SC1[0] |= ADC_SC1_ADCH(DISABLE);
}

ADCResultData_t ADC_getDataResult(void)
{
	return ADCdata;
}

/*******************************************************************************
 ******************************************************************************/
