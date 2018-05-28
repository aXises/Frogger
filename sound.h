/*
 * sound.h
 *
 * Created: 5/27/2018 6:25:02 PM
 *  Author: Xinyi Li
 */ 

void init_sound(void);
void play_sound(uint16_t freq, uint32_t duration);
volatile int allow_sound;