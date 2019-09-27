#include "SPI.h"

/*
 * SPI.c
 *
 *  Created on: 18 sep. 2019
 *      Author: joa-m
 */

static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static SPI_Type* SPIPtrs[] = SPI_BASE_PTRS;
static SIM_Type* sim_ptr = SIM;


void masterConfig(spi_master_config_t * master_cfg){
	master_cfg->whichCTAR = 0;
	master_cfg->SCKE = false;
	master_cfg->MTFE = false;
	master_cfg->ROOE = false;
	master_cfg->SMPL_PT = 0b00;
	master_cfg->DIS_RXF = false;
	master_cfg->DIS_TXF = false;

	master_cfg->CTAR_FMSZ = 8;
	master_cfg->CTAR_CPOL = 0;
	master_cfg->CTAR_CPHA = 0;
	master_cfg->CTAR_LSBFE = 0;
}

void masterInitiliaze(uint8_t SPI_n){
	uint8_t SPI_alternative = 2;

	// cargamos la configuracion de master
	spi_master_config_t master_config;
	masterConfig(&master_config);


	SPIMode(93, DISABLE_PULL, SPI_alternative );
	SPIMode(94, DISABLE_PULL, SPI_alternative );
	SPIMode(95, DISABLE_PULL, SPI_alternative );
	SPIMode(96, DISABLE_PULL, SPI_alternative );

	SPIClockGatingEnable(SPI_n);
	setSPIConfig(SPI_n, SPI_CONFIG);


	moduleEnable(SPI_n);
	HALTStopTransfers(SPI_n);


	SPI_Type* base = SPIPtrs[SPI_n];
	uint32_t temp = 0;

	temp = base->MCR & (~(  SPI_MCR_CONT_SCKE_MASK |
							SPI_MCR_MTFE_MASK |
							SPI_MCR_ROOE_MASK |
							SPI_MCR_SMPL_PT_MASK |
	                        SPI_MCR_DIS_TXF_MASK |
							SPI_MCR_DIS_RXF_MASK));

	base->MCR = temp |
					SPI_MCR_CONT_SCKE(master_config.SCKE) |
	                SPI_MCR_MTFE(master_config.MTFE) |
	                SPI_MCR_ROOE(master_config.ROOE) |
					SPI_MCR_SMPL_PT(master_config.SMPL_PT) |
	                SPI_MCR_DIS_TXF(master_config.DIS_TXF) |
					SPI_MCR_DIS_RXF(master_config.DIS_RXF);

	setMasterBaudRate(SPI_n, 0b1111, master_config.whichCTAR);
	setMasterBaudRatePrescaler(SPI_n, 0b11, master_config.whichCTAR);

	temp = base->CTAR[master_config.whichCTAR] & ~(  SPI_CTAR_FMSZ_MASK |
									 	 	 	 	 SPI_CTAR_CPOL_MASK |
													 SPI_CTAR_CPHA_MASK |
													 SPI_CTAR_LSBFE_MASK);

	base->CTAR[master_config.whichCTAR] = temp | SPI_CTAR_FMSZ(master_config.CTAR_FMSZ - 1) |
								   	   	   	   	 SPI_CTAR_CPOL(master_config.CTAR_CPOL) |
												 SPI_CTAR_CPHA(master_config.CTAR_CPHA) |
												 SPI_CTAR_LSBFE(master_config.CTAR_LSBFE);

	   /*
	    *
		DSPI_MasterSetDelayTimes(base, masterConfig->whichCtar, kDSPI_PcsToSck, srcClock_Hz,
								 masterConfig->ctarConfig.pcsToSckDelayInNanoSec);
		DSPI_MasterSetDelayTimes(base, masterConfig->whichCtar, kDSPI_LastSckToPcs, srcClock_Hz,
								 masterConfig->ctarConfig.lastSckToPcsDelayInNanoSec);
		DSPI_MasterSetDelayTimes(base, masterConfig->whichCtar, kDSPI_BetweenTransfer, srcClock_Hz,
								 masterConfig->ctarConfig.betweenTransferDelayInNanoSec);
	    *
	    *
	    * */
	// setDummyData
	HALTStartTransfers(SPI_n);
}

void SPI_Initialize(void){

	masterInitiliaze(SPI_0);

}

void SPIClockGatingEnable(uint8_t SPI_n){
	switch(SPI_n){
		case SPI_0:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI0(ENABLE); // activo clock gating
			break;
		case SPI_1:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI1(ENABLE);
			break;
		case SPI_2:
			sim_ptr->SCGC3 |= SIM_SCGC3_SPI2(ENABLE);
			break;
	}
}


