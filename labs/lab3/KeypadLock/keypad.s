;/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\ 
;| File: keypad.s                                                                |
;| This file contains a variety of subroutines to interface with the 4x4 keypad  |
;| on the EVBplus EduBase board in conjunction with a Tiva C Launchpad.          |
;| The code is thought to work reasonably well, but there may still be bugs.     |
;| If you find one, email cmcischk@mtu.edu with details of what is going wrong.  |
;| The KeypadGetKey routine scans the keypad 2880 times so that a single keypress|
;| is only recognized *once*.  This may cause fast presses to be ignored.        |
;\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/

 AREA DATA, READWRITE, ALIGN=2 
KeypadLookupTable 	SPACE 17 	; 16 keys * 1 byte + null char
lastScan 			SPACE 1		; The last character read from keypad
debounceCount 		SPACE 4		; Number of sequential reads of the same value

 ALIGN

 AREA |.text|, CODE, READONLY, ALIGN=2
  EXPORT BuildKeypadLookupTable
BuildKeypadLookupTable 
 LDR R0, =KeypadLookupTable
 MOV R1, #0
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'1'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'2'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'3'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'A'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'4'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'5'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'6'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'B'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'7'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'8'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'9'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'C'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'*'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'0'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'#'
 STRB R1, [R0]
 ADD R0, #1
 MOV R1, #'D'
 STRB R1, [R0]

 
 BX LR

;/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\ 
;| KeypadInit                                     |
;| Author: Kit Cischke                            |
;| Based on C code written by Muhammad Ali Mazidi |
;|                                                |
;| Returns: void                                  |
;| Parameters: void                               |
;| The keypad is attached to PD0-3 (rows) and     |
;| PA2-5 (columns). Everything initialized to     |
;| input.  Analog mode turned off.                |
;| Modifies registers R0, R1.                     |
;\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/ 

GPIO_PORTD_DATA_R  EQU 0x400073FC
GPIO_PORTD_DIR_R   EQU 0x40007400
GPIO_PORTD_DEN_R   EQU 0x4000751C
GPIO_PORTD_AMSEL_R EQU 0x40007528
GPIO_PORTA_DATA_R  EQU 0x400043FC
GPIO_PORTA_DIR_R   EQU 0x40004400
GPIO_PORTA_DEN_R   EQU 0x4000451C
GPIO_PORTA_AMSEL_R EQU 0x40004528
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

 EXPORT KeypadInit
KeypadInit
 LDR R0, =SYSCTL_RCGCGPIO_R
 LDR R1, [R0]
 ORR R1, #0x01 ; Port A clocks
 ORR R1, #0x08 ; Port D clocks
 STR R1, [R0]
 
 NOP
 NOP
 NOP
 
 LDR R0, =GPIO_PORTA_DIR_R
 LDR R1, [R0]
 BIC R1, #0x3C
 STR R1, [R0]
 
 LDR R0, =GPIO_PORTA_AMSEL_R
 LDR R1, [R0]
 BIC R1, #0x3C
 STR R1, [R0]
 
 LDR R0, =GPIO_PORTA_DEN_R
 LDR R1, [R0]
 ORR R1, #0x3C
 STR R1, [R0]
 
 LDR R0, =GPIO_PORTD_DIR_R
 LDR R1, [R0]
 BIC R1, #0x0F
 STR R1, [R0]
 
 LDR R0, =GPIO_PORTD_AMSEL_R
 LDR R1, [R0]
 BIC R1, #0x0F
 STR R1, [R0]
 
 LDR R0, =GPIO_PORTD_DEN_R
 LDR R1, [R0]
 ORR R1, #0x0F
 STR R1, [R0]
 
 LDR R0, =lastScan
 MOV R1, #0
 STRB R1, [R0]
 
 LDR R0, =debounceCount
 MOV R1, #0
 STR R1, [R0] 
 
 BX LR
 
 ;/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\ 
;| KeypadGetKey                                    |
;| Author: Kit Cischke                             |
;| Based on C code written by Muhammad Ali Mazidi  |
;|                                                 |
;| Returns: ASCII value of key pressed (or 0 if    |
;|          no key pressed) in R0.                 |
;| Parameters: void                                |
;| A non-blocking function to read the keypad. If  |
;| a key is pressed, it returns the ASCII value of |
;| the key pressed.  Otherwise, it returns 0.      |
;| First all rows are driven high and the input    |
;| pins are read. If no key is pressed, it will    |
;| read a zero.  Otherwise, some key is pressed.   |
;| If any key is pressed, the code drives one row  |
;| high at a time and leaves the rest of the rows  |
;| inactive, then reads the input pins again.      |
;| This combination lets us determine which key    |
;| is pressed.                                     |
;| Modifies register R0 (saves all other regs)     |
;\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/ 
debounceMax EQU 2880
 EXPORT KeypadGetKey
