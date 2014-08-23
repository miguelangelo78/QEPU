#ifndef _eeprom_H_
#define _eeprom_H_
#include "qepu_constants.h"
#define MAX_MEM_SIZE 100
#define FALSE 0
#define TRUE 1
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
class EEProm{
	public:
		EEProm();
		uint8_t write(uint16_t address,uint8_t data);
		uint8_t read(uint16_t address);
		char* readall();
};
#endif