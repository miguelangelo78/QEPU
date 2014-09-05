#include "qmem.h"
QMEM::QMEM(){CTRL_QB_DIR=OUTPUT;}
void QMEM::bus_write(int8_t data){
	BUS_DIR=OUTPUT;
	BUS_OUT=data;
}
void QMEM::close_bus(){
	setctrl(0); // CLOSE ALL CONTROLS
	deselect_qubit(); // DESELECT ALL QUBITS
	bus_write(LOW);
}
void QMEM::write(int index,int dim,int deg){
	BUS_DIR=OUTPUT;
	
	bus_write(deg);
	select_qubit(index);
	setctrlpin(DIM_SEL,dim);
	setctrlpin(WRITE_QB,HIGH); // CLOCK UP LOAD
	setctrlpin(WRITE_QB,LOW); // CLOCK DOWN LOAD
	bus_write(LOW);
	deselect_qubit();
}
int QMEM::read(int index,int dim,bool freeze_bus){
	BUS_DIR=INPUT;
	
	select_qubit(index);
	//OPEN ALL AND GATES
	setctrlpin(5,1); // CLOCK UP READ
	setctrlpin(4,dim);
	int deg_read=BUS_IN; // READ INPUT
	//CLOSE ALL AND GATES
	BUS_DIR=OUTPUT;
	if(!freeze_bus){
		setctrl(0); // CLOSE ALL CONTROLS
		deselect_qubit();
	}
	//TODO: TOUCH QUBIT
	
	return deg_read;
}
void QMEM::deselect_qubit(){
	CTRL_BUFF_DIR=OUTPUT;
	setbuffctrlpin(EO_AB,LOW);
}
void QMEM::select_qubit(int index){
	int address_buffer[ADDRESS_BUFFER_SIZE];
	int mask_bit_addressbuff=0b1111;
	for(int i=0;i<ADDRESS_BUFFER_SIZE;i++){
		address_buffer[i]=(index&mask_bit_addressbuff)>>(4*i);
		mask_bit_addressbuff<<=4;
	}
	CTRL_QB_DIR=OUTPUT;
	CTRL_BUFF_DIR=OUTPUT;
	
	setctrl(address_buffer[0]);
	setbuffctrlpin(IN_ABBUFFER_START,HIGH);
	setbuffctrlpin(IN_ABBUFFER_CLOCK,HIGH);
	setbuffctrlpin(IN_ABBUFFER_CLOCK,LOW);
	setbuffctrlpin(IN_ABBUFFER_START,LOW);
	utils.delay(ADDRESS_WRITE_DELAY);
	for(int i=1;i<ADDRESS_BUFFER_SIZE;i++){
		setbuffctrlpin(IN_ABBUFFER_CLOCK,HIGH);
		setbuffctrlpin(IN_ABBUFFER_CLOCK,LOW);
		setctrl(address_buffer[i]);
		utils.delay(ADDRESS_WRITE_DELAY);
	}
	setbuffctrlpin(IN_ABBUFFER_CLOCK,HIGH);
	setbuffctrlpin(IN_ABBUFFER_CLOCK,LOW);
	setbuffctrlpin(EO_AB,HIGH);
}
void QMEM::setctrl(int bin){
	CTRL_QB=bin;
}
void QMEM::setdatapin(int pin,int state){
	BUS_OUT&=~(1<<pin); // CLEAR PIN
	BUS_OUT|=(state<<pin); // SET PIN
}
void QMEM::setbuffctrlpin(int pin,int state){
	CTRL_BUFF&=~(1<<pin); // CLEAR PIN
	CTRL_BUFF|=(state<<pin); // SET PIN
}
void QMEM::setctrlpin(int pin,int state){
	CTRL_QB&=~(1<<pin); // CLEAR PIN
	CTRL_QB|=(state<<pin); // SET PIN
}
int QMEM::measure(int amplitude){
	return gates.touch(utils.custom_pow(sin(amplitude*M_PI/360),2));
}

int QMEM::fetch_register(int address){
	char data_fetched[BUS_WIDTH+1]="";
	int address_offset=address*BUS_WIDTH;
	for(int i=0;i<BUS_WIDTH;i++)
	sprintf(data_fetched,"%s%d",data_fetched,measure(read(address_offset+i,THE,false)));
	
	strrev(data_fetched);
	return strtol(data_fetched,NULL,2);
}

void QMEM::set_register(int address,int data){
	char* data_str=utils.int2binstr(data,BUS_WIDTH);
	strrev(data_str);
	int address_offset=address*BUS_WIDTH;
	
	for(int i=0;i<BUS_WIDTH;i++)
	if(data_str[i]=='1') write(address_offset+i,THE,180);
	else write(address_offset+i,THE,0);
}

void QMEM::dumpmem(int length){
	serial.writestrln("");
	serial.writestrln("** QUANTUM MEMORY DUMP **");
	for(int i=0;i<length;i++){
		char * thedim=(char*)malloc(sizeof(char)*3); sprintf(thedim,"%d",read(i,THE,false));
		char * phidim=(char*)malloc(sizeof(char)*3); sprintf(phidim,"%d",read(i,PHI,false));
		char * qindex=(char*)malloc(sizeof(char));	 sprintf(qindex,"%d",i);
		serial.writestr("Q"); serial.writestr(qindex); serial.writestr(": Theta - ");
		serial.writestr(thedim);
		serial.writestr(" , Phi - ");
		serial.writestrln(phidim);
	}
}