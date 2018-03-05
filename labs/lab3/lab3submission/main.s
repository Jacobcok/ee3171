 AREA DATA, READWRITE, ALIGN=2
 EXPORT comboDigit1 [DATA, SIZE=1]
comboDigit1 SPACE 1
 EXPORT comboDigit2 [DATA, SIZE=1]
comboDigit2 SPACE 1
 EXPORT comboDigit3 [DATA, SIZE=1]
comboDigit3 SPACE 1
 EXPORT comboDigit4 [DATA, SIZE=1]
comboDigit4 SPACE 1

 EXPORT currentLEDState [DATA, SIZE=1]
currentLEDState SPACE 1

 ALIGN

 AREA |.text|, CODE, READONLY, ALIGN=2
 IMPORT KeypadInit
 IMPORT BuildKeypadLookupTable
 IMPORT KeypadGetKey
 
 EXPORT __main
__main

; Call initialization subroutines here.
 LDR R0, =BuildKeypadLookupTable
 BLX R0 
 LDR R0, =KeypadInit
 BLX R0 
 LDR R0, =PortConfiguration
 BLX R0
 

ComboStart
 LDR R8, =comboDigit1
 LDRB R9, [R8]
 
 LDR R11, =currentLEDState
 MOV R12, #0x08
 STRB R12, [R11] 

; This is the main program loop.
Loop
 LDR R0, =KeypadGetKey
 BLX R0
 
; If no key pressed, just check again.
 CMP R0, #0x00
 BEQ Loop
 
; Compare with the current digit of the combination.
 CMP R0, R9 ; check KeypadGetKey = comboDigit value
 BEQ UpdateLED
 
; Reset everything if it's the wrong key
 B ResetCombo
 
; Update LEDs if it's the right key
UpdateLED 
; LDR R12, =currentLEDState
 LDR R4, =GPIO_PORTB_DATA_R
 STRB R12, [R4] ; currentLEDstate turns on LEDs
 LSR R12, R12, #0x01 ; shift LED from left to right
 ADD R12, #8
 CMP R9, #'5'
 BEQ TheEnd
 
; Move to the next character 
 ADD R8, #1 ; increment address of combo digit by 1 byte
 LDRB R9, [R8] ; loads the value of the combo digit into R9
  
; Do it all again
 B Loop

ResetCombo
; Turn off the LEDs, then restart 
 LDR R4, =GPIO_PORTB_DATA_R
 MOV R12, #0
 STR R12, [R4]
 B ComboStart

; Here is a final infinite loop for successful 
; input of the whole combination.
TheEnd B TheEnd

 EXPORT SystemInit
SystemInit
; These lines initialize the combination:
; 1885
 LDR R0, =comboDigit1
 MOV R1, #'1'
 STRB R1, [R0]
 
 LDR R0, =comboDigit2
 MOV R1, #'8'
 STRB R1, [R0]
 
 LDR R0, =comboDigit3
 MOV R1, #'8'
 STRB R1, [R0]
 
 LDR R0, =comboDigit4
 MOV R1, #'5'
 STRB R1, [R0]
  
; Put any custom initializations here. 
 
 BX LR

GPIO_PORTB_DATA_R  EQU 0x400053FC
GPIO_PORTB_DIR_R   EQU 0x40005400
GPIO_PORTB_DEN_R   EQU 0x4000551C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

; Put all needed initializations here.
; They should be the same as Lab 2.
PortConfiguration
; LED Port B initializations 
 LDR R0, =SYSCTL_RCGCGPIO_R
 LDR R1, [R0]
 ORR R1, R1, #0x02
 STR R1, [R0]
 NOP
 NOP
 NOP
 NOP
 LDR R0, =GPIO_PORTB_DIR_R
 LDR R1, [R0]
 ORR R1, R1, #0x0F
 STR R1, [R0]
 LDR R0, =GPIO_PORTB_DEN_R
 LDR R1, [R0]
 ORR R1, R1, #0x0F
 STR R1, [R0]
 
 BX LR

LEDInit

 BX LR
 
 ALIGN
 
 END
 