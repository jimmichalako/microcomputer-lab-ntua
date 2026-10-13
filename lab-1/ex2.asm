ldi r24, LOW(RAMEND)
out SPL, r24
ldi r24, HIGH(RAMEND)
out SPH, r24
    
ldi r16,0x51 ;A
ldi r17,0x41 ;B
ldi r18,0x21 ;C
ldi r19,0x01 ;D
ldi r20,0x01 ;A increment
ldi r21,0x02 ;B increment
ldi r22,0x03 ;C increment
ldi r23,0x04 ;D increment
ldi r24,0x06 ;counter

main:
    com r17
    mov r25,r16 ;A
    mov r26,r19 ;D
    and r25,r17 ;A·B'
    and r26,r17 ;B'·D
    com r17
    or r25,r26  ;A·B'+B'·D
    com r25     ;F0
    com r18
    com r19
    mov r26,r16 ;A
    or r26,r18  ;A+C'
    mov r27,r17 ;B
    or r27,r19  ;B+D'
    com r18
    com r19
    and r26,r27 ;F1
    add r16,r20	;increment A
    add r17,r21	;increment B
    add r18,r22	;increment C
    add r19,r23	;increment D
    dec r24
    brne main