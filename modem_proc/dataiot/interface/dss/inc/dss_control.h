#ifndef DSS_CONTROL_H
#define DSS_CONTROL_H
/*===========================================================================

   D A T A   S E R V I C E S   C O N T R O L   P A T H     F I L E

DESCRIPTION

 The Data Services control path files contains support for call bring up/
 teardown implementation for the clients.

Copyright (c) 2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/
#include "dss.h"
#include "data_msg.h"
#include "dss_config.h"
#include "ds_defsi.h"
#include "dssocki.h"
#include "ds_fwk.h"
#include <stringl.h>
#include "dssi.h"
#include "dstaski.h"
#include "ds_system_heap.h"
#ifdef FEATURE_DATA_PS_QOS
#include "ds_qos_net_initiated_req.h"
#endif


#ifdef FEATURE_DATA_PS_QOS
#define DSS_MAX_QOS_NET_INITIATED_REQUESTS 64
#define DSS_QOS_HANDLE_FREE                0
#define DSS_QOS_HANDLE_ASSIGNED            1
#define DSS_QOS_INVALID_HANDLE             -1
#endif

/*============================================================================
					STRUCTURES
=============================================================================*/

typedef struct
{
  sint15 dss_nethandle;
  ds_policy_info_type *policy_info;
}dss_control_bringup_type_s;
 

typedef struct
{
  ds_fwk_clnt_handle_type    client_handle;
  ds_pdn_event_enum_type     event;
  ds_pdn_event_info_u_type   event_info;
  void                      *user_data_ptr;
}dss_control_callback_info_s;

/*============================================================================
					API's
=============================================================================*/

/*===========================================================================
FUNCTION DSS_CREATE_ACB()

DESCRIPTION
  This API allocates an application control block and initializes the 
  parameters in the ACB structure with relavent parameters

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID / DSS Nethandle on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.

===========================================================================*/
sint15 dss_create_acb
(
  dss_net_lib_info_type_s *net_params,
  sint15                  *dss_errno        /* error condition value */
);

/*===========================================================================
FUNCTION DSS_PERFORM_POLICY_MATCH()

DESCRIPTION

  This API checks if a call is already present in with the Policy information
  passed.


DEPENDENCIES
  None.

RETURN VALUE
  If a call is already present DSS Handle is returned by DS FWK
  If a call is not present -1 is returned


SIDE EFFECTS
  None
==============================================================================*/
int dss_perform_policy_match
(
  dss_net_lib_info_type_s *net_params
);

/*===========================================================================
FUNCTION DSS_MAP_APPL_FWK_HANDLE()

DESCRIPTION

  This API gets the DSS handle corresponding to the Nethandle stores in the ACB
  (DS framework handle)


DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/
sint15 dss_get_handle
(
  sint15 dss_nethandle
);


/*===========================================================================
FUNCTION DSS_BRINGUP_PDN()

DESCRIPTION

  This API brings up the data call as per the policy passed by the client


DEPENDENCIES
  None.

RETURN VALUE
  void


SIDE EFFECTS
  None
==============================================================================*/
void dss_bringup_pdn
(
  dss_control_bringup_type_s     *bringup_info
);

/*===========================================================================
FUNCTION DSS_ALLOCATE_HANDLE()

DESCRIPTION

  This API allocates an available handle from the pool of DSS handles

DEPENDENCIES
  None.

RETURN VALUE
      DSS handle from the DSS handle pool


SIDE EFFECTS
  None
==============================================================================*/
int8 dss_allocate_handle
(
  void
);

/*===========================================================================
FUNCTION DSS_INCR_HANDLE_REF_CNT()

DESCRIPTION

  This API increments the reference count for the DSS handle if a call is already
  present for the requested Policy

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/
void dss_incr_handle_ref_cnt
(
  sint15 dss_handle
);

/*===========================================================================
FUNCTION DSS_DECR_HANDLE_REF_CNT()

DESCRIPTION

  This API decrements the reference count for the DSS handle 

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/
void dss_decr_handle_ref_cnt
(
  sint15 dss_handle
);

/*===========================================================================
FUNCTION DSS_POST_CLIENT_NET_EVT()

DESCRIPTION

  This API will invoke the NET callback registered by DSS clients

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/
void dss_post_client_net_evt
(
  uint8 dss_handle,
  uint8 pdn_evt,
  ds_pdn_event_info_u_type   event_info
);

/*===========================================================================
FUNCTION DSS_BRINGUP_PDN()

DESCRIPTION

  This API brings down the data call for the Nethandle passed

DEPENDENCIES
  None.

RETURN VALUE
      0  -- if bringup is successful.
     -1 -- if bringup is failed.
      102 -Operation would block.


SIDE EFFECTS
  None
==============================================================================*/
void dss_bringdown_pdn
(
  sint15 *dss_handle                         /* DSS Handle */
);


/*===========================================================================
FUNCTION DSS_POWERUP_INIT()

DESCRIPTION

  This API does the task init functionality for DSS

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/

void dss_init();

/*===========================================================================
FUNCTION DSS_PROCESS_CALLBACK_HDLR()

DESCRIPTION

  This API Handles the functionality for the event posted in the DSS callback
  registered with DS_FWK at DSS task init.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/

void dss_process_callback_hdlr
(
  dss_control_callback_info_s *callback_info
);


/*===========================================================================
FUNCTION DSS_PROCESS_CALLBACK_HDLR()

DESCRIPTION

  This API Stores the DS FWK instance pointer for the DSS handle in the ACB

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/
void dss_store_fw_inst_in_acb
(
  acb_type  *acb_ptr,
  uint8     dss_handle
);

/*===========================================================================
FUNCTION      DSS_PROCESS_PDN_UP_EV_CBACK()

DESCRIPTION
  Called when the PDN comes up. This calls the
  net_callback_fcn() for the specified application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_process_pdn_up_ev_cback
(
  uint8 dss_handle
);


/*===========================================================================
FUNCTION      DSS_PROCESS_PDN_DOWN_EV_CBACK()

DESCRIPTION
  Called when the PDN goes down. This calls the
  net_callback_fcn() for the specified application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_process_pdn_down_ev_cback
(
  uint8 dss_handle,
  ds_net_down_event_info_type  pdn_down_info
);


#ifdef FEATURE_DATA_PS_QOS

/*===========================================================================
FUNCTION      DSS_QOS_IOCTL_HANDLERS()

DESCRIPTION
  This API handles the QOS IOCTL's posted from DS clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dss_qos_ioctl_handler
(
  sint15               dss_nethandle,
  ds_pdn_ioctl_type       ioctl_name,
  void                   *argval_ptr,
  sint15                   *ps_errno
);
#endif



#endif/*DSS_CONTROL_H*/
