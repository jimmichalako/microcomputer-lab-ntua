.include "m328PBdef.inc"	; ATmega328P microcontroller definitions

.org 0x0
rjmp reset
.org 0x4
rjmp ISR1
    
.equ FOSC_MHZ=16		; Microcontroller operating frequency in MHz
.equ DEL_mS=500			; Delay in mS (valid number from 1 to 4095)
.equ DEL_NU=FOSC_MHZ*DEL_mS	; delay_mS routine: (1000*DEL_NU+6) cycles

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
    out DDRC, r26		; PORTC output
    clr r26
    out DDRD, r26		; PORTD input
    clr r16
    
loop1:				; Count from 0-15 in PORTB
    clr r26
loop2:
    out PORTB, r26
    ldi r24, low(DEL_NU)
    ldi r25, high(DEL_NU)
    rcall delay_mS					
    inc r26
    cpi r26, 16			
    breq loop1
    rjmp loop2

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
check:				; Check for flashover
    ldi r24,(1<<INTF1)		
    out EIFR, r24		; load INT1 flag
    ldi r24,low(16*5)		; wait 5ms
    ldi r25,high(16*5)
    rcall delay_mS
    in r24, EIFR		
    sbrc r24,INTF1		; if INTF1 is 0 continue
    rjmp check			; else check again
    
    sbis PIND, 5		; if PD5 is 1 keep counting
    rjmp skip			; but if PD5 is 0 freeze count
    inc r16			; increase interrupts count
    out PORTC, r16		; print them in PORTC
    sbrc r16, 6			; if count > 63
    clr r16			; start again from 0

skip:
    pop r24                     ; Restore r24, r25, SREG
    out SREG, r24
    pop r24
    pop r25
    reti