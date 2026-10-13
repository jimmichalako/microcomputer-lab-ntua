#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

int c=0;

ISR(INT1_vect) // External INT1 ISR
{
    if(c>0){
        PORTB=0x3F;     //light PB5-PB0 on re-press
        _delay_ms(500); //0.5s delay on re-press
        c=4500;         //4.5 remaining delay for PB0
    }
    else{
        _delay_ms(20);  //5s delay for PB0
        c=4980;
        }
    EIFR = (1 << INTF1); // Clear the flag of interrupt INTF1
}
int main(){
    // Interrupt on rising edge of INT1 pin
    EICRA=(1<<ISC11) | (1<<ISC10);

    // Enable the INT1 interrupt (PD3)
    EIMSK=(1<<INT1);

    sei(); // Enable global interrupts

    DDRB=0xFF; // Set PORTB as output

    while(1)
     {
        PORTB=0x00;
        while (c>0){
            PORTB=0x01;
            _delay_ms(1);
            c=c-1;
        }
     }
}