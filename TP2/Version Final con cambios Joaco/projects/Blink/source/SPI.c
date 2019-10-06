/*
 * SPI_driver.c
 *
 *  Created on: Oct 2, 2019
 *      Author: joa-m
 */
#include "board.h"
#include "MK64F12.h"
#include "SPI.h"
#include <stdio.h>


static SPI_Type *spiPtrs[] = SPI_BASE_PTRS;
static PORT_Type * ports[] = PORT_BASE_PTRS;
static uint32_t sim_port[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK};
static SIM_Type* sim_ptr = SIM;

void SPIClockGatingEnable(uint8_t SPI_n){
	switch(SPI_n){
		case SPI_0:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI0(1); // activo clock gating
			break;
		case SPI_1:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI1(1);
			break;
		case SPI_2:
			sim_ptr->SCGC3 |= SIM_SCGC3_SPI2(1);
			break;
	}
}

void SPI_driver_init (void){
	uint8_t SPI_module = SPI_0;
	SPI_Type * spi_mod = spiPtrs[SPI_module];
	setup_pin(PIN_SCK);
	setup_pin(PIN_MOSI);
	setup_pin(PIN_MISO);
	setup_pin(PIN_CS);


//	SIM->SCGC6 |= SIM_SCGC6_SPI0(1); //habilite el clock del periferico de spi
	SPIClockGatingEnable(SPI_module);

	spi_cfg_t master_cfg;
	master_cfg.CTAR_CPOL = 0;
	master_cfg.CTAR_CPHA = 0;
	master_cfg.CTAR_PBR = 1;
	master_cfg.CTAR_BR = 5;
	master_cfg.CTAR_FMSZ = 8;
	master_cfg.CTAR_ASC = master_cfg.CTAR_BR - 3;
	master_cfg.CTAR_PASC = master_cfg.CTAR_PBR;
	master_cfg.CTAR_CSSCK = master_cfg.CTAR_BR -3;
	master_cfg.CTAR_PCSSCK = master_cfg.CTAR_PBR;
	master_cfg.CTAR_PDT = 0;
	master_cfg.CTAR_DT = 0;


	spi_mod->MCR=SPI_MCR_HALT_MASK|SPI_MCR_MSTR_MASK;
	/*
	spi_mod->CTAR[0] = 			 SPI_CTAR_CPOL(0) |  \
							 	 SPI_CTAR_CPHA(0)| \
								 SPI_CTAR_PBR(SPI_DRIVER_PBR) | \
								 SPI_CTAR_BR(SPI_DRIVER_BR) | \
								 SPI_CTAR_FMSZ(8-1)| \
								 SPI_CTAR_ASC(SPI_DRIVER_BR-3) | \
								 SPI_CTAR_PASC(SPI_DRIVER_PBR) | \
								 SPI_CTAR_CSSCK(SPI_DRIVER_BR-3) | \
								 SPI_CTAR_PCSSCK(SPI_DRIVER_PBR) |
								 SPI_CTAR_PDT(0)|\
								 SPI_CTAR_DT(0);
	*/
	spi_mod->CTAR[0] =   SPI_CTAR_CPOL(master_cfg.CTAR_CPOL) | \
					 	 SPI_CTAR_CPHA(master_cfg.CTAR_CPHA) | \
						 SPI_CTAR_PBR(master_cfg.CTAR_PBR) | \
						 SPI_CTAR_BR(master_cfg.CTAR_BR) | \
						 SPI_CTAR_FMSZ(master_cfg.CTAR_FMSZ-1) | \
						 SPI_CTAR_ASC(master_cfg.CTAR_ASC) | \
						 SPI_CTAR_PASC(master_cfg.CTAR_PASC) | \
						 SPI_CTAR_CSSCK(master_cfg.CTAR_CSSCK) | \
						 SPI_CTAR_PCSSCK(master_cfg.CTAR_PCSSCK) |
						 SPI_CTAR_PDT(master_cfg.CTAR_PDT) | \
						 SPI_CTAR_DT(master_cfg.CTAR_DT);

	spi_mod->MCR=(spi_mod->MCR & (~SPI_MCR_PCSIS_MASK))|SPI_MCR_PCSIS(1);
	spi_mod->MCR =(spi_mod->MCR & (~(SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK | SPI_MCR_MSTR_MASK))) \
														| SPI_MCR_MDIS(0) |SPI_MCR_HALT(0) | SPI_MCR_MSTR(1);

}

void setup_pin (pin_t pin){
	int pinPort = PIN2PORT(pin);
	int pinBit = PIN2NUM(pin);
	SIM->SCGC5 |= sim_port[pinPort]; //Habilito el clock al puerto correspondiente

	//configuro el pcr del pin
	ports[pinPort]->PCR[pinBit]= 	PORT_PCR_MUX(SPI_ALTERNATIVE) | \
									PORT_PCR_IRQC(0) | \
									PORT_PCR_PS(0)| \
									PORT_PCR_PE(0);
}


uint8_t SPI_driver_sendRecive(uint8_t * data2end, uint8_t size,uint8_t * recivedData){
	uint8_t SPI_module = SPI_0;
	SPI_Type * spi_mod = spiPtrs[SPI_module];
	uint32_t pushr2send=0;
	uint8_t dataRecived=0;

	for(int i=0;i<size;i++){
		if(i==(size-1)){//estoy en el ultimo dato a mandar
			pushr2send=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(0)|SPI_PUSHR_EOQ(1)|SPI_PUSHR_TXDATA(data2end[i]);
		}else{ //no es el ultimo dato que mando
			pushr2send=SPI_PUSHR_PCS(1)|SPI_PUSHR_CONT(1)|SPI_PUSHR_TXDATA(data2end[i]);
		}

		spi_mod->MCR =(spi_mod->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(1);

		spi_mod->SR=(spi_mod->SR & (~SPI_SR_TCF_MASK) ) | SPI_SR_TCF(1) ; //reinicio el tcf
		spi_mod->PUSHR=pushr2send;

		spi_mod->MCR =(spi_mod->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(0);
		while(!(spi_mod->SR & SPI_SR_TCF_MASK));//espero a que se envie el frame

		if(recivedData!=NULL){
			if(spi_mod->SR & SPI_SR_RXCTR_MASK){
				recivedData[dataRecived]=spi_mod->POPR;
				dataRecived++;

			}
		}else{
			spi_mod->POPR;
		}
	}
	return dataRecived;
}

