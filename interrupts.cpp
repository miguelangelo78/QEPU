#include "qepu.h"

void QEPU::interrupt_cpu(int interrupt_type){
	//NEEDS RESERVED REGISTERS USED FOR ADDRESS POINTING -> ADDRESS SHOULD POINT TO A REGISTER ADDRESS,NOT SRAM
	//NEEDS RESERVED REGISTERS USED FOR DATA SAVING -> SHOULD SAVE ON A REGISTER,NOT SRAM
	int address_pointer_offset=0;
	int data_out_registerpointer=qmem.fetch_register(INT_RESERVED_ADDRESSPOINTER);
	int data_in_registerpointer=qmem.fetch_register(INT_RESERVED_ADDRESSDATASTORE);
	while(!interrupt_signal){
		switch(interrupt_type){
			case INT_UART_OUT_CHAR:
				serial.write(sram.read(data_out_registerpointer));
				interrupt_done();
				break;
			case INT_UART_OUT_BUFFER:
				{
					int data_read=sram.read(data_out_registerpointer+address_pointer_offset);
					if(data_read==STRING_TERMINATOR) interrupt_done();
					else{
						serial.write(data_read);
						address_pointer_offset++;
					}
				}
				break;
			case INT_UART_IN_CHAR_NO_ECHO:
				sram.write(data_in_registerpointer+address_pointer_offset,serial.read());
				interrupt_done();
				break;
			case INT_UART_IN_CHAR_ECHO:{
					int char_echo=serial.read();
					serial.write(char_echo);
					sram.write(data_in_registerpointer+address_pointer_offset,char_echo);
					interrupt_done();
				}
				break;
			case INT_UART_IN_BUFF_NO_ECHO:{
					int char_noecho=serial.read();
					if(char_noecho==STRING_NEWLINE){
						interrupt_done();
						sram.write(data_in_registerpointer+address_pointer_offset,STRING_TERMINATOR);
					}else
						if(char_noecho==0x8 && address_pointer_offset>0) address_pointer_offset--;
						else{
							sram.write(data_in_registerpointer+address_pointer_offset,char_noecho);
							address_pointer_offset++;
						}
				}
				break;
			case INT_UART_IN_BUFF_ECHO:{
					int char_echo=serial.read();
					serial.write(char_echo);
					if(char_echo==STRING_NEWLINE){
						interrupt_done();
						sram.write(data_in_registerpointer+address_pointer_offset,STRING_TERMINATOR);
					}else 
						if(char_echo==0x8 && address_pointer_offset>0) address_pointer_offset--;
						else{
							sram.write(data_in_registerpointer+address_pointer_offset,char_echo);
							address_pointer_offset++;
						}
				}
				break;
		}
	}
	interrupt_signal=false;
}

void QEPU::interrupt_done(){
	interrupt_signal=true;
}