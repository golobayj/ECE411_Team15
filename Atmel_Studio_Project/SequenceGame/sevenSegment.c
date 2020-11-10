/*
 * sevenSegment.c
 *
 * Created: 10/31/2020 11:29:19 PM
 *  Author: golob
 */ 

#include <avr/io.h>
#include "sevenSegment.h"

/*
 * displayDigit function to drive SN74LS47 IC
 * input integer in BCD to drive a common anode 7-seg display
 * PORTC must be used, pins are configurable in struct
 */

void displayDigits(sSegPtr_t disp) {
	int value;
	//ACTIVE LOW PNP
	if(disp->lastDisplayed == disp->digit0) {
		PORTD |= (1 << disp->digit1);
		PORTD &= ~(1 << disp->digit0);
		value = disp->digitValue0;
		disp->lastDisplayed = disp->digit1;
	}
	else  {
		PORTD |= (1 << disp->digit0);
		PORTD &= ~(1 << disp->digit1);
		value = disp->digitValue1;
		disp->lastDisplayed = disp->digit0;
	}
	
	switch (value) {
		case(0): 
			PORTB  &= ~((1 << disp->pinA) | (1 << disp->pinB) | (1 << disp->pinC) | (1 << disp->pinD));
			break;
		case(1):
			PORTB  |= ((1 << disp->pinA));
			PORTB  &= ~((1 << disp->pinB) | (1 << disp->pinC) | (1 << disp->pinD));
			break;
		case(2):
			PORTB  |= ((1 << disp->pinB));
			PORTB  &= ~((1 << disp->pinA) | (1 << disp->pinC) | (1 << disp->pinD));
			break;
		case(3):
			PORTB  |= ((1 << disp->pinA) |  (1 << disp->pinB));
			PORTB  &= ~((1 << disp->pinC) | (1 << disp->pinD));
			break;
		case(4):
			PORTB  |= ((1 << disp->pinC));
			PORTB  &= ~((1 << disp->pinA) | (1 << disp->pinB) |  (1 << disp->pinD));
			break;
		case(5):
			PORTB  |= ((1 << disp->pinA) |  (1 << disp->pinC));
			PORTB  &= ~((1 << disp->pinB) | (1 << disp->pinD));
			break;
		case(6):
			PORTB  |= ((1 << disp->pinB) |  (1 << disp->pinC));
			PORTB  &= ~((1 << disp->pinA) | (1 << disp->pinD));
			break;
		case(7):
			PORTB  |= ((1 << disp->pinA) | (1 << disp->pinB) | (1 << disp->pinC));
			PORTB  &= ~((1 << disp->pinD));
			break;
		case(8):
			PORTB  |= ((1 << disp->pinD));
			PORTB  &= ~((1 << disp->pinA) | (1 << disp->pinB) | (1 << disp->pinC));
			break;
		case(9):			
			PORTB  |= ((1 << disp->pinA) |  (1 << disp->pinD));
			PORTB  &= ~((1 << disp->pinB) | (1 << disp->pinC));
			break;
		default:
			PORTB |= ((1 << disp->pinA) | (1 << disp->pinB) | (1 << disp->pinC) | (1 << disp->pinD));
			break;
	}
}

void countValueUp(sSegPtr_t disp){
	if (disp->digitValue1 == 9 && disp->digitValue0 == 9) {
		disp->digitValue1 = 0;
		disp->digitValue0 = 0;
	}
	else if(disp->digitValue0 == 9){
		disp->digitValue1 ++;
		disp->digitValue0 = 0;
	}
	else disp->digitValue0++;
}

void setValue(int setValue1, int setValue0, sSegPtr_t disp) {
	disp->digitValue1 = setValue1;
	disp->digitValue0 = setValue0;
}

