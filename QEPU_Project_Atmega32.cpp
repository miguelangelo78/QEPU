#include "qepu.h"
int main(void){
	SRAM sram;
	
	sram.push(1);
	sram.push(2);
	sram.dumpmem(5);
	sram.pop();
	sram.pop();
	sram.dumpmem(5);
	sram.push(3);
	sram.dumpmem(5);
	//QEPU qepu;
	//qepu.run();
	while(1);
}