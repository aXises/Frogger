/*
 * sound.c
 *
 * Created: 5/27/2018 6:18:13 PM
 *  Author: Xinyi Li
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL	// 8MHz
#include <util/delay.h>
#include <stdio.h>

#include "timer0.h"
#include "project.h"

int global_disable_sound = 0;
int sound_on = 0;
int sound_quiet = 0;
int playing_sound = 0;
uint32_t current_time;
uint32_t last_play_time;

uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);	// UL makes the constant an unsigned long (32 bits)
	// and ensures we do 32 bit arithmetic, not 16
}

// Return the width of a pulse (in clock cycles) given a duty cycle (%) and
// the period of the clock (measured in clock cycles)
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

void enable_sound(void) {
	TCCR1A |= (1 << COM1B1);
	TCCR1A &= ~(1 <<COM1B0);
	playing_sound = 1;
}

void disable_sound(void) {
	TCCR1A &= ~(1 << COM1B1);
	TCCR1A &= ~(1 <<COM1B0);
	playing_sound = 0;
}

void init_sound(void) {
	DDRD |= (1 << PORTD4);
	DDRD &= ~(1 << PIND3) | ~(1 << PIND5);
	PCICR |= (1 << PCIE3);
	PCMSK3 = (1 << PCINT27) | (1 << PCINT29);
}

uint16_t clockperiod;
uint16_t pulsewidth;
float dutycycle = 2;
void play_sound(uint16_t freq, uint32_t duration) {
	sound_on = bit_is_set(PIND, PIND3) == 8 ? 1 : 0;
	sound_quiet = bit_is_set(PIND, PIND5) == 32 ? 1 : 0;
	if (!paused) {
		if (sound_quiet && sound_on) {
			pulsewidth = duty_cycle_to_pulse_width(0.1, clockperiod);
		}
		else if (sound_on) {
			pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
		}
		else {
			pulsewidth = duty_cycle_to_pulse_width(0, clockperiod);
		}
	}
	clockperiod = freq_to_clock_period(freq);


	// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
	OCR1A = clockperiod - 1;
	TCCR1A = (1 <<WGM11) | (1 << WGM10);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10);
	TIMSK1 |= (1 << OCIE1A);
	TIFR1 &= (1 << OCF1A);
	
	enable_sound();

	current_time = get_current_time();
	last_play_time = get_current_time() + duration;
	
}

uint32_t hold_time = 0;

ISR(TIMER1_COMPA_vect) {
	if (playing_sound) {
		if (paused) {
			OCR1B = 0;
			current_time = get_current_time();
			last_play_time = get_current_time() + hold_time;
		} else {
			if (pulsewidth > 0) {
				OCR1B = pulsewidth - 1;
				} else {
				OCR1B = 0;
			}
			current_time = get_current_time();
			hold_time = last_play_time - current_time;
		}

		if (current_time >= last_play_time) {
			disable_sound();
			last_play_time = current_time;
		}
	}


}

ISR(PCINT3_vect) {
	sound_on = bit_is_set(PIND, PIND3) == 8 ? 1 : 0;
	sound_quiet = bit_is_set(PIND, PIND5) == 32 ? 1 : 0;
	if (!paused) {
		if (sound_quiet && sound_on) {
			pulsewidth = duty_cycle_to_pulse_width(0.15, clockperiod);
		}
		else if (sound_on)
			pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
		else
			pulsewidth = duty_cycle_to_pulse_width(0, clockperiod);
	}
}