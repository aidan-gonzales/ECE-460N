/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0, // I added COND2
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    LD_VECTOR, // added
    LD_PRIV, // added
    LD_ACV, // added
    LD_SAVEDUSP, // added
    LD_SAVEDSSP, // added
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    GATE_VECTOR, // added
    GATE_PSR, // added
    GATE_SP, // added
    GATE_PCmTWO, // added
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0, // added 1
    SR1MUX1, SR1MUX0, // added 1
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    TABLEMUX, // added
    VECTORMUX1, VECTORMUX0, // added
    SPMUX1, SPMUX0, // added
    PSRMUX, // added
    IRMUX, // added
    EXCMUX, // added
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    //number of control store bits provided by shell code:
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetLD_VECTOR(int *x)     { return(x[LD_VECTOR]); }
int GetLD_PRIV(int *x)       { return(x[LD_PRIV]); }
int GetLD_ACV(int *x)        { return(x[LD_ACV]); }
int GetLD_SAVEDUSP(int *x)   { return(x[LD_SAVEDUSP]); }
int GetLD_SAVEDSSP(int *x)   { return(x[LD_SAVEDSSP]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetGATE_VECTOR(int *x)   { return(x[GATE_VECTOR]); }
int GetGATE_PSR(int *x)      { return(x[GATE_PSR]); }
int GetGATE_SP(int *x)       { return(x[GATE_SP]); }
int GetGATE_PCmTWO(int *x)   { return(x[GATE_PCmTWO]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetTABLEMUX(int *x)      { return(x[TABLEMUX]); }
int GetVECTORMUX(int *x)     { return((x[VECTORMUX1] << 1) + x[VECTORMUX0]); }
int GetSPMUX(int *x)         { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetPSRMUX(int *x)        { return(x[PSRMUX]); }
int GetIRMUX(int *x)         { return(x[IRMUX]); }
int GetEXCMUX(int *x)        { return(x[EXCMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* System stack pointer */

//These are system latches that I added:
int INT; // goes high if an interrupt needs to be serviced
int TIMSR; // timer interrupt register (bit 15 tells us that there's an interrupt)
int PSR; // PSR[15] = privelege bit, PSR[2:0] = condition codes
int ACV; // This goes high if we're in user mode and we try to access protected memory
int USP; // User stack pointer
int TABLE; // indicates whether it's a TRAP or INT/EXC
int VECTOR; // indicated where in the lookup table to find the starting address of the service routine
int SAVEDSSP; // input to the SPMUX
int SAVEDUSP; // input to the SPMUX

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);

    //printf("PSR		 :0x%0.4x\n", CURRENT_LATCHES.PSR); // REMOVE WHEN DONE TESTING

    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
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

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 2]);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.SAVEDSSP = 0x3000; // Initial value of saved ssp - I added this
    CURRENT_LATCHES.SAVEDUSP = 0x0; // Initial value of the saved usp - I added this
    CURRENT_LATCHES.PSR = 0x8002; // initial value of PSR, User mode, Z is high - I added this
    CURRENT_LATCHES.TABLE = 0x0; // initialize the table to 0 - I added this
    CURRENT_LATCHES.VECTOR = 0x0; // initialize the vector to 0 - I added this

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
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

//The following functions evaluate different components to help drive the bus
int evaluateSR1MUX();
int evaluateSR2MUX();
int evaluateADDER();
int evaluateADDR1MUX();
int evaluateADDR2MUX();
int evaluatePCMUX();
int evaluateLSHF1();
int evaluateMIO_EN();

//lab 4:
int evaluateSPMUX();
int evaluateTABLEMUX();
int evaluateVECTORMUX();
int evaluatePSRMUXPriv();
int evaluatePSRMUXnzp();


//The following five functions drive the bus
int driveMARMUX();
int drivePC();
int driveALU();
int driveSHF();
int driveMDR();

//lab 4:
int driveVECTOR();
int drivePSR();
int driveSP();
int drivePCmTWO();


int evaluateSR1MUX() {
	int mySR1MUX = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
	switch (mySR1MUX) {
		case 0x0:
			return CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 9];
		case 0x1:
			return CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x01C0) >> 6];
		case 0x2:
			return CURRENT_LATCHES.REGS[6];
	}
}

int evaluateSR2MUX() {
	int mySR2MUX = ((CURRENT_LATCHES.IR & 0x0020) >> 5);
	int returnval;

	switch (mySR2MUX) {
		case 0x0:
			returnval = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x0007];
			break;
		case 0x1:
			returnval = (CURRENT_LATCHES.IR & 0x1F);
			if (returnval & 0x0010) {
				returnval |= 0xFFE0;
			}
			break;
	}

	return returnval;
}

int evaluateADDER() {
	int val1 = evaluateADDR1MUX();
	int val2 = evaluateADDR2MUX();
	int returnval = val1 + val2;

	returnval = Low16bits(returnval);

	return returnval;
}

int evaluateADDR1MUX() {
	int value = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
	switch (value) {
		case 0x0:
			return CURRENT_LATCHES.PC;
		case 0x1:
			return evaluateSR1MUX();
	}
}

//includes LSHF1
int evaluateADDR2MUX() {
	int value = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
	int returnval = 0;
	switch (value) {
		case 0x0:
			break;
		case 0x1:
			returnval = (CURRENT_LATCHES.IR & 0x003F);
			if (returnval & 0x0020) {
				returnval |= 0xFFC0;
			}
			break;
		case 0x2:
			returnval = (CURRENT_LATCHES.IR & 0x01FF);
			if (returnval & 0x0100) {
				returnval |= 0xFE00;
			}
			break;
		case 0x3:
			returnval = (CURRENT_LATCHES.IR & 0x07FF);
			if (returnval & 0x0400) {
				returnval |= 0xF800;
			}
			break;
	}

	int shift = evaluateLSHF1();

	returnval = returnval << shift;

	returnval = Low16bits(returnval);

	return returnval;
}

int evaluatePCMUX() {
	int value = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	switch (value) {
		case 0x0:
			return CURRENT_LATCHES.PC + 2;
		case 0x1:
			return BUS;
		case 0x2:
			return evaluateADDER();
	}
}

int evaluateLSHF1() {
	int value = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);
	return value;
}

