/*
 * Users.c
 *
 *  Created on: Sep 6, 2019
 *      Author: martinamaspero
 */


#include "Users.h"
#include <string.h>

static int index_; //last user index
static user_t users [MAX_USERS];

static uint16_t counter1 = 0;
static uint16_t counter2 = 0;
static uint16_t counter3 = 0;


bool validatePIN(int * pin, int ind);
int validateIDint(int * id); // me devuelve el indice del ID en mi arreglo

bool unblockUser (int * id)
{
	int id_idx = validateIDint(id);
	if (id_idx != ERROR)
	{
		users[id_idx].blocked = false;
		return true;
	}
	return false;
}


void initUser(void)
{
	user_t user1 = { {4, 5, 1, 7, 6, 6, 0, 1}, {1, 2, 3, 4, 5}, 1 , true, false, false, 0};
	user_t user2 = { {6, 0, 1, 0, 5, 6, 2, 2}, {1, 2, 3, 4, 5}, 1 , false, false, false, 0};
	user_t user3 = { {3, 0, 8, 6, 2, 5, 0, 0}, {1, 2, 3, 4, 5}, 2 , false, false, false, 0};
	user_t user4 = { {6, 1, 0, 5, 3, 1, 6, 0}, {1, 2, 3, 4, 5}, 2 , false, false, false, 0};
	user_t user5 = { {6, 0, 1, 0, 5, 6, 8, 3}, {1, 2, 3, 4, 5}, 3 , false, false, false, 0};
	user_t user6 = { {6, 0, 6, 1, 2, 6, 8, 1}, {1, 2, 3, 4, 5}, 3 , false, false, false, 0};

	users[0] = user1;
	users[1] = user2;
	users[2] = user3;
	users[3] = user4;
	users[4] = user5;
	users[5] = user6;

	index_ = 6;
}

uint8_t getUsers1Floor (void)
{
	return counter1;
}

uint8_t getUsers2Floor (void)
{
	return counter2;
}

uint8_t getUsers3Floor (void)
{
	return counter3;
}

bool addUser(int * id, int * pin)
{

	if((index_ == MAX_USERS) && (validateID(pin) == true)) // si alcance mi numero maximo de usuarios o si el id ya existe
	{
		return false;
	}
	else
	{
		user_t new_user;

		memcpy(new_user.id, id, ID_L);
		memcpy(new_user.pin, pin, PIN_L);
		new_user.tries = 0;
		new_user.admin = false;
		new_user.blocked = false;

		users[index_] = new_user;
		index_++;
		return true;
	}
}

bool validateID(int * id)
{
	if(validateIDint(id)!= ERROR)
		return true;
	else
		return false;
}

int validateIDint(int * id)
{
	int i = 0;
	while (i < index_)
	{

		bool ans = true;
		if(users[i].blocked == false) // usuario no bloqueado
		{
			for (int j = 0; j<ID_L; j++)
			{
				if (users[i].id[j] != id[j]){
					ans = false;
				}
			}

		}
		if(ans == false)
		{
			i++;
		}
		else
		{
			return i;
		}
	}

	return ERROR; // no lo encuentra
}


bool validatePIN(int * pin, int ind)
{
	if(memcmp(users[ind].pin , pin, PIN_L) == 0) // SI SON IGUALES
	{
		return true;
	}

	return false;
}


bool validateUser(int * id, int * pin)
{
	int id_idx = validateIDint(id);
	if (id_idx != ERROR) // si el id exite
	{
		if(validatePIN(pin, id_idx))// si el pin esta bien
		{

			if(users[id_idx].inside == false)
			{
				switch(users[id_idx].floor)
				{
					case 1:
						counter1++;
						break;
					case 2:
						counter2++;
						break;
					case 3:
						counter3++;
						break;
				}

			}

			users[id_idx].inside = true;
			users[id_idx].tries = 0;

			return true;
		}
		else // si el pin esta mal
		{
			users[id_idx].tries++;
			if (users[id_idx].tries == MAX_TRIES) // si ya intento 3 veces lo bloqueo
			{
				users[id_idx].blocked = true;
			}
			return false;
		}
	}
	else // si el id no esta
	{
		return false;
	}
}



bool changePin(int * id, int * newPin)
{
	int id_idx = validateIDint(id);
	if (id_idx != ERROR)
	{
		memcpy(users[id_idx].pin, newPin, PIN_L);
		return true;
	}
	return false;
}

bool blockUser(int * id)
{
	int id_idx = validateIDint(id);
	if (id_idx != ERROR)
	{
		users[id_idx].blocked = true;
		return true;
	}
	return false;
}
