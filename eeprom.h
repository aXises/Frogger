/*
 * eeprom.h
 *
 * Created: 5/30/2018 4:58:10 PM
 *  Author: Xinyi Li
 */ 


void write_eeprom(uint8_t name[12], uint32_t score, uint8_t index);
void compare_and_update(uint32_t current_score);
void read_eeprom(void);

