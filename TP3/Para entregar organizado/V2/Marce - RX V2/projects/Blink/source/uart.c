/***************************************************************************//**
  @file     UART.c
  @brief    UART Driver for K64F. Non-Blocking and using FIFO feature
  @author   Nicolás Magliola
 ******************************************************************************/

#include "MK64F12.h"
#include "uart.h"
#include "gpio.h"
#include "board.h"
#include <stdbool.h>
#include <stdio.h>

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
#define __ISR__ 		void __attribute__ ((interrupt))
#define __CORE_CLOCK__ 	100000000U

static uint8_t rx_data;
static uint8_t dummy;

#define BUFFER_SIZE 10000

#define WORD_LENGTH 50 // palabras de hasta longitud 50
#define WORD_COUNT 20 // hasta 20 palabras


char sendBuffer[BUFFER_SIZE];
char recvBuffer[BUFFER_SIZE];

long long porEnviar = 0;
int enviado = 0;

long long porRecibir = 0;
long long recibido = 0;

void (*recvListener)(char);

long long wordMemoryToRead = 0; // memoria leida
long long wordMemoryRead = 0; // memoria por leer
long long longitud = 0; // variable auxiliar de longitud

__ISR__ UART0_RX_TX_IRQHandler (void){ // UART INTERRUPT!!
	uint8_t s1 = UART0->S1;

	if (s1 & UART_S1_RDRF_MASK){ // Llego un dato!! Importante! recibo de a uno

		rx_data = UART0->D; // RX DATA
		//dummy = UART0->S1;
		recvBuffer[porRecibir] = rx_data;
		porRecibir = (porRecibir + 1) % BUFFER_SIZE;
		//UART0->C2 &= ~UART_C2_RIE_MASK;


	}else{ // if (s1 & UART_S1_TDRE_MASK) // TX IRQ
		UART0->D = sendBuffer[enviado];
		enviado = (enviado + 1) % BUFFER_SIZE;

		if (enviado == porEnviar){

			UART0->TWFIFO = 0;
			UART0->C2 &= ~UART_C2_TE_MASK;
			UART0->C2 |= UART_C2_TE_MASK;

			UART0->C2 &= ~UART_C2_TIE_MASK;

		}
	}
}

void sendWord(const char *word){
	int i = 0;
	while (word[i] != '\0'){
		sendBuffer[porEnviar] = word[i];
		porEnviar = (porEnviar + 1) % BUFFER_SIZE;
		i ++;
	}
	UART0->C2 |= UART_C2_TIE_MASK;
}

void popWord(){ // sacamos una palabra de la cola
	wordMemoryRead = (wordMemoryRead + 1) % BUFFER_SIZE;
}
void updateWord(){

	if (recibido != porRecibir){
		recvListener(recvBuffer[recibido]);
		recibido = (recibido + 1) % BUFFER_SIZE;

//		longitud ++;
//
//		if (recvBuffer[recibido] == '\n'){ // palabra completa detectada
//			// escribimos la palabra
//			for (int i = 0;i < longitud;i++){
//				words[wordMemoryToRead][i] = recvBuffer[recibido - longitud + 1 + i];
//			}
//			wordsLength[wordMemoryToRead] = longitud;
//
//			wordMemoryToRead = (wordMemoryToRead + 1) % BUFFER_SIZE;
//			longitud = 0;
//		}
//		recibido = (recibido + 1) % BUFFER_SIZE;
		// ya tome todo lo que hay por recibir
	}

	// luego busco el primer enter

}
void setOnNewWordListener(){

}

