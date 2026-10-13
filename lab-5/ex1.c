#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "interface.h"

int main(){
    twi_init();
    DDRB = 0x00;                                //Set PORTB as input
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
    int input, output, A, comA, B, comB, C, D, comD, F0, F1;
    while(1){
        input=PINB;
        A=input&0x01;           //A=PORTB.0
        comA=~A;
        comA&=0x01;
        B=input&0x02;           //B=PORTB.1
        B>>=1;
        comB=~B;
        comB&=0x01;
        C=input&0x04;           //C=PORTB.2
        C>>=2;
        D=input&0x08;           //D=PORTB.3
        D>>=3;
        comD=~D;
        comD&=0x01;
        F0=(comA&B&C)|(comB&D); //(A'BC + B'D)
        F0=~F0;
        F0&=0x01;               //F0=(A'BC + B'D)' 
        F1=(A|B|C)&(B&comD);
        F1=F1<<1;
        output=F0|F1;
        PCA9555_0_write(REG_OUTPUT_0, output);
    }
}