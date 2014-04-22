/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     GMM Utility Functions (GMMUTILS.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmmutils.c_v   1.5   09 Jul 2002 08:47:10   vdrapkin  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/gmmutils.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/01   jca     Created module
05/23/01   jca     Added function gmm_delete_ps_location_information().
08/03/01   jca     Fixed some bugs found in unit test.
09/12/01   jca     Added RRC_PAGING_CTRL_REQ processing.
07/06/02   vdr     Fixed and modified dword gmm_convert_timer_value_ie_to_usecs
                   in order to support GMM-LLC interface
08/10/02   mks     Obsoleted RRC_PAGING_CTRL_REQ as the information conveyed by
                   this primitive is now sent to RRC through RRC_SIM_INSERTED_REQ
                   & RRC_SIM_UPDATE_REQ.
09/09/02   vdr     Notify the LLC that the P-TMSI has been invalidated
11/06/02   jca     Added function gmm_update_mm_serving_plmn_info().
11/08/02   vdr     Implemented gmm_delete_gprs_cksn( void ).
01/13/03   jca     Added missing case in function gmm_update_mm_serving_plmn_info().
01/27/03   jca     Changes to support PS_ONLY<->CS/PS combined mode switching.
                   Changes to support IMSI Detach via Combined GPRS Detach procedure.
02/07/03   jca     Modified function gmm_update_mm_serving_plmn_info() to correctly
                   set service_status in CS/PS mode & network op mode 1.
02/27/03   ks      Added support for FTM mode in gmm_delete_gprs_cksn, instead
                   of ERR_FATAL.
02/27/03   mks     Added support for PS data continuity feature - implemented
                   gmm_sm_send_rat_change_req() & gmm_sm_send_rat_resume_req().
03/07/03   vdr     Added gmm_process_cm_req() function
03/18/03   ks      Changed the ERR_FATAL to ERR in gmm_delete_gprs_cksn() when
                   there is no ACTIVE_RAT, this could happen when UE is in FTM.
03/18/03   mks     Now sending active RAT information to SM while notifying
                   inter-RAT change.
04/17/03   jca     Now setting MM substate to NO_IMSI if it is indicated that
                   CS portion of SIM has been invalidated on call to function
                   gmm_update_mm_serving_plmn_info().
05/23/03   jca     Modified function gmm_update_mm_serving_plmn_info().
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
06/18/03   ks      Added function gmm_send_nv_params() to send the GPRS NV items.
06/19/03   ks      Calling mmrat to Handle service status changes for PS
                   continuity.  Added prefix "gmm" to the GPRS NV items.
07/08/03   jca     Fixed bug in function gmm_update_mm_serving_plmn_info().
07/16/03   mks     Removed call to mm_rat_handle_srv_status_update() in
                   gmm_update_mm_serving_plmn_info().
08/28/03   ks      Added support for the ANITE GCF FLAG value from the NV
10/17/03   jca     Check that mm_idle_substate is not set to LIMITED_SERVICE
                   before setting service_status to SYS_SRV_STATUS_SRV.
11/07/03   jca     Tighten up code further to catch condition when GMM service
                   is not available and MM is in LIMITED_SERVICE state so that
                   we properly report LIMITED_SERVICE to the REG layer.
04/08/04   vdr     Added follow-on request processing when GMM is sending RA update
                   request message during G2W inter-RAT change and user data is
                   available
06/07/04   jca     Ensure mm_serving_plmn.info.service_status is set to STATUS_LIMITED
                   when CS service is available but UE not registered for CS service.
06/15/04   jca     Fixed bug in function gmm_update_mm_serving_plmn_info().
11/02/04   sv      EDGE changes: Indicating EDGE NV item information to RR.
01/05/05   ks      Disable the gmm_edge_feature_support when the feature
                   FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED is defined.
02/02/04   jca     Ensure mm_serving_plmn.info.service_status is set to STATUS_LIMITED
                   rather than STATUS_SRV whenever CS and/or PS service is available
                   but UE not registered for CS or PS service.
02/14/05   sv      Added support for Rel-4 changes.
07/22/05   abe     If RAU is not required and no inter-RAT in progress then
                   send MMR_SERVICE_IND with ps_data_suspend set to FALSE.
07/26/05   abe     The changes in previous version for the feature
                   FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER not required.
                   Removed the changes.
09/16/05   up      Updated gmm_update_mm_serving_plmn_info to send the GSTK
                   event when service status is changed.
10/11/05   vdr     Stoped calling mm_send_gstk_mm_idle_info()
11/23/05   ajt     Added definition for function gmm_parse_pdp_context_ie.
11/30/05   jca     Added support for new DTM NV parameters.
12/15/05   abe     Supported GSM DTM under FEATURE_GSM_DTM
12/20/05   ajt     While deleting RAI, don't make PLMN ID FF. Instead, let it be same
                   as what it is.
02/02/06   up      Adding interrat_nacc_support NV item for RR.
02/21/06   up      Keep interrat NACC NV reading under feature flag.
02/24/06   abe     Adding support for FEATURE_REL_6_SUPPORT
06/14/06   up      Modified gmm_update_mm_serving_plmn_info 
                   to block/unblock GSTK event.
07/14/06   pk     Updated for Feature DSAC                   
09/21/06   pk     Updated for Feature EDTM
02/26/08   rk     Ignore PS_DATA_AVAILABLE_REQ from CM if current PLMN is FPLMN, 
                  otherwise this triggers RAU which is incorrect.
                  and removed gmm_inter_rat_follow_on_ind as its not used.
03/03/08   ab  Modified gmm_get_ps_cell_access to get ps cell access from dsac 
                  only when mode is umts.
08/19/11   sla    When EDGE is not suported,Dont read the NV 4158 and set 
                         gmm_edge_8psk_power_class and gmm_nv_multislot_8psk_power_profile to zero
10/04/11   sla    Added functions for Adding RAI into foreground search list,deleting 
                  RAI from foreground search list,for checking whether RAI is in
                  foreground search RAI or not.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "sys_cnst_v.h"
#include "gmm_int_v.h"
#ifdef FEATURE_WCDMA
#include "rrcmmif.h"
#include "rrccmd.h"
#endif
#include "mm_umts_v.h"
#include "mmauth.h"
#include "smtask.h"
#include "smtask_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "ghdi_nvmem.h"
#include "ghdi_nvmem_v.h"
#include "gmm_msg.h"
#include "mm_coord.h"
#include "sys_eplmn_list_v.h"
#include "aconfig.h"
#include "fs_lib.h"
#include <stringl/stringl.h>
#include "mm.h"
#include "mcfg_fs.h" 
#ifdef FEATURE_LTE
#include "rfm.h"
#include"mm_v.h"
#include "emm_utility.h"
#endif
#include "ds3gpp_api.h"
#include "ULogFront.h"

#ifdef FEATURE_LTE
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif

#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)
#include "mm_v.h"
#include "gmmllcm.h"
#include "mmrat.h"
#endif /* #if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW) */

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#endif /* FEATURE_FACTORY_TESTMODE */

#ifdef FEATURE_GSTK
#include "mm_gstk.h"
#endif

#ifdef FEATURE_MBMS
#include "msmtask.h"
#include "msm_mm.h"
#endif
#ifdef FEATURE_NAS_REL10
#include "reg_nv.h"
#endif
#include "dog_hb_rex.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_LTE
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_drx_cn_coeff_s1 gmm_drx_cn_coeff_s1_sim[mm_sub_id]
sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_nv_drx_cn_coeff_s1 gmm_nv_drx_cn_coeff_s1_sim[mm_sub_id]
byte gmm_anite_gcf_flag_sim[MAX_AS_IDS] = {FALSE, FALSE};
#define gmm_anite_gcf_flag gmm_anite_gcf_flag_sim[mm_sub_id]
byte gmm_non_drx_timer_sim[MAX_AS_IDS] = {GPRS_NON_DRX_TIMER, GPRS_NON_DRX_TIMER};
#define gmm_non_drx_timer gmm_non_drx_timer_sim[mm_sub_id]
byte gmm_split_paging_cycle_sim[MAX_AS_IDS] = {GPRS_SPLIT_PAGING_CYCLE, GPRS_SPLIT_PAGING_CYCLE};
#define gmm_split_paging_cycle gmm_split_paging_cycle_sim[mm_sub_id]
#else
sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1;
sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1;
byte gmm_anite_gcf_flag     = FALSE;
byte gmm_non_drx_timer      = GPRS_NON_DRX_TIMER;
byte gmm_split_paging_cycle = GPRS_SPLIT_PAGING_CYCLE;
#endif
#endif/*FEATURE_LTE*/

boolean gmm_wwan_tech_active = TRUE;
boolean gmm_wwan_resumption_req = FALSE;

#ifdef FEATURE_NAS_GW

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#include "libprot.h"

uint64  gmm_nv_disable_call_type_sim[MAX_NAS_STACKS];

extern ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
(
  sys_modem_as_id_e_type as_id 
);
#define get_revision_level_indicator() get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id)

#else


uint64  gmm_nv_disable_call_type;

extern ue_rev_level_indicator_type get_revision_level_indicator( void );

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/


#ifdef FEATURE_DUAL_SIM
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#endif
/*
** GPRS NV Items GMM Store
*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

byte gmm_multislot_class_sim[MAX_AS_IDS]= {GPRS_MULTISLOT_CLASS, GPRS_MULTISLOT_CLASS};
#define gmm_multislot_class gmm_multislot_class_sim[mm_sub_id]

uint8 gmm_gea_support_sim[MAX_AS_IDS] = {GPRS_GEA_SUPPORT, GPRS_GEA_SUPPORT};
#define gmm_gea_support gmm_gea_support_sim[mm_sub_id]

#ifdef FEATURE_GSM_EDTM
boolean gmm_nv_edtm_feature_support_sim[MAX_AS_IDS]= {GPRS_EDGE_EDTM_SUPPORTED, GPRS_EDGE_EDTM_SUPPORTED};
#define gmm_nv_edtm_feature_support gmm_nv_edtm_feature_support_sim[mm_sub_id]
#endif

#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_NAS_GW)
/* Default values for EDGE NV items */
boolean gmm_edge_feature_support_sim[MAX_AS_IDS] = {GPRS_EDGE_FEATURE_SUPPORT_DEFAULT, GPRS_EDGE_FEATURE_SUPPORT_DEFAULT};
#define gmm_edge_feature_support gmm_edge_feature_support_sim[mm_sub_id]

byte gmm_edge_multislot_class_sim[MAX_AS_IDS] = {GPRS_EDGE_MULTISLOT_CLASS, GPRS_EDGE_MULTISLOT_CLASS};
#define gmm_edge_multislot_class gmm_edge_multislot_class_sim[mm_sub_id]

byte gmm_edge_8psk_power_class_sim[MAX_AS_IDS] = {GPRS_EDGE_8PSK_POWER_CLASS, GPRS_EDGE_8PSK_POWER_CLASS};
#define gmm_edge_8psk_power_class gmm_edge_8psk_power_class_sim[mm_sub_id]

byte gmm_edge_8psk_power_capability_sim[MAX_AS_IDS] = {GPRS_EDGE_8PSK_POWER_CAPABILITY, GPRS_EDGE_8PSK_POWER_CAPABILITY};
#define gmm_edge_8psk_power_capability gmm_edge_8psk_power_capability_sim[mm_sub_id]
#endif /* FEATURE_GSM_EGPRS */

#ifdef FEATURE_REL_4_SUPPORT
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
boolean gmm_edge_nw_ass_cell_change_sim[MAX_AS_IDS] = {GPRS_EDGE_NW_ASS_CELL_CHANGE, GPRS_EDGE_NW_ASS_CELL_CHANGE};
#define gmm_edge_nw_ass_cell_change gmm_edge_nw_ass_cell_change_sim[mm_sub_id]

byte    gmm_interrat_nacc_support_sim[MAX_AS_IDS] = { GPRS_INTERRAT_NACC_SUPPORT, GPRS_INTERRAT_NACC_SUPPORT};
#define gmm_interrat_nacc_support gmm_interrat_nacc_support_sim[mm_sub_id]
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */
#endif /* FEATURE_REL_4_SUPPORT */

#ifdef FEATURE_GSM_DTM
ghdi_nvmem_data_T gmm_nv_data_buffer;

boolean  gmm_nv_dtm_feature_support_sim[MAX_AS_IDS]  = {GPRS_EDGE_DTM_SUPPORTED, GPRS_EDGE_DTM_SUPPORTED};
#define  gmm_nv_dtm_feature_support gmm_nv_dtm_feature_support_sim[mm_sub_id]

uint8    gmm_nv_dtm_multislot_subclass_sim[MAX_AS_IDS] = {GPRS_EDGE_DTM_MULTISLOT_SUBCLASS, GPRS_EDGE_DTM_MULTISLOT_SUBCLASS};
#define  gmm_nv_dtm_multislot_subclass gmm_nv_dtm_multislot_subclass_sim[mm_sub_id]
#endif /* FEATURE_GSM_DTM */

uint8  gmm_nv_multislot_gmsk_power_profile_sim[MAX_AS_IDS] = {0x03,0x03};
#define gmm_nv_multislot_gmsk_power_profile gmm_nv_multislot_gmsk_power_profile_sim[mm_sub_id]

uint8  gmm_nv_multislot_8psk_power_profile_sim[MAX_AS_IDS] = { 0x03, 0x03};
#define gmm_nv_multislot_8psk_power_profile gmm_nv_multislot_8psk_power_profile_sim[mm_sub_id]

#else
byte gmm_multislot_class    = GPRS_MULTISLOT_CLASS;
uint8 gmm_gea_support       = GPRS_GEA_SUPPORT;

#ifdef FEATURE_GSM_EDTM
boolean gmm_nv_edtm_feature_support = GPRS_EDGE_EDTM_SUPPORTED;
#endif /* FEATURE_GSM_EDTM */

#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_NAS_GW)
/* Default values for EDGE NV items */
boolean gmm_edge_feature_support = GPRS_EDGE_FEATURE_SUPPORT_DEFAULT;
byte gmm_edge_multislot_class = GPRS_EDGE_MULTISLOT_CLASS;
byte gmm_edge_8psk_power_class = GPRS_EDGE_8PSK_POWER_CLASS;
byte gmm_edge_8psk_power_capability = GPRS_EDGE_8PSK_POWER_CAPABILITY;
#endif /* FEATURE_GSM_EGPRS */

#ifdef FEATURE_REL_4_SUPPORT

#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
boolean gmm_edge_nw_ass_cell_change = GPRS_EDGE_NW_ASS_CELL_CHANGE;
byte    gmm_interrat_nacc_support = GPRS_INTERRAT_NACC_SUPPORT;
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */

#endif /* FEATURE_REL_4_SUPPORT */

#ifdef FEATURE_GSM_DTM
ghdi_nvmem_data_T gmm_nv_data_buffer;
boolean           gmm_nv_dtm_feature_support    = GPRS_EDGE_DTM_SUPPORTED;
uint8             gmm_nv_dtm_multislot_subclass = GPRS_EDGE_DTM_MULTISLOT_SUBCLASS;
#endif /* FEATURE_GSM_DTM */
#endif/*FEATURE_DUAL_SIM || FEATURE_SGLTE*/

#ifdef FEATURE_GPRS_EDA
boolean           gmm_nv_eda_feature_support    = GPRS_EDGE_EDA_SUPPORTED;
#endif /* FEATURE_GPRS_EDA */

#ifdef FEATURE_REL_6_SUPPORT
#if (defined(FEATURE_GSM_EGPRS_SAIC) && defined(FEATURE_GSM_MDSP_AEQ))
boolean           gmm_nv_saic_feature_support = GSM_EGPRS_SAIC_SUPPORTED;
#endif /* FEATURE_GSM_EGPRS_SAIC */
#endif /* FEATURE_REL_6_SUPPORT */

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
uint16     gmm_nv_r_acch_feature_support = GSM_R_ACCH_SUPPORTED;
#endif /* FEATURE_GSM_R_SACCH  or FEATURE_GSM_R_FACCH*/

#if defined(FEATURE_GPRS_PS_HANDOVER) && defined(FEATURE_NAS_GW)
boolean           gmm_nv_psho_feature_support = FALSE;
#endif

extern boolean mm_emm_is_qrd_device;


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id) 
#endif

extern void gmm_handle_local_gprs_detach( void );
extern ULogHandle mm_get_ulog_handle();

#endif

uint8  gmm_nv_multislot_gmsk_power_profile = 0x03; 
uint8  gmm_nv_multislot_8psk_power_profile = 0x03;


extern boolean edrx_gsm_fake_negotiation;
extern byte                      gsm_wcdma_amr_codec_preference;

/*===========================================================================

FUNCTION GMM_CONVERT_TIMER3_VALUE_IE_TO_MSECS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  GPRS timer 3 value IE converted to milliseconds.  Note that a return value
  of zero indicates that the timer has been deactivated.

SIDE EFFECTS
  None

===========================================================================*/

uint64 gmm_convert_timer3_value_ie_to_msecs(byte timer_value )
{
  uint64 timer_value_unit;
  uint64 timer_value_msecs;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------------------
  ** Convert timer value per TS 24.008 Table 10.5.162
  ** ------------------------------------------------ */
  if ((timer_value & 0xE0) == 0x00)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 10 minutess
    ** ---------------------------------------------- */
    timer_value_unit = 600000;
  }
  else if ((timer_value & 0xE0) == 0x40)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 10 hours
    ** ---------------------------------------------- */
    timer_value_unit = 36000000 ;
  }
  else if ((timer_value & 0xE0) == 0x60)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 2 seconds
    ** ---------------------------------------------- */
    timer_value_unit = 2000 ;
  }
  else if ((timer_value & 0xE0) == 0x80)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 30 seconds
    ** ---------------------------------------------- */
    timer_value_unit = 30000 ;
  }
  else if ((timer_value & 0xE0) == 0xA0)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 1 minute
    ** ---------------------------------------------- */
    timer_value_unit = 60000 ;
  }
  else if ((timer_value & 0xE0) == 0xC0)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 320 hours
    ** ---------------------------------------------- */
    timer_value_unit = 1152000000 ;
  }
  else if ((timer_value & 0xE0) == 0xE0)
  {
    /* ---------------------------------------------
    ** Value indicates that the timer is deactivated
    ** --------------------------------------------- */
    timer_value_unit = 0;
  }
  else
  {
    /* ----------------------------------------------------------
    ** Other values shall be interpreted as multiples of 1 hour
    ** ---------------------------------------------------------- */
    timer_value_unit = 3600000;
  }

  timer_value = timer_value & 0x1F;

  timer_value_msecs = timer_value_unit * timer_value;

  MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Timer 3 value = %u seconds.", timer_value_msecs/1000);

  return (timer_value_msecs);

} /* gmm_convert_timer3_value_ie_to_msecs() */


/*===========================================================================
FUNCTION GMM_ENCODE_GPRS_TIMER_UNIT_AND_VALUE

DESCRIPTION
  This function encodes the timer value in milliseconds to timer unit and
  timer value as per 10.5.7.3, 10.5.7.4, 10.5.7.4a/3GPP TS 24.008 depending
  on the timer type (GPRS Timer, GPRS Timer 2, GPRS Timer 3)

DEPENDENCIES
  None

RETURN VALUE
  Encoded byte containing timer unit and timer value

SIDE EFFECTS
  None
===========================================================================*/
byte gmm_encode_gprs_timer_unit_and_value
( 
  uint64                 timer_value,
  gmm_gprs_timer_e_type  timer_type
)
{
  byte  unit = 0;
  byte  value = 0;
  uint64 remainder = 0;

  switch(timer_type)
  {
    case GPRS_TIMER:
    case GPRS_TIMER_2:
      /* Table 10.5.172/3GPP TS 24.008
      Bits 5 to 1 represent the binary coded timer value.

      Bits 6 to 8 defines the timer value unit for the GPRS timer as follows:
      Bits 
      8 7 6
      0 0 0  value is incremented in multiples of 2 seconds
      0 0 1  value is incremented in multiples of 1 minute 
      0 1 0  value is incremented in multiples of decihours
      1 1 1  value indicates that the timer is deactivated.*/

      if(timer_value > 11160000) /* Max value corresponding to timer unit 010 and timer value 11111 */
      {
        MSG_ERROR_DS_2(MM_SUB, "=MM= Timer value %u sec for timer type %d > max supported 11160 sec, truncating to max",
                       timer_value/1000, timer_type);
        timer_value = 11160000;
      }

      if(timer_value <= 62000)         /* 62000 is max value corresponding to timer unit 000 and timer value 11111 */
      {
        unit = 0; /* Multiples of 2 seconds */
        value = (byte)(timer_value / 2000);
        remainder = timer_value % 2000;
      }
      else if(timer_value <= 1860000) /* 1860000 is max value corresponding to timer unit 001 and timer value 11111 */
      {
        unit = 1; /* Multiples of 1 minute */
        value = (byte)(timer_value / 60000);
        remainder = timer_value % 60000;
      }
      else if(timer_value <= 11160000) /* 11160000 is max value corresponding to timer unit 010 and timer value 11111 */
      {
        unit = 2; /* Multiples of decihours (6 minutes) */
        value = (byte)(timer_value / 360000);
        remainder = timer_value % 360000;
      }
      break;

    case GPRS_TIMER_3:
      /* Table 10.5.163a/3GPP TS 24.008
      Bits 5 to 1 represent the binary coded timer value.

      Bits 6 to 8 defines the timer value unit for the GPRS timer as follows:
      Bits 
      8 7 6
      0 0 0 value is incremented in multiples of 10 minutes 
      0 0 1 value is incremented in multiples of 1 hour 
      0 1 0 value is incremented in multiples of 10 hours
      0 1 1 value is incremented in multiples of 2 seconds
      1 0 0 value is incremented in multiples of 30 seconds
      1 0 1 value is incremented in multiples of 1 minute
      1 1 0 value is incremented in multiples of 320 hours
      1 1 1 value indicates that the timer is deactivated.*/

      if(timer_value > 35712000000) /* Max value corresponding to timer unit 110 and timer value 11111 */
      {
        MSG_ERROR_DS_2(MM_SUB, "=MM= Timer value %u sec for timer type %d > max supported 35712000 sec, truncating to max",
                       timer_value/1000, timer_type);
        timer_value = 35712000000;
      }

      if(timer_value <= 62000)           /* 62000 is max value corresponding to timer unit 011 and timer value 11111 */
      {
        unit = 3; /* Multiples of 2 seconds */
        value = (byte)(timer_value / 2000);
        remainder = timer_value % 2000;
      }
      else if(timer_value <= 930000)     /* 930000 is max value corresponding to timer unit 100 and timer value 11111 */
      {
        unit = 4; /* Multiples of 30 seconds */
        value = (byte)(timer_value / 30000);
        remainder = timer_value % 30000;
      }
      else if(timer_value <= 1860000)    /* 1860000 is max value corresponding to timer unit 101 and timer value 11111 */
      {
        unit = 5; /* Multiples of 1 minute */
        value = (byte)(timer_value / 60000);
        remainder = timer_value % 60000;
      }
      else if(timer_value <= 18600000)   /* 18600000 is max value corresponding to timer unit 000 and timer value 11111 */
      {
        unit = 0; /* Multiples of 10 minutes */
        value = (byte)(timer_value / 600000);
        remainder = timer_value % 600000;
      }
      else if(timer_value <= 111600000)  /* 111600000 is max value corresponding to timer unit 001 and timer value 11111 */
      {
        unit = 1; /* Multiples of 1 hour */
        value = (byte)(timer_value / 3600000);
        remainder = timer_value % 3600000;
      }
      else if(timer_value <= 1116000000) /* 1116000000 is max value corresponding to timer unit 010 and timer value 11111 */
      {
        unit = 2; /* Multiples of 10 hours */
        value = (byte)(timer_value / 36000000);
        remainder = timer_value % 36000000;
      }
      else if(timer_value <= 35712000000) /* 35712000000 is max value corresponding to timer unit 110 and timer value 11111 */
      {
        unit = 6; /* Multiples of 320 hours */
        value = (byte)(timer_value / 1152000000);
        remainder = timer_value % 1152000000;
      }
      break;

    default:
      break;
  } /* end switch(timer_type) */

  if(remainder > 0)
  {
    MSG_ERROR_DS_4(MM_SUB, "=MM= Timer value %u sec for timer type %d is %u sec more than the allowed value %u sec, truncating to allowed value",
                   timer_value/1000, timer_type, remainder/1000, (timer_value - remainder)/1000);    
  }

  return (byte) ((unit << 5) | value);
} /* end gmm_encode_gprs_timer_unit_and_value() */


