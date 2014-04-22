;/* Invokes SMC instruction to call into Trustzone */

#if defined(__RVCT_GNU__) || defined(__RVCT__)
    AREA    |.text|, CODE
#endif

   EXPORT tz_syscall
   EXPORT tz_smc

   CODE32

;/* r0 - Is fixed to argument count 1
;   r1 - Unique context id (stack pointer will do)
;   r2 - Pointer to command memory area
;*/
tz_syscall
   mov r2,r0
   mov r0,#1
   mov r1,#0
do_smc
   smc 0x0
   adds r0,#0
   bgt do_smc
   bx lr

;TZ SMC call
tz_smc
   smc 0x0
   bx lr

  END
