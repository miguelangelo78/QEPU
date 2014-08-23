#include "sram.h"

SRAM::SRAM(){}
	
int8_t SRAM::read(int8_t address){
	MEM_ADDRESS_DIR=OUTPUT;
	MEM_ADDRESS=address;
	BUS_DIR=INPUT;
	setctrlpin(MRE,HIGH);
	int bus_data=BUS_IN;
	setctrlpin(MRE,LOW);
	BUS_DIR=OUTPUT;
	MEM_ADDRESS=LOW;
	return bus_data;
}
void SRAM::write(int8_t address,int8_t data){
	MEM_ADDRESS_DIR=OUTPUT;
	MEM_ADDRESS=address;
	bus_write(data);
	setctrlpin(MWR,HIGH);
	setctrlpin(MWR,LOW);
	MEM_ADDRESS=LOW;
	bus_write(LOW);
}
void SRAM::bus_write(int8_t data){
	BUS_DIR=OUTPUT;
	BUS_OUT=data;
}
void SRAM::setctrlpin(int pin,int state){
	CTRLMEM&=~(1<<pin); // CLEAR PIN
	CTRLMEM|=(state<<pin); // SET PIN
}