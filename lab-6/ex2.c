#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include "twi_interface.h"                      //TWI interface
#include "lcd_portd.h"                          //LCD functions using PORTD
#include "keyboard.h"                           //keypad functions

int main(void){
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set for EXT_PORT1 P4-P7 as input and P0 as output
    DDRD=0xFF;                                  //PORTD as output 
    lcd_init();
    char read;
    uint16_t pressed_keys = 0;
    _delay_ms(100);
    while(1){
        lcd_clear_display();                          //Clear LCD display
        read = scan_keypad_rising_edge(&pressed_keys);//Get the pressed key
        lcd_data(read);                               //Display ASCII character on LCD
        _delay_ms(500);
        // Wait until a key is pressed before resuming the next iteration
        while (scan_keypad_rising_edge(&pressed_keys) == ' ');
    }
 }