void gmm_read_attach_complete_nv()
{

#ifdef FEATURE_LTE
   emm_ctrl_data_type *emm_s1_ctrl_data_ptr = NULL;
   mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
#endif
   mcfg_fs_status_e_type  status = MCFG_FS_STATUS_EFS_ERR;


#ifdef FEATURE_LTE
  emm_s1_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif


  if ((mm_emm_read_nv & (1<<MM_ATTACH_COMPLETE)))
  {
#if defined(FEATURE_NAS_GW)

     /* Read the WCDMA HSUPA UE CATEGORY which is owned by lower layers */
   if (ghdi_nvmem_read(NV_WCDMA_HSUPA_CATEGORY_I,
            (ghdi_nvmem_data_T *)&mm_nv_wcdma_hsupa_category) != GHDI_SUCCESS)
   {
     mm_nv_wcdma_hsupa_category = MM_RRC_WCMDA_HSUPA_DEFAULT_CATEGORY;  /* Set the default value  */
     MSG_ERROR_DS(MM_SUB, "Unable to Read WCDMA HSUPA category NV (4210); Set to Default value : 5",0,0,0);
   }
   else
   {
     MSG_HIGH_DS_1(MM_SUB, "WCDMA HSUPA category NV (4210) is Read and Set to : %d", mm_nv_wcdma_hsupa_category);
   }
#endif

    /* Report to the Dog HB */
#ifdef FEATURE_LTE
#if defined(FEATURE_NAS_GW)
  dog_hb_report(DOG_MM_RPT);
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call", 
                              &emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'lte_nas_ignore_mt_csfb_during_volte_call' is %d",emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call);
  }
  else
  {
    emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'lte_nas_ignore_mt_csfb_during_volte_call' file could not be read from EFS, status = %d", efs_status);
  }

    dog_hb_report(DOG_MM_RPT);  //bob doubt
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/early_call_init", 
              &emm_s1_ctrl_data_ptr->early_call_init, 
              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Setting early_call_init to %d",
                      emm_s1_ctrl_data_ptr->early_call_init);
  }
  else
  {
    /* Default TRUE, will take decision later for default value*/
    emm_s1_ctrl_data_ptr->early_call_init = TRUE;
    MSG_HIGH_DS_2(MM_SUB ,"=MM= early_call_init file read failed. default %d, status = %d",
                            emm_s1_ctrl_data_ptr->early_call_init, efs_status);
  } 
#endif
#endif

#if defined (FEATURE_LTE_REL12) 
#ifdef FEATURE_DUAL_SIM
   status =  mcfg_fs_read("/nv/item_files/modem/nas/service_req_back_off_timer_value",
                              &mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_1],
                              sizeof(mm_emm_service_req_backoff_timer_value),
                              MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   if (status != MCFG_FS_STATUS_OK)
   {
      mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_1] = 60;
   }
   mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_1] *= 1000;
   status =  mcfg_fs_read("/nv/item_files/modem/nas/service_req_back_off_timer_value",
                              &mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_2],
                              sizeof(mm_emm_service_req_backoff_timer_value),
                              MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_2);
   if (status != MCFG_FS_STATUS_OK)
   {
      mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_2] = 60;
   }
   mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_2] *= 1000;
   MSG_HIGH_DS_3(MM_SUB, "=MM= Nmm_emm_service_req_backoff_timer_value: %d %d %d msecs ",status,
                            mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_1],
                            mm_emm_service_req_backoff_timer_value_sim[MM_AS_ID_2]);
#else 
   status =  mcfg_fs_read("/nv/item_files/modem/nas/service_req_back_off_timer_value",
                             &mm_emm_service_req_backoff_timer_value,
                             sizeof(mm_emm_service_req_backoff_timer_value),
                             MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   if (status != MCFG_FS_STATUS_OK)
   {
      mm_emm_service_req_backoff_timer_value = 60;
   }
   mm_emm_service_req_backoff_timer_value *= 1000;
#ifdef FEATURE_LTE_REL12
   emm_ctrl_data_ptr->service_req_back_off_timer_value = mm_emm_service_req_backoff_timer_value;
#endif
   MSG_HIGH_DS_2(MM_SUB ,"=MM= mm_emm_service_req_backoff_timer_value %d %d msecs", status,
                          mm_emm_service_req_backoff_timer_value);
#endif
#endif

  mm_emm_read_nv = (mm_emm_read_nv ^ (mm_emm_read_nv & (1<<MM_ATTACH_COMPLETE)));

  }

}

#ifdef FEATURE_NAS_GW

/*===========================================================================

FUNCTION GMM_CONVERT_TIMER_VALUE_IE_TO_USECS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  GPRS timer value IE converted to milliseconds.  Note that a return value
  of zero indicates that the timer has been deactivated.

SIDE EFFECTS
  None

===========================================================================*/

dword gmm_convert_timer_value_ie_to_usecs( byte timer_value )
{
  dword timer_value_unit;
  dword timer_value_usecs;

  /* Reset global timer flag */
  gmm_timer_deactivated = FALSE ;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------------------
  ** Convert timer value per TS 24.008 Table 10.5.162
  ** ------------------------------------------------ */
  if ((timer_value & 0xE0) == 0x00)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 2 seconds
    ** ---------------------------------------------- */
    timer_value_unit = 2000;
  }
  else if ((timer_value & 0xE0) == 0x40)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of decihours
    ** ---------------------------------------------- */
    timer_value_unit = 360000 ;
  }
  else if ((timer_value & 0xE0) == 0xE0)
  {
    /* ---------------------------------------------
    ** Value indicates that the timer is deactivated
    ** --------------------------------------------- */
    timer_value_unit = 0;

    /* Indicate to a caller function that the conversion
    ** returns ZERO timer duration due the timer has to
    ** be deactivated
    */
    gmm_timer_deactivated = TRUE ;
  }
  else
  {
    /* ----------------------------------------------------------
    ** Other values shall be interpreted as multiples of 1 minute
    ** ---------------------------------------------------------- */
    timer_value_unit = 60000;
  }

  timer_value = timer_value & 0x1F;

  timer_value_usecs = timer_value_unit * timer_value;

  return (timer_value_usecs);

} /* gmm_convert_timer_value_ie_to_usecs() */


/*===========================================================================

FUNCTION GMM_DELETE_PS_LOCATION_INFORMATION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_delete_ps_location_information( void )
{
  byte i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW_DS( MM_SUB, "=MM= Calling gmm_delete_ps_location_information()", 0,0,0 );

  if (mm_has_valid_ptmsi())
  {
    /* ---------------------------------------------------
    ** Delete P-TMSI, P-TMSI Signature, RAI, and GPRS CKSN
    ** --------------------------------------------------- */
    for (i=0; i < PTMSI_SIZE; i++)
    {
      gmm_stored_ptmsi.digit[i] = 0xFF;
#ifdef FEATURE_SGLTE  
      if(MM_SUB_IS_SGLTE)
      {
        gmm_stored_ptmsi_sim[MM_SGLTE_STACK_2].digit[i] = 0xFF;
      }
#endif
    }
#ifdef FEATURE_DUAL_SIM
    if(MM_MODEM_STATISTICS_SUB1)
#endif
    {
      mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
    }
  }

#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)
  if (is_gsm_mode())
  {
    gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_PTMSI_ASSIGNED,FALSE,0) ;
  }
#endif /* #if defined(FEATURE_GSM) && defined(FEATURE_NAS_GW) */

  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
  {
    gmm_stored_ptmsi_signature.value[i] = 0xFF;
#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      gmm_stored_ptmsi_signature_sim[MM_SGLTE_STACK_2].value[i] = 0xFF;
    }
#endif
  }

  /* ------------------------------------------------------
  ** Reserved value used when no valid RAI exists in the MS
  ** (TS 23.003 4.1 and 4.2)
  ** ------------------------------------------------------ */
  gmm_stored_rai.location_area_code.lac[0] = 0xFF;
  gmm_stored_rai.location_area_code.lac[1] = 0xFE;
  gmm_stored_rai.routing_area_code         = 0xFF;

#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    gmm_stored_rai_sim[MM_SGLTE_STACK_2].location_area_code.lac[0] = 0xFF;
    gmm_stored_rai_sim[MM_SGLTE_STACK_2].location_area_code.lac[1] = 0xFE;
    gmm_stored_rai_sim[MM_SGLTE_STACK_2].routing_area_code         = 0xFF;
  }
#endif

} /* end gmm_delete_ps_location_information() */


/*===========================================================================

FUNCTION GMM_DELETE_GPRS_CKSN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_delete_gprs_cksn( void )
{
  MSG_LOW_DS( MM_SUB, "=MM= Calling gmm_delete_gprs_cksn()", 0,0,0 );

  /* -----------------------------------------------------
  ** Set GPRS CKSN to 0x7  - indicates no key is available
  ** (TS 24.008 Table 5.2)
  ** ----------------------------------------------------- */
  auth_delete_cksn(AUTH_GSM_GPRS_SECURITY_CONTEXT) ;
  auth_delete_cksn(AUTH_PS_GPRS_SECURITY_CONTEXT) ;
} /* end gmm_delete_gprs_cksn() */


/*===========================================================================

FUNCTION GMM_UPDATE_MM_SERVING_PLMN_INFO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_update_mm_serving_plmn_info
(
  sys_sim_state_e_type  sim_state,
  sys_srv_domain_e_type reg_service_domain,
  boolean               mm_deregistered,
  sys_srv_status_e_type service_status,
  boolean               is_gstk_event_required
)
{

  sys_srv_status_e_type prev_service_status = mm_serving_plmn.info.service_status; 
  byte curr_lai[LAI_SIZE];

  if(mm_sim_card_mode != MMGSDI_APP_UNKNOWN)
  {
    if (sim_state == SYS_SIM_STATE_PS_INVALID)
    {
      if (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_INVALID)
      {
        mm_serving_plmn.info.sim_state = SYS_SIM_STATE_CS_PS_INVALID;
        (void) mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,
                                                          MMGSDI_SESSION_APP_IS_ILLEGAL);
      }
      else
      {
        mm_serving_plmn.info.sim_state = SYS_SIM_STATE_PS_INVALID;
      }
    }
    else
    {
      mm_serving_plmn.info.sim_state = sim_state;
    }
  }

  if ((mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_INVALID) ||
      (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_PS_INVALID))
  {
    mm_substate_control( MM_STD_NO_IMSI );
  }

  switch (mm_serving_plmn.ms_op_mode)
  {
    case SYS_SRV_DOMAIN_CS_ONLY:
      switch (reg_service_domain)
      {
        case SYS_SRV_DOMAIN_NO_SRV:
          if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
              (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
          {
            if (mm_deregistered)
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
            }
            else
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_ONLY;
            }
          }
          else
          {
            mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
          }
          break;

        case SYS_SRV_DOMAIN_CS_ONLY:
        case SYS_SRV_DOMAIN_CS_PS:
          mm_serving_plmn.info.reg_service_domain = reg_service_domain;
          mm_serving_plmn.info.service_status     = service_status;
          break;

        case SYS_SRV_DOMAIN_PS_ONLY:
        default:
          MSG_ERROR_DS_1(MM_SUB, "=MM= Unexpected reg_service_domain %d", reg_service_domain);
          break;
      }

      if ((mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE) &&
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) &&
          (mm_check_if_cs_cell_access_allowed(mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION))))
      {
        if (mm_idle_substate == MM_LIMITED_SERVICE)
        {
          mm_serving_plmn.info.service_status = SYS_SRV_STATUS_LIMITED;
        }
        else
        {
          mm_serving_plmn.info.service_status = SYS_SRV_STATUS_SRV;
        }
      }
      else if ((service_status == SYS_SRV_STATUS_NO_SRV) &&
               (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE)&&
               (mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION) == MM_AS_ACCESS_EMERGENCY_ONLY))
      {
        mm_serving_plmn.info.service_status = SYS_SRV_STATUS_LIMITED;
      }
      else
      {
        mm_serving_plmn.info.service_status = service_status;
      }
      break;

    case SYS_SRV_DOMAIN_PS_ONLY:
      mm_serving_plmn.info.reg_service_domain = reg_service_domain;
      mm_serving_plmn.info.service_status     = service_status;
      break;

    case SYS_SRV_DOMAIN_CS_PS:
      if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
      {
        mm_serving_plmn.info.reg_service_domain = reg_service_domain;
      }
      else
      {
        switch (reg_service_domain)
        {
          case SYS_SRV_DOMAIN_NO_SRV:
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              if (mm_deregistered)
              {
                mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
              }
              else
              {
                mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_ONLY;
              }
            }
            else
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
            }
            break;

          case SYS_SRV_DOMAIN_PS_ONLY:
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_PS;
            }
            else if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV)
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_PS_ONLY;
            }
            break;

          case SYS_SRV_DOMAIN_CS_ONLY:
          case SYS_SRV_DOMAIN_CS_PS:
            mm_serving_plmn.info.reg_service_domain = reg_service_domain;
            mm_serving_plmn.info.service_status     = service_status;
            break;

          default:
            MSG_ERROR_DS_1(MM_SUB, "=MM= Unexpected reg_service_domain %d", reg_service_domain);
            break;
        }
      }

      if ((mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE) &&
          ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
           (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)) &&
          (mm_check_if_cs_cell_access_allowed(mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION))))
      {
        if (mm_idle_substate == MM_LIMITED_SERVICE)
        {
          mm_serving_plmn.info.service_status = SYS_SRV_STATUS_LIMITED;
        }
        else
        {
          mm_serving_plmn.info.service_status = SYS_SRV_STATUS_SRV;
        }
      }
      else if ((service_status == SYS_SRV_STATUS_NO_SRV) &&
               (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE) &&
               (mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION) == MM_AS_ACCESS_EMERGENCY_ONLY))
      {
        mm_serving_plmn.info.service_status = SYS_SRV_STATUS_LIMITED;
      }
      else
      {
        mm_serving_plmn.info.service_status = service_status;
      }
      break;

    default:
      MSG_ERROR_DS_1(MM_SUB, "=MM= Unexpected ms_op_mode %d", mm_serving_plmn.ms_op_mode);
      break;
  }

  if ((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV) &&
      (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
  {
    mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
    mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);
    
    if (((mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE) &&
         (cs_cell_access != MM_AS_ACCESS_NO_CALLS))||
        ((mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
         (ps_cell_access != MM_AS_ACCESS_NO_CALLS)))
    {
      mm_serving_plmn.info.service_status = SYS_SRV_STATUS_LIMITED;
    }
    else
    {
      mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
    }
  }
  else if ((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_LIMITED) &&
           (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE) &&
           (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))
  {
    mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
  }

#ifndef GMM_DEBUG
  if ((mm_state == MM_IDLE) && (is_gstk_event_required == TRUE))
  {
    curr_lai[0] = mm_serving_plmn.info.plmn.identity[0];
    curr_lai[1] = mm_serving_plmn.info.plmn.identity[1];
    curr_lai[2] = mm_serving_plmn.info.plmn.identity[2];
   
    curr_lai[3] = mm_serving_plmn.lac.lac[0];
    curr_lai[4] = mm_serving_plmn.lac.lac[1];

    if ((memcmp( mm_previous_serving_lai, curr_lai, LAI_SIZE ) != 0) ||
        (prev_service_status != mm_serving_plmn.info.service_status))
    {
#ifdef FEATURE_GSTK
      mm_send_gstk_mm_idle_info();
#endif
    }
  }
#endif      
}


/*===========================================================================
FUNCTION GMM_SM_SEND_RAT_CHANGE_REQ

DESCRIPTION
  This function notifies RAT change to SM layer. SM halts its operation on the
  current RAT in response to this command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gmm_sm_send_rat_change_req
(
  sys_radio_access_tech_e_type rat
#if defined(FEATURE_LTE)
  , boolean abort_procedure
#endif
)
{
#if 0

  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set         = MS_GMM_SM;
    sm_cmd->header.cmd_id              = GMMSM_RAT_CHANGE_REQ;
    sm_cmd->cmd.gmm_rat_change_req.rat = rat;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_rat_change_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
#if defined(FEATURE_LTE)
    sm_cmd->cmd.gmm_rat_change_req.abort_procedure = abort_procedure;
#endif
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent GMMSM_RAT_CHANGE_REQ");
    sm_put_cmd (sm_cmd);
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
  #endif
}

/*===========================================================================
FUNCTION GMM_SM_SEND_RAT_RESUME_REQ

DESCRIPTION
  This function commands SM to resume its operation on the new RAT

DEPENDENCIES
  This function should only be called when GMM halted SM operation earlier
  during an Inter-RAT Handover/cell reselection procedure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gmm_sm_send_rat_resume_req(void)
{
#if 0
  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_RAT_RESUME_REQ;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_rat_resume_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent GMMSM_RAT_RESUME_REQ");
    sm_put_cmd (sm_cmd);
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
  #endif
}


/*===========================================================================
FUNCTION gmm_send_gmmsm_establish_ind

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gmm_send_gmmsm_establish_ind(sm_cm_status_T cause)
{
  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();


  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_ESTABLISH_IND;
    sm_cmd->cmd.gmm_establish_ind.cause = cause;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_establish_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    //sm_put_cmd (sm_cmd);
   
    sm_process_connection_ind(&sm_cmd->cmd.gmm_establish_ind);
    mm_stop_timer(TIMER_PS_CONN_REL);
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
}


/*===========================================================================
FUNCTION gmm_send_gmmsm_establish_cnf

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gmm_send_gmmsm_establish_cnf(boolean  is_pdp_retransmit_req)
{

  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();


  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_ESTABLISH_CNF;
    sm_cmd->cmd.gmm_establish_cnf.is_pdp_retransmit_req = is_pdp_retransmit_req;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_establish_cnf.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    //sm_put_cmd (sm_cmd);
    sm_process_connection_cnf(
        &sm_cmd->cmd.gmm_establish_cnf,
        TRUE
      );
    mm_stop_timer(TIMER_PS_CONN_REL);

#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif

  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
}



/*===========================================================================
FUNCTION gmm_send_gmmsm_establish_rej

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gmm_send_gmmsm_establish_rej(sm_cm_status_T  cause)

{

  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();


  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_ESTABLISH_REJ;
    sm_cmd->cmd.gmm_establish_rej.cause = cause;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_establish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    //sm_put_cmd (sm_cmd);
   
      sm_process_gprs_rej(&sm_cmd->cmd.gmm_establish_rej);
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif

  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
}


/*===========================================================================
FUNCTION gmm_sm_send_con_release_ind

DESCRIPTION
  This function sends PS Signaling Connection Release to SM with cause passed as input.
  Cause value -SIG_CONNECTION_RELEASE_UE_INITIATED indicates that it is signaling release due to UE request.
  Cause value -SIG_CONNECTION_RELEASE indicates that it is network initiated signaling release. 

  The differentiation (whether network initiated or UE initiated) is required for CM. CM starts some timer after network
  initiated signaling release during which it doesn't allow any PS signaling request to go to UMTS protocol stack.
                        

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void gmm_sm_send_con_release_ind(cc_cause_T cause)
{
    sm_cmd_type * sm_cmd;
    sm_cmd = sm_get_cmd_buf();

    if (sm_cmd != NULL)
    {
      sm_cmd->header.message_set     = MS_GMM_SM;
      sm_cmd->header.cmd_id          = GMMSM_CON_RELEASE_IND;
      sm_cmd->cmd.gmm_con_release_ind.cause = cause;
#ifdef FEATURE_DUAL_SIM
      sm_cmd->cmd.gmm_con_release_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
  
      MSG_HIGH_DS_1(MM_SUB, "=MM= Sent GMMSM_CON_RELEASE_IND with cause : %d", cause);
      
      sm_process_gmm_con_release_ind(&sm_cmd->cmd.gmm_con_release_ind);
      //sm_put_cmd (sm_cmd);
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif

    }
    else
    {
      MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
    }
}

/*===========================================================================
FUNCTION gmm_sm_gprs_release_ind

DESCRIPTION
  This function sends PS Signaling Connection Release to SM with cause passed as input.
  Cause value -SIG_CONNECTION_RELEASE_UE_INITIATED indicates that it is signaling release due to UE request.
  Cause value -SIG_CONNECTION_RELEASE indicates that it is network initiated signaling release. 

  The differentiation (whether network initiated or UE initiated) is required for CM. CM starts some timer after network
  initiated signaling release during which it doesn't allow any PS signaling request to go to UMTS protocol stack.
                        

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void gmm_process_gprs_release_ind(sm_cm_status_T mm_rej_cause)
{
    sm_cmd_type * sm_cmd;
    sm_cmd = sm_get_cmd_buf();

    if (sm_cmd == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
    }

    sm_cmd->header.message_set = MS_GMM_SM;
    sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
    sm_cmd->cmd.gmm_gprs_rel_ind.cause = mm_rej_cause ;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

    //sm_put_cmd( sm_cmd );

    sm_process_gprs_rel(&sm_cmd->cmd.gmm_gprs_rel_ind);

#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif

}


/*===========================================================================
FUNCTION gmm_sm_send_con_release_ind

DESCRIPTION
  This function indicates that PS Signaling Connection Release Request by CM (triggered by RIL due to feature UE Initiated
  Dormancy) is rejected.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void gmm_sm_send_con_release_rej(void)
{
  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_CON_RELEASE_REJ;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmmsm_con_release_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent GMMSM_CON_RELEASE_REJ");
    //sm_put_cmd (sm_cmd);
    
      sm_process_con_rel_rej(
        #ifdef FEATURE_DUAL_SIM
        sm_cmd
        #endif
        );
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
}



/*===========================================================================
FUNCTION gmm_sm_send_fast_dormancy_cnf

DESCRIPTION
  This function sends Fast Dormancy Confirmation to SM with cause passed as input.
  Cause value -SIG_CONNECTION_RELEASE_UE_INITIATED indicates that it is signaling release due to UE request.
  Cause value -SIG_CONNECTION_RELEASE indicates that it is network initiated signaling release. 

  The differentiation (whether network initiated or UE initiated) is required for CM. CM starts some timer after network
  initiated signaling release during which it doesn't allow any PS signaling request to go to UMTS protocol stack.
                        

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void gmm_sm_send_fast_dormancy_cnf(gmmsm_fast_dormancy_cnf_cause_e_type cause)
{
  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_FAST_DORMANCY_CNF;
    sm_cmd->cmd.gmm_con_release_ind.cause = cause;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_con_release_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_1(MM_SUB, "=MM= Sent GMMSM_FAST_DORMANCY_CNF with cause : %d", cause);
    //sm_put_cmd (sm_cmd);
      sm_process_fast_dormancy_cnf(&sm_cmd->cmd.gmm_fast_dormancy_cnf);

#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif

  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
}

#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)

/*===========================================================================

FUNCTION      GMM_PROCESS_DS_REQ

DESCRIPTION
  This function processes MM_PS_DATA_AVAILABLE_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initiate RAU procedure if required

===========================================================================*/
void gmm_process_ds_req
(
  mm_cmd_type *message
)
{
  boolean  routine_man_selection_failed = FALSE;

  switch (message->cmd.hdr.message_id)
  {
    case MM_PS_DATA_AVAIL_REQ :
      /*
      ** If RAU is required by UMTS-To-GSM Inter-RAT change requirements
      ** and has not been initiated from any other reasons, start it now
      */

      MSG_HIGH_DS_0(MM_SUB, "=MM= GMM received MM_PS_DATA_AVAIL_REQ") ;

      if (!((PLMN_MATCH( mm_serving_lai,
                          mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity ) ||
           (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
            sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
     #if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                 && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif   
            ) &&
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) 
      {
        routine_man_selection_failed = TRUE;
        MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
      }

      if ((routine_man_selection_failed) || 
          (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
          (mm_check_forbidden_regional_roaming( &gmm_current_lai )) ||
#ifdef FEATURE_DUAL_SIM
          (reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
#else
          (reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn )) ||
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn ))
#endif
          )
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= GMM ignoring MMCM_PS_DATA_AVAILABLE_REQ") ;
      }
      else if (gmm_rau_is_required)
      {
        /* Initiate RAU */
        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
        {
          gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
        }
        else
        {
          gmm_ra_update_type = GMM_RA_UPDATING;
        }

        gmm_initiate_routing_area_update() ;
      }
      else
      {        
        if ((mm_interrat_state != MM_INTERRAT_CHANGE_STATE_TYPE_LO) || (mm_rat_handler_status != MM_RAT_INITIAL))
        {
          gmm_ps_data_available = TRUE ;
        }
      }
      break;

    default :
      MSG_ERROR_DS_1(MM_SUB, "=MM= Unknown CM command %d", message->cmd.hdr.message_id) ;
      break ;
  }
}

