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


/**
 * @brief initialize users list, adding an admin user.
 */
void initUser(void);

/**
 * @brief adds a new user to the list.
 * @param new user id and pin
 * @return false if the new users id already exists or if my list is full, true otherwise.
 */
bool addUser(int * id, int * pin);

/**
 * @brief checks if the user is on the list.
 * @param user's id and pin to validate
 * @return true if the user's id and pin are on the list.
 */
bool validateUser(int * id, int * pin);

/**
 * @brief checks if the user's id is on the list.
 * @param user's id to validate
 * @return true if the user's id is on the list.
 */
bool validateID(int * id);

/**
 * @brief blocks user
 * @param user's id that has to be blocked
 * @return true if the user's id exists on the list, so user could be blocked
 */
bool blockUser(int * id);

/**
 * @brief unblocks user
 * @param user's id that has to be unblocked
 * @return true if the user's id exists on the list so user could be unblocked
 */
bool unblockUser (int * id);

/**
 * @brief changes user's pin
 * @param user's id to change pin and the new pin.
 * @return HIGH or LOW
 */
bool changePin(int * id, int * newPin);


#endif /* USERS_H_ */
