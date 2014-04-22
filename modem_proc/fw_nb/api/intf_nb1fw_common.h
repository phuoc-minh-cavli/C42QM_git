/*!
  @file
  intf_nb1fw_common.h

  @brief
  FW-SW interface message common definitions NB1 FW

  @detail
  FW-SW interface message common definitions NB1 FW

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_common.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_COMMON_H
#define INTF_NB1FW_COMMON_H

#include "msgr.h"
#include "msgr_nb1.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

/* defines message structure from message payload definition */
#define NB1FW_INTF_MSG_S(msg_name)        \
typedef struct                            \
{                                         \
  msgr_hdr_struct_type    msg_hdr;        \
  nb1fw_##msg_name##_s    msg_payload;    \
} nb1fw_##msg_name##_msg_s

/* declares structure member for given message type */
#define NB1FW_INTF_MSG(msg_name) nb1fw_##msg_name##_msg_s msg_name

/*===========================================================================

      Typedefs

===========================================================================*/

/* Request action time type enum */
typedef enum
{

  NB1FW_ACT_TYPE_TIME_REF = 0, /* Use the action time specified on ref timeline */
  NB1FW_ACT_TYPE_TIME = NB1FW_ACT_TYPE_TIME_REF, /* default aliased to ref */
  NB1FW_ACT_TYPE_TIME_DL,      /* Use the action time specified on DL timeline */
  NB1FW_ACT_TYPE_TIME_UL,      /* Use the action time specified on UL timeline */
  NB1FW_ACT_TYPE_ASAP          /* ignore the action time and do ASAP */
} nb1fw_action_type_e;

/* Request confirmation status enum */
typedef enum
{
  NB1FW_CONF_REQ_COMPLETED = 0,
  NB1FW_CONF_REQ_HALT,
  NB1FW_CONF_REQ_IGNORED,
  NB1FW_CONF_REQ_COMPLETED_WITH_IGNORED_ACT_TIME
} nb1fw_cnf_status_e;

/* action time structure */
typedef union
{
  struct
  {
    uint16 subframe_num : 4;  /* 0-9 */
    uint16 frame_num : 10;    /* radio frame number 0..1023 */
    uint16 type : 2;          /* indicate action time type as in nb1fw_action_type_e */
  };
  uint16 value;
} nb1fw_action_time_s;

/* action time structure */
typedef union
{
  struct
  {
    uint16 subframe_num : 4;  /* 0-9 */
    uint16 frame_num : 10;    /* radio frame number 0..1023 */
    uint16 : 2;
  };
  uint16 value;
} nb1fw_sys_time_s;

/* Request message header */
typedef struct
{
  uint8 seq_id;                    /* message sequence Id */
  nb1fw_action_time_s action_time; /* action time for requests */
} nb1fw_req_hdr_s;

/* Confirmation message header */
typedef struct
{
  uint8 seq_id;                 /* message sequence Id */
  nb1fw_cnf_status_e status;    /* confirm status for request */
} nb1fw_cnf_hdr_s;

/* Request message header */
typedef struct
{
  nb1fw_sys_time_s time; /* time stamp corresponding to indication */
} nb1fw_ind_hdr_s;

#endif /* INTF_NB1FW_COMMON_H */

