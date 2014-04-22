/*!
  @file lte_rrc_ccp_test_common.cpp

  @brief
  Common utilities for CSP test

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/10   sureshs Added stub for lte_rrc_plmn_search_is_in_progress
09/04/09   sureshs Added stub for lte_rrc_irat_proc_is_in_progress
08/26/09   sureshs Added stub for lte_rrc_cre_get_t311_time_remaining
08/10/09   amit    Updated stub for lte_rrc_log_serv_cell_info (added 
                   sys_network_selection_mode_e_type as a parameter)
06/15/09   sureshs Stub for lte_rrc_get_release_version added
03/06/09   sureshs Changes for Dec spec compliance
12/14/08   sureshs Stub for lte_rrc_log_serv_cell_info added
10/23/08   sureshs Support for lte_rrc_controller_get_access_class stub
10/09/08   sureshs Changed SIB logic per new interface
10/09/08   sureshs Added functions to construct and free SIB1
10/08/08   amit    Changed uint8 to lte_rrc_sib_bitmask_t
10/08/08   amit    Renamed lte_rrc_get_cell_sib to lte_rrc_sib_get_cell_sib
10/08/08   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_controller.h>
#include <lte_rrc_sib.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_static_rrc_mem_util.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_int_msg.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_log.h>
#include <lte_rrc_cre.h>
#include <lte_rrc_cep.h>
#include <lte_rrc_plmn_search.h>
#include <lte_rrc_utils.h>
#include <fs_public.h>
}

#include "lte_rrc_cep_test_common.h"
#include <tf_stub.h>
#include <TestFramework.h>
#include <qtf_efs.h>
#include <fs_lib.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_csp_test_common.h"

extern "C"
{
TF_PROTO_STUB(stm_state_t,lte_rrc_controller_get_state,void);
TF_PROTO_STUB(void*,lte_rrc_sib_get_cell_sib,lte_rrc_sib_bitmask_t,
              lte_rrc_sib_db_key_t);
TF_PROTO_STUB(uint16,lte_rrc_controller_get_access_class,void);
TF_PROTO_STUB(void,lte_rrc_log_serv_cell_info,lte_rrc_cell_id_s,uint8,
              lte_rrc_plmn_s,sys_network_selection_mode_e_type);
TF_PROTO_STUB(uint64,lte_rrc_cre_get_t311_time_remaining,void);
//TF_PROTO_STUB(boolean,lte_rrc_irat_proc_is_in_progress,void);
TF_PROTO_STUB(boolean,lte_rrc_plmn_search_is_in_progress,void);
TF_PROTO_STUB(lte_3gpp_release_ver_e,lte_rrc_get_3gpp_release_version,void);
TF_PROTO_STUB(stm_state_t,lte_rrc_cre_get_state,void);
TF_PROTO_STUB(lte_rrc_cep_access_barring_status_e,lte_rrc_cep_get_emergency_access_barred_status,lte_rrc_osys_SystemInformationBlockType1 *,
                                         lte_rrc_osys_SystemInformationBlockType2 *);

}

/*! @brief Controller state for unit test
*/
static stm_state_t lte_rrc_csp_test_controller_state;

/*! @brief CRE state for unit test
*/
static stm_state_t lte_rrc_csp_test_cre_state = LTE_RRC_CRE_SM__WAIT_FOR_CFG_CNFI;;

/*! @brief Access class for unit test
*/
static uint16 lte_rrc_csp_test_controller_access_class;

/*! @brief 3gpp release version
*/
static lte_3gpp_release_ver_e lte_rrc_csp_test_3gpp_release_version = LTE_3GPP_REL8;

static lte_cphy_band_scan_result_s band_scan_result[LTE_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS];

/*! @brief Pointer to allocated SIB PDU
*/
static lte_rrc_osys_BCCH_DL_SCH_Message *lte_rrc_csp_test_sib1_pdu_ptr;
static lte_rrc_osys_BCCH_DL_SCH_Message *lte_rrc_csp_test_sib8_pdu_ptr;
static lte_rrc_osys_BCCH_DL_SCH_Message *lte_rrc_csp_test_sib6_pdu_ptr;
static lte_l1_mib_info_param_s lte_rrc_csp_test_mib_info;

static lte_cphy_band_scan_cnf_s bs_cnf;
static lte_cphy_acq_cnf_s acq_cnf;
static lte_cphy_system_scan_cnf_s ss_cnf;

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_efs_rlf_sys_scan_duration_write

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_test_efs_rlf_sys_scan_duration_write 

    @return
    Sets the RLF Scan Duration EFS value

*/
/*=========================================================================*/

void lte_rrc_csp_test_efs_rlf_sys_scan_duration_write(uint32 duration)
{
  int oflag, mode;
  uint32 dur = duration;  /* Init EFS */
  qtf_efs_initialize("lte_rrc_csp_test_efs_rlf_sys_scan_duration_write");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;
  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/csp/rlf_system_scan_duration", 
                    &dur, sizeof(uint32), oflag, mode) >= 0);
}

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_controller_get_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_controller_get_state 

    @return
    Returns the RRC controller state (in the CSP test framework)

*/
/*=========================================================================*/
static stm_state_t lte_rrc_csp_test_stub_controller_get_state(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_csp_test_controller_state);
}/*lte_rrc_csp_test_stub_controller_get_state*/

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_cre_set_state

===========================================================================*/
/*!
    @brief
    Set the desired output of lte_rrc_controller_get_state 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_stub_cre_set_state
(
  stm_state_t state /*!< Desired state */
)
{
  lte_rrc_csp_test_cre_state = state;
}/*lte_rrc_csp_test_stub_controller_set_state*/

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_cre_get_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_cre_get_state 

    @return
    Returns the RRC cre state (in the CSP test framework)

*/
/*=========================================================================*/
static stm_state_t lte_rrc_csp_test_stub_cre_get_state(void)
{
  TF_MSG("Stub was invoked");

  return lte_rrc_csp_test_cre_state;
}/*lte_rrc_csp_test_stub_cre_get_state*/