void SPIMode(pin_t pin, uint8_t mode, uint8_t mux_alt){
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin); // num es el numero de pin
	port->PCR[num] = 0x00;
	port->PCR[num] |= PORT_PCR_MUX(mux_alt); // ENABLE SPI
	port->PCR[num] |= PORT_PCR_IRQC(0);
	switch(mode){
		case PULL_UP:
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] |= HIGH<<0; //PULL SELECT en 1 (PULLUP)
			break;
		case PULL_DOWN:
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] &= ~(HIGH); //PULL SELECT en 0 (PULLDOWN)
			break;
		case DISABLE_PULL:
			port->PCR[num] &= ~(HIGH<<1); //PULL ENABLE en 0
			break;
	}
}


void testSPI(uint8_t SPI_n){

	static int i = 0;
	if(i % 2){
		uint16_t data = 0xAA;
		spi_command command;
		defCommand(&command);
		MasterWriteDataBlocking(SPI_n, &command, data);

	}else{
		uint16_t data = 0xFF;
		spi_command command;
		defCommand(&command);
		MasterWriteDataBlocking(SPI_n, &command, data);

	}
	i++;
}

void setMasterBaudRate(uint8_t SPI_n,int mode, bool whichCTAR){
	SPIPtrs[SPI_n]->CTAR[whichCTAR] &= ~ SPI_CTAR_BR_MASK;
	SPIPtrs[SPI_n]->CTAR[whichCTAR] |= SPI_CTAR_BR(mode);
}

void setMasterBaudRatePrescaler(uint8_t SPI_n,int mode, bool whichCTAR){
	SPIPtrs[SPI_n]->CTAR[whichCTAR] &= ~ SPI_CTAR_PBR_MASK;
	SPIPtrs[SPI_n]->CTAR[whichCTAR] |= SPI_CTAR_PBR(mode);
}


void setMode(uint8_t SPI_n, bool mode){
	if(mode == MASTER){
		SPIPtrs[SPI_n]->MCR |= SPI_MCR_MSTR(MASTER);
	}else{
		SPIPtrs[SPI_n]->MCR &= ~SPI_MCR_MSTR_SHIFT;
	}
}

bool getMode(uint8_t SPI_n){
	return (SPIPtrs[SPI_n]->MCR & SPI_MCR_MSTR_SHIFT) == (SPI_MCR_MSTR_SHIFT);
}


void setSPIConfig(uint8_t SPI_n, int SPI_config){
	SPIPtrs[SPI_n]->MCR &= ~SPI_MCR_DCONF_MASK;
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DCONF(SPI_config);
}

void moduleDisable(uint8_t SPI_n){ // power saving
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_MDIS(ENABLE);
}

void moduleEnable(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~  SPI_MCR_MDIS_SHIFT;
}

void disableTxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DIS_TXF(ENABLE);
}

void enableTxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_DIS_TXF_SHIFT;
}

void disableRxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DIS_RXF(ENABLE);
}

void enableRxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_DIS_RXF_SHIFT;
}

void clearTxFIFO(uint8_t SPI_n){ //flush
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_CLR_TXF(ENABLE);
}

void dontClearTxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_CLR_TXF_SHIFT;
}

void clearRxFIFO(uint8_t SPI_n){  //flush
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_CLR_RXF(ENABLE);
}

void dontClearRxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_CLR_RXF_SHIFT;
}

void HALTStopTransfers(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_HALT(ENABLE);
}

void HALTStartTransfers(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_HALT_SHIFT;
}

void setPCSActiveHigh(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_PCSIS_MASK;
}

void setPCSActiveLow(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |=  SPI_MCR_PCSIS_MASK;
}





void clearTxCompleteFlag(uint8_t SPI_n){
	SPIPtrs[SPI_n]->SR |= SPI_SR_TCF(1);
}

void clearTxFifoFillRequestFlag(uint8_t SPI_n){
	SPIPtrs[SPI_n]->SR |= SPI_SR_TFFF(1);
}

void defCommand(spi_command* command){
	command->isPcsContinuous = false;
	command->whichCtar = 0;
	command->whichPcs = 0;
	command->isEndOfQueue = false;
	command->clearTransferCount = false;
}

void setCommandAttributes(spi_command * command, bool isPcsContinous,bool whichCtar,bool whichPcs, bool isEndOfQueue, bool clearTransferCount){
	command->isPcsContinuous = isPcsContinous;
	command->whichCtar = 0;
	command->whichPcs = 0;
	command->isEndOfQueue = false;
	command->clearTransferCount = false;
}

