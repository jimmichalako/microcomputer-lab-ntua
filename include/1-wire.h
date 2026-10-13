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