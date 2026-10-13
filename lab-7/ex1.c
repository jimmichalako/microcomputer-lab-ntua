#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include <stdio.h>
#include <string.h>
#include "twi_interface.h" //TWI interface
#include "lcd_portex.h"    //LCD functions using PORT Expander

// Function to reset the 1-Wire bus and detect the presence of devices
uint16_t one_wire_reset(){
    DDRD |= 0b00010000;    // Set PD4 as output
    PORTD &= 0b11101111;   // Pull PD4 low to send reset signal
    _delay_us(480);        // Hold the line low for 480 µs
    DDRD &= 0b11101111;    // Set PD4 as input (release the line)
    PORTD &= 0b11101111;   // Keep pull-up resistor disabled
    _delay_us(100);        // Wait for presence pulse from devices
    uint8_t temp = PIND;   // Read the pin state
    _delay_us(380);        // Wait for the rest of the timing window
    temp &= 0b00010000;    // Isolate the bit corresponding to PD4
    if (temp == 0b00000000)
        return 0x0001;     // Device present
    else
        return 0x8000;     // No device present
}

// Function to receive a single bit from the 1-Wire bus
uint8_t one_wire_receive_bit(){
    DDRD |= 0b00010000;    // Set PD4 as output
    PORTD &= 0b11101111;   // Pull PD4 low to signal bit read
    _delay_us(2);          // Hold line low for 2 µs
    DDRD &= 0b11101111;    // Set PD4 as input (release the line)
    PORTD &= 0b11101111;   // Keep pull-up resistor disabled
    _delay_us(10);         // Wait for the bit value to stabilize
    uint8_t temp = PIND;   // Read the pin state
    temp &= 0b00010000;    // Isolate the bit corresponding to PD4
    if (temp == 0b00010000)
        temp = 0x01;       // Bit value is 1
    else
        temp = 0x00;       // Bit value is 0
    _delay_us(49);         // Wait for the rest of the bit time
    return temp;           // Return the received bit
}

// Function to transmit a single bit to the 1-Wire bus
void one_wire_transmit_bit(uint8_t data){
    DDRD |= 0b00010000;    // Set PD4 as output
    PORTD &= 0b11101111;   // Pull PD4 low to signal bit write
    _delay_us(2);          // Hold line low for 2 µs
    data &= 0x0001;        // Isolate the least significant bit
    data <<= 4;            // Align bit to PD4 position
    PORTD = data;          // Set the bit value on the bus
    _delay_us(58);         // Hold the line for the rest of the bit time
    DDRD &= 0b11101111;    // Set PD4 as input (release the line)
    PORTD &= 0b11101111;   // Keep pull-up resistor disabled
    _delay_us(1);          // Short delay before next operation
}

// Function to receive a 16-bit value from the 1-Wire bus
uint16_t one_wire_receive_byte(){
    uint16_t temp, result = 0x0000; // Initialize result to 0
    for (int i = 0; i < 16; i++) {  // Loop for 16 bits
        temp = one_wire_receive_bit(); // Read a single bit
        result = result >> 1;         // Shift result to the right
        if (temp == 1)
            result |= 0x8000;         // Set the most significant bit if received bit is 1
    }
    return result;                    // Return the received 16-bit value
}

// Function to transmit an 8-bit value to the 1-Wire bus
void one_wire_transmit_byte(uint8_t data){
    for (int i = 0; i < 8; i++) { // Loop for 8 bits
        if ((data | 0b11111110) == 0b11111111)
            one_wire_transmit_bit(0x01); // Transmit 1 if LSB is 1
        else
            one_wire_transmit_bit(0x00); // Transmit 0 if LSB is 0
        data >>= 1;                     // Shift data to the right
    }
}

// Main function to interface with the temperature sensor and display results
int main(void) {
    twi_init(); // Initialize TWI (I2C) communication
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // Configure PCA9555 GPIO expander
    lcd_init(); // Initialize the LCD
    uint16_t init1, init2, result; // Variables for 1-Wire communication and temperature data

    while (1) {
        lcd_clear_display(); // Clear LCD display

        init1 = one_wire_reset(); // Send reset signal to 1-Wire bus
        if (init1 == 0x0001) {    // If a device is detected
            one_wire_transmit_byte(0xCC); // Skip ROM command
            one_wire_transmit_byte(0x44); // Start temperature conversion
            while (one_wire_receive_bit() == 0); // Wait for conversion to complete
        }

        init2 = one_wire_reset(); // Reset the bus again
        if (init2 == 0x0001) {    // If a device is detected
            one_wire_transmit_byte(0xCC); // Skip ROM command
            one_wire_transmit_byte(0xBE); // Read scratchpad command
            result = one_wire_receive_byte(); // Read temperature data

            // Process temperature data (two's complement for negatives)
            if (result > 0x0800) {
                result = ~result;
                result += 1;
            }

            // Extract fractional and integer parts of temperature
            float deci1 = (float)(result & 0x0F) / 16.0; // Fractional part
            int deci2 = (int)(deci1 * 1000);             // Convert fractional part to integer
            while (deci2 % 10 == 0 && deci2 != 0) {
                deci2 /= 10; // Remove trailing zeros
            }
            result >>= 4; // Integer part of the temperature

            // Prepare strings for LCD display
            char integer[5], decimal[5], data[10];
            sprintf(integer, "%d", result); // Convert integer part to string
            sprintf(decimal, "%d", deci2);  // Convert fractional part to string
            strcpy(data, integer);          // Combine integer part
            strcat(data, ".");              // Add decimal point
            strcat(data, decimal);          // Add fractional part

            // Display temperature on LCD
            if (result > 0x0800)
                lcd_data((char)45); // Display "-" for negative temperatures
            else
                lcd_data((char)43); // Display "+" for positive temperatures
            lcd_string(data);       // Display temperature value
            lcd_data((char)223);    // Display degree symbol "°"
            lcd_data('C');          // Display "C" for Celsius
            _delay_ms(1000);        // Wait 1 second
        } else if (init2 == 0x8000) { // If no device is detected
            char message[] = "No Device";
            lcd_string(message); // Display error message
            _delay_ms(1000);     // Wait 1 second
        }
    }
}