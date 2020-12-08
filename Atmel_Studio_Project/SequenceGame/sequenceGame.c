/*
 * sequenceGame.c
 *
 * Author : Jon Golobay
 */ 
#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 16000000UL		// define it now as 16 MHz unsigned long
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>	
#include "sevenSegment.h"

#define ledA PC0
#define ledB PC1
#define ledC PC2

#define digit0 PC3
#define digit1 PC4

#define sSegA PB2	
#define sSegB PB3	
#define sSegC PB4
#define sSegD PB5

#define buttonPur PD2
#define buttonYel PD3
#define buttonRed PD4		
#define buttonBlu PD5
#define buttonGre PD6
#define buttonOra PD7

#define speaker PB1

enum color{PURPLE, YELLOW, RED, BLUE, GREEN, ORANGE, OFF};
enum state{idle, start, playSequence, inputSequence, win, lose};
	
static uint8_t randomSeed = 0, *randomSeedPtr = &randomSeed;

volatile uint8_t toneCount = 0xFF, *toneCountPtr = &toneCount;

static enum state gameState = idle;
volatile static enum color input, lastInput;
volatile uint8_t roundCount = 0;
volatile uint8_t inputCounter = 0;

#define MAX_SEQ_LENGTH 5
uint8_t randomSequence[MAX_SEQ_LENGTH];

sSeg_t disp = {sSegA, sSegB, sSegC, sSegD, digit0, digit1, 0, 0, 0};

// Function Prototypes
void driveOutput(enum color col);
void displaySequence(int length, uint8_t sequence[]);
enum state acceptInput(uint8_t length);
void generateSequence(uint8_t length, uint8_t *seed);
void seedSequence(uint8_t *seed);
void toneOn(uint8_t count);
void toneOff(void);

int main(void)
{	
	// setup speaker as output
	DDRB |= (1 << speaker);
	
	// setup timer 1 for speaker
	TCCR1A = 0x00; // Normal operation
	TCCR1B = (1<<CS11)|(1<<CS10)|(1<<WGM12)|(1<<ICNC1); // 256x prescaler, MODE 4
	TCCR1C = 0x80;

	// Setup seven segment display outputs
	DDRC |= (1 << digit0) | (1 << digit1);
	DDRB |= (1 << sSegA) | (1 << sSegB) | (1 << sSegC) | (1 << sSegD);
	PORTB |= (1 << sSegA) | (1 << sSegB) | (1 << sSegC) | (1 << sSegD);

	// setup timer 2 for seven segment time division multiplexing
	TCCR2A = 0b00000010;
	TCCR2B = 0b00000101;
	TIMSK2 = 0b00000010;
	OCR2A = 0b01000100;
		
	// setup buttons as inputs with pull-ups
	DDRD &= ~(1 << buttonPur) | ~(1 << buttonYel) | ~(1 << buttonRed) | ~(1 << buttonBlu) | ~(1 << buttonGre) | ~(1 << buttonOra);
	PORTD |= (1 << buttonPur) | (1 << buttonYel) | (1 << buttonRed) | (1 << buttonBlu) | (1 << buttonGre) | (1 << buttonOra);

	// enable button interrupts
	PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);
	
	// LED SETUP
	// setup led decoder outputs
	DDRC |= (1 << ledA) | (1 << ledB) | (1 << ledC);
		
	// enable global interrupts
	sei();
	
	//seed random sequence generator at power on
	seedSequence(randomSeedPtr);
	
	while (1)
	{	
		while (gameState == idle) {
			roundCount = 0;
			//enable button interrupts
			PCICR |= (1 << PCIE2);
			driveOutput(OFF);
			setDisplayValue(roundCount,&disp);
			toneOff();
		}
		while (gameState == start) {
			generateSequence(MAX_SEQ_LENGTH, randomSeedPtr);
			gameState = playSequence;
		}
		while (gameState == playSequence) {
			//disable button interrupts
			PCICR &= ~(1 << PCIE2);
			setDisplayValue(roundCount,&disp);
			displaySequence(roundCount+1, randomSequence);
			gameState = inputSequence;
		}
		while (gameState == inputSequence) {
			//enable button interrupts
			PCICR |= (1 << PCIE2);
			gameState = acceptInput(roundCount+1);
		}
		while (gameState == win) {
			setDisplayValue(roundCount,&disp);
			_delay_ms(100);
			for(int i=0; i<3; i++) {
				driveOutput(PURPLE);
				_delay_ms(200);
				driveOutput(BLUE);
				_delay_ms(200);
				driveOutput(YELLOW);
				_delay_ms(200);
				driveOutput(GREEN);
				_delay_ms(200);
				driveOutput(RED);
				_delay_ms(200);
				driveOutput(ORANGE);
				_delay_ms(200);
			}
			gameState = idle;
		}
		while (gameState == lose) {
			TCCR1A = 0x03; 
			toneOn(0xFF);
			_delay_ms(2500);
			toneOff();
			TCCR1A = 0x00;
			gameState = idle;
		}
	}
	return 0;
	
	
}


