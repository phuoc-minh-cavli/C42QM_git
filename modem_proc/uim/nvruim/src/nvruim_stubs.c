/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               N V    R - U I M    S U B S Y S T E M

GENERAL DESCRIPTION
  Stubs file for all nvruim external functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2016 by QUALCOMM Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/nvruim_stubs.c   1.33   19 Jan 2016 08:19:14   tsummers  $
$Header: //components/rel/uim.mpss/3.3.1/nvruim/src/nvruim_stubs.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/16   ar      Featurizing code for IoE
05/11/16   sp      Fix low level compiler warnings
04/06/16   ssr     Removal of NVRUIM Task for IOE variant
03/24/16   lm      Fix compilation errors
02/08/16   ssr     Adding review comments
02/01/16   ssr     Adding stubs for compilation dependencies
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "nvruim_p.h"
#include "nvruim.h"
#include "nvruim_task.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* Indicate existing DFs (MF, CDMA, GSM, or DCS 1800 ) */
byte nvruim_dir_present = NVRUIM_NO_DIR_PRESENT;

/* The following variable gets set during startup, after a read to the CDMA
service table determines if the UIM card supports 3GPD functionality */
boolean nvruim_3gpd_sip_svc=FALSE;
boolean nvruim_3gpd_mip_svc=FALSE;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                             Functions                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION NVRUIM_SET_UIM_DIR_PRESENT

DESCRIPTION
The function set the flag to indicate which of the MF, GSM DF, DCS
1800 DF, CDMA subscription (CDMA DF or CSIM ADF) exist.

DEPENDENCIES
None.

RETURN VALUE
None

SIDE EFFECTS
Updates the variable nvruim_dir_present.
===========================================================================*/
void nvruim_set_uim_dir_present
(
  byte dir
)
{
  (void)dir;
} /* nvruim_set_uim_dir_present */


/*===========================================================================

FUNCTION NVRUIM_INIT_NON_3GPD_CDMA_CARD

DESCRIPTION
  This function is called from mmgsdi task during post pin initialization.
  It passes in a boolean, where
    TRUE : it is 3GPD disabled card
    FALSE: it is 3GPD enabled  card

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
none
===========================================================================*/
void nvruim_init_non_3gpd_cdma_card
(
  boolean                       non_3gpd_cdma_card,
  mmgsdi_session_type_enum_type session_type
)
{
  (void) non_3gpd_cdma_card;
  (void) session_type;
} /* nvruim_init_non_3gpd_cdma_card */


/*===========================================================================

FUNCTION NVRUIM_INIT_HRPD_CAPABILITY

DESCRIPTION
  This function is called from the GSDI task during nvruim initialization.
  It passes in a boolean which indicates whether or not the RUIM have a
  HRPD capability

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_hrpd_control global variable for RUIM support of HRPD
===========================================================================*/
void nvruim_init_hrpd_capability (
  boolean                       hrpd_disabled_card_flag,
  mmgsdi_session_type_enum_type session_type
)
{
  (void) hrpd_disabled_card_flag;
  (void) session_type;
} /* nvruim_init_hrpd_capability */


/*===========================================================================

FUNCTION NVRUIM_INIT_FALLBACK_TO_CAVE

DESCRIPTION
  This function is called from the GSDI task during nvruim initialization.
  It passes in a boolean which indicates fallback to CAVE is allowed or
  not after the MD5 failure

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void nvruim_init_fallback_to_cave (
  boolean                       allow_to_fallback,
  mmgsdi_session_type_enum_type session_type
)
{
  (void) allow_to_fallback;
  (void) session_type;
} /* nvruim_init_fallback_to_cave */


/*===========================================================================

FUNCTION NVRUIM_SET_ESN_USAGE

DESCRIPTION
This function sets the usage indicator that is used by the NV-RUIM module
to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Updates the usage indicator variable nvruim_esn_usage.

===========================================================================*/
void nvruim_set_esn_usage
(
  nvruim_esn_usage_type esn_usage
)
{
  (void) esn_usage;
} /* nvruim_set_esn_usage */


/*===========================================================================

FUNCTION NVRUIM_GET_ESN_USAGE

DESCRIPTION
This function gets the usage indicator that is used by the NV-RUIM module
to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
Non-reentrant

RETURN VALUE
None.

SIDE EFFECTS

===========================================================================*/
nvruim_esn_usage_type nvruim_get_esn_usage
(
  void
)
{
  /* Set as default the ESN Usage indicator - Use ESN as ESN */
  return NV_RUIM_USE_ESN;
}/* nvruim_get_esn_usage */