/*===========================================================================

FUNCTION      GMM_PROCESS_CM_REQ

DESCRIPTION
  This function processes MMCM_PS_DATA_AVAILABLE_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initiate RAU procedure if required

===========================================================================*/
void gmm_process_cm_req
(
  mm_cmd_type *message
)
{
#if 0
  boolean  routine_man_selection_failed = FALSE;
#endif
  switch (message->cmd.hdr.message_id)
  {
#if 0
    case MMCM_PS_DATA_AVAILABLE_REQ :
      /*
      ** If RAU is required by UMTS-To-GSM Inter-RAT change requirements
      ** and has not been initiated from any other reasons, start it now
      */

      MSG_HIGH_DS_0(MM_SUB, "=MM= GMM received MMCM_PS_DATA_AVAILABLE_REQ") ;

      if (!((PLMN_MATCH( mm_serving_lai,
                          mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity ) ||
           (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
            sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
     #if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                 && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif   
            ) &&
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) 
      {
        routine_man_selection_failed = TRUE;
        MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
      }

      if ((routine_man_selection_failed) || 
          (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
          (mm_check_forbidden_regional_roaming( &gmm_current_lai )) ||
#ifdef FEATURE_DUAL_SIM
          (reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
#else
          (reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn )) ||
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn ))
#endif
          )
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= GMM ignoring MMCM_PS_DATA_AVAILABLE_REQ") ;
      }
#ifdef FEATURE_NAS_REL10
           else if( mm_timer_status[TIMER_T3346] == TIMER_ACTIVE
#ifdef FEATURE_NAS_REL11
            &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
            )
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= RAU should not be intiated as T3346 is running");
            }
#endif
      else if (gmm_rau_is_required)
      {
        /* Initiate RAU */
        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
        {
          gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
        }
        else
        {
          gmm_ra_update_type = GMM_RA_UPDATING;
        }

        gmm_initiate_routing_area_update() ;
      }
      else
      {        
        if ((mm_interrat_state != MM_INTERRAT_CHANGE_STATE_TYPE_LO) || (mm_rat_handler_status != MM_RAT_INITIAL))
        {
          gmm_ps_data_available = TRUE ;
        }
      }
      break ;
#endif

#ifndef FEATURE_NAS_DISABLE_VOICE
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
    case MMCM_IMS_REGISTRATION_INFO:
      gmm_process_ims_voice_tag_reg_status_ind(message->cmd.cm_mm_ims_registration_info.ims_registration_status);
      break;
#endif
#endif

    default :
      MSG_ERROR_DS_1(MM_SUB, "=MM= Unknown CM command %d", message->cmd.hdr.message_id) ;
      break ;
  }
} /* end gmmllc_process_cm_req() */



/*===========================================================================

FUNCTION gmm_read_nv_params

DESCRIPTION
       Gets the GPRS NV param which are common for all subscription

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_read_nv_params(void)
{
/* Read PSHO NV item*/
 ghdi_status_T nv_status = GHDI_SUCCESS;
mm_as_id = MM_AS_ID_1;



/* --------------------------------------------------------------------
 ** Read the "Repeated ACCH" bit of classmark 3 support Parameters value from the NV.
 ** Any non-zero value implies this bit be set to 1 else to 0.
 ** ------------------------------------------------------------------*/
 
#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
 if ((nv_status = ghdi_nvmem_read(NV_REPEATED_ACCH_I,
          (ghdi_nvmem_data_T *)&gmm_nv_r_acch_feature_support)) != GHDI_SUCCESS)
 {
   gmm_nv_r_acch_feature_support = GSM_R_ACCH_SUPPORTED;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for R-ACCH Feature support flag: %d", nv_status);
 }
#endif /* FEATURE_GSM_R_SACCH or FEATURE_GSM_R_FACCH*/


#ifdef FEATURE_REL_6_SUPPORT
 /* --------------------------------------------------------------------
 ** Read the SAIC Support Flag from the NV
 ** ------------------------------------------------------------------*/

/* -----------gmm_nv_saic_feature_support global for indicating Downlink Advanced Receiver Performance  ----
 * 
 *- If FEATURE_GSM_EGPRS_SAIC or FEATURE_GSM_MDSP_AEQ is OFF, indicate no support to NW .. 
 * -else if, NV GSMRxDivCtlFlag is 7, phase 2 DARP supported and read the value 
 *                  from NV NV_DARP_FEATURE_SUPPORT_I (value caps at 2)
 *  else UE supports phase 1 DARP and read the support value 
 *                  from NV_DARP_FEATURE_SUPPORT_I (value caps at 1)
 *
 */

#if (defined(FEATURE_GSM_EGPRS_SAIC) && defined(FEATURE_GSM_MDSP_AEQ))
 if ((nv_status = ghdi_nvmem_read(NV_DARP_FEATURE_SUPPORT_I,
          (ghdi_nvmem_data_T *)&gmm_nv_saic_feature_support)) != GHDI_SUCCESS)
 {
   /*GSM_EGPRS_SAIC_SUPPORTED default value is 1 if both the feature flags FEATURE_GSM_EGPRS_SAIC and
   FEATURE_GSM_MDSP_AEQ are enabled otherwise default value is  0*/

   gmm_nv_saic_feature_support = GSM_EGPRS_SAIC_SUPPORTED;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for SAIC Support: %d", nv_status);
 }
 else if (gmm_nv_saic_feature_support > 1)
 {
   boolean  local_gsm_rx_diversity = 0; 
   mcfg_fs_status_e_type efs_get_status = mcfg_fs_read("/nv/item_files/gsm/gl1/gsm_rx_diversity",
                                                         &local_gsm_rx_diversity,sizeof(boolean),MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_as_id);
   if(efs_get_status != MCFG_FS_STATUS_OK)
   {
     local_gsm_rx_diversity = 0;
   }
   MSG_HIGH_DS_2(MM_SUB, "=MM= Value of local_gsm_rx_diversity = %d, status = %d", local_gsm_rx_diversity, efs_get_status);

   if(local_gsm_rx_diversity == 7)
   {
     if(gmm_nv_saic_feature_support > 2)
     {
       gmm_nv_saic_feature_support = 2;
     }
   }
   else
   {
     if(gmm_nv_saic_feature_support > 1)
     {
       gmm_nv_saic_feature_support = 1;
     }
   }
   
 }
  MSG_HIGH_DS_1(MM_SUB, "=MM= Value of gmm_nv_saic_feature_support = %d", gmm_nv_saic_feature_support);
#endif /* FEATURE_GSM_EGPRS_SAIC */
#endif /* FEATURE_REL_6_SUPPORT */

}
#endif /* #if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW) */
/*===========================================================================

FUNCTION GMM_SEND_NV_PARAMS

DESCRIPTION
       Gets the GPRS NV Items from the NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_send_nv_params (void)
{
 
 mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)
 ghdi_status_T nv_status;
 
 #ifdef FEATURE_LTE
 mm_nas_nv_classmark_ie_T mm_nas_nv_classmark_ie;
 #endif

 boolean qos_support_enabled = TRUE;

 /* --------------------------------------------------------------------
 ** Read the MULTISLOT_CLASS value from the NV
 ** ------------------------------------------------------------------*/
 if ((nv_status = mm_ghdi_nvmem_read(NV_MULTISLOT_CLASS_I,
          (ghdi_nvmem_data_T *)&gmm_multislot_class)) != GHDI_SUCCESS)
 {
   gmm_multislot_class = GPRS_MULTISLOT_CLASS;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for Multislot class: %d", nv_status);
 }
 else if (( gmm_multislot_class == 0) || ((gmm_multislot_class > 18) && (gmm_multislot_class < 30)) || (gmm_multislot_class > 34))
 {
   MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for Multislot_class %d", gmm_multislot_class);
   gmm_multislot_class = GPRS_MULTISLOT_CLASS;  /* Set the default value */
 }
#if defined (FEATURE_GSM_GPRS_MSC34) && !defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
 else if ((gmm_multislot_class >= 30) && (gmm_multislot_class <= 34))
 {
   MSG_HIGH_DS_2(MM_SUB, "=MM= GPRS Higher Multislot Class:%d not supported, default to class:%d", 
                 gmm_multislot_class, GPRS_MULTISLOT_CLASS);
   gmm_multislot_class = GPRS_MULTISLOT_CLASS;  /* Set the default value */  
 }
#endif /*defined (FEATURE_GSM_GPRS_MSC34) && "defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)*/

 #if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_NAS_GW)
 /* --------------------------------------------------------------------
 ** Read the EDGE FEATURE SUPPORT value from the NV
 ** ------------------------------------------------------------------*/
 if ((nv_status = mm_ghdi_nvmem_read(NV_EDGE_FEATURE_SUPPORT_I,
          (ghdi_nvmem_data_T *)&gmm_edge_feature_support)) != GHDI_SUCCESS)
 {
     gmm_edge_feature_support = GPRS_EDGE_FEATURE_SUPPORT_DEFAULT;
 }
 else if (gmm_edge_feature_support > 1)
 {
     gmm_edge_feature_support = GPRS_EDGE_FEATURE_SUPPORT_DEFAULT;
 }

/* ------------------------------------------------------------------------
** This feature needs to be enabled to enable the EGPRS support on MSM6250
** targets.
** ----------------------------------------------------------------------*/

 #ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
 gmm_edge_feature_support = GPRS_EDGE_FEATURE_NOT_SUPPORTED;
 #endif


 MSG_HIGH_DS_1(MM_SUB, "=MM= EDGE Feature Support %d", gmm_edge_feature_support);

 /* --------------------------------------------------------------------
 ** Read and send the EDGE feature related NV information to
 ** lower layers only if EDGE is supported by the phone
 ** ------------------------------------------------------------------*/

 if (gmm_edge_feature_support)
 {
     /** ------------------------------------------------------------------
     ** Read the EDGE MULTISLOT CLASS value from the NV
     ** ------------------------------------------------------------------*/
     if ((nv_status = mm_ghdi_nvmem_read(NV_EDGE_MULTISLOT_CLASS_I,
              (ghdi_nvmem_data_T *)&gmm_edge_multislot_class)) != GHDI_SUCCESS)
     {
         gmm_edge_multislot_class = GPRS_EDGE_MULTISLOT_CLASS;
     }
     else if (( gmm_edge_multislot_class == 0) || (gmm_edge_multislot_class > 34))
     {
         gmm_edge_multislot_class = GPRS_EDGE_MULTISLOT_CLASS;
     }
#if defined (FEATURE_GSM_GPRS_MSC34) && !defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
     else if ((gmm_edge_multislot_class >= 30) && (gmm_edge_multislot_class <= 34))
     {
       MSG_HIGH_DS_2(MM_SUB, "=MM= EGPRS Higher Multislot Class:%d not supported, default to class:%d", 
                     gmm_edge_multislot_class, GPRS_EDGE_MULTISLOT_CLASS);
       gmm_edge_multislot_class = GPRS_EDGE_MULTISLOT_CLASS;  /* Set the default value */  
     }
#endif /*defined (FEATURE_GSM_GPRS_MSC34) && "defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)*/

     MSG_HIGH_DS_1(MM_SUB, "=MM= Value of EDGE Multislot Class %d", gmm_edge_multislot_class);

     /* --------------------------------------------------------------------
     ** Read the 8PSK POWER CLASS value from the NV
     ** ------------------------------------------------------------------*/
     if ((nv_status = mm_ghdi_nvmem_read(NV_EDGE_8PSK_POWER_CLASS_I,
              (ghdi_nvmem_data_T *)&gmm_edge_8psk_power_class)) != GHDI_SUCCESS)
     {
         gmm_edge_8psk_power_class = GPRS_EDGE_8PSK_POWER_CLASS;/* Set the default value indicating the power class as E2 */
         MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for EDGE_8PSK_POWER_CLASS  Feature support flag : %d", nv_status);
     }
     else if (gmm_edge_8psk_power_class > 3)
     {
         gmm_edge_8psk_power_class = GPRS_EDGE_8PSK_POWER_CLASS;
     }
     MSG_HIGH_DS_1(MM_SUB, "=MM= 8PSK Uplink Modulation support %d", gmm_edge_8psk_power_class);

     /* --------------------------------------------------------------------
     ** Read the 8PSK POWER CAPABILITY value from the NV
     ** ------------------------------------------------------------------*/
     if ((nv_status = mm_ghdi_nvmem_read(NV_EDGE_8PSK_POWER_CAPABILITY_I,
              (ghdi_nvmem_data_T *)&gmm_edge_8psk_power_capability)) != GHDI_SUCCESS)
     {
         gmm_edge_8psk_power_capability = GPRS_EDGE_8PSK_POWER_CAPABILITY;
     }
     else if (gmm_edge_8psk_power_capability > 3)
     {
         gmm_edge_8psk_power_capability = GPRS_EDGE_8PSK_POWER_CAPABILITY; /* This is currently not used */
     }
     MSG_HIGH_DS_1(MM_SUB, "=MM= EDGE 8PSK Power Capability %d", gmm_edge_8psk_power_capability);
     /* set 8-PSK multislot profile based on two NV items   NV_EDGE_8PSK_POWER_CLASS_I and
     NV_MULTISLOT_8PSK_POWER_PROFILE_I . NV_EDGE_8PSK_POWER_CLASS_I identifies what is 8psk tx power class. 
     if it is set to 0, it indicates MS does not support 8psk in the UL. Default value is 1 which is Power class E2.
     If both the NVs are set then gsclass mark will pass
     the NV value coded in NV_MULTISLOT_8PSK_POWER_PROFILE_I.   If NV_EDGE_8PSK_POWER_CLASS_I indicate no 8psk UL support,
     then gsclass mark will pass '00' indicating it is not suppported. if NV_EDGE_8PSK_POWER_CLASS_I is defined as power class 1 
     and NV reading of NV_MULTISLOT_8PSK_POWER_PROFILE_I is failed then pass '0x03' indicating 8-PSK multislot profile
     is not supported. */
     if ((nv_status = mm_ghdi_nvmem_read( NV_MULTISLOT_8PSK_POWER_PROFILE_I ,
         (ghdi_nvmem_data_T *)&gmm_nv_multislot_8psk_power_profile)) != GHDI_SUCCESS)
     {
         gmm_nv_multislot_8psk_power_profile = 0x03;  /* Set the default value when UL 8 psk power profile is supported */
         MSG_ERROR_DS_1(MM_SUB,"=MM= Unable to read NV for MULTISLOT_8PSK_POWER_PROFILE  Feature support flag : %d", nv_status);
     }
 
#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_NAS_GW)
     if (gmm_edge_8psk_power_class == 0x00) /*NV read successful and has a value of no 8psk  UL support*/
#endif
     {
         gmm_nv_multislot_8psk_power_profile = 0x00; /* Set the default value when UL 8 psk power profile is not supported*/
     }
 }
 else
 {
     gmm_edge_8psk_power_class = 0; /* EDGE is not supported.So no UL 8psk modulation*/
     gmm_nv_multislot_8psk_power_profile = 0; /* EDGE is not supported.So no UL 8psk modulation*/
 }
#endif /* FEATURE_GSM_EGPRS */

#ifdef FEATURE_REL_4_SUPPORT

#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
 /* --------------------------------------------------------------------
 ** Read the N/W ASSISTED CELL CHANGE SUPPORT value from the NV
 ** ------------------------------------------------------------------*/
 if ((nv_status = mm_ghdi_nvmem_read(NV_GERAN_FEATURE_PACK_1_I,
          (ghdi_nvmem_data_T *)&gmm_edge_nw_ass_cell_change)) != GHDI_SUCCESS)
 {
     gmm_edge_nw_ass_cell_change = GPRS_EDGE_NW_ASS_CELL_CHANGE;
 }
 else if (gmm_edge_nw_ass_cell_change > 1)
 {
     gmm_edge_nw_ass_cell_change = GPRS_EDGE_NW_ASS_CELL_CHANGE; /* This is NOT supported - hence default to '0' */
 }

 MSG_HIGH_DS_1(MM_SUB, "=MM= Support for N/W Ass Cell Change %d", gmm_edge_nw_ass_cell_change);

  /* --------------------------------------------------------------------
 ** Read the INTERRAT NACC Support value from the NV
 ** ------------------------------------------------------------------*/
#if defined FEATURE_WTOG_NACC
 if ((nv_status = mm_ghdi_nvmem_read(NV_INTERRAT_NACC_SUPPORT_I,
          (ghdi_nvmem_data_T *)&gmm_interrat_nacc_support)) != GHDI_SUCCESS)