void MasterWriteDataBlocking(uint8_t SPI_n, spi_command *command, uint16_t data)
{
	SPI_Type * base = SPIPtrs[SPI_n];
    /* First, clear Transmit Complete Flag (TCF) */
	void clearTxCompleteFlag(uint8_t SPI_n);
	//bool = kDSPI_TxFifoFillRequestFlag =SPI_SR_TFFF_SHIFT
	//   while (!(DSPI_GetStatusFlags(base) & kDSPI_TxFifoFillRequestFlag))
    while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TFFF_SHIFT))
    {
    	clearTxFifoFillRequestFlag(SPI_0);
    }

    base->PUSHR = SPI_PUSHR_CONT(command->isPcsContinuous) | SPI_PUSHR_CTAS(command->whichCtar) |
                  SPI_PUSHR_PCS(command->whichPcs) | SPI_PUSHR_EOQ(command->isEndOfQueue) |
                  SPI_PUSHR_CTCNT(command->clearTransferCount) | SPI_PUSHR_TXDATA(data);


    //DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);
    void clearTxFifoFillRequestFlag(uint8_t SPI_n);


    /* Wait till TCF sets */
    //while (!(DSPI_GetStatusFlags(base) & kDSPI_TxCompleteFlag))
    while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TCF_SHIFT))
    {
    }
}


