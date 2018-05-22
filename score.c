/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include "score.h"
#include <stdio.h>
#include "terminalio.h"

uint32_t score;

void init_score(void) {
	score = 0;
}

void add_to_score(uint16_t value) {
	score += value;
	move_cursor(10,15);
	printf("\n %30s : %lu \n", "Score", score);
}

uint32_t get_score(void) {
	return score;
}
