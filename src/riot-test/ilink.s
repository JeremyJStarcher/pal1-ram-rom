; START 1A96
; END 1BF9

; // 1FF7, 1FF8, 1FF9 // IO 

XCH_TO_PAL = $1FF7
XCH_FROM_PAL = $1FF8
XCH_CMD_REG = $1FF9

XCH_CMD_NONE = 0
XCH_CMD_CHOUT = 1
XCH_CMD_CHIN = 2

; KIM-ROUTINES
OUTCH = $1EA0; output a character to the TTY
KIMMON  = $1C4F; Return to monitor
CRLF = $1E2F ; Out a CR/LF to TTY
PRTBYT = $1E3B ; Output a hex byte to TTY
OUTSP = $1E9E; Output a space to the TTY
GETCH = $1E5A; Get one ASCII character from TTY and return in A.
SAD = $1740     ; 6530 A DATA
SBD = $1742 ; KIM 6532 PIA B data register

        .org $1A96
        ; MAIN ENTRY POINT
        JMP MAIN

        ; Entry point for upper case readch
        JMP ucasech

ucasech:
        JSR EGETCHAR
        PHA
        jsr OUTCH
        PLA
        rts

MAIN:
        CLD
        LDA XCH_CMD_REG
        CMP #XCH_CMD_CHOUT
        BEQ PCHOUT

        CMP #XCH_CMD_CHIN
        BEQ PCHIN

        JMP MAIN

PCHOUT:
        ; See if there is a character to print
        LDA XCH_TO_PAL
        jsr OUTCH
        jmp END_CMD

PCHIN:
        JSR EGETCHAR;
        ; JSR GETCH;
        STA XCH_FROM_PAL
        JMP END_CMD

END_CMD:
        LDA #XCH_CMD_NONE
        STA XCH_CMD_REG
        JMP MAIN



echoflag: .byte 1  ; flag: 0 normal echo

EGETCHAR:
        LDA echoflag ; if notechoflag 
        beq normal ;  then normal echo 
        LDA SBD  ; else set TTY bit PB0 to 0 
        AND #$FE
        STA SBD ; 
normal: JSR GETCH ; get character from input
        PHA ; save
        LDA SBD ; set TTY bit PB0 
        ORA #$01 
        STA SBD 
        PLA ; restore received character


        ; Assuming the character is already loaded in the A register
        CMP #$61         ; Compare with ASCII value of 'a'
        BCC NotLowercase ; If less than 'a', it's not lowercase
        CMP #$7A         ; Compare with ASCII value of 'z'
        BCS NotLowercase ; If greater than 'z', it's not lowercase

        ; Correct way to convert lowercase to uppercase
        SEC              ; Set carry flag for subtraction
        SBC #$20         ; Subtract 32 to convert to uppercase

NotLowercase:
        RTS 