/*

status_t DSPI_MasterTransferBlocking(SPI_Type *base, dspi_transfer_t *transfer)
{
    uint16_t wordToSend = 0;
    uint16_t wordReceived = 0;
    uint8_t dummyData = 0xAA; // DUMMY DATA!!
    uint8_t bitsPerFrame;

    uint32_t command;
    uint32_t lastCommand;

    uint8_t *txData;
    uint8_t *rxData;
    uint32_t remainingSendByteCount;
    uint32_t remainingReceiveByteCount;

    uint32_t fifoSize;
    dspi_command_data_config_t commandStruct;

    // If the transfer count is zero, then return immediately.
    if (transfer->dataSize == 0)
    {
        return kStatus_InvalidArgument;
    }

    HALTStopTransfers(SPI_n);
    //DSPI_StopTransfer(base);
    DSPI_DisableInterrupts(base, kDSPI_AllInterruptEnable);
    DSPI_FlushFifo(base, true, true);
    DSPI_ClearStatusFlags(base, kDSPI_AllStatusFlag);

    //Calculate the command and lastCommand
    commandStruct.whichPcs =
        (dspi_which_pcs_t)(1U << ((transfer->configFlags & DSPI_MASTER_PCS_MASK) >> DSPI_MASTER_PCS_SHIFT));
    commandStruct.isEndOfQueue = false;
    commandStruct.clearTransferCount = false;
    commandStruct.whichCtar =
        (dspi_ctar_selection_t)((transfer->configFlags & DSPI_MASTER_CTAR_MASK) >> DSPI_MASTER_CTAR_SHIFT);
    commandStruct.isPcsContinuous = (bool)(transfer->configFlags & kDSPI_MasterPcsContinuous);

    command = DSPI_MasterGetFormattedCommand(&(commandStruct));

    commandStruct.isEndOfQueue = true;
    commandStruct.isPcsContinuous = (bool)(transfer->configFlags & kDSPI_MasterActiveAfterTransfer);
    lastCommand = DSPI_MasterGetFormattedCommand(&(commandStruct));

    //Calculate the bitsPerFrame//
    bitsPerFrame = ((base->CTAR[commandStruct.whichCtar] & SPI_CTAR_FMSZ_MASK) >> SPI_CTAR_FMSZ_SHIFT) + 1;

    txData = transfer->txData;
    rxData = transfer->rxData;
    remainingSendByteCount = transfer->dataSize;
    remainingReceiveByteCount = transfer->dataSize;

    if ((base->MCR & SPI_MCR_DIS_RXF_MASK) || (base->MCR & SPI_MCR_DIS_TXF_MASK))
    {
        fifoSize = 1;
    }
    else
    {
        fifoSize = FSL_FEATURE_DSPI_FIFO_SIZEn(base);
    }

    DSPI_StartTransfer(base);

    if (bitsPerFrame <= 8)
    {
        while (remainingSendByteCount > 0)
        {
            if (remainingSendByteCount == 1)
            {
                while (!(DSPI_GetStatusFlags(base) & kDSPI_TxFifoFillRequestFlag))
                {
                    DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);
                }

                if (txData != NULL)
                {
                    base->PUSHR = (*txData) | (lastCommand);
                    txData++;
                }
                else
                {
                    base->PUSHR = (lastCommand) | (dummyData);
                }
                DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);
                remainingSendByteCount--;

                while (remainingReceiveByteCount > 0)
                {
                    if (DSPI_GetStatusFlags(base) & kDSPI_RxFifoDrainRequestFlag)
                    {
                        if (rxData != NULL)
                        {
                            // Read data from POPR//
                            *(rxData) = DSPI_ReadData(base);
                            rxData++;
                        }
                        else
                        {
                            DSPI_ReadData(base);
                        }
                        remainingReceiveByteCount--;

                        DSPI_ClearStatusFlags(base, kDSPI_RxFifoDrainRequestFlag);
                    }
                }
            }
            else
            {
                //Wait until Tx Fifo is not full
                while (!(DSPI_GetStatusFlags(base) & kDSPI_TxFifoFillRequestFlag))
                {
                    DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);
                }
                if (txData != NULL)
                {
                    base->PUSHR = command | (uint16_t)(*txData);
                    txData++;
                }
                else
                {
                    base->PUSHR = command | dummyData;
                }
                remainingSendByteCount--;

                DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);

                while ((remainingReceiveByteCount - remainingSendByteCount) >= fifoSize)
                {
                    if (DSPI_GetStatusFlags(base) & kDSPI_RxFifoDrainRequestFlag)
                    {
                        if (rxData != NULL)
                        {
                            *(rxData) = DSPI_ReadData(base);
                            rxData++;
                        }
                        else
                        {
                            DSPI_ReadData(base);
                        }
                        remainingReceiveByteCount--;

                        DSPI_ClearStatusFlags(base, kDSPI_RxFifoDrainRequestFlag);
                    }
                }
            }
        }
    }
    else
    {
        while (remainingSendByteCount > 0)
        {
            if (remainingSendByteCount <= 2)
            {
                while (!(DSPI_GetStatusFlags(base) & kDSPI_TxFifoFillRequestFlag))
                {
                    DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);
                }

                if (txData != NULL)
                {
                    wordToSend = *(txData);
                    ++txData;

                    if (remainingSendByteCount > 1)
                    {
                        wordToSend |= (unsigned)(*(txData)) << 8U;
                        ++txData;
                    }
                }
                else
                {
                    wordToSend = dummyData;
                }

                base->PUSHR = lastCommand | wordToSend;

                DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);
                remainingSendByteCount = 0;

                while (remainingReceiveByteCount > 0)
                {
                    if (DSPI_GetStatusFlags(base) & kDSPI_RxFifoDrainRequestFlag)
                    {
                        wordReceived = DSPI_ReadData(base);

                        if (remainingReceiveByteCount != 1)
                        {
                            if (rxData != NULL)
                            {
                                *(rxData) = wordReceived;
                                ++rxData;
                                *(rxData) = wordReceived >> 8;
                                ++rxData;
                            }
                            remainingReceiveByteCount -= 2;
                        }
                        else
                        {
                            if (rxData != NULL)
                            {
                                *(rxData) = wordReceived;
                                ++rxData;
                            }
                            remainingReceiveByteCount--;
                        }
                        DSPI_ClearStatusFlags(base, kDSPI_RxFifoDrainRequestFlag);
                    }
                }
            }
            else
            {
                //Wait until Tx Fifo is not full//
                while (!(DSPI_GetStatusFlags(base) & kDSPI_TxFifoFillRequestFlag))
                {
                    DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);
                }

                if (txData != NULL)
                {
                    wordToSend = *(txData);
                    ++txData;
                    wordToSend |= (unsigned)(*(txData)) << 8U;
                    ++txData;
                }
                else
                {
                    wordToSend = dummyData;
                }
                base->PUSHR = command | wordToSend;
                remainingSendByteCount -= 2;

                DSPI_ClearStatusFlags(base, kDSPI_TxFifoFillRequestFlag);

                while (((remainingReceiveByteCount - remainingSendByteCount) / 2) >= fifoSize)
                {
                    if (DSPI_GetStatusFlags(base) & kDSPI_RxFifoDrainRequestFlag)
                    {
                        wordReceived = DSPI_ReadData(base);

                        if (rxData != NULL)
                        {
                            *rxData = wordReceived;
                            ++rxData;
                            *rxData = wordReceived >> 8;
                            ++rxData;
                        }
                        remainingReceiveByteCount -= 2;

                        DSPI_ClearStatusFlags(base, kDSPI_RxFifoDrainRequestFlag);
                    }
                }
            }
        }
    }

    return kStatus_Success;
}

*/
