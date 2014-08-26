#include "qepu.h"
int main(void){
	/*SRAM sram;
	Serial s;
	
	sram.dumpmem(5);
	s.writestrln("");
	
	sram.push(1);
	sram.push(2);
	sram.push(3);
	sram.dumpmem(5);
	s.writestrln("");
	
	sram.pop();
	sram.pop();
	sram.dumpmem(5);
	s.writestrln("");
	
	sram.push(3);
	sram.dumpmem(5);
	*/

	QEPU qepu;
	qepu.run();
	while(1);
}