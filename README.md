QEPU - 'Quantum Electronic Processing Unit' Project
====
*************** INSTRUCTION SET: *****************************

***** INFORMATION: *******************************************
- INSTRUCTION WIDTH: 13 WORDS | 26 BYTES | 208 BITS
- 
- STRUCTURE: 1 WORD + 4 WORDS + 4 WORDS + 4 WORDS

_________________________________________

|   FUNC  |   OP1   |   OP2   |    OP3  |

-----------------------------------------

| 2 BYTES | 4 BYTES | 4 BYTES | 4 BYTES |

-----------------------------------------


***** GENERAL PURPOSE FUNCTIONS (DATA MOVEMENTPROGRAM CONTROL):

01: MOR (Move Register) 		 -	MOR OP1 OP2 | Qop1=Qop2

02: MOM (Move Memory)   		 -	MOM OP1 OP2 | Mop1=Mop2

03: STR (Store)		  		 -	STR OP1 OP2 | Mop1=Qop2 	(Qop2 is a register of 8 qubits)

04: LOD (Load)		  		 -  LOD OP1 OP2	| Qop1=Mop2 	(Qop2 is a register of 8 qubits)

05: CRW (Constant Ram Write)   -  CRW OP1 OP2 | Mop1=op2  	(op2 is a constant number)

06: CQW (Constant Qubit Write) -  CQW OP1 OP2 | Qop1=op2  	(op2 is a constant number)

07: POP (Pop)					 -  POP OP1 	| Mop1=Mn0  	(Mn0 is the first address of the SRAM) 

08: PSH (Push)				 -	PSH OP1		| Mn0=Mop1  	(Mn0 is the first address of the SRAM)

09: CMT (Constant Move Theta)	 -	CMT OP1 OP2 | Qop1theta=op2 (op2 is a constant number)

0A: CMP (Constant Move Phi)	 -  CMP OP1 OP2 | Qop1phi=op2   (op2 is a constant number)

0B: CME (Compare)				 -  CMW OP1 OP2 | Set flags     (this operation makes <,<=,=,>=,> and != between op1 and op2)

0C: RET (Return)				 -	RET OP1 	| Return func.  (this operation returns the address of the SRAM which contains the result)

0D: INT (Interrupt)			 -  INT OP1 	| INTERRUPT     (NOT IMPLEMENTED YET)

0E: JMP (Jump) 				 -  JMP OP1     | pc=op1        (pc=Program Counter, op1 is a constant number)

0F: DLY (Delay)				 - 	DLY OP1     | wait op1 ms   

10: NOP (Nop) 				 -	NOP         | does nothing  (may be useful for delays)

11: HLT (Halt) 				 -  HLT         | pc=pc_max     (pc_max is the last line of the program)


***** QUANTUM FUNCTIONS: **************************************************************************************

* 1 QUBIT GATES -

12:  X (PAULI-X GATE)			 - X OP1 		| op1=X(op1)

13:  Y (PAULI-Y GATE)			 - Y OP1 		| op1=Y(op1)

14:  Z (PAULI-Z GATE)			 - Z OP1 		| op1=Z(op1)

15:  H (HADAMARD GATE)		 - H OP1 		| op1=H(op1)

16:  S (PHASE SHIFT S GATE)	 - S OP1 		| op1=S(op1)

17:  T (PHASE SHIFT T GATE)	 - T OP1 		| op1=T(op1)

18:  ROX (ROTATE X GATE)		 - ROX OP1 OP2 	| op1=ROX(op1,op2) (op2 is an amplitude in degrees)

19:  ROY (ROTATE Y GATE)		 - ROY OP1 OP2  | op1=ROY(op1,op2) (op2 is an amplitude in degrees)

1A:  ROZ (ROTATE Z GATE) 		 - ROZ OP1 OP2  | op1=ROZ(op1,op2) (op2 is an amplitude in degrees)


* 2 QUBIT GATES -

1B:  CNO (CNOT GATE) 			 - CNO OP1 OP2 	| op1,2=CNO(op1,op2)

1C:  CSI (CSIGN GATE)		     - CSI OP1 OP2  | op1,2=CSI(op1,op2)

1D:  SWA (SWAP GATE)			 - SWA OP1 OP2  | op1,2=SWA(op1,op2)

1E:  INC (INCREMENT GATE)		 - INC OP1 OP2 	| op1,2=INC(op1,op2)

1F:  DEC (DECREMENT GATE)		 - DEC OP1 OP2 	| op1,2=DEC(op1,op2)

20:  SWQ (SWAGSQ GATE)		 - SWQ OP1 OP2 	| op1,2=SWQ(op1,op2)

21:  SWI (SWAPI GATE)			 - SWI OP1 OP2 	| op1,2=SWI(op1,op2)


* 3 QUBIT GATES -

22:  CSW (CONTROL SWAP GATES)	 - CSW OP1 OP2 OP3	| op1,2,3=CSW(op1,op2,op3)

23:  TOF (TOFFOLI GATE)		 - TOF OP1 OP2 OP3 	| op1,2,3=TOF(op1,op2,op3)

24:  DEU (DEUTSCH GATE)		 - DEU OP1 OP2 OP3 	| op1,2,3=DEU(op1,op2,op3)

****************************************************************************************************************
