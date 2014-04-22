/*!
  @file
  intf_nbfw_app.h

  @brief
  FW-SW interface message definitions for APP enable/disable

  @detail
  description of functions in this file

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/api/intf_nbfw_app.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NBFW_APP_H
#define INTF_NBFW_APP_H

#include "msgr.h"
#include "msgr_nb1.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

/* defines message structure from message payload definition */
#define NBFW_INTF_MSG_S(msg_name)         \
typedef struct                            \
{                                         \
  msgr_hdr_struct_type    msg_hdr;        \
  nbfw_##msg_name##_s    msg_payload;    \
} nbfw_##msg_name##_msg_s

/* declares structure member for given message type */
#define NBFW_INTF_MSG(msg_name) nbfw_##msg_name##_msg_s msg_name

#define NBFW_INTF_NUM_REQ_MSG 2

/*===========================================================================

      Typedefs

===========================================================================*/


/* Type of NB tech instances. */
typedef enum {
  NBFW_TECH_INSTANCE_NB1,
  NBFW_NUM_TECH_INSTANCES
} nbfw_tech_instance_e;

/* NB tech instances state. */
typedef enum {
  NBFW_TECH_INSTANCE_DISABLED,
  NBFW_TECH_INSTANCE_ENABLED
} nbfw_tech_instance_state_e;

/* Request confirmation status enum */
typedef enum
{
  NBFW_CONF_REQ_COMPLETED = 0,
  NBFW_CONF_REQ_HALT,
  NBFW_CONF_REQ_IGNORED,
  NBFW_CONF_REQ_COMPLETED_WITH_IGNORED_ACT_TIME
} nbfw_cnf_status_e;

// --------------------------------------------------------------------------
//
// APP NB1 FW tech enable request
//
// --------------------------------------------------------------------------

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
} nbfw_action_time_s;

/* Request message header */
typedef struct {
  uint8 seq_id;
  nbfw_action_time_s action_time; /* action time for requests */
} nbfw_req_hdr_s;

/* Confirmation message header */
typedef struct {
  uint8 seq_id;
  nbfw_cnf_status_e status;
} nbfw_cnf_hdr_s;


/* profiling cfg structure */
typedef union
{
  struct
  {
    uint32 is_sleep_dis : 1;             /* if set, FW autonomous sleesp will be disabled */
    uint32 is_early_term_dis : 1;        /* if set, FW page early termination will be disabled */
    uint32 is_high_srch_th : 1;          /* if set, FW will use high threshold for srch detection, which helps when NW is doing txd for PSS/SSS */
    uint32 reserved : 29;               /* reserved field */
  };
  uint32 value;
} nbfw_profiling_cfg_s;

/* tech enable request */
typedef struct {
  nbfw_req_hdr_s hdr;
  nbfw_tech_instance_e tech;
  nbfw_profiling_cfg_s profiling_cfg;
} nbfw_app_enable_req_s;

NBFW_INTF_MSG_S(app_enable_req);

/* tech Disable request */
typedef struct {
  nbfw_req_hdr_s hdr;
  nbfw_tech_instance_e tech;
} nbfw_app_disable_req_s;

NBFW_INTF_MSG_S(app_disable_req);

/* tech enable complete confirm */
typedef struct {
  nbfw_cnf_hdr_s hdr;
} nbfw_app_enable_cnf_s;

NBFW_INTF_MSG_S(app_enable_cnf);

/* tech disable complete confirm */
typedef struct {
  nbfw_cnf_hdr_s hdr;
} nbfw_app_disable_cnf_s;

NBFW_INTF_MSG_S(app_disable_cnf);

typedef msgr_hdr_struct_type nbfw_terminate_cmdi_msg_s;

/* Union of all messages received from software */
typedef union {
  msgr_hdr_struct_type hdr;
  NBFW_INTF_MSG(app_enable_req);
  NBFW_INTF_MSG(app_disable_req);
  NBFW_INTF_MSG(terminate_cmdi);
} nbfw_intf_req_msg_u;

/* Union of all messages sent to software */
typedef union {
  msgr_hdr_struct_type hdr;
  NBFW_INTF_MSG(app_enable_cnf);
  NBFW_INTF_MSG(app_disable_cnf);
} nbfw_intf_cnf_msg_u;

#endif /* INTF_NBFW_APP_H */

