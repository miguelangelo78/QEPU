#include "qepu.h"
QEPU::QEPU(){serial.writestr("INITIALIZING . . . ");DDRA=0xFF;program_counter=0;}
void QEPU::write(int index,int dim,int deg){
	DDRB=0xFF;
	
	setdata(deg);
	setctrl(index);
	setctrlpin(4,dim);
	setctrlpin(6,1); // CLOCK UP LOAD
	setctrlpin(6,0); // CLOCK DOWN LOAD
	setdata(0);
}
int QEPU::read(int index,int dim){
	DDRB=0x00;
	setctrl(index);
	//OPEN ALL AND GATES
	setctrlpin(5,1); // CLOCK UP READ
	setctrlpin(4,dim);
	int deg_read=PINB; // READ INPUT
	//CLOSE ALL AND GATES
	setctrl(0); // CLOSE ALL CONTROLS
	DDRB=0xFF;
	return deg_read;
}
void QEPU::setdata(int bin){
	PORTB=bin;
}
void QEPU::setctrl(int bin){
	PORTA=bin;
}
void QEPU::setdatapin(int pin,int state){
	PORTB&=~(1<<pin); // CLEAR PIN
	PORTB|=(state<<pin); // SET PIN
}
void QEPU::setctrlpin(int pin,int state){
	PORTA&=~(1<<pin); // CLEAR PIN
	PORTA|=(state<<pin); // SET PIN
}
void QEPU::dumpmem(){
	serial.writestrln("");
	serial.writestrln("** QUANTUM MEMORY DUMP **");
	for(int i=0;i<QUBIT_COUNT;i++){
		char * thedim=(char*)malloc(sizeof(char)*3); sprintf(thedim,"%d",read(i,THE));
		char * phidim=(char*)malloc(sizeof(char)*3); sprintf(phidim,"%d",read(i,PHI));
		char * qindex=(char*)malloc(sizeof(char));	 sprintf(qindex,"%d",i);
		serial.writestr("Q"); serial.writestr(qindex); serial.writestr(": Theta - ");
		serial.writestr(thedim);
		serial.writestr(" , Phi - ");
		serial.writestrln(phidim);
	}
}

char * QEPU::Utils::int2binstr(int num,int strlength){
    char * str = (char*)malloc(strlength + 1);
    if(!str) return NULL;
    str[strlength] = 0;

    // type punning because signed shift is implementation-defined
    unsigned u = *(unsigned *)&num;
    for(; strlength--; u >>= 1)
    str[strlength] = u & 1 ? '1' : '0';

    return str;
}

char* QEPU::Utils::char2str(char c){
	char*str=(char*)malloc(sizeof(char));
	sprintf(str,"%d",c);
	return str;
}
int*  QEPU::Utils::str2intarr(char* c){
	int c_length=0;
	for(int i=0;true;i++) if(c[i]==0xFF && c[i-1]==0xFF && c[i-2]==0xFF && c[i-3]==0xFF && c[i-4]==0xFF) break; else c_length++; c_length-=4;
	
	int *intarr=(int*)malloc(sizeof(int)*(c_length+5));
	for(int i=0;i<c_length;i++) intarr[i]=(int)c[i];
	for(int i=c_length;i<c_length+5;i++) intarr[i]=0xFF;
	return intarr;
}
int QEPU::Utils::countdigits(int dec){
	int counter=0;
	while(dec!=0){
		dec/=10;
		++counter;
	}
	return counter;
}
char* QEPU::Utils::int2str(int dec){
	char str[100];
	sprintf(str,"%d",dec);
	return str;
}
int QEPU::Utils::dec2hex(int dec){
	char str[100];
	sprintf(str,"%x",dec);
	return (int)strtol(str,NULL,16);
}
int QEPU::Utils::concint(int n1,int n2){
	char str1[10],str2[10];
	sprintf(str1,"%d",n1);
	sprintf(str2,"%d",n2);
	strcat(str1,str2);
	return atoi(str1);
}
int QEPU::Utils::delay(int ms){
	while(ms--) _delay_ms(1);
}

void QEPU::run(){
	serial.writestrln(" RUNNING "); serial.writestrln("");
	
	dumpmem();
	/*EEPROM FETCH*/
	#pragma region EEPROM_FETCH
	int * eeprom_mem=utils.str2intarr(eeprom.readall()); //TODO: READ ALL EEPROM
	int eeprom_mem_length=0;
	for(int i=0;true;i++) if(eeprom_mem[i]==0xFF && eeprom_mem[i-1]==0xFF && eeprom_mem[i-2]==0xFF && eeprom_mem[i-3]==0xFF && eeprom_mem[i-4]==0xFF) break; else eeprom_mem_length++; eeprom_mem_length-=4;
	//TODO: PUT ALL LINES FROM EEPROM INTO AN ARRAY OF INSTRUCTIONS
	int eeprom_splitted[INSTR_HEIGHT][INSTR_WIDTH];
	int line=0,eeprom_splittedx=0;
	for(int i=0;i<eeprom_mem_length;i++){
		if(eeprom_splittedx>=INSTR_WIDTH) {line++;eeprom_splittedx=0;} 
		eeprom_splitted[line][eeprom_splittedx++]=eeprom_mem[i];
	}
	#pragma endregion
	/*INSTRUCTION FETCH*/
	#pragma region INSTRUCTION_FETCH
	//TODO: EXECUTE INSTRUCTIONS INSIDE THE ARRAY OF INSTRUCTIONS:
	for(program_counter=0;program_counter<line;program_counter++){
		int func=0;	char op1s[OP1_WIDTH]=""; char op2s[OP2_WIDTH]="";		
		//FIXED WIDTH INSTRUCTION:
		func=eeprom_splitted[program_counter][FIXED_FUNC_OFFSET]; // FUNCTION FETCH
		for(int k=0;k<QUBIT_BYTE_SIZE;k++){
			sprintf(op1s,"%s%x",op1s,eeprom_splitted[program_counter][k+FIXED_OP1_OFFSET]); // HEX CONCAT TO STRING (OP1 FETCH)
			sprintf(op2s,"%s%x",op2s,eeprom_splitted[program_counter][k+FIXED_OP2_OFFSET]); // HEX CONCAT TO STRING (OP2 FETCH)
		}
		execute(func,strtol(op1s,NULL,16),strtol(op2s,NULL,16)); //*INSTRUCTION DECODE AND EXECUTE*/
	}
	dumpmem();
	#pragma endregion
}

