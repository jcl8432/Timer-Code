/* This timer is using a msp430 micro-controller with multiplexing to create a timer that you see in the video. Only one led is on at once and you will see this in
 * the video where it is flashing through 9s on the 4  7-segment displays. The use of multiplexing is so then I could use all four displays due to the fact that if I didn't
 * I would not have  enough pins on the board to create the 4 7-segment displays there would only be two. The effect where all the LEDs look like they are on is
 * persistance of vision where the eyes reach a point where a flashing light or led appears to be on all the time even if it isn't.
 */
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define TIMER_TICKS_INTERRUPT 32678 //32,678 ticks at 32,678 Hz gives one second



#define POSITION_1   (BIT0)
#define POSITION_2   (BIT1)
#define POSITION_3   (BIT2)
#define POSITION_4   (BIT4)
#define POSITION_ALL (POSITION_1 + POSITION_2 + POSITION_3 + POSITION_4)
const uint8_t position_map[4] = {POSITION_1, POSITION_2, POSITION_3, POSITION_4};

#define DIGITS_IN_DISP 4

#define DIGIT_0      (BIT5 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0) //The LEDs that need to be on to make a 0
#define DIGIT_1      (BIT2 + BIT1) // gives the design of the digit 1
#define DIGIT_2      (BIT6 + BIT4 + BIT3 + BIT1 + BIT0) // gives which leds need to be on to give a 2
#define DIGIT_3      (BIT6 + BIT3 + BIT2 + BIT1 + BIT0) // gives the leds to put the number 3 on the display
#define DIGIT_4      (BIT6 + BIT5 + BIT2 + BIT1) // gives the LEDs that need to be on to make the number 4
#define DIGIT_5      (BIT6 + BIT5 + BIT3 + BIT2 + BIT0)
#define DIGIT_6      (BIT6 + BIT5 + BIT4 + BIT3 + BIT2 + BIT0)
#define DIGIT_7      (BIT2 + BIT1 + BIT0)
#define DIGIT_8      (BIT6 + BIT5 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0)
#define DIGIT_9      (BIT6 + BIT5 + BIT2 + BIT1 + BIT0)
#define DIGIT_ALL    (BIT7 + BIT6 + BIT5 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0)
#define DIGIT_DP     (BIT7)
const uint8_t digit_map[10] = {DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
							   DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9};
// Truth Table for the pins and the numbers
//     DP  G   F   E   D   C   B   A  | NUM
//    1.7 1.6 1.5 1.4 1.3 1.2 1.1 1.0 |
//    --- --- --- --- --- --- --- ---   ---
//     0   0   1   1   1   1   1   1  |  0
//     0   0   0   1   1   0   0   0  |  1
//     0   1   0   1   1   0   1   1  |  2
//     0   1   0   0   1   1   1   1  |  3
//     0   1   1   0   0   1   1   0  |  4
//     0   1   1   0   1   1   0   1  |  5
//     0   1   1   1   1   1   0   1  |  6
//     0   0   0   0   0   1   1   1  |  7
//     0   1   1   1   1   1   1   1  |  8
//     0   1   1   0   0   1   1   1  |  9
//     1   0   0   0   0   0   0   0  |  DP

uint8_t timer_count = 0;
uint8_t number_digits[4] = {0, 0, 0, 0};

bool timer_running = false; // timer is set to not running

void initialize_GPIO(void);
void clear_digits(void);
void disp_digit(uint8_t position, uint8_t digit);
void initialize_TIMER(void);

