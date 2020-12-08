/*
 * sevensegment.h
 *
 * Created: 10/31/2020 11:27:47 PM
 *  Author: golob
 */ 


#ifndef SEVENSEGMENT_H_
#define SEVENSEGMENT_H_

typedef struct sSeg {
	uint8_t pinA;
	uint8_t pinB;
	uint8_t pinC;
	uint8_t pinD;
	uint8_t digit0;
	uint8_t digit1;
	uint8_t digitValue0;
	uint8_t digitValue1;
	uint8_t lastDisplayed;
} sSeg_t, *sSegPtr_t;

void displayDigits(sSegPtr_t disp);
void countValueUp(sSegPtr_t disp);
void setDisplayValue(uint8_t value, sSegPtr_t disp);

#endif /* SEVENSEGMENT_H_ */