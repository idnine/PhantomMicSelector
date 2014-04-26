/*
 * Phantom Power MICs Selector
 * April 25, 2014
 * by Koo Jonghoe @ AudioCookie.com
 *
 * https://github.com/idnine/PhantomMicSelector
 * https://www.facebook.com/groups/ew4sm/
 */

#include <msp430.h>

const int		LED1 = 0x04;	// P2.3
const int		LED2 = 0x08;	// P2.4
const int		LED3 = 0x10;	// P2.5
const int		LED4 = 0x20;	// P2.6
const int		EN   = 0x01;	// P1.0
const int		IN1  = 0x01;	// P2.0
const int		IN2  = 0x02;	// P2.1
const int		SW1  = 0x10;	// P1.4
const int		SW2  = 0x20;	// P1.5
const int		SW3  = 0x40;	// P1.6
const int		SW4  = 0x80;	// P1.7

volatile int	selectCh = 1;

/*
 * main.c
 */
int main(void) {
	// Watchdog Off, Int. Clock 1MHz
	WDTCTL = WDTPW + WDTHOLD;	// Stop watchdog timer
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	// IO Setup
	P1DIR &= ~(SW1 + SW2 + SW3 + SW4);
	P1REN |= (SW1 + SW2 + SW3 + SW4);
	P1OUT |= (SW1 + SW2 + SW3 + SW4);
	P1DIR |= EN;
	P2DIR |= (IN1 + IN2);
	P2DIR |= (LED1 + LED2 + LED3 + LED4);

	// Port Change Interrupt Setup
	P1IES = (SW1 + SW2 + SW3 + SW4);
	P1IFG &= ~(SW1 + SW2 + SW3 + SW4);
	P1IE = (SW1 + SW2 + SW3 + SW4);

	// Initial Value Output
	P2OUT |= (LED1 + LED2 + LED3 + LED4);	// All LED Off (Active Low)

	// Default Set CH.1
	P1OUT &= ~EN;
	P2OUT &= ~(IN1 + IN2);
	P2OUT &= ~LED1;

	// Timer Setup
	TACTL = TASSEL_2 + MC_1 + ID_3;
	TACCR0 = 20000;
	TACCTL0 &= ~CCIE;						// Timer Off

	// Global Interrupt Start, System goes Sleep(Power Save) Mode
	_BIS_SR(LPM0_bits + GIE);
}

// =================================================
//                Interrupt: Timer
// =================================================
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
	P2OUT ^= (0x02 << selectCh);
}

// =================================================
//             Interrupt: Port1 Change
// =================================================
#pragma vector=PORT1_VECTOR
__interrupt void selButton(void) {
	// All LED Off
	P2OUT |= (LED1 + LED2 + LED3 + LED4);

	if(P1IFG & SW1) {
		selectCh = 1;
	} else if(P1IFG & SW2) {
		selectCh = 2;
	} else if(P1IFG & SW3) {
		selectCh = 3;
	} else if(P1IFG & SW4) {
		selectCh = 4;
	}
	_delay_cycles(10000);
	switch(selectCh) {
		case 1 :
			P2OUT &= ~LED1;
			P2OUT &= ~(IN1 + IN2);
			break;
		case 2 :
			P2OUT &= ~LED2;
			P2OUT |= IN1;
			P2OUT &= ~IN2;
			break;
		case 3 :
			P2OUT &= ~LED3;
			P2OUT &= ~IN1;
			P2OUT |= IN2;
			break;
		case 4 :
			P2OUT &= ~LED4;
			P2OUT |= (IN1 + IN2);
			break;
	}
	P1IFG &= ~(SW1 + SW2 + SW3 + SW4);
}
