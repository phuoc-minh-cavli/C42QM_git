/* @file devprg_packet_id.h

  Packet IDs for HSUART packets

  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------
2020-07-01   as      Create

=============================================================================*/

#ifndef __DEVPRG_PACKET_ID_H__
#define __DEVPRG_PACKET_ID_H__

/* Below enum values are so chosen that bit flips of one packet id gets detected
   as corrupt id and minimises the possibility of changing to another packet id.
   For eg: END_OF_TRANSFER and VERSION packet has are 8 bit differences.So it will 
   take 8 bit flips for END_OF_TRANSFER to change to VERSION.
   0x55 -->	0b01010101
   0xAA --> 0b10101010
*/

enum devprg_packet_id_t {
    PROTOCOL_PACKET = 0xF0,
    END_OF_TRANSFER_PACKET = 0x55,
    ACK_PACKET = 0x06,
    NAK_PACKET = 0x09,
    VERSION_PACKET = 0xAA,
    READY_TO_READ_PACKET = 0x0F,
};

#endif