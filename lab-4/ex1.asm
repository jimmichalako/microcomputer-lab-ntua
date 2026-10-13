.include "m328PBdef.inc" 

.org 0x0
    rjmp reset
.org 0x2A
    rjmp isr

.equ PD2=2
.equ PD3=3

.def temp1=r16
.def temp2=r17
.def regL=r26
.def regH=r27
.def count=r20
reset:
    ldi r24, low(RAMEND)
    out SPL, r24
    ldi r24, high(RAMEND)
    out SPH, r24
    clr r24
    out DDRC, r24 ; PORTC input
    ser r24
    out DDRD, r24 ; PORTD output
    rcall lcd_init
    ldi r24, low(100)
    ldi r25, high(100)
    rcall wait_msec
    ldi r24, 0b01000001 ; Vref=5V->01, Right adjusted->0, ADC1->0001
    sts ADMUX, r24
    ldi r24, 0b10000111 ; ADEN->1, fADC=fCLK/128
    sts ADCSRA, r24

main:
    ldi r24, low(1000) ; wait 1s
    ldi r25, high(1000)
    rcall wait_msec
    rcall lcd_clear_display
    sei
    lds r24, ADCSRA
    ori r24, (1<<ADSC)|(1<<ADIE) ; new ADC conversion 
    sts ADCSRA, r24
    rjmp main

isr:
    ldi temp1, 5 ; Vref=5
    lds temp2, ADCL
    mul temp2, temp1
    mov regL, r0 ; ADCL*Vref
    mov regH, r1 
    lds temp2, ADCH
    andi temp2, 0b00000011
    mul temp2, temp1
    add regH, r0 ; ADCH*Vref
    ldi temp1, 100 ; multiply with 100 for two decimal places
    mul regL, temp1
    mov regL, r0
    mov temp2, r1
    mul regH, temp1
    mov regH, r0
    add regH, temp2
    mov r21, r1
    mov regL, regH ; ADCL*Vref*100
    mov regH, r21 ; ADCH*Vref*100
    ldi count, 2
rotate:
    ror regH ; We only care about high reg so we shift right
    ror regL ; 2 times instead of 10 to do division by 1024
    dec count
    brne rotate
    andi regH, 1 ; keep LSB of regH -> max number is 499 (111110011)
    sbrc regH, 0 ; check if number is >255
    rjmp count100s
    cpi regL, 99 ; check if number is >99
    brlo output100s
count100s:
    inc count 
    sbiw regL, 63 ; subtract 100 in two steps
    sbiw regL, 37 ; because sbiw has limit of 63
    sbrc regH, 0 ; check if number is >255
    rjmp count100s
    cpi regL, 99 ; check if number is >99
    brlo output100s
    breq output100s
    rjmp count100s
output100s:
    mov r24, count	; integer place
    ori r24, 0b00110000 
    rcall lcd_data
    ldi r24, 0b00101110 ; dot character
    rcall lcd_data
    ldi count, 0
    cpi regL, 10
    brlo output10s
count10s:
    inc count
    subi regL, 10 ; subtract 10
    cpi regL, 10 ; check if number is >=10
    brge count10s
output10s:
    mov r24, count	; 1st decimal place
    ori r24, 0b00110000
    rcall lcd_data
    mov r24, regL	; 2nd decimal place
    ori r24, 0b00110000
    rcall lcd_data
    ldi r24, 0b01010110 ; V character
    rcall lcd_data
    reti
   
write_2_nibbles:
    push r24 ; save r24(LCD_Data)
    in r25 ,PIND ; read PIND
    andi r25 ,0x0f ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_High_Byte
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    pop r24 ; Recover r24(LCD_Data)
    swap r24 ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_Low_Byte
    out PORTD ,r24
    sbi PORTD ,PD3;
    nop
    nop
    cbi PORTD ,PD3
    ret

lcd_data:
    sbi PORTD ,PD2 ; LCD_RS=1(PD2=1), Data
    rcall write_2_nibbles ; send data
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret
    
lcd_command:
    cbi PORTD ,PD2 ; LCD_RS=0(PD2=0), Instruction
    rcall write_2_nibbles ; send Instruction
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret
    
lcd_clear_display:
    ldi r24 ,0x01 ; clear display command
    rcall lcd_command
    ldi r24 ,low(5) ;
    ldi r25 ,high(5) ; Wait 5 mSec
    rcall wait_msec ;
    ret
    
lcd_init:
    ldi r24 ,low(200) ;
    ldi r25 ,high(200) ; Wait 200 mSec
    rcall wait_msec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x20 ; command to switch to 4 bit mode
    out PORTD ,r24
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x28 ; 5x8 dots, 2 lines
    rcall lcd_command
    ldi r24 ,0x0c ; dislay on, cursor off
    rcall lcd_command
    rcall lcd_clear_display
    ldi r24 ,0x06 ; Increase address, no display shift
    rcall lcd_command ;
    ret
    
wait_msec:
    push r24 ; 2 cycles
    push r25 ; 2 cycles
    ldi r24 , low(999) ; 1 cycle
    ldi r25 , high(999) ; 1 cycle
    rcall wait_usec ; 998.375 usec
    pop r25 ; 2 cycles
    pop r24 ; 2 cycles
    nop ; 1 cycle
    nop ; 1 cycle
    sbiw r24 , 1 ; 2 cycles
    brne wait_msec ; 1 or 2 cycles
    ret ; 4 cycles
wait_usec:
    sbiw r24 ,1 ; 2 cycles (2/16 usec)
    call delay_8cycles ; 4+8=12 cycles
    brne wait_usec ; 1 or 2 cycles
    ret
delay_8cycles:
    nop
    nop
    nop
    ret