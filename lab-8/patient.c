#define F_CPU 16000000UL // Define CPU frequency for delay functions
#include<avr/io.h>       // Include header for input-output operations
#include<util/delay.h>   // Include header for delay functions
#include <stdio.h>       // Include standard I/O library for sprintf
#include <stdlib.h>      // Include standard library for general utilities
#include <string.h>      // Include string library for string manipulations
#include "twi_interface.h" // Include header for TWI/I2C interface
#include "lcd_portex.h"    // Include header for LCD operations
#include "keyboard.h"      // Include header for keypad interface
#include "1-wire.h"        // Include header for 1-wire communication
#include "usart.h"         // Include header for USART communication

int main(void){
    // Initialize I2C, LCD, USART and ADC
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // Set Port 0 as output
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); // Set Port 1 upper nibble as input
    lcd_init();
    usart_init(103); // Initialize USART with baud rate 9600
    ADMUX = 0b01000000; // Set ADC reference to AVcc, select ADC0, right-adjusted
    ADCSRA = 0b10000111; // Enable ADC, set prescaler to 128 for 125 kHz ADC clock

    // Variables for data storage
    char temperature[10]; // Store temperature string
    char pressure[10];    // Store pressure string
    char status[20];      // Store status string
    char payload[200];    // Store payload string for transmission
    char buffer[20];      // Temporary buffer for USART data

    lcd_clear_display(); // Clear the LCD display

    // Connect ESP to WiFi
    usart_transmit_string("ESP:connect\n"); // Send connect command to ESP
    char a = usart_receive(); // Receive acknowledgment
    if (a == 'S') // Check if the connection was successful
        lcd_string("1.Success");
    else
        lcd_string("1.Fail");
    _delay_ms(1000); // Delay to display the result
    lcd_clear_display();
    while (a != '\n') // Clear USART buffer
        a = usart_receive();

    // Set URL for the ESP
    usart_transmit_string("ESP:url:\"http://192.168.1.250:5000/data\"\n"); // Set server URL
    a = usart_receive(); // Receive acknowledgment
    if (a == 'S') // Check if URL setup was successful
        lcd_string("2.Success");
    else
        lcd_string("2.Fail");
    _delay_ms(1000);
    while (a != '\n') // Clear USART buffer
        a = usart_receive();

    while (1){ // Main loop
        lcd_clear_display(); // Clear LCD before updating
        
        // Variables for sensor readings
        uint16_t result = 0; // Raw temperature data
        float temp = 0;      // Converted temperature

        // Read temperature using 1-wire sensor
        if (one_wire_reset() == 0x0001){ // Check if sensor is available
            one_wire_transmit_byte(0xCC); // Skip ROM command
            one_wire_transmit_byte(0x44); // Start temperature conversion
            while (one_wire_receive_bit() == 0); // Wait for conversion to complete
        }
        if (one_wire_reset() == 0x0001){ // Check sensor again
            one_wire_transmit_byte(0xCC); // Skip ROM command
            one_wire_transmit_byte(0xBE); // Read scratchpad
            result = one_wire_receive_byte(); // Read temperature data
            
            if (result > 0x0800){ // Check for negative temperature
                result = ~result; // Convert to positive
                result += 1;
            }

            temp = (float)result / 16.0; // Convert to Celsius
            temp += 12;                 // Apply calibration offset
            temp = ((int)(temp * 1000 + 0.5)) / 1000.0; // Round to 3 decimal places
            sprintf(temperature, "%.3f", temp); // Format temperature as string
        }

        // Read pressure using ADC
        ADCSRA |= (1 << ADSC); // Start ADC conversion
        while (ADCSRA & (1 << ADSC)); // Wait for ADC conversion to complete
        float piesi = (float)ADC * 20 / 1023; // Convert ADC value to pressure
        piesi = ((int)(piesi * 100 + 0.5)) / 100.0; // Round to 2 decimal places
        sprintf(pressure, "%.2f", piesi); // Format pressure as string

        // Keypad input handling for status
        uint16_t pressed_keys = 0; // Track pressed keys
        for (int i = 0; i < 100; i++){ // Scan keypad for input
            char read = scan_keypad_rising_edge(&pressed_keys);
            if (read == '2'){ // Nurse call
                sprintf(status, "NURSE CALL");
                break;
            }
            else if (read == '#'){ // OK signal
                sprintf(status, "OK");
                break;
            }
            _delay_ms(1); // Short delay for debounce
        }
        
        // Determine status based on readings
        if(!strcmp(status, "NURSE CALL"))
            ; // Maintain "NURSE CALL" status
        else if (piesi > 12 || piesi < 4) // Check pressure thresholds
            sprintf(status, "CHECK PRESSURE");
        else if (temp > 37 || temp < 34) // Check temperature thresholds
            sprintf(status, "CHECK TEMP");
        else
            sprintf(status, "OK");

        // Create payload for transmission
        sprintf(payload, "ESP:payload:[{\"name\": \"temperature\",\"value\": \"");
        strcat(payload, temperature);
        strcat(payload, "\"},");
        strcat(payload, "{\"name\": \"pressure\",\"value\": \"");
        strcat(payload, pressure);
        strcat(payload, "\"},");
        strcat(payload, "{\"name\": \"team\",\"value\": \"42\"},");
        strcat(payload, "{\"name\": \"status\",\"value\": \"");
        strcat(payload, status);
        strcat(payload, "\"}]\n");

        // Update LCD display with readings and status
        lcd_string("T:");
        lcd_string(temperature);
        lcd_string(" P:");
        lcd_string(pressure);
        lcd_command(0xC0); // Move to second line of LCD
        lcd_string(status);
        _delay_ms(1000);

        lcd_clear_display();

        // Transmit payload to ESP
        usart_transmit_string(payload);

        // Handle transmission acknowledgment
        a = usart_receive();
        a = usart_receive();
        if (a == 'S') // Check if transmission was successful
            lcd_string("3.Success");
        else
            lcd_string("3.Fail");
        _delay_ms(1000);
        lcd_clear_display();
        while (a != '\n') // Clear USART buffer
            a = usart_receive();
        
        usart_transmit_string("ESP:transmit\n"); // Transmit data command
        usart_receive_string(buffer); // Receive response from ESP
        lcd_string(buffer); // Display response on LCD
        _delay_ms(1000);
    }
}
