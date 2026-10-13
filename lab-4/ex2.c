#define F_CPU 16000000UL
#include "avr/io.h"
#include <util/delay.h>
#include <stdio.h>

void write_2_nibbles(uint8_t a){
    uint8_t temp= a;
    PORTD = (PIND & 0x0f) + (temp & 0xf0); //LCD Data High Bytes
    PORTD|=0x08;
    asm("nop");
    asm("nop");
    PORTD&=~(0x08);
    a=(a<<4)|(a>>4);
    PORTD = (PIND & 0x0f) + (a & 0xf0); //LCD Data Low Bytes
    PORTD|=0x08;
    asm("nop");
    asm("nop");
    PORTD&=~(0x08);
    }

void lcd_data(unsigned char data){
    PORTD|=4; //LCD_RS=1 => Data
    write_2_nibbles(data);
    _delay_us(250);
    }

void lcd_command(uint8_t com){
    PORTD&=~4; //LCD_RS=0 => Instruction
    write_2_nibbles(com);
    _delay_us(250);
    }

void lcd_clear_display(){
    lcd_command(0x01);
    _delay_ms(5);
    }

void lcd_init(){
    _delay_ms(200);
    int i=0;
    while(i<3){ //command to switch to 8 bit mode
        PORTD=0x30;
        PORTD|=0x08;
        asm("nop");
        asm("nop");
        PORTD&=~0x08;
        _delay_us(250);
        ++i;
    }
    PORTD=0x20; //command to switch to 4 bit mode
    PORTD|=0x08;
    _delay_us(2);
    PORTD&=~0x08;
    _delay_us(250);
    lcd_command(0x28); //5*8 dots, 2 lines
    lcd_command(0x0c); //display on, cursor off
    lcd_clear_display();
    }

void output(int vin){
    int num[3];
    num[2]=vin%10;
    vin/=10;
    num[1]=vin%10;
    vin/=10;
    num[0]=vin%10;
    lcd_data(num[0] + 0x30);
    lcd_data('.');
    lcd_data(num[1] + 0x30);
    lcd_data(num[2] + 0x30);
    lcd_data('V');
    }

int main(){
    uint32_t temp;
    int value;
    DDRC |= 0x00; //PORTC input
    DDRD |= 0xFF; //PORTD output
    ADMUX |= 0b01000001; //Vref=5V, Right adjusted, ADC1
    ADCSRA |= 0b10000111; //ADEN=1, fADC=fCLK/128
    lcd_init();
    while (1){
        lcd_clear_display();
        ADCSRA|= (1<<ADSC); //new ADC value
        while((ADCSRA & 0x40)==0x40){} //Wait until ADC is finished
        temp=ADC;
        value=(temp*500)>>10;
        output(value);
        _delay_ms(1000);
    }
}