#ifndef SRAM_H_
#define SRAM_H_
#include "qepu_constants.h"
#include "utils.h"
#include "uart.h"
#include <math.h>
#include <avr/io.h>
#include <stdio.h>
class SRAM{
	public: 
		SRAM();
		int8_t read(int address);
		void write(int address,int data);
		int pop();
		void push(int register_data);
		void permissions(bool allowed);
		void dumpmem(int length);
		int stack_head_offset;
		
	private:
		Utils utils;
		Serial serial;
		void init();
		void set_address(int address);
		void memory_management();
		int* read_sram();
		void bus_write(int data);
		void setctrlpin(int pin,int state);
		int stack_tail_offset;
		bool memory_restrictedaccess_allowed;
};
#endif