/*===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_lte_rrc_cep_test_stub_cep_get_emergency_access_barred_status 

    @return
    Returns the RRC cep get_emergency_access_barred_status (in the CSP test framework)

*/
/*=========================================================================*/
static lte_rrc_cep_access_barring_status_e lte_rrc_cep_test_stub_cep_get_emergency_access_barred_status(lte_rrc_osys_SystemInformationBlockType1 *sib1,
                                         lte_rrc_osys_SystemInformationBlockType2 *sib2)
{
  TF_MSG("Stub was invoked");

  return LTE_RRC_CEP_ACCESS_NOT_BARRED;
}/*lte_rrc_cep_test_stub_cep_get_emergency_access_barred_status*/


/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_controller_set_state

===========================================================================*/
/*!
    @brief
    Set the desired output of lte_rrc_controller_get_state 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_stub_controller_set_state
(
  stm_state_t state /*!< Desired state */
)
{
  lte_rrc_csp_test_controller_state = state;
}/*lte_rrc_csp_test_stub_controller_set_state*/

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_controller_get_access_class

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_controller_get_access_class 

    @return
    Returns the access class (in the CSP test framework)

*/
/*=========================================================================*/
uint16 lte_rrc_csp_test_stub_controller_get_access_class(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_csp_test_controller_access_class);
}/* lte_rrc_csp_test_stub_controller_get_access_class */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_set_access_class

===========================================================================*/
/*!
    @brief
    Set the desired output of lte_rrc_controller_get_access_class 

    @return
    void

*/
/*=========================================================================*/
void lte_rrc_csp_test_stub_controller_set_access_class
(
  uint16 access_class /*!< access class */
)
{
  lte_rrc_csp_test_controller_access_class = access_class;
} /* lte_rrc_csp_test_stub_controller_set_access_class */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_set_3gpp_release_version

===========================================================================*/
/*!
    @brief
    Set the desired output of lte_rrc_csp_test_stub_get_3gpp_release_version 

    @return
    void

*/
/*=========================================================================*/
void lte_rrc_csp_test_stub_set_3gpp_release_version
(
  lte_3gpp_release_ver_e rel_ver
)
{
    lte_rrc_csp_test_3gpp_release_version = rel_ver;
} /* lte_rrc_csp_test_stub_set_3gpp_release_version */


/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_set_band_scan_result_ptr

===========================================================================*/
/*!
    @brief
    Set the memory for band scan results in lte_cphy_band_scan_cnf_s

    @return
    void

*/
/*=========================================================================*/
void lte_rrc_csp_test_set_band_scan_result_ptr
(
  lte_cphy_band_scan_cnf_s *bs_cnf_ptr
)
{
    bs_cnf_ptr->band_ptr = band_scan_result;
} /* lte_rrc_csp_test_set_band_scan_result_ptr */


/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_set_num_tx_antennas

===========================================================================*/
/*!
    @brief
    Set num_tx_antennas in MIB

    @return
    void *

*/
/*=========================================================================*/
void lte_rrc_csp_test_set_num_tx_antennas
(
  uint8 num_tx_antennas
)
{
  lte_rrc_csp_test_mib_info.num_tx_antennas = num_tx_antennas;
} /* lte_rrc_csp_test_set_num_tx_antennas */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_get_cell_sib

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_sib_get_cell_sib 

    @return
    void *

*/
/*=========================================================================*/
static void *lte_rrc_csp_test_stub_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/,
  lte_rrc_sib_db_key_t sib_db_index /*!< Index into SIB db */
)
{
  LTE_RRC_UNUSED(sib);
  LTE_RRC_UNUSED(sib_db_index);

  TF_MSG("Stub was invoked");

  if (sib & LTE_RRC_SIB8_BITMASK)
      return lte_rrc_csp_test_sib8_pdu_ptr->message.u.c1->u.
          systemInformation->criticalExtensions.u.systemInformation_r8->
          sib_TypeAndInfo.elem->u.sib8;
  else if (sib & LTE_RRC_SIB6_BITMASK)
      return lte_rrc_csp_test_sib6_pdu_ptr->message.u.c1->u.
          systemInformation->criticalExtensions.u.systemInformation_r8->
          sib_TypeAndInfo.elem->u.sib6;
  else if (sib & LTE_RRC_SIB7_BITMASK)
      return NULL;
  else if (sib & LTE_RRC_MIB_BITMASK)
  {
      if (!lte_rrc_csp_test_mib_info.num_tx_antennas)
          lte_rrc_csp_test_mib_info.num_tx_antennas = 1;
      lte_rrc_csp_test_mib_info.dl_bandwidth = LTE_BW_NRB_6;
      return &lte_rrc_csp_test_mib_info;
  }
  else
      return lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->u.
          systemInformationBlockType1;
}/* lte_rrc_csp_test_stub_get_cell_sib */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_log_serv_cell_info

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_log_serv_cell_info 

    @return
    void

*/
/*=========================================================================*/
static void lte_rrc_csp_test_stub_log_serv_cell_info
(
  lte_rrc_cell_id_s cell_id, /*!< Cell ID */
  uint8 sib_db_key, /*!< Index into SIB db */
  lte_rrc_plmn_s sel_plmn, /*!< Selected PLMN */
  sys_network_selection_mode_e_type sys_network_selection_mode /*!< NW selection mode */
)
{
  LTE_RRC_UNUSED(cell_id);
  LTE_RRC_UNUSED(sib_db_key);
  LTE_RRC_UNUSED(sel_plmn);
  LTE_RRC_UNUSED(sys_network_selection_mode);

  TF_MSG("Stub was invoked");
}/* lte_rrc_csp_test_stub_log_serv_cell_info */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_cre_get_t311_time_remaining

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_cre_get_t311_time_remaining 

    @return
    Returns 0

*/
/*=========================================================================*/
static uint64 lte_rrc_csp_test_stub_cre_get_t311_time_remaining(void)
{
  TF_MSG("Stub was invoked");

  return (0);
}/* lte_rrc_csp_test_stub_cre_get_t311_time_remaining */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_irat_proc_is_in_progress

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_irat_proc_is_in_progress 

    @return
    boolean

