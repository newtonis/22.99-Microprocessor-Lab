/*
 * DMA.h
 *
 *  Created on: 26 Oct 2019
 *      Author: Marcelo
 */

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void DMA_Config(uint16_t *source_add, uint16_t* dest_add, void(*funcallback)(void));

void DMA_ConfigCounters(uint32_t source_full_size, uint32_t source_unit_size);

void DMA_ConfigSourceAddress(uint16_t *source_add);

void DMA_ConfigDestAddress(uint16_t *dest_add);

void DMA_EnableRequest(void);

void DMA_DisableRequest(void);

#endif /* DMA_H_ */
