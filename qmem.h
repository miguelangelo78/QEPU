#ifndef QMEM_H_
#define QMEM_H_
#include "qepu_constants.h"
#include "gates.h"
#include "uart.h"
#include "utils.h"
#include <inttypes.h>

class QMEM{
	public:
		QMEM();
		void write(int index,int dim,int deg);
		int read(int index,int dim,bool freeze_bus);
		void dumpmem(int length);
		int fetch_register(int address);
		void set_register(int address,int data);
	private:
		void interrupt_cpu(int interrupt_type);
		void interrupt_done();
		void setup_seed();
		int measure(int amplitude);
		void bus_write(int8_t data);
		void setctrl(int bin);
		void setdatapin(int pin,int state);
		void setctrlpin(int pin,int state);
		void setbuffctrlpin(int pin,int state);
		void select_qubit(int index);
		void deselect_qubit();
		void close_bus();
		Gates gates;
		Serial serial;
		Utils utils;
};
#endif