int evaluateMIO_EN() {
	int value = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
	return value;
}


int evaluateSPMUX() {
	int mySPMUX = GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	int val1 = evaluateSR1MUX();
	int returnval;
	switch (mySPMUX) {
		case 0:
			returnval = val1 + 2;
			break;
		case 1:
			returnval = val1 - 2;
			break;
		case 2:
			returnval = CURRENT_LATCHES.SAVEDSSP;
			break;
		case 3:
			returnval = CURRENT_LATCHES.SAVEDUSP;
			break;
	}
	return returnval;
}

//Vector register is latched within this function
int evaluateTABLEMUX() {
	int myTABLEMUX = GetTABLEMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	int returnval;
	switch (myTABLEMUX) {
		case 0:
			returnval = 0x00;
			NEXT_LATCHES.VECTOR = (BUS & 0x1FF);
			break;
		case 1:
			returnval = 0x01;
			NEXT_LATCHES.VECTOR = evaluateVECTORMUX();
			break;
	}
	
	return returnval;
}

int evaluateVECTORMUX() {
	int myVECTORMUX = GetVECTORMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	int returnval;
	switch (myVECTORMUX) {
		case 0:
			returnval = CURRENT_LATCHES.INTV;
			break;
		case 1:
			NEXT_LATCHES.EXCV = 0x02;
			returnval = 0x02;
			break;
		case 2:
			NEXT_LATCHES.EXCV = 0x3;
			returnval = 0x3;
			break;
		case 3:
			NEXT_LATCHES.EXCV = 0x4;
			returnval = 0x4;
			break;
	}

	returnval = returnval << 1;
	return returnval;
}

int evaluatePSRMUXPriv() {
	int myPSRMUX = GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	int returnval;
	switch (myPSRMUX) {
		case 0:
			returnval = 0x0;
			break;
		case 1:
			returnval = ((BUS & 0x8000) >> 15);
			break;
	}
	return returnval;
}

int evaluatePSRMUXnzp() {
	return GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
}

int driveMARMUX() {
	int myMARMUX = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	switch (myMARMUX) {
		case 0x0:
			return ((CURRENT_LATCHES.IR & 0x00FF) << 1);
		case 0x1:
			return evaluateADDER();
	}
}

int drivePC() {
	return CURRENT_LATCHES.PC;
}

int driveALU() {
	int val1 = evaluateSR1MUX();
	int val2 = evaluateSR2MUX();
	int returnval;


	int myALUK = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);
	switch (myALUK) {
		case 0x0:
			returnval = val1 + val2;
			break;
		case 0x1:
			returnval = val1 & val2;
			break;
		case 0x2:
			returnval = val1 ^ val2;
			break;
		case 0x3:
			returnval = val1;
			break;
	}

	returnval = Low16bits(returnval);

	return returnval;
}

