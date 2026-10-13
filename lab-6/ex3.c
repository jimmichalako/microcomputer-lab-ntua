#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include "twi_interface.h"                      //TWI interface
#include "keyboard.h"                           //keypad functions

// Function to handle the case where the password is incorrect
void wrong_pw(){
    for(int i = 0; i < 5; i++){ // Repeat the sequence 5 times
        PORTB = 0xFF;        // Turn on all LEDs on PORTB
        _delay_ms(500);      // Keep LEDs on for 500ms
        PORTB = 0x00;        // Turn off all LEDs on PORTB
        _delay_ms(500);      // Keep LEDs off for 500ms
    }
}

int main(void){
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1 P4-P7 as input and P0-P3 as output
    DDRB=0xFF;                                  //PORTB as output
    char read;
    uint16_t pressed_keys = 0;
    while(1){
        PORTB = 0x00; // Turn off all LEDs initially

        // Wait for a key press
        read = scan_keypad_rising_edge(&pressed_keys);
        while(read == ' ') // If no key is pressed, keep scanning
            read = scan_keypad_rising_edge(&pressed_keys);

        // Check if the first key is '4' (first digit of the group number)
        if (read == '4'){
            // Wait until the key is released
            while(read == '4')
                read = scan_keypad_rising_edge(&pressed_keys);

            // Wait for the second key press
            while(read == ' ')
                read = scan_keypad_rising_edge(&pressed_keys);

            // Check if the second key is '2' (second digit of the group number)
            if(read == '2'){
                PORTB = 0xFF;    // Turn on all LEDs on PORTB
                _delay_ms(3000); // Keep LEDs on for 3 seconds
            }
            else {
                wrong_pw(); // If the second digit is incorrect, execute the wrong password sequence
            }
        }
        else {
            // Handle the case where the first key is not '4'
            char wrong = read; // Store the wrong input to handle debounce
            while(read == wrong) // Wait until the wrong key is released
                read = scan_keypad_rising_edge(&pressed_keys);

            // Wait for the keypad to be idle before proceeding
            while(read == ' ')
                read = scan_keypad_rising_edge(&pressed_keys);

            wrong_pw(); // Execute the wrong password sequence
        }
    }
}