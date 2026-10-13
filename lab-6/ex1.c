#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "twi_interface.h"                      //TWI interface
#include "keyboard.h"                           //keypad functions

int main(void){
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1 P4-P7 as input and P0-P3 as output
    DDRB=0xFF;                                  //PORTB as output
    char read;
    uint16_t pressed_keys = 0;
    while(1){
        read = scan_keypad_rising_edge(&pressed_keys);
        if(read=='A')                           //If 'A' is pressed
           PORTB=0x01;                          //PB0->on
        else if(read=='8')                      //If '8' is pressed
           PORTB=0x02;                          //PB1->on
        else if(read=='6')                      //If '6' is pressed
           PORTB=0x04;                          //PB2->on
        else if(read=='*')                      //If '*' is pressed
           PORTB=0x08;                          //PB3->on
        else                                    //Else
           PORTB=0x00;                          //PORTB->off
    }
}