void main()
[]{
	volatile uint16_t i;        // iteration counter for delay loop
	uint8_t digit_position = 0;

	WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       //always include these two lines

	initialize_GPIO();
	initialize_TIMER();

	while(1)
	{
		// This loops through the digits and when it equals 9 it adds one to the next position and resets the current position to 0
		if(timer_count != 0)
		{
			if(number_digits[3] == 9)
			{
				number_digits[3] = 0;
				if(number_digits[2] == 9)
				{
					number_digits[2] = 0;
					if(number_digits[1] == 9)
					{
						number_digits[1] = 0;
						if(number_digits[0] == 9)
						{
							number_digits[0] = 0;
						}
						else
						{
							number_digits[0] += 1;
						}
					}
					else
					{
						number_digits[1] += 1;
					}
				}
				else
				{
					number_digits[2] += 1;
				}
			}
			else
			{
				number_digits[3] += 1;

			}

			timer_count = 0;

		}
		// When the timer reaches 9,999 seconds it will stop the timer
		if((number_digits[3] == 9) &&
				(number_digits[2] == 9) &&
				(number_digits[1] == 9) &&
				(number_digits[0] == 9))
		{
			TA0CTL &= ~MC_3;
			timer_running = false; //sets the boolean expression to false
		}


		// this is for the start of the timer
		if(digit_position == 3)
		{
			digit_position = 0;
		}
		else
		{
			digit_position += 1; // adds one to the digit if the digit is in position 3
		}

		uint8_t digit = number_digits[digit_position];
		disp_digit(position_map[digit_position], digit_map[digit]);

		for(i=50;i>0;i--){ // this is set to be 500 so it allows the persistence   of vision
			                                        //empty for loop for delay
		}

	}





}

void initialize_GPIO(void)
{

	//set all pins for digits and positions to output
	P1DIR |= DIGIT_ALL;
	P2DIR |= POSITION_ALL;
	//https://www.ocfreaks.com/msp430-gpio-programming-tutorial/
	P2DIR &= ~(BIT3 + BIT7); // enabling input for pins p2.3 and p2.7
	P2REN |= (BIT3 + BIT7);// enabling pull up resistor
	P2OUT |= (BIT3 + BIT7);
	P2IFG &= ~(BIT3 + BIT7);// clearing interrupts
	P2IES |= (BIT3 + BIT7); // enabling high to low edge interrupt
	P2IE |= (BIT3 + BIT7); // enabling interrupts

}

void clear_digits(void)
{
	//set the position pins to low
	P2OUT &= ~POSITION_ALL;
	//setting the digit pins to high
	P1OUT |= DIGIT_ALL;
}

void disp_digit(uint8_t position, uint8_t digit)
{
	clear_digits();
	//set the digit pin outputs to low
	P1OUT &= ~digit;
	//set the position pin outputs to high
	P2OUT |= position;
}

void initialize_TIMER(void)
{
	//Initialize timer using ACLK source with interrupts
	// reference https://www.ocfreaks.com/msp430-timer-programming-tutorial/
	TA0CCTL0 |= CCIE; // enable capture control interrupt
	TA0CCR0 = TIMER_TICKS_INTERRUPT; // configure interrupt compare for 32,678 ticks or one second
	TA0CTL  |= TASSEL_1 | MC__STOP;// sets the clock source to aclk
	_enable_interrupt();
}
// sets the interrupts
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
	TA0CCR0 += TIMER_TICKS_INTERRUPT; // update interrupt compare count
	timer_count += 1; // increases the timer count by one

}
// Reference for  pull up and pull down registers https://www.ti.com/lit/ug/slau396f/slau396f.pdf?ts=1732984696054
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_IRS(void)
{
	if (P2IFG & BIT3)
	{
		if(TA0CTL & MC_2)
		{
			TA0CTL &= ~MC_3; // stop timer
			timer_running = false; // checks to see if the loop is running
		}
		else
		{
			// if the numbers on each display are not equal to 9999 it will continue to run.
			if((number_digits[3] != 9) &&
					(number_digits[2] != 9) &&
					(number_digits[1] != 9) &&
					(number_digits[0] != 9))
			{
				TA0CTL |= MC_2;
				timer_running = true; // timer is running
			}


		}

		P2IFG &= ~BIT3; //register and pin of 2.3

	}
	if (P2IFG & BIT7) //register and pin
	{
		if(!timer_running) // if check state to see if it is off
		{
			number_digits[0] = 0;
			number_digits[1] = 0;
			number_digits[2] = 0;
			number_digits[3] = 0;
		}// resets the digits to 0000 if the clock is stopped
		P2IFG &= ~BIT7; //setting the pin to check the switch of pin 2.7
	}
}