/*===========================================================================

FUNCTION UIM_INIT_3GPD_PREFERENCE

DESCRIPTION
This function is called from the PS task during initialization for 3GPD.
It passes in a boolean which indicates whether or not the RUIM provides
for 3GPD services

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
Sets the nvruim_3gpd_control global variable for RUIM support of 3GPD
Sets the nvruim_3gpd_mip_svc and nvruim_3gpd_sip_svc
global variables to indicate service support
===========================================================================*/
void uim_init_3gpd_preference
(
  boolean                       service_3gpd_sip,
  boolean                       service_3gpd_mip,
  mmgsdi_session_type_enum_type session_type
)
{
  (void) service_3gpd_sip;
  (void) service_3gpd_mip;
  (void) session_type;
} /* uim_init_3gpd_preference */


/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_INIT_EXTENSIONS_SUPPORT

DESCRIPTION
  This function is called from the Data task during initialization for 3GPD
  Extentions. It passes in a boolean which indicates whether or not the RUIM
  provides support for service 15 (Messagind and 3GPD Extentions).
  The requirement on the RUIM card for this serviec is defined in Open Market
  Handset RUIM Specification(CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_3gpd_ext_support global variable for extended 3GPD RUIM support
===========================================================================*/
void nvruim_data_3gpd_init_extensions_support
(
  boolean                       service_3gpd_extensions,
  mmgsdi_session_type_enum_type session_type
)
{
  (void) service_3gpd_extensions;
  (void) session_type;
} /* nvruim_data_3gpd_init_extensions_support */


/*===========================================================================

FUNCTION NVRUIM_SESSION_INIT

DESCRIPTION
  This function will register with MMGSDI and obtain a client id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void nvruim_session_init
(
  void
)
{
} /* nvruim_session_init */


/*===========================================================================

FUNCTION NVRUIM_FLUSH_CACHE

DESCRIPTION
  This function clears the cache control so that all items are read from the
  RUIM the next time they are requested.

DEPENDENCIES
  This function affects the cache control variable and therefore affects the
  operation of the read of cached NV items from the RUIM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void nvruim_flush_cache
(
  void
)
{
  /* No need to clear cache as it is deprecated.
       This function is just for compatibility of other modules*/
} /* nvruim_flush_cache */


/*===========================================================================

FUNCTION UIM_3GPD_SUPPORT

DESCRIPTION
This function returns the preferred source for 3GPD operations.


DEPENDENCIES
None.

RETURN VALUE
nvruim_3gpd_support_status - either NVRUIM_3GPD_SUPPORT or
NVRUIM_3GPD_MIP_NV_SIP_NV

SIDE EFFECTS
None.

===========================================================================*/
nvruim_3gpd_support_status uim_3gpd_support
(
  void
)
{
  /* Set as default - ME should use NV/SW for 3GPD  */
  return NVRUIM_3GPD_MIP_NV_SIP_NV;
} /* uim_3gpd_support */


/*===========================================================================

FUNCTION UIM_AN_HRPD_SUPPORT

DESCRIPTION
This function returns the preferred source for HRPD operations.


DEPENDENCIES
None.

RETURN VALUE
uim_an_hrpd_support_status - either NVRUIM_AN_HRPD_SUPPORT, NVRUIM_AN_HRPD_NO_SUPPORT
or UIM_AN_HRPD_USE_CAVE

SIDE EFFECTS
None.

===========================================================================*/
nvruim_an_hrpd_support_status uim_an_hrpd_support
(
  void
)
{
  /* Set as default - ME should use NV/SW for HRPD  */
  return NVRUIM_AN_HRPD_NO_SUPPORT;
} /* uim_an_hrpd_support */


/*===========================================================================

FUNCTION nvruim_init_wms_svc_items

DESCRIPTION
  This function is called from the WMS task during initialization for SMS.
  It passes in boolean values to indicate whether or not the RUIM provides
  for BCSMS and SMS Capabilities provisioning.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_bcsms_svc and nvruim_smscap_svc
  global variables to indicate service support.

===========================================================================*/
void nvruim_init_wms_svc_items
(
  boolean service_bcsms,
  boolean service_smscap
)
{
  (void) service_bcsms;
  (void) service_smscap;
} /* nvruim_init_wms_svc_items() */


/*===================================================================
FUNCTION NVRUIM_PPP_CAVE_FALLBACK

DESCRIPTION
  Sets the hrpd control variable to Cave

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==================================================================*/
void nvruim_ppp_cave_fallback (
  void
)
{
} /* nvruim_ppp_cave_fallback */


/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_EXT_SUPPORT

DESCRIPTION
This function returns 3GPD_EXT service availability

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
 TRUE : When card has 3GPD_EXT service available
 FALSE: When card does not have 3GPD_EXT service

