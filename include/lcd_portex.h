void write_2_nibbles(uint8_t a){
    uint8_t temp= a;
    PCA9555_0_write(REG_OUTPUT_0, (PCA9555_0_read(REG_OUTPUT_0) & 0x0F) | (temp & 0xF0));
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) | 0x08); // Set E high
    asm("nop");
    asm("nop");
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) & ~0x08); // Set E low
    a=(a<<4)|(a>>4);
    PCA9555_0_write(REG_OUTPUT_0, (PCA9555_0_read(REG_OUTPUT_0) & 0x0F) | (a & 0xF0)); // Set the low nibble
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) | 0x08); // Set E high
    asm("nop");
    asm("nop");
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) & ~0x08); // Set E low
    }

void lcd_data(unsigned char data){
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) | 0x04); // LCD_RS = 1
    write_2_nibbles(data);
    _delay_us(250);
}

void lcd_command(uint8_t com){
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) & ~0x04); // LCD_RS = 0
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
        PCA9555_0_write(REG_OUTPUT_0, 0x30); // Set data to 0x30
        PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) | 0x08); // E high
        asm("nop");
        asm("nop");
        PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) & ~0x08); // E low
        _delay_us(250);
        ++i;
    }
    PCA9555_0_write(REG_OUTPUT_0, 0x20); // Set data to 0x20 to switch to 4-bit mode
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) | 0x08); // E high
    _delay_us(2);
    PCA9555_0_write(REG_OUTPUT_0, PCA9555_0_read(REG_OUTPUT_0) & ~0x08); // E low
    _delay_us(250);
    lcd_command(0x28); //5*8 dots, 2 lines
    lcd_command(0x0c); //display on, cursor off
    lcd_clear_display();
}

void lcd_string(char *str){
    int i;
    for(i=0; str[i]!='\0'; i++) lcd_data(str[i]);
}