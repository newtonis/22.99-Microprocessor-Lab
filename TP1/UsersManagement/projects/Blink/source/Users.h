/*
 * Users.h
 *
 *  Created on: Sep 6, 2019
 *      Author: martinamaspero
 */

#ifndef USERS_H_
#define USERS_H_

#include <stdint.h>
#include <stdbool.h>

#define ID_L 8
#define PIN_L 5
#define MAX_USERS 10
#define MAX_TRIES 3
#define ERROR -1

typedef struct {
    int	id[ID_L];
    int pin[PIN_L];
    bool admin;
    bool blocked; //true if it is blocked
    uint8_t tries;
} user_t;

void initUser(void);
bool addUser(int * id, int * pin);
bool validateUser(int * id, int * pin);
bool blockUser(int * id);
bool unblockUser (int * id);
bool changePin(int * id, int * newPin);


#endif /* USERS_H_ */
