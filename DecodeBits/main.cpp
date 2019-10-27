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


int main() {
    uint8_t  status = NOT_TAKEN_SYMBOLS;
    uint8_t a0;
    uint8_t a1;
    uint8_t input;
    //uint8_t data[] ={55, 55, 100,0, 45, 52, 95, 5, 99, 0}; // esto sería 1 0 0 1 1
    // si hay uno malo, no cmabia de estado hasta que haya uno bueno

    uint8_t data[] ={55, 33, 100,0, 45, 52, 95, 5, 99, 0}; // esto sería 1 0 0 1 1

    // con el 33 entonces seria:
    //  0 solamente (o sea una cagada)

    // arreglado seria:
    // 1 0 1 1

    // pero estaria perdiendo el bit 55 33...

    bool message_received =  true;
    for(int  i =0 ; i< (sizeof(data)); i++){
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
                    status = NOT_TAKEN_SYMBOLS;
                    //aca deberia escribri la respeusta al conflcito
                }
            }
            if(status == SECOND_DC_TAKEN){
                if(!isBufferFull()){
                    writeBuffer(a0,a1,10);
                }else{
                    buffer2UART();
                    bufferFlush();
                    writeBuffer(a0,a1,10);
                }
                status = NOT_TAKEN_SYMBOLS;
            }
        }
    }
    cout << "hello" << endl;

    return 0;
}