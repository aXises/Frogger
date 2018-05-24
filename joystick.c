#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "joystick.h"
#include "serialio.h"
#include "timer0.h"
#include "game.h"

uint16_t value;
uint8_t x_or_y = 0;	/* 0 = x, 1 = y */
uint16_t x = 500;
uint16_t y = 500;
uint32_t current_time, last_move_time; 
int init_push = 1;

void init_joy_interrupts(void) {
	DDRA |= (0<<PINC5) | (0<<PINC6);
	// Turn on the ADC (but don't start a conversion yet). Choose a clock
	// divider of 64. (The ADC clock must be somewhere
	// between 50kHz and 200kHz. We will divide our 8MHz clock by 64
	// to give us 125kHz.)
	current_time = get_current_time();
	last_move_time = current_time;
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADIE);
	ADCSRA |= (1<<ADSC);
}
