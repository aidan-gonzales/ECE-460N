	.orig #3000
	LEA R0, NUM
	LDW R1, R0, #0
	ldw r2, r0, #-1
	ldw r2, r0, #1
	ldw r2, r7, x-B
	ldw r5, r6, x6	
	halt
NUM	.fill x34
	.end

