.ORIG x3000
    ;-Init x4000 to 1-;
    LEA R0 TIM      ; R0 <- PTR to x4000
    LDW R0 R0 #0    ; R0 <- x4000
    AND R1 R1 #0    ; CLR R1
    ADD R1 R1 #1    ; R1 = 1
    STW R1 R0 #0    ; M[x4000] <- 1
    ;-TIM-;

    ;-ADD-;
    LEA R0 PTR      ; R0 <- PTR to xC000
    LDW R0 R0 #0    ; R0 <- xC000
    AND R1 R1 #0    ; CLR R1 [SUM]
    AND R2 R2 #0    ; CLR R2 [CTR]
    ADD R2 R2 #10   ; R2 <- 10
    ADD R2 R2 #10   ; R2 <- 20 (num of datas)
    AND R3 R3 #0    ; TEMP

LOOP    LDB R3 R0 #0    ; TEMP <- MEM[I]
        ADD R1 R1 R3    ; R1 += R3
        ADD R0 R0 #1    ; PTR += 1
        ADD R2 R2 #-1   ; R2 -= 1
        BRP LOOP

    LEA R0 SUM      ; R0 <- PTR to SUM
    LDW R0 R0 #0    ; R0 <- xC014
    STW R1 R0 #0    ; M[xC014] <- R1

    AND R0 R0 #0    ; R0 <- x0000
    STW R1 R0 #0    ; EXC

    TRAP x25

PTR .FILL xC000
SUM .FILL xC014
TIM .FILL x4000
.END
