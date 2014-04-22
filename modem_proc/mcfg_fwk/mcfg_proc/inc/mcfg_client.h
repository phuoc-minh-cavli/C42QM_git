
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	mcfg_client.h

GENERAL DESCRIPTION

  MCFG header file for client registration
  
Copyright (c) 2016 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_proc/inc/mcfg_client.h#1 $
  $DateTime: 2020/02/19 23:07:19 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2016/03/22   cys     Create API file

===========================================================================*/
#ifndef _MCFG_CLIENT_H_
#define _MCFG_CLIENT_H_

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mcfg_refresh.h"


/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_INDEX_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_CLIENT_ID_ANONYMOUS = 0,
  MCFG_CLIENT_ID_1X,
  MCFG_CLIENT_ID_DATA,
  MCFG_CLIENT_ID_GERAN,
  MCFG_CLIENT_ID_GPS,
  MCFG_CLIENT_ID_HDR,
  MCFG_CLIENT_ID_IMS,
  MCFG_CLIENT_ID_LTE,
  MCFG_CLIENT_ID_MCS,
  MCFG_CLIENT_ID_MMCP_CM,
  MCFG_CLIENT_ID_MMCP_MMOC,
  MCFG_CLIENT_ID_MMCP_QMI,
  MCFG_CLIENT_ID_MMCP_SD,
  MCFG_CLIENT_ID_MMCP_NAS,
  MCFG_CLIENT_ID_MMCP_PM,
  MCFG_CLIENT_ID_MMCP_WMS,
  MCFG_CLIENT_ID_TDS,
  MCFG_CLIENT_ID_UIM_DRV,
  MCFG_CLIENT_ID_UIM_PBM,
  MCFG_CLIENT_ID_UIM_MMGSDI,
  MCFG_CLIENT_ID_WCDMA,
  MCFG_CLIENT_ID_INVALID = 0x7F,
} mcfg_client_id_type;

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_CLIENT_EVENT_E_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_CLIENT_EVENT_REFRESH_START =     (1<<0),       /**< client event for starting refresh */
  MCFG_CLIENT_EVENT_REFRESH_CB =        (1<<1),       /**< client event for refresh cb */
  MCFG_CLIENT_EVENT_REFRESH_COMPLETE =  (1<<2),       /**< client event for completing refresh  */
  MCFG_CLIENT_EVENT_MODEM_RESTART =     (1<<3),       /**< client event for modem initiated restart */
} mcfg_client_event_e_type;  

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_REFRESH_EVENT_S_TYPE
-------------------------------------------------------------------------------*/
typedef struct
{
  mcfg_client_event_e_type  mcfg_event;      /**< Refresh event type */
  mcfg_refresh_type_e_type  type;            /**< Refresh for slot or sub specific items */
  mcfg_refresh_index_type   slot_index;      /**< Slot index for this refresh */
  mcfg_refresh_index_type   sub_index;       /**< Subscription index for this refresh */
} mcfg_refresh_event_s_type;

/* Client task is expected to queue cb function into its own context */
typedef boolean (*mcfg_client_event_ssr_cb_fn)(void);
typedef boolean (*mcfg_refresh_event_cb_fn)(mcfg_refresh_event_s_type *p_event);

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_CLIENT_EVENT_SSR_REG_S_TYPE
-------------------------------------------------------------------------------*/
typedef struct
{
  mcfg_client_event_ssr_cb_fn       ssr_event_cb;
} mcfg_client_event_ssr_reg_s_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_REFRESH_EVENT_REG_S_TYPE
-------------------------------------------------------------------------------*/
typedef struct
{
  mcfg_refresh_type_e_type                  type;      /**< Refresh for slot or sub specific items */
  mcfg_refresh_index_mask_type         slot_mask;      /**< Slot mask for this refresh */
  mcfg_refresh_index_mask_type          sub_mask;       /**< Subscription mask for this refresh */
  mcfg_refresh_event_cb_fn      refresh_event_cb;
} mcfg_refresh_event_reg_s_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_CLIENT_EVENT_REG_INFO_S_TYPE
-------------------------------------------------------------------------------*/
typedef union mcfg_client_event_reg_info_s
{
  mcfg_refresh_registration_s_type  refresh_reg;        //existing refresh cb
  mcfg_refresh_event_reg_s_type     refresh_event_reg;  //refresh start/complete events
  mcfg_client_event_ssr_reg_s_type  ssr_event_reg;      //modem restart-SSR events
}mcfg_client_event_reg_s_type;


/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/* ==========================================================================
FUNCTION:       MCFG_CLIENT_REGISTER
========================================================================== */
/**
  This function is used to register with MCFG to receive event update through
  registered callback function.
 
  @param[in]
  client_id:       mcfg client ID, use anonymous if not listed or request one.
  event_reg_type:  may register multiple events sharing same registration info structions
                  (like EVENT_REFRESH_START/EVENT_REFRESH_COMPLETE shares same structure, but
                  EVENT_REFRESH_START/COMPLETE and EVENT_REFRESH_CB doesn't share same structure).
  event_reg_info:  Registration info, it should be zero initialized for extension

  @return
  TRUE for success, otherwise FALSE
*/
boolean mcfg_client_register
(
  mcfg_client_id_type               client_id,
  mcfg_client_event_e_type          event_reg_type,
  mcfg_client_event_reg_s_type     *event_reg_info
);

/* ==========================================================================
FUNCTION:       MCFG_CLIENT_DEREGISTER
========================================================================== */
/**
  This function is used to deregister with MCFG to stop receiving event updates.
  Note:This api should only be used when using new api "mcfg_client_register" and not with
  the previous api "mcfg_refresh_register".
 
  @param[in]
  client_id:       clientID for the client deregistration,
  event_reg_type:  event registration type(START/COMPLETE) for deregistration of the client.

  @return
  TRUE for success, otherwise FALSE
*/
boolean mcfg_client_deregister
(
  mcfg_client_id_type               client_id,
  mcfg_client_event_e_type          event_reg_type
);

//ToDo:Placeholder:Need to revisit this for SSR.
/*void mcfg_client_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
);*/

#endif /* _MCFG_CLIENT_H_ */