*/
/*=========================================================================*/
static boolean lte_rrc_csp_test_stub_irat_proc_is_in_progress(void)
{
  TF_MSG("Stub was invoked");

  return FALSE;
}/* lte_rrc_csp_test_stub_irat_proc_is_in_progress */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_plmn_search_is_in_progress

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_irat_plmn_search_is_in_progress 

    @return
    boolean

*/
/*=========================================================================*/
static boolean lte_rrc_csp_test_stub_plmn_search_is_in_progress(void)
{
  TF_MSG("Stub was invoked");

  return FALSE;
}/* lte_rrc_csp_test_stub_plmn_search_is_in_progress */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_get_3gpp_release_version

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_get_3gpp_release_version

    @return
    boolean

*/
/*=========================================================================*/
static lte_3gpp_release_ver_e lte_rrc_csp_test_stub_get_3gpp_release_version(void)
{
  TF_MSG("Stub was invoked");

  return lte_rrc_csp_test_3gpp_release_version;
}/* lte_rrc_csp_test_stub_get_3gpp_release_version */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_get_sib1_pdu_ptr

===========================================================================*/
/*!
    @brief
    Returns SIB1 PUD pointer 

    @return
    lte_rrc_osys_BCCH_DL_SCH_Message *

*/
/*=========================================================================*/
void *lte_rrc_csp_test_get_sib1_pdu_ptr(void)
{
  return lte_rrc_csp_test_sib1_pdu_ptr;
} /* lte_rrc_csp_test_get_sib1_pdu_ptr */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_get_sib8_pdu_ptr

===========================================================================*/
/*!
    @brief
    Returns SIB8 PUD pointer 

    @return
    lte_rrc_osys_BCCH_DL_SCH_Message *

*/
/*=========================================================================*/
void *lte_rrc_csp_test_get_sib8_pdu_ptr(void)
{
  return lte_rrc_csp_test_sib8_pdu_ptr;
} /* lte_rrc_csp_test_get_sib8_pdu_ptr */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_get_sib6_pdu_ptr

===========================================================================*/
/*!
    @brief
    Returns SIB6 PUD pointer 

    @return
    lte_rrc_osys_BCCH_DL_SCH_Message *

*/
/*=========================================================================*/
void *lte_rrc_csp_test_get_sib6_pdu_ptr(void)
{
  return lte_rrc_csp_test_sib6_pdu_ptr;
} /* lte_rrc_csp_test_get_sib6_pdu_ptr */


/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_stub_init

===========================================================================*/
/*!
    @brief
    Initializes the stub required for CSP unit test 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_stub_init()
{
  TF_STUB(lte_rrc_controller_get_state) = 
  lte_rrc_csp_test_stub_controller_get_state;
  TF_STUB(lte_rrc_sib_get_cell_sib) = 
  lte_rrc_csp_test_stub_get_cell_sib;
  TF_STUB(lte_rrc_controller_get_access_class) = 
  lte_rrc_csp_test_stub_controller_get_access_class;
  TF_STUB(lte_rrc_log_serv_cell_info) = 
  lte_rrc_csp_test_stub_log_serv_cell_info;
  TF_STUB(lte_rrc_cre_get_t311_time_remaining) = 
    lte_rrc_csp_test_stub_cre_get_t311_time_remaining;
  //TF_STUB(lte_rrc_irat_proc_is_in_progress) = 
  //lte_rrc_csp_test_stub_irat_proc_is_in_progress;
  TF_STUB(lte_rrc_plmn_search_is_in_progress) = 
  lte_rrc_csp_test_stub_plmn_search_is_in_progress;
  TF_STUB(lte_rrc_get_3gpp_release_version) = 
      lte_rrc_csp_test_stub_get_3gpp_release_version;
  TF_STUB(lte_rrc_cre_get_state) = 
  lte_rrc_csp_test_stub_cre_get_state;
  TF_STUB(lte_rrc_cep_get_emergency_access_barred_status) = 
  lte_rrc_cep_test_stub_cep_get_emergency_access_barred_status;  
}/* lte_rrc_csp_test_stub_init */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_construct_basic_sib1

