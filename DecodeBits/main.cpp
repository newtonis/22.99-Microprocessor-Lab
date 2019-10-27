#include <iostream>

using namespace std;

enum{NOT_TAKEN_SYMBOLS, FIRST_DC_TAKEN, SECOND_DC_TAKEN};

bool isNumberN(uint8_t N, uint8_t n, uint8_t threshold){
    if( (N-threshold)<=n && n<=(N+threshold) ){
        return true;
    }else{
        return false;
    }
}
bool isZero(uint8_t n, uint8_t threshold){
    if(n<=threshold){
        return true;
    }else{
        return false;
    }
}


bool isValidA0(uint8_t input,uint8_t thresh_hold){
    if(isNumberN(100,input,thresh_hold)){
        return true;
    }else if(isNumberN(50,input,thresh_hold)) {
        return true;
    }else{
        return false;
    }
}

bool isValidA1(uint8_t a0,uint8_t input,uint8_t thresh_hold){
    if( isNumberN(100,a0,thresh_hold) && isZero(input,thresh_hold)){
        return true;
    }else if( isNumberN(50,a0,thresh_hold) && isNumberN(50,input,thresh_hold)) {
        return true;
    }else{
        return false;
    }
}


static uint8_t buffer[8] = {255,255,255,255,255,255,255,255};
static uint8_t index = 0;
static bool is_buff_full = false;

static uint8_t  status = NOT_TAKEN_SYMBOLS;
static uint8_t a0;
static uint8_t a1;

void bufferFlush(void){
    for(int i = 0; i < sizeof(buffer) ; i++) {
        buffer[i] = 0;
    }
    is_buff_full = false;
}
void buffer2UART(void){ // esto debería ser un callback
    return;
}



void writeBuffer(uint8_t a0, uint8_t a1, uint8_t threshold){
    if( isNumberN(100,a0,threshold) &&  isZero(a1,threshold)){
        buffer[index] = 1;
    }else if( isNumberN(50,a0,threshold) && isNumberN(50,a1,threshold) ){
        buffer[index] = 0;
    }
    if(index == (sizeof(buffer)-1) ){
        is_buff_full = true;
    }
    index ++;
    index %= sizeof(buffer);
}

bool isBufferFull(void){
    return is_buff_full;
}

void write2BuffLogic(uint8_t a0, uint8_t a1, uint8_t threshold){
    if(!isBufferFull()){
        writeBuffer(a0,a1,threshold);
    }else{
        buffer2UART();
        bufferFlush();
        writeBuffer(a0,a1,threshold);
    }
}

int main() {

    uint8_t input;

    // la logica funciona mientras sean números validos de duty.
    // voy a pasar los datos "on the fly" (es decir, duty tras duty)

    uint8_t data[] ={55, 55, 100,0, 45, 52, 95, 5, 99, 0}; // esto sería 0 1 0 1 1

    // caso en el que viene mal el segundo y tiene que predecir en base a a0:
    //uint8_t data[] ={55, 33, 100,0, 45, 52, 95, 5, 99, 0}; // esto sería 0 1 0 1 1

    // caso en el que empiezo por el segundo (a1):
    //uint8_t data[] ={55, 100, 0, 55, 45, 100, 0, 50, 50}; // esto sería 0 1 0 1 0
    // en este caso, me re sincronizo y empieza a funcionar normal de nuevo desde 100,0


    bool message_received =  true;
    for(int  i =0 ; i< (sizeof(data)); i++){ // con esto simulo los datos on the fly!! (no son buffer, cuidado!)
        input = data[i];
        if (message_received){
            if(status==NOT_TAKEN_SYMBOLS){
                if(isValidA0(input,10)){
                    a0 = input;
                    status = FIRST_DC_TAKEN;
                }
            }else if(status == FIRST_DC_TAKEN){
                if(isValidA1(a0,input,10)){
                    a1 = input;
                    status = SECOND_DC_TAKEN;
                }else{
                    if(isNumberN(100,input,10)) { // si era valido a0 pero es el seugndo caracter
                        if(isNumberN(50,a0, 10)){ // si de 50 pasa a 100, entonces antes era 50,50 y ahora agarro 100
                            write2BuffLogic(50,50,10);
                        }
                        a0 = 100;
                        status = FIRST_DC_TAKEN;
                    }else if(isNumberN(50, input, 10)){ // si de 0 pasa a 50
                        if(isZero(a0,10)){              // asumo que nates hubo un 100, 0 y ahora viene
                            write2BuffLogic(100,0,10);  //
                        }
                        a0 = 50;
                        status = FIRST_DC_TAKEN;
                    }else{
                        // si fue totalmente invalido el input predigo qué va a venir
                        if(isNumberN(100,a0,10)){
                            a1= 0;
                        }else if(isNumberN(50,a0,10)){
                            a1= 50;
                        }
                        status = SECOND_DC_TAKEN;
                    }
                }
            }
            if(status == SECOND_DC_TAKEN){
                write2BuffLogic(a0,a1,10);
                status = NOT_TAKEN_SYMBOLS;
            }
        }
    }
    cout << "hello" << endl;

    return 0;
}