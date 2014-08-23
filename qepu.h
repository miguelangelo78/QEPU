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
		QEPU();
		void run();
		void write(int index,int dim,int deg);
		int read(int index,int dim,bool freeze_bus);
		void dumpmem();
	private:
		void bus_write(int8_t data);
		void setctrl(int bin);
		void setdatapin(int pin,int state);
		void setctrlpin(int pin,int state);
		void setbuffctrlpin(int pin,int state);
		void select_qubit(int index);
		void deselect_qubit();
		void close_bus();
		void execute(int func,int32_t op1,int32_t op2,int32_t op3);
		EEProm eeprom;
		Serial serial;
		SRAM sram;
		int program_counter;
		Gates gates;
		Utils utils;
};
#endif