#line 1 "Z:\\b\\apps_proc\\core\\kernel\\rtos\\threadx\\txm_demo_app\\src\\txm_module_preamble.S"
        AREA  Init, CODE, READONLY
        CODE32
;     

    EXPORT __txm_module_preamble


;     

    IMPORT demo_module_start
    IMPORT demo_module_stop


;     

   IMPORT  _txm_module_thread_shell_entry
   IMPORT  _txm_module_callback_request_thread_entry
   IMPORT  |Image$$ER_RO$$Length|
   IMPORT  |Image$$ER_RW$$Length|

	IMPORT  |Image$$ER_ZI$$ZI$$Length|


__txm_module_preamble
        DCD       0x4D4F4455                                        ; Module ID
        DCD       0x5                                               ; Module Major Version
        DCD       0x3                                               ; Module Minor Version
        DCD       32                                                ; Module Preamble Size in 32-bit words
        DCD       0x12345678                                        ; Module ID (application defined) 
        DCD       0x01000001                                        ; Module Properties where:
                                                                    ;   Bits 31-24: Compiler ID
                                                                    ;           0 -> IAR
                                                                    ;           1 -> RVDS
                                                                    ;           2 -> GNU
																	;			4 -> LLVM
                                                                    ;   Bits 23-1:  Reserved
                                                                    ;   Bit 0:  0 -> Privileged mode execution (no MMU protection)
        DCD       _txm_module_thread_shell_entry - . + .            ; Module Shell Entry Point
        DCD       demo_module_start - . + .                         ; Module Start Thread Entry Point
        DCD       demo_module_stop - . + .                          ; Module Stop Thread Entry Point 
        DCD       20                                                ; Module Start/Stop Thread Priority
        DCD       2046                                              ; Module Start/Stop Thread Stack Size
        DCD       _txm_module_callback_request_thread_entry - . + . ; Module Callback Thread Entry
        DCD       25                                                ; Module Callback Thread Priority     
        DCD       2046                                              ; Module Callback Thread Stack Size   
        DCD       |Image$$ER_RO$$Length|                            ; Module Code Size

        DCD       |Image$$ER_ZI$$ZI$$Length|                        ; Module data size - get it from ZI section
        DCD       __txm_module_preamble                             ; Reserved 0
        DCD       |Image$$ER_RW$$Length|                            ; Reserved 1





        DCD       0                                                 ; Reserved 2
        DCD       0                                                 ; Reserved 3
        DCD       0                                                 ; Reserved 4
        DCD       0                                                 ; Reserved 5     
        DCD       0                                                 ; Reserved 6     
        DCD       0                                                 ; Reserved 7   
        DCD       0                                                 ; Reserved 8  
        DCD       0                                                 ; Reserved 9

		DCD       0                            						; txm_module_preamble_debug_enable 
																	; 1 : enabled
																	; 0 : disabled



        DCD       0                                                 ; Reserved 11
        DCD       0                                                 ; Reserved 12
        DCD       0                                                 ; Reserved 13

        DCD       0xABCDEF01                                        ; Reserved 14 : Module Unique ID



        DCD       0                                                 ; Reserved 15

        END


