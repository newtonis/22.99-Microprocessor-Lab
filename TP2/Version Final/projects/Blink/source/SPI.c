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


void SPI_Init (void){
	SPI_Type * spi_pointer = spiPtrs[0];
	setup_pin(PIN_SCK);
	setup_pin(PIN_MOSI);
	setup_pin(PIN_MISO);
	setup_pin(PIN_CS);

	SIM->SCGC6 |= SIM_SCGC6_SPI0(1); //habilite el clock del periferico de spi

	spi_pointer->MCR=SPI_MCR_HALT_MASK|SPI_MCR_MSTR_MASK;

	spi_pointer->CTAR[0] = 	  SPI_CTAR_CPOL(0) |  \
							 SPI_CTAR_CPHA(0)| \
							 SPI_CTAR_PBR(BAUD_RATE_PRESCALER) | \
							 SPI_CTAR_BR(BAUD_RATE) | \
							 SPI_CTAR_FMSZ(8-1)| \
							 SPI_CTAR_ASC(BAUD_RATE-3) | \
							 SPI_CTAR_PASC(BAUD_RATE_PRESCALER) | \
							 SPI_CTAR_CSSCK(BAUD_RATE-3) | \
							 SPI_CTAR_PCSSCK(BAUD_RATE_PRESCALER) |
							 SPI_CTAR_PDT(0)|\
							 SPI_CTAR_DT(0);
	spi_pointer->MCR=(spi_pointer->MCR & (~SPI_MCR_PCSIS_MASK))|SPI_MCR_PCSIS(1);
	spi_pointer->MCR =(spi_pointer->MCR & (~(SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK | SPI_MCR_MSTR_MASK)))| \
														 SPI_MCR_MDIS(0) | \
														 SPI_MCR_HALT(0) | \
														 SPI_MCR_MSTR(1);

}

void setup_pin (pin_t pin){
	int pinPort = PIN2PORT(pin);
	int pinBit = PIN2NUM(pin);
	SIM->SCGC5 |= sim_port[pinPort]; //clock gating
	ports[pinPort]->PCR[pinBit]=	PORT_PCR_MUX(SPI_ALTERNATIVE) | \
									PORT_PCR_IRQC(0) | \
									PORT_PCR_PS(0)| \
									PORT_PCR_PE(0);
}


uint8_t SPI_MasterReadWrite(uint8_t * data2end, uint8_t size,uint8_t * recivedData){
	SPI_Type * spi_pointer = spiPtrs[0];
	uint32_t data_2_send=0;
	uint8_t received_data_cnt=0;

	for(int i=0;i<size;i++){
		if(i== (size-1)){// si es el ultimo
			data_2_send=FINAL_DATA2SEND_MASK|SPI_PUSHR_TXDATA(data2end[i]);
		}else{
			data_2_send=INITIAL_DATA2SEND_MASK|SPI_PUSHR_TXDATA(data2end[i]);
		}

		spi_pointer->MCR =(spi_pointer->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(1);

		spi_pointer->SR=(spi_pointer->SR & (~SPI_SR_TCF_MASK) ) | SPI_SR_TCF(1) ; //reinicio el tcf
		spi_pointer->PUSHR=data_2_send;

		spi_pointer->MCR =(spi_pointer->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(0);

		while(!(spi_pointer->SR & SPI_SR_TCF_MASK));//espero a que se envie el frame

		if(recivedData!=NULL){
			if(spi_pointer->SR & SPI_SR_RXCTR_MASK){
				recivedData[received_data_cnt]=spi_pointer->POPR;
				received_data_cnt++;

			}
		}else{
			spi_pointer->POPR;
		}
	}
	return received_data_cnt;
}