SIDE EFFECTS
None.

===========================================================================*/
boolean nvruim_data_3gpd_ext_support (
  void
)
{
  /* Card does not have 3GPD_EXT service */
  return FALSE;
} /* nvruim_data_3gpd_ext_support */


/*===========================================================================

FUNCTION NVRUIM_CHECK_NON_3GPD_CDMA_CARD

DESCRIPTION
 This function returns the non legacy cdma card presence.

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
  nvruim_3gpd_nv_card_status where
  NVRUIM_NO_CARD        : NO Card
  NVRUIM_NON_3GPD_CARD  : 3GPD disabled card
  NVRUIM_3GPD_CARD      : 3GPD enabled card

SIDE EFFECTS
none
===========================================================================*/
nvruim_3gpd_nv_card_status nvruim_check_non_3gpd_cdma_card
(
  void
)
{
  return NVRUIM_NO_CARD;
} /* nvruim_check_non_3gpd_cdma_card */


/*===========================================================================

FUNCTION NVRUIM_LBS_INIT_SUPPORT

DESCRIPTION
  This function is called from the Data task during initialization for LBS
  Support. It passes in a boolean value which indicates whether or not the
  RUIM card have support for service 24 (LBS)in its CDMA Service Table.
  The requirement on the RUIM card for this serviec is defined in Open Market
  Handset RUIM Specification(CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_lbs_support global variable for LBS support
===========================================================================*/
void nvruim_lbs_init_support
(
  boolean service_lbs
)
{
  (void) service_lbs;
} /* nvruim_lbs_init_support */


/*===========================================================================

FUNCTION UIM_INIT_AN_HRPD_PREFERENCE

DESCRIPTION
This function is called from the PS task during initialization for HRPD.
It passes in a boolean which indicates whether or not the RUIM provides
for HRPD services, and a boolean which indicates whether cave is to be used

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
Sets the nvruim_hrpd_control global variable for RUIM support of HRPD
Sets the nvruim_hrpd_cdma_svc global variable to indicate CDMA service support
===========================================================================*/
void uim_init_an_hrpd_preference
(
  boolean service_hrpd
)
{
  (void) service_hrpd;
} /* nvruim_init_an_hrpd_preference */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONFIG_NV_REQ

DESCRIPTION
  This function processes the NV request to set the RTRE control and
  RTRE polling control values based on the configuration (NV 855)
  read from EFS.

RETURN VALUE
  nv_stat_enum_type

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_stat_enum_type nvruim_process_rtre_config_nv_req (
  void *in_ptr
)
{
  nv_cmd_type                  *nv_cmd_ptr    = (nv_cmd_type *)in_ptr;

  if (in_ptr == NULL)
  {
    return NV_BADPARM_S;
  }

  if (nv_cmd_ptr->cmd != NV_RTRE_OP_CONFIG_F)
  {
    nv_cmd_ptr->status = NV_RUIM_NOT_SUPPORTED_S;
    return NV_RUIM_NOT_SUPPORTED_S;
  }

  if (nv_cmd_ptr->sigs == 0)
  {
    nv_cmd_ptr->status = NV_BADPARM_S;
    return NV_BADPARM_S;
  }

  if (nv_cmd_ptr->done_q_ptr  && nv_cmd_ptr->tcb_ptr)
  {
    /* Put the data on done q provided by async client */
    (void)q_link(nv_cmd_ptr, &nv_cmd_ptr->link);
    q_put(nv_cmd_ptr->done_q_ptr, &nv_cmd_ptr->link);
  }

  if (nv_cmd_ptr->tcb_ptr)
  {
    /* Let the client know the NV command/request completed */
    (void) rex_set_sigs(nv_cmd_ptr->tcb_ptr, nv_cmd_ptr->sigs);
  }
  return NV_DONE_S;
} /* nvruim_process_rtre_config_nv_req */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_ROAMING_LIST_SIZE_NV_REQ

DESCRIPTION
  This function processes the roaming list size request from NV task

RETURN VALUE
  nv_stat_enum_type

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_stat_enum_type nvruim_process_roaming_list_size_nv_req (
  void *out_ptr
)
{
  if (out_ptr == NULL)
  {
    return NV_BADPARM_S;
  }
  *((uint16 *)out_ptr) = NV_ROAMING_LIST_MAX_SIZE;
  return NV_DONE_S;
} /* nvruim_process_roaming_list_size_nv_req */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONTROL_NV_REQ

DESCRIPTION
  This function processes the RTRE control and RTRE polling control request
  from NV task

