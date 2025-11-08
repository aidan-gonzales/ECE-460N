        .ORIG x3000
START   AND R0,R0,#0        ; sig = 0 [init] [1]
        AND R1,R1,#0        ; R1 = 0 [1]
        ADD R1,R1,#15       ; R1 = +15 (imm5 max +) [1]
        AND R2,R2,#0        ; R2 = 0 [1]
        ADD R2,R2,#-16      ; R2 = -16 (imm5 min) -> xFFF0 [1]
        ADD R3,R1,R2        ; R3 = 15 + (-16) = -1 -> xFFFF [1]
        XOR R4,R3,#-1       ; R4 = NOT(-1) = 0x0000 (XOR imm form) [1]
        AND R5,R3,#15       ; R5 = 0x000F (mask low nibble) [1]
        XOR R0,R2,R5        ; sig = 0xFFF0 XOR 0x000F = 0xFFFF (sets N) [1] 0x3010

        ; BR tests: use N->Z->P sequence and accumulate a small value
        BRn B1              ; take if N=1 (from sig=-1) [1]
        ADD R0,R0,#1        ; would run only if BRn failed [1]
B1      ADD R0,R0,#1        ; sig += 1, now 0 -> Z=1 [1]
        BRz B2              ; take if Z=1 [1]
        ADD R0,R0,#1        ; would run only if BRz failed [1]
B2      ADD R0,R0,#1        ; sig = 1 -> P=1 [1]
        BRp B3              ; take if P=1 [1]
        ADD R0,R0,#1        ; would run only if BRp failed [1] 0x3020
B3      ADD R0,R0,#1        ; sig = 2 [1]

        ; SHF: left to 0x8000, logical-right to 1, arithmetic-right (-1 >>15)->-1
        AND R6,R6,#0        ; R6 = 0 [1]
        ADD R6,R6,#1        ; R6 = 1 [1]
        LSHF R6,R6,#15      ; R6 = 0x8000 [1]
        ADD R0,R0,R6        ; sig += 0x8000 -> 0x8002 [1]
        RSHFL R7,R6,#15     ; R7 = 0x0001 [1]
        ADD R0,R0,R7        ; sig -> 0x8003 [1]
        RSHFA R2,R3,#15     ; R2 = -1 (arith shift of -1 by 15) [1] 0x3030
        ADD R0,R0,R2        ; sig += 0xFFFF -> 0x8002 [1]

        ; LDW/LDB with sign extension and alignment; STW/STB round-trips
        LEA R4,DATA         ; R4 = &DATA (word-aligned) [1]
        LDW R1,R4,#0        ; R1 = x1234 [1]
        ADD R0,R0,R1        ; sig -> 0x9236 [1]
        LDB R2,R4,#0        ; R2 = SEXT(0x34) = 0x0034 [1]
        ADD R0,R0,R2        ; sig -> 0x926A [1]
        LDB R3,R4,#1        ; R3 = SEXT(0x12) = 0x0012 [1]
        ADD R0,R0,R3        ; sig -> 0x927C [1] 0x3040

        LEA R5,BYTE1        ; R5 = &BYTE1 (x0080) [1]
        LDB R6,R5,#0        ; R6 = SEXT(0x80) = 0xFF80 (-128) [1]
        ADD R0,R0,R6        ; sig -> 0x91FC [1]
        ADD R5,R5,#1        ; R5 = BYTE1 + 1 [1]
        LDB R7,R5,#-1       ; R7 = SEXT(byte[BYTE1]) = 0xFF80 [1]
        ADD R0,R0,R7        ; sig -> 0x917C [1]

        LEA R4,WORD1        ; R4 = &WORD1 [1]
        STW R1,R4,#0        ; WORD1 = x1234 (store word, aligned) [1] 0x3050
        LDW R6,R4,#0        ; R6 = x1234 [1]
        ADD R0,R0,R6        ; sig -> 0xA3B0 [1]

        LEA R4,TEMP         ; R4 = &TEMP [1]
        STW R6,R4,#0        ; TEMP = x1234 [1]
        LDW R7,R4,#0        ; R7 = x1234 [1]
        ADD R0,R0,R7        ; sig -> 0xB5E4 [1]

        ; STB/LDB round-trip with 0x7F then 0x80 to test sign [1]
        LEA R4,TEMP2        ; R4 = &TEMP2 [1]
        LEA R5,C7F          ; R5 = &C7F (x007F) [1] 0x3060
        LDW R2,R5,#0        ; R2 = 0x007F [1]
        STB R2,R4,#0        ; store low byte 0x7F [1]
        LDB R3,R4,#0        ; R3 = 0x007F [1]
        ADD R0,R0,R3        ; sig -> 0xB663 [1]
        XOR R2,R2,#-1       ; R2 = 0xFF80 [1]
        STB R2,R4,#0        ; store low byte 0x80 [1]
        LDB R6,R4,#0        ; R6 = 0xFF80 [1]
        ADD R0,R0,R6        ; sig -> 0xB5E3 [1] 0x3070

        ; JSR (PC-relative) adds +3; JSRR (base) bitwise-NOT via XOR #-1; JMP/RET [1]
        JSR SUB1            ; call SUB1; R7=PC′; PC = PC′ + PCoffset11 [1]
        LEA R3,SUB2         ; R3 = &SUB2 [1]
        JSRR R3             ; call SUB2 via base register [1]
        LEA R2,JMP_T        ; R2 = &JMP_T [1]
        JMP R2              ; jump via base register (even address) [1]

SUB1    ADD R0,R0,#1        ; sig += 1 [1]
        ADD R0,R0,#2        ; sig += 2 (total +3) [1]
        RET                 ; PC = R7 (return) [1] 0x3080

SUB2    XOR R0,R0,#-1       ; sig = bitwise NOT(sig) [1]
        RET                 ; return [1]

JMP_T   ADD R0,R0,#1        ; final tweak after JMP [1]

        ; Store signature and HALT
        LEA R7,SIG          ; R7 = &SIG [1]
        STW R0,R7,#0        ; MEM[SIG] = R0 (expected x4A1A) [1]
        TRAP x25            ; HALT (PC loads from trap vector, 0 by default) [1]

; Data (no .BLKW)
DATA    .FILL x1234         ; bytes: =0x34, [1]=0x12 [1]
BYTE1   .FILL x0080         ; low byte 0x80 for LDB sign test [1] 0x3090
WORD1   .FILL x5555         ; overwritten with x1234 during test [1]
TEMP    .FILL x0000         ; word temp for STW/LDW [1]
TEMP2   .FILL x0000         ; byte temp for STB/LDB [1]
C7F     .FILL x007F         ; constant 127 [1]
SIG     .FILL x0000         ; final signature storage (expect x4A1A) [1] 0x309a
        .END
