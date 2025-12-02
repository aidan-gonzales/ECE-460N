	.ORIG x0200
	.FILL x0000 ;nothing is here
	.FILL x1200 ;address of interrupt service routine
	.FILL x1400 ;address of the page fault exception routine
	.FILL x1A00 ;address of the unaligned access exception routine
	.FILL x1600 ;address of the protection exception routine
	.FILL x1C00 ;address of the unknown opcode exception routine
	.END
