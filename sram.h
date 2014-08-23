#ifndef SRAM_H_
#define SRAM_H_
#include "qepu_constants.h"
#include <math.h>
#include <avr/io.h>
#include <stdio.h>
class SRAM{
	public: 
		SRAM();
		int8_t read(int8_t address);
		void write(int8_t address,int8_t data);
	private:
		void bus_write(int8_t data);
		void setctrlpin(int pin,int state);
};
#endif