ldr r0,=0x20200004
mov r1,#1
lsl r1,#18
str r1,[r0]
ldr r2,=0x20200028
mov r3,#1
lsl r3,#16
ldr r4,=0x2020001C
loop:
str r3,[r2]
mov r5,#16777216
loop2:
sub r5,r5,#1
cmp r5,#0
bne loop2
str r3,[r4]
mov r6,#16777216
loop3:
sub r6,r6,#1
cmp r6,#0
bne loop3
b loop