===========================================================================*/
/*!
    @brief
    Constructs a basic SIB1 PDU 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_construct_basic_sib1(void)
{
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr;
  lte_rrc_osys_PLMN_IdentityList *plmn_list_ptr;
  lte_rrc_osys_MCC_MNC_Digit *mcc_elem_ptr;
  lte_rrc_osys_MCC_MNC_Digit *mnc_elem_ptr;

  lte_rrc_csp_test_sib1_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *) 
                                   lte_rrc_asn1_osys_IE_malloc
                                  (sizeof(lte_rrc_osys_BCCH_DL_SCH_Message));

  lte_rrc_csp_test_sib1_pdu_ptr->message.t = 
    T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1;

  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1 = 
    (lte_rrc_osys_BCCH_DL_SCH_MessageType_c1 *)
      lte_rrc_asn1_osys_IE_malloc(
        sizeof(lte_rrc_osys_BCCH_DL_SCH_MessageType_c1));
  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->t = 
    T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformationBlockType1;

  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1 = 
    (lte_rrc_osys_SystemInformationBlockType1 *)
      lte_rrc_asn1_osys_IE_malloc(
        sizeof(lte_rrc_osys_SystemInformationBlockType1));
  sib1_ptr = 
    lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1;

  sib1_ptr->m.nonCriticalExtensionPresent = 0;
  sib1_ptr->m.p_MaxPresent = 0;
  sib1_ptr->m.tdd_ConfigPresent = 0;
  sib1_ptr->cellAccessRelatedInfo.m.csg_IdentityPresent = 0;

  sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.n = 1;
  sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.elem =
    (lte_rrc_osys_PLMN_IdentityInfo *) 
      lte_rrc_asn1_osys_IE_malloc(
        sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.n * 
        sizeof(lte_rrc_osys_PLMN_IdentityInfo));
  plmn_list_ptr =  &(sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList);
  plmn_list_ptr->elem->cellReservedForOperatorUse = 
    lte_rrc_osys_PLMN_IdentityInfo_cellReservedForOperatorUse_notReserved;
  plmn_list_ptr->elem->plmn_Identity.m.mccPresent = 1;

  plmn_list_ptr->elem->plmn_Identity.mcc.n = 3;
  plmn_list_ptr->elem->plmn_Identity.mcc.elem = 
    (lte_rrc_osys_MCC_MNC_Digit *)
      lte_rrc_asn1_osys_IE_malloc(plmn_list_ptr->elem->plmn_Identity.mcc.n *
                                  sizeof(lte_rrc_osys_MCC_MNC_Digit));
  mcc_elem_ptr = plmn_list_ptr->elem->plmn_Identity.mcc.elem;
  *(mcc_elem_ptr) = 1;
  mcc_elem_ptr++;
  *(mcc_elem_ptr) = 2;
  mcc_elem_ptr++;
  *(mcc_elem_ptr) = 3;

  plmn_list_ptr->elem->plmn_Identity.mnc.n = 3;
  plmn_list_ptr->elem->plmn_Identity.mnc.elem = 
    (lte_rrc_osys_MCC_MNC_Digit *) 
    lte_rrc_asn1_osys_IE_malloc(plmn_list_ptr->elem->plmn_Identity.mnc.n  * 
                                sizeof(lte_rrc_osys_MCC_MNC_Digit));
  mnc_elem_ptr = plmn_list_ptr->elem->plmn_Identity.mnc.elem;
  *(mnc_elem_ptr) = 4;
  mnc_elem_ptr++;
  *(mnc_elem_ptr) = 5;
  mnc_elem_ptr++;
  *(mnc_elem_ptr) = 6;

  //actually 16 bits, here 8 bits for simplicity
  sib1_ptr->cellAccessRelatedInfo.trackingAreaCode.numbits = 8;
  *sib1_ptr->cellAccessRelatedInfo.trackingAreaCode.data = 1;

  //actually 28 bits, here 8 bits for simplicity
  sib1_ptr->cellAccessRelatedInfo.cellIdentity.numbits = 8;
  *sib1_ptr->cellAccessRelatedInfo.cellIdentity.data = 1;

  sib1_ptr->cellAccessRelatedInfo.cellBarred = 
    lte_rrc_osys_SystemInformationBlockType1_cellAccessRelatedInfo_cellBarred_notBarred;
  sib1_ptr->cellAccessRelatedInfo.csg_Indication = 0;

  sib1_ptr->cellSelectionInfo.m.q_RxLevMinOffsetPresent = 0;
  sib1_ptr->cellSelectionInfo.q_RxLevMin = -70;

  sib1_ptr->freqBandIndicator = 1;

  sib1_ptr->schedulingInfoList.n = 1;
  sib1_ptr->schedulingInfoList.elem = (lte_rrc_osys_SchedulingInfo *)
    lte_rrc_asn1_osys_IE_malloc(sib1_ptr->schedulingInfoList.n *
                                sizeof(lte_rrc_osys_SchedulingInfo));
  sib1_ptr->schedulingInfoList.elem->si_Periodicity = 
    lte_rrc_osys_SchedulingInfo_si_Periodicity_rf8;

  sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.n = 1;
  sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.elem = 
    (lte_rrc_osys_SIB_Type *) 
      lte_rrc_asn1_osys_IE_malloc(
          sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.n *
          sizeof(lte_rrc_osys_SIB_Type));
  *(sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.elem) = 
    lte_rrc_osys_SIB_Type_sibType3;

  sib1_ptr->si_WindowLength = 
    lte_rrc_osys_SystemInformationBlockType1_si_WindowLength_ms1;

  sib1_ptr->systemInfoValueTag = 0;

  sib1_ptr->m.nonCriticalExtensionPresent = 1;
  sib1_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent = 1;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.m.cellSelectionInfo_v920Present = 1;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.cellSelectionInfo_v920.q_QualMin_r9 = -10;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.cellSelectionInfo_v920.m.q_QualMinOffset_r9Present = 1;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.cellSelectionInfo_v920.q_QualMinOffset_r9 = 1;
} /* lte_rrc_csp_test_construct_basic_sib1 */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_construct_basic_sib1_bandChgFly

