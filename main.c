#include <msp430.h> 

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    P1DIR |= 0x01;

    volatile unsigned int i;

    while(1) {
    	P1OUT = 0x01;
    	for(i=0; i <= 20000; i++);
    	P1OUT = 0x00;
    	for(i=0; i <= 20000; i++);
    }
	return 0;
}