#endif
 {
     gmm_interrat_nacc_support = GPRS_INTERRAT_NACC_SUPPORT;
 }

 MSG_HIGH_DS_1(MM_SUB, "=MM= Support for INTERRAT NACC %d", gmm_interrat_nacc_support);

#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */

#endif /* FEATURE_REL_4_SUPPORT */

 /* --------------------------------------------------------------------
 ** Read the NON DRX TIMER value from the NV
 ** ------------------------------------------------------------------*/
 if ((nv_status = mm_ghdi_nvmem_read(NV_NON_DRX_TIMER_I,
          (ghdi_nvmem_data_T *)&gmm_non_drx_timer)) != GHDI_SUCCESS)
 {
   gmm_non_drx_timer = GPRS_NON_DRX_TIMER;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for non_drx_timer: %d", nv_status);
 }
 else if (gmm_non_drx_timer > 7)
 {
   MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for non_drx_timer %d", gmm_non_drx_timer);
   gmm_non_drx_timer = GPRS_NON_DRX_TIMER;  /* Set the default value */
 }

 /* --------------------------------------------------------------------
 ** Read the SPLIT PAGING CYCLE value from the NV
 ** ------------------------------------------------------------------*/
 if ((nv_status = mm_ghdi_nvmem_read(NV_SPLIT_PAGING_CYCLE_I,
          (ghdi_nvmem_data_T *)&gmm_split_paging_cycle)) != GHDI_SUCCESS)
 {
   gmm_split_paging_cycle = GPRS_SPLIT_PAGING_CYCLE;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for split_paging_cycle: %d", nv_status);
 }
 /* Table 10.5.139 has the the binary coded value Split paging cycle */
 else if (gmm_split_paging_cycle > 98)
 {
   MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for split_paging_cycle %d", gmm_split_paging_cycle);
   gmm_split_paging_cycle = GPRS_SPLIT_PAGING_CYCLE;  /* Set the default value */
 }

 /* --------------------------------------------------------------------
 ** Read the ANITE GCF FLAG value from the NV
 ** ------------------------------------------------------------------*/
 if ((nv_status = mm_ghdi_nvmem_read(NV_GPRS_ANITE_GCF_I,
          (ghdi_nvmem_data_T *)&gmm_anite_gcf_flag)) != GHDI_SUCCESS)
 {
   gmm_anite_gcf_flag = FALSE;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for Anite GCF Flag: %d", nv_status);
 }
 /* Table 10.5.139 has the the binary coded value Split paging cycle */
 else if ( (gmm_anite_gcf_flag != FALSE) && (gmm_anite_gcf_flag != TRUE))
 {
   MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for Anite GCF Flag %d", gmm_anite_gcf_flag);
   gmm_anite_gcf_flag = FALSE;  /* Set the default value */
 }

 if(gmm_anite_gcf_flag)
 {

   mm_managed_roaming_enabled = FALSE;

   mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page = 0;
 }

 MSG_HIGH_DS_2(MM_SUB, "=MM= MAN_ROAM: NV Feature Flag: %d, NV NAS CONFIG FEATURE  UE_init_rel_on_spurious_page: %d",
               mm_managed_roaming_enabled, mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page);

/* --------------------------------------------------------------------
 ** Read the EDTM Support Parameters value from the NV
 ** ------------------------------------------------------------------*/

#ifdef FEATURE_GSM_EDTM
 if ((nv_status = mm_ghdi_nvmem_read(NV_EDTM_FEATURE_SUPPORT_I ,
          (ghdi_nvmem_data_T *)&gmm_nv_edtm_feature_support)) != GHDI_SUCCESS)
 {
   gmm_nv_edtm_feature_support = GPRS_EDGE_EDTM_SUPPORTED;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for EDTM Feature support flag: %d", nv_status);
 }
 else
 {
   if ((gmm_nv_edtm_feature_support != FALSE) && (gmm_nv_edtm_feature_support != TRUE))
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for EDTM Feature support flag %d", gmm_nv_edtm_feature_support);
     gmm_nv_edtm_feature_support = GPRS_EDGE_EDTM_SUPPORTED;  /* Set the default value */
   }
 }
#endif

 /* --------------------------------------------------------------------
 ** Read the MSC12/EDA Support Flag from the NV
 ** ------------------------------------------------------------------*/

#ifdef FEATURE_GPRS_EDA
 if ( gmm_nv_eda_feature_support )
 {
   /* no capping is required */
 }
 else
#endif  
 {
   /* EDA is disabled, so ensure MSC is capped at Class 10 */

   if (( gmm_multislot_class > 0xA ) && ( gmm_multislot_class < 0x1E ))
   {
     gmm_multislot_class = 0xA;
   }
   else if (( gmm_multislot_class > 0x1F ) && ( gmm_multislot_class < 0x23 ))
   {
     gmm_multislot_class = 0x1F;
     MSG_ERROR_DS_0(MM_SUB, "=MM= Inconsistency in the values of gmm_multislot_class and nv_eda_feature_support");
   }
   
#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_NAS_GW)
   if (( gmm_edge_multislot_class > 0xA ) && ( gmm_edge_multislot_class < 0x1E ))
   {  
     gmm_edge_multislot_class = 0xA;
   }     
   else if (( gmm_edge_multislot_class > 0x1F ) && ( gmm_edge_multislot_class < 0x23 ))
   {
     gmm_edge_multislot_class = 0x1F;
     MSG_ERROR_DS_0(MM_SUB, "=MM= Inconsistency in the values of gmm_edge_multislot and nv_eda_feature_support");
   }    
#endif /* FEATURE_GSM_EGPRS */
 }
    

/* --------------------------------------------------------------------
 ** Read the GEA Algorithm Supported request from the NV
 ** ------------------------------------------------------------------*/
 if ((nv_status = mm_ghdi_nvmem_read(NV_GPRS_GEA_ALGORITHMS_SUPPORTED_I ,
          (ghdi_nvmem_data_T *)&gmm_gea_support)) != GHDI_SUCCESS)
 {
   gmm_gea_support = GPRS_GEA_SUPPORT;  /* Set the default value to support GEA1 & GEA2*/
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for GEA Algorithm support: %d", nv_status);
 }
 /* Changing the condition from 7 to 127 because,NV read is bit format of all supported algo
    so the format will be '01111111b' in binary, so the  condition will be >127 instead of >7*/
 else if (gmm_gea_support > 127) 
 {
   gmm_gea_support = GPRS_GEA_SUPPORT;
 }    
#ifdef FEATURE_NAS_REL12
  if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL12)
  {
    /* Spec 24.008, Spec CR 2352 -Prohibited usage of GEA1 by SA3 
     * For Rel-11, text and a note is added pointing to the SA3 recommendation not to use GEA1.
     * For Rel-12, text introduced to indicate that GEA1 is prohibited.
     */

    /* If GEA1 support is enabled from NV, disable it 
     * here for Rel-12 
     */
    gmm_gea_support &= ~0x01;
  }
#endif
 MSG_HIGH_DS_1(MM_SUB, "=MM= GEA Algorithm support value: %d", gmm_gea_support);

 /* Read NV items related to GMSK Multislot Power Profile */
 if ((nv_status = mm_ghdi_nvmem_read(NV_MULTISLOT_GMSK_POWER_PROFILE_I ,
               (ghdi_nvmem_data_T *)&gmm_nv_multislot_gmsk_power_profile)) != GHDI_SUCCESS)
 {
   gmm_nv_multislot_gmsk_power_profile = 0x03;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for MULTISLOT_GMSK_POWER_PROFILE Feature support : %d", nv_status);
 }
#endif
#ifdef FEATURE_LTE 
  status = mcfg_fs_read("/nv/item_files/modem/nas/nas_nv_classmark_ie",&mm_nas_nv_classmark_ie,sizeof(mm_nas_nv_classmark_ie),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_as_id);


  if(status != MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV for nas_nv_classmark_ie, status = %d", status);

#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)
    /* If nv read fails, we set it to default value TRUE*/
    gmm_nv_geran_to_eutra_measurement_support = 0x01;
#endif

    /* If nv read fails, we set it to default value TRUE*/
    gmm_nv_eutra_measurement_reporting_support = 0x01;
  }
  else
  {
#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)  
    gmm_nv_geran_to_eutra_measurement_support = mm_nas_nv_classmark_ie.geran_to_eutra_measurement_support;

	/* If the value is out of range, we set it to default value TRUE*/
    if(gmm_nv_geran_to_eutra_measurement_support >1)
    {
      gmm_nv_geran_to_eutra_measurement_support = 0x01;
     
    }
#endif
    gmm_nv_eutra_measurement_reporting_support = mm_nas_nv_classmark_ie.eutra_measurement_reporting_support;
    MSG_HIGH_DS_2(MM_SUB, "=MM= gmm_nv_geran_to_eutra_measurement_support = %lu, gmm_nv_eutra_measurement_reporting_support = %lu, val from NV - ", 
                                gmm_nv_geran_to_eutra_measurement_support,gmm_nv_eutra_measurement_reporting_support);
   
   /* If the value is out of range, we set it to default value TRUE*/
    if(gmm_nv_eutra_measurement_reporting_support >1)
    {
      gmm_nv_eutra_measurement_reporting_support = 0x01;
    }
 }

#endif  /*FEATURE_LTE*/
#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)  
   if((status = mcfg_fs_read("/nv/item_files/modem/data/3gpp/umtsps/qos_support",&qos_support_enabled,sizeof(boolean),
                     MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_as_id)) == MCFG_FS_STATUS_OK)
   {
      if(!qos_support_enabled)
      {
        gmm_xid_negotiation_completed = TRUE;
        gmm_max_L3_length = MAX_OCTETS_L3_MESSAGE_PS_GRAT;
        MSG_HIGH_DS_1(MM_SUB, "=MM= setting XID negotiation to TRUE by default: %d ", gmm_xid_negotiation_completed);
      }
      else
      {
        gmm_xid_negotiation_completed = FALSE;
      }
   }
   else
   {
      MSG_HIGH_DS_1(MM_SUB, "=MM= Setting XID negotiation as FALSE  as read failed, status = %d", status);
      gmm_xid_negotiation_completed = FALSE;
   }

  gmm_read_carrier_specific_efs();
  gmm_send_nv_params_to_rr();
#endif
}

/*===========================================================================

FUNCTION GMM_PARSE_PDP_CONTEXT_IE

DESCRIPTION
       Parses PDP Context IE received from N/w.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_parse_pdp_context_status_ie
(
  gmm_ie_pdp_context_status_T  *pdp_context_status,
  byte                         *msg_p
)
{
  /*************************************************************************
  *TS 24.007, section 11.4.2
  * --  It is not a syntactical error that a type 4 standard IE specifies in 
  * its length indicator a greater length than possible according to the 
  * value part specification: extra bits shall be ignored.
  *
  *TS 24.007, section 11.2.1.1.4
  *A type 4 standard information element has format LV or TLV. Its LI precedes 
  *the value part, which consists of zero, one, or more octets; if present, 
  *its IEI has one octet length and precedes the LI. 
  *
  *Hence, this is a type 4 information element.
  **************************************************************************/ 
  if(msg_p[0] > GMM_PDP_CONTEXT_STATUS_LENGTH)
  {
   MSG_ERROR_DS_2(MM_SUB, "=MM= PDP Context Status IE length %d is more than spec defined length %d", 
                  msg_p[0], GMM_PDP_CONTEXT_STATUS_LENGTH);
  }
  pdp_context_status->length = MIN(msg_p[0],GMM_PDP_CONTEXT_STATUS_LENGTH);
  memscpy(&(pdp_context_status->value[0]), GMM_PDP_CONTEXT_STATUS_LENGTH, ++msg_p, pdp_context_status->length);
}

/*===========================================================================

FUNCTION GMM_PARSE_MBMS_CONTEXT_IE

DESCRIPTION
       Parses MBMS Context IE received from N/w.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_parse_mbms_context_status_ie
(
  gmm_ie_mbms_context_status_T  *mbms_context_status,
  const byte                          *msg_p
)
{
  /*************************************************************************
  *TS 24.007, section 11.4.2
  * --  It is not a syntactical error that a type 4 standard IE specifies in 
  * its length indicator a greater length than possible according to the 
  * value part specification: extra bits shall be ignored.
  *
  *TS 24.007, section 11.2.1.1.4
  *A type 4 standard information element has format LV or TLV. Its LI precedes 
  *the value part, which consists of zero, one, or more octets; if present, 
  *its IEI has one octet length and precedes the LI. 
  *
  *Hence, this is a type 4 information element.
  **************************************************************************/ 
  if(msg_p[0] > GMM_MBMS_CONTEXT_STATUS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=MM= MBMS Context Status IE length %d is more than spec defined length %d", 
                   msg_p[0], GMM_MBMS_CONTEXT_STATUS_LENGTH);
  }
  mbms_context_status->length = MIN(msg_p[0],GMM_MBMS_CONTEXT_STATUS_LENGTH);
  memscpy(&(mbms_context_status->value[0]), GMM_MBMS_CONTEXT_STATUS_LENGTH, ++msg_p, mbms_context_status->length);
}

/*===========================================================================

FUNCTION GMM_SEND_SM_PDP_MBMS_STATUS_IND_IF_NEEDED

DESCRIPTION
       Sends GMMSM_PDP_MBMS_STATUS_IND to SM if pdp_context_status_present or
       mbms_context_status_present is TRUE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_send_sm_pdp_mbms_status_ind_if_needed
(
  const gmm_ie_pdp_context_status_T  *pdp_context_status,
  boolean                      pdp_context_status_present,
  const gmm_ie_mbms_context_status_T *mbms_context_status,
  boolean                      mbms_context_status_present
)
{
  if (pdp_context_status_present  == TRUE ||
      mbms_context_status_present == TRUE)
  {
    gmm_send_sm_pdp_mbms_status_ind(pdp_context_status, mbms_context_status);
  }
}

/*===========================================================================

FUNCTION GMM_SEND_SM_PDP_MBMS_STATUS_IND

DESCRIPTION
       Sends GMMSM_PDP_MBMS_STATUS_IND to SM. The content of the message is taken from
       pdp_context_status and mbms_context_status_present passed as input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void gmm_send_sm_pdp_mbms_status_ind
(
  const gmm_ie_pdp_context_status_T  *pdp_context_status,
  const gmm_ie_mbms_context_status_T *mbms_context_status
)  
{
  sm_cmd_type  *sm_cmd;
  sm_cmd       = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_PDP_MBMS_STATUS_IND;

    sm_cmd->cmd.gmm_pdp_mbms_context_status.pdp_status_length = 
           pdp_context_status->length;
                   
   memscpy(&(sm_cmd->cmd.gmm_pdp_mbms_context_status.value[0]), GMM_PDP_MBMS_CONTEXT_STATUS_LENGTH,
           pdp_context_status->value, pdp_context_status->length);

    sm_cmd->cmd.gmm_pdp_mbms_context_status.mbms_status_length = 
           mbms_context_status->length;

    if (pdp_context_status->length>0)
    {
      memscpy(&(sm_cmd->cmd.gmm_pdp_mbms_context_status.value[2]), GMM_PDP_MBMS_CONTEXT_STATUS_LENGTH-2,
             mbms_context_status->value, mbms_context_status->length);
    }
    else
    {
      memscpy(&(sm_cmd->cmd.gmm_pdp_mbms_context_status.value[0]), GMM_PDP_MBMS_CONTEXT_STATUS_LENGTH,
         mbms_context_status->value, mbms_context_status->length);
    }
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_pdp_mbms_context_status.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent GMMSM_PDP_MBMS_STATUS_IND");
    //sm_put_cmd (sm_cmd);
    
    sm_process_pdp_mbms_status(&sm_cmd->cmd.gmm_pdp_mbms_context_status);
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
}


/*===========================================================================

FUNCTION GMM_CHECK_IF_PS_CELL_ACCESS_BARRED

DESCRIPTION
       This function checks if PS Access is allowed in the current cell.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if
    (a) Network has barred(*) PS Access with Domain Specific Access Restrictions. OR
    (b) Network has barred UE from accessing cell with Access Class Restrictions.  
  FALSE otherwise.  
(*) Barring means access restrictions set to Emergency or No Calls.

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_check_if_ps_cell_access_barred(mm_as_cell_access_status_T ps_cell_access)
{
  if ( (ps_cell_access == MM_AS_ACCESS_NO_CALLS) ||
       (ps_cell_access == MM_AS_ACCESS_EMERGENCY_ONLY))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }     
}

/*===========================================================================

FUNCTION GMM_CHECK_IF_PS_CELL_ACCESS_ALLOWED

DESCRIPTION
       This function checks if PS Access is allowed in the current cell.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if
    (a) Network has not barred(*) PS Access with Domain Specific Access Restrictions. OR
    (b) Network has not barred UE from accessing cell with Access Class Restrictions.  
  FALSE otherwise.  
(*) Barring means access restrictions set to Emergency or No Calls.

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_check_if_ps_cell_access_allowed(mm_as_cell_access_status_T ps_cell_access)
{
  if ( (ps_cell_access == MM_AS_ACCESS_ALL_CALLS) ||
       (ps_cell_access == MM_AS_ACCESS_NORMAL_ONLY))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }     
}

/*===========================================================================

FUNCTION GMM_GET_PS_CELL_ACCESS

DESCRIPTION
       This function returns cell access status for PS Domain.

       Call this fundtion with the below values as orguments.

       For PS registration :PROCEDURE_PS_REGISTRATION
       For Ps paging response : PROCEDURE_PS_PAGING_RSP
       For any other purpose : PROCEDURE_NONE
 

DEPENDENCIES
  None

RETURN VALUE
  Applied Access Class Restriction for PS Domain in current cell.

SIDE EFFECTS
  None

===========================================================================*/
mm_as_cell_access_status_T gmm_get_ps_cell_access
(
  nas_procedure_for_cell_access_T nas_procedure
)
{
  mm_as_cell_access_status_T ps_cell_access = MM_AS_ACCESS_ALL_CALLS;
  
  
  switch (nas_procedure)
  {
    case PROCEDURE_PS_REGISTRATION:
       if((mm_system_information.ppac_present == TRUE) && is_wcdma_mode())
       {
          MSG_HIGH_DS_1(MM_SUB, "=MM= PPAC says PS registration allowed is %d", mm_system_information.ps_reg_allowed);
          if (mm_system_information.ps_reg_allowed == FALSE)
          {
           ps_cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
          }
       }
#ifdef FEATURE_DSAC
       else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
       {
         ps_cell_access = mm_system_information.ps_cell_access;
       }
       else
       {
         ps_cell_access = mm_system_information.cell_access;
       }
#else
       else
       {
         ps_cell_access = mm_system_information.cell_access;
       }
#endif
       break;
    case PROCEDURE_PS_PAGING_RSP:

#ifdef FEATURE_WCDMA
       if((mm_system_information.ppac_present == TRUE) && is_wcdma_mode())
       {
          if ((mm_system_information.page_rsp_allowed == MM_AS_CS_PS_ALLOWED) ||
              (mm_system_information.page_rsp_allowed == MM_AS_PS_ALLOWED))
          {
            MSG_HIGH_DS_1(MM_SUB, "=MM= PPAC allows ps paging response %d", mm_system_information.page_rsp_allowed);
          }
#ifdef FEATURE_DSAC
          else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
          {
             ps_cell_access = mm_system_information.ps_cell_access;
          }
          else
          {
             ps_cell_access = mm_system_information.cell_access;
          }
#else   
          else
          {
             ps_cell_access = mm_system_information.cell_access;
          }
#endif
       }
#endif
#ifdef FEATURE_DSAC
       else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
       {
         ps_cell_access = mm_system_information.ps_cell_access;
       }
       else
       {
         ps_cell_access = mm_system_information.cell_access;
       }
       else
#else

       {
         ps_cell_access = mm_system_information.cell_access;
       }
#endif
       break;
    default:
#ifdef FEATURE_DSAC
       if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
       {
         ps_cell_access = mm_system_information.ps_cell_access;
       }
       else
       {
         ps_cell_access = mm_system_information.cell_access;
       }
#else
       {
         ps_cell_access = mm_system_information.cell_access;
       }
#endif
       break;
  }
  return ps_cell_access;  
}

#ifdef FEATURE_MBMS
/*===========================================================================

FUNCTION GMM_SEND_MSM_MBMS_SUPP_IND

DESCRIPTION
  This function notifies MBMS support at RA level received in Attach Accept
  and RAU Accept to MBMS Service Manager (Acronym: MSM)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_send_msm_mbms_supp_ind
(
   byte nw_feat_supp
)
{
   msm_ext_cmd_type *msm_cmd;
   
   msm_cmd = msm_get_cmd_buf();
   
   if (msm_cmd == NULL)
   {
      MSG_FATAL_DS(MM_SUB, "=MM= ERROR: Unable to allocate memory", 0, 0, 0);
   }      

   msm_cmd->cmd_hdr.cmd_id  = MSM_MM_MBMS_SUPP_IND;
   
   if ((nw_feat_supp & GMM_MBMS_SUPPORTED) == GMM_MBMS_SUPPORTED)
   { 
      msm_cmd->cmd.mm_msm_supp_ind.mbms_support = TRUE;
   }
   else 
   {
      msm_cmd->cmd.mm_msm_supp_ind.mbms_support = FALSE;   
   } 
      
   msm_cmd->cmd.mm_msm_supp_ind.plmn_id[0] = mm_serving_plmn.info.plmn.identity[0];
   msm_cmd->cmd.mm_msm_supp_ind.plmn_id[1] = mm_serving_plmn.info.plmn.identity[1];
   msm_cmd->cmd.mm_msm_supp_ind.plmn_id[2] = mm_serving_plmn.info.plmn.identity[2];
      
   MSG_HIGH_DS_1(MM_SUB, "=MM= MM sent MSM_MM_MBMS_SUPP_IND. MBMS: %d", msm_cmd->cmd.mm_msm_supp_ind.mbms_support);
      
   msm_put_cmd(msm_cmd);  
} /* end gmm_send_mmr_nw_feature_support_ind() */
#endif

