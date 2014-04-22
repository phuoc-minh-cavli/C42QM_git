#ifndef MSG_I_H
#define MSG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing internal declarations.

Copyright (c) 2007-2012,2014-2015, 2019-2020 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.tx/6.0/services/diag/LSM/common/src/msgi.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/15   rh      Removed spurious status field
06/25/14   rs      Added message mask table definition for build mask centralization. 
11/15/12   is      Support for preset masks
08/31/12   sr      Support for mask retrieval command  
08/14/12   ra      Added prototype for msg_mask_binary_search
08/10/12   sa      Mainlined DIAG_CENTRAL_ROUTING
07/05/12   ra      Features for F3 Mask Check Optimization 
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
12/23/10   mad     Added prototype for msg_get_ssid_masks_adv 
05/10/10   sg      moved msg_mask type from msg.c to msgi.h
01/15/09   mad     defined MSG_I_H, updated copyright
12/02/08   mad     Created file.
===========================================================================*/

#include "diagcomm_ctrl.h" /* For diag_ctrl_msg_type */


typedef struct{
  uint32 args[10];
}msg_large_args;

#define MSG_LARGE_ARGS(X) (((msg_large_args *)X)->args)

typedef enum
{
  MSG_MASK_BT,          /* Build-time mask */
  MSG_MASK_RT           /* Run-time mask */
} msg_mask_type;

#if defined(DIAG_MP_MASTER)
typedef struct
{
  uint16 ssid_first;      /* Start of range of supported SSIDs */
  uint16 ssid_last;       /* Last SSID in range */
  uint16 ssid_last_max;   /* Max SSID in range */
  uint16 mask_array_max;  /* Length of BT mask array */
//  uint8 * status[DIAG_MAX_STREAM_ID];

  /* Pointer to array of (ssid_last - ssid_first + 1) runtime masks */
  uint32 * rt_mask_array[DIAG_MAX_STREAM_ID];

  /* pointers to backing masks of the above rt_mask_array and status */
  uint32 * rt_preset_mask[DIAG_MAX_PRESET_ID]; /* Maintain the tools run time mask for msg mask table for each preset id. */
  uint32 * rt_dci_mask[DIAG_MAX_PRESET_ID]; /* Maintain the DCI run time mask for msg mask table for each preset id. */
  uint8 preset_status[DIAG_MAX_PRESET_ID]; /* Maintain the update status for Log/Event/Msg service for STREAM 1 */

  #ifdef DIAG_CONSUMER_API
    uint8 dci_status[DIAG_MAX_DCI_ID]; /* Maintain the update status for Log/Event/Msg service for STREAM 2 (i.e DCI)*/
    uint8 msg_mask_dci_status[NUM_SMD_PORTS][DIAG_MAX_DCI_ID]; /* Maintain the update status for msg mask table for STREAM 2 (DCI) */
  #endif /* DIAG_CONSUMER_API */
  uint8 msg_mask_preset_status[NUM_SMD_PORTS][DIAG_MAX_PRESET_ID]; /* Maintain the update status for msg mask table for STREAM 1 */


  /* Pointer to array of (ssid_last - ssid_first + 1) build masks */
  uint32 *bt_mask_array;
} diag_msg_mask_mstr_type;

typedef struct
{
  int mask_count;
  diag_msg_mask_mstr_type *bld_masks;
} diag_msg_mask_mstr_tbl_type;

/* Array of (ssid_last - ssid_first + 1) masks */
extern diag_msg_mask_mstr_tbl_type diag_msg_mask_mstr_tbl;                /* Message masks */
extern diag_msg_mask_mstr_type *diag_msg_rt_mask_mstr_tbl;                /* Kept around as a pointer into the above to save development time */
#endif

uint8 msg_mask_binary_search (uint16 curr_ssid, boolean scan_mstr_tbl);
boolean msg_get_ssid_masks_adv (msg_mask_type mask_type, uint16 ssid_start,
                                uint16 ssid_end, uint32 * mask, byte stream_id, uint8 preset_id,boolean scan_mstr_tbl);

void msg_set_all_rt_masks_adv (uint32 mask, byte stream_id, uint8 preset_id);
                                
void diag_switch_msg_preset_mask( void );

PACK(void *) msg_pkt_get_local_masks(PACK(void *) req_pkt, uint16 pkt_len);
 
PACK(void *) msg_pkt_get_local_ssid_range(PACK(void *) req_pkt, uint16 pkt_len);


/*===========================================================================

FUNCTION MSG_GET_QSR4_TABLE_ENTRY

DESCRIPTION
  This routine calculate the index in the msg_v4_ss_aam_table or msg_v4_ss_uam_table 
  and extract the ssid and mask from the msg_v4_ss_table accordingly.

PARAMETERS:
  qsr_hash_v4 - Hash value
  msg_ssid  - SSID will be populated from msg_v4_ss_table
  msg_mask  - MASK will be populated from msg_v4_ss_table

RETURN VALUE
 TRUE - QSR hash value lies within the qsr 4 aligned or unaligned boundaries.
 FALSE - otherwise.

===========================================================================*/

boolean msg_get_qsr4_table_entry(uint32 qsr_hash_v4, uint16 *msg_ssid, uint32 *msg_mask);


/* On MP Master only */
#if defined(DIAG_MP_MASTER)
void diagcomm_ctrl_send_msg_mask( diagcomm_port_type port_type, 
                                  diagcomm_enum_port_type port_num, 
                                  uint8 stream_id,
                                  uint8 preset_id,
                                  boolean send_now );
#endif /*DIAG_MP_MASTER */

/* On MP Slave only */
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
void diag_ctrl_update_msg_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_update_msg_preset_mask( diag_ctrl_msg_type * ctrl_msg );
#endif /*DIAG_MP && !DIAG_MP_MASTER */

/* For F3 Mask Check Optimization - Used to set
   corresponding bits in msg_enabled_mask.
   These denote if a msg mask / listener is turned
   "on" or "off"
*/
#define DIAG_MSG_NONE (0x00)
#define DIAG_MSG_STREAM_1 (0x01)
#define DIAG_MSG_STREAM_2 (0x02)
#define DIAG_MSG_LISTENER (0x04)
#define DIAG_MSG_EXT_LISTENER (0x08)

#endif /* MSG_I_H */
