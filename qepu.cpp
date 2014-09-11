#include "qepu.h"
QEPU::QEPU(){serial.writestr("INITIALIZING . . . ");setup_seed();program_counter=0;program_counter_maximum=0;}

//SETUP SEED FOR RAND():
uint16_t EEMEM randinit;

void QEPU::setup_seed(){
	srand(eeprom_read_word(&randinit));
	eeprom_write_word(&randinit,rand()%10000);
}

void QEPU::set_programcounter(int newaddress){
	program_counter=newaddress-1;
}

void QEPU::run(){
	//TEMPORARY PRINTING FOR DEBUGGING PURPOSES
	serial.writestrln(" RUNNING "); serial.writestrln("");
	
	int line_width=INSTR_WIDTH;
	program_counter_maximum=eeprom.count_lines();
	for(program_counter=0;program_counter<program_counter_maximum;program_counter++){
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
	
	if(SHOW_LAST_STATE){
		dumpmem(QUBIT_COUNT);
		serial.writestrln("");
		sram.dumpmem(5);
	}
	serial.writestrln("");
	serial.writestr("The execution of the program has finished");
	#pragma endregion
}

void QEPU::execute(int func,int32_t op1,int32_t op2,int32_t op3){
	//TODO: MAKE A SWITCH ON THE FUNCTION
	switch(func){
		//DATA MOVEMENT AND PROGRAM CONTROL/FLUX/IO FUNCTIONS:
		case 0x01: /*MOQ (move qubit) */
			qmem.write(op1,THE,qmem.read(op2,THE,false));
			qmem.write(op1,PHI,qmem.read(op2,PHI,false));
			break;
		case 0x02: /*MOR (move register)*/
			qmem.set_register(op1,qmem.fetch_register(op1)); // TOUCH ITSELF 1ST
			qmem.set_register(op1,qmem.fetch_register(op2)); // MOVE TO THE NEW REGISTER (IF IT NEEDS TO BE UNTOUCHED IT NEEDS TO USE MATRICES INSTEAD)
			break;
		case 0x03: /*MOM (move memory)*/
			sram.write(op1,sram.read(op2));
			break;
		case 0x04: /*STR (store)*/
			sram.write(op1,qmem.fetch_register(op2));
			break;
		case 0x05: /*LOD (load)*/
			qmem.set_register(op1,sram.read(op2));
			break;
		case 0x06: /*CRW (Constant Ram Write)*/
			sram.write(op1,op2);
			break;
		case 0x07: /*CQW* (Constant Qubit Write)*/
			qmem.set_register(op1,op2);
			break;
		case 0x08: /*POP (pop)*/
			qmem.set_register(op1,sram.pop());
			break;
		case 0x09: /*PSH (push)*/
			sram.push(qmem.fetch_register(op1));
			break;
		case 0x0A: /*CMT (constantmovtheta)*/
			qmem.write(op1,THE,op2);
			break;
		case 0x0B: /*CMP (constantmovphi)*/
			qmem.write(op1,PHI,op2);
			break;
		case 0x0C: /*CME (compare)*/
			flags.compare(qmem.fetch_register(op1),qmem.fetch_register(op2));
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
		case 0x1C: /*ADD RK (add reg const)*/
			sram.write(op1,sram.read(op2)+op3);
			break;
		case 0x1D: /*SUB*/
			sram.write(op1,sram.read(op2)-sram.read(op3));
			break;
		case 0x1E: /*SUB RK (sub reg const)*/
			sram.write(op1,sram.read(op2)-op3);
			break;
		case 0x1F: /*SUB KR (sub const reg)*/
			sram.write(op1,op2-sram.read(op3));
			break;
		case 0x20: /*MUL*/
			sram.write(op1,sram.read(op2)*sram.read(op3));
			break;
		case 0x21: /*MUL RK (mul reg const)*/
			sram.write(op1,sram.read(op2)*op3);
			break;
		case 0x22: /*DIV*/
			sram.write(op1,sram.read(op2)/sram.read(op3));
			break;
		case 0x23: /*DIV RK (div reg const)*/
			sram.write(op1,sram.read(op2)/op3);
			break;
		case 0x24: /*DIV KR (div const reg)*/
			sram.write(op1,op2/sram.read(op3));
			break;
		case 0x25: /*AND*/
			sram.write(op1,sram.read(op2)&sram.read(op3));
			break;
		case 0x26: /*AND RK (and reg const)*/
			sram.write(op1,sram.read(op2)&op3);
			break;
		case 0x27: /*AND KR (and const reg)*/
			sram.write(op1,op2&sram.read(op3));
			break;
		case 0x28: /*OR*/
			sram.write(op1,sram.read(op2)|sram.read(op3));
			break;
		case 0x29: /*OR RK (or reg const)*/
			sram.write(op1,sram.read(op2)|op3);
			break;
		case 0x2A: /*OR KR (or const reg)*/
			sram.write(op1,op2|sram.read(op3));
			break;
		case 0x2B: /*NOR*/
			sram.write(op1,~(sram.read(op2)|sram.read(op3)));
			break;
		case 0x2C: /*NOR RK (nor reg const)*/
			sram.write(op1,~(sram.read(op2)|op3));
			break;
		case 0x2D: /*NOR KR (nor const reg)*/
			sram.write(op1,~(op2|sram.read(op3)));
			break;
		case 0x2E: /*XOR*/
			sram.write(op1,sram.read(op2)^sram.read(op3));
			break;
		case 0x2F: /*XOR RK (xor reg const)*/
			sram.write(op1,sram.read(op2)^op3);
			break;
		case 0x30: /*XOR KR (xor const reg)*/
			sram.write(op1,op2^sram.read(op3));
			break;
		case 0x31: /*NAN*/
			sram.write(op1,~(sram.read(op2)&sram.read(op3)));
			break;
		case 0x32: /*NAN RK (nand reg const)*/
			sram.write(op1,~(sram.read(op2)&op3));
			break;
		case 0x33: /*NAN KR (nand const reg)*/
			sram.write(op1,~(op2&sram.read(op3)));
			break;
		case 0x34: /*NOT*/
			sram.write(op1,~sram.read(op2));
			break;
		case 0x35: /*SHL*/
			sram.write(op1,sram.read(op2)<<sram.read(op3));
			break;
		case 0x36: /*SHL RK (shl reg const)*/
			sram.write(op1,sram.read(op2)<<op3);
			break;
		case 0x37: /*SHL KR (shl const reg)*/
			sram.write(op1,op2<<sram.read(op2));
			break;
		case 0x38: /*SHR*/
			sram.write(op1,sram.read(op2)>>sram.read(op3));
			break;
		case 0x39: /*SHR RK (shr reg const)*/
			sram.write(op1,sram.read(op2)>>op3);
			break;
		case 0x3A: /*SHR KR (shr const reg)*/
			sram.write(op1,op2>>sram.read(op2));
			break;
		case 0x3B: /*INT (interrupt)*/ //NEEDS TABLE SYSTEM
			interrupt_cpu(op1);
			break;
		case 0x3C: /*DLY (delay)*/
			utils.delay(op1);
			break;
		case 0x3D: /*NOP (nop) - DOES NOTHING*/ 
			break;
		case 0x3E: /*HLT (halt)*/ 
			program_counter=program_counter_maximum;
			break;
		//QUANTUM FUNCTIONS:
		//1 QUBIT GATES -
		case 0x3F: // X GATE
		{
			int * newthephi=gates.X(qmem.read(op1,THE,false),qmem.read(op1,PHI,false));
			qmem.write(op1,THE,newthephi[0]);qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x40: // Y GATE
		{
			int * newthephi=gates.Y(qmem.read(op1,THE,false),qmem.read(op1,PHI,false));
			qmem.write(op1,THE,newthephi[0]);qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x41: // Z GATE
		{
			int * newthephi=gates.Z(qmem.read(op1,THE,false),qmem.read(op1,PHI,false));
			qmem.write(op1,THE,newthephi[0]);qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x42: // H GATE
		{
			int * newthephi=gates.H(qmem.read(op1,THE,false),qmem.read(op1,PHI,false));
			qmem.write(op1,THE,newthephi[0]);qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x43: // S GATE
		{
			int * newthephi=gates.S(qmem.read(op1,THE,false),qmem.read(op1,PHI,false));
			qmem.write(op1,THE,newthephi[0]);qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x44: // T GATE
		{
			int * newthephi=gates.T(qmem.read(op1,THE,false),qmem.read(op1,PHI,false));
			qmem.write(op1,THE,newthephi[0]);qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x45: // ROTATE X GATE
		{
			int * newthephi=gates.ROX(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),op2);
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x46: // ROTATE Y GATE
		{
			int * newthephi=gates.ROY(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),op2);
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		case 0x47: // ROTATE Z GATE
		{
			int * newthephi=gates.ROZ(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),op2);
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			free(newthephi);
		}
		break;
		//2 QUBIT GATES -
		case 0x48: // CNOT GATE
		{
			int * newthephi=gates.CNO(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			free(newthephi);
		}
		break;
		case 0x49: // CSIGN GATE
		{
			int * newthephi=gates.CSI(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			free(newthephi);
		}
		break;
		case 0x4A: // SWAP GATE
		{
			int * newthephi=gates.SWA(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			free(newthephi);
		}
		break;
		case 0x4B: // INCREMENT GATE
		{
			int * newthephi=gates.INC(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			free(newthephi);
		}
		break;
		case 0x4C: // DECREMENT GATE
		{
			int * newthephi=gates.DEC(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			free(newthephi);
		}
		break;
		case 0x4D: // SWAGSQ GATE
		{
			int * newthephi=gates.SWQ(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			free(newthephi);
		}
		break;
		case 0x4E: // SWAPI GATE
		{
			int * newthephi=gates.SWI(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			free(newthephi);
		}
		break;
		//3 QUBIT GATES -
		case 0x4F: // CONTROL SWAP GATE
		{
			int * newthephi=gates.CSW(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false),qmem.read(op3,THE,false),qmem.read(op3,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			qmem.write(op3,THE,newthephi[4]); qmem.write(op3,PHI,newthephi[5]);
			free(newthephi);
		}
		break;
		case 0x50: // TOFFOLI GATE
		{
			int * newthephi=gates.TOF(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false),qmem.read(op3,THE,false),qmem.read(op3,PHI,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			qmem.write(op3,THE,newthephi[4]); qmem.write(op3,PHI,newthephi[5]);
			free(newthephi);
		}
		break;
		case 0x51: // DEUTSCH GATE
		{
			int * newthephi=gates.DEU(qmem.read(op1,THE,false),qmem.read(op1,PHI,false),qmem.read(op2,THE,false),qmem.read(op2,PHI,false),qmem.read(op3,THE,false),qmem.read(op3,PHI,false),qmem.read(0,THE,false));
			qmem.write(op1,THE,newthephi[0]); qmem.write(op1,PHI,newthephi[1]);
			qmem.write(op2,THE,newthephi[2]); qmem.write(op2,PHI,newthephi[3]);
			qmem.write(op3,THE,newthephi[4]); qmem.write(op3,PHI,newthephi[5]);
			free(newthephi);
		}
		break;
		default: /*THIS FUNCTION DOES NOT EXIST*/
			break;
	}
	
	if(DEBUG_MODE){
		serial.writestr("Function: "); serial.writestr(utils.int2str(func));
		serial.writestr(" , OP1: ");   serial.writestr(utils.int2str(op1));
		serial.writestr(" , OP2: ");   serial.writestr(utils.int2str(op2));
		serial.writestr(" PC: ");	   serial.writestrln(utils.int2str(program_counter));
		serial.writestrln("");
	}
}