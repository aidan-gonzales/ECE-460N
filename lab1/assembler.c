/*
 * Name 1: Aidan Gonzales
 * EID 1: agg3498
 *
 * This is the assembler written in c.
 * */

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

FILE* infile = NULL;
FILE* outfile = NULL;

#define MAX_LINE_LENGTH 255
enum
{
   DONE, OK, EMPTY_LINE
};

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];	/* Question for the reader: Why do we need to add 1? */
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int numSymbols = 0;

int PC = 0;


//If is an opcode, return 0. If not an opcode, return -1
int isOpcode(char *lPtr) {

	char OpArray[28][6] = {"add", "and", "halt", "jmp", "jsr", "jsrr", "ldb", "ldw", "lea", "nop", "not", "ret", "lshf", "rshfl", "rshfa", "rti", "stb", "stw", "trap", "xor", "brn", "brz", "brp", "brnz", "brnp", "brzp", "br", "brnzp"};

	for (int i = 0; i < 28; i++) {
	     if (strcmp(lPtr, OpArray[i]) == 0) {
	     return 0;
	     }
	}
	return -1;
}

//returns 0 if it is a trap command (can't be a label) and a 1 if it can be a label
int checkTrapCommands(char *lPtr) {
        char TrapArray[4][5] = {"getc", "puts", "in", "out"};

	for (int i = 0; i < 4; i++) {
	     if (strcmp(lPtr, TrapArray[i]) == 0) {
		     return 0;
	     }
	}
	return 1;
			     
}

//returns 1 if it's a valid label, and 0 if it's invalid
int isValidLabel(char *lPtr) {
	int length = strlen(lPtr);
	//label must be less than 20 characters long
	if (length > 20) {
		return 0;
	}

	//label must start with an alphabetic character
	if (!isalpha(lPtr[0])) {
		return 0;
	}

	//label can't start with the letter 'x'
	if (lPtr[0] == 'x') {
		return 0;
	}

	//label must be all alphanumeric
	for (int i = 0; i < length; i++) {
		if (!isalnum(lPtr[i])) {
			return 0;
		}
	}

	//label can't be one of the listed trap commands
	if (!checkTrapCommands(lPtr)) {
		return 0;
	}
	return 1;
}

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
   char * lRet, * lPtr;
   int i;
   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
	return( DONE );
   for( i = 0; i < strlen( pLine ); i++ )
	pLine[i] = tolower( pLine[i] );

   /* convert entire line to lowercase */
   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

   /* ignore the comments */
   lPtr = pLine;

   while( *lPtr != ';' && *lPtr != '\0' &&
   *lPtr != '\n' )
	lPtr++;

   *lPtr = '\0';
   if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
	return( EMPTY_LINE );

   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' && isValidLabel(lPtr)) /* found a label */
   {
	*pLabel = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
   }

   *pOpcode = lPtr;

   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

   *pArg1 = lPtr;

   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

   *pArg2 = lPtr;
   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

   *pArg3 = lPtr;

   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

   *pArg4 = lPtr;

   return( OK );
}

int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

int val = 0;

//looks for the current label, and returns the address it corresponds to
int symbolLookup(char *label) {
	for (int i = 0; i < numSymbols; i++) {
		if (!strcmp(label, symbolTable[i].label)) {
			return symbolTable[i].address;
		}
	}
	return -1;
}

void addHandler(char *a1, char *a2, char *a3) {
	val = 0x1 << 12;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;
	if (a3[0] == 'r') {
		val += (a3[1] - 0x30);
	} else if (a3[0] == '#' || a3[0] == 'x') {
		val |= 0x1 << 5;
		val += (toNum(a3) & 0x1F);
	}
}

void andHandler(char *a1, char *a2, char *a3) {
	val = 0x5 << 12;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;
	if (a3[0] == 'r') {
		val += a3[1] - 0x30;
	} else if (a3[0] == '#' || a3[0] == 'x') {
		val |= (1 << 5);
		val += (toNum(a3) & 0x1F);
	}
}

