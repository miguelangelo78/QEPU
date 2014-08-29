#include "qepu.h"

void QEPU::interrupt_cpu(int interrupt_type){
	//NEEDS RESERVED REGISTERS USED FOR ADDRESS POINTING -> ADDRESS SHOULD POINT TO A REGISTER ADDRESS,NOT SRAM
	//NEEDS RESERVED REGISTERS USED FOR DATA SAVING -> SHOULD SAVE ON A REGISTER,NOT SRAM
	int address_pointer_offset=0;
	while(!interrupt_signal){
		switch(interrupt_type){
			case INT_UART_OUT_CHAR:
				serial.write(sram.read(fetch_register(INT_RESERVED_ADDRESSPOINTER)));
				interrupt_done();
				break;
			case INT_UART_OUT_BUFFER:
				{
					int data_read=sram.read(fetch_register(INT_RESERVED_ADDRESSPOINTER)+address_pointer_offset);
					if(data_read==STRING_TERMINATOR) interrupt_done();
					else{
						serial.write(data_read);
						address_pointer_offset++;
					}
				}
				break;
			case INT_UART_IN_CHAR_NO_ECHO:
				break;
			case INT_UART_IN_CHAR_ECHO:
				break;
			case INT_UART_IN_BUFF_NO_ECHO:
				break;
			case INT_UART_IN_BUFF_ECHO:
				break;
		}
	}
	interrupt_signal=false;
}

void QEPU::interrupt_done(){
	interrupt_signal=true;
}