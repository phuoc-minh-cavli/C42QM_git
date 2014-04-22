#ifndef L2_L1_G_H
#define L2_L1_G_H

/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl2/inc/l2_l1_g.h#2 $    $DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------    ---     --------------------------------------------------------
13/09/12    SJW     Refactored for QDSP6 DSDS porting
dd/mm/yy    SJW     Sample text for edit history
===========================================================================*/

/* Alphabetical Order */
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rr_l2.h"
#include "rr_l2_l1.h"
#include "sys_cnst.h"
#include "sys_type.h"
#include "geran_multi_sim.h"

//#ifdef FEATURE_GERAN_CS_SUPPORT
typedef enum
{
  L2_NOT_REPETITION_CANDIDATE, /* L1 should not buffer this data */
  L2_SET_REPETITION_CANDIDATE, /* L1 should buffer this data */
  L2_USE_REPETITION_CANDIDATE  /* L1 should ignore this data and use its buffer */
} l2_l1_repetition_T;

/**********************************************************/
/* PH_DATA_REQ primitive :                                */
/* FROM :  LAYER 2                                        */
/* TO   :  LAYER 1                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*    Message sent to layer 1 requesting the transmission */
/* of the associated frame when next possible, on the     */
/* indicated channel (One of SACCH or DCCH)               */
/*                                                        */
/* This is sent in response to a PH_READY_TO_SEND_IND     */
/* being received by layer 2                              */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T              message_header;
   l2_channel_type_T  l2_channel_type;
   l2_l1_repetition_T l2_repetition_info;
   byte               layer2_frame[N201_MAX];
} ph_data_req_T;

/**********************************************************/
/* PH_EMPTY_FRAME_REQ primitive :                         */
/* FROM :  LAYER 2                                        */
/* TO   :  LAYER 1                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*   Message sent to layer 1 requesting the transmission  */
/* of an empty frame. Used when layer 2 has no            */
/* outstanding frames to be sent when it receives         */
/* PH_READY_TO_SEND_IND                                   */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   l2_channel_type_T l2_channel_type;
} ph_empty_frame_req_T;
//#endif /* FEATURE_GERAN_CS_SUPPORT */

/**********************************************************/
/* PH_RANDOM_ACCESS_REQ primitive :                       */
/* FROM :  LAYER 2                                        */
/* TO   :  LAYER 1                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*  Message sent to layer 1 requesting the transmission   */
/* of 1 or 4 (when part of finely synchronised handover)  */
/* random access bursts.                                   */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   byte              random_access_content;
   boolean           egprs_ra_content_present;
   uint16            egprs_ra_content;
   uint8             egprs_tsc;
   l2_channel_type_T l2_channel_type;
   boolean           pwr_reduction_valid;
   uint8             pwr_reduction_value;
   boolean           emergency_call;
   boolean           rach_for_ps_traffic;
} ph_random_access_req_T;

/**********************************************************/
/* PH_RANDOM_ACCESS_IND primitive :                       */
/* FROM :  LAYER 2                                        */
/* TO   :  LAYER 1                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*  Message sent to Layer 1 requesting permission to      */
/*  begin random access procedures on behalf of GRR       */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   l2_channel_type_T l2_channel_type;
   boolean           rach_for_ps_traffic;
   boolean           rach_repeat_ind;
} ph_random_access_ind_T;

//#ifdef FEATURE_GERAN_CS_SUPPORT
/**********************************************************/
/* PH_STOP_SACCH_MEASUREMENT_REPORT primitive :           */
/* FROM :  LAYER 1                                        */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/*                                                        */
/*    Message from layer 1 to layer 2 indicating that the */
/* buffer in layer 1 corresponding to the channel type    */
/* is available to receive the next message to transmit   */
/* on that channel                                        */
/*                                                        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   uint16            dummy;   /* That prevents static analysis error */
} ph_stop_sacch_measurement_report_T;
//#endif /* FEATURE_GERAN_CS_SUPPORT */

#ifdef FEATURE_GERAN_CS_SUPPORT
extern void l2_generate_filler(byte *, uint16);
#endif /* FEATURE_GERAN_CS_SUPPORT */

#endif
