#include "Lector.h"
#include "board.h"

static int pan [PAN_MAX_L];
static uint8_t data [DATA_LENGTH];
static bool Enable = LOW;
static void (*callback)();

static bool chkEnable = 0;

static uint8_t count = 0;
static uint8_t bits = 0;
static uint8_t lrc = 0;
static uint8_t word = 0;
static bool end = LOW;


static bool parity (uint8_t lrc);
static bool Check_LRC (void);
static void get_Data (bool my_data);
static void set_Enable(bool status);
static void isr_enable (void);
static void isr_clk (void);
static void clear_PAN (void);


void isr_enable (void)
{
	if(Enable){
		chkEnable = 1;
	}else{
		chkEnable = 0;
	}

	set_Enable(gpioRead(EN));
}
void isr_clk (void)
{
	get_Data(gpioRead(DATA));
}

/*
bool newReadAvailable() //agregue esta funcion
{
	if ((Enable == LOW) && (end == HIGH)) // si esta bien
	{
		return true;
	}
	return false;
}
*/

int * lector_get_PAN (void)
{
	//if ((Enable == LOW) && (end == HIGH)) //esta bien y aca saque el checkLRC
	//{
		int i = 0;
		while ((data[i+1] != FS) && (i < PAN_MAX_L))
		{
			pan[i] = (data[i+1] & PARITY_BIT_MASK_ODD); //le saco el bit de paridad
			i++;
		}
		//end = LOW;
		return &pan[0];
	//}
	//else
	//{
	//	return NULL;
	//}
}

void clear_PAN (void)
{
	for (int i = 0; i < PAN_MAX_L ; i++)
		{
			pan[i] = 0;
		}
}


void lectorInit (void (*funcallback)(void))
{
	 gpioMode(PIN_LED_BLUE, OUTPUT);

	 gpioWrite(PIN_LED_BLUE, HIGH);

	 gpioMode(EN, INPUT);
	 gpioMode(DATA, INPUT);
	 gpioMode(CLK, INPUT);

	 gpioIRQ(EN, GPIO_IRQ_MODE_BOTH_EDGES, isr_enable);
	 gpioIRQ(CLK, GPIO_IRQ_MODE_FALLING_EDGE, isr_clk);

	 NVIC_EnableIRQ(PORTD_IRQn);

	 callback = funcallback;
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
						if(Check_LRC()) //agregue un if aca
						{
							end = HIGH;
							callback();//end = HIGH; puse callback aca
						}
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
		end = LOW;
		word = 0;
		clear_PAN();
	}

}

bool get_Enable(void){
	return chkEnable;
}

void clear_Chk(void){
	chkEnable = 0;
}
