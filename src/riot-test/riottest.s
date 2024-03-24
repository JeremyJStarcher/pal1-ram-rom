        .macro  timerbasictest base_addr, timer_offset, expected, nop_count
.scope 
                JSR PRIMM
                .BYTE "Running test timerbasictest: ",0

                lda #>(base_addr + timer_offset) 
                jsr PRTBYT
                lda #<(base_addr + timer_offset) 
                jsr PRTBYT
                jsr OUTSP

                lda #$01
                sta base_addr+timer_offset

                .repeat nop_count
                jsr busy_loop
                .endrepeat

                lda base_addr+6
                STA TESTR

                cmp #expected
                beq pass

                JSR PRIMM
                .BYTE "failed!  Expected ",$0
                LDA #expected
                jsr PRTBYT

                JSR PRIMM
                .BYTE " actual ",$0

                jmp end

                pass:
                JSR PRIMM
                .BYTE "passed: ",$0

                end:

                LDA TESTR
                jsr PRTBYT
                JSR CRLF
.endscope
        .endmacro


        .org $0200

DPL = $00
DPH = $01

TESTR = $02 ; The test results spot
TESTRS = $03 ; Test result status
RIOT_BASE = $1700 ; or 1740 for built-in

; USER  / SYSTEM          User or System TIMER
; $1704 / $1744           Write: Timer, 1 usec, no interrupt
; $1705 / $1745           Write: Timer, 8 usec, no interrupt
; $1706 / $1746           Write: Timer, 64 usec, no interrupt. Read: timer count, disable interrupt.
; $1707 / $1747           Write: Timer, 1024 usec, no interrupt. Read: timer status, Bit 7 = 1 on timeout.
; $170C / $174C           Write: Timer, 1 usec, interrupt
; $170D / $174D           Write: Timer, 8 usec, interrupt
; $170E / $174E           Write: Timer, 64 usec, interrupt. Read: timer count, enable interrupt.
; $170F / $174F           Write: Timer, 1024 usec, interrupt



; KIM-ROUTINES
OUTCH = $1EA0; output a character to the TTY
KIMMON  = $1C4F; Return to monitor
CRLF = $1E2F ; Out a CR/LF to TTY
PRTBYT = $1E3B ; Output a hex byte to TTY
OUTSP = $1E9E; Output a space to the TTY


	JMP	START

START:
        CLD
        JSR CRLF
        JSR CRLF
	JSR PRIMM
	.BYTE "RUNNING RIOT COMPATIBILITY TEST!",$00

        JSR CRLF

	JSR PRIMM
	.BYTE "WITHOUT BUSY NOOPS!",$00
        JSR CRLF

        timerbasictest RIOT_BASE, $04, $FD, $00
        ;timerbasictest RIOT_BASE, $0C, $FD, $00
        timerbasictest RIOT_BASE, $05, $00, $00
        ;timerbasictest RIOT_BASE, $0D, $00, $00
        timerbasictest RIOT_BASE, $06, $00, $00
        ;timerbasictest RIOT_BASE, $0E, $00, $00
        timerbasictest RIOT_BASE, $07, $00, $00
        ;timerbasictest RIOT_BASE, $0F, $00, $00

	JSR PRIMM
	.BYTE "WITH BUSY NOOPS!",$00
        JSR CRLF

        timerbasictest RIOT_BASE, $04, $F5, 1
        ;timerbasictest RIOT_BASE, $0C, $F5, 1
        timerbasictest RIOT_BASE, $05, $F4, 2
        ;timerbasictest RIOT_BASE, $0D, $F4, 2
        timerbasictest RIOT_BASE, $06, $24, 3
        ;timerbasictest RIOT_BASE, $0E, $24, 3
        timerbasictest RIOT_BASE, $07, $D4, 5
        ;timerbasictest RIOT_BASE, $0F, $D4, 5

        JMP     KIMMON           ; ..and exit to KIM

;Put the string following in-line until a NULL out to the console
PRIMM:  pla			; Get the low part of "return" address
                                ; (data start address)
        sta     DPL
        pla
        sta     DPH             ; Get the high part of "return" address
                                ; (data start address)
        ; Note: actually we're pointing one short
@PSINB: ldy     #1
        lda     (DPL),y         ; Get the next string character
        inc     DPL             ; update the pointer
        bne     @PSICHO          ; if not, we're pointing to next character
        inc     DPH             ; account for page crossing
@PSICHO: ora     #0              ; Set flags according to contents of
                                ;    Accumulator
        beq     @PSIX1           ; don't print the final NULL
        jsr     OUTCH         ; write it out
        jmp     @PSINB           ; back around

@PSIX1: inc     DPL             ;
        bne     @PSIX2           ;
        inc     DPH             ; account for page crossing
@PSIX2: jmp     (DPL)           ; return to byte following final NULL


busy_loop:
    LDX #$FF    ; Load the X register with 255 (FF in hex)
loop_start:
    DEX         ; Decrement the X register by 1
    BNE loop_start ; Branch to loop_start if the result is Not Equal to zero (Z flag is not set)
    ; The loop exits when X wraps around from 0 to $FF, which will not branch since Z flag will be set
    rts
