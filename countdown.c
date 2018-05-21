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
	PORTA = digit == 0 ? 0 : digit;
	PORTA = decimal == 0 ? PORTA & ~(1<<PORTA7) : PORTA | (1<<PORTA7);
	PORTC = cc_switch == 0 ? PORTC & ~(1<<PORTC5) : PORTC | (1<<PORTC5);
}

void init_countdown() {
	DDRA = 0xFF;
	reset_countdown();
}

void reset_countdown() {
	time_remaining_ms = 10;
	time_remaining_s = 15;
}