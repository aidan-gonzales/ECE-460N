; NAME 1: Aidan Gonzales
; UTEID 1: agg3498

	.ORIG x3000
	LEA R0, NUM1 ; loads address for the first number
	LDW R6, R0, #0 ; loads x3100
	
	LDB R1, R6, #0 ; loads contents at x3100
	LDB R2, R6, #1 ; loads contents at x3101
	
	AND R3, R3, #0

LOOP	ADD R3, R3, R1
	ADD R2, R2, #-1
	BRP LOOP

	LEA R4, MASK
	LDW R5, R4, #0
	AND R4, R5, R3
	BRZ CLEAR ; if no overflow, don't set the overflow bit
	
	AND R0, R0, #0
	ADD R0, R0, #1
	STB R0, R6, #3 ; stores a 1 in x3103
	BR OVER

CLEAR	AND R0, R0, #0
	STB R0, R6, #3 ; stores a 0 in x3103

OVER	STB R3, R6, #2 ; stores the result in x3102
	HALT
	

NUM1	.FILL x3100
MASK	.FILL xFF00
	.END
