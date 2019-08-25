#include "gpio.h"

static GPIO_Type* gpioPtrs[] = GPIO_BASE_PTRS;
static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static SIM_Type* simPtrs[] = SIM_BASE_PTRS;


/* * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void gpioMode (pin_t pin, uint8_t mode){
	SIM_Type* sim = simPtrs[0];
	sim->SCGC5 |= SIM_SCGC5_PORTB_MASK; //activo clock gating para B

	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	// connect to gpio
	port->PCR[8] = 1;
	port->PCR[9] = 0;
	port->PCR[10] = 0;

	GPIO_Type *gpio_port = gpioPtrs[PIN2PORT(pin)];
	int num = PIN2NUM(pin); // num es el numero de pin

	switch(mode){
		case INPUT:
			gpio_port->PDDR &= ~ (1<<num); // seteamos el pin como input
			break;
		case OUTPUT:
			gpio_port->PDDR |= 1<<num;// seteamos el pin como output
			break;
		case INPUT_PULLUP:
			gpio_port->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[1] = HIGH; //PULL ENABLE en 1
			port->PCR[0] = HIGH; //PULL SELECT en 1 (PULLUP)
			break;
		case INPUT_PULLDOWN:
			gpio_port->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[1] = HIGH; //PULL ENABLE en 1
			port->PCR[0] = LOW; //PULL SELECT en 0 (PULLDOWN)
			break;
	}

}

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value){
	uint32_t port_name = PIN2PORT(pin);
	uint32_t port_num = PIN2NUM(pin);
	GPIO_Type *gpio = gpioPtrs[port_name];
	if(value == HIGH){
		gpio->PDOR |= (1<<port_num);
	}else{
		gpio->PDOR &= ~ (1<<port_num);
	}
}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin){
	static bool toggle = 1;
	int num = PIN2NUM(pin);
	GPIO_Type *gpio_port = gpioPtrs[PIN2PORT(pin)];
	if (toggle){
		gpio_port->PTOR |= 1<<num; // prendemos el toggle
		toggle = 0;
	}else{
		gpio_port->PTOR &= ~(1<<num); // apagamos el toggle
		toggle = 1;
	}
}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin);


/*******************************************************************************
 ******************************************************************************/
