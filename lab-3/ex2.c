#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include <stdio.h>

void check(){
    uint32_t ADCvalue=0;
    
    for(int i=0; i<16; i++){
        ADCSRA |= (1<<ADSC);        //get new ADC value
        while(ADCSRA & (1<<ADSC));
        ADCvalue += ADC;
        _delay_ms(100);
    }
    ADCvalue >>= 4;
    
    if(ADCvalue>=0 && ADCvalue<=200)        //ADCvalue levels
        PORTD = 0b00000001;                 //PD0 on
    else if(ADCvalue>200 && ADCvalue<=400)
        PORTD = 0b00000010;                 //PD1 on
    else if(ADCvalue>400 && ADCvalue<=600)
        PORTD = 0b00000100;                 //PD2 on
    else if(ADCvalue>600 && ADCvalue<=800)
        PORTD = 0b00001000;                 //PD3 on
    else
        PORTD = 0b00010000;                 //PD4 on
    return;
}

int main() {
    DDRC = 0b00000000;
    DDRB = 0b00000010;
    DDRD = 0b00111111;
    
    TCCR1A = (1<<COM1A1)|(1<<WGM10);// fast PWM, 8bit, non-inverted output
    TCCR1B = (1<<WGM12)|(1<<CS12);  // with frequency -> CLK/256=62500Hz
    ADMUX |= 0b01000001;            // Vref->5V, Right adjusted, ADC1
    ADCSRA |= 0b10000111;           // ADC Enable, fADC=fCLK/128=125kHz
    
    uint16_t duty[]={5, 26, 46, 66, 87, 107, 127, 148, 168, 189, 209, 230, 250};
    int i = 6;
    OCR1AL = duty[i];
    while(1){
        uint8_t input = PIND;       //check input
        input = input & 0b11000000;
        _delay_ms(100);             //debouncing

        if(input == 0b01000000){
            if(i < 12){             //check DC upper limit (98%)
                i++;                //increase DC by 8%
                OCR1AL = duty[i];
            }
            check();
        }
        else if(input == 0b10000000){
            if(i > 0){              //check DC lower limit (2%)
                i--;                //decrease DC by 8%
                OCR1AL = duty[i];
            }
            check();
        }
    }
}