===========================================================================*/
/*!
    @brief
    Constructs a basic SIB1 PDU 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_construct_basic_sib1_bandChgFly
(
   sys_sband_lte_e_type band,
   boolean intrFreqSelectionNotAllowed,
   boolean cellNotBarred
 )
{
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr;
  lte_rrc_osys_PLMN_IdentityList *plmn_list_ptr;
  lte_rrc_osys_MCC_MNC_Digit *mcc_elem_ptr;
  lte_rrc_osys_MCC_MNC_Digit *mnc_elem_ptr;

  lte_rrc_csp_test_sib1_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *) 
                                   lte_rrc_asn1_osys_IE_malloc
                                  (sizeof(lte_rrc_osys_BCCH_DL_SCH_Message));

  lte_rrc_csp_test_sib1_pdu_ptr->message.t = 
    T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1;

  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1 = 
    (lte_rrc_osys_BCCH_DL_SCH_MessageType_c1 *)
      lte_rrc_asn1_osys_IE_malloc(
        sizeof(lte_rrc_osys_BCCH_DL_SCH_MessageType_c1));
  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->t = 
    T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformationBlockType1;

  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1 = 
    (lte_rrc_osys_SystemInformationBlockType1 *)
      lte_rrc_asn1_osys_IE_malloc(
        sizeof(lte_rrc_osys_SystemInformationBlockType1));
  sib1_ptr = 
    lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1;

  sib1_ptr->m.nonCriticalExtensionPresent = 0;
  sib1_ptr->m.p_MaxPresent = 0;
  sib1_ptr->m.tdd_ConfigPresent = 0;
  sib1_ptr->cellAccessRelatedInfo.m.csg_IdentityPresent = 0;

  sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.n = 1;
  sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.elem =
    (lte_rrc_osys_PLMN_IdentityInfo *) 
      lte_rrc_asn1_osys_IE_malloc(
        sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.n * 
        sizeof(lte_rrc_osys_PLMN_IdentityInfo));
  plmn_list_ptr =  &(sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList);
  plmn_list_ptr->elem->cellReservedForOperatorUse = 
    lte_rrc_osys_PLMN_IdentityInfo_cellReservedForOperatorUse_notReserved;
  plmn_list_ptr->elem->plmn_Identity.m.mccPresent = 1;

  plmn_list_ptr->elem->plmn_Identity.mcc.n = 3;
  plmn_list_ptr->elem->plmn_Identity.mcc.elem = 
    (lte_rrc_osys_MCC_MNC_Digit *)
      lte_rrc_asn1_osys_IE_malloc(plmn_list_ptr->elem->plmn_Identity.mcc.n *
                                  sizeof(lte_rrc_osys_MCC_MNC_Digit));
  mcc_elem_ptr = plmn_list_ptr->elem->plmn_Identity.mcc.elem;
  *(mcc_elem_ptr) = 1;
  mcc_elem_ptr++;
  *(mcc_elem_ptr) = 2;
  mcc_elem_ptr++;
  *(mcc_elem_ptr) = 3;

  plmn_list_ptr->elem->plmn_Identity.mnc.n = 3;
  plmn_list_ptr->elem->plmn_Identity.mnc.elem = 
    (lte_rrc_osys_MCC_MNC_Digit *) 
    lte_rrc_asn1_osys_IE_malloc(plmn_list_ptr->elem->plmn_Identity.mnc.n  * 
                                sizeof(lte_rrc_osys_MCC_MNC_Digit));
  mnc_elem_ptr = plmn_list_ptr->elem->plmn_Identity.mnc.elem;
  *(mnc_elem_ptr) = 4;
  mnc_elem_ptr++;
  *(mnc_elem_ptr) = 5;
  mnc_elem_ptr++;
  *(mnc_elem_ptr) = 6;

  //actually 16 bits, here 8 bits for simplicity
  sib1_ptr->cellAccessRelatedInfo.trackingAreaCode.numbits = 8;
  *sib1_ptr->cellAccessRelatedInfo.trackingAreaCode.data = 1;

  //actually 28 bits, here 8 bits for simplicity
  sib1_ptr->cellAccessRelatedInfo.cellIdentity.numbits = 8;
  *sib1_ptr->cellAccessRelatedInfo.cellIdentity.data = 1;

  sib1_ptr->cellAccessRelatedInfo.cellBarred = 
                                  cellNotBarred;
  sib1_ptr->cellAccessRelatedInfo.intraFreqReselection = 
                                  intrFreqSelectionNotAllowed;
  sib1_ptr->cellAccessRelatedInfo.csg_Indication = 0;

  sib1_ptr->cellSelectionInfo.m.q_RxLevMinOffsetPresent = 0;
  sib1_ptr->cellSelectionInfo.q_RxLevMin = -70;

  sib1_ptr->freqBandIndicator = band;

  sib1_ptr->schedulingInfoList.n = 1;
  sib1_ptr->schedulingInfoList.elem = (lte_rrc_osys_SchedulingInfo *)
    lte_rrc_asn1_osys_IE_malloc(sib1_ptr->schedulingInfoList.n *
                                sizeof(lte_rrc_osys_SchedulingInfo));
  sib1_ptr->schedulingInfoList.elem->si_Periodicity = 
    lte_rrc_osys_SchedulingInfo_si_Periodicity_rf8;

  sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.n = 1;
  sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.elem = 
    (lte_rrc_osys_SIB_Type *) 
      lte_rrc_asn1_osys_IE_malloc(
          sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.n *
          sizeof(lte_rrc_osys_SIB_Type));
  *(sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.elem) = 
    lte_rrc_osys_SIB_Type_sibType3;

  sib1_ptr->si_WindowLength = 
    lte_rrc_osys_SystemInformationBlockType1_si_WindowLength_ms1;

  sib1_ptr->systemInfoValueTag = 0;

  sib1_ptr->m.nonCriticalExtensionPresent = 1;
  sib1_ptr->nonCriticalExtension.m.nonCriticalExtensionPresent = 1;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.m.cellSelectionInfo_v920Present = 1;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.cellSelectionInfo_v920.q_QualMin_r9 = -10;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.cellSelectionInfo_v920.m.q_QualMinOffset_r9Present = 1;
  sib1_ptr->nonCriticalExtension.nonCriticalExtension.cellSelectionInfo_v920.q_QualMinOffset_r9 = 1;
} /* lte_rrc_csp_test_construct_basic_sib1 */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_construct_sib1_with_sib8