/*===========================================================================

FUNCTION GET_MS_NETWORK_CAPABILITY

DESCRIPTION
  This function constructs MS Network capability IE using the classmark
  information

DEPENDENCIES
  None

RETURN VALUE
  MS Net Caps IE structure of type gmm_ie_LV_ms_network_caps_T

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NAS_GW
gmm_ie_LV_ms_network_caps_T get_ms_network_capability( void )
{
  gmm_ie_LV_ms_network_caps_T net_caps;
  gprs_hw_classmark_T         classmark;
  csn_encoder_T               encoder;
  ue_rev_level_indicator_type ms_revision_level;

#if defined(FEATURE_LTE)
  
  sys_lte_band_mask_e_type lte_band_mask;

  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();


  ASSERT(emm_ctrl_data_ptr != NULL);

  lte_band_mask = SYS_LTE_BAND_MASK_CONST_NONE;

  
#endif

#ifdef FEATURE_DUAL_SIM
  nas_read_gprs_hw_classmark_per_subs(&classmark,(sys_modem_as_id_e_type)mm_sub_id);
#else
  nas_read_gprs_hw_classmark(&classmark);
#endif
  memset(net_caps.value, 0, 8);

  /* Init CSN encoder */
  csn_encoder_init(&encoder, net_caps.value, sizeof(net_caps.value));

  csn_encode_bit(&encoder, (byte) classmark.GEA_1_supported );
  csn_encode_bit(&encoder, (byte) sm_capability );
  csn_encode_bit(&encoder, (byte) sm_capability_gprs );
  csn_encode_bit(&encoder, (byte) ((ucs2_support_default_preferred == FALSE)?1 : 0) );

  /* SS Screening indicator (2 bit) */
  csn_encode_bits(&encoder, 2, (byte) ss_screening_indicator );

  ms_revision_level = get_revision_level_indicator();

  if (ms_revision_level == UE_REV_REL99)
  {
    csn_encode_bit(&encoder, (byte) solsa_supported );

    /* Revision level is set to 1 for Rel 99 and onwards, zero otherwise */
    csn_encode_bit(&encoder, (byte) ms_revision_level);

    csn_encode_bit(&encoder, (byte) PFC_feature_mode_supported );

    csn_encode_bit(&encoder, (byte) classmark.GEA_2_supported);
    csn_encode_bit(&encoder, (byte) classmark.GEA_3_supported);
    csn_encode_bit(&encoder, (byte) classmark.GEA_4_supported);
    csn_encode_bit(&encoder, (byte) classmark.GEA_5_supported);
    csn_encode_bit(&encoder, (byte) classmark.GEA_6_supported);
    csn_encode_bit(&encoder, (byte) classmark.GEA_7_supported);
    /* LCS VA capability bit */
    csn_encode_bit(&encoder, (byte) 0);
#ifndef FEATURE_LTE
#if defined(FEATURE_GPRS_PS_HANDOVER) && defined(FEATURE_NAS_GW)   
#if !defined (FEATURE_REL_6_SUPPORT)
#error FEATURE_GPRS_PS_HANDOVER requires FEATURE_REL_6_SUPPORT
#endif
   /* PS inter-RAT HO to UTRAN Iu mode capability */
   csn_encode_bit(&encoder, (byte) 0);
#endif
#else
   csn_encode_bit(&encoder, (byte) 0);
#endif /*FEATURE_LTE*/

#if defined(FEATURE_LTE)
    /* PS inter-RAT HO to E-UTRAN S1 mode capability*/
    csn_encode_bit(&encoder, (byte) 0);
  
  /* REL 11: EMM Combined Procedures bit should always be set as 1
     if device is LTE capable */
    lte_band_mask = rfm_get_lte_band_mask();
  
#if defined(FEATURE_LTE)    
#ifdef FEATURE_LTE_REL11
    if( ( ( (mm_sim_card_mode == MMGSDI_APP_USIM)
       && (SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_mask) == FALSE)
       && (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) ) ||
       (!((mm_sim_card_mode == MMGSDI_APP_USIM)
       && (SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_mask) == FALSE)
       && (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)) && 
       mm_checkif_rat_pri_list_has_lte_rat(&mm_rat_pri_list_info) == TRUE) )
#else
    if(mm_checkif_rat_pri_list_has_lte_rat(&mm_rat_pri_list_info)
#endif
#ifdef FEATURE_SGLTE
       && !MM_IS_IN_SGLTE_STATE_4
#endif
    )
    {
      csn_encode_bit(&encoder, (byte) emm_ctrl_data_ptr->emm_combined_proc_efs_flag); 
    }
    else
#endif
    { 
      csn_encode_bit(&encoder, (byte) 0); 
    }
  
  
    /* ISR support*/
    if(mm_checkif_rat_pri_list_has_lte_rat(&mm_rat_pri_list_info)
#ifdef FEATURE_SGLTE
       && !MM_IS_IN_SGLTE_STATE_4
#endif
    )
    {
      csn_encode_bit(&encoder, (byte) emm_ctrl_data_ptr->isr_efs_flag); 
    }
    else
    { 
      csn_encode_bit(&encoder, (byte) 0); 
    }

#ifdef FEATURE_LTE
    /* SRVCC to GERAN/UTRAN capability. Indicate support only is active RAT is LTE */
#ifdef FEATURE_SGLTE
    if(MM_IS_IN_SGLTE_MODE)
    {
      if((mm_serving_plmn.info.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
         (emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE))
      {
        csn_encode_bit(&encoder, (byte) 1); 
      }
      else
      {
        csn_encode_bit(&encoder, (byte) 0); 
      }
    }
    else
#endif
    if((mm_is_rat_lte(mm_serving_plmn.info.active_rat)) &&
       (emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE))
    {
      csn_encode_bit(&encoder, (byte) 1); 
    }
    else
#endif
    {
      csn_encode_bit(&encoder, (byte) 0); 
    }
  
    /* EPC capability */
    /* Supported if UE supports LTE and bit should be set to 0 if SIM is inserted and UE supports LTE */
    if ((mm_sim_card_mode == MMGSDI_APP_USIM)
       && (SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_mask) == FALSE))
    {
      /* EPC capability */
      csn_encode_bit(&encoder, (byte) 1);
    }
    else 
    {
      /* EPC capability */
      csn_encode_bit(&encoder, (byte) 0);
    }

#ifdef FEATURE_LTE
    /* NF capability. Indicated as supported only if active RAT is LTE */
#ifdef FEATURE_SGLTE
    if(MM_IS_IN_SGLTE_MODE)
    {
      if((mm_serving_plmn.info.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
         (emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE))
      {
        csn_encode_bit(&encoder, (byte) 1); 
      }
      else
      {
        csn_encode_bit(&encoder, (byte) 0); 
      }
    }
    else
#endif
    if((mm_is_rat_lte(mm_serving_plmn.info.active_rat)) &&
       (emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE))
    {
      csn_encode_bit(&encoder, (byte) 1); 
    }
    else
#endif
    {
      csn_encode_bit(&encoder, (byte) 0); 
    }
#endif
  }

  /* Fill the rest of the byte with spare bits of 0's */
  csn_fill_spare_bits(&encoder);
  /* The bit_pos must be byte aligned as spares are filled */
  if( csn_bit_pos(&encoder) % 8 == 0)
  {
    net_caps.length = (byte)csn_bit_pos(&encoder) / 8;
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=MM= Error encoding RAT");
    net_caps.length = 0;
  }

  return net_caps;
} /* end get_ms_network_capability( void ) */
#endif
#if defined(FEATURE_LTE)
#ifdef FEATURE_NAS_SM_IRAT

/*===========================================================================

FUNCTION GMM_SEND_SM_ISR_STATUS_IND

DESCRIPTION
       Sends GMM_SEND_SM_ISR_STATUS_IND to SM. The content of the message is taken from
       isr_status passed as input
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void gmm_send_sm_isr_status_ind(boolean isr_status)
{
  sm_cmd_type  *sm_cmd;
  sm_cmd       = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set     = MS_GMM_SM;
    sm_cmd->header.cmd_id          = GMMSM_ISR_STATUS_IND;

    sm_cmd->cmd.gmm_isr_status_ind.isr_status = isr_status;
#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmm_isr_status_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent GMMSM_ISR_STATUS_IND");
    //sm_put_cmd (sm_cmd);
    sm_process_isr_status_ind(sm_cmd->cmd.gmm_isr_status_ind);
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
}
#endif
#endif

/*===========================================================================

FUNCTION GMM_READ_GPRS_MULTISLOT_CLASS

DESCRIPTION
       returns gmm_multislot_class value.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
byte gmm_read_gprs_multislot_class(void)
{
  return GPRS_MULTISLOT_CLASS;
}


/*===========================================================================

FUNCTION GMM_READ_EDGE_MULTISLOT_CLASS

DESCRIPTION
       returns gmm_edge_multislot_class value.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
byte gmm_read_edge_multislot_class(void)
{
  return GPRS_EDGE_MULTISLOT_CLASS;
}
#ifdef FEATURE_ENHANCED_NW_SELECTION
/*===========================================================================

FUNCTION GMM_IS_RAI_IN_FOREGROUND_SEARCH_LAI_RAI_TIMER_LIST

DESCRIPTION
  This function checks whether serving RAI is in  foreground search lai timer list or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean gmm_is_rai_in_foreground_search_lai_rai_timer_list(byte* lai,byte rac)
{
  boolean result = FALSE;
  byte  i;
  for (i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
  {
    if ( (memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,lai,LAI_SIZE) ==0) &&
          (mm_foreground_search_lai_timer_list.lai_timer_list[i].rac == rac) &&
          (mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter >= 5))
    {
      result = TRUE;
      MSG_HIGH_DS_0(MM_SUB, "=MM= RAI is in fore ground search lai list");
      break;
    }
  }
  return result;
}
/*===========================================================================

FUNCTION GMM_ADD_RAC_TO_FOREGROUND_SEARCH_LAI_RAI_TIMER_LIST

DESCRIPTION
  This function adds the serving RAC to foreground search lai timer list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_add_rac_to_foreground_search_lai_rai_timer_list(byte* lai,byte rac)
{
   byte  i;
   
   for (i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
   {
    if ( (memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,lai,LAI_SIZE) ==0))
    {

      if(mm_foreground_search_lai_timer_list.lai_timer_list[i].rac != rac)
      {
        mm_foreground_search_lai_timer_list.lai_timer_list[i].rac = rac;
          
        MSG_HIGH_DS_2(MM_SUB, "=MM= Serving RAC  %d is added to FOREGROUND SEARCH LAI RAI TIMER LIST.length  %d", 
                      rac, mm_foreground_search_lai_timer_list.length);
      }

      if(gmm_state == GMM_REGISTERED_INITIATED )
      {
        mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter = gprs_attach_attempt_counter;
      }
      else if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED )
      {
        mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter = gmm_rau_attempt_counter;
      }

      MSG_HIGH_DS_2(MM_SUB, "=MM= Serving RAC  %d  updated with global PS attempt counter %d", 
                    rac, mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter);
      break;
    }
   }

}



/*===========================================================================

FUNCTION GMM_UPDATE_PS_ATTEMPT_COUNTER_FROM_FOREGROUND_SEARCH_LAI_RAI_TIMER_LIST

DESCRIPTION
  This function gets the sstored PS attempt counter value of serving RAC present in  foreground search lai timer list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void gmm_update_ps_attempt_counter_from_foreground_search_lai_rai_timer_list(byte* lai,byte rac)
{
   byte  i;
   
   for (i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
   {
    if ( (memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,lai,LAI_SIZE) ==0) &&
          (mm_foreground_search_lai_timer_list.lai_timer_list[i].rac == rac))
    {
      if(gmm_state == GMM_REGISTERED_INITIATED )
      {
        gprs_attach_attempt_counter = mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter;
      }
      else if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED )
      {
        gmm_rau_attempt_counter     = mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter;
      }

      MSG_HIGH_DS_2(MM_SUB, "=MM= Serving RAC  %d is updated with stored PS attempt counter %d", 
                    rac, mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter);
      break;
    }
   }

}


/*===========================================================================

FUNCTION GMM_GET_FOREGROUND_SEARCH_LAI_TIMER_EXPIRY_RAI

DESCRIPTION
  This function gives the RAI whose foreground search timer got expired

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_get_foreground_timer_expiry_rai(byte *lai,byte* rac)
{
  uint32 i = 0;

  while ( (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH))
  {
    if ((mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count == 0) &&
        (gmm_is_rai_in_foreground_search_lai_rai_timer_list(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,
                                                                                        mm_foreground_search_lai_timer_list.lai_timer_list[i].rac)))
    {
        memscpy(lai, LAI_SIZE,mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,LAI_SIZE);
        *rac = mm_foreground_search_lai_timer_list.lai_timer_list[i].rac;
        break;
    }
    else
    {
      i++;
    }
  }
}
/*===========================================================================

FUNCTION GMM_INVALIDATE_FOREGROUND_SEARCH_RAI

DESCRIPTION
  This function removes the RAC from foreground search LAI list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_invalidate_foreground_search_rai(byte * serving_lai,byte  rac)
{
  uint32 i = 0;

  while ( (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH))
  {
    if ( (memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,serving_lai,LAI_SIZE) ==0) &&
          (mm_foreground_search_lai_timer_list.lai_timer_list[i].rac == rac))
    {
      mm_foreground_search_lai_timer_list.lai_timer_list[i].rac = 0xFF;
      mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter = 0;
      break;
    }
    else
    {
      i++;
    }
  }
}

#endif
boolean gmm_check_gprs_forbidden_rai(PLMN_id_T plmn_id,location_area_code_T lac,routing_area_code_T rac)
{
  boolean result = FALSE;
  if(!((gmm_plmn_changed( gmm_reg_rejected_rai.plmn_id, plmn_id ) )||
                        (gmm_rai_changed( gmm_reg_rejected_rai.location_area_code,
                         gmm_reg_rejected_rai.routing_area_code,lac,rac ))))
  {
    result = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=MM= Servng RAI is forbiden for GPRS services");
  }
  return result;
}
void gmm_update_gprs_forbidden_rai(routing_area_id_T rai)
{
  memscpy(&gmm_reg_rejected_rai, sizeof(routing_area_id_T),&rai, sizeof(routing_area_id_T));
  MSG_HIGH_DS_0(MM_SUB, "=MM= Servng RAI is added to GPRS forbiden RAI");
}
void gmm_delete_gprs_forbidden_rai(void)
{
  MSG_HIGH_DS_0(MM_SUB, "=MM= GPRS forbiden RAI is deleted");
  gmm_reg_rejected_rai.plmn_id.identity[0]       = 0xFF;
  gmm_reg_rejected_rai.plmn_id.identity[1]       = 0xFF;
  gmm_reg_rejected_rai.plmn_id.identity[2]       = 0xFF;
  gmm_reg_rejected_rai.location_area_code.lac[0] = 0xFF;
  gmm_reg_rejected_rai.location_area_code.lac[1] = 0xFF;
  gmm_reg_rejected_rai.routing_area_code = 0xFF;
}
/*===========================================================================

FUNCTION GMM_UPDATE_CELL_STATUS

DESCRIPTION
  This function set both cs and PS status as barred if RRC connection was aborted with cause
  access class not allowed 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_update_cell_status(rrc_abort_cause_e_type abort_cause)
{
  
  if((mm_system_information.ppac_present == TRUE) && is_umts_mode())
  {
	  if(abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN)
	  {
	    mm_system_information.ps_reg_allowed = FALSE;
	  }
	  MSG_HIGH_DS_1(MM_SUB, "=MM= Set PS Cell access : %d ", mm_system_information.ps_reg_allowed);
  }
#ifdef FEATURE_DSAC
  else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
  {
	  if(abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN)
	  {
	    mm_system_information.ps_cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
	  }
	  MSG_HIGH_DS_1(MM_SUB, "=MM= Set PS Cell access : %d ", mm_system_information.ps_reg_allowed);
                                                    
  }
  else
  {
      mm_system_information.cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
  }
#else
  else
  {
      mm_system_information.cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
  }
#endif 
}

#ifdef FEATURE_NAS_REL10
/*===========================================================================
FUNCTION      GMM_IS_PLMN_EQUIVALENT_T3346

DESCRIPTION
 it would check if plmn is same as t3346 or is in eplmn list
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If plmn is t3346 or in eplmn list
  FALSE  -  If plmn is a new plmn and not in eplmn list
 
SIDE EFFECTS
  None
===========================================================================*/
boolean gmm_is_plmn_equivalent_t3346(void)
{
  sys_plmn_id_s_type   last_t3346_plmn;
 
  memscpy(&last_t3346_plmn, sizeof(sys_plmn_id_s_type), &gmm_3gpp_t3346_plmn_id, 
                                sizeof(sys_plmn_id_s_type));

  /* if last attempted plmn is same as previous plmn or is equivalent
     do not attempt registration */
  if((!sys_plmn_id_is_undefined(last_t3346_plmn)) &&
     (!sys_plmn_id_is_undefined(mm_serving_plmn.info.plmn)) &&
     ((sys_plmn_match(mm_serving_plmn.info.plmn, last_t3346_plmn) == TRUE)
      ||
      ((sys_eplmn_list_equivalent_plmn(last_t3346_plmn)   == TRUE) &&
       (sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn) == TRUE))))
  {
    return TRUE;
  }

  return FALSE;
}

#endif

void gmm_sm_send_gprs_rel_ind_if_needed(void)
{
  if(!(gmm_state == GMM_DEREGISTERED ||
       gmm_state == GMM_REGISTERED_INITIATED ||
       gmm_state == GMM_NULL))
  {
    gmm_process_gprs_release_ind(NO_GPRS_CONTEXT);
  }
}
/*===========================================================================
FUNCTION      gmm_send_reset_ind_to_ds
DESCRIPTION
 Function to send reset indication(for throttling timer) to ds in following situation
 1. RAU succeeds
 2. IRAT 
 3. power cycle.
 
DEPENDENCIES
  None 
RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void gmm_send_reset_ind_to_ds(ds_3gpp_notify_ind_e_type ind_type, ds_3gpp_rau_event_e_type event , ds_3gpp_rau_type rau_type)    
{
    ds_3gpp_rau_event_s_type* notify_ind_data_ptr;

#ifndef FEATURE_MODEM_HEAP
    notify_ind_data_ptr = (ds_3gpp_rau_event_s_type*)gs_alloc(sizeof(ds_3gpp_rau_event_s_type));
#else
    notify_ind_data_ptr = (ds_3gpp_rau_event_s_type*)modem_mem_calloc(1, sizeof(ds_3gpp_rau_event_s_type), MODEM_MEM_CLIENT_NAS);
#endif
    mm_check_for_null_ptr((void*)notify_ind_data_ptr);

    if(NULL == notify_ind_data_ptr)
    {
      return; 
    }

    notify_ind_data_ptr->event    = event;
#if defined(FEATURE_WCDMA) && defined(FEATURE_NAS_GW) 
    if ( is_wcdma_mode() )
    {
#ifdef FEATURE_SEGMENT_LOADING
       interface_t* ptr_W = get_wcdma_interface();
       if(ptr_W == NULL)
       {
         MSG_HIGH_DS_0(MM_SUB, "=MM= ERROR: Wrong segment (W)So not sending reset ind for throttling timer");
         return; 
       }
       notify_ind_data_ptr->sys_mode = SYS_SYS_MODE_WCDMA;
#else
       notify_ind_data_ptr->sys_mode = SYS_SYS_MODE_WCDMA;
#endif
    }
#endif
#ifdef FEATURE_TDSCDMA
    if ( is_tdscdma_mode() )
    {
#ifdef FEATURE_SEGMENT_LOADING
       interface_t* ptr_T = get_tdscdma_interface();
       if(ptr_T == NULL)
       {
         MSG_HIGH_DS_0(MM_SUB, "=MM= ERROR: Wrong segment (T) So not sending reset ind for throttling timer");
         return;
       }
       notify_ind_data_ptr->sys_mode = SYS_SYS_MODE_TDS;
#else
       notify_ind_data_ptr->sys_mode = SYS_SYS_MODE_TDS;
#endif
    }
#endif
    if(is_gsm_mode())
    {
      notify_ind_data_ptr->sys_mode = SYS_SYS_MODE_GSM;
    }
#ifdef FEATURE_DUAL_SIM
    notify_ind_data_ptr->subs_id  = (sys_modem_as_id_e_type) mm_sub_id;
#else
    notify_ind_data_ptr->subs_id  = SYS_MODEM_AS_ID_1;
#endif
    notify_ind_data_ptr->rau_type = rau_type;
    MSG_HIGH_DS_2(MM_SUB, "=MM= NAS sending reset indication for throttling timer to DS, event = %d(0 for RAU completion) type = %d", event,rau_type); 
    ds_3gpp_notify_ind(ind_type, (void *) notify_ind_data_ptr);

#ifndef FEATURE_MODEM_HEAP
    gs_free(notify_ind_data_ptr);
#else
    modem_mem_free(notify_ind_data_ptr, MODEM_MEM_CLIENT_NAS);
#endif
    notify_ind_data_ptr = NULL;

}
/*===========================================================================
FUNCTION      gmm_max_L3_msg_length

DESCRIPTION
 Function to decide the maximum size of L3 message
 
DEPENDENCIES
  None 

RETURN VALUE
  max l3 message length
 
SIDE EFFECTS
  None
===========================================================================*/
uint16 gmm_max_L3_msg_length( mm_cn_domain_type_T cn_domain )
{
   uint16 L3_message_length = MAX_OCTETS_L3_MESSAGE;
   
   if (cn_domain == MM_CS_DOMAIN)
   {
     L3_message_length = MAX_OCTETS_L3_MESSAGE;
   }
   else if(mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) 
   {
     L3_message_length = MAX_OCTETS_L3_MESSAGE_PS_WRAT; 
   }
   else if(mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) 
   {
     if(gmm_xid_negotiation_completed)
     {
       L3_message_length = gmm_max_L3_length;
     }
     else
     {
       L3_message_length = MAX_OCTETS_L3_MESSAGE_PS_GRAT;
     }
   } 
   return L3_message_length;
 }