void configPCR(int portName, int nume, uint32_t value, int pull_up){
	int pin = PORTNUM2PIN(portName, nume);
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin);

	if (pull_up){
		port->PCR[num] |= PORT_PCR_PE(1);
		port->PCR[num] |= PORT_PCR_PS(1);
	}
	//if (portName == PB){ // clock gating
	//	SIM->SCGC5 |= SIM_SCGC5_PORTA(num<<1);
	//}
	port->PCR[num] &= ~PORT_PCR_MUX(7); // fuerzo a valer 0 los PCR MUX
	port->PCR[num] |= PORT_PCR_MUX(value); // hago valer lo que tienen que valer los PCR MUX
	port->PCR[num] &= ~PORT_PCR_IRQC_MASK;
	port->PCR[num] |= PORT_PCR_IRQC(0b0000);
}
void uartInit (uart_cfg_t config){

	// Activo el clock gating

	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART3_MASK;
	SIM->SCGC1 |= SIM_SCGC1_UART4_MASK;
	SIM->SCGC1 |= SIM_SCGC1_UART5_MASK;

	// Configuro puertos
	configPCR(PB, 17, 3, 0); //PB para la PC tx
	configPCR(PB, 16, 3, 0); //PB para la PC rx

	//gpioWrite(PIN_PRUEBA,HIGH);
	//gpioWrite(PIN_PRUEBA,LOW);

	// Habilito interrupciones de UART0


	UART0->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
	UART0->D = 0;

	UART0->TWFIFO = 0;
	UART0->C1 = 0x00;

	NVIC_EnableIRQ(UART0_RX_TX_IRQn);
	UART0->C2 |= UART_C2_RIE_MASK;
	// Baudrate, brfa
	uint32_t clock;
	uint16_t sbr, brfa;

	clock = __CORE_CLOCK__; //5000000U;

	sbr = clock / (config.baudrate << 4);
	brfa = (clock << 1) / config.baudrate - (sbr << 5);

	//UART_Type *config;

	// BAUDRATE

	// pongo todos para hacer una prueba
	UART0->BDH = UART_BDH_SBR(sbr >> 8);
	UART0->BDL = UART_BDL_SBR(sbr);

	// CONFIGURO length, parity (C1)

	//UART0->C1 = 0x00;// 0x01;
	// ESCRIBO MA1, MA2, C4
	UART0->C4 &= ~UART_C4_BRFA_MASK;
	UART0->C4 |= (uint8_t)(brfa);

	//UART3->MA1 = 0x55;
	//UART3->MA2 = 0x33;

	// ACTIVO TRANSMISOR INTERRUPCIONES Y RECEPTOR, WAKEUP CON C2, S2, C3

	 // TE, RE, RIE-> interrupciones
	//UART0->S2 &= ~(0x06); // MSBF = 0, BRK13 = 0


	//UART0->TWFIFO = 0;
	//UART0->C2 = 0;

//	UART0->TWFIFO = 0; /* Set tx/rx FIFO watermark */
//	//UART0->PFIFO |= UART_PFIFO_TXFE_MASK | UART_PFIFO_RXFE_MASK; /* Enable TX/RX FIFO */
//	//UART0->CFIFO = UART_CFIFO_TXFLUSH_MASK | UART_CFIFO_RXFLUSH_MASK; /* Flush FIFO */
//	UART0->PFIFO = 0;
//
//	UART0->PFIFO |= UART_PFIFO_TXFE_MASK | UART_PFIFO_RXFE_MASK; /* Enable TX/RX FIFO */
//	UART0->PFIFO |= UART_PFIFO_RXFIFOSIZE(0);
//	UART0->PFIFO |= UART_PFIFO_TXFIFOSIZE(0);
//
//	UART0->CFIFO = UART_CFIFO_TXFLUSH_MASK | UART_CFIFO_RXFLUSH_MASK; /* Flush FIFO */


	UART0->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;


	//UART0->C2 &= ~UART_C2_RIE_MASK;
	//UART0->C2 &= ~UART_C2_TIE_MASK;
	delayLoop(10000000);
}
void setOnNewCharListener(void *recv(char)){
	recvListener = recv;
}

void delayLoop(uint32_t veces)
{
    while (veces--);
}



