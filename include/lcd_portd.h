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