#ifdef FEATURE_SGLTE
void gmm_sglte_reset_ps_attempt_counters(void)
{
  mm_stop_timer(TIMER_T3302);
  mm_stop_timer(TIMER_T3310);
  mm_stop_timer(TIMER_T3311);
  mm_stop_timer(TIMER_T3330);
  mm_stop_timer(TIMER_T3302);
  gmm_rau_attempt_counter = 0;
  gprs_attach_attempt_counter = 0;
  t3330_timeout_counter = 0;
  t3310_timeout_counter = 0;
}

#endif

  /*===========================================================================
  
  FUNCTION: gmm_ps_registration_vote_for_reselection()
  
  DESCRIPTION:    Vote No RESEL if RAU/ATTACH is on going or PAGE is being responded
  
  DEPENDENCIES:
  
  PARAMETERS: as_id
  
  RETURN VALUE:
  
  SIDE EFFECTS:
  None
  ===========================================================================*/
  
nas_vote_for_reselection_e_type gmm_ps_registration_vote_for_reselection(sys_modem_as_id_e_type as_id)
{
 
  nas_vote_for_reselection_e_type ret_val  = NAS_VOTE_FOR_RESEL_TRUE;
 
#if defined(FEATURE_DUAL_SIM)
 
  MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_cs_registration_vote_for_reselection - Recd as_id = %d", as_id);
  if (IS_NOT_VALID_AS_ID(as_id))
  {
    ret_val = NAS_VOTE_FOR_RESEL_TRUE;
  }
  else if((gmm_state_sim[as_id] == GMM_REGISTERED_INITIATED ||
     gmm_state_sim[as_id] == GMM_ROUTING_AREA_UPDATING_INITIATED))
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_ps_registration_vote_for_reselection votes %d, RAU/ATTACH in progress", NAS_VOTE_FOR_RESEL_FALSE);
    ret_val = NAS_VOTE_FOR_RESEL_FALSE;
  }
  if(gmm_controls_resel_dec_mt_ps_call_sim[as_id] == TRUE)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_ps_registration_vote_for_reselection votes %d, PS Page being responded", NAS_VOTE_FOR_RESEL_FALSE);
    ret_val = NAS_VOTE_FOR_RESEL_FALSE;
  }
 
#else
 
  if( (gmm_state == GMM_REGISTERED_INITIATED ||
     gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) )
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_ps_registration_vote_for_reselection votes %d, RAU/ATTACH in progress", NAS_VOTE_FOR_RESEL_FALSE);
    ret_val = NAS_VOTE_FOR_RESEL_FALSE;
  }
  if(gmm_controls_resel_dec_mt_ps_call == TRUE)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_ps_registration_vote_for_reselection votes %d, PS Page being responded", NAS_VOTE_FOR_RESEL_FALSE);
    ret_val = NAS_VOTE_FOR_RESEL_FALSE;
  }
#endif

  MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_ps_registration_vote_for_reselection returns %d", ret_val);
  return ret_val;

 }
  
/*===========================================================================
FUNCTION gmm_send_nv_params_to_rr

DESCRIPTION
  This function sends NV params to RR

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/
void gmm_send_nv_params_to_rr(void)
{
  rr_gmm_gprs_nv_params_ind_t nv_parm_msg;

  nv_parm_msg.message_header.message_set = MS_MM_RR;
  nv_parm_msg.message_header.message_id = (int) RR_GMM_GPRS_NV_PARAMS_IND;
  PUT_IMH_LEN( sizeof( rr_gmm_gprs_nv_params_ind_t ) - sizeof(IMH_T),
                 &nv_parm_msg.message_header );

  nv_parm_msg.nv_disable_call_type = (uint16)gmm_nv_disable_call_type;

  nv_parm_msg.nv_multislot_class    = gmm_multislot_class;
  nv_parm_msg.nv_non_drx_timer      = gmm_non_drx_timer;
  nv_parm_msg.nv_split_paging_cycle = gmm_split_paging_cycle;
  nv_parm_msg.nv_anite_gcf_flag     = gmm_anite_gcf_flag;
  nv_parm_msg.nv_gea_supported      = gmm_gea_support;

#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_NAS_GW)
  nv_parm_msg.nv_edge_feature_support = gmm_edge_feature_support;

  /* Send EDGE feature support info to RR only if EDGE is turned ON */
  if(gmm_edge_feature_support)
  {
    nv_parm_msg.nv_edge_multislot_class = gmm_edge_multislot_class;
    nv_parm_msg.nv_edge_8psk_power_class = gmm_edge_8psk_power_class;
    nv_parm_msg.nv_edge_8psk_power_capability = gmm_edge_8psk_power_capability;
  }
#endif /* FEATURE_GSM_EGPRS */

#ifdef FEATURE_REL_4_SUPPORT
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
  nv_parm_msg.nv_edge_nw_ass_cell_change = gmm_edge_nw_ass_cell_change;
  nv_parm_msg.nv_interrat_nacc_support = gmm_interrat_nacc_support;
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */
#endif /* FEATURE_REL_4_SUPPORT */

#ifdef FEATURE_GSM_DTM
  nv_parm_msg.nv_dtm_feature_support    = gmm_nv_dtm_feature_support;
  nv_parm_msg.nv_dtm_multislot_subclass = gmm_nv_dtm_multislot_subclass;
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_EDTM
  nv_parm_msg.nv_edtm_feature_support = gmm_nv_edtm_feature_support;
#endif /* FEATURE_GSM_EDTM */

#if defined(FEATURE_GSM_GPRS_MSC34)
  nv_parm_msg.nv_hmsc_gprs_coded_ms_class  = (byte) gmm_set_multislot_class (gmm_multislot_class);
#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_NAS_GW)
  nv_parm_msg.nv_hmsc_egprs_coded_ms_class = (byte) gmm_set_multislot_class (gmm_edge_multislot_class);
#endif
#endif /* FEATURE_HMSC34*/

  mm_send_message( &nv_parm_msg.message_header, GS_QUEUE_RR );
}

/*===========================================================================
FUNCTION  gmm_read_carrier_specific_efs

DESCRIPTION
  This function reads carrier specific EFS/NV values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_read_carrier_specific_efs(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

  ghdi_status_T nv_status;

  /* NV_DISABLE_CM_CALL_TYPE_I */
  nv_status  = mm_ghdi_nvmem_read(NV_DISABLE_CM_CALL_TYPE_I,
                        (ghdi_nvmem_data_T *)&gmm_nv_disable_call_type);
  if(nv_status == GHDI_SUCCESS)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Read NV_DISABLE_CM_CALL_TYPE_I value as %d", gmm_nv_disable_call_type);
  }
  else
  {
    gmm_nv_disable_call_type = 0;
    MSG_ERROR_DS_2(MM_SUB, "=MM= Unable to read NV_DISABLE_CM_CALL_TYPE_I: %d, default %d",
                   nv_status, gmm_nv_disable_call_type);
  }

#ifdef FEATURE_GSM_DTM
  /* NV_DTM_FEATURE_SUPPORT_I */
  nv_status = mm_ghdi_nvmem_read( NV_DTM_FEATURE_SUPPORT_I, &gmm_nv_data_buffer);

  if(nv_status != GHDI_SUCCESS)
  {
    gmm_nv_dtm_feature_support = GPRS_EDGE_DTM_SUPPORTED;  /* Set the default value */
    MSG_ERROR_DS_2(MM_SUB, "=MM= Unable to read NV for DTM Feature Support flag: %d, default %d",
                   nv_status, gmm_nv_dtm_feature_support);
  }
  else
  {
    gmm_nv_dtm_feature_support = gmm_nv_data_buffer.dtm_feature_support;

    if((gmm_nv_dtm_feature_support != FALSE) && (gmm_nv_dtm_feature_support != TRUE))
    {
      MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for DTM Feature Support flag %d. Setting to GPRS_EDGE_DTM_SUPPORTED",
                    gmm_nv_dtm_feature_support);
      gmm_nv_dtm_feature_support = GPRS_EDGE_DTM_SUPPORTED;  /* Set the default value */
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB, "=MM= Read DTM Feature Support flag as %d", gmm_nv_dtm_feature_support);
    }
  }

  /* NV_DTM_MULTISLOT_CLASS_I */
  nv_status = mm_ghdi_nvmem_read(NV_DTM_MULTISLOT_CLASS_I, &gmm_nv_data_buffer );

  if(nv_status != GHDI_SUCCESS)
  {
    gmm_nv_dtm_multislot_subclass = GPRS_EDGE_DTM_MULTISLOT_SUBCLASS;  /* Set the default value */
    MSG_ERROR_DS_2(MM_SUB, "=MM= Unable to read NV for DTM Multislot Sub Class: %d, default %d",
                   nv_status, gmm_nv_dtm_multislot_subclass);
  }
  else
  {
    gmm_nv_dtm_multislot_subclass = gmm_nv_data_buffer.dtm_multislot_class;

    if (gmm_nv_dtm_multislot_subclass > 3)
    {
      MSG_HIGH_DS_1(MM_SUB, "=MM= Invalid value for DTM Multislot Sub Class %d. Setting to GPRS_EDGE_DTM_MULTISLOT_SUBCLASS",
                    gmm_nv_dtm_multislot_subclass);
      gmm_nv_dtm_multislot_subclass = GPRS_EDGE_DTM_MULTISLOT_SUBCLASS;  /* Set the default value */
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB, "=MM= Read DTM Multislot Sub Class NV as %d", gmm_nv_dtm_multislot_subclass);
    }
  }
#endif
}
 
#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION gmm_process_mcfg_refresh

DESCRIPTION
  This function processes MCFG NV refresh indication for GMM specific EFS/NVs

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/
void gmm_process_mcfg_refresh(void)
{
  gmm_read_carrier_specific_efs();
  gmm_send_nv_params_to_rr();
  MSG_HIGH_DS_0(MM_SUB, "=MM= GMM Refresh Done");
}
#endif

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION gmm_get_cm_ds_tuneaway_status

DESCRIPTION
  This function gets the tuneaway status received in CM_DS_TUNEAWAY_STATUS_CHANGE_IND

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean gmm_get_cm_ds_tuneaway_status()
{
    MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_get_cm_ds_tuneaway_status %d", gmm_cm_ds_tuneaway_status);
    return gmm_cm_ds_tuneaway_status;
}
#endif /*FEATURE_DUAL_SIM*/

/*===========================================================================
FUNCTION  gmm_process_receive_psm_timers

DESCRIPTION: This function updates the value of t3324 received from NW
             and sends psm timer change indication to upper layers if
             needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean gmm_process_receive_psm_timers
(
  boolean             t3324_val_exists,
  byte                t3324_value
)
{
  boolean psm_timers_changed = FALSE;
  uint32 t3324_old_value;

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

  if (edrx_gsm_fake_negotiation)
  {
     /* With fake PSM Rel12 NV enabled, PSM negotiation will always be successful in NAS irrespective of
            testbox return value and NAS will inform the active value configured in below NV to lower layer 
            after every successful registration.*/
     /* t3324_value = gmm_psm_timer_ctxt.t3324_value_to_nw; */
     t3324_val_exists = TRUE;
  }

  
  MSG_HIGH_DS_3(MM_SUB, "=MM= PSM t3324_val_exists = %d New t3324_value = %d Old t3324_value = %d", t3324_val_exists, t3324_value, gmm_psm_timer_ctxt.t3324_value);

  t3324_old_value = gmm_psm_timer_ctxt.t3324_value;
  gmm_psm_timer_ctxt.t3324_value = NAS_INVALID_PSM_TIMER_VALUE;

  if(gmm_psm_timer_ctxt.t3324_value_to_nw != NAS_INVALID_PSM_TIMER_VALUE)
  {
    if(t3324_val_exists == FALSE)
    {
      /* NW doesn't support R12 PSM, send PSM timer change indication */
      psm_timers_changed = TRUE;
    }
    else
    {
      gmm_psm_timer_ctxt.t3324_value = gmm_convert_timer_value_ie_to_usecs(t3324_value);
      if (edrx_gsm_fake_negotiation)
      {
         gmm_psm_timer_ctxt.t3324_value = gmm_psm_timer_ctxt.t3324_value_to_nw;
         gmm_t3312_timer_value = gmm_psm_timer_ctxt.t3312_ext_value_to_nw;
      }

      if((t3324_old_value == NAS_INVALID_PSM_TIMER_VALUE) ||
         (gmm_psm_timer_ctxt.t3324_value_to_nw != gmm_psm_timer_ctxt.t3324_value) ||
         (t3324_old_value != gmm_psm_timer_ctxt.t3324_value) ||
         (gmm_psm_timer_ctxt.t3312_ext_value_to_nw != gmm_t3312_timer_value)
#ifdef FEATURE_LTE
         ||
         (emm_ctrl_data_ptr->t3324_value != gmm_psm_timer_ctxt.t3324_value)
#endif

        )
      {
        /* PSM previously inactive, to_nw values different from from_nw for t3324 or t3312 or both
                 send PSM timer change indication */
        psm_timers_changed = TRUE;
      }
    }
  }

  if (psm_timers_changed == TRUE)
  {
    mm_send_mmr_psm_timer_to_clients_ind(gmm_psm_timer_ctxt.t3324_value,
                                      glif_ready_timer_value,
                                      gmm_t3312_timer_value);
  }
  return psm_timers_changed;
}

 
/*===========================================================================
FUNCTION  gmm_invalidate_psm_params

DESCRIPTION
  This function invalidates PSM related parameters and updates upper layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_invalidate_psm_params (void)
{

  /* Send psm timer change indication to upper layers since the negotiation failed.
     This indication will let upper layers know that R12-PSM is currently not supported at NAS */
 
  if ((mm_psm_r12_enabled == TRUE) &&
      (gmm_psm_timer_ctxt.t3324_value_to_nw != NAS_INVALID_PSM_TIMER_VALUE))
  {
      gmm_psm_timer_ctxt.t3324_value = NAS_INVALID_PSM_TIMER_VALUE;
      mm_send_mmr_psm_timer_to_clients_ind(gmm_psm_timer_ctxt.t3324_value,
                                       glif_ready_timer_value,
                                       gmm_t3312_timer_value);
  }
}


#ifdef FEATURE_NAS_GW

/*===========================================================================
FUNCTION  gmm_save_psm_context

DESCRIPTION: This function saves PSM context upon entering PSM

DEPENDENCIES
  None

RETURN VALUE
  TRUE: PSM context saved successfully, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean gmm_save_psm_context(void)
{
  boolean result = TRUE;

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

  if (mm_psm_cache_ptr == NULL)
  {
    mm_psm_cache_ptr = (mm_psm_cache_s_type *)modem_mem_alloc(sizeof(mm_psm_cache_s_type), MODEM_MEM_CLIENT_NAS);
    mm_check_for_null_ptr((void*)mm_psm_cache_ptr);
    memset((void*)mm_psm_cache_ptr, 0, sizeof(mm_psm_cache_s_type));
  }

  mm_psm_cache_ptr->rat = SYS_RAT_GSM_RADIO_ACCESS;
  mm_psm_cache_ptr->plmn = mm_serving_plmn.info.plmn;
  mm_psm_cache_ptr->sim_state = mm_serving_plmn.info.sim_state;
  mm_psm_cache_ptr->reg_service_domain = mm_serving_plmn.info.reg_service_domain;
  mm_psm_cache_ptr->t3324_value = gmm_psm_timer_ctxt.t3324_value;
  mm_psm_cache_ptr->timers_timestamp = time_get_ms_native();
  mm_psm_cache_ptr->gsm.plmn_service_capability = mm_serving_plmn.info.plmn_service_capability;
  mm_psm_cache_ptr->gsm.gmm_stored_rai = gmm_stored_rai;
  mm_psm_cache_ptr->gsm.last_attempted_rai = gmm_previous_registration_serving_rai;
  mm_psm_cache_ptr->gsm.rat_pri_list_info_to_nw   = mm_rat_pri_list_info_to_nw;
  mm_psm_cache_ptr->gsm.rat_pri_list_info         = mm_rat_pri_list_info;
  mm_psm_cache_ptr->gsm.is_lte_advertised_in_last_reg   = gmm_is_lte_advertised_in_last_reg;
  mm_psm_cache_ptr->gsm.lte_cs_domain_param   = gmm_reg_lte_cs_param;
 
  mm_psm_cache_ptr->gsm.gmm_previous_T3311_gmm_substate = (byte)gmm_previous_T3311_gmm_substate;
  mm_psm_cache_ptr->gsm.gmm_ra_update_type = (byte)gmm_ra_update_type;
  mm_psm_cache_ptr->gsm.is_ptmsi_present_in_prev_gmm_msg = gmm_is_ptmsi_present_in_prev_gmm_msg;
  
  mm_psm_cache_ptr->gsm.t3312_value = gmm_t3312_timer_value;
  mm_psm_cache_ptr->gsm.t3302_value = gmm_t3302_timer_value;
  mm_psm_cache_ptr->gsm.t3323_value = gmm_t3323_timer_value;
  mm_psm_cache_ptr->gsm.t3319_value = gmm_t3319_timer_value;
  mm_psm_cache_ptr->gsm.t3312_timer = (uint64)gs_enquire_timer_ext(get_mm_timer_queue(),
                                                                             TIMER_T3312);
  mm_psm_cache_ptr->gsm.gprs_ready_timer_value = glif_ready_timer_value;
  mm_psm_cache_ptr->gsm.gprs_ready_timer_deactivated = glif_get_ready_timer_deactivated();
  mm_psm_cache_ptr->gsm.gmm_nw_feature_support = gmm_stored_nw_feature_support;
  mm_psm_cache_ptr->gsm.gmm_drx_cn_coeff_s1 = gmm_drx_cn_coeff_s1;
  mm_psm_cache_ptr->gsm.edrx_params = gmm_edrx_params_gsm_ctxt.edrx_params;
  
  mm_psm_cache_ptr->gsm.ftai_clear_timer = (uint32)gs_enquire_timer_ext(get_mm_timer_queue(),
                                                                        TIMER_MM_FORBIDDEN_LAI_CLEAR);
  if (mm_psm_cache_ptr->gsm.ftai_clear_timer != 0)
  {
    mm_per_subs_store_forbidden_lai_list(mm_psm_cache_ptr->gsm.forbidden_lai_list);
    mm_per_subs_store_lai_reject_list(&mm_psm_cache_ptr->gsm.manual_forbidden_lai_reject_list);
  }

  mm_psm_cache_ptr->gsm.gprs_rejected_rai = gmm_reg_rejected_rai;
  mm_psm_cache_ptr->gsm.rau_attempt_counter = gmm_rau_attempt_counter;
  mm_psm_cache_ptr->gsm.ra_update_pending = gmm_ra_update_pending;
  mm_psm_cache_ptr->gsm.multi_slot_ra_update_pending = gmm_multi_slot_ra_update_pending;
  mm_psm_cache_ptr->gsm.pended_ra_update_type = (byte)gmm_pending_ra_update_type;
  

  mm_psm_cache_ptr->gsm.multi_slot_class = gmm_multislot_class;
  mm_psm_cache_ptr->gsm.edge_multi_slot_class = gmm_edge_multislot_class;

#ifdef FEATURE_GSM_DTM
  mm_psm_cache_ptr->gsm.dtm_multi_slot_subclass = gmm_nv_dtm_multislot_subclass;
#endif

  mm_psm_cache_ptr->gsm.xid_negotiation_completed = gmm_xid_negotiation_completed;
  mm_psm_cache_ptr->gsm.gmm_n201_u = gmm_max_L3_length;

#ifdef FEATURE_DSAC
  mm_psm_cache_ptr->gsm.last_procedure_to_update_registration = mm_last_procedure_to_update_registration;
#endif

#ifdef FEATURE_CIOT
  mm_psm_cache_ptr->gsm.attached_lte_rat = emm_ctrl_data_ptr->attached_lte_rat;
#endif

#if defined(FEATURE_NAS_REL10) || defined(FEATURE_LTE_REL10)
  if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
  {
    result = (result && mm_write_backoff_remaining_info_to_efs()) ? TRUE : FALSE;
#ifdef FEATURE_NAS_GW
    gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
  }
#endif

  MSG_HIGH_DS_1(MM_SUB, "=MM= PSM Context Saved Succesfully Status = %d", result);

  return result;
}
  

/*===========================================================================
FUNCTION  GMM_PROCESS_SAVE_CTXT_PSM_CNF

DESCRIPTION
  This function processes NAS_GMM_SAVE_CTXT_PSM_CNF  from SM received after PDP context saving
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_process_save_ctxt_psm_cnf
(
  boolean             status
)
{
  boolean result = FALSE;

  /* Save GMM context only if GMM state is Registered Normal sevice */
  if((gmm_state == GMM_REGISTERED) &&
     (gmm_substate == GMM_NORMAL_SERVICE))
  {
    result = gmm_save_psm_context();
  }

  mm_send_mmr_enter_psm_mode_rsp(result);
  if(result == TRUE)
  {
    mm_psm_mode_req_received = TRUE;
  } 
} /* end gmm_process_save_ctxt_psm_cnf() */


