#include "qepu.h"
QEPU::QEPU(){serial.writestr("INITIALIZING . . . ");setup_seed();CTRL_QB_DIR=OUTPUT;program_counter=0;program_counter_maximum=0;}

//SETUP SEED FOR RAND():
uint16_t EEMEM randinit;

void QEPU::setup_seed(){
	srand(eeprom_read_word(&randinit));
	eeprom_write_word(&randinit,rand()%10000);
}

//QUBIT MEMORY HANDLING:
void QEPU::bus_write(int8_t data){
	BUS_DIR=OUTPUT;
	BUS_OUT=data;
}
void QEPU::close_bus(){
	setctrl(0); // CLOSE ALL CONTROLS
	deselect_qubit(); // DESELECT ALL QUBITS
	bus_write(LOW);
}
void QEPU::write(int index,int dim,int deg){
	BUS_DIR=OUTPUT;
	
	bus_write(deg);
	select_qubit(index);
	setctrlpin(DIM_SEL,dim);
	setctrlpin(WRITE_QB,HIGH); // CLOCK UP LOAD
	setctrlpin(WRITE_QB,LOW); // CLOCK DOWN LOAD
	bus_write(LOW);
	deselect_qubit();
}
int QEPU::read(int index,int dim,bool freeze_bus){
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
void QEPU::deselect_qubit(){
	CTRL_BUFF_DIR=OUTPUT;
	setbuffctrlpin(EO_AB,LOW);
}
void QEPU::select_qubit(int index){
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
void QEPU::setctrl(int bin){
	CTRL_QB=bin;
}
void QEPU::setdatapin(int pin,int state){
	BUS_OUT&=~(1<<pin); // CLEAR PIN
	BUS_OUT|=(state<<pin); // SET PIN
}
void QEPU::setbuffctrlpin(int pin,int state){
	CTRL_BUFF&=~(1<<pin); // CLEAR PIN
	CTRL_BUFF|=(state<<pin); // SET PIN
}
void QEPU::setctrlpin(int pin,int state){
	CTRL_QB&=~(1<<pin); // CLEAR PIN
	CTRL_QB|=(state<<pin); // SET PIN
}
int QEPU::measure(int amplitude){
	return gates.touch(utils.custom_pow(sin(amplitude*M_PI/360),2));
}

int QEPU::fetch_register(int address){
	char data_fetched[BUS_WIDTH+1]="";
	int address_offset=address*BUS_WIDTH;
	for(int i=0;i<BUS_WIDTH;i++)
		sprintf(data_fetched,"%s%d",data_fetched,measure(read(address_offset+i,THE,false)));
	strrev(data_fetched);
	return strtol(data_fetched,NULL,2);
}

void QEPU::set_register(int address,int data){
	char* data_str=utils.int2binstr(data,BUS_WIDTH);
	strrev(data_str);
	int address_offset=address*BUS_WIDTH;
	
	for(int i=0;i<BUS_WIDTH;i++)
		if(data_str[i]=='1') write(address_offset+i,THE,180);
		else write(address_offset+i,THE,0);
}

void QEPU::dumpmem(int length){
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

void QEPU::set_programcounter(int newaddress){
	program_counter=newaddress-1;
}

void QEPU::run(){
	//TEMPORARY PRINTING FOR DEBUGGING PURPOSES
	serial.writestrln(" RUNNING "); serial.writestrln("");
	
	int line_width=INSTR_WIDTH;
	program_counter_maximum=eeprom.count_lines();
	for(int program_counter=0;program_counter<program_counter_maximum;program_counter++){
		//FETCH OPERANDS FROM THE EEPROM:
		int eeprom_line_selection=program_counter*line_width;
		int func=0;	char op1s[OP1_WIDTH]=""; char op2s[OP2_WIDTH]=""; char op3s[OP3_WIDTH]="";
		func=eeprom.read(eeprom_line_selection+FIXED_FUNC_OFFSET); // FUNCTION FETCH
		for(int k=0;k<QUBIT_BYTE_SIZE;k++){
			sprintf(op1s,"%s%x",op1s,eeprom.read(eeprom_line_selection+FIXED_OP1_OFFSET+k)); // HEX CONCAT TO STRING (OP1 FETCH)
			sprintf(op2s,"%s%x",op2s,eeprom.read(eeprom_line_selection+FIXED_OP2_OFFSET+k)); // HEX CONCAT TO STRING (OP2 FETCH)
			sprintf(op3s,"%s%x",op3s,eeprom.read(eeprom_line_selection+FIXED_OP3_OFFSET+k)); // HEX CONCAT TO STRING (OP3 FETCH)
		}
		//EXECUTE:
		execute(func,strtol(op1s,NULL,16),strtol(op2s,NULL,16),strtol(op3s,NULL,16)); //INSTRUCTION DECODE AND EXECUTE
	}
	dumpmem(QUBIT_COUNT);
	serial.writestrln("");
	sram.dumpmem(5);
	serial.writestr("The execution of the program has finished");
	#pragma endregion
}

void QEPU::execute(int func,int32_t op1,int32_t op2,int32_t op3){
	int * newthephi=(int*)malloc(sizeof(int)*MAX_NEWTHEPHI);
	//TODO: MAKE A SWITCH ON THE FUNCTION
	switch(func){
		//DATA MOVEMENT AND PROGRAM CONTROL/FLUX/IO FUNCTIONS:
		case 0x01: /*MOQ (move qubit) */
			write(op1,THE,read(op2,THE,false));
			write(op1,PHI,read(op2,PHI,false));
			break;
		case 0x02: /*MOR (move register)*/
			set_register(op1,fetch_register(op1)); // TOUCH ITSELF 1ST
			set_register(op1,fetch_register(op2)); // MOVE TO THE NEW REGISTER (IF IT NEEDS TO BE UNTOUCHED IT NEEDS TO USE MATRICES INSTEAD)
			break;
		case 0x03: /*MOM (move memory)*/
			sram.write(op1,sram.read(op2));
			break;
		case 0x04: /*STR (store)*/
			sram.write(op1,fetch_register(op2));
			break;
		case 0x05: /*LOD (load)*/
			set_register(op1,sram.read(op2));
			break;
		case 0x06: /*CRW (Constant Ram Write)*/
			sram.write(op1,op2);
			break;
		case 0x07: /*CQW* (Constant Qubit Write)*/
			set_register(op1,op2);
			break;
		case 0x08: /*POP (pop)*/
			set_register(op1,sram.pop());
			break;
		case 0x09: /*PSH (push)*/
			sram.push(fetch_register(op1));
			break;
		case 0x0A: /*CMT (constantmovtheta)*/
			write(op1,THE,op2);
			break;
		case 0x0B: /*CMP (constantmovphi)*/
			write(op1,PHI,op2);
			break;
		case 0x0C: /*CME (compare)*/
			flags.compare(fetch_register(op1),fetch_register(op2));
			break;
		case 0x0D: /*SEF (SET FLAG)*/
			flags.flaglist[op1]=op2;
			break;
		case 0x0E: /*GEF (GET FLAG)*/
			sram.write(op1,flags.flaglist[op2]);
			break;
		/*IMPLEMENT CONDICIONAL/INCONDICIONAL BRANCHES HERE*/
		case 0x0F: /*BES (branch enable stack)*/
			flags.flaglist[ALLOW_BRANCH_STACK]=op1;
			break;
		case 0x10: /*BLW (branch if lower (with jumpstack))*/
			if(flags.flaglist[CND_LWER]){
				jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x11: /*BLE (branch if lower or equal (with jumpstack))*/
			if(flags.flaglist[CND_LWER_EQUAL]){
				if(flags.flaglist[ALLOW_BRANCH_STACK])
					jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x12: /*BEQ (branch if equal) (with jumpstack)*/
			if(flags.flaglist[CND_EQUAL]){
				if(flags.flaglist[ALLOW_BRANCH_STACK])
					jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x13: /*BGE (branch if greater or equal (with jumpstack))*/
			if(flags.flaglist[CND_GRTER_EQUAL]){
				if(flags.flaglist[ALLOW_BRANCH_STACK])
					jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x14: /*BGR (branch if greater (with jumpstack))*/
			if(flags.flaglist[CND_GRTER]){
				if(flags.flaglist[ALLOW_BRANCH_STACK])
					jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x15: /*BDI (branch if different (with jumpstack))*/
			if(flags.flaglist[CND_DIFF]){
				if(flags.flaglist[ALLOW_BRANCH_STACK])
					jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x16: /*BZE (branch if register(op1)=0 (zero) (with jumpstack))*/
			if(flags.flaglist[CND_ZERO]){
				if(flags.flaglist[ALLOW_BRANCH_STACK])
					jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x17: /*BNZ (branch if register(op1)!=0 (not zero) (with jumpstack))*/
			if(flags.flaglist[CND_NOT_ZERO]){
				if(flags.flaglist[ALLOW_BRANCH_STACK])
					jumpstack.push(program_counter+1);
				set_programcounter(op1);
			}
			break;
		case 0x18: /*CALL (incondicional branch WITH jumpstack)*/
			jumpstack.push(program_counter+1);
			set_programcounter(op1);
			break;
		case 0x19: /*RET (return)*/
			set_programcounter(jumpstack.pop());
			break;
		case 0x1A: /*JMP (jump (incondicional branch WITHOUT jumpstack))*/
			set_programcounter(op1);
			break;
		/*IMPLEMENT LOGIC AND ARITHMETIC (CLASSICAL) CALCULATIONS HERE*/
		case 0x1B: /*ADD*/
			sram.write(op1,sram.read(op2)+sram.read(op3));
			break;
		case 0x1C: /*SUB*/
			sram.write(op1,sram.read(op2)-sram.read(op3));
			break;
		case 0x1D: /*MUL*/
			sram.write(op1,sram.read(op2)*sram.read(op3));
			break;
		case 0x1E: /*DIV*/
			sram.write(op1,sram.read(op2)/sram.read(op3));
			break;
		case 0x1F: /*AND*/
			sram.write(op1,sram.read(op2)&sram.read(op3));
			break;
		case 0x20: /*OR*/
			sram.write(op1,sram.read(op2)|sram.read(op3));
			break;
		case 0x21: /*NOR*/
			sram.write(op1,~(sram.read(op2)|sram.read(op3)));
			break;
		case 0x22: /*XOR*/
			sram.write(op1,sram.read(op2)^sram.read(op3));
			break;
		case 0x23: /*NAN*/
			sram.write(op1,~(sram.read(op2)&sram.read(op3)));
			break;
		case 0x24: /*NOT*/
			sram.write(op1,~sram.read(op2));
			break;
		case 0x25: /*SHL*/
			sram.write(op1,sram.read(op2)<<op3);
			break;
		case 0x26: /*SHR*/
			sram.write(op1,sram.read(op2)>>op3);
			break;
		case 0x27: /*INT (interrupt)*/ //NEEDS TABLE SYSTEM
			interrupt_cpu(op1);
			break;
		case 0x28: /*DLY (delay)*/
			utils.delay(op1);
			break;
		case 0x29: /*NOP (nop) - DOES NOTHING*/ 
			break;
		case 0x2A: /*HLT (halt)*/ 
			program_counter=program_counter_maximum;
			break;
		//QUANTUM FUNCTIONS:
		//1 QUBIT GATES -
		case 0x2B: // X GATE
			newthephi=gates.X(read(op1,THE,false),read(op1,PHI,false));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
			break;
		case 0x2C: // Y GATE
			newthephi=gates.Y(read(op1,THE,false),read(op1,PHI,false));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
			break;
		case 0x2D: // Z GATE
			newthephi=gates.Z(read(op1,THE,false),read(op1,PHI,false));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
			break;
		case 0x2E: // H GATE
			newthephi=gates.H(read(op1,THE,false),read(op1,PHI,false));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
			break;
		case 0x2F: // S GATE
			newthephi=gates.S(read(op1,THE,false),read(op1,PHI,false));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
			break;
		case 0x30: // T GATE
			newthephi=gates.T(read(op1,THE,false),read(op1,PHI,false));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
			break;
		case 0x31: // ROTATE X GATE
			newthephi=gates.ROX(read(op1,THE,false),read(op1,PHI,false),op2);
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			break;
		case 0x32: // ROTATE Y GATE
			newthephi=gates.ROY(read(op1,THE,false),read(op1,PHI,false),op2);
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			break;
		case 0x33: // ROTATE Z GATE
			newthephi=gates.ROZ(read(op1,THE,false),read(op1,PHI,false),op2);
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			break;
		//2 QUBIT GATES -
		case 0x34: // CNOT GATE
			newthephi=gates.CNO(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			break;
		case 0x35: // CSIGN GATE
			newthephi=gates.CSI(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			break;
		case 0x36: // SWAP GATE
			newthephi=gates.SWA(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			break;
		case 0x37: // INCREMENT GATE
			newthephi=gates.INC(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			break;
		case 0x38: // DECREMENT GATE
			newthephi=gates.DEC(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			break;
		case 0x39: // SWAGSQ GATE
			newthephi=gates.SWQ(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			break;
		case 0x3A: // SWAPI GATE
			newthephi=gates.SWI(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			break;
		//3 QUBIT GATES -
		case 0x3B: // CONTROL SWAP GATE
			newthephi=gates.CSW(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false),read(op3,THE,false),read(op3,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			write(op3,THE,newthephi[4]); write(op3,PHI,newthephi[5]);
			break;
		case 0x3C: // TOFFOLI GATE
			newthephi=gates.TOF(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false),read(op3,THE,false),read(op3,PHI,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			write(op3,THE,newthephi[4]); write(op3,PHI,newthephi[5]);
			break;
		case 0x3D: // DEUTSCH GATE
			newthephi=gates.DEU(read(op1,THE,false),read(op1,PHI,false),read(op2,THE,false),read(op2,PHI,false),read(op3,THE,false),read(op3,PHI,false),read(0,THE,false));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
			write(op3,THE,newthephi[4]); write(op3,PHI,newthephi[5]);
			break;
		default: /*THIS FUNCTION DOES NOT EXIST*/
			break;
	}
	
	if(false){
		serial.writestr("Function: "); serial.writestr(utils.int2str(func));
		serial.writestr(" , OP1: ");   serial.writestr(utils.int2str(op1));
		serial.writestr(" , OP2: ");   serial.writestr(utils.int2str(op2));
		serial.writestr(" PC: ");	   serial.writestrln(utils.int2str(program_counter));
		serial.writestrln("");
	}
	free(newthephi);	
}