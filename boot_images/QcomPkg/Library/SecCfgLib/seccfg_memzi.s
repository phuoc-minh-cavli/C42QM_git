
.text
.align 2

    GCC_ASM_EXPORT (qsee_memzi)

qsee_memzi:
   vmov.i64 Q0, #0x0
   vmov.i64 Q1, Q0
   mov      r2, #0x0

   cmp      r1,   #0x80           // check bigger than 128 bytes or not.
   blt      check_zi_32byte_left_over
   
   vmov.i64 Q2, Q0
   vmov.i64 Q3, Q0
   vmov.i64 Q4, Q0
   vmov.i64 Q5, Q0
   vmov.i64 Q6, Q0
   vmov.i64 Q7, Q0
   b        loop_for_128byte_set

check_zi_32byte_left_over:
   cmp      r1,   #0x20   
   bge      loop_for_32byte_set
check_zi_byte_left_over:
   cmp      r1,   #0    
   bgt      loop_for_byte_set
tzbsp_memzi_end:
   bx       lr
loop_for_128byte_set:
   vstm     r0!, {Q0-Q7}
   sub      r1,   r1, #0x80
   cmp      r1,   #0x80        
   bge      loop_for_128byte_set
   blt      check_zi_32byte_left_over  
loop_for_32byte_set:
   vstm     r0!, {Q0-Q1}
   sub      r1,   r1, #0x20
   cmp      r1,   #0x20
   bge      loop_for_32byte_set
   blt      check_zi_byte_left_over
loop_for_byte_set:
   strb     r2,   [r0], #1
   sub      r1,   r1, #0x1
   cmp      r1,   #0x0
   bgt      loop_for_byte_set
   ble      tzbsp_memzi_end
   


