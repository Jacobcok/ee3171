 AREA |.text|, CODE, READONLY, ALIGN=2
 EXPORT __main
__main
 LDR R0, =j
 LDR R1, [R0]
 LDR R2, =k
 LDR R3, [R2]
 MOV R4, #0
 MOV R5, #0
TheLabel
 CMP R3, R4
 BEQ TheEnd
 ADD R5, R5, R1
 ADD R4, R4, #1
 B TheLabel
 
TheEnd
 LDR R6, =theAnswer
 STR R5, [R6]
 
BlackHole B BlackHole

 ALIGN
 
 EXPORT SystemInit
SystemInit
 LDR R0, =j
 MOV R1, #5
 STR R1, [R0]
 LDR R0, =k
 MOV R1, #0x3
 STR R1, [R0]
 LDR R0, =theAnswer
 MOV R1, #0x00
 STR R1, [R0]
 BX LR
 
 ALIGN
 
 AREA DATA, READWRITE, ALIGN=2
 EXPORT j [DATA, SIZE=4]
j SPACE 4
 EXPORT k [DATA, SIZE=4]
k SPACE 4
 EXPORT theAnswer [DATA, Size=4]
theAnswer SPACE 4

 ALIGN
 END