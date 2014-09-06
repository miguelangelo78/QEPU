#include "qepu.h"

QEPU::Jumpstack::Jumpstack():newest_address_index(0){
	
}
void QEPU::Jumpstack::push(int newaddress){
	addresses_toreturn[newest_address_index]=newaddress;
	newest_address_index++;
}
int QEPU::Jumpstack::pop(){
	if(newest_address_index<=0) return MAX_THEORETICAL_PC;
	int address_returning=addresses_toreturn[newest_address_index-1];
	addresses_toreturn[newest_address_index-1]=0;
	newest_address_index--;
	return address_returning;
}

void QEPU::Jumpstack::dumpjumpstack(){
	Serial s;
	for(int i=0;i<newest_address_index;i++){
		char msg [15];
		sprintf(msg,"i: %d address: %d",i,addresses_toreturn[i]);
		s.writestrln(msg);
	}
}