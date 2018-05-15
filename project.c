/*
 * FroggerProject.c
 *
 * Main file
 *
 * Author: Peter Sutton. Modified by <YOUR NAME HERE>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <math.h>

#include "ledmatrix.h"
#include "scrolling_char_display.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "score.h"
#include "timer0.h"
#include "game.h"

#define F_CPU 8000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void splash_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);
void init_life(void);
void set_life(uint8_t life);

// ASCII code for Escape character
#define ESCAPE_CHAR 27


// Global variables
uint8_t current_life = 3;
int on_same_game;

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	splash_screen();
	
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	init_timer0();
	init_life();
	// Turn on global interrupts
	sei();
}

void splash_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_cursor(10,10);
	printf_P(PSTR("Frogger"));
	move_cursor(10,12);
	printf_P(PSTR("CSSE2010/7201 project by Xinyi Li"));
	
	// Output the scrolling message to the LED matrix
	// and wait for a push button to be pushed.
	ledmatrix_clear();
	while(1) {
		set_scrolling_display_text("FROGGER S4478355", COLOUR_GREEN);
		// Scroll the message until it has scrolled off the 
		// display or a button is pushed
		while(scroll_display()) {
			_delay_ms(150);
			if(button_pushed() != NO_BUTTON_PUSHED) {
				return;
			}
		}
	}
}

void init_life(void) {
	DDRC = 0b00000111;
}

void set_life(uint8_t life) {
	uint8_t new_life = 0;
	for (int i = 0; i < life; i++) {
		new_life += i >= 2 ? pow(2, i) + 1 : pow(2, i);
	}
	PORTC = DDRC & new_life;
}

void new_game(void) {

	// Initialise the game and display
	initialise_game();
	
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the score
	init_score();
	
	if (!on_same_game) {
		current_life = 3;
		set_life(current_life);
		
	}
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	uint32_t current_time, last_move_time;
	int8_t button;
	char serial_input, escape_sequence_char;
	uint8_t characters_into_escape_sequence = 0;
	
	// Get the current time and remember this as the last time the vehicles
	// and logs were moved.
	current_time = get_current_time();
	last_move_time = current_time;
	
	// We play the game while the frog is alive and we haven't filled up the 
	// far riverbank
	
	// Setup array of counters;
	int counters[5] = {0, 0, 0, 0, 0};
	while(!is_frog_dead() && !is_riverbank_full()) {
		if(!is_frog_dead() && frog_has_reached_riverbank()) {
			// Frog reached the other side successfully but the
			// riverbank isn't full, put a new frog at the start
			put_frog_in_start_position();
		}
		
		// Check for input - which could be a button push or serial input.
		// Serial input may be part of an escape sequence, e.g. ESC [ D
		// is a left cursor key press. At most one of the following three
		// variables will be set to a value other than -1 if input is available.
		// (We don't initalise button to -1 since button_pushed() will return -1
		// if no button pushes are waiting to be returned.)
		// Button pushes take priority over serial input. If there are both then
		// we'll retrieve the serial input the next time through this loop
		serial_input = -1;
		escape_sequence_char = -1;
		button = button_pushed();
		
		if(button == NO_BUTTON_PUSHED) {
			// No push button was pushed, see if there is any serial input
			if(serial_input_available()) {
				// Serial data was available - read the data from standard input
				serial_input = fgetc(stdin);
				// Check if the character is part of an escape sequence
				if(characters_into_escape_sequence == 0 && serial_input == ESCAPE_CHAR) {
					// We've hit the first character in an escape sequence (escape)
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 1 && serial_input == '[') {
					// We've hit the second character in an escape sequence
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 2) {
					// Third (and last) character in the escape sequence
					escape_sequence_char = serial_input;
					serial_input = -1;  // Don't further process this character - we
										// deal with it as part of the escape sequence
					characters_into_escape_sequence = 0;
				} else {
					// Character was not part of an escape sequence (or we received
					// an invalid second character in the sequence). We'll process 
					// the data in the serial_input variable.
					characters_into_escape_sequence = 0;
				}
			}
		}
		
		// Process the input. 
		if(button==3 || escape_sequence_char=='D' || serial_input=='L' || serial_input=='l') {
			// Attempt to move left
			move_frog_to_left();
		} else if(button==2 || escape_sequence_char=='A' || serial_input=='U' || serial_input=='u') {
			// Attempt to move forward
			move_frog_forward();
		} else if(button==1 || escape_sequence_char=='B' || serial_input=='D' || serial_input=='d') {
			// Attempt to move down
			move_frog_backward();
		} else if(button==0 || escape_sequence_char=='C' || serial_input=='R' || serial_input=='r') {
			// Attempt to move right
			move_frog_to_right();
		} else if(serial_input == 'p' || serial_input == 'P') {
			// Unimplemented feature - pause/unpause the game until 'p' or 'P' is
			// pressed again
		} 
		// else - invalid input or we're part way through an escape sequence -
		// do nothing
		
		//uint32_t offsets[] = {750, 860, 1000, 1180, 1300};
		current_time = get_current_time();
	
		if(!is_frog_dead() && current_time >= last_move_time + 100) {
			// 1000ms (1 second) has passed since the last time we moved
			// the vehicles and logs - move them again and keep track of
			// the time when we did this.
			if (counters[0] > 10) {
				scroll_vehicle_lane(0, 1);
				counters[0] = 0;
			}
			if (counters[1] > 13) {
				scroll_vehicle_lane(1, -1);
				counters[1] = 0;
			}
			if (counters[2] > 8) {
				scroll_vehicle_lane(2, 1);
				counters[2] = 0;
			}
			if (counters[3] > 9) {
				scroll_river_channel(0, -1);
				counters[3] = 0;
			}
			if (counters[4] > 11) {
				scroll_river_channel(1, 1);
				counters[4] = 0;
			}
			for (int i = 0; i < (sizeof(counters) / sizeof(int)); i++) {
				counters[i]++;
			}
			last_move_time = current_time;
		}
	}
	// We get here if the frog is dead or the riverbank is full
	// The game is over.
}

void handle_game_over() {
	current_life--;
	set_life(current_life);
	on_same_game = 1;
	printf("\n %i lives remaining \n", (int) current_life + 1);
	if (current_life <= 0) {
		current_life = 3;
		on_same_game = 0;
		move_cursor(10,14);
		printf_P(PSTR("GAME OVER"));
		move_cursor(10,15);
		printf_P(PSTR("Press a button to start again"));
		while(button_pushed() == NO_BUTTON_PUSHED) {
			; // wait
		}
	}
}