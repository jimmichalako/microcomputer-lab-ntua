.include "m328PBdef.inc"

.org 0x00
    rjmp reset

.def DC_VALUE=r16

ARRAY:
    .dw 0x0005, 0x001A, 0x002E, 0x0042, 0x0057, 0x006B, 0x007F
    .dw 0x0094, 0x00A8, 0x00BD, 0x00D1, 0x00E6, 0x00FA

reset:
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    
    ldi r24, (1<<COM1A1)|(1<<WGM10) ; fast PWM, 8bit, non-inverted output
    sts TCCR1A, r24
    ldi r24, (1<<WGM12)|(1<<CS12)   ; with frequency -> CLK/256=62500Hz
    sts TCCR1B, r24
    
    ser r24
    out DDRB, r24		; PORTB output
    clr r24
    out DDRD, r24		; PORTD input
    
    ldi ZL, LOW(ARRAY*2)	; Z-register pair holds DC values...
    ldi ZH, HIGH(ARRAY*2)	; ...with size of 26 bytes
    adiw ZL, 12			; start at 50% DC (middle value)
    lpm
    mov DC_VALUE, r0
    
main:
    sts OCR1AL, DC_VALUE
    in r17, PIND		; check input
    sbrs r17, 3
    rjmp increase
    sbrs r17, 4
    rjmp decrease
    rjmp main
    
increase:
    ldi r24, LOW(16*100)	; debouncing
    ldi r25, HIGH(16*100)    
    rcall delay_mS
    cpi DC_VALUE, 0x00FA	; check DC upper limit (98%)
    breq main
    adiw ZL, 2			; increase DC by 8%
    lpm
    mov DC_VALUE, r0
    rjmp main
    
decrease:
    ldi r24, LOW(16*100)	; debouncing
    ldi r25, HIGH(16*100)    
    rcall delay_mS
    cpi DC_VALUE, 0x0005	; check DC lower limit (2%)
    breq main
    sbiw ZL, 2			; decrease DC by 8%
    lpm
    mov DC_VALUE, r0
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