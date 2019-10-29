/*
 * DMA.c
 *
 *  Created on: 26 Oct 2019
 *      Author: Marcelo
 */

#include "DMA.h"
#include "hardware.h"

void (*LoopCallback)(void);
void (*InputCapCallback)(void);
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void DMA0_Config(uint16_t *source_add, uint16_t* dest_add, void(*funcallback)(void))
{
	LoopCallback = funcallback;

	/* Enable the clock for the eDMA and the DMAMUX. */
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	/* Enable the eDMA channel 0 and set the FTM CH0 as the DMA request source. */
	DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(20);   // FTM0 CH0

	/* Enable the interrupts for the channel 0. */

	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	/* Enable the DMA interrupts. */
	NVIC_EnableIRQ(DMA0_IRQn);

	/// ============= INIT TCD0 ===================//
	/* Set memory address for source and destination. */
	DMA0_ConfigSourceAddress(source_add);

    //DMA_TCD0_DADDR = (uint32_t)(destinationBuffer);
	DMA0_ConfigDestAddress(dest_add);


		/* Set an offset for source and destination address. */
	DMA0->TCD[0].SOFF =0x02; // Source address offset of 2 bytes per transaction.
	DMA0->TCD[0].DOFF =0x00; // Destination address offset is 0. (Siempre al mismo lugar)

	/* Set source and destination data transfer size to 16 bits (CnV is 2 bytes wide). */
	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);

	/*Number of bytes to be transfered in each service request of the channel.*/
	DMA0->TCD[0].NBYTES_MLNO = 0x02;


    /* DLASTSGA DLAST Scatter and Gatter */
    DMA0->TCD[0].DLAST_SGA = 0x00;

	/* Setup control and status register. */
    DMA0->TCD[0].CSR = DMA_CSR_INTMAJOR_MASK;

}

void DMA0_ConfigCounters(uint32_t source_full_size, uint32_t source_unit_size)
{
	/* Autosize for Current major iteration count */
	DMA0->TCD[0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(source_full_size/source_unit_size);
	DMA0->TCD[0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(source_full_size/source_unit_size);


	//DMA_TCD0_SLAST = 0x00;
    //DMA_TCD0_SLAST = -5*sizeof(uint16_t);
	//DMA_TCD0_SLAST = -((sizeof(sourceBuffer)/sizeof(sourceBuffer[0])*sizeof(uint16_t)));

	/* Autosize SLAST for Wrap Around. This value is added to SADD at the end of Major Loop */
	DMA0->TCD[0].SLAST = -source_full_size;
}

void DMA0_EnableRequest(void)
{
	/* Enable request signal for channel 0. */
	DMA0->ERQ &= ~(DMA_ERQ_ERQ0_MASK);
	DMA0->ERQ |= DMA_ERQ_ERQ0(1);
	DMAMUX->CHCFG[0] &= ~(DMAMUX_CHCFG_ENBL_MASK);
	DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL(1);
}

void DMA0_DisableRequest(void)
{
	/* Enable request signal for channel 0. */
	DMA0->ERQ &= ~(DMA_ERQ_ERQ0_MASK);
	DMA0->ERQ |= DMA_ERQ_ERQ0(0);
	DMAMUX->CHCFG[0] &= ~(DMAMUX_CHCFG_ENBL_MASK);
	DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL(0);
}

void DMA0_ConfigSourceAddress(uint16_t *source_add)
{
	DMA0->TCD[0].SADDR = (uint32_t)(source_add); //List of Duties
}

void DMA0_ConfigDestAddress(uint16_t *dest_add)
{
	DMA0->TCD[0].DADDR = (uint32_t)(dest_add);  // To change FTM Duty
}

void DMA0_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMA0->CINT |= 0;
	LoopCallback();
}

void DMA1_Config(uint16_t *source_add, uint16_t* dest_add, void(*funcallback)(void))
{
	InputCapCallback = funcallback;

	/* Enable the eDMA channel 1 and set the FTM CH5 as the DMA request source. */
	DMAMUX->CHCFG[1] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(37);   // FTM3 CH5

	/* Enable the interrupts for the channel 0. */

	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA1_IRQn);
	/* Enable the DMA interrupts. */
	NVIC_EnableIRQ(DMA1_IRQn);

	/// ============= INIT TCD0 ===================//
	//DMA1_ConfigSourceAddress(source_add);

    //DMA_TCD0_DADDR = (uint32_t)(destinationBuffer);
	//DMA1_ConfigDestAddress(dest_add);


		/* Set an offset for source and destination address. */
	DMA0->TCD[0].SOFF =0x02; // Source address offset of 2 bytes per transaction.
	DMA0->TCD[0].DOFF =0x00; // Destination address offset is 0. (Siempre al mismo lugar)

	/* Set source and destination data transfer size to 16 bits (CnV is 2 bytes wide). */
	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);

	/*Number of bytes to be transfered in each service request of the channel.*/
	DMA0->TCD[0].NBYTES_MLNO = 0x02;


    /* DLASTSGA DLAST Scatter and Gatter */
    DMA0->TCD[0].DLAST_SGA = 0x00;

	/* Setup control and status register. */
    DMA0->TCD[0].CSR = DMA_CSR_INTMAJOR_MASK;
}


void DMA1_IRQHandler()
{
	/* Clear the interrupt flag. */
	DMA0->CINT |= 0;
	InputCapCallback();
}

/*******************************************************************************
 ******************************************************************************/
