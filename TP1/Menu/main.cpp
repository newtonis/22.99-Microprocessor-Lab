#include <iostream>


enum {MENU_TARJETA, MENU_PIN, CHECK_PW};
int main() {
    std::cout << "Hello, World!" << std::endl;

    uint32_t input = 8;


    // primero leo tarjeta, luego pido PIN
    // luego chequeo password, sino vuelvo a pedir tarjeta
    int state = MENU_TARJETA;
    switch(state){
        case MENU_TARJETA: //se ingresan 4 caracteres
        if (interrupt_flag_pin124 == 1){

        }
        state = MENU_PIN;
        break;
        case MENU_PIN:

        break;
        //case MENU_PIN: // se ingresan 8 caracteres
        //break;
    }

    return 0;
}