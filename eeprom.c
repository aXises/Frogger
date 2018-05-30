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

void write_eeprom(uint8_t name[12], uint32_t score, uint8_t index) {
	write_eeprom_name(name, index);
	write_eeprom_score(score, index);
}

void compare_and_update(uint32_t current_score) {
	while(EECR & (1<<EEPE));
	EECR |= (1<<EERE);
	uint32_t score;
	uint32_t replacables[5][2] = {
		{0 ,0},
		{0 ,0},
		{0 ,0},
		{0 ,0},
		{0 ,0}
	};
	int j = 0; 
	for (int i = 0; i < 5; i++) {
		score = eeprom_read_dword((uint32_t*) (i * 32) + offset_s);
		if (current_score > score) {
			replacables[j][0] = i;
			replacables[j][1] = score;
			j++; 
		}
	}
	uint32_t lowest_score[2] = {0, 0};
	for (int i = 0; i < sizeof(replacables) / sizeof(replacables[0]); i++) {
		if (replacables[i][1] != 0) {
			if (replacables[i][1] < lowest_score[1] || lowest_score[1] == 0) {
				lowest_score[0] = replacables[i][0];
				lowest_score[1] = replacables[i][1];
			}
		}
	}
	if (lowest_score[1] != 0) {
		uint8_t *name = request_name();
		if (name != '\0')
			write_eeprom(name, current_score, lowest_score[0]);
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