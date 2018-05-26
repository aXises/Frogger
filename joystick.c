/*
 * joystick.c
 *
 * Created: 5/22/2018 11:47:18 PM
 *  Author: Xinyi Li
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void initialise_joystick(void) {
	DDRA &= ~(0<<PINC5) | ~(0<<PINC6);
	// Turn on the ADC (but don't start a conversion yet). Choose a clock
	// divider of 64. (The ADC clock must be somewhere
	// between 50kHz and 200kHz. We will divide our 8MHz clock by 64
	// to give us 125kHz.)
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);
}