KeypadGetKey

; Push all utilized registers onto the stack.
 STR R1, [SP], #-4
 STR R2, [SP], #-4
 STR R3, [SP], #-4
 STR R4, [SP], #-4
 STR R5, [SP], #-4
 STR R6, [SP], #-4
 STR R10, [SP], #-4
 
 LDR R0, =debounceCount
 MOV R1, #0
 STR R1, [R0]

; Enable all rows as output
; Yes, output. We have to write in order to read.
KeypadScan
 LDR R0, =GPIO_PORTD_DIR_R
 LDR R1, [R0]
 ORR R1, #0x0F
 STR R1, [R0]
 
 ; Enable the rows.
 LDR R0, =GPIO_PORTD_DATA_R
 LDR R1, [R0]
 ORR R1, #0x0F
 STR R1, [R0]
 
; Now read the input 
 LDR R0, =GPIO_PORTA_DATA_R
 LDR R1, [R0]
 AND R1, #0x3C
 
; Disable the rows
 LDR R2, =GPIO_PORTD_DIR_R
 LDR R3, [R2]
 BIC R3, #0x0F
 
; If R1 is 0, no button pressed.  Return.
 CMP R1, #0
 BNE WhichKey
 MOV R0, #0
 B KeypadReturn
 
WhichKey
 MOV R4, #0x01
 MOV R5, #0
KeypadLoop
; Disable all rows (again)
 LDR R0, =GPIO_PORTD_DIR_R
 LDR R1, [R0]
 BIC R1, #0x0F
 STR R1, [R0]
; Now enable just the row we're interested in 
 ORR R1, R4
 STR R1, [R0]
 
; Energize the keypad again to determine if
; the pressed button is in this row
 LDR R0, =GPIO_PORTD_DATA_R
 LDR R1, [R0]
 ORR R1, #0x0F
 STR R1, [R0]
 
 MOV R10, #160
KeypadDelayLoop 
 SUB R10, #1
 CMP R10, #0
 BNE KeypadDelayLoop
 

; After the delay, read a row.
 LDR R0, =GPIO_PORTA_DATA_R
 LDR R3, [R0]
 AND R3, #0x3C
; If 0, the key pressed isn't in this row. 
 CMP R3, #0
 BEQ NextRow
 
; Disable all rows (yet again)
 LDR R0, =GPIO_PORTD_DIR_R
 LDR R1, [R0]
 BIC R1, #0x0F
 STR R1, [R0]
 
; We're out of the for loop. One of two things happened:
; 1. We found a key (R3 != 0)
; 2. We found nothing. (R3 == 0)
; If 2, return.
 CMP R3, #0
 BNE DetermineChar
 
 MOV R0, #0
 B KeypadReturn

DetermineChar
 CMP R3, #0x04
 BNE CheckColumn1
 MOV R0, R5
 LSL R0, #2
 ADD R0, #1
 B KeypadReturn
 
CheckColumn1
 CMP R3, #0x08
 BNE CheckColumn2
 MOV R0, R5
 LSL R0, #2
 ADD R0, #2
 B KeypadReturn

CheckColumn2
 CMP R3, #0x10
 BNE CheckColumn3
 MOV R0, R5
 LSL R0, #2
 ADD R0, #3
 B KeypadReturn
 
CheckColumn3
 CMP R3, #0x20
 BNE NoChar
 MOV R0, R5
 LSL R0, #2
 ADD R0, #4
 B KeypadReturn

NextRow 
; Shift the mask over 1 bit to energize next row
 LSL R4, R4, #1
; Increment the counter
 ADD R5, R5, #1
 CMP R5, #4
 BNE KeypadLoop

NoChar
 MOV R0, #0
KeypadReturn
 LDR R1, =KeypadLookupTable
 LDRB R2, [R1, R0]
 MOV R0, R2
 
 LDR R2, =lastScan
 LDRB R3, [R2]
 CMP R3, R0
 BEQ IncDebounceCount
ResetDebounceCount
; Update lastScan with currentValue
 STRB R0, [R2]
; Reset Debounce counter
 LDR R2, =debounceCount
 MOV R3, #0
 STR R3, [R2]
 B KeypadScan
 
IncDebounceCount
 LDR R2, =debounceCount
 LDR R3, [R2]
 ADD R3, #1
 STR R3, [R2]
 CMP R3, #debounceMax
 BNE KeypadScan
 
; Restore saved registers
 LDR R10, [SP], #4
 LDR R6, [SP], #4
 LDR R5, [SP], #4
 LDR R4, [SP], #4
 LDR R3, [SP], #4
 LDR R2, [SP], #4
 LDR R1, [SP], #4

 BX LR
 
 
 ALIGN
 END
 