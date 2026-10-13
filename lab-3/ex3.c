#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include <stdio.h>

void mode1(); // DC changes with PD1 and PD2
void mode2(); // DC changes with POT1

uint8_t duty[]={5, 26, 46, 66, 87, 107, 127, 148, 168, 189, 209, 230, 250};

void mode1(){
    PORTD = 0x00;
    int i = (13*OCR1AL)/256;
    while(1){
        uint8_t input = PIND;
        input = input & 0b10000110;
        _delay_ms(100);
        if(input == 0b00000110)     //PD7 pressed
            mode2();                //switch to mode2
        if(input == 0b10000100){
            if(i < 12){             //check DC upper limit (98%)
                i++;                //increase DC by 8%
                OCR1AL = duty[i];
            }    
        }
        else if(input == 0b10000010){
            if(i > 0){              //check DC lower limit (2%)
                i--;                //decrease DC by 8%
                OCR1AL = duty[i];
            }
        }
    }
}

void mode2(){
    PORTD = 0x01;
    while(1){
        uint8_t input = PIND;
        input = input & 0b01000000;
        _delay_ms(100);
        if (input == 0b00000000)   //PD6 pressed
            mode1();                //switch to mode1
        ADCSRA |= (1<<ADSC);        //get new ADC value
        while(ADCSRA & (1<<ADSC));
        OCR1AL = ADCH;
    }
}

int main(){
    DDRD = 0b00000001;              //PORTD input
    DDRB = 0b00000010;              //PORTB output
    TCCR1A = (1<<COM1A1)|(1<<WGM10);// fast PWM, 8bit, non-inverted output
    TCCR1B = (1<<WGM12)|(1<<CS12);  // with frequency -> CLK/256=62500Hz
    ADMUX = 0b01100000;             // Vref->5V, Left adjusted, ADC0
    ADCSRA = 0b10000111;            // ADC Enable, fADC=fCLK/128=125kHz
    OCR1AL = duty[6];
    
    while(1){
        uint8_t input = PIND;
        _delay_ms(100);
        input = input & 0b11000000; //only keep PD6-PD7 
        if(input == 0b10000000)     //PD6 pressed
            mode1();
        if(input == 0b01000000)     //PD7 pressed
            mode2();
    }
}