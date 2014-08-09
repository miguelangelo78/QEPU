#ifndef QEPU_H_
#define QEPU_H_
#define QUBIT_COUNT 4
#define QUBIT_BYTE_SIZE 4
#define FIXED_FUNC_OFFSET 0
#define FIXED_OP1_OFFSET 1
#define FIXED_OP2_OFFSET 5
#define FIXED_OP3_OFFSET 9
#define OP1N2_OFFSET 3
#define OPERAND_COUNT 3
#define OP1_WIDTH 4+OP1N2_OFFSET
#define OP2_WIDTH 4+OP1N2_OFFSET
#define OP3_WIDTH 4+OP1N2_OFFSET
#define FUNC_WIDTH 1
#define INSTR_WIDTH (FUNC_WIDTH+OP1_WIDTH+OP2_WIDTH+OP3_WIDTH)-(OP1N2_OFFSET*OPERAND_COUNT)
#define INSTR_HEIGHT 20
#define THE 0
#define PHI 1
#define MAX_NEWTHEPHI 6
#define F_CPU 14745600
#include <util/delay.h>
#include <math.h>
#include <avr/io.h>
#include <limits.h>
#include "uart.h"
#include "eeprom.h"
#include "gates.h"
class QEPU{
	public:
	class Utils{
		public:
			char* char2str(char c);
			int* str2intarr(char* c);
			int dec2hex(int dec);
			char* int2str(int dec);
			int countdigits(int dec);
			int concint(int n1,int n2);
			int delay(int ms);
			char *int2binstr(int num,int strlength);
	};
		QEPU();
		void run();
		void write(int index,int dim,int deg);
		int read(int index,int dim);
		void dumpmem();
	private:
		void setdata(int bin);
		void setctrl(int bin);
		void setdatapin(int pin,int state);
		void setctrlpin(int pin,int state);
		void execute(int func,int32_t op1,int32_t op2,int32_t op3);
		EEProm eeprom;
		Serial serial;
		Utils utils;
		int program_counter;
		Gates gates;
};
#endif