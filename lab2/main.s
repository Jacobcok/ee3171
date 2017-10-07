GPIO_PORTD_DATA_R EQU 0x400073FC
GPIO_PORTD_DIR_R EQU 0x40007400
GPIO_PORTD_DEN_R EQU 0x4000751C
GPIO_PORTD_AMSEL_R EQU 0x40007528
GPIO_PORTB_DATA_R EQU 0x400053FC
GPIO_PORTB_DIR_R EQU 0x40005400
GPIO_PORTB_DEN_R EQU 0x4000551C
SYSCTL_RCGCGPIO_R EQU 0x400FE608

 AREA |.text|, CODE, READONLY, ALIGN=2
 EXPORT __main
__main
 LDR R0, =PortConfiguration
 BLX R0
 
 LDR R0, =GPIO_PORTB_DATA_R
 LDR R1, =GPIO_PORTD_DATA_R
 
 MOV R6, #0
 
;RunForever
; LDR R2, [R1]
; STR R2, [R0]
; B RunForever

; turn the LED3 on
MainLoop 
 MOV R2, #0x08
 STR R2, [R0]
 
; delay 0.25 secs
DelayLoop CMP R6, #2
 ADD R6, R6, #1
 BNE DelayLoop
 MOV R6, #0
 
; turn LED3 off
 MOV R2, #0
 STR R2, [R0]
 
 B MainLoop

PortConfiguration
; Port B initializations first
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
 ORR R1, R1, #0x08
 STR R1, [R0]
 LDR R0, =GPIO_PORTB_DEN_R
 LDR R1, [R0]
 ORR R1, R1, #0x08
 STR R1, [R0]
 
; Port D initializaion
 LDR R0, =SYSCTL_RCGCGPIO_R
 LDR R1, [R0]
 ORR R1, R1, #0x08
 STR R1, [R0]
 NOP
 NOP
 NOP
 NOP
 LDR R0, =GPIO_PORTD_DIR_R
 LDR R1, [R0]
 BIC R1, R1, #0x0F
 STR R1, [R0]
 LDR R0, =GPIO_PORTD_AMSEL_R
 LDR R1, [R0]
 BIC R1, R1, #0x0F
 STR R1, [R0]
 LDR R0, =GPIO_PORTD_DEN_R
 LDR R1, [R0]
 ORR R1, R1, #0x0F
 STR R1, [R0]
 
 BX LR
 
 EXPORT SystemInit
SystemInit
 BX LR
 
 ALIGN
 END
 
 