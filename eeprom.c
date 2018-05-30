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

void write_eeprom_score(uint32_t score, uint8_t index) {
	while(EECR & (1<<EEPE));
	int8_t interrupts_on = bit_is_set(SREG, SREG_I);
	cli();
	
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
	
	eeprom_update_dword((uint32_t*) (index * 32) + offset_s, score);
	
	if (interrupts_on) {
		sei();
	}
}

void read_eeprom(void) {
	while(EECR & (1<<EEPE));
	EECR |= (1<<EERE);
	uint8_t name[12] = "temp";
	uint32_t score;
	move_cursor(10, 22);
	printf("High scores: \n");
	for (int i = 0; i < 5; i++) {
		eeprom_read_block((void*) name, (void*) (i * 12) + offset, 12);
		score = eeprom_read_dword((uint32_t*) (i * 32) + offset_s);
		move_cursor(10,23+i);
		printf("%s: %lu \n", name, score);
	}
}