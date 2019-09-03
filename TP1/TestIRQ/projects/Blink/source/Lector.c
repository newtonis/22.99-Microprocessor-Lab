#include "Lector.h"
#include "board.h"



static int ID_num [ID_LENGTH];
static uint8_t data [DATA_LENGTH];
static bool Enable = LOW;
static uint8_t count = 0;
static uint8_t bits = 0;
static uint8_t lrc = 0;
uint8_t word = 0;
bool end = LOW;
bool parity (uint8_t lrc);
void clear_ID (void);
bool Check_LRC (void);


__ISR__ PORTC_IRQHandler (void){

	if( ((PORTC -> PCR[ PIN2NUM(EN)]) & (PORT_PCR_ISF_MASK)) == (PORT_PCR_ISF_MASK))
	{
		set_Enable(gpioRead(EN));
		PORT_ClearInterruptFlag(EN);


	}
	else if (((PORTC -> PCR[ PIN2NUM(CLK)]) & (PORT_PCR_ISF_MASK)) ==PORT_PCR_ISF_MASK)
	{
		get_Data(gpioRead(DATA));
		PORT_ClearInterruptFlag(CLK);
	}

}


void Lector_Init(void)
{
	gpioMode(EN, INPUT);
	gpioMode(DATA, INPUT);
	gpioMode(CLK, INPUT);

	gpioIRQC(EN, INTERRUPT_BOTH_EDGES);
	gpioIRQC(CLK, INTERRUPT_FALLING_EDGE);

	NVIC_EnableIRQ(PORTC_IRQn);
}

int * get_ID (void)
{
	if ((Enable == LOW) && (end == HIGH) && (Check_LRC())) //esta bien
	{
		for (int i = 0; i < (ID_LENGTH); i++)
		{
			ID_num[i] = (data[i+1] & PARITY_BIT_MASK_ODD); //le saco el bit de paridad
		}
		return &ID_num[0];
	}
	else
	{
		return NULL;
	}
}
void clear_ID (void)
{
	for (int i = 0; i < ID_LENGTH ; i++)
		{
			ID_num[i] = 0;
		}
}


bool Check_LRC (void)
{
	uint8_t my_lrc = 0;
	for (int i = 0; i < count ; i++ )
	{
		my_lrc ^= data[i];
	}

	if (parity(my_lrc))
	{
		my_lrc |= PARITY_BIT_MASK_EVEN;
	}
	else
	{
		my_lrc &= PARITY_BIT_MASK_ODD;
	}

	if (lrc == my_lrc)
		return true;
	else
		return false;

}

bool parity (uint8_t num)
{
	bool par = 1;
	for (int i = 0 ; i < (BITSXNUM - 1); i++)
	{
		par ^= ((num & (1<<i))>>i);
	}
	return par;
}

void get_Data (bool my_data)
{
	if(Enable == HIGH) //CUANDO EL ENABLE ESTA PRENDIDO LEE SI NO NO
	{
		if (my_data == 0)
			{
				word = (word)| (1 <<bits);
			}

			bits++;
			if(bits == (BITSXNUM))
			{
				if ((count == 0) & (word != SS) ) //BUSCA START SENTINEL
				{
					bits--;
					word = word >> 1;
				}

				else
				{
					data[count] = word;
					if (data[count-1] == END_SENTINEL ) // BUSCA END SENTINEL
					{
						lrc = data[count];
						end = HIGH;
					}
					if (end == LOW)
					{
						count++;
						bits = 0;
						word = 0;
					}

				}
			}
	}
}


void set_Enable(bool status)
{
	Enable = !status;
	if (Enable == HIGH) //cuando empieza a leer
	{
		count = 0;
		bits = 0;
		end = 0;
		word = 0;
		clear_ID();
	}

}
