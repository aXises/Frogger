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
