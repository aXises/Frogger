/*
 * countdown.h
 *
 * Created: 5/19/2018 5:19:40 PM
 *  Author: Xinyi Li
 */ 

void display_digit(uint8_t digit, int cc_switch, int decimal);

void init_countdown();

void reset_countdown();

// Global variables;
uint32_t time_remaining_s;
uint32_t time_remaining_ms;