===========================================================================*/
/*!
    @brief
    Constructs a SIB1 PDU with SIB 8 scheduled also

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_construct_sib1_with_sib8(void)
{
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr;
  lte_rrc_osys_PLMN_IdentityInfo *plmn_elem_ptr;
  lte_rrc_osys_MCC_MNC_Digit *mcc_elem_ptr;
  lte_rrc_osys_MCC_MNC_Digit *mnc_elem_ptr;

  lte_rrc_csp_test_sib1_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *) 
                                   lte_rrc_asn1_osys_IE_malloc
                                  (sizeof(lte_rrc_osys_BCCH_DL_SCH_Message));

  lte_rrc_csp_test_sib1_pdu_ptr->message.t = 
    T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1;
  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1 = 
    (lte_rrc_osys_BCCH_DL_SCH_MessageType_c1 *) 
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_BCCH_DL_SCH_MessageType_c1));
  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->t = 
    T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation;
  lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1 = 
    (lte_rrc_osys_SystemInformationBlockType1 *) 
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_SystemInformationBlockType1));
  sib1_ptr = 
    lte_rrc_csp_test_sib1_pdu_ptr->message.u.c1->u.systemInformationBlockType1;

  sib1_ptr->m.nonCriticalExtensionPresent = 0;
  sib1_ptr->m.p_MaxPresent = 0;
  sib1_ptr->m.tdd_ConfigPresent = 0;
  sib1_ptr->cellAccessRelatedInfo.m.csg_IdentityPresent = 0;

  sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.n = 1;
  sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.elem = 
    (lte_rrc_osys_PLMN_IdentityInfo *) 
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_PLMN_IdentityInfo));
  plmn_elem_ptr =  sib1_ptr->cellAccessRelatedInfo.plmn_IdentityList.elem;
  plmn_elem_ptr->cellReservedForOperatorUse = 
    lte_rrc_osys_PLMN_IdentityInfo_cellReservedForOperatorUse_notReserved;
  plmn_elem_ptr->plmn_Identity.m.mccPresent = 1;

  plmn_elem_ptr->plmn_Identity.mcc.n = 3;
  plmn_elem_ptr->plmn_Identity.mcc.elem = 
    (lte_rrc_osys_MCC_MNC_Digit *)
      lte_rrc_asn1_osys_IE_malloc(plmn_elem_ptr->plmn_Identity.mcc.n *
                                  sizeof(lte_rrc_osys_MCC_MNC_Digit));
  mcc_elem_ptr = plmn_elem_ptr->plmn_Identity.mcc.elem;
  *(mcc_elem_ptr) = 1;
  mcc_elem_ptr++;
  *(mcc_elem_ptr) = 2;
  mcc_elem_ptr++;
  *(mcc_elem_ptr) = 3;

  plmn_elem_ptr->plmn_Identity.mnc.n = 3;
  plmn_elem_ptr->plmn_Identity.mnc.elem = 
    (lte_rrc_osys_MCC_MNC_Digit *) 
    lte_rrc_asn1_osys_IE_malloc(plmn_elem_ptr->plmn_Identity.mnc.n  * 
                                sizeof(lte_rrc_osys_MCC_MNC_Digit));
  mnc_elem_ptr = plmn_elem_ptr->plmn_Identity.mnc.elem;
  *(mnc_elem_ptr) = 4;
  mnc_elem_ptr++;
  *(mnc_elem_ptr) = 5;
  mnc_elem_ptr++;
  *(mnc_elem_ptr) = 6;

  //actually 16 bits, here 8 bits for simplicity
  sib1_ptr->cellAccessRelatedInfo.trackingAreaCode.numbits = 8;
  *sib1_ptr->cellAccessRelatedInfo.trackingAreaCode.data = 1;

  //actually 28 bits, here 8 bits for simplicity
  sib1_ptr->cellAccessRelatedInfo.cellIdentity.numbits = 8;
  *sib1_ptr->cellAccessRelatedInfo.cellIdentity.data = 1;

  sib1_ptr->cellAccessRelatedInfo.cellBarred = 
    lte_rrc_osys_SystemInformationBlockType1_cellAccessRelatedInfo_cellBarred_notBarred;
  sib1_ptr->cellAccessRelatedInfo.csg_Indication = 0;

  sib1_ptr->cellSelectionInfo.m.q_RxLevMinOffsetPresent = 0;
  sib1_ptr->cellSelectionInfo.q_RxLevMin = -70;

  sib1_ptr->freqBandIndicator = 1;

  sib1_ptr->schedulingInfoList.n = 1;
  sib1_ptr->schedulingInfoList.elem = (lte_rrc_osys_SchedulingInfo *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_SchedulingInfo));
  sib1_ptr->schedulingInfoList.elem->si_Periodicity = lte_rrc_osys_SchedulingInfo_si_Periodicity_rf8;

  sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.n = 1;
  sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.elem = 
    (lte_rrc_osys_SIB_Type *) 
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_SIB_Type));
  *sib1_ptr->schedulingInfoList.elem->sib_MappingInfo.elem = 
    lte_rrc_osys_SIB_Type_sibType8; // (Perhaps not spec compliant - Simplified to only show SIB-8 for Unit testing

  sib1_ptr->si_WindowLength = lte_rrc_osys_SystemInformationBlockType1_si_WindowLength_ms1;

  sib1_ptr->systemInfoValueTag = 0;
} /* lte_rrc_csp_test_construct_sib1_with_sib8 */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_free_sib1_pdu

===========================================================================*/
/*!
    @brief
    Frees a basic SIB1 PDU 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_free_sib1_pdu(void)
{
  lte_rrc_osys_asn1_free_pdu(lte_rrc_csp_test_sib1_pdu_ptr,
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
} /* lte_rrc_csp_test_free_sib1_pdu */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_construct_basic_sib8

===========================================================================*/
/*!
    @brief
    Constructs a basic SIB8 PDU 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_construct_basic_sib8(void)
{
  int decode_status = 0;
  uint8 sib8_msg[] = 
        {
            0x00, 0x18, 0x50, 0x00, 0x00, 0x00, 0x00, 0x03, 0xDB,
            0x05, 0x91, 0x2C, 0xE1, 0x02, 0x64, 0x4B, 0x00
        };       

  lte_rrc_csp_test_sib8_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(sib8_msg, sizeof(sib8_msg), 
                               BCCH_DL_SCH_Message, &decode_status);

} /* lte_rrc_csp_test_construct_basic_sib8 */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_free_sib8_pdu

===========================================================================*/
/*!
    @brief
    Frees a basic SIB8 PDU 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_free_sib8_pdu(void)
{
  lte_rrc_osys_asn1_free_pdu(lte_rrc_csp_test_sib8_pdu_ptr,
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
} /* lte_rrc_csp_test_free_sib8_pdu */


/*
sib8_msg
sib8_msg:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib8 : 
                {
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          threshX-High 0,
                          threshX-Low 0
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 1900,
                              physCellIdList 
                              {
                                200,
                                300
                              }
                            },
                            {
                              arfcn 1800,
                              physCellIdList 
                              {
                                100,
                                150
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0
                    }
                  }
                }
            }
          }
      }
}

sib8_msg:CP_EncodedMessageType
{
  msg_type = CP_RRC_MSG;
  pdu_id = 53;
  msg_len = 0;
  msg_buf = "00 18 50 00 00 00 00 03 DB 05 91 2C E1 02 64 4B 00 ";
}

*/

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_construct_basic_sib6