int driveSHF() {
	int amount4 = (CURRENT_LATCHES.IR & 0x000F);
	int type = ((CURRENT_LATCHES.IR & 0x0030) >> 4);
	int SR = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x01C0) >> 6];
	int value;

	switch (type) {
		case 0x0:
			value = SR << amount4;
			break;
		case 0x1:
			value = SR >> amount4;
			break;
		case 0x3:
			value = SR >> amount4;
			if ((SR & 0x8000) > 0) {

				int mask = 0x0;

				for (int i = 0; i < amount4; i++) {
					mask = ((mask >> 1) | 0x8000);
				}

				value |= mask;
			}
			break;
	}

	value = Low16bits(value);
	return value;	
}

int driveMDR() {
	int myDATA_SIZE = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
	int returnval;

	switch (myDATA_SIZE) {
		case 0x0:
			if (CURRENT_LATCHES.MAR & 0x1) {
				returnval = (CURRENT_LATCHES.MDR  >> 8);
			} else {
				returnval = (CURRENT_LATCHES.MDR & 0x00FF);
			}

			if (returnval & 0x0080) {
				returnval |= 0xFF00;
			}

			break;
		case 0x1:
			returnval = CURRENT_LATCHES.MDR;
			break;
	}

	return returnval;
}

//concatenates TABLE and VECTOR to get the address in the lookup table
int driveVECTOR() {
	int returnval = (CURRENT_LATCHES.TABLE << 9) | (CURRENT_LATCHES.VECTOR);
	return returnval;
}

//returns the PSR
int drivePSR() {
	int returnval = CURRENT_LATCHES.PSR;
	return returnval;
}

//returns the output of the SPMUX
int driveSP() {
	int returnval = evaluateSPMUX();
	return returnval;
}

//returns current PC - 2
int drivePCmTWO() {
	int returnval = CURRENT_LATCHES.PC - 2;
	return returnval;
}


void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */

	if (CYCLE_COUNT == 300) {
		NEXT_LATCHES.INT = 1;
		NEXT_LATCHES.INTV = 0x01;
	}

	if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION)) {
		if (GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
			if (CURRENT_LATCHES.MAR & 0x1) {
				NEXT_LATCHES.STATE_NUMBER = 0x3D; // this is the memory unalignment handler
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
			} else {
				if (CURRENT_LATCHES.IR & 0x1000) {
					NEXT_LATCHES.STATE_NUMBER = 0x17; // continue with STW
					memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				} else {
					NEXT_LATCHES.STATE_NUMBER = 0x19; // continue with LDW
					memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				}
			}
		} else {
			if (CURRENT_LATCHES.ACV) {
				NEXT_LATCHES.STATE_NUMBER = 0x3C; // this is the ACV handler
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
			} else {
				if (GetIRMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
					int val3 = ((CURRENT_LATCHES.IR & 0x4000) >> 13) | ((CURRENT_LATCHES.IR & 0x1000) >> 12);
					switch (val3) {
						case 0:
							NEXT_LATCHES.STATE_NUMBER = 0x1D; // LDB
							memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER],
							sizeof(int)*CONTROL_STORE_BITS);
							break;
						case 1:
							NEXT_LATCHES.STATE_NUMBER = 0x18; // STB
							memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER],
							sizeof(int)*CONTROL_STORE_BITS);
							break;
						case 2:
							NEXT_LATCHES.STATE_NUMBER = 0x2F; // LDW
							memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER],
							sizeof(int)*CONTROL_STORE_BITS);
							break;
						case 3:
							NEXT_LATCHES.STATE_NUMBER = 0x2F; // STW
							memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER],
							sizeof(int)*CONTROL_STORE_BITS);
							break;
					}
				} else {
					NEXT_LATCHES.STATE_NUMBER = ((CURRENT_LATCHES.IR & 0xF000) >> 12); // basic IR branch
					memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				}
			}
		}

	} else {
		switch (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION)) {
			case 0x0:	
				NEXT_LATCHES.STATE_NUMBER = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				break;
			case 0x1:
				//if ready == 1, update state. Else, stay in same state.
				NEXT_LATCHES.STATE_NUMBER = (GetJ(CURRENT_LATCHES.MICROINSTRUCTION) | (CURRENT_LATCHES.READY << 1));
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				break;
			case 0x2:
				NEXT_LATCHES.STATE_NUMBER = (GetJ(CURRENT_LATCHES.MICROINSTRUCTION) | (CURRENT_LATCHES.BEN << 2));
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				break;
			case 0x3:
				NEXT_LATCHES.STATE_NUMBER = (GetJ(CURRENT_LATCHES.MICROINSTRUCTION) | ((CURRENT_LATCHES.IR & 0x0800) >> 11));
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				break;
			case 0x4:
				NEXT_LATCHES.STATE_NUMBER = (GetJ(CURRENT_LATCHES.MICROINSTRUCTION) | ((CURRENT_LATCHES.PSR & 0x8000) >> 12));
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				break;
			case 0x5:
				NEXT_LATCHES.STATE_NUMBER = (GetJ(CURRENT_LATCHES.MICROINSTRUCTION) | (CURRENT_LATCHES.INT << 4));
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				NEXT_LATCHES.INT = 0;
				break;
			case 0x6:
				NEXT_LATCHES.STATE_NUMBER = (GetJ(CURRENT_LATCHES.MICROINSTRUCTION) | (CURRENT_LATCHES.ACV << 5));
				memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
				break;
		}
	}
	
}

