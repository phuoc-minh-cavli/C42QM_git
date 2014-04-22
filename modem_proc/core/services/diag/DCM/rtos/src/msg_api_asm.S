
/* Copyright (c) 2018 by QUALCOMM Technologies, Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary 

 ===========================================================================
                         EDIT HISTORY FOR FILE

 when       who     what, where, why
 --------   ---   ----------------------------------------------------------
 06/20/18   nk      Added changes for optimization of DIAG MSG apis
 --------   ---   ----------------------------------------------------------
 
  Diag message call before optimization
  {r0 = ##some.xx_msg_v3_const // 8 bytes const address
   call msg_v3_send }
 
  Diag message call after optimization
  { call msg_v3_send_pcrelR0 }
  { Rx = add (Ry, #some.xx_msg_v3_const) } //This takes only 4 bytes
 
  Thus we save 4 bytes per message.
  Here Assembler marks the "add" instruction with special relocation. 
  Linker uses relocation and puts the message offset into the instruction.
 
  The offset is encoded in the add instruction and can be extracted using the mask 0x0fff3fef 
  We have only 25 free bits to use in add instruction(which is 32-bits) without disturbing 
  the functionality of add instruction. As the const - address is always 4-byte aligned, we 
  can have 2 extra bits to store offset. Hence maximum offset can be of 27 bits.
  We will exceed the maximum offset if we apply the optimization to all MSG api's. 
  Hence optimization is applied to only msg_v3_send/1/2/3 api's
 
  The purpose of each stub is to calculate const address using the offset and load it into R0
  Each stub does following
 - Fetch instruction word from RP
 - Extract 25 bits representing bit mask 0x0fff3fef using extracts and shifts
 - Shift that left by 2 since all _send messages are atleast 4 byte aligned
 - Add the 27 bit offset to MSG_BASE specified in linker script
 - Jump to the original function with above as first argument
*/

#ifdef __HEXMSGABI_3_SUPPORTED__
  .text
  .globl msg_v3_send_pcrelR0
  .type  msg_v3_send_pcrelR0, @function
msg_v3_send_pcrelR0:
  {
    r0 = memw(r31)
  }
  {
    r7 = extractu(r0, #9, #5)
    r6 = extractu(r0, #4, #0)
  }
  {
    r8 = extractu(r0, #12, #16)
    r6 += asl(r7, #4)
  }
  {
    r6 += asl(r8, #13)
    r0 = ##_MSG_BASE_
  }
  {
    r0 += asl(r6, #2)
    jump msg_v3_send
  }
  .size  msg_v3_send_pcrelR0, .-msg_v3_send_pcrelR0

  .globl msg_v3_send_1_pcrelR0
  .type  msg_v3_send_1_pcrelR0, @function
msg_v3_send_1_pcrelR0:
  {
    r0 = memw(r31)
  }
  {
    r7 = extractu(r0, #9, #5)
    r6 = extractu(r0, #4, #0)
  }
  {
    r8 = extractu(r0, #12, #16)
    r6 += asl(r7, #4)
  }
  {
    r6 += asl(r8, #13)
    r0 = ##_MSG_BASE_
  }
  {
    r0 += asl(r6, #2)
    jump msg_v3_send_1
  }
  .size  msg_v3_send_1_pcrelR0, .-msg_v3_send_1_pcrelR0

  .globl msg_v3_send_2_pcrelR0
  .type  msg_v3_send_2_pcrelR0, @function
msg_v3_send_2_pcrelR0:
  {
    r0 = memw(r31)
  }
  {
    r7 = extractu(r0, #9, #5)
    r6 = extractu(r0, #4, #0)
  }
  {
    r8 = extractu(r0, #12, #16)
    r6 += asl(r7, #4)
  }
  {
    r6 += asl(r8, #13)
    r0 = ##_MSG_BASE_
  }
  {
    r0 += asl(r6, #2)
    jump msg_v3_send_2
  }
  .size  msg_v3_send_2_pcrelR0, .-msg_v3_send_2_pcrelR0

  .globl msg_v3_send_3_pcrelR0
  .type  msg_v3_send_3_pcrelR0, @function
msg_v3_send_3_pcrelR0:
  {
    r0 = memw(r31)
  }
  {
    r7 = extractu(r0, #9, #5)
    r6 = extractu(r0, #4, #0)
  }
  {
    r8 = extractu(r0, #12, #16)
    r6 += asl(r7, #4)
  }
  {
    r6 += asl(r8, #13)
    r0 = ##_MSG_BASE_
  }
  {
    r0 += asl(r6, #2)
    jump msg_v3_send_3
  }
  .size  msg_v3_send_3_pcrelR0, .-msg_v3_send_3_pcrelR0
#endif
