#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

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

uint16_t gas = 0;

ISR(ADC_vect) { //ADC interrupt handler
    gas = ADC;
}

int main() {
    DDRC |= 0x00; //PORTC input
    DDRB |= 0xFF; //PORTB output
    DDRD |= 0xFF; //PORTD output
    ADMUX |= 0b01000010; // Vref=5V, Right adjusted, ADC2
    ADCSRA |= 0b10001111; // ADEN=1, ADIE=1, fADC=fCLK/128
    sei();
    lcd_init();
    while (1) {
        ADCSRA |= (1 << ADSC); //new ADC value
        if (gas > 205) {
            lcd_clear_display();
            char output[] = "GAS DETECTED";
            for(int i=0; output[i]!=0; i++) 
                lcd_data(output[i]);
            PORTB = 0x3F;                   //PORTB on
            _delay_ms(50);                 
            PORTB = 0x00;                   //PORTB off
            _delay_ms(50);
        } else {
            lcd_clear_display();
            char output[] = "CLEAR";
            for(int i=0; output[i]!=0; i++) 
                lcd_data(output[i]);
            if (gas >= 0 && gas <= 41)
                PORTB = 0x01;               //PB0 on (Level 1)
            else if (gas > 41 && gas <= 82)
                PORTB = 0x03;               //PB0-PB1 on (Level 2)
            else if (gas > 82 && gas <= 123)
                PORTB = 0x07;               //PB0-PB1-PB2 on (Level 3)
            else if (gas > 123 && gas <= 164)
                PORTB = 0x0F;               //PB0-PB1-PB2-PB3 on (Level 4)
            else
                PORTB = 0x1F;               //PB0-PB1-PB2-PB3-PB4 on (Level 5)
            _delay_ms(100);
        }
    }
}