void haltHandler() {
	val = 0xF025;
}

void jmpHandler(char *a1) {
	val = 0xC000;
	val += (a1[1] - 0x30 << 6);
}

void jsrHandler(char *a1) {
	val = 0x4800;
	int labelVal = symbolLookup(a1);
	val += (((labelVal - PC) / 2) & 0x7FF);
}

void jsrrHandler(char *a1) {
	val = 0x4000;
	val += (a1[1] - 0x30) << 6;
}

void ldbHandler(char *a1, char *a2, char *a3) {
	val = 0x2000;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;
	val += (toNum(a3) & 0x3F);
}

void ldwHandler(char *a1, char *a2, char *a3) {
	val = 0x6000;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;
	val += (toNum(a3) & 0x3F);
}

void leaHandler(char *a1, char *a2) {
	val = 0xE000;
	val += (a1[1] - 0x30) << 9;
	//symbol lookup
	int labelVal = symbolLookup(a2);
	val += (((labelVal - PC) / 2) & 0x1FF);
}

void nopHandler() {
	val = 0x0000;
}

void notHandler(char *a1, char *a2) {
	val = 0x9000;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;
	val += 0x3F;
}

void retHandler() {
	val = 0xC1C0;
}

void lshfHandler(char *a1, char *a2, char *a3) {
	val = 0xD000;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;

	val += (toNum(a3) & 0xF);
}

void rshflHandler(char *a1, char *a2, char *a3) {
	val = 0xD010;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;

	val += (toNum(a3) & 0xF);
}


void rshfaHandler(char *a1, char *a2, char *a3) {
	val = 0xD030;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;

	val += (toNum(a3) & 0xF);
}

void rtiHandler() {
	val = 0x8000;
}

void stbHandler(char *a1, char *a2, char *a3) {
	val = 0x3000;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;

	val += (toNum(a3) & 0x3F);
}

void stwHandler(char *a1, char *a2, char *a3) {
	val = 0x7000;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;

	val += (toNum(a3) & 0x3F);
}

void trapHandler(char *a1) {
	val = 0xF000;

	val += (toNum(a1) & 0xFF);
}

void xorHandler(char *a1, char *a2, char *a3) {
	val = 0x9000;
	val += (a1[1] - 0x30) << 9;
	val += (a2[1] - 0x30) << 6;
	if (a3[0] == 'r') {
		val += (a3[1] - 0x30);
	} else if (a3[0] == '#' || a3[0] == 'x') {
		val |= 0x1 << 5;
		val += (toNum(a3) & 0x1F);
	}
}

void brHandler(char *op, char *a1) {
	int nzp = 0;
	for (int i = 0; i < strlen(op); i++) {
		if (op[i] == 'n') {
			nzp |= 0x4;
		} else if (op[i] == 'z') {
			nzp |= 0x2;
		} else if (op[i] == 'p') {
			nzp |= 0x1;
		}
	}
	if (nzp == 0) {
		nzp = 0x7;
	}

	val = 0x0000;
	val |= nzp << 9;
	
	//symbol lookup
	int labelVal = symbolLookup(a1);
	val += (((labelVal - PC) / 2) & 0x1FF);

}

void origHandler(char *a1) {
	PC = toNum(a1) & (0xFFFF);
	val = PC;
}

void fillHandler(char *a1) {
	val = toNum(a1) & (0xFFFF);
}

int endFlag = 0;
void endHandler() {
	endFlag = 1;
}

