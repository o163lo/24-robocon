#include "mbed.h"
#include <cmath>

DigitalOut Dig[2] = {PC_0, PB_10};
PwmOut P[2] = {PA_11, PB_6};
PwmOut servo[5] = {PC_7, PB_4, PC_8, PC_9, PB_7}; //[0][1]→オーブ [2][3][4]→キー
DigitalOut limit[4] = {PB_5, PB_8, PC_1, PC_5};


RawSerial pc(USBTX,USBRX,9600);
RawSerial cont(PA_9, PA_10, 2400);

int PS3[7];
double target[2], current[2], acceleration = 0.3;

void Config(void);
void PS3Data(void);
void Orb(void);
void Key(void);


int main(void){
    Config();
    int i;
    for(i=0;i<5;i++){
        servo[i].period(0.02); //period 周期
    }
    while(true){
        pc.printf("\033[H"); //画面クリア
        // for (i = 0; i < 7; i++){
        //     pc.printf("%02X, ", PS3[i]);
        // }
        Orb();
        Key();
    }
}

void Orb(void){
    // if(((PS3[1]>>1) % 0b10) == 1){ //L1
    //     servo[1].pulsewidth(0.00160);
    //     // wait_ms(1000);
    //     // servo[0].pulsewidth(0.00145);
    // }else{
    //     servo[1].pulsewidth(0.0024);
    // }
    
    //L2押したらオーブの2つのサーボが同時に動いて1個ずつオーブが落ちる
    if((PS3[1]>>2)%0b10 == 1){
        servo[0].pulsewidth(0.00145);
        servo[1].pulsewidth(0.0024);
        // wait_ms(1000);
        // servo[1].pulsewidth(0.00145);
    }else{
        servo[0].pulsewidth(0.0024);
        servo[1].pulsewidth(0.00185);
    }
}

void Key(void){
    // while((PS3[2]>>6) % 0b10 != 1){
    //     servo[2].pulsewidth(0.00145);
    // }
    int i;
    //〇 キーの機構が展開
    if((PS3[2]>>6) % 0b10 == 1){
        servo[2].pulsewidth(0.002);
    }else{
        servo[2].pulsewidth(0.00145);
    }
    //キーの機構上下左右調節
    if((PS3[2] % 0b100 == 1)){ //↑
        target[0] = -0.5;
    }
    else if(PS3[2] % 0b100 == 0b10){ //↓
        target[0] = 0.5;
    }else{
        target[0] = 0;
    }
    if((PS3[2]>>3) % 0b10 == 1){ //←
        target[1] = -0.5;
    }
    else if((PS3[2]>>2) % 0b10 == 1){ //→
        target[1] = 0.5;
    }else{
        target[1] = 0;
    }
    for(i=0;i<2;i++){
        current[i] += (target[i] - current[i]) * acceleration;
        P[i] = fabs(current[i]);
        if(current[i] > 0){
            Dig[i] = 0;
        }else{
            Dig[i] = 1;
        }
    }
    //キーのサーボ開く
    if((PS3[1]>>3) % 0b10 == 1){ //R1
        servo[3].pulsewidth(0.0022);
    }else{
        servo[3].pulsewidth(0.00145);
    }
    //ゴールゾーンのオーブのサーボ開閉
    if((PS3[1]>>4)%0b10 == 1){ //R2
        servo[4].pulsewidth(0.0020);
    }else{
        servo[4].pulsewidth(0.00140);
    }
}

void Config(void){
    int i;
    cont.attach(&PS3Data, Serial::RxIrq);
    while(PS3[2] != 3){ //start押されるまでwhile繰り返し
        for (i = 0; i < 7; i++){
            pc.printf("%d, ", PS3[i]);
        }
        pc.printf("\r\n");
    }
}

void PS3Data(void){
    //受信割り込みのときに来るよ
    int SBDBT_Data = cont.getc();
    static int bits = 0;
    
    if(SBDBT_Data == 128){
        bits = 0;
    }

    if (SBDBT_Data >= 0){
        PS3[bits] = SBDBT_Data;
        
        if (bits == 7){
            bits = 0;
        }
        else {
            bits++;
        }
    }
}
