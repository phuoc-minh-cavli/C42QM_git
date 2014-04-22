;/* Invokes SMC instruction to call into Trustzone */

#if defined(__RVCT_GNU__) || defined(__RVCT__)
    AREA    |.text|, CODE
#endif

   EXPORT tz_smc2

   CODE32

;TZ SMC call
;r0 -> array of 6 args
;Supprting 32-bits only
tz_smc2
  push {r4-r7}
	mov r7, r0
	mov r6, #0
do_smc2
	ldr r0, [r7]
	ldr r1, [r7, #4]
	ldr r2, [r7, #8]
	ldr r3, [r7, #12]
	ldr r4, [r7, #16]
	ldr r5, [r7, #20]
   smc 0
   cmp r0,#1         ;re-try if we were interrupted
	beq do_smc2
	str r1, [r7]
	str r2, [r7, #4]
	str r3, [r7, #8]
  pop {r4-r7}
   bx  lr

  END
