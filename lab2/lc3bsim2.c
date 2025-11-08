/*
    Name 1: Aidan Gonzales 
    UTEID 1: agg3498
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/
int instruction;

typedef enum {
	ADD, AND, BR, JMP, JSR, LDB, LDW, LEA, SHF, STB, STW, TRAP, XOR, NOP
	//probably need to add .orig, .fill, .end, ret, and check others implemented in assembler
} instrType;

instrType type = NOP;

void SEXT5(int *val) {
	if ((*val & 0x10) > 0) {
		*val |= 0xFFE0;
	}
}

void SEXT6(int *val) {
	if ((*val & 0x20) > 0) {
		*val |= 0xFFC0;
	}
}
	
void SEXT8(int *val) {
	if ((*val & 0x80) > 0) {
		*val |= 0xFF00;
	}
}

void SEXT9(int *val) {
	if ((*val & 0x100) > 0) {
		*val |= 0xFE00;
	}
}

void SEXT11(int *val) {
	if ((*val & 0x400) > 0) {
		*val |= 0xF800;
	}
}

void setcc(int DR) {
	if (DR == 0) {
		NEXT_LATCHES.N = 0;
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
	} else if ((DR & 0x8000) == 0) {
		NEXT_LATCHES.N = 0;
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
	} else {
		NEXT_LATCHES.N = 1;
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
	}
}



void fetch() {
	//printf("current PC: 0x%.4X\n", CURRENT_LATCHES.PC);
	//printf("msB memory at PC: 0x%.2X\n", MEMORY[CURRENT_LATCHES.PC >> 1][1]);
	//printf("lsB memory at PC: 0x%.2X\n", MEMORY[CURRENT_LATCHES.PC][0]);
	//printf("value loaded into instruction: %d\n", (MEMORY[CURRENT_LATCHES.PC][1] << 8) + MEMORY[CURRENT_LATCHES.PC][0]); 
	instruction = (MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8) + MEMORY[CURRENT_LATCHES.PC >> 1][0];
	instruction = Low16bits(instruction); // if bit 15 is set, then the instructioin will be 32 bits long because of sext, so we need to mask it
}

void decode() {
	//printf("instr: 0x%.4x\n", instruction);
	//printf("opcode: 0x%.1x\n", (instruction & 0xF000) >> 12);
	switch (instruction >> 12) {
		case 0x0:
			type = BR;
			break;
		case 0x1:
			type = ADD;
			break;
		case 0x2:
			type = LDB;		
			break;
		case 0x3:
			type = STB;
			break;
		case 0x4:
			type = JSR;
			break;
		case 0x5:
			type = AND;
			break;
		case 0x6:
			type = LDW;
			break;
		case 0x7:
			type = STW;
			break;
		//0x8 is RTI. Not supposed to include it for this assignment
		case 0x9:
			type = XOR;
			break;
		//0xA and 0xB are unused opcodes
		case 0xC:
			type = JMP;
			break;
		case 0xD:
			type = SHF;
			break;
		case 0xE:
			type = LEA;
			break;
		case 0xF:
			type = TRAP;
			break;
		default:
			//printf("Error: Invalid opcode\n");
			//exit(-1);
			break;
	}
}

void executeBR() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//check CC and update PC
	int cc = (instruction & 0x0E00) >> 9;
	if (((cc & 0x4) && CURRENT_LATCHES.N) || ((cc & 0x2) && CURRENT_LATCHES.Z) || ((cc & 0x1) && CURRENT_LATCHES.P)) {
		int imm9 = instruction & 0x01FF;
		SEXT9(&imm9);
		NEXT_LATCHES.PC += (imm9 << 1);
		NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC);
	}
}
void executeADD() {
	//increment PC
	NEXT_LATCHES.PC += 2;
	
	//update DR
	int *DR = &NEXT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int SR1 = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	if ((instruction & 0x0020) == 0) {
		int SR2 = CURRENT_LATCHES.REGS[instruction & 0x0007];
		*DR = SR1 + SR2;
	} else {
		int imm5 = instruction & 0x1F;
		SEXT5(&imm5);
		*DR = SR1 + imm5;
	}
	*DR = Low16bits(*DR);

	//set CC
	setcc(*DR);
}

void executeLDB() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//load all instruction values
	int *DR = &NEXT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int BaseR = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	int boffset6 = instruction & 0x3F;
	SEXT6(&boffset6);
	int value = BaseR + boffset6;
	value = Low16bits(value); // gonna have to do this for every base + offset
	
	//have to load the correct byte
	if ((value % 2) == 0) {
		value /= 2;
		value = MEMORY[value][0];
	} else {
		value /= 2;
		value = MEMORY[value][1];
	}

	//sign extend value and put into DR
	SEXT8(&value);
	*DR = value;
	*DR = Low16bits(*DR);

	//set CC
	setcc(*DR);
}

void executeSTB() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//load all instruction values
	int SR = CURRENT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int BaseR = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	int boffset6 = instruction & 0x3F;
	SEXT6(&boffset6);
	int value = BaseR + boffset6;
	value = Low16bits(value); // get rid of overflow

	//have to store to the correct byte
	if ((value % 2) == 0) {
		value /= 2;
		MEMORY[value][0] = (SR & 0xFF);
	} else {
		value /= 2;
		MEMORY[value][1] = (SR & 0xFF);
	}
	
}

void executeJSR() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//R7 = incremented PC
	NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;

	//if JSRR, jump to base regiser, if JSR, jump to PC offset
	if ((instruction & 0x0800) == 0) {
		NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	} else {
		int imm11 = instruction & 0x07FF;
		SEXT11(&imm11);
		NEXT_LATCHES.PC += (imm11 << 1);
		NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC);
	}

}

void executeAND() {
	//increment PC
	NEXT_LATCHES.PC += 2;
	
	//update DR
	int *DR = &NEXT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int SR1 = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	if ((instruction & 0x0020) == 0) {
		int SR2 = CURRENT_LATCHES.REGS[instruction & 0x0007];
		*DR = SR1 & SR2;
	} else {
		int imm5 = instruction & 0x1F;
		SEXT5(&imm5);
		*DR = SR1 & imm5;
	}
	*DR = Low16bits(*DR);

	//set CC
	setcc(*DR);
	
}

void executeLDW() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//update DR
	int *DR = &NEXT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int BaseR = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	int offset6 = instruction & 0x003F;
	SEXT6(&offset6);
	offset6 = offset6 << 1;
	int value = BaseR + offset6;
	value = Low16bits(value); // get rid of overflow
	value /= 2;
	*DR = (MEMORY[value][1] << 8) + MEMORY[value][0];
	*DR = Low16bits(*DR);

	//set CC
	setcc(*DR);
}

void executeSTW() {
	//increment PC
	NEXT_LATCHES.PC += 2;
	
	//store to memory
	int SR = CURRENT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int BaseR = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	int offset6 = instruction & 0x003F;
	SEXT6(&offset6); // sign extend offset
	offset6 = offset6 << 1; // left shift offset because stw
	offset6 = Low16bits(offset6); // cut off the extra bit
	int value = BaseR + offset6;
	value = Low16bits(value); // get rid of overflow
	value /= 2;
	MEMORY[value][1] = (SR & 0xFF00) >> 8;
	MEMORY[value][0] = SR & 0x00FF;

}

void executeXOR() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//load DR, SR1
	int *DR = &NEXT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int SR1 = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	
	//execute correct XOR/NOT
	if ((instruction & 0x0020) == 0) {
		int SR2 = CURRENT_LATCHES.REGS[instruction & 0x0007];
		*DR = SR1 ^ SR2;
	} else {
		int imm5 = instruction & 0x001F;
		SEXT5(&imm5);
		*DR = SR1 ^ imm5;
	}

	*DR = Low16bits(*DR);

	//set CC
	setcc(*DR);
}

void executeJMP() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//jump
	NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
}

void executeSHF() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//load DR, SR, and amount4
	int *DR = &NEXT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int SR = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
	int amount4 = instruction & 0x000F;

	//execute the correct shift
	if ((instruction & 0x0010) == 0) {
		*DR = SR << amount4;
	} else if ((instruction & 0x0020) == 0) {
		*DR = (SR >> amount4) & 0x7FFF;
	} else {
		//since the machine is 32/64 bits, regular arithmetic shifts won't work on 16 bit numbers
		if ((SR & 0x8000) == 0) {
			*DR = SR >> amount4;
		} else {
			*DR = (SR >> amount4) | 0x8000;
			
			int mask = 0x0000;
			for (int i = 0; i < amount4; i++) {
				mask = (mask >> 1) | 0x8000;
			}

			*DR |= mask;
		}
	}

	*DR = Low16bits(*DR);

	//set CC
	setcc(*DR);	
}

void executeLEA() {
	//increment PC
	NEXT_LATCHES.PC += 2;
	
	//load the address
	int *DR = &NEXT_LATCHES.REGS[(instruction & 0x0E00) >> 9];
	int PCoffset9 = instruction & 0x01FF;
	SEXT9(&PCoffset9);
	*DR = NEXT_LATCHES.PC + (PCoffset9 << 1);
	*DR = Low16bits(*DR);
}

void executeTRAP() {
	//increment PC
	NEXT_LATCHES.PC += 2;

	//R7 = incremented PC
	NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
	
	//PC = MEM[LSHF(ZEXT(trapvect8), 1)];
	NEXT_LATCHES.PC = (MEMORY[(instruction & 0xFF) << 1][1] << 8) + MEMORY[(instruction & 0xFF) << 1][0];
}

void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     
	fetch();
	decode();

	switch (type) {
		case BR:
			executeBR();
			break;
		case ADD:
			executeADD();
			break;
		case LDB:
			executeLDB();
			break;
		case STB:
			executeSTB();
			break;
		case JSR:
			executeJSR();
			break;
		case AND:
			executeAND();
			break;
		case LDW:
			executeLDW();
			break;
		case STW:
			executeSTW();
			break;
		//0x8 is RTI. Not supposed to include it for this assignment
		case XOR:
			executeXOR();
			break;
		//0xA and 0xB are unused opcodes
		case JMP:
			executeJMP();
			break;
		case SHF:
			executeSHF();
			break;
		case LEA:
			executeLEA();
			break;
		case TRAP:
			executeTRAP();
			break;
		default:
			break;
	}

}
