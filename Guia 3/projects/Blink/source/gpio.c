#include "gpio.h"

static GPIO_Type* gpioPtrs[] = GPIO_BASE_PTRS;
static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static SIM_Type* sim_ptr = SIM;


/* * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void gpioMode (pin_t pin, uint8_t mode){
	sim_ptr->SCGC5 |= SIM_SCGC5_PORTB_MASK; //activo clock gating para B

	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	GPIO_Type *gpio_port = gpioPtrs[PIN2PORT(pin)];

	uint32_t num = PIN2NUM(pin); // num es el numero de pin

	// connect to gpio (hay un PCR por pin)
	port->PCR[num] = 0x0;
	port->PCR[num] |= PORT_PCR_MUX(1);
	port->PCR[num] |= PORT_PCR_DSE(1);
	port->PCR[num] |= PORT_PCR_IRQC(0);

	switch(mode){
		case INPUT:
			gpio_port->PDDR &= ~ (1<<num); // seteamos el pin como input
			break;
		case OUTPUT:
			gpio_port->PDDR |= 1<<num;// seteamos el pin como output
			break;
		case INPUT_PULLUP:
			gpio_port->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] |= HIGH<<0; //PULL SELECT en 1 (PULLUP)
			break;
		case INPUT_PULLDOWN:
			gpio_port->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] &= ~(HIGH); //PULL SELECT en 0 (PULLDOWN)
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
	if (gpioRead(pin) == LOW){
		gpioWrite(pin,HIGH);
	}else{
		gpioWrite(pin,LOW);
	}
}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin){
	uint32_t port_name = PIN2PORT(pin);
	uint32_t port_num = PIN2NUM(pin);
	GPIO_Type *gpio = gpioPtrs[port_name];
	return ( (1<<port_num) & gpio->PDOR ) == 1<<port_num ;
}


/*******************************************************************************
 ******************************************************************************/
