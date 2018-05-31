/*
 * countdown.c
 *
 * Created: 5/19/2018 5:15:04 PM
 *  Author: Xinyi Li
 */


#include <avr/io.h>
#include <stdio.h>

#include "countdown.h"
#include "timer0.h"

void display_digit(uint8_t digit, int cc_switch, int decimal) {	
	PORTC = digit == 0 ? 0 : digit;
	PORTC = decimal == 0 ? PORTC & ~(1<<PORTC7) : PORTC | (1<<PORTC7);
	PORTD = cc_switch == 0 ? PORTD & ~(1<<PORTD2) : PORTD | (1<<PORTD2);
}

void init_countdown() {
	DDRC = 0xFF;
	DDRD = (1<<DDRD2);
	reset_countdown();
}

void reset_countdown() {
	time_remaining_ms = 11;
	time_remaining_s = 15;
}