/*===========================================================================
FUNCTION  GMM_PROCESS_RESTORE_CTXT_PSM_CNF

DESCRIPTION
  This function processes GMMSM_RESTORE_CTXT_PSM_CNF  from SM and sends SIM available CNF to CM after PSM restore.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  gmm_process_restore_ctxt_psm_cnf
(
  boolean             status
)
{
  boolean psm_restore_done = FALSE;
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

  MSG_HIGH_DS_3(MM_SUB, "=MM= PSM restore = %d gmm_state = %d gmm_substate = %d", mm_psm_restore_pending, gmm_state, gmm_substate);

  if((mm_psm_restore_pending == TRUE) &&
      (gmm_state == GMM_DEREGISTERED) &&
      (gmm_substate == GMM_WAITING_PSM_RESTORE_RSP))
  {
    if((mm_psm_cache_ptr != NULL))
    {
      sys_plmn_id_s_type    rplmn;
      sys_plmn_list_s_type  eplmn_list;
      byte                  index = 0;
      uint64 time_elapsed = 0;

      /* Restore saved context */
      mm_serving_plmn.info.active_rat = SYS_RAT_GSM_RADIO_ACCESS;

      mm_serving_plmn.info.reg_service_domain = mm_psm_cache_ptr->reg_service_domain;

      mm_serving_plmn.info.plmn_service_capability = mm_psm_cache_ptr->gsm.plmn_service_capability;
      gmm_stored_rai = mm_psm_cache_ptr->gsm.gmm_stored_rai;
      gmm_previous_registration_serving_rai = mm_psm_cache_ptr->gsm.last_attempted_rai;
      mm_rat_pri_list_info_to_nw = mm_psm_cache_ptr->gsm.rat_pri_list_info_to_nw;
      mm_rat_pri_list_info = mm_psm_cache_ptr->gsm.rat_pri_list_info;
      gmm_is_lte_advertised_in_last_reg = mm_psm_cache_ptr->gsm.is_lte_advertised_in_last_reg;
      gmm_reg_lte_cs_param = mm_psm_cache_ptr->gsm.lte_cs_domain_param;
  
      gmm_previous_T3311_gmm_substate = (gmm_substate_type)mm_psm_cache_ptr->gsm.gmm_previous_T3311_gmm_substate;
      gmm_ra_update_type = (gmm_ra_update_type_type)mm_psm_cache_ptr->gsm.gmm_ra_update_type;
      gmm_is_ptmsi_present_in_prev_gmm_msg = mm_psm_cache_ptr->gsm.is_ptmsi_present_in_prev_gmm_msg;
  
      gmm_t3312_timer_value = mm_psm_cache_ptr->gsm.t3312_value;
      gmm_t3302_timer_value = mm_psm_cache_ptr->gsm.t3302_value;
      gmm_t3323_timer_value = mm_psm_cache_ptr->gsm.t3323_value;
      gmm_t3319_timer_value = mm_psm_cache_ptr->gsm.t3319_value;
      glif_ready_timer_value = mm_psm_cache_ptr->gsm.gprs_ready_timer_value;
      glif_set_ready_timer_deactivated(mm_psm_cache_ptr->gsm.gprs_ready_timer_deactivated);

      gmm_stored_nw_feature_support = mm_psm_cache_ptr->gsm.gmm_nw_feature_support;
      gmm_drx_cn_coeff_s1 = mm_psm_cache_ptr->gsm.gmm_drx_cn_coeff_s1;

      gmm_reg_rejected_rai = mm_psm_cache_ptr->gsm.gprs_rejected_rai;
      gmm_rau_attempt_counter = mm_psm_cache_ptr->gsm.rau_attempt_counter;
      mm_gs_association_established = mm_psm_cache_ptr->gsm.gs_association_to_be_established;
      gmm_ra_update_pending = mm_psm_cache_ptr->gsm.ra_update_pending;
      gmm_multi_slot_ra_update_pending = mm_psm_cache_ptr->gsm.multi_slot_ra_update_pending;
      gmm_pending_ra_update_type = (gmm_ra_update_type_type)mm_psm_cache_ptr->gsm.pended_ra_update_type;

      gmm_multislot_class = mm_psm_cache_ptr->gsm.multi_slot_class;
      gmm_edge_multislot_class = mm_psm_cache_ptr->gsm.edge_multi_slot_class;

#ifdef FEATURE_GSM_DTM
      gmm_nv_dtm_multislot_subclass = mm_psm_cache_ptr->gsm.dtm_multi_slot_subclass;
#endif
      gmm_xid_negotiation_completed = mm_psm_cache_ptr->gsm.xid_negotiation_completed;
      gmm_max_L3_length = mm_psm_cache_ptr->gsm.gmm_n201_u;
#ifdef FEATURE_DSAC
      mm_last_procedure_to_update_registration = mm_psm_cache_ptr->gsm.last_procedure_to_update_registration;
#endif

#ifdef FEATURE_CIOT
      emm_ctrl_data_ptr->attached_lte_rat = mm_psm_cache_ptr->gsm.attached_lte_rat;
#endif
      if (mm_psm_r12_enabled == TRUE)
      {
        gmm_psm_timer_ctxt.t3324_value = mm_psm_cache_ptr->t3324_value;
#ifdef FEATURE_LTE
       emm_ctrl_data_ptr->t3324_value = gmm_psm_timer_ctxt.t3324_value;
#endif
        gmm_psm_timer_ctxt.allow_psm_in_oos_ltd_srv = TRUE;
      }
      else
      {
        gmm_psm_timer_ctxt.t3324_value = NAS_INVALID_PSM_TIMER_VALUE;
      }
    
      if(mm_edrx_cfg.gsm.enabled == TRUE)
      {
        gmm_edrx_params_gsm_ctxt.edrx_params = mm_psm_cache_ptr->gsm.edrx_params;
      }
      else
      {
        gmm_edrx_params_gsm_ctxt.edrx_params.enabled = FALSE;
        gmm_edrx_params_gsm_ctxt.edrx_params.ptw = NAS_INVALID_EDRX_VALUE;
        gmm_edrx_params_gsm_ctxt.edrx_params.cycle_length = NAS_INVALID_EDRX_VALUE;
      }
      sys_eplmn_list_get(&eplmn_list, &rplmn);
      for(index = 0; (index < eplmn_list.length) && (index < MAX_NO_OF_PLMNs); index++)
      {
        mm_serving_plmn.equiv_plmn_list.plmn[index] = eplmn_list.plmn[index];
      }
      mm_serving_plmn.equiv_plmn_list.length = index;

      mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
      mm_service_loss_on_rat = SYS_RAT_GSM_RADIO_ACCESS;
      gmm_set_state(GMM_REGISTERED);
      gmm_set_substate(GMM_NO_CELL_AVAILABLE);

      time_elapsed = time_get_ms_native() - mm_psm_cache_ptr->timers_timestamp;

      MSG_HIGH_DS_3(MM_SUB, "=MM= Time elapsed = %d T3312 Remaining timeout = %d Orig T3312 = %d", time_elapsed, mm_psm_cache_ptr->gsm.t3312_timer, gmm_t3312_timer_value);

      if((uint64)mm_psm_cache_ptr->gsm.t3312_timer > time_elapsed)
      {
        mm_start_timer(TIMER_T3312, (uint64)mm_psm_cache_ptr->gsm.t3312_timer - time_elapsed);
      }
      else if(gmm_t3312_timer_value != 0)
      {
        if(mm_get_ulog_handle() != NULL)
        {
          ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=MM= T3312 expired in PSM");
        }
        MSG_HIGH_DS_0(MM_SUB, "=MM= T3312 expired in PSM");
        gmm_timer_t3312_expired_in_psm();
      }

#ifdef FEATURE_LTE
      /* Update EMM states */
      EMM_SET_STATE(EMM_REGISTERED);
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
      mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
#endif

#if defined(FEATURE_GSM) && defined(FEATURE_NAS_GW)
      mm_send_rr_forbidden_lai_update();
      mm_add_lai_in_reject_list(NULL);
#endif

      /* Send psm timer change indication to upper layers */
      if(mm_psm_r12_enabled == TRUE)
      {
        mm_send_mmr_psm_timer_to_clients_ind(gmm_psm_timer_ctxt.t3324_value,
                                          glif_ready_timer_value,
                                          gmm_t3312_timer_value);

        mm_send_rr_mtc_param_update_ind(FALSE);

        if (gmmllc_get_glif_llc_conn_state() != LLC_RESUMED)
        {
          if(mm_get_ulog_handle() != NULL)
          {
           ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=MM= GLIF: Sent LLGMM_RESUME_REQ to LLC");
          }
          /* Permit user plane transfers from now on */
          glif_llc_send_resume_request();
        }
      }
      psm_restore_done = TRUE;
    }
    else
    {
      if(mm_sim_revert_sim_state_on_psm_restore_failure == TRUE)
      {
        mm_serving_plmn.info.sim_state = SYS_SIM_STATE_AVAILABLE;
        mm_sim_revert_sim_state_on_psm_restore_failure = FALSE;
      }

      mm_send_mmr_psm_restore_failed_ind();
      sm_psm_free_cache();

      if(mm_timer_status[TIMER_MM_FORBIDDEN_LAI_CLEAR] == TIMER_ACTIVE)
      {
        mm_stop_timer(TIMER_MM_FORBIDDEN_LAI_CLEAR);
        mm_clear_forbidden_LAIs();
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
        mm_possible_imsi_attach = TRUE;
        /* clear rejected csg list */
        mm_clear_rejected_csg_list();
#endif
      }
      gmm_set_state(GMM_NULL);
    }
    mm_psm_restore_pending = FALSE;
    if((psm_restore_done == FALSE) ||
       (gmm_t3346_backoff_timer_value == 0))
    {
      mm_psm_free_cache();
    }
    mm_send_mmr_sim_available_cnf();
  }
} /* end gmm_process_restore_ctxt_psm_cnf() */

/*===========================================================================
FUNCTION    GMM_RESTORE_CTXT_ON_PSM_EXIT

DESCRIPTION
  This function starts GMM/SM context restore in GSM if UE exited PSM

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Context restore started,else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean gmm_restore_ctxt_on_psm_exit
(
  void
)
{
  if (mm_psm_restore_pending)
  {
    gmm_set_state(GMM_DEREGISTERED);
    gmm_set_substate(GMM_WAITING_PSM_RESTORE_RSP);
    gmm_send_sm_restore_ctxt_psm_req();

    mm_serving_plmn.ms_op_mode = SYS_SRV_DOMAIN_PS_ONLY;
    gmmllc_tlli_assign_on_psm_exit();

    return TRUE;
  }
  else
  {
    if(mm_sim_revert_sim_state_on_psm_restore_failure == TRUE)
    {
      mm_serving_plmn.info.sim_state = SYS_SIM_STATE_AVAILABLE;
      mm_sim_revert_sim_state_on_psm_restore_failure = FALSE;
    }

    mm_send_mmr_psm_restore_failed_ind();

    if(mm_timer_status[TIMER_MM_FORBIDDEN_LAI_CLEAR] == TIMER_ACTIVE)
    {
      mm_stop_timer(TIMER_MM_FORBIDDEN_LAI_CLEAR);
      mm_clear_forbidden_LAIs();
      mm_clear_lai_reject_list();
    }
    mm_psm_restore_pending = FALSE;
    mm_psm_free_cache();
    sm_psm_free_cache();
    if (mm_get_ulog_handle() != NULL)
    {
      ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=MM= Error: PSM restore failed");
    }
  }

  return FALSE;
} /* end gmm_restore_ctxt_on_psm_exit() */


/*===========================================================================
FUNCTION    GMM_SEND_SM_RESTORE_CTXT_PSM_REQ

DESCRIPTION
  This function sends a request to SM to restore the saved PDP context upon
  exiting PSM mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_send_sm_restore_ctxt_psm_req(void)
{
  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set         = MS_GMM_SM;
    sm_cmd->header.cmd_id              = GMMSM_RESTORE_CTXT_PSM_REQ;

#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmmsm_restore_ctxt_psm_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    if(mm_get_ulog_handle() != NULL)
    {
      ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=MM= Sent GMMSM_RESTORE_CTXT_PSM_REQ");
    }
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent GMMSM_RESTORE_CTXT_PSM_REQ");
    //sm_put_cmd (sm_cmd);
    
      sm_process_restore_ctxt_psm_req(sm_cmd);
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
} /* end of gmm_send_sm_restore_ctxt_psm_req() */

/*===========================================================================
FUNCTION    GMM_SEND_SM_SAVE_CTXT_PSM_REQ

DESCRIPTION
  This function sends a request to SM to saved the current PDP context upon
  entering PSM mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_send_sm_save_ctxt_psm_req(void)
{
  sm_cmd_type * sm_cmd;
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd != NULL)
  {
    sm_cmd->header.message_set         = MS_GMM_SM;
    sm_cmd->header.cmd_id              = GMMSM_SAVE_CTXT_PSM_REQ;

#ifdef FEATURE_DUAL_SIM
    sm_cmd->cmd.gmmsm_save_ctxt_psm_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    if(mm_get_ulog_handle() != NULL)
    {
      ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=MM= Sent GMMSM_SAVE_CTXT_PSM_REQ");
    }
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent GMMSM_SAVE_CTXT_PSM_REQ");
    //sm_put_cmd (sm_cmd);
   
    sm_process_save_ctxt_psm_req(sm_cmd);
#ifndef FEATURE_MODEM_HEAP
      mem_free( &(tmc_heap), sm_cmd );
#else
      modem_mem_free(sm_cmd, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Unable to get an SM command buffer", 0, 0, 0);
  }
} /* end of emm_send_esm_restore_ctxt_psm_req() */


/*===========================================================================
FUNCTION    GMM_TIMER_T3324_EXPIRE

DESCRIPTION
  Check PSM conditions and send ready indication if GMM is ready for PSM  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_timer_t3324_expire
(
  void
)
{
  boolean               is_ps_sig_pending  = FALSE;
  sys_psm_status_e_type status = mm_check_ready_for_psm();

  MSG_HIGH_DS_1(MM_SUB, "=MM= PSM Active timer expired PSM Ready Status = %d",status);

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)) && defined(FEATURE_NAS_GW) 
  if ( is_umts_mode() || is_gsm_mode())
  {
    if ((gmm_substate == GMM_NORMAL_SERVICE) &&
         (sm_check_pdp_procedure_pending()))
    {
      /* Don't allow to enter PSM mode , if PS signalling connection is pending*/
      is_ps_sig_pending = TRUE;
    }
  }
#endif
 
  if(status == SYS_PSM_STATUS_READY)
  {
    if(mm_psm_ready_req_rejected == TRUE)
    {
      mm_send_mmr_psm_ready_ind();
    }
    else
    {
      mm_send_mmr_psm_auto_ready_ind();
    }
    gmm_psm_timer_ctxt.t3324_expired = FALSE;
    gmm_psm_timer_ctxt.psm_active = TRUE;
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH	
	mm_send_t3324_ready_event();
#endif
  }
  else if(((status == SYS_PSM_STATUS_NOT_READY_NOT_IDLE) ||
            (status == SYS_PSM_STATUS_NOT_READY_ACTIVE_OOS)) &&
          (gmm_state == GMM_REGISTERED) &&
          ((gmm_substate != GMM_NORMAL_SERVICE) ||
           (is_ps_sig_pending == TRUE)))
  {
    gmm_psm_timer_ctxt.t3324_expired = TRUE;
  }
}

/*===========================================================================
FUNCTION    GMM_TIMER_T3312_EXPIRED_IN_PSM

DESCRIPTION
  Check whether periodic RAU needs to be performed after UE exits from PSM mode on PRAU timer expiry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gmm_timer_t3312_expired_in_psm
(
  void
)
{

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

  /* ----------------------------------------------------------------------
    ** When timer T3312 expires, the periodic routing area updating procedure
    ** shall be started and the timer shall be set to its initial value for
    ** the next start
    ** ---------------------------------------------------------------------- */

  if (gmm_state == GMM_REGISTERED)
  {
#if defined(FEATURE_LTE)
    /* If ISR is ON, start ISR deactivate timer T3323. If UE is able to be perform RAU
            then this timer will be stopped immediately after UE moves to connected state */
    if (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
    {
      if (gmm_t3323_timer_value != 0)
      {
        mm_start_timer( TIMER_T3323, gmm_t3323_timer_value );
      }
    }
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
    if ( gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte *)&gmm_current_lai,gmm_current_rac))
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= RAI is in foreground search LAI list");
    }
    else
#endif
/*24.008: 4.7.5.1.5 (j)  Timer T3346 is running
    The MS shall not start the routing area updating procedure unless the MS is accessing the network with access class 11 � 15*/
#ifdef FEATURE_NAS_REL10
    if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
          &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
        )
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore GMM Procedure since T3346 congestion timer is running");
    }
    else
#endif
    if (gmm_substate != GMM_NORMAL_SERVICE)
    {
          /* -------------------------------------------------------------------
            ** If the MS is in other state than GMM-REGISTERED.NORMAL-SERVICE when
            ** the timer expires the periodic routing area updating procedure is
            ** delayed until the MS returns to GMM-REGISTERED.NORMAL-SERVICE
            ** ------------------------------------------------------------------- */
       gmm_periodic_rau_pending = TRUE;
       if (!((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
              (gmm_rai_changed ( gmm_stored_rai.location_area_code,
                                 gmm_stored_rai.routing_area_code, 
                                 gmm_previous_registration_serving_rai.location_area_code,
                                 gmm_previous_registration_serving_rai.routing_area_code ))) && 
             (gmm_update_status == GMM_GU1_UPDATED))
       {
         gmm_ra_update_type       = GMM_PERIODIC_UPDATING;
       }
     }  
   }
}

#endif

