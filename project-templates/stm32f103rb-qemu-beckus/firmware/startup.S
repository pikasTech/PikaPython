.syntax unified
.cpu cortex-m3
.thumb

.section .isr_vector, "a", %progbits
.align 7
.word __stack_top
.word Reset_Handler
.rept 14
.word Default_Handler
.endr

.section .text.Reset_Handler, "ax", %progbits
.global Reset_Handler
.type Reset_Handler, %function
.thumb_func
Reset_Handler:
  ldr r0, =_data_load
  ldr r1, =_data_start
  ldr r2, =_data_end
1:
  cmp r1, r2
  bcs 2f
  ldr r3, [r0], #4
  str r3, [r1], #4
  b 1b
2:
  ldr r0, =_bss_start
  ldr r1, =_bss_end
  movs r2, #0
3:
  cmp r0, r1
  bcs 4f
  str r2, [r0], #4
  b 3b
4:
  ldr r0, =__stack_bottom
  ldr r1, =__stack_top
  ldr r2, =0xa5a5a5a5
5:
  cmp r0, r1
  bcs 6f
  str r2, [r0], #4
  b 5b
6:
  ldr sp, =__stack_top
  bl main
7:
  b 7b
.size Reset_Handler, . - Reset_Handler

.section .text.Default_Handler, "ax", %progbits
.global Default_Handler
.type Default_Handler, %function
.thumb_func
Default_Handler:
  b Default_Handler
.size Default_Handler, . - Default_Handler

.section .text.pika_semihost_call, "ax", %progbits
.global pika_semihost_call
.type pika_semihost_call, %function
.thumb_func
pika_semihost_call:
  bkpt 0xab
  bx lr
.size pika_semihost_call, . - pika_semihost_call
