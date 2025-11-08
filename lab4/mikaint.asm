	.ORIG x1200
    ;-PUSH R-;
    ADD R6, R6, #-2   ; SP -= 2
    STW R0, R6, #0    ; PUSH R0
    ADD R6, R6, #-2   ; SP -= 2
    STW R1, R6, #0    ; PUSH R1
    ;-;

    ;-TIM-;
    LEA R0 PTR      ; R0 <- PTR to x4000
    LDW R0 R0 #0    ; R0 <- x4000
    LDW R1 R0 #0    ; R1 < M[x4000]
    ADD R1 R1 #1    ; R1 += 1
    STW R1 R0 #0    ; M[x4000] <- R1
    ;-;

    ;-POP R-;
    LDW R1, R6, #0    ; POP R1
    ADD R6, R6, #2    ; SP += 2
    LDW R0, R6, #0    ; POP R2
    ADD R6, R6, #2    ; SP += 2
    ;-;

    RTI
PTR .FILL x4000
.END  
