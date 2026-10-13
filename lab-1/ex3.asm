.include "m328PBdef.inc"

.equ FOSC_MHZ=16        ; MHz
.equ DEL_mS=1000         ; mS
.equ F1=FOSC_MHZ*DEL_mS
.def output=r20
.def count=r21

ldi r16, LOW(RAMEND)
out SPL, r16 
ldi r16, HIGH(RAMEND)
out SPH, r16

ldi r24, low(F1)
ldi r25, high(F1)
ser output
out DDRD,output
ldi output,1
ldi count,7
seT
out PORTD,output

right:
    rcall wait_x_msec
    lsl output			; shift output 1 left
    out PORTD, output		; send it to output
    dec count			; decrease the counter
    cpi count, 00		; check if it is 0
    brne right
    rcall wait_x_msec
    clT				;T flag=0
    
left:
    rcall wait_x_msec
    lsr output			; shift output 1 right
    out PORTD, output		; send it to output
    inc count			; increase the counter
    cpi count, 07		; check if it is 7
    brne left			; if it is then don't loop
    rcall wait_x_msec
    seT				;T flag=1
    rjmp right

wait_x_msec:
    push r24              
    push r25              
loop4:
    rcall delay_inner     
    sbiw r24,1            
    brne loop4            
    pop r25               
    pop r24               
    ret
delay_inner:
    ldi r23, 247          
loop3:
    dec r23               
    nop                   
    brne loop3            
    nop                   
    ret     