int memCycle = 0;

void cycle_memory() {

  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
	if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)) {
		memCycle++;
	}

	if (memCycle == 4) {
		NEXT_LATCHES.READY = 1;
	}
	
	if (memCycle > 4) {
		memCycle = 0;
		NEXT_LATCHES.READY = 0;
	}
}

typedef enum {
	MARMUX_GATE,
	PC_GATE,
	ALU_GATE,
	SHF_GATE,
	MDR_GATE,
	VECTOR_GATE,
	PSR_GATE,
	SP_GATE,
	PCmTWO_GATE,
	NOTHING
} Gates;

Gates TRISTATE_DRIVERS;

void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR,
   *		 Gate_VECTOR,
   *		 Gate_PSR,
   *		 Gate_SP,
   *		 Gate_PCmTWO.
   *
   */    
	if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = MARMUX_GATE;
	} else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = PC_GATE;
	} else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = ALU_GATE;
	} else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = SHF_GATE;
	} else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = MDR_GATE;
	} else if (GetGATE_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = VECTOR_GATE;
	} else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = PSR_GATE;
	} else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = SP_GATE;
	} else if (GetGATE_PCmTWO(CURRENT_LATCHES.MICROINSTRUCTION)) {
		TRISTATE_DRIVERS = PCmTWO_GATE;
	} else {
		TRISTATE_DRIVERS = NOTHING;
	}
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */

	switch (TRISTATE_DRIVERS) {
		case MARMUX_GATE:
			BUS = driveMARMUX();
			break;
		case PC_GATE:
			BUS = drivePC();
			break;
		case ALU_GATE:
			BUS = driveALU();
			break;
		case SHF_GATE:
			BUS = driveSHF();
			break;
		case MDR_GATE:
			BUS = driveMDR();
			break;
		case VECTOR_GATE:
			BUS = driveVECTOR();
			break;
		case PSR_GATE:
			BUS = drivePSR();
			break;
		case SP_GATE:
			BUS = driveSP();
			break;
		case PCmTWO_GATE:
			BUS = drivePCmTWO();
			break;
		case NOTHING:
			BUS = 0;
			break;
	}

}


