/*
 * SPI.h
 *
 *  Created on: 18 sep. 2019
 *      Author: joa-m
 */

#ifndef SPI_H_
#define SPI_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "board.h"
//#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//#define PIN2PORT(p)         (((p)>>5) & 0x07)
//#define LOW     0
//#define HIGH    1
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint8_t pin_t;
enum{SLAVE,MASTER};
enum{DISABLE,ENABLE};
enum{PULL_DOWN,PULL_UP,DISABLE_PULL};
enum{SPI_0,SPI_1,SPI_2};
enum{SPI_CONFIG,RESERVED_0,RESERVED_1,RESERVED_2};

typedef struct{
	bool whichCTAR;
	bool SCKE;
	bool MTFE;
	bool ROOE;
	uint8_t SMPL_PT;
	bool DIS_TXF;
	bool DIS_RXF;

	uint8_t CTAR_FMSZ;
	bool CTAR_CPOL;
	bool CTAR_CPHA;
	bool CTAR_LSBFE;
} spi_master_config_t;


typedef struct{

	bool isPcsContinuous;
	bool whichCtar;
	bool whichPcs;
	bool isEndOfQueue;
	bool clearTransferCount;

}spi_command;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief sets Master Mode
 * @param the number of SPI
 * @return void
 */
//void setMasterMode(uint8_t SPI_n);

void masterConfig(spi_master_config_t * master_cfg);

void masterInitiliaze(uint8_t SPI_n);

void SPI_Initialize(void);

void SPIClockGatingEnable(uint8_t SPI_n);

void SPIMode(pin_t pin, uint8_t mode, uint8_t mux_alt);

void testSPI(uint8_t SPI_n);

void setMasterBaudRate(uint8_t SPI_n,int mode, bool whichCTAR);

void setMasterBaudRatePrescaler(uint8_t SPI_n,int mode, bool whichCTAR);

void setMode(uint8_t SPI_n, bool mode);

bool getMode(uint8_t SPI_n);

void setSPIConfig(uint8_t SPI_n, int SPI_config);

void moduleDisable(uint8_t SPI_n);

void moduleEnable(uint8_t SPI_n);

void disableTxFIFO(uint8_t SPI_n);

void enableTxFIFO(uint8_t SPI_n);

void disableRxFIFO(uint8_t SPI_n);

void enableRxFIFO(uint8_t SPI_n);


void clearTxFIFO(uint8_t SPI_n); //flush

void dontClearTxFIFO(uint8_t SPI_n);


void clearRxFIFO(uint8_t SPI_n); //flush

void dontClearRxFIFO(uint8_t SPI_n);


void HALTStopTransfers(uint8_t SPI_n);

void HALTStartTransfers(uint8_t SPI_n);


void setPCSActiveHigh(uint8_t SPI_n);

void setPCSActiveLow(uint8_t SPI_n);





void clearTxCompleteFlag(uint8_t SPI_n);

void clearTxFifoFillRequestFlag(uint8_t SPI_n);


void defCommand(spi_command* command);


void MasterWriteDataBlocking(uint8_t SPI_n, spi_command *command, uint16_t data);






/*******************************************************************************
 ******************************************************************************/


#endif /* SPI_H_ */
