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
	user_t admin = { {4, 5, 1, 7, 6, 6, 0, 1}, {1, 1, 1, 1, 1},  true, false, 0};
	users[0] = admin;
	index_++;
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
		if(users[i].blocked == false) // usuario no bloqueado
		{
			if (memcmp(users[i].id, id, ID_L) == 0) // si son iguales
			{
				return i;
			}
		}
		i++;
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