void QEPU::execute(int func,int32_t op1,int32_t op2){
	int * newthephi=(int*)malloc(sizeof(int)*5);
	//TODO: MAKE A SWITCH ON THE FUNCTION
	switch(func){
		//DATA MOVEMENT AND PROGRAM CONTROL/FLUX/IO FUNCTIONS:
		case 0x01: /*MOV*/
			write(op1,THE,read(op2,THE));
			write(op1,PHI,read(op2,PHI));
		break;
		case 0x02: /*JMP(jump)*/ 
			program_counter=op1-1; 
		break;
		case 0x03: /*INT(interrupt)*/ break;
		case 0x04: /*CME(compare)*/ break;
		case 0x05: /*HLT(halt)*/ break;
		case 0x06: /*POP(pop)*/ break;
		case 0x07: /*PSH(push)*/ break;
		case 0x08: /*RET(return)*/ break;
		case 0x09: /*END(end)*/ break;
		case 0x0A: /*DLY(delay)*/ 
			utils.delay(op1);	break;
		case 0x0B: /*NOP(nop)*/ break;
		case 0x0C: /*LOD(load)*/ break;
		case 0x0D: /*STR(store)*/ break;
		case 0x0E: /*CMT(constantmovtheta)*/ 
			write(op1,THE,op2);
		break;
		case 0x0F: /*CMP(constantmovphi)*/
			write(op1,PHI,op2);
		break;
		//QUANTUM FUNCTIONS:
		//1 QUBIT GATES -
		case 0x10: // X GATE
			newthephi=gates.X(read(op1,THE),read(op1,PHI));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
		break;
		case 0x11: // Y GATE
			newthephi=gates.Y(read(op1,THE),read(op1,PHI));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
		break;
		case 0x12: // Z GATE
			newthephi=gates.Z(read(op1,THE),read(op1,PHI));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
		break;
		case 0x13: // H GATE
			newthephi=gates.H(read(op1,THE),read(op1,PHI));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
		break;
		case 0x14: // S GATE
			newthephi=gates.S(read(op1,THE),read(op1,PHI));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
		break;
		case 0x15: // T GATE
			newthephi=gates.T(read(op1,THE),read(op1,PHI));
			write(op1,THE,newthephi[0]);write(op1,PHI,newthephi[1]);
		break;
		//2 QUBIT GATES -
		case 0x16: 
			newthephi=gates.CNO(read(op1,THE),read(op1,PHI),read(op2,THE),read(op2,PHI));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
		break;
		case 0x17:
			newthephi=gates.CSI(read(op1,THE),read(op1,PHI),read(op2,THE),read(op2,PHI));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
		break;
		case 0x18: 
			newthephi=gates.SWA(read(op1,THE),read(op1,PHI),read(op2,THE),read(op2,PHI));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
		break;
		case 0x19: 
			newthephi=gates.INC(read(op1,THE),read(op1,PHI),read(op2,THE),read(op2,PHI));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
		break;
		case 0x1A: 
			newthephi=gates.DEC(read(op1,THE),read(op1,PHI),read(op2,THE),read(op2,PHI));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
		break;
		case 0x1B: 
			//CSWAP (FREDKIN ) -> 3 QUBIT GATE
		break;
		case 0x1C: 
			//TOFFOLI -> 3 QUBIT GATE
		break;
		case 0x1D: 
			//DEUTSCH -> 3 QUBIT GATE
		break;
		case 0x1E: 
			newthephi=gates.SWQ(read(op1,THE),read(op1,PHI),read(op2,THE),read(op2,PHI));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
		break;
		case 0x1F: 
			newthephi=gates.SWI(read(op1,THE),read(op1,PHI),read(op2,THE),read(op2,PHI));
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
			write(op2,THE,newthephi[2]); write(op2,PHI,newthephi[3]);
		break;
		case 0x20:
			newthephi=gates.ROX(read(op1,THE),read(op1,PHI),op2);
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
		break;
		case 0x21: 
			newthephi=gates.ROY(read(op1,THE),read(op1,PHI),op2);
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
		break;
		case 0x22: 
			newthephi=gates.ROZ(read(op1,THE),read(op1,PHI),op2);
			write(op1,THE,newthephi[0]); write(op1,PHI,newthephi[1]);
		break;
	}
	
	/*serial.writestr("Function: "); serial.writestr(utils.int2str(func));
	serial.writestr(" , OP1: ");   serial.writestr(utils.int2str(op1));
	serial.writestr(" , OP2: ");   serial.writestr(utils.int2str(op2));
	serial.writestr(" PC: ");	   serial.writestrln(utils.int2str(program_counter));
	serial.writestrln("");*/
	free(newthephi);	
}