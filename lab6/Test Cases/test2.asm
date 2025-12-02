        .ORIG x3000

        LEA   R1, DATA        ; R1 = &DATA (addr dep)
        LDW   R2, R1, #0      ; R2 = DATA (load-use dependency on R1)
        ADD   R3, R2, #1      ; R3 = DATA+1 (RAW on R2)

        ADD   R1, R3, #0      ; R1 = R3 (WAR hazard vs earlier R1 use)
        STW   R3, R1, #0     ; MEM[R1-1] = R3 (addr uses freshly written R1)

        LDB   R4, R1, #0     ; R4 = low byte of stored R3 (load-use after store)
        ADD   R4, R4, #2      ; R4 += 2 (ALU dep on R4)

        STB   R4, R1, #1      ; store byte using updated R4 (store dep)

	
        LEA   R5, RESULT      ; R5 = &RESULT (addr dep)
        ADD   R6, R4, R2      ; R6 = (stored_byte+2) + DATA (combined RAW)

;	halt

        STW   R6, R5, #0      ; RESULT = R6 (final value)

        HALT

; -------- Memory --------
DATA    .FILL   x1234          ;
RESULT  .FILL   x0000          ; x126B

        .END
