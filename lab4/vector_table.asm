	.ORIG x0200
	.FILL x0000 ;nothing is here
	.FILL x1200 ;address of interrupt service routine
	.FILL x1600 ;address of protection exception service routine
	.FILL x1A00 ;address of unaligned access exception service routine
	.FILL x1C00 ;address of unkown opcode exception service routine
	.END
