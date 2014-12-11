#include <msp430.h>
#include "maze.h"

void initMSP430();
unsigned short sampleL[16], sampleC[16], sampleR[16];
unsigned short rangeL, rangeR, rangeF;

void main(void) {
	WDTCTL = WDTPW | WDTHOLD;                 // stop the watchdog timer

	unsigned char i = 0;
	IFG1 = 0; 											// clear interrupt flag1

	BCSCTL1 = CALBC1_8MHZ;										// 8MHz clock
	DCOCTL = CALDCO_8MHZ;

	P1DIR = BIT0 | BIT6;

	while (1) {

		// Configure P1.4 to be the ADC input LEFT
		ADC10CTL0 = 0;								// Turn off ADC subsystem
		ADC10CTL1 = INCH_4 | ADC10DIV_3;				// Channel 4, ADC10CLK/4
		ADC10AE0 = BIT4;		 					// Make P1.4 analog input
		ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;// Vcc & Vss as reference

		ADC10CTL0 |= ADC10SC;							// Start a conversion
		while (ADC10CTL1 & ADC10BUSY)
			;							// Wait for conversion to complete
		sampleL[i] = ADC10MEM;						// collect that 10-bit value
		if (sampleL[i] > 0x0200) {
			P1OUT |= BIT0;
			turnRight();
			_delay_cycles(10000);
			stopMotor();
			_delay_cycles(1000000);
		} else {
			P1OUT &= ~BIT0;

		}

		//i = (i+1) & 0xF;							// This is a mod 16 increment

		// Configure P1.3 to be the ADC input RIGHT
		ADC10CTL0 = 0;								// Turn off ADC subsystem
		ADC10CTL1 = INCH_3 | ADC10DIV_3;				// Channel 3, ADC10CLK/4
		ADC10AE0 = BIT3;		 					// Make P1.3 analog input
		ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;// Vcc & Vss as reference

		ADC10CTL0 |= ADC10SC;							// Start a conversion
		while (ADC10CTL1 & ADC10BUSY)
			;							// Wait for conversion to complete
		sampleR[i] = ADC10MEM;						// collect that 10-bit value
		if (sampleR[i] > 0x0200)
			P1OUT |= BIT6;
		else
			P1OUT &= ~BIT6;

		//i = (i+1) & 0xF;							// This is a mod 16 increment

		// Configure P1.5 to be the ADC input CENTER
		ADC10CTL0 = 0;								// Turn off ADC subsystem
		ADC10CTL1 = INCH_5 | ADC10DIV_3;				// Channel 2, ADC10CLK/4
		ADC10AE0 = BIT5;		 					// Make P1.2 analog input
		ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;// Vcc & Vss as reference

		ADC10CTL0 |= ADC10SC;							// Start a conversion
		while (ADC10CTL1 & ADC10BUSY)
			;							// Wait for conversion to complete
		sampleC[i] = ADC10MEM;						// collect that 10-bit value
		if (sampleC[i] > 0x0200) {
			P1OUT |= BIT0 | BIT6;
			turnRight();
			_delay_cycles(2500000);
			stopMotor();
			_delay_cycles(10000000);
//			turnRight();
//			_delay_cycles(1000000);
//			stopMotor();
		} else {
			P1OUT &= ~(BIT0 | BIT6);
			goForward();
			_delay_cycles(1800000);
			stopMotor();
			_delay_cycles(10000000);

		}

		i = (i + 1) & 0xF;						// This is a mod 16 increment

	} // end infinite loop

} // end main

void goForward(void) {
	P1DIR &= ~BIT3;
	P1REN |= BIT3;

	P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
	P2SEL |= BIT2;							// Turns on L wheel

	P2DIR |= BIT4;							// P2.2 is associated with TA1CCR2
	P2SEL |= BIT4;							// Turns on R wheel

	P2DIR |= BIT1;							// P2.2 is associated with TA1CCR2
	P2OUT |= BIT1;				// OUT bc coming out of board... setting motor

	P2DIR |= BIT3;							// P2.2 is associated with TA1CCR2
	P2OUT |= BIT3;				// OUT bc coming out of board... setting motor

	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
	TA1CCR0 = 0x0100;						// set signal period

	TA1CCR1 = 0x0020;
	TA1CCTL1 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode

	TA1CCR2 = 0x0020;
	TA1CCTL2 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode

}
void goBackward(void) {
	P1DIR &= ~BIT3;
	P1REN |= BIT3;

	P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
	P2SEL |= BIT2;							// P2.2 is associated with TA1CCTL1

	P2DIR |= BIT4;							// P2.2 is associated with TA1CCR2
	P2SEL |= BIT4;							// P2.2 is associated with TA1CCTL2

	P2DIR |= (BIT1 | BIT3);					// P2.2 is associated with TA1CCR2
	P2OUT &= ~(BIT1 | BIT3);	// OUT bc coming out of board... setting motor

	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
	TA1CCR0 = 0x0100;						// set signal period

	TA1CCR1 = 0x0010;
	TA1CCTL1 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode

	TA1CCR2 = 0x0010;
	TA1CCTL2 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode

}

void stopMotor(void) {
	P1DIR &= ~BIT3;
	P1REN |= BIT3;

	P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
	P2SEL |= BIT2;							// P2.2 is associated with TA1CCTL1

	P2DIR |= BIT4;							// P2.2 is associated with TA1CCR2
	P2SEL |= BIT4;							// P2.2 is associated with TA1CCTL2

	P2DIR = 0;
	P2OUT = 0;

	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
	TA1CCR0 = 0x0100;						// set signal period

	TA1CCR1 = 0x0020;
	TA1CCTL1 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode

	TA1CCR2 = 0x0020;
	TA1CCTL2 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode
}
void turnLeft(void) {
	P1DIR &= ~BIT3;
	P1REN |= BIT3;

	P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
	P2SEL |= BIT2;							// P2.2 is associated with TA1CCTL1

	P2DIR |= BIT4;							// P2.2 is associated with TA1CCR2
	P2SEL |= BIT4;							// P2.2 is associated with TA1CCTL2

	P2DIR |= BIT1;							// P2.2 is associated with TA1CCR2
	P2OUT |= BIT1;				// OUT bc coming out of board... setting motor

	P2DIR &= ~BIT3;							// P2.2 is associated with TA1CCR2
	P2OUT &= ~BIT3;				// OUT bc coming out of board... setting motor

	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
	TA1CCR0 = 0x0100;						// set signal period

	TA1CCR1 = 0x0020;
	TA1CCTL1 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode

	TA1CCR2 = 0x0020;
	TA1CCTL2 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode
}
void turnRight(void) {
	P1DIR &= ~BIT3;
	P1REN |= BIT3;

	P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
	P2SEL |= BIT2;							// P2.2 is associated with TA1CCTL1

	P2DIR |= BIT4;							// P2.2 is associated with TA1CCR2
	P2SEL |= BIT4;							// P2.2 is associated with TA1CCTL2

	P2DIR &= ~BIT1;							// P2.2 is associated with TA1CCR2
	P2OUT &= ~BIT1;				// OUT bc coming out of board... setting motor

	P2DIR |= BIT3;							// P2.2 is associated with TA1CCR2
	P2OUT |= BIT3;				// OUT bc coming out of board... setting motor

	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
	TA1CCR0 = 0x0100;						// set signal period

	TA1CCR1 = 0x0020;
	TA1CCTL1 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode

	TA1CCR2 = 0x0020;
	TA1CCTL2 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode
}