===========================================================================*/
/*!
    @brief
    Constructs a basic SIB6 PDU 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_construct_basic_sib6(void)
{
  int decode_status = 0;
  uint8 sib6_msg[] = 
    {
      0x00, 0x11, 0x04, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x20, 0x80,
      0x0B, 0xE9, 0xE0, 0x00
    };      
    lte_rrc_csp_test_sib6_pdu_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(sib6_msg, sizeof(sib6_msg),                              
                                     BCCH_DL_SCH_Message, &decode_status);
    
} /* lte_rrc_csp_test_construct_basic_sib6 */

/*===========================================================================

   FUNCTION:  lte_rrc_csp_test_free_sib6_pdu

===========================================================================*/
/*!
    @brief
    Frees a basic SIB6 PDU 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_csp_test_free_sib6_pdu(void)
{
  lte_rrc_osys_asn1_free_pdu(lte_rrc_csp_test_sib6_pdu_ptr,
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
} /* lte_rrc_csp_test_free_sib6_pdu */


/*
sib6_msg:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-FDD 
                  {
                    {
                      carrierFreq 120,
                      threshX-High 0,
                      threshX-Low 0,
                      q-RxLevMin -60,
                      p-MaxUTRA -50,
                      q-QualMin -24
                    },
                    {
                      carrierFreq 130,
                      threshX-High 0,
                      threshX-Low 0,
                      q-RxLevMin -13,
                      p-MaxUTRA 33,
                      q-QualMin 0
                    }
                  },
                  t-ReselectionUTRA 0
                }
            }
          }
      }
}

sib6_msg:CP_EncodedMessageType
{
  msg_type = CP_RRC_MSG;
  pdu_id = 53;
  msg_len = 0;
  msg_buf = "00 11 04 01 E0 00 00 00 00 20 80 0B E9 E0 00 ";
}

*/

