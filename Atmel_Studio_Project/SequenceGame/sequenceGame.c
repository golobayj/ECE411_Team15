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

#define digit0 PD6
#define digit1 PD7

#define sSegA PB2	
#define sSegB PB3	
#define sSegC PB4
#define sSegD PB5

#define speaker PB1

enum color{PURPLE, ORANGE, BLUE, YELLOW, GREEN, RED, OFF};
enum state{idle, start, playSequence, inputSequence, win, lose};
	
static uint8_t randomSeed = 0, *randomSeedPtr = &randomSeed;

volatile uint8_t toneCount = 0xFF, *toneCountPtr = &toneCount;

static enum state gameState = idle;
volatile uint8_t roundCount;

#define MAX_SEQ_LENGTH 10
uint8_t sequence[MAX_SEQ_LENGTH];

static uint8_t currentRound = 0;
sSeg_t disp = {sSegA, sSegB, sSegC, sSegD, digit0, digit1, 0, 0, 0};

// Function Prototypes
void driveLED(enum color col);
void displaySequence(int length);
void generateSequence(uint8_t length, uint8_t *seed);
void seedSequence(uint8_t *seed);
void toneOn(uint8_t count);
void toneOff(void);

int main(void)
{	
	int currentColor;
	
	DDRB |= (1 << speaker);

	TCCR2A = 0b00000010;
	TCCR2B = 0b00000101;
	TIMSK2 = 0b00000010;
	OCR2A = 0b10100000;
	
	TCCR1A = 0x00; // Normal operation
	TCCR1B = (1<<CS11)|(1<<CS10)|(1<<WGM12)|(1<<ICNC1); // 256x prescaler, MODE 4

	PCMSK2 |= (1 << PCINT16) | (1 << PCINT17) | (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21);
	
	//enable global interrupts
	sei();
	
	// setup led decoder outputs
	DDRC |= (1 << ledA) | (1 << ledB) | (1 << ledC);
	
	// setup seven segment display outputs
	DDRD |= (1 << digit0) | (1 << digit1);
	DDRB |= (1 << sSegA) | (1 << sSegB) | (1 << sSegC) | (1 << sSegD);
	PORTB |= (1 << sSegA) | (1 << sSegB) | (1 << sSegC) | (1 << sSegD);
	
	// setup buttons as inputs with pull-ups
	DDRD &= ~(1 << PURPLE) | ~(1 << ORANGE) | ~(1 << BLUE) | ~(1 << GREEN) | ~(1 << RED);
	PORTD |= (1 << PURPLE) | (1 << ORANGE) | (1 << BLUE) | (1 << YELLOW) | (1 << GREEN) | (1 << RED);			
	//seed random sequence generator
	seedSequence(randomSeedPtr);
	while (1)
	{	
		while (gameState == idle) {
			//enable button interrupts
			PCICR |= (1 << PCIE2);
			driveLED(OFF);
			setValue(0,0,&disp);
			toneOff();
		}
		while (gameState == start) {
			setValue(1,1,&disp);  //USED TO SHOW STATE FOR DEBUG
			
			generateSequence(MAX_SEQ_LENGTH, randomSeedPtr);
			gameState = playSequence;
		}
		while (gameState == playSequence) {
			setValue(2,2,&disp);  //USED TO SHOW STATE FOR DEBUG
			displaySequence(MAX_SEQ_LENGTH);
			gameState = inputSequence;
		}
		while (gameState == inputSequence) {
			
			//if input wrong lose
			//if input correct to max length win
			//if input correct mid game goto playSequence
			if(currentRound >= MAX_SEQ_LENGTH) gameState = win;
			gameState = idle;
		}
		while (gameState == win) {
			setValue(3,3,&disp);  //USED TO SHOW STATE FOR DEBUG
			countValueUp(&disp);
		}
		while (gameState == lose) {
			setValue(4,4,&disp);  //USED TO SHOW STATE FOR DEBUG
		}
	}
	return 0;
	
	TCCR1C = 0x80;
}

void driveLED(enum color col) {
	PORTC = col;	
}

void driveSpeaker(enum color col) {
	switch(col) {
		case(PURPLE): toneOn(0x80); break;
		case(ORANGE): toneOn(0x70);	break;
		case(BLUE): toneOn(0x60);	break;
		case(YELLOW): toneOn(0x50);	break;
		case(GREEN): toneOn(0x40);	break;
		case(RED): toneOn(0x30);	break;
		default: toneOn(0xFF); break;
	};
}

void displaySequence(int length){
	//disable button interrupts
	PCICR &= ~(1 << PCIE2);
	//loop through and display length of sequence
	for(int i=0; i<length; i++){
		driveLED(sequence[i]);
		driveSpeaker(sequence[i]);
		_delay_ms(1000);
		driveLED(OFF);
		toneOff();
		_delay_ms(100);
	}
	//enable buttons interrupts
	PCICR |= (1 << PCIE2);
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
		sequence[i] = *seed % 6;
	}
}

void toneOn(uint8_t count)
{
	OCR1A = count;
	TIMSK1 = (1<< OCIE1A);
}

void toneOff(void)
{
	// disable timer1 interrupts to stop tone
	TIMSK1 = 0;
}


ISR(TIMER2_COMPA_vect) {
	TCCR1C = 0x80;
	displayDigits(&disp);
	TCCR1C = 0x80;
}

ISR(TIMER1_COMPA_vect)
{
	PORTB ^= (1 << speaker);
}

ISR(PCINT2_vect) {
	enum color input;
	
	if(gameState==idle) gameState=start;
	
	if(!(PIND & (1 << PURPLE))) {
		input = PURPLE;
	}
	else if(!(PIND & (1 << ORANGE))) {
		input = ORANGE;
	}
	else if(!(PIND & (1 << BLUE))) {
		input = BLUE;
	}
	else if(!(PIND & (1 << YELLOW))) {
		input = YELLOW;
	}
	else if(!(PIND & (1 << GREEN))) {
		input = GREEN;
	}
	else if(!(PIND & (1 << RED))) {
		input = RED;
	}
	else {
		input = OFF;
	}
	driveLED(input);
	driveSpeaker(input);
}