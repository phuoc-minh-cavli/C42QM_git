// ****************************************************************************
// QUALCOMM Proprietary Design Data
// Copyright (c) 2015, Qualcomm Technologies Incorporated. All rights reserved.
// ****************************************************************************
// ================================================================================
//                                 EDIT HISTORY
//
// $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/DDRTargetLib/mem_rdwr.s#1 $
// $DateTime: 2018/08/08 05:31:55 $
// $Author: pwbldsvc $
// ================================================================================
// when       who          what, where, why
// --------   ---          --------------------------------------------------------
// 05/11/14   arindamm     First edit history header. Add new entries at top.
// ================================================================================
                        .section DDRSS_MEM_RDWR, "ax", %progbits

//====================================================================================================
// void __blocksCopy(uint32 *source, uint32 *destination, uint32 num_words)//
// Register usage:
// Passed parameters:
//   X0: uint32 *source
//   X1: uint32 *destination
//   X2: uint32 num_words. Also = number of uint32s. Should be an even number.
// Function-internal register usage:
//   X3: Number of uint64s to copy.
//====================================================================================================
                        .global  __blocksCopy
                        .align 0x4
.type __blocksScopy, %function
__blocksCopy:
                        MOVS    r3,r2, LSR #3           // Number of eight word multiples.
                        BEQ     readwords               // Less than eight words to move?
                        STMFD   sp!, {r4-r11}           // Save some working registers.
octread:                LDMIA   r0!, {r4-r11}           //; Load 8 words from the source
                        STMIA   r1!, {r4-r11}           // and put them at the destination.
                        SUBS    r3, r3, #1              // Decrement the counter.
                        BNE     octread                 // ... copy more.
                        LDMFD   sp!, {r4-r11}           // Dont need these now - restore
                                                        //
readwords:              ANDS    r2, r2, #7              // Number of odd words to copy.
                        BEQ     ret                     // No words left to copy?
                                                        //
copy:                   STMFD   sp!, {r4-r10}           // Save some working registers.
                        cmp     r2,#1                   //
                        beq     oneword                 // write the remaining one word
                        cmp     r2,#2                   //
                        beq     twowords                // do a burst of two words
                        cmp     r2,#3                   //
                        beq     threewords              // do a burst of three words
                        cmp     r2,#4                   //
                        beq     fourwords               // do a burst of four words
                        cmp     r2,#5                   //
                        beq     fivewords               // do a burst of five words
                        cmp     r2,#6                   //
                        beq     sixwords                // do a burst of six words
                        cmp     r2,#7                   //
                        beq     sevenwords              // do a burst of seven words
                        b       stop

oneword:                ldr     r3, [r0]
                        str     r3, [r1]
                        b       stop

twowords:               ldmia   r0!,{r4-r5}
                        stmia   r1!,{r4-r5}
                        b       stop

threewords:             ldmia   r0!,{r4-r6}
                        stmia   r1!,{r4-r6}
                        b       stop

fourwords:              ldmia   r0!,{r4-r7}
                        stmia   r1!,{r4-r7}
                        b       stop

fivewords:              ldmia   r0!,{r4-r8}
                        stmia   r1!,{r4-r8}
                        b       stop

sixwords:               ldmia   r0!,{r4-r9}
                        stmia   r1!,{r4-r9}
                        b       stop

sevenwords:             ldmia   r0!,{r4-r10}
                        stmia   r1!,{r4-r10}

stop:                   LDMFD   sp!, {r4-r10}           //restore r4 to r11
ret:                     BX      lr                      // Return.
__endBlocksCopy:

                       .end
                       