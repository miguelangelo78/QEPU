#ifndef _eeprom_H_
#define _eeprom_H_
#include "qepu_constants.h"
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
class EEProm{
	public:
		EEProm();
		uint8_t write(uint16_t address,uint8_t data);
		uint8_t read(uint16_t address);
		char* readall();
		int count_lines();
		int * get_line(int line);
};
#endif