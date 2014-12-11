ECE382-Lab8
===========


#Functionality
Required: https://www.youtube.com/watch?v=6VaRGae-kNU&feature=youtu.be 

#Code Walk Through

Header file
-includes function prototypes, listed below

void goForward(void);
void goBackward(void);
void stopMotor(void);
void turnRight(void);
void turnLeft(void);

C file
-Main loop controls the sensors and robot movement 

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


#Debugging
My initial attempt at required functionality was unsuccessful. Originally I had created subroutines to poll and read the IR sensors, then call them in main and choose a movement then. After much troubleshooting, and some advice from C2C Eric Wardner, I reorganized the main loop more like Lab 7. This second attempt WAS successful and can be viewed on the video linked above. 

#Observations/Conclusions
Simpler is easier for me! I can't handle lots of coordinating details. I could have done all of the code in one file instead of two. 

#Documentation
C2C Eric Wardner suggested I reorganize my main loop. 
