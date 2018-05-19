/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include "score.h"
#include <stdio.h>

uint32_t score;

void init_score(void) {
	score = 0;
}

void add_to_score(uint16_t value) {
	score += value;
	printf("\n %15d \n", score);
}

uint32_t get_score(void) {
	return score;
}
