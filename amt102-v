#include "mbed.h"
#include "RotaryEncoder.h"

DigitalOut Dig(D4);
PwmOut P(D5);
RotaryEncoder R(D3,D2);
RawSerial PC(USBTX,USBRX,115200);

int main(){
    while(1){
        Dig=0;
        P=1;
        double current = R.Get_Count()*0.17578125;
        PC.printf("%f",current);
        PC.printf("\r\n");

        if(360<current)
        R.Reset();
    }
    return 0;
}
