#ifndef DS_GPRS_BEARER_MANAGER_H
#define DS_GPRS_BEARER_MANAGER_H

/*===========================================================================
                      DS_GPRS_BEARER_MANAGER.H
DESCRIPTION
This file exposes bearer manager for GSM through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_gprs_bearer_manager.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Net down reason type. This type would map the call end reason from lower layers
  to DS net down cause code.
----------------------------------------------------------------------------*/
#define DS_LL_NET_DOWN_REASON_TYPE      6


/*-----------------------------------------------------------------------------
   This structure stores the GPRS QoS parameters.
-----------------------------------------------------------------------------*/
typedef struct
{
  boolean   valid_flag;     /* Whether the parameters are set and valid */
  uint32    precedence;     /* Precedence class. */
  uint32    delay;          /* Delay class. */
  uint32    reliability;    /* Reliability class. */
  uint32    peak;           /* Peak throughput class. */
  uint32    mean;           /* Mean throughput class. */
} ds_gprs_qos_params_type;


/*-----------------------------------------------------------------------------
  This structure contains all info needed in the GPRS bearer context.
-----------------------------------------------------------------------------*/
struct ds_gprs_bearer_manager
{    
  ds_gprs_qos_params_type      gprs_qos_val; /* GPRS specific QOS values */
  uint32                       src_stat_desc;
  protocol_cfg_options_T       pco;          /* UMTS specific Incoming call PCO */
  uint8                        u_lc_id;      /* uplink logical channel id */
  uint8                        d_lc_id;      /* downlink logical channel id */
};


/* Structure for GSNDCP Registration/Resumption Confirmation Events */
typedef struct
{
  void               *bearer_mgr_p;    /* instance of the call */
  boolean            cnf_status;       /* confirmation status  */
} ds_gprs_gsndcp_reg_cnf_ev_type;

/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_BRING_UP_PDP

DESCRIPTION
  This function initiates call bring up for GSM.
  
PARAMETERS  
  bearer ptr
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_gprs_bearer_mgr_bring_up_pdp
(
  ds_bearer_mgr_s   *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_PDP_IP_REQ_REG_CNF_HDLR

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_mgr_pdp_ip_req_reg_cnf_hdlr
(
  ds_gprs_gsndcp_reg_cnf_ev_type    *reg_cnf_ev_ptr
);

/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_RESUME_SNDCP_REG_REQ_CNF_HDLR

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_mgr_resume_sndcp_reg_req_cnf_hdlr
(
  ds_gprs_gsndcp_reg_cnf_ev_type    *reg_cnf_ev_ptr
);

/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_RESUME_SNDCP_REG_REQ

DESCRIPTION
  This function initiates resume for data path with SNDCP..

PARAMETERS
  bearer_ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_mgr_resume_sndcp_reg_req
(
  ds_bearer_mgr_s    *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_GPRS_MGR_PDP_ACT_CNF_HDLR

DESCRIPTION
  This function handles the PDP activate confirmation from NAS.
  
PARAMETERS  
  mm_pdp_activate_cnf_T - PDP ACT CNF pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_gprs_mgr_pdp_activate_cnf_hdlr
(
  mm_pdp_activate_cnf_T   *pdp_act_cnf
);

/*===========================================================================
FUNCTION DS_GPRS_MGR_TEAR_DOWN_PDN_CONNECTION

DESCRIPTION
  This function is used to tear down a PDN connection.  

PARAMETERS
  bearer_mgr_p  - pointer to the bearer context that is to be used
                         to end the call

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_gprs_mgr_tear_down_pdn_connection
(
  ds_bearer_mgr_s        *bearer_mgr_p
);


/*===========================================================================
FUNCTION DS_GPRS_MGR_PDP_DEACT_CNF_HDLR

DESCRIPTION
  This function handles the PDP deactivate confirmation from NAS.
  
PARAMETERS  
  mm_pdp_deactivate_cnf_T - PDP DEACT CNF pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_gprs_mgr_pdp_deactivate_hdlr
(
  ds_bearer_deact_params_s   *info
);

/*===========================================================================
FUNCTION DS_GPRS_MGR_DEREG_WM_WITH_SNDCP

DESCRIPTION
  This function posts WM dereg cmd to SNDCP.
  
PARAMETERS  
  bearer_mgr_p* - bearer pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_gprs_mgr_dereg_wm_with_sndcp
(
  ds_bearer_mgr_s    *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_GPRS_MGR_GO_NULL_HDLR

DESCRIPTION
  This function handles the complete clean up of GPRS bearer
  
PARAMETERS  
  bearer_mgr_p* - bearer pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_gprs_mgr_go_null_hdlr
(
  ds_bearer_mgr_s    *bearer_mgr_p,
  ds_pdn_mgr_s       *pdn_mgr_p 
);

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_ACTIVATE_ON_FULL_SRV

DESCRIPTION
  This function registers with GSNDCP on FULL srv while coming out of PSM
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_gprs_bearer_cntxt_activate_on_full_srv();

#endif /* DS_GPRS_BEARER_MANAGER_H */
