	.ORIG x3000
	LEA R0, NUM
	LDW R1, R0, #0 ; this should load 0x1234
	LDB R2, R0, #0 ; this should load 0x34
	LDB R3, R0, #1 ; this should load 0x12
	
	LEA R0, NUM2
	LDW R4, R0, #0 ; loads 0x12FF
	LDB R5, R0, #0 ; loads 0xFF
	
	LEA R0, NUM3
	LDW R6, R0, #0 ; laods 0xFF12
	LDB R7, R0, #1 ; laods 0xFF
	
	HALT



NUM	.FILL x1234 ; 0x3016
NUM2	.FILL x12FF ; 0x3018
NUM3	.FILL xFF12 ; 0x301A
	.END