#ifdef FEATURE_NAS_REL10
/*===========================================================================
FUNCTION  gmm_evaluate_low_priority_indicator_value

DESCRIPTION
  This function evaluates low priority indicator value for Device Properties IE 
  and informs low priority and EAB supported parameters to GRR.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if low priority, otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
void gmm_evaluate_low_priority_indicator_value (void)
{
  boolean low_priority = FALSE;
  boolean eab_enabled  = FALSE;
  boolean old_priority = gmm_low_priority_indicator;

  /* Low priority indicator shall be set to TRUE when all below conditions are satisfied:
     a) UE is configured for NAS signalling low priority
     a.1) And if UE supports REL-11 then UE is also configured for Extended access barring (EAB)
     b) UE is not performing emergency attach
     c) UE does not have a PDN connection for emergency bearer services established or
        UE is not establishing a PDN connection for emergency bearer services
     d) UE is not configured to use access class 11 - 15 in selected PLMN
     e) UE is not responding to paging */
     
  if((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10) &&
     ((reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE) 
#ifdef FEATURE_NAS_EAB
      &&
      ((nas_gsm_eab_support == FALSE) ||
       (reg_sim_per_subs_chk_eab_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
#endif
     ) &&
     (gmm_check_high_priority() == FALSE) &&
     (gmm_responded_ps_page == FALSE))
  {
    low_priority = TRUE;
  }

  MSG_HIGH_DS_6(MM_SUB, "=MM= Old LAPI %d, New LAPI= %d, NV REL = %d , EAB NV = %d PS_PAGE= %d EAB Config = %d",
             gmm_low_priority_indicator,low_priority,mm_nv_nas_rel_compliance,nas_gsm_eab_support,gmm_responded_ps_page,(reg_sim_per_subs_chk_eab_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE));

  if ((is_gsm_mode()) &&
      (low_priority != gmm_low_priority_indicator))
  {
    gmm_low_priority_indicator = low_priority;

    if ((gmm_responded_ps_page == FALSE) ||
         ((gmm_state == GMM_REGISTERED_INITIATED) ||
           (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)))
    {
      mm_send_rr_device_priority_ind(low_priority);
    }

#ifdef FEATURE_NAS_FREE_FLOAT
    if ((is_gsm_mode()) &&
         (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
         (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE) &&
         (gmmllc_get_glif_grr_service_req_state() == GLIF_NO_SERVICE))
    {
         gmmllc_send_service_req_if_needed();
    }
#endif

  }

}

 
 /*===========================================================================
 FUNCTION	 GMM_CHECK_HIGH_PRIORITY
 
 DESCRIPTION
   This function determinates whether a call is high priority access or not 
   according to Annex D of NAS Spec 
 
 DEPENDENCIES
   None
 
 RETURN VALUE
   TRUE: High priority
   FALSE: Not high priority
 
 SIDE EFFECTS
   None
 ===========================================================================*/
 boolean  gmm_check_high_priority
 (
   void
 )
 {
   sys_plmn_id_s_type  home_plmn;
   boolean plmn_id_is_undefined;
   boolean mnc_includes_pcs_digit;
   uint32  mcc,saved_hplmn_mcc;
   uint32  mnc;

   MSG_HIGH_DS_1(MM_SUB, "=MM= Checking for high priority - AC 0x%x", mm_hi_access_class);
 
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
   home_plmn = reg_sim_per_subs_hplmn_get((sys_modem_as_id_e_type)mm_as_id);
#else
   home_plmn = reg_sim_hplmn_get(); 
#endif
 
   /* Access class 12, 13 or 14 */
   if(((mm_hi_access_class & RRC_ACCESS_CLASS_12_BIT) != 0) ||
       ((mm_hi_access_class & RRC_ACCESS_CLASS_13_BIT) != 0) ||
       ((mm_hi_access_class & RRC_ACCESS_CLASS_14_BIT) != 0))
   {
 
     /* Check if UE is in home country */

     if (memcmp( &(mm_serving_plmn.info.plmn.identity[0]), &(home_plmn.identity[0]), PLMN_SIZE ) == 0 )
     {
       MSG_HIGH_DS_0(MM_SUB, "=MM= AC 12/13/14 Home country - Set priority high");
       return TRUE;
     }
     else
     {
         /*-------------------------------------------------------------- 
         ** Get the MCC of the Home PLMN country and compare it with
         ** current PLMN's MCC, If a match occurs then UE belongs to AC 12-14
         **  ---------------------------------------------------------*/
         sys_plmn_get_mcc_mnc
         (
           mm_serving_plmn.info.plmn,
           &plmn_id_is_undefined,
           &mnc_includes_pcs_digit,
           &mcc,
           &mnc
         );
 
         sys_plmn_get_mcc_mnc
         (
           home_plmn,
           &plmn_id_is_undefined,
           &mnc_includes_pcs_digit,
           &saved_hplmn_mcc,
           &mnc
         );
 
         if ((sys_mcc_same_country(saved_hplmn_mcc,mcc) == TRUE) &&
             (saved_hplmn_mcc != 0))
         {
           MSG_HIGH_DS_0(MM_SUB, "=MM= AC 12/13/14 Visited PLMNs of home country - Set priority high");
           return TRUE;
         }
     }
   }
 
   /* Access class 11 or 15 */
   if (((mm_hi_access_class & RRC_ACCESS_CLASS_11_BIT) != 0) ||
       ((mm_hi_access_class & RRC_ACCESS_CLASS_15_BIT) != 0))
   {

#ifdef FEATURE_EQUIVALENT_HPLMN
     /* Check if serving PLMN is EHPLMN */
     if(reg_sim_is_ehplmn(mm_serving_plmn.info.plmn) == TRUE)
     {
       MSG_HIGH_DS_0(MM_SUB, "=MM= AC 11/15 EHPLMN - Set priority high");
       return TRUE;
     }
#endif
 
     /* Check if serving PLMN is HPLMN */
     if(sys_plmn_match(home_plmn, mm_serving_plmn.info.plmn) == TRUE)
     {
       MSG_HIGH_DS_0(MM_SUB, "=MM= AC 11/15 HPLMN - Set priority high");
       return TRUE;
     }
   }
 
   return FALSE;
 
 } /* end of gmm_check_high_priority() */
 
/*===========================================================================
FUNCTION    GMM_HANDLE_EXTENDED_WAIT_TIME

DESCRIPTION
  This function handles implicit reject wait time received from lower layers for PS domain

DEPENDENCIES
  None

RETURN VALUE
  True if extended wait time is processed else False

SIDE EFFECTS
  None
===========================================================================*/
void gmm_handle_extended_wait_time(mm_cmd_type *message)

{
  dword wait_time = message->cmd.rr_ps_implicit_reject_ind.wait_time;
  mmr_service_ind_s_type     mmr_service_ind;

  MSG_HIGH_DS_2(MM_SUB, "=MM= MM received RR_PS_IMPLICIT_REJECT_IND wait_time = %d (ms) LAPI = %d",
                wait_time,gmm_low_priority_indicator);
  
  /* Start timer T3346 if lower layers provided a non-zero value for extendedWaitTime,
     and the conection establishment cause was delay tolerant access, else ignore the
     extendedWaitTime from lower layers */
  if ((is_gsm_mode()) &&
      (gmm_low_priority_indicator == TRUE))
  {

#ifdef FEATURE_NAS_FREE_FLOAT
      gmmllc_reset_service_state();
#endif
    if (wait_time == 0)
    {
      if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
          (gmm_congestion_due_to_implicit_reject == TRUE))
      {
        mm_stop_timer(TIMER_T3346);
	/* Restart GMM signalling procedure as performed on backoff timer T3346 expiry */
        gmm_reset_congestion_implicit_reject_timer_stopped();
      }
    }
    else if (mm_timer_status[TIMER_T3346] == TIMER_STOPPED)
    {
      if (gmm_state == GMM_REGISTERED_INITIATED)
      {
        gprs_attach_attempt_counter = 0;
        mm_stop_timer( TIMER_T3310 );
        gmm_set_state( GMM_DEREGISTERED );
        gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);
      }
      else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
      {
        gmm_rau_attempt_counter = 0;
        mm_stop_timer( TIMER_T3330 );
        gmm_set_state( GMM_REGISTERED );
        gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE);
        gmm_is_congestion_due_to_rau = TRUE;
      }

      if (gmm_state == GMM_REGISTERED_INITIATED ||
          gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
      {

        if (mm_managed_roaming_enabled)
        {
          mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
        }

#ifdef FEATURE_ENHANCED_NW_SELECTION
        if (reg_sim_read_ens_flag())
        {
          mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
        }
#endif

        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
             (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
             (mm_location_information.location_update_status == UPDATED) &&
             (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
             ((mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_PS) ||
              (mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_ONLY)))
        {
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            SYS_SRV_DOMAIN_CS_ONLY,
                                            FALSE,
                                            SYS_SRV_STATUS_SRV,
                                            TRUE );
        }
        else
        {
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
        }
      }

      mm_stop_timer(TIMER_T3346);
      mm_start_timer(TIMER_T3346, wait_time);

      /*saves congestion plmn associated with the timer*/
      memscpy((void *)&gmm_3gpp_t3346_plmn_id,sizeof(sys_plmn_id_s_type),(void *)&(mm_serving_plmn.info.plmn), sizeof(sys_plmn_id_s_type));
      gmm_t3346_backoff_timer_value = 0;
      gmm_congestion_due_to_implicit_reject = TRUE;

      mm_serving_plmn.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
   
      if (mm_reg_waiting_for_reg_cnf)
      {
        //mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf(NULL,mm_serving_plmn.info.service_status);
   
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
      }
      else
      {
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mmr_service_ind.cause = mm_serving_plmn.cause;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
     }
   }

} /* end emm_handle_extended_wait_time() */


/*===========================================================================
 FUNCTION  gmm_eab_cat_c_chk
 
 DESCRIPTION: This function checks whether EAB is applicable for the UE of category C,
                      i.e, the UE is neither in the PLMN listed as most preferred PLMN of
                      the country where the UEs are roaming in the operator-defined PLMN
                      selector list on the USIM, nor in their HPLMN nor in a PLMN that is
                      equivalent to their HPLMN
 
 DEPENDENCIES
   None

 RETURN VALUE
   TRUE if EAB is applicable else FALSE
 
 SIDE EFFECTS
   None
 ===========================================================================*/
 
 boolean gmm_eab_cat_c_chk(void)
 {
   sys_plmn_id_s_type    current_camped_plmn;
   boolean plmn_id_is_undefined;
   boolean mnc_includes_pcs_digit;
   uint32  mcc,saved_oplmn_mcc;
   uint32  mnc;
   boolean result_eab = TRUE;

   current_camped_plmn = mm_serving_plmn.info.plmn;

   MSG_HIGH_DS_3(MM_SUB, "=MM= Operator Preferred PLMN %X %X %X",
             gmm_highest_preferred_oplmn.identity[0], gmm_highest_preferred_oplmn.identity[1], gmm_highest_preferred_oplmn.identity[2]);
   if (
#ifdef FEATURE_DUAL_SIM
        (reg_sim_per_subs_is_ehplmn(current_camped_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
        (reg_sim_is_ehplmn(current_camped_plmn) )
#endif
        ||(sys_eplmn_list_equivalent_plmn(current_camped_plmn)&& reg_sim_is_ehplmn_in_eplmn())
      )
      {
        result_eab = FALSE;
      }
      else
      {
          sys_plmn_get_mcc_mnc
         (
           current_camped_plmn,
           &plmn_id_is_undefined,
           &mnc_includes_pcs_digit,
           &mcc,
           &mnc
         );
 
         sys_plmn_get_mcc_mnc
         (
           gmm_highest_preferred_oplmn,
           &plmn_id_is_undefined,
           &mnc_includes_pcs_digit,
           &saved_oplmn_mcc,
           &mnc
         );
 
         if (sys_mcc_same_country(saved_oplmn_mcc,mcc) == FALSE)
         {
           gmm_highest_preferred_oplmn = reg_sim_get_most_pref_oplmn_plmn(mcc
#ifdef FEATURE_DUAL_SIM                          
                                             ,(sys_modem_as_id_e_type)mm_sub_id
#endif                   
                                                                                );
         }
 
         if (sys_plmn_match(current_camped_plmn,gmm_highest_preferred_oplmn)== TRUE)
         {
           result_eab = FALSE;
         }

    }

  return result_eab;
}

/*===========================================================================
 FUNCTION  gmm_eab_cat_b_chk
 
 DESCRIPTION: This function checks whether EAB is applicable for the UE of EAB category B,
                      i.e, the UE is neither in their HPLMN/EHPLMN nor in a PLMN that is
                      equivalent to their HPLMN
 
 DEPENDENCIES
   None

 RETURN VALUE
   TRUE if EAB is applicable else FALSE
 
 SIDE EFFECTS
   None
 ===========================================================================*/
 
 boolean gmm_eab_cat_b_chk(void)
 {
   sys_plmn_id_s_type    current_camped_plmn;
   boolean result_eab = TRUE;

   current_camped_plmn = mm_serving_plmn.info.plmn;
   
   if (
#ifdef FEATURE_DUAL_SIM
        (reg_sim_per_subs_is_ehplmn(current_camped_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
        (reg_sim_is_ehplmn(current_camped_plmn) )
#endif
        ||(sys_eplmn_list_equivalent_plmn(current_camped_plmn)&& reg_sim_is_ehplmn_in_eplmn())
      )
      {
        result_eab = FALSE;
      }

  return result_eab;
}

/*===========================================================================
FUNCTION    GMM_RESET_CONGESTION_IMPLICIT_REJECT_TIMER_STOPPED

DESCRIPTION
  This function handles the restart of GMM signalling procedure when GRR stops
  implicit reject timer and sends wait time as 0 to NAS.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void gmm_reset_congestion_implicit_reject_timer_stopped (void)
{
  mmr_service_ind_s_type     mmr_service_ind;

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

#endif

  sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id);

#ifdef FEATURE_NAS_FREE_FLOAT
  /* Enable PS at GRR on T3346 expiry if timer started due to implicit reject*/
  if ((is_gsm_mode()) &&
      (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
      (gmm_low_priority_indicator == TRUE))
  {
    gmmllc_send_service_req_if_needed();
  }
#endif

  if (gmm_state == GMM_REGISTERED && gmm_is_congestion_due_to_rau)
  {
    gmm_update_ra_type();
  }

  /* If T3312 expired when T3346 was running and the cause of
  ** T3346 start was Service Reject (Congestion), not RAU reject (Congestion) */
  else if (mm_timer_status[TIMER_T3312] == TIMER_STOPPED)
  {
    gmm_ra_update_type = GMM_PERIODIC_UPDATING;
  }

  if ((gmm_state == GMM_DEREGISTERED) &&
       (mm_waiting_for_stop_mode_cnf))
  {
    MSG_HIGH_DS_0( MM_SUB,"=MM= Attach will be done after camping on new RAT" );
    gmm_set_substate(GMM_ATTACH_NEEDED);
  }
  else if ((gmm_state == GMM_DEREGISTERED) &&
            (gmm_substate == GMM_ATTEMPTING_TO_ATTACH))
  {
    /* ----------------------------------------------------------
    ** Perform GPRS attach on the expiry of timers T3311 or T3302
    ** (TS 24.008 4.2.4.2.2)
    ** ---------------------------------------------------------- */
    gmm_initiate_gprs_attach();
  }
  else if ((gmm_state == GMM_DEREGISTERED) &&
           ((gmm_substate == GMM_SUSPENDED) || (is_gsm_mode() && gmm_is_suspended)))
  {
    gmm_gprs_attach_pending = TRUE;
  }
  else if ( (gmm_state == GMM_REGISTERED) &&
             (!gmm_is_congestion_due_to_rau && !gmm_periodic_rau_pending ) &&
#ifdef FEATURE_LTE
             (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI) &&
#endif 
             /*  ---------------------------------------------------------- 
             ** Start RAU at the expiry of T3346, if T3312 timeout given by NW is non-zero and 
             ** T3312 is not running (i.e. it expired while T3346 was running).
             ** ---------------------------------------------------------- */
             ((gmm_t3312_timer_value == 0) || (mm_timer_status[TIMER_T3312] != TIMER_STOPPED) ||
              (is_gsm_mode() && 
               (gmm_congestion_due_to_implicit_reject == TRUE) &&
               (gmmllc_get_glif_state() == GMM_READY_STATE)))
           )
   {
     MSG_HIGH_DS_0( MM_SUB,"=MM= RAU not required as already UPDATED"); 

      if (((gmm_update_status == GMM_GU1_UPDATED) &&
                 (!(gmm_rai_changed( gmm_stored_rai.location_area_code,
                                     gmm_stored_rai.routing_area_code,
                                     mm_serving_plmn.lac, mm_serving_plmn.rac ) ||
                    gmm_plmn_changed(gmm_stored_rai.plmn_id,mm_serving_plmn.info.plmn)))) ||
                   (gmm_substate == GMM_LIMITED_SERVICE)) 

     {
       mm_serving_plmn.info.ps_data_suspend = FALSE;
       /* -------------------------------------------------------
       ** Send indication to REG that PS data no longer suspended
       ** ------------------------------------------------------- */
       if (mm_reg_waiting_for_reg_cnf)
       {
         if (mm_ready_to_send_reg_cnf)
         {
           //mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
           mm_send_mmr_reg_cnf(NULL,mm_serving_plmn.info.service_status);

           mm_reg_waiting_for_reg_cnf = FALSE;
           mm_ready_to_send_reg_cnf   = FALSE;
         }
       }
       else
       {
         mmr_service_ind.service_state = mm_serving_plmn.info;
         mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
         mm_send_mmr_service_ind( &mmr_service_ind );
       }
     }
   }
   else if ((gmm_state == GMM_REGISTERED) &&
            (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION))))
   {    
     gmm_set_substate(GMM_UPDATE_NEEDED);
#ifdef FEATURE_DSAC
#ifndef FEATURE_NAS_CS_DISABLED

     if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        ((mm_gs_association_established == TRUE) ||
         (gmm_ra_update_type == GMM_PERIODIC_UPDATING)))
     {
       if (mm_state == MM_IDLE)
       {
         mm_initiate_lu( MM_LU_CAUSE_NORMAL );
       }
       else
       {
         mm_pending_lu_request = NORMAL_LU;
       }
     }
#endif
#endif
   }
   else if((gmm_state == GMM_REGISTERED) &&
           (mm_waiting_for_stop_mode_cnf))
   {
     MSG_HIGH_DS_0( MM_SUB,"=MM= RAU will be done after camping on new RAT" );
     gmm_set_substate(GMM_UPDATE_NEEDED);
     gmm_ra_update_type = gmm_pending_ra_update_type;
     if (gmm_ra_update_type == GMM_PERIODIC_UPDATING)
     {
       gmm_periodic_rau_pending = TRUE;
     }
   }       
   else if ((gmm_state == GMM_REGISTERED) &&
            ((gmm_substate == GMM_NORMAL_SERVICE) ||
             (gmm_substate == GMM_ATTEMPTING_TO_UPDATE) ||
             (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM)))
   {
     /* If UE is configured for "AttachwithIMSI" and current camped PLMN is neither RPLMN 
     ** nor equivalent PLMN then attach with IMSI is performed instead of RAU procedure on T3346 expiry */

     if (mm_check_attach_with_imsi(MM_PS_DOMAIN) == TRUE)
     {
       gmm_handle_local_gprs_detach();
       gmm_initiate_gprs_attach();
     }
     else
     {
        /* --------------------
        ** See 24.008 4.7.5.1.5
        ** -------------------- */
       if (gmm_ra_update_pending)
       {
         gmm_ra_update_type = gmm_pending_ra_update_type;
       }            
       gmm_initiate_routing_area_update();
     }
   }
   else if ((gmm_state == GMM_REGISTERED) &&
            ((gmm_substate == GMM_SUSPENDED) || (is_gsm_mode() && gmm_is_suspended)))
   {
     /* If UE is configured for "AttachwithIMSI" and current camped PLMN is neither RPLMN 
     ** nor equivalent PLMN then attach with IMSI shall be performed instead of RAU procedure on T3340 expiry*/

     if (mm_check_attach_with_imsi(MM_PS_DOMAIN) == TRUE)
     {
       gmm_handle_local_gprs_detach();
       gmm_gprs_attach_pending = TRUE;
     }
     else
     {
       gmm_ra_update_pending = TRUE;
       gmm_pending_ra_update_type = gmm_ra_update_type;
     }
   }
   else
   {
     /* when GS association or periodic needs to be performed and T3311/T3302 expires in OOS/Lim*/
     if ( (gmm_ra_update_type == GMM_PERIODIC_UPDATING) || 
          (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) )
     {
       gmm_periodic_rau_pending = TRUE;
     }
     gmm_previous_T3311_gmm_substate = GMM_NULL_SUBSTATE;
     if((gmm_substate == GMM_LIMITED_SERVICE) && 
        (mm_timer_status[TIMER_T3210] == TIMER_STOPPED) &&
        (mm_timer_status[TIMER_T3211] == TIMER_STOPPED)) 
     {
       mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
       mmr_service_ind.service_state = mm_serving_plmn.info;
      mm_send_mmr_service_ind( &mmr_service_ind );
     }
   }
   /* Reset GMM congestion related flags */
   gmm_is_congestion_due_to_rau = FALSE;
   gmm_congestion_due_to_implicit_reject    = FALSE;
}


#endif /* FEATURE_NAS_REL10 */



#endif
/*===========================================================================
FUNCTION MM_WWAN_SET_TECH_STATUS

DESCRIPTION
This is a function used by NB1/CATM/GERAN for informing change in WWAN tech status whether tech is loaded or unloaded
This function puts the received WWAN current tech status indication into MM command queue

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_wwan_set_tech_status (boolean tech_loaded)
{
   mm_cmd_type   *cmd_ptr;

   /* Get command buffer from MM task */
   if ((cmd_ptr = mm_get_cmd_buf()) != NULL)
   {
      /* Construct message */
     cmd_ptr->cmd.mm_wwan_tech_suspend_ind.IMH.message_set = MS_MM_MM;
     cmd_ptr->cmd.mm_wwan_tech_suspend_ind.IMH.message_id  = MM_WWAN_TECH_SUSPEND_IND;

     PUT_IMH_LEN (sizeof(cmd_ptr->cmd.mm_wwan_tech_suspend_ind) - sizeof(IMH_T),
                       &(cmd_ptr->cmd.mm_wwan_tech_suspend_ind.IMH));
     if (tech_loaded == TRUE)
     {
       cmd_ptr->cmd.mm_wwan_tech_suspend_ind.tech_suspend = FALSE;
     }
     else
     {
       cmd_ptr->cmd.mm_wwan_tech_suspend_ind.tech_suspend = TRUE;
     }

     /* Put on MM command queue */
     mm_put_cmd(cmd_ptr);
   }
   else
   {
     MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }

   return TRUE;
}

/*===========================================================================
FUNCTION GMM_CHECK_WWAN_TECH_ACTIVE

DESCRIPTION
This function returns whether WWAN tech status is loaded or unloaded.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/


boolean  gmm_check_wwan_tech_active (void)
{
  return gmm_wwan_tech_active;
}

/*===========================================================================
FUNCTION GMM_CHECK_WWAN_RESUMPTION_REQ

DESCRIPTION
This function returns whether WWAN resumption is requested to perform high priority 
procedure like OTA detach, stop mode, deactivation.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/


boolean  gmm_check_wwan_resumption_req (void)
{
  return gmm_wwan_resumption_req;
}

/*===========================================================================
FUNCTION GMM_SET_WWAN_RESUMPTION_REQ

DESCRIPTION
This function sets flag as TRUE when tech is in suspened state and 
high priority procedure like OTA detach, stop mode, deactivation needs to be processed.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/


void  gmm_set_wwan_resumption_req (boolean high_priority)
{
  gmm_wwan_resumption_req = high_priority;
}
/*===========================================================================

FUNCTION GMM_SEND_GSTK_IDLE_EVENT IF_NEEDED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_gstk_idle_event_if_needed
(
byte gmm_cause,
boolean service_status_change
)
{
  switch (gmm_cause)
  {
     case ILLEGAL_MS:
     case ILLEGAL_ME:
     case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
     case GPRS_SERVICES_NOT_ALLOWED:
     case PLMN_NOT_ALLOWED:
     case LA_NOT_ALLOWED:
     case NATIONAL_ROAMING_NOT_ALLOWED:
     case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
            ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH) ||
            ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
             (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING))) &&
             (service_status_change == FALSE))
        {
          mm_send_gstk_mm_idle_info();
        }
     default:
       break;
  }
}
/*===========================================================================

FUNCTION GMM_IS_CAMPED_ON_HPLMN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If we get reject cause 7 on HPLMN.
  FALSE: If we don't get reject cause 7 on HPLMN.

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_is_camped_on_hplmn(void)
{
  boolean ret_value = FALSE;

#ifndef FEATURE_EQUIVALENT_HPLMN
	sys_plmn_id_s_type hplmn;
#ifdef FEATURE_DUAL_SIM
		hplmn = reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id);
	else
		hplmn = reg_sim_read_hplmn();
#endif
#endif
	if(
#ifdef FEATURE_EQUIVALENT_HPLMN
#ifdef FEATURE_DUAL_SIM
	   (reg_sim_per_subs_is_ehplmn(mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id) ||
	   (mm_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)&&
					reg_sim_per_subs_is_ehplmn_in_eplmn((sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id)))
#else
	   (reg_sim_is_ehplmn(mm_serving_plmn.info.plmn) ||
	   (mm_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)&&
					reg_sim_is_ehplmn_in_eplmn()))
#endif
#else
	   (sys_plmn_match(hplmn, mm_serving_plmn.info.plmn) ||
	   (mm_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)&&
				  mm_eplmn_list_equivalent_plmn(hplmn)))
#endif
	
	  )
	{
	  ret_value = TRUE;
	}
	MSG_HIGH_1("=MM= got reject cause on HPLMN = %d", ret_value);
  
	return ret_value;

}

boolean gmm_check_if_cause12_to_cause15_allowed(void)
{
  MSG_HIGH_DS_2(MM_SUB,"=MM= Value of mm_map_rej_cause12_to_rej_cause15 is %d gmm_anite_gcf_flag %d ", mm_map_rej_cause12_to_rej_cause15 ,gmm_anite_gcf_flag);
  if(gmm_anite_gcf_flag == FALSE &&
      gmm_is_camped_on_hplmn() == FALSE &&
      mm_map_rej_cause12_to_rej_cause15 == TRUE)
  {
    return TRUE;
  }
  return FALSE;
}
