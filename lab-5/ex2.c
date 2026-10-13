#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "interface.h"

int main(){
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1 P4-P7 as input and P0 as output
    PCA9555_0_write(REG_OUTPUT_1, 0xfe);        //Enable Pull-up IO1

    while(1)
        PCA9555_0_write(REG_OUTPUT_0, ~(PCA9555_0_read(REG_INPUT_1) >> 4));
}