/*===========================================================================

   FUNCTION:  te_rrc_csp_test_handle_rrc_cfg_reqi

===========================================================================*/
/*!
	@brief
	To send response of  : LTE_RRC_CFG_SYSTEM_SCAN, LTE_RRC_CFG_BAND_SCAN, LTE_RRC_CFG_ACQ

	@return
	void

*/
/*=========================================================================*/

  void lte_rrc_csp_test_handle_rrc_cfg_req
  (
 	lte_rrc_csp_test_cfg_req_param_s        *cfg_req_param,
    lte_rrc_csp_validate_system_scan_req_s  *validate_system_scan,
	lte_rrc_csp_send_band_scan_cnf_s       *send_band_scan_cnf
  )
  {
	lte_rrc_cfg_reqi_s			*cfg_req_ptr;
	lte_cphy_band_scan_req_s	*band_scan_cfg_ptr;
	lte_cphy_system_scan_req_s	*cphy_system_scan_ptr;
	lte_cphy_acq_req_s			*acq_req_ptr;
	byte                        *buf_ptr;
	uint32                       buf_len;
	uint16                       index;
    lte_rrc_csp_validate_system_scan_req_s  *validate_system_scan_req = (lte_rrc_csp_validate_system_scan_req_s  *)validate_system_scan;
	TF_MSG("Checking for LTE_RRC_CFG_REQI");
	lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
	TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
	
	cfg_req_ptr = (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
	TF_ASSERT(LTE_RRC_CFG_REQI == cfg_req_ptr->msg_hdr.id);
	TF_ASSERT(FALSE == cfg_req_ptr->cnfi_required);
	TF_ASSERT(LTE_RRC_PROC_CSP == cfg_req_ptr->proc);
	TF_ASSERT(1 == cfg_req_ptr->num_cfg);
	
	switch(cfg_req_ptr->cfg_info[0].cfg_type)
	{
		case LTE_RRC_CFG_SYSTEM_SCAN:
		  {
		  	TF_MSG("LTE_RRC_CFG_SYSTEM_SCAN rcvd");
			TF_ASSERT(sizeof(lte_cphy_system_scan_req_s) == sizeof(cfg_req_ptr->cfg_info[0].cfg_u.system_scan_cfg));
			cphy_system_scan_ptr = (lte_cphy_system_scan_req_s*) &(cfg_req_ptr->cfg_info[0].cfg_u.system_scan_cfg); 	


			TF_ASSERT(LTE_CPHY_SYSTEM_SCAN_REQ == cphy_system_scan_ptr->msgr_hdr.id);
			TF_ASSERT(cfg_req_param->trans_id == cphy_system_scan_ptr->trans_id);
			//TF_ASSERT(LTE_CPHY_PARTIAL_ROAMING_ALLOW_NONE == cphy_system_scan_ptr->restriction_cfg);
			for ( index = 0; index < cfg_req_param->num_systems; index++)
			{
			  TF_ASSERT(validate_system_scan_req[index].band == cphy_system_scan_ptr->req_systems.system[index].band);
			  TF_ASSERT(validate_system_scan_req[index].bandwidth	== cphy_system_scan_ptr->req_systems.system[index].bandwidth);
			  TF_ASSERT(validate_system_scan_req[index].earfcn == cphy_system_scan_ptr->req_systems.system[index].earfcn);
			}
			
			lte_rrc_utf_wait_for_done();
			
			TF_MSG("Checking for WT_BAND_SCAN_CNF state");
			TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__WT_SYSTEM_SCAN_CNF);
			TF_MSG("Sending LTE_CPHY_SYSTEM_SCAN_CNF");
			msgr_init_hdr(&ss_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_SYSTEM_SCAN_CNF);
			
			ss_cnf.trans_id = cfg_req_param->trans_id;
			ss_cnf.sys_scan_results.num_systems = cfg_req_param->num_systems;
			for ( index = 0; index < cfg_req_param->num_systems; index++)
			{
			  ss_cnf.sys_scan_results.system[index].energy = cfg_req_param->energy;
			  ss_cnf.sys_scan_results.system[index].system.band = cphy_system_scan_ptr->req_systems.system[index].band;
			  ss_cnf.sys_scan_results.system[index].system.bandwidth = cphy_system_scan_ptr->req_systems.system[index].bandwidth;
			  ss_cnf.sys_scan_results.system[index].system.earfcn = cphy_system_scan_ptr->req_systems.system[index].earfcn;
			}
            lte_rrc_utf_send_msg((byte*)&ss_cnf, sizeof(ss_cnf));
			
			break;
		  }
		case LTE_RRC_CFG_BAND_SCAN:
		  {
			TF_MSG("LTE_RRC_CFG_BAND_SCAN rcvd");
			TF_ASSERT(sizeof(lte_cphy_band_scan_req_s) == sizeof(cfg_req_ptr->cfg_info[0].cfg_u.band_scan_cfg));
			band_scan_cfg_ptr = (lte_cphy_band_scan_req_s *) &(cfg_req_ptr->cfg_info[0].cfg_u.band_scan_cfg);			

			TF_ASSERT(LTE_CPHY_BAND_SCAN_REQ == band_scan_cfg_ptr->msgr_hdr.id);
			TF_ASSERT(1 == band_scan_cfg_ptr->req_bands.num_bands);
			TF_ASSERT(cfg_req_param->band== band_scan_cfg_ptr->req_bands.band[0]);
			TF_ASSERT(cfg_req_param->trans_id== band_scan_cfg_ptr->trans_id);

			for (index = 0; index < send_band_scan_cnf->unusable_systems.num_systems; index++)
			{
			  TF_ASSERT(send_band_scan_cnf->unusable_systems.system[index].band == band_scan_cfg_ptr->req_bands.unusable_systems.system[index].band);
			  TF_ASSERT(send_band_scan_cnf->unusable_systems.system[index].bandwidth == band_scan_cfg_ptr->req_bands.unusable_systems.system[index].bandwidth);
			  TF_ASSERT(send_band_scan_cnf->unusable_systems.system[index].earfcn == band_scan_cfg_ptr->req_bands.unusable_systems.system[index].earfcn);
			}

			lte_rrc_utf_wait_for_done();
			
			TF_MSG("Checking for WT_BAND_SCAN_CNF state");
			TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__WT_BAND_SCAN_CNF);

			
			TF_MSG("Sending LTE_CPHY_BAND_SCAN_CNF");
			msgr_init_hdr(&bs_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_BAND_SCAN_CNF);
			lte_rrc_csp_test_set_band_scan_result_ptr(&bs_cnf);
			bs_cnf.trans_id = cfg_req_param->trans_id;
			
			bs_cnf.num_bands = 1;
			bs_cnf.band_ptr[0].band = cfg_req_param->band;//SYS_SBAND_LTE_EUTRAN_BAND1;
			bs_cnf.band_ptr[0].num_candidates = cfg_req_param->num_candidates;
			for (index = 0; index < cfg_req_param->num_candidates; index++)
			{
			  bs_cnf.band_ptr[0].candidates[index].system.band      = send_band_scan_cnf->candidates[index].system.band;
			  bs_cnf.band_ptr[0].candidates[index].system.bandwidth = send_band_scan_cnf->candidates[index].system.bandwidth;
			  bs_cnf.band_ptr[0].candidates[index].system.earfcn    = send_band_scan_cnf->candidates[index].system.earfcn;
			  bs_cnf.band_ptr[0].candidates[index].energy           = send_band_scan_cnf->candidates[index].energy;
		   }
            lte_rrc_utf_send_msg((byte*)&bs_cnf, sizeof(bs_cnf));

			break;
		  }
		case LTE_RRC_CFG_ACQ:
		  {
			TF_MSG("LTE_RRC_CFG_ACQ rcvd");
			TF_ASSERT(sizeof(lte_cphy_acq_req_s) == sizeof(cfg_req_ptr->cfg_info[0].cfg_u.acq_req_cfg));
			acq_req_ptr = (lte_cphy_acq_req_s *) &(cfg_req_ptr->cfg_info[0].cfg_u.acq_req_cfg);

			TF_ASSERT(LTE_CPHY_ACQ_REQ == acq_req_ptr->msgr_hdr.id);
			TF_ASSERT(cfg_req_param->trans_id == acq_req_ptr->trans_id);
			TF_ASSERT(cfg_req_param->earfcn== acq_req_ptr->earfcn);
			TF_ASSERT(cfg_req_param->band== acq_req_ptr->band);
			TF_ASSERT(cfg_req_param->num_black_cells == acq_req_ptr->num_black_cells);
			
			lte_rrc_utf_wait_for_done();
			
			TF_MSG("Checking for WT_ACQ_CNF state");
			TF_ASSERT(stm_get_state(LTE_RRC_CSP_SM) == LTE_RRC_CSP_SM__WT_ACQ_CNF);
			TF_MSG("Sending LTE_CPHY_ACQ_CNF");
			msgr_init_hdr(&acq_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_ACQ_CNF);
			acq_cnf.trans_id = cfg_req_param->trans_id;
			acq_cnf.status = cfg_req_param->status; 
			acq_cnf.band   = cfg_req_param->band;
			acq_cnf.earfcn = cfg_req_param->earfcn;
			acq_cnf.acquired_cell.cp = LTE_L1_CP_MODE_NORMAL;
			memset(&acq_cnf.acquired_cell.mib,0,sizeof(acq_cnf.acquired_cell.mib));
			acq_cnf.acquired_cell.mib.dl_bandwidth =  cfg_req_param->bandwidth;
			acq_cnf.acquired_cell.phy_cell_id = cfg_req_param->phy_cell_id;
            lte_rrc_utf_send_msg((byte*)&acq_cnf, sizeof(acq_cnf));

			break;
		  }
		default:
		  {
			TF_MSG("Invalid cfg_type");
			TF_ASSERT((cfg_req_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_SYSTEM_SCAN)|
					  (cfg_req_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_BAND_SCAN)|
					  (cfg_req_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_ACQ));
			break;
		  }
	}
}