void latch_datapath_values() {
  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */       


	//Check all LD signals
	if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)) {
			NEXT_LATCHES.MAR = BUS;
	}

	if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
		int myMIO_EN = evaluateMIO_EN();
		int myDATA_SIZE = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
		int value;


		switch (myMIO_EN) {
			case 0x0:	
				switch (myDATA_SIZE) {
					case 0x0:	
						if (CURRENT_LATCHES.MAR & 0x1) {
							value = ((BUS & 0x00FF) << 8); // bottom eight bits are always taken from SR. Either stored in top or bottom 8 bits of MDR
						} else {
							value = (BUS & 0x00FF);
						}
						break;
					case 0x1:
						value = BUS;
						break;
				}

				NEXT_LATCHES.MDR = value;
				break;
			case 0x1:
				if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
					if (CURRENT_LATCHES.READY) {
						NEXT_LATCHES.MDR = (MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) + MEMORY[CURRENT_LATCHES.MAR >> 1][0];
					}
					break;
				}
		}
	
	}

	if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION)) {
		NEXT_LATCHES.IR = BUS;
	}

	if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)) {
		NEXT_LATCHES.BEN = (((CURRENT_LATCHES.IR & 0x0800) && CURRENT_LATCHES.N) || ((CURRENT_LATCHES.IR & 0x0400) && CURRENT_LATCHES.Z) 						|| ((CURRENT_LATCHES.IR & 0x0200) && CURRENT_LATCHES.P));
	}

	if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION)) {
		int myDRMUX = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
		int *DR;
		switch (myDRMUX) {
			case 0x0:
				DR = &NEXT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 9];
				break;
			case 0x1:
				DR = &NEXT_LATCHES.REGS[7];
				break;
			case 0x2:
				DR = &NEXT_LATCHES.REGS[6];
				break;
		}

		*DR = BUS;

	}

	if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION)) {
		int myPSRMUX = evaluatePSRMUXnzp();
		switch (myPSRMUX) {
			case 0:
				if (BUS & 0x8000) {
					NEXT_LATCHES.N = 1;
					NEXT_LATCHES.Z = 0;
					NEXT_LATCHES.P = 0;
				} else if (BUS == 0) {
					NEXT_LATCHES.N = 0;
					NEXT_LATCHES.Z = 1;
					NEXT_LATCHES.P = 0;
				} else {
					NEXT_LATCHES.N = 0;
					NEXT_LATCHES.Z = 0;
					NEXT_LATCHES.P = 1;
				}
				break;
			case 1:
				NEXT_LATCHES.N = ((BUS & 0x4) >> 2);
				NEXT_LATCHES.Z = ((BUS & 0x2) >> 1);
				NEXT_LATCHES.P = (BUS & 0x1);
				break;
		}

		NEXT_LATCHES.PSR &= 0xFFF8;
		NEXT_LATCHES.PSR |= ((NEXT_LATCHES.N << 2) + (NEXT_LATCHES.Z << 1) + NEXT_LATCHES.P);
	}

	if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
		int myPCMUX = evaluatePCMUX();
		NEXT_LATCHES.PC = myPCMUX;
	}

	if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) && GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) && CURRENT_LATCHES.READY) {
		int myDATA_SIZE = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
		switch (myDATA_SIZE) {
			case 0x0:
				if (CURRENT_LATCHES.MAR & 0x0001) {
					MEMORY[CURRENT_LATCHES.MAR >> 1][1] = (CURRENT_LATCHES.MDR >> 8); // according to ISA
				} else {
					MEMORY[CURRENT_LATCHES.MAR >> 1][0] = (CURRENT_LATCHES.MDR & 0x00FF); // according to ISA
				}
				break;
			case 0x1:
				MEMORY[CURRENT_LATCHES.MAR >> 1][1] = (CURRENT_LATCHES.MDR >> 8);
				MEMORY[CURRENT_LATCHES.MAR >> 1][0] = (CURRENT_LATCHES.MDR & 0x00FF);
				break;
		}
	}

	//Lab 4:
	
	if (GetLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)) {
		NEXT_LATCHES.TABLE = evaluateTABLEMUX(); // The vector register is loaded within evaluateTABLEMUX();
	}

	if (GetLD_PRIV(CURRENT_LATCHES.MICROINSTRUCTION)) {
		int val1 = evaluatePSRMUXPriv();
		NEXT_LATCHES.PSR &= 0x7FFF;
		NEXT_LATCHES.PSR |= (val1 << 15);
	}

	if (GetLD_ACV(CURRENT_LATCHES.MICROINSTRUCTION)) {
		if ((BUS < 0x3000) && (CURRENT_LATCHES.PSR & 0x8000)) {
			NEXT_LATCHES.ACV = 1;
		} else {
			NEXT_LATCHES.ACV = 0;
		}
	}

	if (GetLD_SAVEDUSP(CURRENT_LATCHES.MICROINSTRUCTION)) {
		int val1 = evaluateSR1MUX();
		NEXT_LATCHES.SAVEDUSP = val1;
	}

	if (GetLD_SAVEDSSP(CURRENT_LATCHES.MICROINSTRUCTION)) {
		int val1 = evaluateSR1MUX();
		NEXT_LATCHES.SSP = val1;
		NEXT_LATCHES.SAVEDSSP = val1;
	}

}