void secondPass(FILE *lInfile) 
{
   char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
	*lArg2, *lArg3, *lArg4;

   int lRet;

   //lInfile = fopen( "data.in", "r" );	/* open the input file */

   do
   {
	lRet = readAndParse( lInfile, lLine, &lLabel,
		&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
	if( lRet != DONE && lRet != EMPTY_LINE )
	{
		if (!strcmp(lOpcode, "add")) {
			addHandler(lArg1, lArg2, lArg3);
		} else if(!strcmp(lOpcode, "and")) {
			andHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "halt")) {
			haltHandler();
		} else if (!strcmp(lOpcode, "jmp")) {
			jmpHandler(lArg1);
		} else if (!strcmp(lOpcode, "jsr")) {
			jsrHandler(lArg1);
		} else if (!strcmp(lOpcode, "jsrr")) {
			jsrrHandler(lArg1);
		} else if (!strcmp(lOpcode, "ldb")) {
			ldbHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "ldw")) {
			ldwHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "lea")) {
			leaHandler(lArg1, lArg2);
		} else if (!strcmp(lOpcode, "nop")) {
			nopHandler();
		} else if (!strcmp(lOpcode, "not")) {
			notHandler(lArg1, lArg2);
		} else if (!strcmp(lOpcode, "ret")) {
			retHandler();
		} else if (!strcmp(lOpcode, "lshf")) {
			lshfHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "rshfl")) {
			rshflHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "rshfa")) {
			rshfaHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "rti")) {
			rtiHandler();
		} else if (!strcmp(lOpcode, "stb")) {
			stbHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "stw")) {
			stwHandler(lArg1, lArg2, lArg3);
		} else if (!strcmp(lOpcode, "trap")) {
			trapHandler(lArg1);
		} else if (!strcmp(lOpcode, "xor")) {
			xorHandler(lArg1, lArg2, lArg3);
		} else if (lOpcode[0] == 'b' && lOpcode[1] == 'r') {
			brHandler(lOpcode, lArg1);
		} else if (!strcmp(lOpcode, ".orig")) {
	 		origHandler(lArg1);
		} else if (!strcmp(lOpcode, ".fill")) {
			fillHandler(lArg1);
		} else if (!strcmp(lOpcode, ".end")) {
			endHandler();
		} else {
			printf("invalid opCode: %s\n", lOpcode);
		}
		
		if (!endFlag) {
			fprintf(outfile, "0x%.4X\n", val);
			PC += 2;
		}

		//PC += 2;


		//gonna have to use the passed by reference variables above to create
		//the machine code and output using the file output sample code
		//printf("lLine: %s lLabel: %s lOpcode: %s lArg1: %s lArg2: %s lArg3: %s lArg4: %s\n", lLine, lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
	}
   } while( lRet != DONE );
}


void symbolTableMaker(FILE *lInfile) 
{
   char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
	*lArg2, *lArg3, *lArg4;

   int lRet;

   //lInfile = fopen( "data.in", "r" );	/* open the input file */

   int currAddr = 0;

   do
   {
	lRet = readAndParse( lInfile, lLine, &lLabel,
		&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

	if( lRet != DONE && lRet != EMPTY_LINE )
	{
		if (!strcmp(lOpcode, ".orig")) {
			currAddr = toNum(lArg1) - 2;
		} else {
			currAddr += 2;
		}
		if (strlen(lLabel) > 0) {
			symbolTable[numSymbols].address = currAddr;
			strcpy(symbolTable[numSymbols].label, lLabel);
			numSymbols++;
		}
	}
   } while( lRet != DONE );
}


int main(int argc, char* argv[]) {


     //This is the code to check the number of arguments
     char *prgName   = NULL;
     char *iFileName = NULL;
     char *oFileName = NULL;
	
     if (argc != 3) {
	     printf("invalid arguments\n");
	     return 0;
     }
     prgName   = argv[0];
     iFileName = argv[1];
     oFileName = argv[2];

     printf("program name = '%s'\n", prgName);
     printf("input file name = '%s'\n", iFileName);
     printf("output file name = '%s'\n", oFileName);

     //This is the code to open the files
     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");

     if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }

     /* Do stuff with files */
     symbolTableMaker(infile);
     for (int i = 0; i < numSymbols; i++) {
     }

     rewind(infile);

     secondPass(infile);

     //do the first pass to complete the symbol table
     //do the second pass to convert the assembly to machine code

     //This is the code to close files
     fclose(infile);
     fclose(outfile);
}
