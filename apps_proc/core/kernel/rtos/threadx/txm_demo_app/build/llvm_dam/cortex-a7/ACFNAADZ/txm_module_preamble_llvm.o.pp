# 1 "Z:\\b\\apps_proc\\core\\kernel\\rtos\\threadx\\txm_demo_app\\src\\txm_module_preamble_llvm.S"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 333 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "Z:\\b\\apps_proc\\core\\kernel\\rtos\\threadx\\txm_demo_app\\src\\txm_module_preamble_llvm.S" 2
@ AREA Init, CODE, READONLY
   .section Init, "ax", %progbits
        .CODE32:
@

    .global _txm_module_preamble


@

    .extern demo_module_start


@

   .extern _txm_module_thread_shell_entry
   .extern _txm_module_callback_request_thread_entry
   .extern Image$$ER_RO$$Length
 .extern Image$$ER_RW$$Length

 .extern Image$$ER_ZI$$ZI$$Length


_txm_module_preamble:
        .p2align 2
        .word 0x4D4F4455 @ Module ID
  .p2align 2
        .word 0x5 @ Module Major Version
  .p2align 2
        .word 0x3 @ Module Minor Version
  .p2align 2
        .word 32 @ Module Preamble Size in 32-bit words
  .p2align 2
        .word 0x12345678 @ Module ID (application defined)
  .p2align 2
        .word 0x04000001 @ Module Properties where:
  .p2align 2
                                                                   @ Bits 31-24: Compiler ID
                                                                   @ 0 -> IAR
                                                                   @ 1 -> RVDS
                                                                   @ 2 -> GNU
                   @ 4 -> LLVM
                                                                   @ Bits 23-1: Reserved
                   @ Bit 0: Privileged mode execution
                   @ 0 -> no MMU protection
                   @ 1 -> MMU protection Enabled
        .word _txm_module_thread_shell_entry - . + . @ Module Shell Entry Point
  .p2align 2
        .word demo_module_start - . + . @ Module Start Thread Entry Point
  .p2align 2
        .word 0 @ Module Stop Thread Entry Point
  .p2align 2
        .word 20 @ Module Start/Stop Thread Priority
  .p2align 2
        .word 2046 @ Module Start/Stop Thread Stack Size
  .p2align 2
        .word _txm_module_callback_request_thread_entry - . + . @ Module Callback Thread Entry
  .p2align 2
        .word 25 @ Module Callback Thread Priority
        .p2align 2
        .word 2046 @ Module Callback Thread Stack Size
  .p2align 2
        .word (Image$$ER_RO$$Length) @ Module Code Size

        .p2align 2
        .word (Image$$ER_ZI$$ZI$$Length) @ Module data size - get it from ZI section
  .p2align 2
        .word _txm_module_preamble @ Reserved 0
  .p2align 2
        .word (Image$$ER_RW$$Length) @ Reserved 1
# 79 "Z:\\b\\apps_proc\\core\\kernel\\rtos\\threadx\\txm_demo_app\\src\\txm_module_preamble_llvm.S"
        .p2align 2
        .word 0 @ Reserved 2
  .p2align 2
        .word 0 @ Reserved 3
  .p2align 2
        .word 0 @ Reserved 4
  .p2align 2
        .word 0 @ Reserved 5
        .p2align 2
        .word 0 @ Reserved 6
        .p2align 2
        .word 0 @ Reserved 7
  .p2align 2
        .word 0 @ Reserved 8
        .p2align 2
        .word 0 @ Reserved 9

  .p2align 2
        .word 0 @ txm_module_preamble_debug_enable; 1: enabled; 0:disabled




  .p2align 2
        .word 0 @ Reserved 11
  .p2align 2
        .word 0 @ Reserved 12
  .p2align 2
        .word 0 @ Reserved 13

  .p2align 2
        .word 0xABCDEF01 @ Reserved 14 : Module Unique ID




  .p2align 2
        .word 0 @ Reserved 15

        .end