void displaySequence(int length, uint8_t sequence[]){
	//loop through and display length of sequence
	for(int i=0; i < length; i++){
		driveOutput(OFF);
		_delay_ms(100);
		driveOutput(sequence[i]);
		_delay_ms(1000);
	}
	driveOutput(OFF);
	_delay_ms(100);
}

enum state acceptInput(uint8_t length){
	
	inputCounter = 0;
	uint8_t i = 0;

	while(i < length) {
		lastInput = OFF;
		while(inputCounter <= 20) {
			_delay_ms(100);  // 100 works good here for play, 1000 for testing
			inputCounter++;
			//setDisplayValue(inputCounter, &disp);
			if(lastInput == randomSequence[i]){
				inputCounter=0;
				i++;
				_delay_ms(100);
				if(length == MAX_SEQ_LENGTH  && i == length) {
					roundCount++;
					_delay_ms(1000);
					driveOutput(OFF);
					_delay_ms(1000);
					return win;
				}
				if(i == length) {
					roundCount++;
					_delay_ms(1000);
					driveOutput(OFF);
					_delay_ms(1000);
					return playSequence;
				}
				break;
			}
			else if(inputCounter == 5) driveOutput(OFF);
			else if(inputCounter >= 20) return lose;
			else if(lastInput != randomSequence[i] && lastInput != OFF) return lose;
		}
		inputCounter=0;
	}
	_delay_ms(500);
	return playSequence;
}


void driveOutput(enum color col) {
	switch (col) {
		case(PURPLE):	// 000
			toneOn(0x80);
			PORTC  &= ~((1 << ledC) | (1 << ledB) | (1 << ledA));
			break;
		case(YELLOW):	// 001
			toneOn(0x70);
			PORTC  |= (1 << ledA);
			PORTC  &= ~((1 << ledB) | (1 << ledC));
			break;
		case(RED):		// 010
			toneOn(0x60);
			PORTC  |= (1 << ledB);
			PORTC  &= ~((1 << ledC) | (1 << ledA));
			break;
		case(BLUE):		// 011
			toneOn(0x50);
			PORTC  |= ((1 << ledB) | (1 << ledA));
			PORTC  &= ~(1 << ledC);
			break;
		case(GREEN):	// 100
			toneOn(0x40);
			PORTC  |= (1 << ledC) ;
			PORTC  &= ~((1 << ledB) | (1 << ledA));
			break;
		case(ORANGE):	// 101
			toneOn(0x30);
			PORTC  |= ((1 << ledC) | (1 << ledA));
			PORTC  &= ~(1 << ledB);
			break;
		case(OFF):		// 111
			PORTC  |= ((1 << ledC) | (1 << ledB) | (1 << ledA));
			toneOff();
			break;
	}
}


void seedSequence(uint8_t *seed){
	*seed = 1;
	uint8_t ADCval;

	ADMUX = 5;         // use ADC5
	ADMUX |= (1 << REFS0);    // use AVcc as the reference
	ADMUX &= ~(1 << ADLAR);   // clear for 10 bit resolution
		
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescale 
	ADCSRA |= (1 << ADEN);    // Enable the ADC

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion

	while(ADCSRA & (1 << ADSC));      // Waits for ADC to finish conversion

	ADCval = ADCL;
	ADCval = (ADCH << 8) + ADCval;    // ADCH is read so ADC can be updated again
	ADCSRA &= ~(1 << ADEN);  //disable the ADC
	*seed = ADCval;
}

void generateSequence(uint8_t length, uint8_t *seed){
	for(int i=0; i<length; i++) {
		*seed ^= (*seed << 7);
		*seed ^= (*seed >> 5);
		*seed ^= (*seed << 3);
		randomSequence[i] = *seed % 6;
	}
}

void toneOn(uint8_t count)
{	
	OCR1A = count;
	TIMSK1 |= (1 << OCIE1A);
}

void toneOff(void)
{
	// disable timer1 interrupts to stop tone
	TIMSK1 &= (~1 << OCIE1A);
}


ISR(TIMER2_COMPA_vect) {
	displayDigits(&disp);
}

ISR(TIMER1_COMPA_vect)
{
	PORTB ^= (1 << speaker);
}

ISR(PCINT2_vect) {
	if(gameState == idle) gameState = start;
	else if(gameState == inputSequence  && inputCounter != 0) {
		if(!(PIND & (1 << buttonPur))) {
			input = PURPLE;
			lastInput = PURPLE;
		}
		else if(!(PIND & (1 << buttonYel))) {
			input = YELLOW;
			lastInput = YELLOW;
		}
		else if(!(PIND & (1 << buttonRed))) {
			input = RED;
			lastInput = RED;

		}
		else if(!(PIND & (1 << buttonBlu))) {
			input = BLUE;
			lastInput = BLUE;
		}
		else if(!(PIND & (1 << buttonGre))) {
			input = GREEN;
			lastInput = GREEN;
		}
		else if(!(PIND & (1 << buttonOra))) {
			input = ORANGE;
			lastInput = ORANGE;
		}
		else {
			input = OFF;
		}
		driveOutput(input);
	}
}