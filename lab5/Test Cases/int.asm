	.ORIG x1200
	;PUSH registers
	ADD R6, R6, #-2
	STW R0, R6, #0
	ADD R6, R6, #-2
	STW R1, R6, #0
	ADD R6, R6, #-2
	STW R2, R6, #0
	ADD R6, R6, #-2
	STW R3, R6, #0
	
	;perform operation
	LEA R1, MASK
	LDW R1, R1, #0 ; R1 = MASK
	LEA R0, LOC
	LDW R0, R0, #0 ; R0 = MEMORY ADDRESS
	LEA R3, STOP
	LDW R3, R3, #0 ; R3 = 128


LOOP	LDW R2, R0, #0 ; R2 = PTE
	AND R2, R2, R1 ; clear R
	STW R2, R0, #0 ; store PTE back
	ADD R0, R0, #2 ; increment to next memory address
	ADD R3, R3, #-1 ; decrement counter
	BRP LOOP ; check to see if loop is over
		
	;POP registers
	LDW R3, R6, #0
	ADD R6, R6, #2
	LDW R2, R6, #0
	ADD R6, R6, #2
	LDW R1, R6, #0
	ADD R6, R6, #2
	LDW R0, R6, #0
	ADD R6, R6, #2
	RTI
LOC	.FILL x1000
MASK	.FILL xFFFE
STOP	.FILL #128
	.END
