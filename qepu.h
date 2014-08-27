#ifndef QEPU_H_
#define QEPU_H_

#include "qepu_constants.h"
#include "uart.h"
#include "eeprom.h"
#include "sram.h"
#include "gates.h"
#include "utils.h"

class QEPU{
	public:
		class Flag{
			public:
				Flag();
				static void compare(int data1,int data2);
				static int8_t flaglist[FLAG_COUNT];
		};
		class Jumpstack{
			public:
				Jumpstack();
				void push(int newaddress);
				int pop();
				void dumpjumpstack(); // TEMPORARY
			private:
				int newest_address_index;
				int addresses_toreturn[JUMP_STACK_SIZE];
		};
		QEPU();
		void run();
		void write(int index,int dim,int deg);
		int read(int index,int dim,bool freeze_bus);
		void dumpmem(int length);
		Flag flags;
	private:
		void interrupt_cpu(int interrupt_type);	
		void setup_seed();
		int measure(int amplitude);
		void bus_write(int8_t data);
		void setctrl(int bin);
		void setdatapin(int pin,int state);
		void setctrlpin(int pin,int state);
		void setbuffctrlpin(int pin,int state);
		int fetch_register(int address);
		void set_register(int address,int data);
		void select_qubit(int index);
		void deselect_qubit();
		void close_bus();
		void set_programcounter(int newaddress);
		void execute(int func,int32_t op1,int32_t op2,int32_t op3);
		Serial serial;
		EEProm eeprom;
		SRAM sram;
		Gates gates;
		Jumpstack jumpstack;
		Utils utils;
		int program_counter;
		int program_counter_maximum;
};
#endif