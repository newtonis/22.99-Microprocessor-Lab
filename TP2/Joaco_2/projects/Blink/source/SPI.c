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

	//int aux = PORTNUM2PIN(PA,4);
	/*
	SPIMode(93, DISABLE_PULL, SPI_alternative );
	SPIMode(94, DISABLE_PULL, SPI_alternative );
	SPIMode(95, DISABLE_PULL, SPI_alternative );
	SPIMode(96, DISABLE_PULL, SPI_alternative );
	*/
	SPIMode(PIN_PCS0, DISABLE_PULL, SPI_alternative );
	SPIMode(PIN_SCK_MASTER, DISABLE_PULL, SPI_alternative );
	SPIMode(PIN_MOSI_MASTER, DISABLE_PULL, SPI_alternative );
	SPIMode(PIN_MISO_MASTER, DISABLE_PULL, SPI_alternative );


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
	//spiEnableInterrupts(SPI_0, SPI_RSER_TFFF_RE_MASK);
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
	int threshold = 100;
	if(i < threshold){
		uint16_t data = 0x00;
		spi_command command;
		command.isPcsContinuous = true;
		command.isEndOfQueue = false;
		command.whichPcs = Pcs0;
		command.whichCtar = 0;
		command.clearTransferCount = 0;
		MasterWriteCommandDataBlocking(SPI_n, data);
		//MasterWriteDataBlocking(SPI_n, &command, data);

	}else{
		uint16_t data = 0xFF;
		spi_command command;

		command.isPcsContinuous = true;
		command.isEndOfQueue = false;
		command.whichPcs = Pcs0;
		command.whichCtar = 0;
		command.clearTransferCount = 0;

		MasterWriteCommandDataBlocking(SPI_n, data);
		//MasterWriteDataBlocking(SPI_n, &command, data);
		if(i == threshold*2){
			i = -1 ;
		}
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
	SPIPtrs[SPI_n]->SR |= SPI_SR_TFFF_SHIFT;
}

void defCommand(spi_command* command){
	command->isPcsContinuous = false;
	command->whichCtar = 0;
	command->whichPcs = 0;
	command->isEndOfQueue = false;
	command->clearTransferCount = false;
}

void MasterWriteDataBlocking(uint8_t SPI_n, spi_command *command, uint16_t data)
{
	/* First, clear Transmit Complete Flag (TCF) */
	clearTxCompleteFlag(SPI_n);

	while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TFFF_SHIFT))
    {
    	clearTxFifoFillRequestFlag(SPI_n);
    }

	SPIPtrs[SPI_n]->PUSHR = SPI_PUSHR_CONT(command->isPcsContinuous) | SPI_PUSHR_CTAS(command->whichCtar) |
                  SPI_PUSHR_PCS(command->whichPcs) | SPI_PUSHR_EOQ(command->isEndOfQueue) |
                  SPI_PUSHR_CTCNT(command->clearTransferCount) | SPI_PUSHR_TXDATA(data);


    clearTxFifoFillRequestFlag(SPI_n);

    /* Wait till TCF sets */
    while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TCF_SHIFT))
    {
    }
}



void MasterWriteCommandDataBlocking(uint8_t SPI_n, uint32_t data)
{

	clearTxCompleteFlag(SPI_n);
	/*
	while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TFFF_SHIFT))

    {
    	clearTxFifoFillRequestFlag(SPI_n);
    }
    */

	SPIPtrs[SPI_n]->PUSHR = data;

    clearTxFifoFillRequestFlag(SPI_n);

    /* Wait till TCF sets */
    //while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TCF_SHIFT))
    //{
    //}
}

/*
void clearInterruptFlag(pin_t pin){
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin); // num es el numero de pin
	port->PCR[num] |= PORT_PCR_ISF_MASK;
}

void clearAllSPIInterruptFlags(void){
	clearInterruptFlag(PIN_PCS0);
	clearInterruptFlag(PIN_SCK_MASTER);
	clearInterruptFlag(PIN_MOSI_MASTER);
	clearInterruptFlag(PIN_MISO_MASTER);
}

void setInterruptConfig(pin_t pin, isf_configs_t config){
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin); // num es el numero de pin
	port->PCR[num] &= ~PORT_PCR_IRQC_MASK;
	port->PCR[num] |= PORT_PCR_IRQC(config);
}

void configAllSPIInterrupts(void){
	setInterruptConfig(PIN_PCS0, ISF_DMA_RISING_EDGE);
	setInterruptConfig(PIN_SCK_MASTER, ISF_DMA_RISING_EDGE);
	setInterruptConfig(PIN_MOSI_MASTER, ISF_DMA_RISING_EDGE);
	setInterruptConfig(PIN_MISO_MASTER, ISF_DMA_RISING_EDGE);
}

void spiEnableInterrupts(uint8_t SPI_n, uint32_t mask)
{
	SPI_Type* base = SPIPtrs[SPI_n];
    if (mask & SPI_RSER_TFFF_RE_MASK)
    {
        base->RSER &= ~SPI_RSER_TFFF_DIRS_MASK;
    }
    if (mask & SPI_RSER_RFDF_RE_MASK)
    {
        base->RSER &= ~SPI_RSER_RFDF_DIRS_MASK;
    }
    base->RSER |= mask;
}
*/

