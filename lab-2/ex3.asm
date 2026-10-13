.include "m328PBdef.inc"	; ATmega328P microcontroller definitions

.org 0x0
rjmp reset
.org 0x4
rjmp ISR1

.def output=r16
.def switch=r17
.def count=r18
    
reset:
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24

    ldi r24,(1<<ISC11)|(1<<ISC10) ; INT1 happens on rising edge
    sts EICRA, r24
    ldi r24,(1<<INT1)		; Enable INT1 
    out EIMSK, r24
    sei				; Enable interrupts
    
    ser r26
    out DDRB, r26		; PORTB output
    clr r26
    out DDRD, r26		; PORTD input
    clr switch
    
main:
    cpi switch, 0		; check if switch was pressed
    brne on			; if switch=1 PD3 was pressed
    rjmp main
on:
    ldi output, 0x01		; PB0 --> on
    out PORTB, output
    ldi r24,low(16*20)		; 20ms delay
    ldi r25,high(16*20)		; for each loop
    rcall delay_mS
    dec count			; loop for 5s total
    brne on
    clr switch			
    clr output			; turn all LEDs off
    out PORTB, output		
    rjmp main

delay_mS:			; Delay routine
    ldi r23, 249
loop_inn:
	dec r23
	nop
	brne loop_inn

	sbiw r24, 1
	brne delay_mS
	ret

ISR1:				; INT1 service routine
    push r25			; Save r24, r25, SREG
    push r24
    in r24, SREG                
    push r24

    cpi switch, 0		; check if switch was pressed...
    brne on_again		; ...while PB0 was on 
    ldi switch, 0x01		; PD3 was pressed
    ldi count, 250		; 250*20ms=5s delay for PB0
    pop r24                     ; Restore r24, r25, SREG
    out SREG, r24
    pop r24
    pop r25
    reti

on_again:
    ldi output, 0x3f		; (PB5-PB0) --> ON
    out PORTB, output
    ldi r24,low(16*500)		; 0.5s delay
    ldi r25,high(16*500)	; on refresh
    rcall delay_mS
    ldi count, 225		; 225*20ms=4.5s remaining for PB0
    pop r24                     ; Restore r24, r25, SREG
    out SREG, r24
    pop r24
    pop r25
    reti