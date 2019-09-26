/***************************************************************************//**
  @file     UART.c
  @brief    UART Driver for K64F. Non-Blocking and using FIFO feature
  @author   Nicolás Magliola
 ******************************************************************************/

#include "MK64F12.h"
#include "uart.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

static PORT_Type* portPtrs[] = PORT_BASE_PTRS;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize UART driver
 * @param id UART's number
 * @param config UART's configuration (baudrate, parity, etc.)
*/

//static void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate)
//{
//	uint16_t sbr, brfa;
//	uint32_t clock;
//
//	clock = ((uart == UART0) || (uart == UART1))?(__CORE_CLOCK__):(__CORE_CLOCK__ >> 1);
//
//	sbr = clock / (baudrate << 4);               // sbr = clock/(Baudrate x 16)
//	brfa = (clock << 1) / baudrate - (sbr << 5); // brfa = 2*Clock/baudrate - 32*sbr
//
//
//	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
//}

#define UART0_TX_PIN 	17   //PTB17
#define UART0_RX_PIN 	16   //PTB16
#define PORT_UART0	PORTB
#define GPIO_UART0	PTB

void configPCR(int portName, int nume, uint32_t value){

	int pin = PORTNUM2PIN(portName, nume);
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin);
	port->PCR[num] &= ~PORT_PCR_MUX(7); // fuerzo a valer 0 los PCR MUX
	port->PCR[num] |= PORT_PCR_MUX(value); // hago valer lo que tienen que valer los PCR MUX



}
void uartInit (uart_cfg_t config){
	//SIM_SCGC4 |= SIM_SCGC4_UART3_MASK;

	// ACTIVO EL CLOCK
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART3_MASK;
	SIM->SCGC1 |= SIM_SCGC1_UART4_MASK;
	SIM->SCGC1 |= SIM_SCGC1_UART5_MASK;


	// CONFIGURO PUERTOS
	configPCR(PC, 17, 3);
	configPCR(PC, 16, 3);

	// BAUDRATE, BRFA

	uint32_t clock;
	uint16_t sbr, brfa;

	clock = 5000000U;

	sbr = clock / (config.baudrate << 4);
	brfa = (clock << 1) / config.baudrate - (sbr << 5);

	//UART_Type *config;

	// BAUDRATE

	UART3->BDH = UART_BDH_SBR(sbr >> 8);
	UART3->BDL = UART_BDL_SBR(sbr);

	// CONFIGURO length, parity (C1)

	UART3->C1 = 0x01;

	// ESCRIBO MA1, MA2, C4
	UART3->C4 = 0xE0 | (uint8_t)(brfa);

	//UART3->MA1 = 0x55;
	//UART3->MA2 = 0x33;

	// ACTIVO TRANSMISOR INTERRUPCIONES Y RECEPTOR, WAKEUP CON C2, S2, C3

	UART3->C2 |= 0x0A; // TE, RE = 1 RIE-> interrupciones
	UART3->S2 &= ~(0x06); // MSBF = 0, BRK13 = 0

	UART3->C3 &= ~0x0F; // ORIE=NEIE=PEIE=FEIE = 0 -> solo con interrupciones


	int i = 0;

	while (1){
		while (!(UART3->S1 & 0x80)){}
		//int j = 1000;
		//while (j < 1000) j--;

		if (i % 20 == 0){
			UART3->C3 |= UART_C3_T8_SHIFT; // enviamos un 1
			UART3->D = 0xAA;
		}else{
			//UART3->C3 &= ~0x40; //enviamos un 1
			//UART3->D = 0x55;
			UART3->C3 |= UART_C3_T8_SHIFT; // enviamos un 0
			UART3->D = 0x00;
		}
		i ++;
	}

}



/**
 * @brief Check if a new byte was received
 * @param id UART's number
 * @return A new byte has being received
*/

uint8_t uartIsRxMsg(uint8_t id){

}

/**
 * @brief Check how many bytes were received
 * @param id UART's number
 * @return Quantity of received bytes
*/
uint8_t uartGetRxMsgLength(uint8_t id){

}

/**
 * @brief Read a received message. Non-Blocking
 * @param id UART's number
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant){

}

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id UART's number
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant){

}

/**
 * @brief Check if all bytes were transfered
 * @param id UART's number
 * @return All bytes were transfered
*/
uint8_t uartIsTxMsgComplete(uint8_t id){

}

