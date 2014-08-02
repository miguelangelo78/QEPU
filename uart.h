#ifndef UART_H_
#define UART_H_
#include <avr/io.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
class Serial{
	public:
		Serial();
		char read();
		void write(char data);
		void writestr(char * data);
		void writeln(char data);
		void writestrln(char * data);
};
#endif