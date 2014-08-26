#ifndef QEPU_CONSTANTS_H_
#define QEPU_CONSTANTS_H_

//QUBIT MEMORY AND INSTRUCTIONS CONSTANTS:
#define REGISTER_SIZE 8
#define REGISTER_COUNT 2
#define QUBIT_COUNT REGISTER_SIZE*REGISTER_COUNT
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
#define WRITE_QB 6
#define READ_QB 5
#define DIM_SEL 4
#define HIGH 1
#define LOW 0
#define INPUT 0x0
#define OUTPUT 0xFF

//PIN INPUT/OUTPUT/DIRECTION CONSTANTS:
#define CTRL_BUFF_DIR DDRC
#define CTRL_BUFF PORTC
#define CTRL_QB PORTA
#define CTRL_QB_DIR DDRA
#define BUS_IN PINB
#define BUS_DIR DDRB
#define BUS_OUT PORTB
#define CTRLMEM_DIR DDRC
#define CTRLMEM PORTC
#define MEM_ADDRESS_DIR DDRA
#define MEM_ADDRESS PORTA

//ADDRESS BUFFER CONSTANTS:
#define ADDRESS_BUFFER_SIZE 3
#define IN_ABBUFFER_START 2
#define IN_ABBUFFER_CLOCK 3
#define EO_AB 4
#define ADDRESS_WRITE_DELAY 10
#define BUS_WIDTH 8

//SRAM CONSTANTS:
#define MWR 5
#define MRE 6
#define MEMORY_HEAD_PERMISSION_OFFSET 0
#define MEMORY_TAIL_PERMISSION_OFFSET 200
#define ADDRESS_MAX 256
#define HEAD_PROGRAMMER_OFFSET 20
#define TAIL_PROGRAMMER_OFFSET 0

//FLAG CONSTANTS:
#define FLAG_COUNT 8
#define CND_LWER 0
#define CND_LWER_EQUAL 1
#define CND_EQUAL 2
#define CND_GRTER_EQUAL 3
#define CND_GRTER 4
#define CND_DIFF 5
#define CND_ZERO 6
#define CND_NOT_ZERO 7

//JUMP STACK CONSTANTS:
#define JUMP_STACK_MAX 100

//GENERAL PURPOSE CONSTANTS:
#define INPUT 0x0
#define OUTPUT 0xFF
#define HIGH 1
#define LOW 0
#define M_PI 3.14159265358979323846
#define F_CPU 14745600

#endif