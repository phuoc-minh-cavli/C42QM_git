#ifndef DS_PDN_DEFS_H
#define DS_PDN_DEFS_H

/*===========================================================================
                      DS_PDN_DEFS.H
DESCRIPTION
This file exposes PDN definitions through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "sys.h"
#include "sys_cnst.h"
#include "cm.h"
#include "cm_gw.h"
#include "emm_ext_msg.h"
#include "emm_irat_if_msg.h"
#include "mmtask_v.h"
#include "gs.h"
#include "sys_v.h"

#include "lte.h"
#include "lte_pdcp_msg.h"
#include "lte_cphy_msg.h"
#include "lte_pdcp_ext_api.h"
#include "lte_rrc_ext_msg.h"

#include "nv.h"
#include "nv_items.h"
#include "mcfg_fs.h"
#include "list.h"
#include "queue.h"
#include <stringl/stringl.h>
#include "modem_mem.h"
#include "dsm_item.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "assert.h"


#include "ds_defs.h"
#include "ds_defsi.h"
#include "ds_pdn_ioctl.h"
#include "ds_prof.h"
#include "ds_profile.h"
#include "ps_ppp_ext.h"
#include "dsmsgr.h"
#include "data_msg.h"
#include "dstask.h"
#include "dstaski.h"
#include "ps_byte.h"
#include "ds_system_heap.h"


/*-------------------------------------------------------------------------- 
  This enum defines the various RAT Types. RAT Based PDN Throttle Table 
  will have an array entry for each of the below RAT Types
 -------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_THROT_RAT_MIN = -1,
  DS_3GPP_THROT_RAT_GLOBAL = 0, /* Across all RATs */
  DS_3GPP_THROT_RAT_LTE,
  DS_3GPP_THROT_RAT_UMTS,
  DS_3GPP_THROT_RAT_ILLEGAL,
  DS_3GPP_THROT_RAT_MAX = DS_3GPP_THROT_RAT_ILLEGAL
} ds_3gpp_throt_rat_e_type;

typedef struct
{
  boolean  throttle_needed;
  boolean  lower_layer_failure;
}ds_3gpp_throt_rab_info_s;

/*---------------------------------------------------------------------------
	This enumeration lists the TAG IDs that can be read from the EFNAS Config
	file present in the USIM
	---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_MMGSDI_TAG_LOW_PRIORITY_SIG = 0x80,
  DS3G_MMGSDI_TAG_SM_RETRY_TIMER = 0x8A,
  DS3G_MMGSDI_TAG_SM_RETRY_RAT_CHANGE = 0x8B,
  DS3G_MMGSDI_TAG_EXCEP_DATA_REPORT_ALLOWED = 0x8D
} ds3g_mmgsdi_tag_e_type;


/* Forward declarations - Incomplete data types to avoid inter dependent compilation */
typedef struct ds_pdn_mgr_type ds_pdn_mgr_s;

typedef struct ds_bearer_mgr ds_bearer_mgr_s;

typedef struct ds_flow_s ds_flow_type;

typedef struct ds_flow_list ds_flow_list_s;

typedef struct ds_pdn_flow_manager ds_pdn_flow_manager_s;

typedef struct ds_bearer_flow_manager ds_bearer_flow_manager_s;

typedef struct ds_eps_bearer_manager ds_eps_bearer_manager_s;

typedef struct ds_gprs_bearer_manager ds_gprs_bearer_manager_s;

typedef struct ds_pdn_mgr_inst_type ds_pdn_mgr_inst_type_s;

typedef struct ds_bearer_deact_params_type ds_bearer_deact_params_s;

typedef struct dssysmgr_ss_event_info_s dssysmgr_ss_event_info_type;

typedef struct dssysmgr_ph_event_info_s dssysmgr_ph_event_info_type;

typedef struct ds_rmsm_ati_prim_profile_list_type ds_prim_profile_list_type; 

/*---------------------------------------------------------------------------
TYPEDEF DSSNET6_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events received by the dssnet6 sm.
---------------------------------------------------------------------------*/
typedef struct __ds_pdn_v6_sm_cb_s ds_pdn_v6_sm_cb_type;


/*===========================================================================
FUNCTION DS_PDN_MGR_NAS_TO_DS_MSG_HDLR
 
DESCRIPTION
  This function handles messages from NAS to DS.

PARAMETERS 
  Message - void*
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_mgr_nas_to_ds_msg_hdlr
(
  void*             msg_p
);

#endif /* DS_PDN_DEFS_H */

