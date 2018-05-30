/*
 * eeprom.c
 *
 * Created: 5/30/2018 4:57:36 PM
 *  Author: Xinyi Li
 */ 

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "eeprom.h"
#include "terminalio.h"
#include "project.h"

uint32_t offset = 50;
uint32_t offset_s = 150;

void write_eeprom_name(uint8_t name[12], uint8_t index) {
	while(EECR & (1<<EEPE));
	int8_t interrupts_on = bit_is_set(SREG, SREG_I);
	cli();
	
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
	
	eeprom_update_block((void*) name, (void*) (index * 12) + offset, 12);
	
	if (interrupts_on) {
		sei();
	}
}