/*
 * project.h
 *
 * Created: 5/19/2018 5:33:36 PM
 *  Author: Xinyi Li
 */ 

#define STARTING_LIVES 3;

// Global variables
// Initial lives of the player
uint8_t current_life;
// Determine whether if the player is on the same game with reduced lives or in a fresh start.
int on_same_game;
// Whether if the game is paused
int paused;