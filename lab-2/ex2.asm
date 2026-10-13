.include "m328PBdef.inc"	; ATmega328P microcontroller definitions

.org 0x0
rjmp reset
.org 0x2
rjmp ISR0
    
.equ FOSC_MHZ=16		; Microcontroller operating frequency in MHz
.equ DEL_mS=2000		; Delay in mS (valid number from 1 to 4095)
.equ DEL_NU=FOSC_MHZ*DEL_mS	; delay_mS routine: (1000*DEL_NU+6) cycles

reset:
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24

    ldi r24,(1<<ISC01)|(1<<ISC00) ; INT0 happens on rising edge
    sts EICRA, r24
    ldi r24,(1<<INT0)		; Enable INT0
    out EIMSK, r24		
    sei				; Enable interrupts
    
    ser r26
    out DDRC, r26		; PORTC output
    clr r26
    out DDRB, r26		; PORTB input
    
loop1:				; Count from 0-31 in PORTC
    clr r26
loop2:
    out PORTC, r26
    ldi r24, low(DEL_NU)
    ldi r25, high(DEL_NU)
    rcall delay_mS					
    inc r26
    cpi r26, 32	
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

ISR0:				; INT0 service routine
    push r25			; Save r24, r25, SREG
    push r24
    in r24, SREG
    push r24
    
    ldi r16, 4			; number of buttons to check
    ldi r17, 0			; count the buttons pressed
    in r18, PINB		; input register
    clr r19			; output register

check:    
    ror r18			; rotate right (C flag=LSB of input)
    brcc count			; check value of C flag
    rjmp skip			; if C=1 -> button not pressed -> skip
count:
    inc r17			; if C=0 -> button pressed -> increase count
skip:
    dec r16			; check next bit
    brne check

    cpi r17, 0			; if count=0
    breq end			; go to output
output:
    seC				; set C flag
    rol r19			; rotate left to set one bit...
    dec r17			; ...for each button pressed
    brne output
   
end:
    out PORTC, r19		; output in PORTC
    ldi r24,low(16*1000)
    ldi r25,high(16*1000)
    rcall delay_mS
    pop r24                     ; Restore r24, r25, SREG
    out SREG, r24
    pop r24
    pop r25
    reti