RETURN VALUE
  nv_stat_enum_type

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
static nv_stat_enum_type nvruim_process_rtre_control_nv_req (
  nv_ruim_enum_type  type,
  const void        *in_ptr,
  void              *out_ptr
)
{
  nv_stat_enum_type  nv_operation_status = NV_DONE_S;

  switch (type)
  {
    case NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL:
      if (out_ptr == NULL)
      {
        nv_operation_status = NV_BADPARM_S;
        break;
      }
      *((nv_rtre_control_type *)out_ptr) = NV_RTRE_CONTROL_NO_RUIM;
      break;

    case NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL:
      if (out_ptr == NULL)
      {
        nv_operation_status = NV_BADPARM_S;
        break;
      }
      *((nv_rtre_polling_control_type *)out_ptr) = NV_RTRE_POLLING_CONTROL_NO_POLL;
      break;

    case NV_RUIM_CMD_RTRE_NAM_IOCTL:
      if (in_ptr == NULL || out_ptr == NULL)
      {
        nv_operation_status = NV_BADPARM_S;
        break;
      }
      *((boolean *)out_ptr) = FALSE;
      break;

    case NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL:
      if (in_ptr == NULL || out_ptr == NULL)
      {
        nv_operation_status = NV_BADPARM_S;
        break;
      }
      *((boolean *)out_ptr) = FALSE;
      break;

    default:
      nv_operation_status = NV_BADPARM_S;
      break;
  }
  return nv_operation_status;
} /* nvruim_process_rtre_control_nv_req */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_NV_REQ

DESCRIPTION
  This function processes the following requests from NV task:
  NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL
    - Verify if and NV item maps to EF on card and if so access (read/write) it
  NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL
    - Update/fetch rtre_control_value
  NV_RUIM_CMD_RTRE_NAM_IOCTL
    - Update/fetch nv_rtre_use_ruim_for_nam
  NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL
    - Update/fetch nv_rtre_polling_control
  NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL
    - Update/fetch nv_rtre_polling_for_nam
  NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL
    - Update/fetch nv_max_size_of_roaming_list
  NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL
    - Process RTRE configuration command

RETURN VALUE
  Of the enumerations defined in nv_stat_enum_type this function will return:
    NV_DONE_S      - Request processed or to be processed by MMGSDI
    NV_BADPARM_S   - Bad parameter in the command block
    NV_RUIM_NOT_SUPPORTED_S - Indicates an NV item not supported in RUIM

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
nv_stat_enum_type nvruim_process_nv_req (
  nv_ruim_enum_type  type,       /* Command type to be processed */
  void              *in_ptr,     /* Input buffer for the command type */
  void              *out_ptr     /* Output buffer for the command type */
)
{
  nv_stat_enum_type  nv_operation_status = NV_DONE_S;

  switch (type)
  {
    case NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL:
      nv_operation_status =  NV_RUIM_NOT_SUPPORTED_S;
      break;

    case NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL:
    case NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL:
    case NV_RUIM_CMD_RTRE_NAM_IOCTL:
    case NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL:
      nv_operation_status =
        nvruim_process_rtre_control_nv_req(type, in_ptr, out_ptr);
      break;

    case NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL:
      nv_operation_status =
        nvruim_process_roaming_list_size_nv_req(out_ptr);
      break;

    case NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL:
      nv_operation_status =
        nvruim_process_rtre_config_nv_req(in_ptr);
      break;

    default:
      nv_operation_status = NV_BADPARM_S;
  }

  return nv_operation_status;
} /* nvruim_process_nv_req */


/*===========================================================================
FUNCTION NVRUIM_MMGSDI_INIT

DESCRIPTION
  Function called from the MMGSDI task when NVRUIM task is not running, 
  to allow registration of NVRUIM module with NV.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
========================================================================= */
void nvruim_mmgsdi_init (
  void
)
{
  /*  Variable for NVRUIM initialization */
    nv_ruim_register(nvruim_process_nv_req);
} /* nvruim_mmgsdi_init */


/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONFIGURATION

DESCRIPTION
  This function updates the global RTRE control and RTRE polling control
  values based on the configuration value provided in the command

DEPENDENCIES
  This function should only be called by the NV_CMD function to process
  an NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  nv_stat_enum_type

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type nvruim_process_rtre_configuration
(
  nv_cmd_type                  *cmd_ptr,    /* Pointer to command received on nv_cmd_q */
  nv_rtre_control_type         *nv_rtre_ctrl,
  nv_rtre_polling_control_type *nv_rtre_polling_ctrl
)
{
  (void)cmd_ptr;
  (void)nv_rtre_ctrl;
  (void)nv_rtre_polling_ctrl;
  return NV_BADTG_S;
} /* nvruim_process_rtre_configuration */
