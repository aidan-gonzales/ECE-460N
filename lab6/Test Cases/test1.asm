        .ORIG x3000

        LEA   R1, DATA         ; R1 = &DATA
        LDW   R2, R1, #0       ; R2 = DATA   (RAW on R1)
        ADD   R3, R2, #1       ; R3 = DATA+1 (RAW)

        ADD   R1, R3, #0       ; R1 = R3     (WAR against earlier R1)
        ADD   R1, R1, #0       ; R1 = R1     (forces second WAR hazard)

        STW   R3, R1, #0       ; MEM[R1] = R3   (store-use on R1)

        LDB   R4, R1, #0       ; R4 = low byte of R3 (load-use after store)
        ADD   R4, R4, #1       ; R4 = lowbyte(R3)+1 (RAW)

        LEA   R5, RESULT       ; R5 = &RESULT  (addr dep)
        ADD   R6, R4, R2       ; R6 = (byte+1)+DATA (combined RAW chain)
        STW   R6, R5, #0       ; RESULT = R6

        HALT

; ---- Data ----
DATA    .FILL  x1234    
RESULT  .FILL  x0000    ; x126A

        .END
