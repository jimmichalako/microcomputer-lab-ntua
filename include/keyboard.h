
uint8_t scan_row(uint8_t row){
    row=~row;
    PCA9555_0_write(REG_OUTPUT_1, row);     //Set I0 row as 1 to enable pull up
    uint8_t read=PCA9555_0_read(REG_INPUT_1);
    read=~read;
    read=read>>4;
    PCA9555_0_write(REG_OUTPUT_1, 0xFF);
    return read;
}

uint16_t scan_keypad(){
    uint16_t tmp = 0x00;
    tmp = scan_row(8);
    tmp = tmp<<4;
    tmp |= scan_row(4);
    tmp = tmp<<4;
    tmp |= scan_row(2);
    tmp = tmp<<4;
    tmp |= scan_row(1);
    return tmp;
}

uint16_t scan_keypad_rising_edge(){
    uint16_t pressed_keys = 0, pressed_keys_tempo = 0;
    pressed_keys = scan_keypad();
    while(1){
        pressed_keys_tempo=scan_keypad();
        if(pressed_keys==pressed_keys_tempo)break;
       _delay_ms(20);
        pressed_keys= scan_keypad();
    }
    return pressed_keys;
}

char keypad_to_ascii(){
    uint16_t read = scan_keypad_rising_edge();
    char result;
    
    switch (read) {
        case 0x0100:
            result = '4';
            break;
        case 0x0200:
            result = '5';
            break;
        case 0x0400:
            result = '6';
            break;
        case 0x0800:
            result = 'B';
            break;
        case 0x1000:
            result = '1';
            break;
        case 0x2000:
            result = '2';
            break;
        case 0x4000:
            result = '3';
            break;
        case 0x8000:
            result = 'A';
            break;
        case 0x0010:
            result = '7';
            break;
        case 0x0020:
            result = '8';
            break;
        case 0x0040:
            result = '9';
            break;
        case 0x0080:
            result = 'C';
            break;
        case 0x0001:
            result = '*';
            break;
        case 0x0002:
            result = '0';
            break;
        case 0x0004:
            result = '#';
            break;
        case 0x0008:
            result = 'D';
            break;
        default:
            result =' ';  // Indicate an error if the value is not 1 or 2
    }
    return result;
}