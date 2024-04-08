; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This is a tie into the PAL-1 PICO Expansion that allowed overriding ROM
; and RAM on the system, mapping it to the internal PICO memory.
;
; 1A96 G
; To enter the configuration.  The same config can also be reached by the
; PICO USB cable as well.
;
; The PICO Firmware also has the option to redirect the GETCH to a local
; copy in this routine:
;  pokerom(0x1E5A + 0, 0x4C);
;  dpokerom(0x1E5A + 1, 0x1A99);
;
; The new GETCH routines will
;   * Cancel hardware echo
;   * Get the character
;   * Convert the character to upper case
;   * Print the character via OUTCH
;

; START 1A96
; END 1BF9
; LENGTH: 163

; This is the byte that is to be printed.
XCH_TO_PAL = $1FF7
; This is the input from the PAL to the pico firmware
XCH_FROM_PAL = $1FF8
; This is the action that the PICO is expecting the routine to do
; When the 6502 code is finished, it will write 'XCH_CMD_NONE' to this
; address to inform the pico it can continue
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
DEHALF = $1EEB; Half delay
DELAY = $1ED4; Full-length delay

SAD = $1740; 6530 A DATA
SBD = $1742; KIM 6532 PIA B data register
CHAR = $FE
TMPX = $FD


        .org $1A96
        ; MAIN ENTRY POINT
        JMP MAIN

        ; Entry point for upper case readch

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
        STA XCH_FROM_PAL
        JMP END_CMD

END_CMD:
        LDA #XCH_CMD_NONE
        STA XCH_CMD_REG
        JMP MAIN

;echoflag: .byte 1  ; flag: 0 normal echo

EGETCHAR:
        ;LDA echoflag ; if notechoflag
        ;beq normal ;  then normal echo
        LDA SBD  ; else set TTY bit PB0 to 0
        AND #$FE
        STA SBD ;
normal: JSR GETCH2 ; get character from input
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


;
; Rather than try to hack up the GETCH routine in ROM and figure out where we can
; interject the convert to lower case, it was easier to just grab that code and
; duplicate it here...
;
; We have the room.
;
;       ** GET 1 CHAR FROM TTY, CHAR IN A
GETCH2: STX   TMPX      ; SAVE X REG     1E5A
        LDX   #$08      ; SET UP 8-BIT COUNT
        LDA   #$01
GET1:   BIT   SAD
        BNE   GET6
        BMI   GET1      ; WAIT FOR START BIT
        JSR   DELAY     ; DELAY 1 BIT
GET5:   JSR   DEHALF    ; DELAY 1/2 BIT TIME
GET2:   LDA   SAD       ; GET 8 BITS
        AND   #$80      ; MASK OFF LOW ORDER BITS
        LSR   CHAR      ; SHIFT RIGHT CHAR
        ORA   CHAR
        STA   CHAR
        JSR   DELAY     ; DELAY 1 BIT TIME
        DEX
        BNE   GET2      ; GET NEXT CHAR
        JSR   DEHALF    ; EXIT THIS ROUTINE
        LDX   TMPX
        LDA   CHAR
        ROL   A         ; SHIFT OFF PARITY
        LSR   A
GET6:   RTS
