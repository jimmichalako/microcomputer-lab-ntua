void usart_init(unsigned int ubrr){
    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0C = (3 << UCSZ00);
    return;
}

void usart_transmit(uint8_t data){
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

uint8_t usart_receive(){
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void usart_transmit_string(const char *data){
    for (int i = 0; i < strlen(data); i++)
        usart_transmit(data[i]);
}

void usart_receive_string(char *data) {
    int i = 0;
    char received;
    while ((received = usart_receive()) != '\n')
        data[i++] = received;
    data[i] = '\0';
}