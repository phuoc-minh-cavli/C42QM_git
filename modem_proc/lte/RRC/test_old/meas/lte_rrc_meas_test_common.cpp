/*!
  @file lte_rrc_meas_test_common.cpp

  @brief
  Common routines for meas unit tests.

  TODO: Update lte_rrc_meas_test_stub__sib_get_cell_sib to include setting
  the scheduled SIBs bitmask.

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------------------------------------
08/24/11   pk	   Added changes for testing connected mode measurements configuration for IRAT GSM and WCDMA
06/15/10   svk     Modified the name from UTRA to WCDMA
02/26/10   mm      Added changes for testing SIB6, SIB7 and SIB8 in meas
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas 
03/19/09   sk      Added support for rxLevMin and pMax for inter and intra 
                   freq meas
02/23/09   sk      Added support for inter-frequency measurements config
01/12/09   sk      Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <TestFramework.h>
#include <tf_stub.h>

extern "C"
{
#include <lte.h>
#include <lte_rrc_int_msg.h>
#include <lte_rrc_csp.h>
#include <lte_rrc_sib.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_controller.h>
#include <lte_rrc_cre.h> 
#include "lte_static_rrc_mem_util.h"
#include "__lte_rrc_meas.h"
#include "lte_rrc_measi.h"
#include <lte_rrc_cep.h>
#include <lte_rrc_capabilities.h>
#include <lte_rrc_irat_to_tds_mgr.h> 
}

#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  TF_PROTO_STUB(void*, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(lte_rrc_camped_status_e, lte_rrc_csp_get_camped_status, void);
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(boolean, lte_rrc_csp_lte_band_is_supported, sys_sband_lte_e_type);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_wcdma_band_is_supported, sys_sband_e_type);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_tdscdma_band_is_supported, sys_sband_tds_e_type);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_gsm_band_is_supported, sys_sband_e_type);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_cdma_1x_band_is_supported, sys_band_class_e_type);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_cdma_do_band_is_supported, sys_band_class_e_type);
  TF_PROTO_STUB(boolean, lte_rrc_cap_inter_freq_is_supported, void);
  //TF_PROTO_STUB(uint8, lte_rrc_cap_hrpd_rx_config, void);
  //TF_PROTO_STUB(uint8, lte_rrc_cap_1x_rx_config, void);
  TF_PROTO_STUB(stm_state_t, lte_rrc_cep_get_state, void);
  TF_PROTO_STUB(lte_rrc_revoke_priority_e, lte_rrc_get_revoke_priority, lte_rrc_revoke_trigger_e);
  TF_PROTO_STUB( lte_errno_e, lte_rrc_csp_get_selected_plmn, lte_rrc_plmn_s *, uint32 *);
  TF_PROTO_STUB(const lte_rrc_csp_nas_req_info_s *, lte_rrc_csp_get_nas_req_info, void);
  //TF_PROTO_STUB( sys_sband_e_type, lte_rrc_get_wcdma_band_from_uarfcn, lte_uarfcn_t, lte_rrc_plmn_s *);
 // TF_PROTO_STUB( sys_sband_tds_e_type, lte_rrc_get_tdscdma_band_from_uarfcn, lte_uarfcn_t, lte_rrc_plmn_s *);
  TF_PROTO_STUB(boolean, lte_rrc_sib_b12_to_b17_conversion_is_needed, void);
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  //TF_PROTO_STUB(sys_sband_e_type, lte_rrc_get_gsm_band_from_arfcn, geran_grr_band_ind_e, uint16, boolean *);
  TF_PROTO_STUB(boolean, lte_rrc_config_ho_in_progress, void);
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
  TF_PROTO_STUB(boolean, lte_rrc_ctlr_irat_cgi_in_progress, void);
  //TF_PROTO_STUB(boolean, lte_rrc_irat_proc_is_in_progress, void);
#ifdef FEATURE_FEMTO_CSG
  TF_PROTO_STUB(boolean, lte_rrc_csg_asf_check_csg_in_whitelist, const lte_rrc_plmn_s *, sys_csg_id_type, lte_earfcn_t);
  #endif
  TF_PROTO_STUB(boolean, lte_rrc_esmgr_earfcn_in_cfl, lte_earfcn_t);
  //TF_PROTO_STUB(boolean,lte_rrc_csp_cap_tdscdma_is_supported, void);
  //TF_PROTO_STUB(boolean,lte_rrc_csp_cap_wcdma_is_supported, void);
  //TF_PROTO_STUB(void, lte_rrc_controller_get_cphy_rf_status_info, lte_rrc_rf_status_info_s *);
#ifdef FEATURE_LTE_CA
  TF_PROTO_STUB(lte_rrc_cell_type_e, lte_rrc_llc_check_freq_match_with_pcell_scell_freq, lte_earfcn_t, lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *);
#endif
  //TF_PROTO_STUB(boolean, lte_rrc_cap_get_si_req_ho_utra, lte_rrc_div_duplex_e);
  //TF_PROTO_STUB(boolean, lte_rrc_cap_get_si_req_ho_eutra, lte_rrc_div_duplex_e);
  TF_PROTO_STUB(const lte_rrc_irat_to_tds_prev_acq_freq_s *, lte_rrc_irat_get_tds_freq_from_irat_db, void);

#ifdef FEATURE_LTE_CA
  TF_PROTO_STUB(boolean, lte_rrc_is_rel_10_ca_enabled, void);
#endif
}

/*===========================================================================

                         DEFINES

===========================================================================*/

/* Maximum, minimum numbers used for PGSM 900
band */
#define ARFCN_PGSM_900_MIN               1
#define ARFCN_PGSM_900_MAX               124

/* Maximum, minimum numbers used for EGSM 900
band (lower range) */
#define ARFCN_EGSM_900_LOWER_RANGE_MIN   0
#define ARFCN_EGSM_900_LOWER_RANGE_MAX   124

/* Maximum, minimum numbers used for EGSM 900
band (upper range) */
#define ARFCN_EGSM_900_UPPER_RANGE_MIN   975
#define ARFCN_EGSM_900_UPPER_RANGE_MAX   1023

/* Maximum, minimum numbers used for DCS 1800
band */
#define ARFCN_DCS_1800_MIN               512
#define ARFCN_DCS_1800_MAX               885

/* Maximum, minimumnumbers used for PCS 1900
band */
#define ARFCN_PCS_1900_MIN               512
#define ARFCN_PCS_1900_MAX               810

/* Maximum, minimum numbers used for Cellular
850 band */
#define ARFCN_CELL_850_MIN               128
#define ARFCN_CELL_850_MAX               251


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

lte_rrc_osys_SystemInformationBlockType1 *lte_rrc_meas_test_sib1;
lte_rrc_osys_SystemInformationBlockType3 *lte_rrc_meas_test_sib3;
lte_rrc_osys_SystemInformationBlockType4 *lte_rrc_meas_test_sib4;
lte_rrc_osys_SystemInformationBlockType5 *lte_rrc_meas_test_sib5;
lte_rrc_osys_SystemInformationBlockType6 *lte_rrc_meas_test_sib6;
lte_rrc_osys_SystemInformationBlockType7 *lte_rrc_meas_test_sib7;
lte_rrc_osys_SystemInformationBlockType8 *lte_rrc_meas_test_sib8;
lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr;
lte_rrc_osys_RRCConnectionReconfiguration *lte_rrc_meas_test_meas_config;
lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib1_ptr;
lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib3_ptr;
lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib4_ptr;
lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib5_ptr;
lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib6_ptr;
lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib7_ptr;
lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib8_ptr;

lte_rrc_camped_status_e lte_rrc_meas_test_camped_status = LTE_RRC_STATUS_CAMPED_SUITABLE;
stm_state_t lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
stm_state_t lte_rrc_meas_test_cep_state = LTE_RRC_CEP_SM__INACTIVE;
boolean lte_rrc_meas_test_lte_band_is_supported = TRUE;
boolean lte_rrc_meas_test_wcdma_band_is_supported = TRUE;
boolean lte_rrc_meas_test_tdscdma_band_is_supported = TRUE;
boolean lte_rrc_meas_test_gsm_band_is_supported = TRUE;
boolean lte_rrc_meas_test_cdma_1x_band_is_supported = TRUE;
boolean lte_rrc_meas_test_cdma_do_band_is_supported = TRUE;
lte_rrc_cell_id_s lte_rrc_meas_test_cell_id = {0x10, 0x100 };
boolean lte_rrc_meas_test_sib5_sched_and_rcvd = TRUE;
lte_rrc_irat_to_tds_prev_acq_freq_s ret[6];
boolean lte_rrc_meas_test_wcdma_supported = TRUE;
boolean lte_rrc_meas_test_tdscdma_supported = FALSE;

extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas_data;
}
/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr
);

lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params1
(
  lte_rrc_cell_info_s *cell_info_ptr
);


lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params_failure
(
  lte_rrc_cell_info_s *cell_info_ptr
);

lte_rrc_camped_status_e lte_rrc_meas_test_stub__csp_get_camped_status
(
  void
);
stm_state_t lte_rrc_meas_test_stub__controller_get_state
(
  void
);
stm_state_t lte_rrc_meas_test_stub__cep_get_state
(
  void
);
void* lte_rrc_meas_test_stub__sib_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib_bitmask, /*!< Sib requested; LTE_RRC_SIB1_BITMASK=SIB1, LTE_RRC_SIB2_BITMASK=SIB2, ...*/
  lte_rrc_sib_db_key_t sib_db_key /*!< Index in sib_db of the cell */
);

boolean lte_rrc_meas_test_stub__csp_lte_band_is_supported
(
  sys_sband_lte_e_type band
);

boolean lte_rrc_meas_test_stub__csp_wcdma_band_is_supported
(
  sys_sband_e_type band
);

boolean lte_rrc_meas_test_stub__csp_tdscdma_band_is_supported
(
  sys_sband_tds_e_type band
);

boolean lte_rrc_meas_test_stub__csp_gsm_band_is_supported
(
  sys_sband_e_type band
);

boolean lte_rrc_meas_test_stub__csp_cdma_1x_band_is_supported
(
  sys_band_class_e_type band
);

boolean lte_rrc_meas_test_stub__csp_cdma_do_band_is_supported
(
  sys_band_class_e_type band
);

uint8 lte_rrc_meas_test_stub__cap_hrpd_rx_config
(
 void
);

uint8 lte_rrc_meas_test_stub__cap_1x_rx_config
(
 void
);

lte_rrc_revoke_priority_e lte_rrc_meas_test_stub__get_revoke_priority_success
(
  lte_rrc_revoke_trigger_e trigger /*!< Revoke trigger */
);

lte_rrc_revoke_priority_e lte_rrc_meas_test_stub__get_revoke_priority_failure
(
  lte_rrc_revoke_trigger_e trigger /*!< Revoke trigger */
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_lte_band_is_supported

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether band is supported.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csp_lte_band_is_supported
(
  sys_sband_lte_e_type band
)
{

  return lte_rrc_meas_test_lte_band_is_supported;

} /* lte_rrc_meas_test_stub__csp_lte_band_is_supported*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_wcdma_band_is_supported

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether band is supported.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csp_wcdma_band_is_supported
(
  sys_sband_e_type band
)
{

  return lte_rrc_meas_test_wcdma_band_is_supported;

} /* lte_rrc_meas_test_stub__csp_wcdma_band_is_supported*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_tdscdma_band_is_supported

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether band is supported.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csp_tdscdma_band_is_supported
(
  sys_sband_tds_e_type band
)
{

  return lte_rrc_meas_test_tdscdma_band_is_supported;

} /* lte_rrc_meas_test_stub__csp_tdscdma_band_is_supported*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_gsm_band_is_supported

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether band is supported.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csp_gsm_band_is_supported
(
  sys_sband_e_type band
)
{

  return lte_rrc_meas_test_gsm_band_is_supported;

} /* lte_rrc_meas_test_stub__csp_gsm_band_is_supported*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_cdma_1x_band_is_supported

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether band is supported.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csp_cdma_1x_band_is_supported
(
  sys_band_class_e_type band
)
{

  return lte_rrc_meas_test_cdma_1x_band_is_supported;

}/* lte_rrc_meas_test_stub__csp_cdma_1x_band_is_supported*/
/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_cdma_do_band_is_supported

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether band is supported.

  @return
  boolean 
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csp_cdma_do_band_is_supported
(
  sys_band_class_e_type band
)
{
  return lte_rrc_meas_test_cdma_do_band_is_supported;
} /* lte_rrc_meas_test_stub__csp_cdma_do_band_is_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__cap_hrpd_rx_config

===========================================================================*/
/*!
    @brief
    Returns the HRPD rx config
    @return
    Returns the HRPD rx config
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__cap_hrpd_rx_config
(
 void
)
{
  uint8 rx_config =  1;
  return rx_config;
}

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__cap_1x_rx_config

===========================================================================*/
/*!
    @brief
    Returns the 1x rx config
    @return
    Returns the 1x rx config
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__cap_1x_rx_config
(
 void
)
{
  uint8 rx_config =  1;
  return rx_config;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cap_test_stub__inter_freq_is_supported

===========================================================================*/
/*!
  @brief
  Simulates the function in Capabilities

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_rrc_cap_test_stub__inter_freq_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_cap_test_stub__inter_freq_is_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_cap_test_stub__si_req_ho_utra_supported

===========================================================================*/
/*!
  @brief
  Simulates the function in Capabilities

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_rrc_cap_test_stub__si_req_ho_utra_supported
(
  lte_rrc_div_duplex_e serv_cell_mode
)
{
  return TRUE;

} /* lte_rrc_cap_test_stub__si_req_ho_utra_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_cap_test_stub__si_req_ho_eutra_supported

===========================================================================*/
/*!
  @brief
  Simulates the function in Capabilities

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_rrc_cap_test_stub__si_req_ho_eutra_supported
(
  lte_rrc_div_duplex_e serv_cell_mode
)
{
  return TRUE;

} /* lte_rrc_cap_test_stub__si_req_ho_eutra_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_irat_to_tds_mgr_irat_acq_db_stub

===========================================================================*/
/*!
  @brief
  Simulates the function in IRAT TO TDS MGR

  @return
  boolean
*/
/*=========================================================================*/
const lte_rrc_irat_to_tds_prev_acq_freq_s* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub
(
  void
)
{
  return NULL;

} /* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub */

/*===========================================================================

  FUNCTION:  lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_1

===========================================================================*/
/*!
  @brief
  Simulates the function in IRAT TO TDS MGR

  @return
  boolean
*/
/*=========================================================================*/
const lte_rrc_irat_to_tds_prev_acq_freq_s* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_1
(
  void
)
{
  ret[0].uarfcn = 10080;
  ret[0].acq_count = 50;
  ret[1].uarfcn = 11690;
  ret[1].acq_count = 40;
  ret[2].uarfcn = 11830;
  ret[2].acq_count = 40;
  ret[3].uarfcn = 11730;
  ret[3].acq_count = 20;
  ret[4].uarfcn = 11710;
  ret[4].acq_count = 10;
  ret[5].uarfcn = 0;
  ret[5].acq_count = 0; 
  return &ret[0];
} /* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_1 */

/*===========================================================================

  FUNCTION:  lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_2

===========================================================================*/
/*!
  @brief
  Simulates the function in IRAT TO TDS MGR

  @return
  boolean
*/
/*=========================================================================*/
const lte_rrc_irat_to_tds_prev_acq_freq_s* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_2
(
  void
)
{
  ret[0].uarfcn = 10080;
  ret[0].acq_count = 50;
  ret[1].uarfcn = 11750;
  ret[1].acq_count = 40;
  ret[2].uarfcn = 11870;
  ret[2].acq_count = 30;
  ret[3].uarfcn = 11810;
  ret[3].acq_count = 20;
  ret[4].uarfcn = 11715;
  ret[4].acq_count = 15;
  ret[5].uarfcn = 11930;
  ret[5].acq_count = 10; 
  return &ret[0];
} /* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_2 */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__config_ho_in_progress

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether HO is in progress or not.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__config_ho_in_progress
(
  void
)
{

  return FALSE;

} /* lte_rrc_meas_test_stub__config_ho_in_progress*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__irat_proc_is_in_progress

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether IRAT PROC is in progress or not.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__irat_proc_is_in_progress
(
  void
)
{

  return TRUE;

} /* lte_rrc_meas_test_stub__irat_proc_is_in_progress*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csg_in_whitelist

===========================================================================*/
/*!
  @brief
  Returns true for CSG in whitelist function

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csg_in_whitelist
(
  const lte_rrc_plmn_s *plmn_ptr, /*!< PLMN ptr */
  sys_csg_id_type csg_id, /*!< CSG id */
  lte_earfcn_t freq /*!< Freq */
)
{

  return TRUE;

} /* lte_rrc_meas_test_stub__csg_in_whitelist*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_cphy_rf_status_available

===========================================================================*/
/*!
    @brief
    Returns a simulated TRUE for CPHY RF Status

    @return
    void

*/
/*=========================================================================*/

void lte_rrc_meas_test_cphy_rf_status_available(lte_rrc_rf_status_info_s *info_ptr)
{
  info_ptr->rf_status = LTE_RRC_RF_AVAILABLE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_freq_match_with_pcell_scell

===========================================================================*/
/*!
    @brief
    Returns a simulated PCELL for lte_rrc_llc_check_freq_match_with_pcell_scell_freq

    @return
    void

*/
/*=========================================================================*/

lte_rrc_cell_type_e lte_rrc_meas_test_freq_match_with_pcell_scell(lte_earfcn_t freq, lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *reconfig_ptr)
{
  return LTE_RRC_CELL_TYPE_PCELL;
}

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_freq_match_with_pcell_scell

===========================================================================*/
/*!
    @brief
    Returns a simulated CELL NONE for lte_rrc_llc_check_freq_match_with_pcell_scell_freq

    @return
    void

*/
/*=========================================================================*/

lte_rrc_cell_type_e lte_rrc_meas_test_freq_match_with_pcell_scell_none(lte_earfcn_t freq, lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *reconfig_ptr)
{
  return LTE_RRC_CELL_TYPE_NONE;
}




/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csg_not_in_whitelist

===========================================================================*/
/*!
  @brief
  Returns false for CSG in whitelist function

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__csg_not_in_whitelist
(
  const lte_rrc_plmn_s *plmn_ptr, /*!< PLMN ptr */
  sys_csg_id_type csg_id, /*!< CSG id */
  lte_earfcn_t freq /*!< Freq */
)
{

  return FALSE;

} /* lte_rrc_meas_test_stub__csg_not_in_whitelist*/


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__irat_proc_is_in_progress_false

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether IRAT PROC is in progress or not.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__irat_proc_is_in_progress_false
(
  void
)
{

  return FALSE;

} /* lte_rrc_meas_test_stub__irat_proc_is_in_progress_false*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__ctlr_irat_cgi_in_progress

===========================================================================*/
/*!
  @brief
  Returns true or false depending on whether CTRL IRAT CGI is in progress or not.

  @return
  None
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__ctlr_irat_cgi_in_progress
(
  void
)
{

  return TRUE;

} /* lte_rrc_meas_test_stub__ctlr_irat_cgi_in_progress*/

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_mode_fdd_for_last_camped_cell

===========================================================================*/
/*!
  @brief
  Simulates the function that returns mode for the last camped cell

  @return
  lte_rrc_div_duplex_e
*/
/*=========================================================================*/
lte_rrc_div_duplex_e lte_rrc_meas_test_stub__get_mode_fdd_for_last_camped_cell
(
  void
)
{

  return LTE_RRC_FREQ_DIV_DUPLEX;

} /* lte_rrc_meas_test_stub__get_mode_fdd_for_last_camped_cell */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_csp_tdscdma_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_csp_tdscdma_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_meas_test_csp_tdscdma_is_supported */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_csp_wcdma_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_csp_wcdma_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_meas_test_csp_wcdma_is_supported */



/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_mode_tdd_for_last_camped_cell

===========================================================================*/
/*!
  @brief
  Simulates the function that returns mode for the last camped cell

  @return
  lte_rrc_div_duplex_e
*/
/*=========================================================================*/
lte_rrc_div_duplex_e lte_rrc_meas_test_stub__get_mode_tdd_for_last_camped_cell
(
  void
)
{

  return LTE_RRC_TIME_DIV_DUPLEX;

} /* lte_rrc_meas_test_stub__get_mode_tdd_for_last_camped_cell */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_get_meas_sm_to_idle_camped

===========================================================================*/
/*!
  @brief
  Initializes the meas sm and gets it to idle camped state.

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_get_meas_sm_to_idle_camped
(
  void
)
{
  lte_rrc_camped_indi_s         camped_indi        ;
  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

} /* lte_rrc_meas_test_get_meas_sm_to_idle_camped() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_sib1

===========================================================================*/
/*!
  @brief
  Decodes SIB1 from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_sib1
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformationBlockType1);

  lte_rrc_meas_test_sib1 = bcch_msg->message.u.c1->u.systemInformationBlockType1;
  bcch_dl_sch_sib1_ptr = bcch_msg;

} /* lte_rrc_meas_test_decode_sib3() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_sib3

===========================================================================*/
/*!
  @brief
  Decodes SIB3 from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_sib3
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.t ==
            T_lte_rrc_osys_SystemInformation_criticalExtensions_systemInformation_r8);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem != NULL);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.n == 1);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].t == T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib3);
  
  lte_rrc_meas_test_sib3 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib3;
  bcch_dl_sch_sib3_ptr = bcch_msg;

} /* lte_rrc_meas_test_decode_sib3() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_sib4

===========================================================================*/
/*!
  @brief
  Decodes SIB4 from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_sib4
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.t ==
            T_lte_rrc_osys_SystemInformation_criticalExtensions_systemInformation_r8);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem != NULL);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.n == 1);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].t == T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib4);
  
  lte_rrc_meas_test_sib4 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib4;
  bcch_dl_sch_sib4_ptr = bcch_msg;

} /* lte_rrc_meas_test_decode_sib4() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_sib5

===========================================================================*/
/*!
  @brief
  Decodes SIB5 from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_sib5
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.t ==
            T_lte_rrc_osys_SystemInformation_criticalExtensions_systemInformation_r8);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem != NULL);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.n == 1);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].t == T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib5);
  
  lte_rrc_meas_test_sib5 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib5;
  bcch_dl_sch_sib5_ptr = bcch_msg;

} /* lte_rrc_meas_test_decode_sib5() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_sib6

===========================================================================*/
/*!
  @brief
  Decodes SIB6 from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_sib6
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.t ==
            T_lte_rrc_osys_SystemInformation_criticalExtensions_systemInformation_r8);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem != NULL);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.n == 1);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].t == T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib6);
  
  lte_rrc_meas_test_sib6 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib6;
  bcch_dl_sch_sib6_ptr = bcch_msg;

} /* lte_rrc_meas_test_decode_sib6() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_sib7

===========================================================================*/
/*!
  @brief
  Decodes SIB7 from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_sib7
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.t ==
            T_lte_rrc_osys_SystemInformation_criticalExtensions_systemInformation_r8);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem != NULL);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.n == 1);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].t == T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib7);
  
  lte_rrc_meas_test_sib7 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib7;
  bcch_dl_sch_sib7_ptr = bcch_msg;

} /* lte_rrc_meas_test_decode_sib7() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_sib8

===========================================================================*/
/*!
  @brief
  Decodes SIB8 from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_sib8
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.t ==
            T_lte_rrc_osys_SystemInformation_criticalExtensions_systemInformation_r8);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->m.nonCriticalExtensionPresent == 0);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem != NULL);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.n == 1);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].t == T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib8);
  
  lte_rrc_meas_test_sib8 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem[0].u.sib8;
  bcch_dl_sch_sib8_ptr = bcch_msg;

} /* lte_rrc_meas_test_decode_sib8() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_si

===========================================================================*/
/*!
  @brief
  Decodes SIs from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_si
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
)
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_msg;
  lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo *si_info_ptr;
  int decode_status;
  uint8 i = 0, num_si_msg = 0;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);

  /*-----------------------------------------------------------------------*/

  bcch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(bcch_msg != NULL);
  TF_ASSERT(bcch_msg->message.t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1);
  TF_ASSERT(bcch_msg->message.u.c1->t == T_lte_rrc_osys_BCCH_DL_SCH_MessageType_c1_systemInformation);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.t ==
            T_lte_rrc_osys_SystemInformation_criticalExtensions_systemInformation_r8);
  TF_ASSERT(bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->m.nonCriticalExtensionPresent == 0);

  si_info_ptr = &bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo;
  num_si_msg = si_info_ptr->n;
  for (i = 0; i < num_si_msg; i++)
  {
    switch (si_info_ptr->elem[i].t)
    {
      case T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib3:
        lte_rrc_meas_test_sib3 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.
          u.systemInformation_r8->sib_TypeAndInfo.elem[i].u.sib3;
        bcch_dl_sch_sib3_ptr = bcch_msg;
        break;
      case T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib4:
        lte_rrc_meas_test_sib4 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.
          u.systemInformation_r8->sib_TypeAndInfo.elem[i].u.sib4;
        bcch_dl_sch_sib4_ptr = bcch_msg;
        break;
      case T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib5:
        lte_rrc_meas_test_sib5 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.
          u.systemInformation_r8->sib_TypeAndInfo.elem[i].u.sib5;
        bcch_dl_sch_sib5_ptr = bcch_msg;
        break;
      case T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib6:
        lte_rrc_meas_test_sib6 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.
          u.systemInformation_r8->sib_TypeAndInfo.elem[i].u.sib6;
        bcch_dl_sch_sib6_ptr = bcch_msg;
        break;
      case T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib7:
        lte_rrc_meas_test_sib7 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.
          u.systemInformation_r8->sib_TypeAndInfo.elem[i].u.sib7;
        bcch_dl_sch_sib7_ptr = bcch_msg;
        break;
      case T_lte_rrc_osys_SystemInformation_r8_IEs_sib_TypeAndInfo_element_sib8:
        lte_rrc_meas_test_sib8 = bcch_msg->message.u.c1->u.systemInformation->criticalExtensions.
          u.systemInformation_r8->sib_TypeAndInfo.elem[i].u.sib8;
        bcch_dl_sch_sib8_ptr = bcch_msg;
        break;
      default:
        break;
    }
  }

} /* lte_rrc_meas_test_decode_si() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_decode_meas_config

===========================================================================*/
/*!
  @brief
  Decodes MEAS_CONFIG from passed in encoded string

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_decode_meas_config
(
  void *encoded_bytes,   /*!< Ptr to encoded bytes */
  uint32 encoded_length, /*!< Length of encoded string (in bytes) */
  lte_rrc_osys_MeasConfig **vmc_ptr
)
{
  lte_rrc_osys_DL_DCCH_Message *dcch_msg;
  int decode_status;

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(encoded_bytes != NULL);
  TF_ASSERT(encoded_length != 0);
  TF_ASSERT(vmc_ptr != NULL);

  /*-----------------------------------------------------------------------*/

  dcch_msg = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*) encoded_bytes,
                               encoded_length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(dcch_msg != NULL);

  TF_ASSERT(dcch_msg->message.t == T_lte_rrc_osys_DL_DCCH_MessageType_c1);
  TF_ASSERT(dcch_msg->message.u.c1->t == T_lte_rrc_osys_DL_DCCH_MessageType_c1_rrcConnectionReconfiguration);
  TF_ASSERT(dcch_msg->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.t == T_lte_rrc_osys_RRCConnectionReconfiguration_criticalExtensions_c1);
  TF_ASSERT(dcch_msg->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->t == T_lte_rrc_osys_RRCConnectionReconfiguration_criticalExtensions_c1_rrcConnectionReconfiguration_r8);

  *vmc_ptr = &dcch_msg->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->measConfig;
  
  lte_rrc_meas_test_meas_config = dcch_msg->message.u.c1->u.rrcConnectionReconfiguration;

} /* lte_rrc_meas_test_decode_meas_config() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_get_camped_status

===========================================================================*/
/*!
  @brief
  Stub func for csp_get_camped_status

  @return
  Status code.
*/
/*=========================================================================*/
lte_rrc_camped_status_e lte_rrc_meas_test_stub__csp_get_camped_status
(
  void
)
{
  return lte_rrc_meas_test_camped_status;

}/* lte_rrc_meas_test_stub__csp_get_camped_status */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__controller_get_state

===========================================================================*/
/*!
  @brief
  Stub func for csp_get_camped_status

  @return
  Status code.
*/
/*=========================================================================*/
stm_state_t lte_rrc_meas_test_stub__controller_get_state
(
  void
)
{

  return lte_rrc_meas_test_ctrl_state;

} /* lte_rrc_meas_test_stub__controller_get_state */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__cep_get_state

===========================================================================*/
/*!
  @brief
  Stub func for csp_get_camped_status

  @return
  Status code.
*/
/*=========================================================================*/
stm_state_t lte_rrc_meas_test_stub__cep_get_state
(
  void
)
{

  return lte_rrc_meas_test_cep_state;

} /* lte_rrc_meas_test_stub__cep_get_state */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_get_serving_cell_params

===========================================================================*/
/*!
  @brief
  Stub func for csp_get_serving_cell_params

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr
)
{
  /*-----------------------------------------------------------------------*/

  TF_ASSERT(cell_info_ptr != NULL);

  /*-----------------------------------------------------------------------*/

  // Fill in some bogus values
  cell_info_ptr->cell_id.phy_cell_id = lte_rrc_meas_test_cell_id.phy_cell_id;
  cell_info_ptr->cell_id.freq = lte_rrc_meas_test_cell_id.freq;

  return E_SUCCESS;

} /* lte_rrc_meas_test_stub__csp_get_serving_cell_params() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_get_serving_cell_params1

===========================================================================*/
/*!
  @brief
  Stub func for csp_get_serving_cell_params

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params1
(
  lte_rrc_cell_info_s *cell_info_ptr
)
{
  /*-----------------------------------------------------------------------*/

  TF_ASSERT(cell_info_ptr != NULL);

  /*-----------------------------------------------------------------------*/

  // Fill in some bogus values
  cell_info_ptr->cell_id.phy_cell_id = 17;
  cell_info_ptr->cell_id.freq = 256;

  return E_SUCCESS;

} /* lte_rrc_meas_test_stub__csp_get_serving_cell_params1() */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__csp_get_serving_cell_params_failure

===========================================================================*/
/*!
  @brief
  Stub func for csp_get_serving_cell_params

  @return
  E_FAILURE
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params_failure
(
  lte_rrc_cell_info_s *cell_info_ptr
)
{

  TF_ASSERT(cell_info_ptr != NULL);

  return E_FAILURE;

} /* lte_rrc_meas_test_stub__csp_get_serving_cell_params_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__sib_get_cell_sib

===========================================================================*/
/*!
  @brief
  Stub function for sib_get_cell_sib

  @return
  Ptr to sib requested
*/
/*=========================================================================*/
void* lte_rrc_meas_test_stub__sib_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib_bitmask, /*!< Sib requested; LTE_RRC_SIB1_BITMASK=SIB1, LTE_RRC_SIB2_BITMASK=SIB2, ...*/
  lte_rrc_sib_db_key_t sib_db_key /*!< Index in sib_db of the cell */
)
{

  /*-----------------------------------------------------------------------*/

  TF_ASSERT((sib_bitmask == LTE_RRC_SIB1_BITMASK) ||
            (sib_bitmask == LTE_RRC_SIB3_BITMASK) ||
            (sib_bitmask == LTE_RRC_SIB4_BITMASK) ||
            (sib_bitmask == LTE_RRC_SIB5_BITMASK) ||
            (sib_bitmask == LTE_RRC_SIB6_BITMASK) ||
            (sib_bitmask == LTE_RRC_SIB7_BITMASK) ||
            (sib_bitmask == LTE_RRC_SIB8_BITMASK));

  /*-----------------------------------------------------------------------*/

  if(sib_bitmask == LTE_RRC_SIB1_BITMASK)
    return lte_rrc_meas_test_sib1;
  else if(sib_bitmask == LTE_RRC_SIB3_BITMASK)
    return lte_rrc_meas_test_sib3;
  else if(sib_bitmask == LTE_RRC_SIB4_BITMASK)
    return lte_rrc_meas_test_sib4;
  else if(sib_bitmask == LTE_RRC_SIB5_BITMASK)
    if (lte_rrc_meas_test_sib5_sched_and_rcvd)
    {
    return lte_rrc_meas_test_sib5;
    }
    else
    {
      return NULL;
    }
  else if(sib_bitmask == LTE_RRC_SIB6_BITMASK)
    return lte_rrc_meas_test_sib6;
  else if(sib_bitmask == LTE_RRC_SIB7_BITMASK)
    return lte_rrc_meas_test_sib7;
  else if(sib_bitmask == LTE_RRC_SIB8_BITMASK)
    return lte_rrc_meas_test_sib8;
  else 
    return NULL;

} /* lte_rrc_meas_test_stub__sib_get_cell_sib() */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_revoke_priority_success

===========================================================================*/
/*!
  @brief
  Stub func for get_revoke_priority

  @return
  Status code.
*/
/*=========================================================================*/
lte_rrc_revoke_priority_e lte_rrc_meas_test_stub__get_revoke_priority_success
(
  lte_rrc_revoke_trigger_e trigger /*!< Revoke trigger */
)
{

  return LTE_RRC_REVOKE_PRIORITY_NOW;

} /* lte_rrc_meas_test_stub__get_revoke_priority_success */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_revoke_priority_failure

===========================================================================*/
/*!
  @brief
  Stub func for get_revoke_priority

  @return
  Status code.
*/
/*=========================================================================*/
lte_rrc_revoke_priority_e lte_rrc_meas_test_stub__get_revoke_priority_failure
(
  lte_rrc_revoke_trigger_e trigger /*!< Revoke trigger */
)
{

  return LTE_RRC_REVOKE_PRIORITY_NOT_REQD;

} /* lte_rrc_meas_test_stub__get_revoke_priority_failure */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_selected_plmn

===========================================================================*/
/*!
  @brief
  Stub func for getting selected PLMN

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
)
{
  selected_plmn_ptr->mcc[0] = 0;
  selected_plmn_ptr->mcc[1] = 9;
  selected_plmn_ptr->mcc[2] = 1;
  selected_plmn_ptr->num_mnc_digits = 3;
  selected_plmn_ptr->mnc[0] = 7;
  selected_plmn_ptr->mnc[1] = 1;
  selected_plmn_ptr->mnc[2] = 4;
  
  *selected_plmn_index_ptr = 0;

  return E_SUCCESS;

} /* lte_rrc_meas_test_stub__get_selected_plmn */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_selected_plmn_for_depri

===========================================================================*/
/*!
  @brief
  Stub func for getting selected PLMN

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
)
{
  selected_plmn_ptr->mcc[0] = 0;
  selected_plmn_ptr->mcc[1] = 8;
  selected_plmn_ptr->mcc[2] = 1;
  selected_plmn_ptr->num_mnc_digits = 3;
  selected_plmn_ptr->mnc[0] = 6;
  selected_plmn_ptr->mnc[1] = 1;
  selected_plmn_ptr->mnc[2] = 4;
  
  *selected_plmn_index_ptr = 0;

  return E_SUCCESS;

} /* lte_rrc_meas_test_stub__get_selected_plmn_for_depri */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_selected_plmn_for_depri1

===========================================================================*/
/*!
  @brief
  Stub func for getting selected PLMN

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri1
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
)
{
  selected_plmn_ptr->mcc[0] = 1;
  selected_plmn_ptr->mcc[1] = 0;
  selected_plmn_ptr->mcc[2] = 2;
  selected_plmn_ptr->num_mnc_digits = 3;
  selected_plmn_ptr->mnc[0] = 2;
  selected_plmn_ptr->mnc[1] = 0;
  selected_plmn_ptr->mnc[2] = 2;
  
  *selected_plmn_index_ptr = 0;

  return E_SUCCESS;

} /* lte_rrc_meas_test_stub__get_selected_plmn_for_depri1 */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_selected_plmn_for_depri2

===========================================================================*/
/*!
  @brief
  Stub func for getting selected PLMN

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri2
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
)
{
  selected_plmn_ptr->mcc[0] = 1;
  selected_plmn_ptr->mcc[1] = 0;
  selected_plmn_ptr->mcc[2] = 0;
  selected_plmn_ptr->num_mnc_digits = 3;
  selected_plmn_ptr->mnc[0] = 2;
  selected_plmn_ptr->mnc[1] = 0;
  selected_plmn_ptr->mnc[2] = 0;
  
  *selected_plmn_index_ptr = 0;

  return E_SUCCESS;

} /* lte_rrc_meas_test_stub__get_selected_plmn_for_depri2 */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_selected_plmn_for_depri3

===========================================================================*/
/*!
  @brief
  Stub func for getting selected PLMN

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri3
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
)
{
  selected_plmn_ptr->mcc[0] = 5;
  selected_plmn_ptr->mcc[1] = 0;
  selected_plmn_ptr->mcc[2] = 0;
  selected_plmn_ptr->num_mnc_digits = 3;
  selected_plmn_ptr->mnc[0] = 6;
  selected_plmn_ptr->mnc[1] = 0;
  selected_plmn_ptr->mnc[2] = 0;
  
  *selected_plmn_index_ptr = 0;

  return E_SUCCESS;

} /* lte_rrc_meas_test_stub__get_selected_plmn_for_depri3 */



/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_nas_req_info

===========================================================================*/
/*!
  @brief
  Stub func for getting selected PLMN

  @return
  Status code.
*/
/*=========================================================================*/
const lte_rrc_csp_nas_req_info_s * lte_rrc_meas_test_stub__get_nas_req_info
(
  void
)
{
  lte_rrc_csp_nas_req_info_s nas_req_info;

  nas_req_info.eplmn_list.num_plmns = 0;

  nas_req_info.req_plmn_info_is_valid = TRUE;
  nas_req_info.req_plmn.mcc[0] = 0;
  nas_req_info.req_plmn.mcc[1] = 9;
  nas_req_info.req_plmn.mcc[2] = 1;
  nas_req_info.req_plmn.num_mnc_digits = 3;
  nas_req_info.req_plmn.mnc[0] = 7;
  nas_req_info.req_plmn.mnc[1] = 1;
  nas_req_info.req_plmn.mnc[2] = 4;

  nas_req_info.rplmn_info_is_valid = TRUE;
  nas_req_info.rplmn.mcc[0] = 0;
  nas_req_info.rplmn.mcc[1] = 9;
  nas_req_info.rplmn.mcc[2] = 1;
  nas_req_info.rplmn.num_mnc_digits = 3;
  nas_req_info.rplmn.mnc[0] = 7;
  nas_req_info.rplmn.mnc[1] = 1;
  nas_req_info.rplmn.mnc[2] = 4;
  
  return &nas_req_info;

} /* lte_rrc_meas_test_stub__get_nas_req_info */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_wcdma_band_from_uarfcn

===========================================================================*/
/*!
    @brief
    This function frees the allcoated SIB heap memory used in unit tests.

    @return
    void

 */
/*=========================================================================*/
sys_sband_e_type lte_rrc_meas_test_stub__get_wcdma_band_from_uarfcn
(
   lte_uarfcn_t dl_uarfcn, /*!< DL UARFCN */
   lte_rrc_plmn_s *camped_plmn_ptr /*!< Ptr to camped PLMN */
)
{

  sys_sband_e_type wcdma_band = SYS_SBAND_WCDMA_I_IMT_2000; /* default band */

  sys_plmn_id_s_type sys_plmn_id;
  sys_band_mask_type wcdma_band_mask;
  boolean wcdma_band_valid = FALSE;

  LTE_ASSERT(camped_plmn_ptr != NULL);

  sys_plmn_id.identity[0] = (byte) camped_plmn_ptr->mcc[1];
  sys_plmn_id.identity[0] <<= 4;
  sys_plmn_id.identity[0] |= (byte) camped_plmn_ptr->mcc[0];

  if (camped_plmn_ptr->num_mnc_digits == 3)
  {
    sys_plmn_id.identity[1] = (byte) camped_plmn_ptr->mnc[2];
    sys_plmn_id.identity[1] <<= 4;
    sys_plmn_id.identity[1] |= (byte) camped_plmn_ptr->mcc[2];
  }
  else
  {
    sys_plmn_id.identity[1] = (byte) camped_plmn_ptr->mcc[2];
  }

  sys_plmn_id.identity[2] = (byte) camped_plmn_ptr->mnc[1];
  sys_plmn_id.identity[2] <<= 4;
  sys_plmn_id.identity[2] |= (byte) camped_plmn_ptr->mnc[0];

  wcdma_band_mask = SYS_BM_64BIT(SYS_SBAND_WCDMA_I_IMT_2000);
  wcdma_band_valid = TRUE;

  TF_ASSERT(sys_plmn_id.identity[0] == 144);
  TF_ASSERT(sys_plmn_id.identity[1] == 65);
  TF_ASSERT(sys_plmn_id.identity[2] == 23);

  if (wcdma_band_valid)
  {
    if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_I_IMT_2000))
    {
      wcdma_band = SYS_SBAND_WCDMA_I_IMT_2000;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_II_PCS_1900))
    {
      wcdma_band = SYS_SBAND_WCDMA_II_PCS_1900;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_III_1700))
    {
      wcdma_band = SYS_SBAND_WCDMA_III_1700;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_IV_1700))
    {
      wcdma_band = SYS_SBAND_WCDMA_IV_1700;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_V_850))
    {
      wcdma_band = SYS_SBAND_WCDMA_V_850;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_VI_800))
    {
      wcdma_band = SYS_SBAND_WCDMA_VI_800;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_VII_2600))
    {
      wcdma_band = SYS_SBAND_WCDMA_VII_2600;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_VIII_900))
    {
      wcdma_band = SYS_SBAND_WCDMA_VIII_900;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_IX_1700))
    {
      wcdma_band = SYS_SBAND_WCDMA_IX_1700;
    }
    else if (wcdma_band_mask & SYS_BM_64BIT(SYS_SBAND_WCDMA_XI_1500))
    {
      wcdma_band = SYS_SBAND_WCDMA_XI_1500;
    }
  }

  return wcdma_band;

} /* lte_rrc_meas_test_stub__get_wcdma_band_from_uarfcn */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_tdscdma_band_from_uarfcn

===========================================================================*/
/*!
    @brief
    This function frees the allcoated SIB heap memory used in unit tests.

    @return
    void

 */
/*=========================================================================*/
sys_sband_tds_e_type lte_rrc_meas_test_stub__get_tdscdma_band_from_uarfcn
(
   lte_uarfcn_t dl_uarfcn, /*!< DL UARFCN */
   lte_rrc_plmn_s *camped_plmn_ptr /*!< Ptr to camped PLMN */
)
{

  sys_sband_tds_e_type tdscdma_band = SYS_SBAND_TDS_BAND_MAX; /* default band */
  sys_band_mask_type tdscdma_band_mask;

  sys_plmn_id_s_type sys_plmn_id;
  boolean tdscdma_band_valid = FALSE;

  LTE_ASSERT(camped_plmn_ptr != NULL);

  sys_plmn_id.identity[0] = (byte) camped_plmn_ptr->mcc[1];
  sys_plmn_id.identity[0] <<= 4;
  sys_plmn_id.identity[0] |= (byte) camped_plmn_ptr->mcc[0];

  if (camped_plmn_ptr->num_mnc_digits == 3)
  {
    sys_plmn_id.identity[1] = (byte) camped_plmn_ptr->mnc[2];
    sys_plmn_id.identity[1] <<= 4;
    sys_plmn_id.identity[1] |= (byte) camped_plmn_ptr->mcc[2];
  }
  else
  {
    sys_plmn_id.identity[1] = (byte) camped_plmn_ptr->mcc[2];
  }

  sys_plmn_id.identity[2] = (byte) camped_plmn_ptr->mnc[1];
  sys_plmn_id.identity[2] <<= 4;
  sys_plmn_id.identity[2] |= (byte) camped_plmn_ptr->mnc[0];

  tdscdma_band_mask = SYS_BM_64BIT(SYS_SBAND_TDS_BANDA);
  tdscdma_band_valid = TRUE;

  TF_ASSERT(sys_plmn_id.identity[0] == 144);
  TF_ASSERT(sys_plmn_id.identity[1] == 65);
  TF_ASSERT(sys_plmn_id.identity[2] == 23);

  if (tdscdma_band_valid)
  {
    if (tdscdma_band_mask & SYS_BM_64BIT(SYS_SBAND_TDS_BANDA))
    {
      tdscdma_band = SYS_SBAND_TDS_BANDA;
    }
    else if (tdscdma_band_mask & SYS_BM_64BIT(SYS_SBAND_TDS_BANDB))
    {
      tdscdma_band = SYS_SBAND_TDS_BANDB;
    }
    else if (tdscdma_band_mask & SYS_BM_64BIT(SYS_SBAND_TDS_BANDC))
    {
      tdscdma_band = SYS_SBAND_TDS_BANDC;
    }
    else if (tdscdma_band_mask & SYS_BM_64BIT(SYS_SBAND_TDS_BANDD))
    {
      tdscdma_band = SYS_SBAND_TDS_BANDD;
    }
    else if (tdscdma_band_mask & SYS_BM_64BIT(SYS_SBAND_TDS_BANDE))
    {
      tdscdma_band = SYS_SBAND_TDS_BANDE;
    }
    else if (tdscdma_band_mask & SYS_BM_64BIT(SYS_SBAND_TDS_BANDF))
    {
      tdscdma_band = SYS_SBAND_TDS_BANDF;
    }
  }

  return tdscdma_band;

} /* lte_rrc_meas_test_stub__get_tdscdma_band_from_uarfcn */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__b12_to_b17_conversion_is_needed

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__b12_to_b17_conversion_is_needed
(
  void
)
{
  return TRUE;

} /* lte_rrc_meas_test_stub__b12_to_b17_conversion_is_needed() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__b12_to_b17_conversion_is_not_needed

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__b12_to_b17_conversion_is_not_needed
(
  void
)
{
  return FALSE;

} /* lte_rrc_meas_test_stub__b12_to_b17_conversion_is_not_needed() */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_fgi_bit

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__get_fgi_bit
(
  uint8 bit_pos
)
{
  return 1;

} /* lte_rrc_meas_test_stub__get_fgi_bit() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_fgi_bit_not_supported

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__get_fgi_bit_not_supported
(
  uint8 bit_pos
)
{
  return 0;

} /* lte_rrc_meas_test_stub__get_fgi_bit_not_supported() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_fgi_bit_not_supported_except_utran_bit

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__get_fgi_bit_not_supported_except_utran_bit
(
  uint8 bit_pos
)
{
  if ((bit_pos == LTE_RRC_CAP_FGI_BIT_22) ||
      (bit_pos == LTE_RRC_CAP_FGI_BIT_33) ||
      (bit_pos == LTE_RRC_CAP_FGI_BIT_19))
  {
    return 1;
  }
  else
  {
    return 0;
  }

} /* lte_rrc_meas_test_stub__get_fgi_bit_not_supported_except_utran_bit() */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_irat_fgi_bit_not_supported

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__get_irat_fgi_bit_not_supported
(
  uint8 bit_pos
)
{
  if ((bit_pos == LTE_RRC_CAP_FGI_BIT_22) ||
      (bit_pos == LTE_RRC_CAP_FGI_BIT_23) ||
      (bit_pos == LTE_RRC_CAP_FGI_BIT_24) ||
      (bit_pos == LTE_RRC_CAP_FGI_BIT_26))
  {
    return 0;
  }
  else
  {
    return 1;
  }

} /* lte_rrc_meas_test_stub__get_irat_fgi_bit_not_supported() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_fgi_bit_cgi_not_supported

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__get_fgi_bit_cgi_not_supported
(
  uint8 bit_pos
)
{
  if (bit_pos == LTE_RRC_CAP_FGI_BIT_17)
  {
    return 0;
  }
  else
  {
    return 1;
  }

} /* lte_rrc_meas_test_stub__get_fgi_bit_cgi_not_supported() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_fgi_bit_son_not_supported

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_meas_test_stub__get_fgi_bit_son_not_supported
(
  uint8 bit_pos
)
{
  if (bit_pos == LTE_RRC_CAP_FGI_BIT_19)
  {
    return 0;
  }
  else
  {
    return 1;
  }

} /* lte_rrc_meas_test_stub__get_fgi_bit_son_not_supported() */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_free_sib_ptr

===========================================================================*/
/*!
    @brief
    This function frees the allcoated SIB heap memory used in unit tests.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_meas_test_free_sib_ptr
(
  void
)
{

  if (bcch_dl_sch_sib3_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu(bcch_dl_sch_sib3_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    bcch_dl_sch_sib3_ptr = NULL; 
  }

  if (bcch_dl_sch_sib4_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu( bcch_dl_sch_sib4_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    bcch_dl_sch_sib4_ptr = NULL; 
  }

  if (bcch_dl_sch_sib5_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu( bcch_dl_sch_sib5_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    bcch_dl_sch_sib5_ptr = NULL; 
  }

  if (bcch_dl_sch_sib6_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu( bcch_dl_sch_sib6_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    bcch_dl_sch_sib6_ptr = NULL; 
  }

  if (bcch_dl_sch_sib7_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu( bcch_dl_sch_sib7_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    bcch_dl_sch_sib7_ptr = NULL; 
  }

  if (bcch_dl_sch_sib8_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu( bcch_dl_sch_sib8_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    bcch_dl_sch_sib8_ptr = NULL; 
  }

}

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_free_unused_memory_in_sib8

===========================================================================*/
/*!
    @brief
    This function frees the allcoated memory in SIB8 mesg1 that is unused in a 
    given unit test and will not be freed by asn1_free_pdu.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_meas_test_free_unused_memory_in_sib8
(
  boolean sys_time_memory,
  boolean free_hrpd_memory,
  boolean free_1x_memory
)
{
  // Free all allocated but unused memory

  if (free_hrpd_memory)
  {
    // Free memory allocated for HRPD 
    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     bandClassList.elem);
    lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     bandClassList.n = 0;



    // Free memory for neighbor cells in bc1
    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.elem[1].neighCellsPerFreqList.elem[1].physCellIdList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.elem[1].neighCellsPerFreqList.elem[0].physCellIdList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.elem[1].neighCellsPerFreqList.elem);

    // Free memory for neighbor cells in bc0
    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.elem[0].neighCellsPerFreqList.elem[1].physCellIdList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.elem[0].neighCellsPerFreqList.elem[0].physCellIdList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.elem[0].neighCellsPerFreqList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.elem);
    lte_rrc_meas_test_sib8->parametersHRPD.cellReselectionParametersHRPD.
                     neighCellList.n = 0;
  }

  if (free_1x_memory)
  {
    // Free memory allocated for 1x
    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     bandClassList.elem);
    lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     bandClassList.n = 0;
    
    // Free memory for neighbor cells in bc17
    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     neighCellList.elem[1].neighCellsPerFreqList.elem[0].physCellIdList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     neighCellList.elem[1].neighCellsPerFreqList.elem);
    
    // Free memory for neighbor cells in bc2
    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     neighCellList.elem[0].neighCellsPerFreqList.elem[1].physCellIdList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     neighCellList.elem[0].neighCellsPerFreqList.elem[0].physCellIdList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     neighCellList.elem[0].neighCellsPerFreqList.elem);

    lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     neighCellList.elem);
    lte_rrc_meas_test_sib8->parameters1XRTT.cellReselectionParameters1XRTT.
                     neighCellList.n = 0;
  }

} /* lte_rrc_meas_test_free_unused_memory_in_sib8 */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_free_reconfig_msg_ptr

===========================================================================*/
/*!
    @brief
    This function frees the allcoated memory for config message used in unit tests.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_meas_test_free_reconfig_msg_ptr
(
  void
)
{

  if (lte_rrc_meas_test_meas_config != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu( lte_rrc_meas_test_meas_config, 
                               lte_rrc_osys_DL_DCCH_Message_PDU);
    lte_rrc_meas_test_meas_config = NULL; 
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_verify_serving_cell_info

===========================================================================*/
/*!
  @brief
  Function to verify serving cell info

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_serving_cell_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
)
{
  TF_ASSERT(cfg_req_ptr != NULL);
  TF_ASSERT(cfg_req_ptr->serving_cell_info.phy_cell_id == lte_rrc_meas_test_cell_id.phy_cell_id);
  TF_ASSERT(cfg_req_ptr->serving_cell_info.dl_cell_frequency == lte_rrc_meas_test_cell_id.freq);
}
/* lte_rrc_verify_serving_cell_info */

/*===========================================================================

  FUNCTION:  lte_rrc_verify_common_resel_info

===========================================================================*/
/*!
  @brief
  Function to verify common reselection info

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_common_resel_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
)
{
  TF_ASSERT(cfg_req_ptr != NULL);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);
}
/* lte_rrc_verify_common_resel_info */

/*===========================================================================

  FUNCTION:  lte_rrc_verify_serving_freq_info

===========================================================================*/
/*!
  @brief
  Function to verify serving frequency info

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_serving_freq_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
)
{
  TF_ASSERT(cfg_req_ptr != NULL);
  TF_ASSERT(cfg_req_ptr->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req_ptr->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req_ptr->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);
}
/* lte_rrc_verify_serving_freq_info */

/*===========================================================================

  FUNCTION:  lte_rrc_verify_default_sib3_param

===========================================================================*/
/*!
  @brief
  Function to verify the default SIB3 parameters

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_default_sib3_param
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
)
{
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == FALSE);
  TF_ASSERT(cfg_req_ptr->meas_cfg.resel_common_info_ptr->q_hyst == 0); 
  TF_ASSERT(cfg_req_ptr->meas_cfg.serving_freq_info_ptr->cell_resel_priority == LTE_CPHY_MEAS_RESEL_PRIORITY_NONE);
  TF_ASSERT(cfg_req_ptr->meas_cfg.serving_freq_info_ptr->s_non_intra_search 
            == LTE_CPHY_MEAS_SRCH_THRESHOLD_INVALID);
}
/* lte_rrc_verify_default_sib3_param */


/*===========================================================================

  FUNCTION:  lte_rrc_verify_intra_freq_info

===========================================================================*/
/*!
  @brief
  Function to verify intra frequency info

  @return
  None
*/
/*=========================================================================*/
void  lte_rrc_verify_intra_freq_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
)
{
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);

  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->antenna_port_1 == FALSE);
  TF_ASSERT(cfg_req_ptr->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);
}/* lte_rrc_verify_intra_freq_info */
#if 0
/*===========================================================================

  FUNCTION:  lte_rrc_verify_cdma_bandclass_info

===========================================================================*/
/*!
  @brief
  Function to verify CDMA bandclass info

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_cdma_bandclass_info
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
)
{
  TF_ASSERT(cdma_info_ptr != NULL);

  // Verify bandclass list for eHRPD, if present
  if (cdma_info_ptr->irat_cdma_ehrpd_resel_enabled)
  {
    // Validate bandclass list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list_size == 2);

    // Validate first bandclass in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              cell_resel_priority == 1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              threshX_high == 60);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              threshX_low == 10);
    
    // Validate next bandclass in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].
              cell_resel_priority == 2);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].
              threshX_high == 60);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[1].
              threshX_low == 25);
  }

  // Verify bandclass list for 1X, if present
  if (cdma_info_ptr->irat_cdma_1x_resel_enabled)
  {
    // Validate bandclass list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list_size == 1);
    // Validate first bandclass in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              cell_resel_priority == 3);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              threshX_high == 63);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              threshX_low == 0);

  }
}
/* lte_rrc_verify_cdma_bandclass_info */

/*===========================================================================

  FUNCTION:  lte_rrc_verify_cdma_bandclass_invalid

===========================================================================*/
/*!
  @brief
  Function to verify CDMA bandclass info where some bandclass entries are invalid

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_cdma_bandclass_invalid
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
)
{
  TF_ASSERT(cdma_info_ptr != NULL);

  // Verify bandclass list for eHRPD, if present
  if (cdma_info_ptr->irat_cdma_ehrpd_resel_enabled)
  {
    // Validate bandclass list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list_size == 1);

    // Validate next bandclass in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              cell_resel_priority == 2);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              threshX_high == 60);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.band_class_list[0].
              threshX_low == 25);
  }

  // Verify bandclass list for 1X, if present
  if (cdma_info_ptr->irat_cdma_1x_resel_enabled)
  {
    // Validate bandclass list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list_size == 1);
    // Validate first bandclass in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              cell_resel_priority == 3);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              threshX_high == 63);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.band_class_list[0].
              threshX_low == 0);
  }
}
/* lte_rrc_verify_cdma_bandclass_invalid */

/*===========================================================================

  FUNCTION:  lte_rrc_verify_cdma_neigh_cell_list

===========================================================================*/
/*!
  @brief
  Function to verify CDMA neighbor cell list

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_cdma_neigh_cell_list
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
)
{
  TF_ASSERT(cdma_info_ptr != NULL);

  // Verify neighbor cell list for eHRPD, if present
  if (cdma_info_ptr->irat_cdma_ehrpd_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list_size == 8);

    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              channel == 10);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              cell_id == 10);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              channel == 10);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              cell_id == 15);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              channel == 20);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              cell_id == 20);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              channel == 20);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              cell_id == 25);

    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[4].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[4].
              channel == 30);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[4].
              cell_id == 30);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[5].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[5].
              channel == 30);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[5].
              cell_id == 35);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[6].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[6].
              channel == 40);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[6].
              cell_id == 40);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[7].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[7].
              channel == 40);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[7].
              cell_id == 45);
  }

  // Verify neighbor cell list for 1X, if present
  if (cdma_info_ptr->irat_cdma_1x_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list_size == 8);
    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              cell_id == 100);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              cell_id == 102);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              cell_id == 104);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              cell_id == 106);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              cell_id == 108);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              cell_id == 110);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
              cell_id == 112);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
              cell_id == 115);
  }
}
/* lte_rrc_verify_cdma_neigh_cell_list */

/*===========================================================================

  FUNCTION:  lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries

===========================================================================*/
/*!
  @brief
  Function to verify CDMA neighbor cell list for duplicate entries

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
)
{
  TF_ASSERT(cdma_info_ptr != NULL);

  // Verify neighbor cell list for eHRPD, if present
  if (cdma_info_ptr->irat_cdma_ehrpd_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list_size == 3);
    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              channel == 10);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              cell_id == 10);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              channel == 10);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              cell_id == 20);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              channel == 10);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              cell_id == 25);
  }

  // Verify neighbor cell list for 1X, if present
  if (cdma_info_ptr->irat_cdma_1x_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list_size == 5);
    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              cell_id == 100);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              cell_id == 102);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              cell_id == 104);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              cell_id == 106);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              cell_id == 108);
  }
}
/* lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries */

/*===========================================================================

  FUNCTION:  lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries_1

===========================================================================*/
/*!
  @brief
  Function to verify CDMA neighbor cell list for duplicate entries

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries_1
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
)
{
  TF_ASSERT(cdma_info_ptr != NULL);

  // Verify neighbor cell list for eHRPD, if present
  if (cdma_info_ptr->irat_cdma_ehrpd_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list_size == 8);

    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              channel == 10);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              cell_id == 10);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              channel == 10);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              cell_id == 15);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              channel == 20);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              cell_id == 20);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              band_class == SYS_BAND_BC0);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              channel == 20);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              cell_id == 25);

    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[4].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[4].
              channel == 60);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[4].
              cell_id == 73);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[5].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[5].
              channel == 60);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[5].
              cell_id == 74);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[6].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[6].
              channel == 40);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[6].
              cell_id == 40);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[7].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[7].
              channel == 40);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[7].
              cell_id == 45);
  }

  // Verify neighbor cell list for 1X, if present
  if (cdma_info_ptr->irat_cdma_1x_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list_size == 6);
    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              cell_id == 100);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              cell_id == 102);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              cell_id == 115);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              cell_id == 106);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              cell_id == 108);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              cell_id == 110);
  }
}
/* lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries_1 */
/*===========================================================================

  FUNCTION:  lte_rrc_verify_cdma_neigh_cell_list_invalid

===========================================================================*/
/*!
  @brief
  Function to verify CDMA neighbor cell list with invalid entries for bandclass

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_verify_cdma_neigh_cell_list_invalid
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
)
{
  TF_ASSERT(cdma_info_ptr != NULL);

  // Verify neighbor cell list for eHRPD, if present
  if (cdma_info_ptr->irat_cdma_ehrpd_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list_size == 4);
    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              channel == 30);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[0].
              cell_id == 30);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              channel == 30);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[1].
              cell_id == 35);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              channel == 40);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[2].
              cell_id == 40);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              band_class == SYS_BAND_BC1);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              channel == 40);
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[3].
              cell_id == 45);
  }

  // Verify neighbor cell list for 1X, if present
  if (cdma_info_ptr->irat_cdma_1x_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list_size == 8);
    // Validate first neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
              cell_id == 100);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
              cell_id == 102);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
              cell_id == 104);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
              cell_id == 106);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              channel == 100);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
              cell_id == 108);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
              cell_id == 110);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
              cell_id == 112);

    // Validate next neighbor cell in the list
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
              band_class == SYS_BAND_BC4);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
              channel == 110);
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
              cell_id == 115);
  }
}
/* lte_rrc_verify_cdma_neigh_cell_list_invalid */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_populate_max_cdma_neigh_cells

===========================================================================*/
/*!
  @brief
  Function that populates SIB8 with maximum number of neighbor cells. If the
  test_invalid_neigh_cells parameter is set to TRUE, then more than the allowed
  number of neighbor cells are inserted. 

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_populate_max_cdma_neigh_cells
(
  lte_rrc_osys_NeighCellListCDMA2000 *neighCellList_ptr,
  boolean test_excess_neigh_cells
)
{
  lte_rrc_osys_NeighCellsPerBandclassListCDMA2000 *neighCellsPerFreqList_ptr = NULL;
  lte_rrc_osys_PhysCellIdListCDMA2000 *physCellIdList_ptr  = NULL;
  uint8                         neigh_cell_cnt     = 0;
  uint8                         arfcn              = 0;
  uint32 j;
  int no_of_freq;

  no_of_freq = LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS / LTE_RRC_MEAS_TEST_MAX_PHY_CELL_IN_CHANNEL;

  neighCellsPerFreqList_ptr = &neighCellList_ptr->elem[0].neighCellsPerFreqList;

  if(!test_excess_neigh_cells)
  {
    neighCellsPerFreqList_ptr->elem = (lte_rrc_osys_NeighCellsPerBandclassCDMA2000 *)
            lte_rrc_asn1_osys_IE_realloc(neighCellsPerFreqList_ptr->elem, sizeof(lte_rrc_osys_NeighCellsPerBandclassCDMA2000) * no_of_freq);
    neighCellsPerFreqList_ptr->n = no_of_freq;
  }
  else
  {
    neighCellsPerFreqList_ptr->elem = (lte_rrc_osys_NeighCellsPerBandclassCDMA2000 *)
            lte_rrc_asn1_osys_IE_realloc(neighCellsPerFreqList_ptr->elem, sizeof(lte_rrc_osys_NeighCellsPerBandclassCDMA2000) * (no_of_freq + 1));
    neighCellsPerFreqList_ptr->n = (no_of_freq + 1);
  }
  
  // Populate the neighbor cell list so that the number of neighbors equals the
  // maximum allowed value
  while (arfcn < no_of_freq)
  {
    neighCellsPerFreqList_ptr->elem[arfcn].arfcn = arfcn;
    physCellIdList_ptr = &neighCellsPerFreqList_ptr->elem[arfcn].physCellIdList;
    if(arfcn == no_of_freq - 1)
    {
      physCellIdList_ptr->elem = (lte_rrc_osys_PhysCellIdCDMA2000 *) 
        lte_rrc_asn1_osys_IE_realloc(physCellIdList_ptr->elem, sizeof(lte_rrc_osys_PhysCellIdCDMA2000) * (LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS - neigh_cell_cnt));
      physCellIdList_ptr->n = LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS - neigh_cell_cnt;
    }
    else
    {
      physCellIdList_ptr->elem = (lte_rrc_osys_PhysCellIdCDMA2000 *) 
        lte_rrc_asn1_osys_IE_realloc(physCellIdList_ptr->elem, sizeof(lte_rrc_osys_PhysCellIdCDMA2000) * LTE_RRC_MEAS_TEST_MAX_PHY_CELL_IN_CHANNEL);
      physCellIdList_ptr->n = LTE_RRC_MEAS_TEST_MAX_PHY_CELL_IN_CHANNEL;
    }
    for (j = 0; j < physCellIdList_ptr->n; j++)
    {
      physCellIdList_ptr->elem[j] = neigh_cell_cnt;
      neigh_cell_cnt++;
    }
    arfcn++;
  }

  if(test_excess_neigh_cells)
  {
    neighCellsPerFreqList_ptr->elem[arfcn].arfcn = arfcn;
    physCellIdList_ptr = &neighCellsPerFreqList_ptr->elem[arfcn].physCellIdList;
    physCellIdList_ptr->elem = (lte_rrc_osys_PhysCellIdCDMA2000 *) 
      lte_rrc_asn1_osys_IE_realloc(physCellIdList_ptr->elem, sizeof(lte_rrc_osys_PhysCellIdCDMA2000));
    physCellIdList_ptr->n = 1;
    physCellIdList_ptr->elem[0] = neigh_cell_cnt;
    neigh_cell_cnt++;
  }

} /* lte_rrc_meas_test_populate_max_cdma_neigh_cells */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_verify_max_cdma_neigh_cells

===========================================================================*/
/*!
  @brief
  Function that verifies the CDMA neighbor cell list with maximum allowed number of 
  neighbor cells.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_verify_max_cdma_neigh_cells
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr,
  boolean max_cells_exceeded
)
{
  uint8 i = 0;

  // Verify neighbor cell list for eHRPD, if present
  if (cdma_info_ptr->irat_cdma_ehrpd_resel_enabled)
  {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.
              neigh_cell_info_list_size == LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS);

    for (i = 0; i < LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS; i++)
    {
      // Validate first neighbor cell in the list
      TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[i].
                band_class == SYS_BAND_BC0);
      TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[i].
                channel == (i/LTE_RRC_MEAS_TEST_MAX_PHY_CELL_IN_CHANNEL));
      TF_ASSERT(cdma_info_ptr->irat_cdma_ehrpd_resel_param.neigh_cell_info_list[i].
                cell_id == i);
    }
  }

  // Verify neighbor cell list for 1x, if present
  if (cdma_info_ptr->irat_cdma_1x_resel_enabled)
  {
     if (max_cells_exceeded) 
     {
    // Validate neighbor cell list size
    TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.
              neigh_cell_info_list_size == (LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS + 1));

     }
     else
     {
        TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.
              neigh_cell_info_list_size == (LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS));

     }
    
    for (i = 0; i < LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS; i++)
    {
      // Validate first neighbor cell in the list
      TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[i].
                band_class == SYS_BAND_BC4);
      TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[i].
                channel == (i/LTE_RRC_MEAS_TEST_MAX_PHY_CELL_IN_CHANNEL));
      TF_ASSERT(cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[i].
                cell_id == i);
    }
  }

} /* lte_rrc_meas_test_verify_max_cdma_neigh_cells */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_populate_max_wcdma_neigh_cells

===========================================================================*/
/*!
  @brief
  Function that populates SIB6 with maximum number of neighbor cell CarrierFreqs. 
  If the test_invalid_neigh_cells parameter is set to TRUE, then more than the allowed
  number of neighbor cells are inserted. 

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_populate_max_wcdma_neigh_cells
(
  lte_rrc_osys_CarrierFreqListUTRA_FDD *neigh_list_ptr,
  boolean test_excess_neigh_cells
)
{
  uint8                         neigh_cell_cnt     = 0;
  
  if (!test_excess_neigh_cells)
  {
    neigh_list_ptr->elem = (lte_rrc_osys_CarrierFreqUTRA_FDD *) 
      lte_rrc_asn1_osys_IE_realloc(neigh_list_ptr->elem, sizeof(lte_rrc_osys_CarrierFreqUTRA_FDD) * LTE_CPHY_MEAS_MAX_WCDMA_FREQ);
    neigh_list_ptr->n = LTE_CPHY_MEAS_MAX_WCDMA_FREQ;
  }
  else
  {
    neigh_list_ptr->elem = (lte_rrc_osys_CarrierFreqUTRA_FDD *) 
      lte_rrc_asn1_osys_IE_realloc(neigh_list_ptr->elem, sizeof(lte_rrc_osys_CarrierFreqUTRA_FDD) * (LTE_CPHY_MEAS_MAX_WCDMA_FREQ + 1));
    neigh_list_ptr->n = LTE_CPHY_MEAS_MAX_WCDMA_FREQ + 1;
  }

  // Populate the neighbor cell list so that the number of neighbors equals the
  // maximum allowed value
  while (neigh_cell_cnt < LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS)
  {
    neigh_list_ptr->elem[neigh_cell_cnt].carrierFreq = neigh_cell_cnt * 10;
    neigh_list_ptr->elem[neigh_cell_cnt].m.cellReselectionPriorityPresent = 1;
    neigh_list_ptr->elem[neigh_cell_cnt].cellReselectionPriority = 5;
    neigh_list_ptr->elem[neigh_cell_cnt].threshX_High = 25;
    neigh_list_ptr->elem[neigh_cell_cnt].threshX_Low = 10;
    neigh_list_ptr->elem[neigh_cell_cnt].q_RxLevMin = -40;
    neigh_list_ptr->elem[neigh_cell_cnt].q_QualMin = -10;
    neigh_list_ptr->elem[neigh_cell_cnt].p_MaxUTRA = -30;
    neigh_cell_cnt++;

    if ((neigh_cell_cnt >= LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS) && (test_excess_neigh_cells))
    {
      neigh_list_ptr->elem[neigh_cell_cnt].carrierFreq = neigh_cell_cnt * 10;
      neigh_list_ptr->elem[neigh_cell_cnt].m.cellReselectionPriorityPresent = 1;
      neigh_list_ptr->elem[neigh_cell_cnt].cellReselectionPriority = 5;
      neigh_list_ptr->elem[neigh_cell_cnt].threshX_High = 25;
      neigh_list_ptr->elem[neigh_cell_cnt].threshX_Low = 10;
      neigh_list_ptr->elem[neigh_cell_cnt].q_RxLevMin = -40;
      neigh_list_ptr->elem[neigh_cell_cnt].q_QualMin = -10;
      neigh_list_ptr->elem[neigh_cell_cnt].p_MaxUTRA = -30;
      neigh_cell_cnt++;
    }
  }
} /* lte_rrc_meas_test_populate_max_wcdma_neigh_cells */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_verify_max_wcdma_neigh_cells

===========================================================================*/
/*!
  @brief
  Function that verifies the WCDMA neighbor cell list with maximum allowed number of 
  neighbor cells.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_verify_max_wcdma_neigh_cells
(
  const lte_cphy_meas_irat_wcdma_resel_info_s *wcdma_info_ptr
)
{
  uint8 i = 0;

  for (i = 0; i < (LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS - 1); i++)
  {
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].freq == (i * 10));
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].cell_resel_priority == 5);
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].thresh_Xhigh == 50);
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].thresh_Xlow == 20);
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].q_rx_lev_min_actual == -79);
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].q_qual_min == -10);
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].p_max == -30);
    TF_ASSERT(wcdma_info_ptr->fdd_freq_list[i].thresh_rel9_present == FALSE);
  }
} /* lte_rrc_meas_test_verify_max_wcdma_neigh_cells */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_populate_max_tdscdma_neigh_cells

===========================================================================*/
/*!
  @brief
  Function that populates SIB6 with maximum number of neighbor cell CarrierFreqs. 
  If the test_invalid_neigh_cells parameter is set to TRUE, then more than the allowed
  number of neighbor cells are inserted. 

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_populate_max_tdscdma_neigh_cells
(
  lte_rrc_osys_CarrierFreqListUTRA_TDD *neigh_list_ptr,
  boolean test_excess_neigh_cells
)
{
  uint8                         neigh_cell_cnt     = 0;
  
  if (!test_excess_neigh_cells)
  {
    neigh_list_ptr->elem = (lte_rrc_osys_CarrierFreqUTRA_TDD *) 
      lte_rrc_asn1_osys_IE_realloc(neigh_list_ptr->elem, sizeof(lte_rrc_osys_CarrierFreqUTRA_TDD) * LTE_CPHY_MEAS_MAX_TDSCDMA_FREQ);
    neigh_list_ptr->n = LTE_CPHY_MEAS_MAX_TDSCDMA_FREQ;
  }
  else
  {
    neigh_list_ptr->elem = (lte_rrc_osys_CarrierFreqUTRA_TDD *) 
      lte_rrc_asn1_osys_IE_realloc(neigh_list_ptr->elem, sizeof(lte_rrc_osys_CarrierFreqUTRA_TDD) * (LTE_CPHY_MEAS_MAX_TDSCDMA_FREQ + 1));
    neigh_list_ptr->n = LTE_CPHY_MEAS_MAX_TDSCDMA_FREQ + 1;
  }

  // Populate the neighbor cell list so that the number of neighbors equals the
  // maximum allowed value
  while (neigh_cell_cnt < LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS)
  {
    neigh_list_ptr->elem[neigh_cell_cnt].carrierFreq = neigh_cell_cnt * 10;
    neigh_list_ptr->elem[neigh_cell_cnt].m.cellReselectionPriorityPresent = 1;
    neigh_list_ptr->elem[neigh_cell_cnt].cellReselectionPriority = 5;
    neigh_list_ptr->elem[neigh_cell_cnt].threshX_High = 25;
    neigh_list_ptr->elem[neigh_cell_cnt].threshX_Low = 10;
    neigh_list_ptr->elem[neigh_cell_cnt].q_RxLevMin = -40;
    neigh_list_ptr->elem[neigh_cell_cnt].p_MaxUTRA = -30;
    neigh_cell_cnt++;

    if ((neigh_cell_cnt >= LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS) && (test_excess_neigh_cells))
    {
      neigh_list_ptr->elem[neigh_cell_cnt].carrierFreq = neigh_cell_cnt * 10;
      neigh_list_ptr->elem[neigh_cell_cnt].m.cellReselectionPriorityPresent = 1;
      neigh_list_ptr->elem[neigh_cell_cnt].cellReselectionPriority = 5;
      neigh_list_ptr->elem[neigh_cell_cnt].threshX_High = 25;
      neigh_list_ptr->elem[neigh_cell_cnt].threshX_Low = 10;
      neigh_list_ptr->elem[neigh_cell_cnt].q_RxLevMin = -40;
      neigh_list_ptr->elem[neigh_cell_cnt].p_MaxUTRA = -30;
      neigh_cell_cnt++;
    }
  }
} /* lte_rrc_meas_test_populate_max_tdscdma_neigh_cells */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_verify_max_tdscdma_neigh_cells

===========================================================================*/
/*!
  @brief
  Function that verifies the TDSCDMA neighbor cell list with maximum allowed number of 
  neighbor cells.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_verify_max_tdscdma_neigh_cells
(
  const lte_cphy_meas_irat_tdscdma_resel_info_s *tdscdma_info_ptr
)
{
  uint8 i = 0;

  for (i = 0; i < (LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS - 1); i++)
  {
    TF_ASSERT(tdscdma_info_ptr->tdd_freq_list[i].freq == (i * 10));
    TF_ASSERT(tdscdma_info_ptr->tdd_freq_list[i].cell_resel_priority == 5);
    TF_ASSERT(tdscdma_info_ptr->tdd_freq_list[i].thresh_Xhigh == 50);
    TF_ASSERT(tdscdma_info_ptr->tdd_freq_list[i].thresh_Xlow == 20);
    TF_ASSERT(tdscdma_info_ptr->tdd_freq_list[i].q_rx_lev_min_actual == -79);
    TF_ASSERT(tdscdma_info_ptr->tdd_freq_list[i].p_max == -30);
  }
} /* lte_rrc_meas_test_verify_max_tdscdma_neigh_cells */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_populate_max_gsm_neigh_freq_groups

===========================================================================*/
/*!
  @brief
  Function that populates SIB7 with maximum number of neighbor freq groups. Each
  group contains two ARFCNs, there by maximum allowed number of neighbor ARFCNs
  is populated in SIB7.

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_populate_max_gsm_neigh_freq_groups
(
  lte_rrc_osys_CarrierFreqsInfoListGERAN *neigh_freq_group_list
)
{
  uint8 neigh_freq_group_cnt = 0;
  unsigned char bit_map = 1;
  uint64 ncc_permitted = 20;

  neigh_freq_group_list->elem = (lte_rrc_osys_CarrierFreqsInfoGERAN *) 
                           lte_rrc_asn1_osys_IE_realloc(neigh_freq_group_list->elem, sizeof(lte_rrc_osys_CarrierFreqsInfoGERAN) * LTE_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS);
  neigh_freq_group_list->n = LTE_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS;

  // Populate the neighbor cell list so that the number of neighbors equals the
  // maximum allowed value
  while (neigh_freq_group_cnt < LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS)
  {
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.m.cellReselectionPriorityPresent = 1;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.m.p_MaxGERANPresent = 1;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.cellReselectionPriority = 7;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.ncc_Permitted.numbits = 8;
    TF_ASSERT(E_SUCCESS == lte_rrc_osys_asn1_int_to_bitstring(ncc_permitted, &neigh_freq_group_list->elem[neigh_freq_group_cnt].
                                                         commonInfo.ncc_Permitted));
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.p_MaxGERAN = 3;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.q_RxLevMin = 12;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.threshX_High = 28;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].commonInfo.threshX_Low = 14;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].carrierFreqs.startingARFCN = (neigh_freq_group_cnt + 1) * 10;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].carrierFreqs.bandIndicator = lte_rrc_osys_BandIndicatorGERAN_dcs1800;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].carrierFreqs.followingARFCNs.t = T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs;
    neigh_freq_group_list->elem[neigh_freq_group_cnt].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs = 
      (lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs));
    neigh_freq_group_list->elem[neigh_freq_group_cnt].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->numocts = 1;
    /* The following statements imply there is 1 followingARFCN */
    neigh_freq_group_list->elem[neigh_freq_group_cnt].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->data[0] = bit_map;
    neigh_freq_group_cnt++;
  }
} /* lte_rrc_meas_test_populate_max_gsm_neigh_freq_groups */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_populate_max_gsm_neigh_freqs_in_bitmap

===========================================================================*/
/*!
  @brief
  Function that populates SIB7 with maximum number of neighbor ARFCNs possible
  in followingARFCNs bitmap.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_populate_max_gsm_neigh_freqs_in_bitmap
(
  lte_rrc_osys_CarrierFreqsInfoListGERAN *neigh_freq_group_list
)
{
  unsigned char bit_map = 0xFF; 
  uint64 ncc_permitted = 20;
  
  neigh_freq_group_list->elem[0].commonInfo.m.cellReselectionPriorityPresent = 1;
  neigh_freq_group_list->elem[0].commonInfo.m.p_MaxGERANPresent = 1;
  neigh_freq_group_list->elem[0].commonInfo.cellReselectionPriority = 7;
  neigh_freq_group_list->elem[0].commonInfo.ncc_Permitted.numbits = 8;
  TF_ASSERT(E_SUCCESS == lte_rrc_osys_asn1_int_to_bitstring(ncc_permitted, &neigh_freq_group_list->elem[0].
                                                       commonInfo.ncc_Permitted));
  neigh_freq_group_list->elem[0].commonInfo.p_MaxGERAN = 3;
  neigh_freq_group_list->elem[0].commonInfo.q_RxLevMin = 12;
  neigh_freq_group_list->elem[0].commonInfo.threshX_High = 28;
  neigh_freq_group_list->elem[0].commonInfo.threshX_Low = 14;
  neigh_freq_group_list->elem[0].carrierFreqs.startingARFCN = 10;
  neigh_freq_group_list->elem[0].carrierFreqs.bandIndicator = lte_rrc_osys_BandIndicatorGERAN_dcs1800;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.t = T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs;
  /* The following statements imply there are 32 followingARFCNs */
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.
    variableBitMapOfARFCNs = (lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs));
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->numocts = 4;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->data[0] = bit_map;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->data[1] = bit_map;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->data[2] = bit_map;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->data[3] = bit_map;
} /* lte_rrc_meas_test_populate_max_gsm_neigh_freqs_in_bitmap */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_populate_max_gsm_freqs_in_measObj_using_bitmap

===========================================================================*/
/*!
  @brief
  Function that populates measObj with maximum number of neighbor ARFCNs possible
  in followingARFCNs bitmap.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_populate_max_gsm_freqs_in_measObj_using_bitmap
(
  lte_rrc_osys_CarrierFreqsGERAN *freq_list
)
{
  unsigned char bit_map = 0xFF; 
  freq_list->startingARFCN = 100;
  freq_list->bandIndicator = lte_rrc_osys_BandIndicatorGERAN_pcs1900;
  freq_list->followingARFCNs.t = T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs;
  /* The following statements imply there are 32 followingARFCNs */
  freq_list->followingARFCNs.u.
    variableBitMapOfARFCNs = (lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs));
  freq_list->followingARFCNs.u.variableBitMapOfARFCNs->numocts = 4;
  freq_list->followingARFCNs.u.variableBitMapOfARFCNs->data[0] = bit_map;
  freq_list->followingARFCNs.u.variableBitMapOfARFCNs->data[1] = bit_map;
  freq_list->followingARFCNs.u.variableBitMapOfARFCNs->data[2] = bit_map;
  freq_list->followingARFCNs.u.variableBitMapOfARFCNs->data[3] = bit_map;
} /* lte_rrc_meas_test_populate_max_gsm_freqs_in_measObj_using_bitmap */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_verify_max_gsm_neigh_freq_groups

===========================================================================*/
/*!
  @brief
  Function that verifies the GSM CarrierFreqsInfo list with maximum allowed 
  number of neighbor freq groups.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_verify_max_gsm_neigh_freq_groups
(
  const lte_cphy_meas_irat_gsm_info_s *gsm_info_ptr
)
{
  uint8 i = 0, num_freq = 0;

  TF_ASSERT(gsm_info_ptr->num_frequency_groups == LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS);
  TF_ASSERT(gsm_info_ptr->num_freqs == 16);
  for (i = 0; i < LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS; i++)
  {
    TF_ASSERT(gsm_info_ptr->freq_groups[i].cell_resel_priority == 7);
    TF_ASSERT(gsm_info_ptr->freq_groups[i].gsm_band == LTE_L1_GSM_BAND_DCS1800);
    TF_ASSERT(gsm_info_ptr->freq_groups[i].ncc_permitted == 20);
    TF_ASSERT(gsm_info_ptr->freq_groups[i].p_max == 3);
    TF_ASSERT(gsm_info_ptr->freq_groups[i].q_rxlev_min == -91);
    TF_ASSERT(gsm_info_ptr->freq_groups[i].thresh_gsm_high == 56);
    TF_ASSERT(gsm_info_ptr->freq_groups[i].thresh_gsm_low == 28);
    TF_ASSERT(gsm_info_ptr->freqs[num_freq].freq_group_index == i);
    TF_ASSERT(gsm_info_ptr->freqs[num_freq].freq == (i + 1) * 10);
    num_freq++;
    TF_ASSERT(gsm_info_ptr->freqs[num_freq].freq_group_index == i);
    TF_ASSERT(gsm_info_ptr->freqs[num_freq].freq == (((i + 1) * 10) + 8));
    num_freq++;
  }
} /* lte_rrc_meas_test_verify_max_gsm_neigh_freq_groups */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_verify_max_gsm_neigh_freqs_in_bitmap

===========================================================================*/
/*!
  @brief
  Function that verifies the GSM CarrierFreqsInfo list with maximum allowed 
  number of neighbor freqs in the followingARFCNs bitmap.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_verify_max_gsm_neigh_freqs_in_bitmap
(
  const lte_cphy_meas_irat_gsm_info_s *gsm_info_ptr
)
{
  uint8 i = 0, starting_arfcn = 10;

  // Verify starting ARFCN
  TF_ASSERT(gsm_info_ptr->num_frequency_groups == 1);
  TF_ASSERT(gsm_info_ptr->num_freqs == LTE_CPHY_MEAS_MAX_GERAN_FREQ);
  TF_ASSERT(gsm_info_ptr->freq_groups[0].cell_resel_priority == 7);
  TF_ASSERT(gsm_info_ptr->freq_groups[0].gsm_band == LTE_L1_GSM_BAND_DCS1800);
  TF_ASSERT(gsm_info_ptr->freq_groups[0].ncc_permitted == 20);
  TF_ASSERT(gsm_info_ptr->freq_groups[0].p_max == 3);
  TF_ASSERT(gsm_info_ptr->freq_groups[0].q_rxlev_min == -91);
  TF_ASSERT(gsm_info_ptr->freq_groups[0].thresh_gsm_high == 56);
  TF_ASSERT(gsm_info_ptr->freq_groups[0].thresh_gsm_low == 28);

  // Verify following ARFCNs
  for (i = 0; i < LTE_CPHY_MEAS_MAX_GERAN_FREQ; i++)
  {
    TF_ASSERT(gsm_info_ptr->freqs[i].freq_group_index == 0);
    TF_ASSERT(gsm_info_ptr->freqs[i].freq == (starting_arfcn + i));
  }

} /* lte_rrc_meas_test_verify_max_gsm_neigh_freqs_in_bitmap */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_verify_max_gsm_freqs_in_measObj_using_bitmap

===========================================================================*/
/*!
  @brief
  Function that verifies the GSM CarrierFreqs with maximum allowed number of 
  neighbor freqs in the followingARFCNs bitmap in a measObj.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_verify_max_gsm_freqs_in_measObj_using_bitmap
(
  const lte_l1_gsm_arfcn_t *gsm_freq_list
)
{
  uint8 i = 0, starting_arfcn = 100;

  // Verify following ARFCNs
  for (i = 0; i < LTE_CPHY_MEAS_MAX_GERAN_FREQ; i++)
  {
    TF_ASSERT(*(gsm_freq_list + i) == (starting_arfcn + i));
  }
} /* lte_rrc_meas_test_verify_max_gsm_freqs_in_measObj_using_bitmap */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_populate_max_gsm_neigh_freqs

===========================================================================*/
/*!
  @brief
  Function that populates SIB7 with maximum number of neighbor ARFCNs.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_populate_max_gsm_neigh_freqs
(
  lte_rrc_osys_CarrierFreqsInfoListGERAN *neigh_freq_group_list
)
{
  uint64 ncc_permitted = 20;
  
  // Populate the neighbor cell list so that the number of neighbors equals the
  // maximum allowed value
  neigh_freq_group_list->elem[0].commonInfo.m.cellReselectionPriorityPresent = 1;
  neigh_freq_group_list->elem[0].commonInfo.m.p_MaxGERANPresent = 1;
  neigh_freq_group_list->elem[0].commonInfo.cellReselectionPriority = 7;
  neigh_freq_group_list->elem[0].commonInfo.ncc_Permitted.numbits = 8;
  TF_ASSERT(E_SUCCESS == lte_rrc_osys_asn1_int_to_bitstring(ncc_permitted, &neigh_freq_group_list->elem[0].
                                                       commonInfo.ncc_Permitted));
  neigh_freq_group_list->elem[0].commonInfo.p_MaxGERAN = 3;
  neigh_freq_group_list->elem[0].commonInfo.q_RxLevMin = 12;
  neigh_freq_group_list->elem[0].commonInfo.threshX_High = 28;
  neigh_freq_group_list->elem[0].commonInfo.threshX_Low = 14;
  neigh_freq_group_list->elem[0].carrierFreqs.startingARFCN = 100;
  neigh_freq_group_list->elem[0].carrierFreqs.bandIndicator = lte_rrc_osys_BandIndicatorGERAN_pcs1900;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.t = T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs = 
    (lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs *)
  lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs));
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs->arfcn_Spacing = 10;
  neigh_freq_group_list->elem[0].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs->numberOfFollowingARFCNs = 31;

} /* lte_rrc_meas_test_populate_max_gsm_neigh_freqs */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_verify_max_gsm_neigh_freqs

===========================================================================*/
/*!
  @brief
  Function that verifies the GSM CarrierFreqsInfo list with maximum allowed 
  number of neighbor freqs.  

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_verify_max_gsm_neigh_freqs
(
  const lte_cphy_meas_irat_gsm_info_s *gsm_info_ptr
)
{
  uint8 i = 0, group_idx = 0;
  uint8 starting_arfcn = 100, arfcn_spacing = 10;

  TF_ASSERT(gsm_info_ptr->num_frequency_groups == 1);
  TF_ASSERT(gsm_info_ptr->num_freqs == LTE_CPHY_MEAS_MAX_GERAN_FREQ);
  // Verify following ARFCNs
  for (i = 0; i < LTE_CPHY_MEAS_MAX_GERAN_FREQ; i++)
  {
    TF_ASSERT(gsm_info_ptr->freq_groups[group_idx].cell_resel_priority == 7);
    TF_ASSERT(gsm_info_ptr->freq_groups[group_idx].gsm_band == LTE_L1_GSM_BAND_PCS1900);
    TF_ASSERT(gsm_info_ptr->freq_groups[group_idx].ncc_permitted == 20);
    TF_ASSERT(gsm_info_ptr->freq_groups[group_idx].p_max == 3);
    TF_ASSERT(gsm_info_ptr->freq_groups[group_idx].q_rxlev_min == -91);
    TF_ASSERT(gsm_info_ptr->freq_groups[group_idx].thresh_gsm_high == 56);
    TF_ASSERT(gsm_info_ptr->freq_groups[group_idx].thresh_gsm_low == 28);
    TF_ASSERT(gsm_info_ptr->freqs[i].freq_group_index == group_idx);
    TF_ASSERT(gsm_info_ptr->freqs[i].freq == (starting_arfcn + (i * arfcn_spacing)));
  }
} /* lte_rrc_meas_test_verify_max_gsm_neigh_freqs */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_gsm_band_from_arfcn

===========================================================================*/
/*!
    @brief
    This function gets the GSM band from the given ARFCN

    @return
    sys_sband_e_type

 */
/*=========================================================================*/
sys_sband_e_type lte_rrc_meas_test_stub__get_gsm_band_from_arfcn
(
  geran_grr_band_ind_e    band_ind,
  uint16                  arfcn,
  boolean                 *gsm_band_valid_ptr
)
{
  sys_sband_e_type sys_gsm_band = SYS_SBAND_MAX;

  LTE_ASSERT( gsm_band_valid_ptr != NULL );
  *gsm_band_valid_ptr = FALSE;

/* Lint complains about comparison always being TRUE
  for >= ARFCN_EGSM_900_LOWER_RANGE_MIN as arfcn is unsigned. */
  if (/*arfcn >= ARFCN_EGSM_900_LOWER_RANGE_MIN && */
      arfcn <= ARFCN_EGSM_900_LOWER_RANGE_MAX)
  {
    if (arfcn >= ARFCN_PGSM_900_MIN)
    {
      sys_gsm_band = SYS_SBAND_GSM_PGSM_900;
    }
    else
    {
      sys_gsm_band = SYS_SBAND_GSM_EGSM_900;
    }
  }
  else if (arfcn >= ARFCN_EGSM_900_UPPER_RANGE_MIN &&
           arfcn <= ARFCN_EGSM_900_UPPER_RANGE_MAX)
  {
    sys_gsm_band = SYS_SBAND_GSM_EGSM_900;
  }
  else if (arfcn >= ARFCN_CELL_850_MIN &&
           arfcn <= ARFCN_CELL_850_MAX)
  {
    sys_gsm_band = SYS_SBAND_GSM_850;
  }
  else if (arfcn >= ARFCN_DCS_1800_MIN &&
           arfcn <= ARFCN_DCS_1800_MAX)
  {
    if (arfcn <= ARFCN_PCS_1900_MAX)
    {
      if (band_ind == GERAN_GRR_BAND_IND_PCS_1900)
      {
        sys_gsm_band = SYS_SBAND_GSM_PCS_1900;
      }
      else if (band_ind == GERAN_GRR_BAND_IND_DCS_1800)
      {
        sys_gsm_band = SYS_SBAND_GSM_DCS_1800;
      }
    }
    else
    {
      sys_gsm_band = SYS_SBAND_GSM_DCS_1800;
    }
  }

  if (gsm_band_valid_ptr != NULL)
  {
    if (sys_gsm_band == SYS_SBAND_MAX)
    {
      *gsm_band_valid_ptr = FALSE;
    }
    else
    {
      *gsm_band_valid_ptr = TRUE;
    }
  }

  if (*gsm_band_valid_ptr == TRUE)
  {
    switch(sys_gsm_band)
    {
    case SYS_SBAND_GSM_DCS_1800:
      if (band_ind != (int)GERAN_GRR_BAND_IND_DCS_1800)
      {
        LTE_RRC_MSG(LTE_RRC_MEAS_HIGH,
                    "RRC MEAS: Ignoring band indicator field for non-overlapping bands");
      }
      break;
    case SYS_SBAND_GSM_PCS_1900:
      if (band_ind != (int)GERAN_GRR_BAND_IND_PCS_1900)
      {
        LTE_RRC_MSG(LTE_RRC_MEAS_HIGH,
                    "RRC MEAS: Ignoring band indicator field for non-overlapping bands");
      }
      break;
    case SYS_SBAND_GSM_850:
      LTE_RRC_MSG(LTE_RRC_MEAS_HIGH,
                  "RRC MEAS: Ignoring band indicator field for non-overlapping bands");
      break;
    case SYS_SBAND_GSM_EGSM_900:
      LTE_RRC_MSG(LTE_RRC_MEAS_HIGH,
                  "RRC MEAS: Ignoring band indicator field for non-overlapping bands");
      break;
    case SYS_SBAND_GSM_PGSM_900:
      LTE_RRC_MSG(LTE_RRC_MEAS_HIGH,
                  "RRC MEAS: Ignoring band indicator field for non-overlapping bands");
      break;
    default:
      LTE_ASSERT(0);
    }
  }

  return sys_gsm_band;
} /* lte_rrc_meas_test_stub__get_gsm_band_from_arfcn */
#endif

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_get_meas_sm_to_connected

===========================================================================*/
/*!
  @brief
  This Function sends the relevant UMIDs to get Meas SM to Connected state

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_get_meas_sm_to_connected
(
  void
)
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  lte_rrc_meas_test_ctrl_state = LTE_RRC_CONTROLLER_SM__CONNECTED;

} /* lte_rrc_meas_test_get_meas_sm_to_connected */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi

===========================================================================*/
/*!
  @brief
  This Function sends the CPHY_MEAS_CFG_CNF to Meas and verifies if Meas sends
  MEAS_CFG_CNFI.

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi
(
  void
)
{
  lte_cphy_conn_meas_cfg_cnf_s cfg_cnf;
  lte_rrc_meas_cfg_cnfi_s *cfg_cnfi;
  uint32 buf_len;

  // Send successful config cnf from CPHY
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_CONN_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  
  // We wait for Meas to process the meas cfg cnf
  lte_rrc_utf_wait_for_done();
  
  // We expect RRC to be in the connected state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);
  
  // We expect meas sm to have thrown the internal meas_cfg_cnfi
  lte_rrc_utf_get_next_int_msg((byte**)&cfg_cnfi, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_meas_cfg_cnfi_s));
  TF_ASSERT(cfg_cnfi->status == E_SUCCESS);

} /* lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_get_cre_state

===========================================================================*/
/*!
    @brief
    This function simulates the CRE API for returning the state of CRE SM.

    @return
    stm_state_t

 */
/*=========================================================================*/
stm_state_t lte_rrc_meas_test_get_cre_state
(
  void
)
{
  return LTE_RRC_CRE_SM__INITIAL;
} /* lte_rrc_meas_test_get_cre_state */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel8

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel8
(
  void
)
{
  return LTE_3GPP_REL8_MAR09;

} /* lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel8 */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9
(
  void
)
{
  return LTE_3GPP_REL9_JUN10;

} /* lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9 */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel10

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel10
(
  void
)
{
  return LTE_3GPP_REL10_SEP12;

} /* lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel10 */

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel11

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel11
(
  void
)
{
  return LTE_3GPP_REL11;

} /* lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel11 */


/*===========================================================================

  FUNCTION:  lte_rrc_is_rel_10_ca_enabled_false

===========================================================================*/
/*!
    @brief
    This function simulates CA support API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_is_rel_10_ca_enabled_false
(
  void
)
{
  return FALSE;

} /* lte_rrc_is_rel_10_ca_enabled_false */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__get_gsm_band_from_arfcn

===========================================================================*/
/*!
    @brief
    This function gets the GSM band from the given ARFCN

    @return
    sys_sband_e_type

 */
/*=========================================================================*/
sys_sband_e_type lte_rrc_meas_test_stub__get_gsm_band_from_arfcn_valid
(
  geran_grr_band_ind_e    band_ind,
  uint16                  arfcn,
  boolean                 *gsm_band_valid_ptr
)
{

  *gsm_band_valid_ptr = TRUE;

  return SYS_SBAND_GSM_PGSM_900;
} /* lte_rrc_meas_test_stub__get_gsm_band_from_arfcn */


/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_stub__earfcn_in_cfl

===========================================================================*/
/*!
    @brief
    This function simulates ESMGR fn that determines if earfcn is part of CFL

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_meas_test_stub__earfcn_in_cfl
(
  lte_earfcn_t earfcn
)
{

  return FALSE;

} /* lte_rrc_meas_test_stub__earfcn_in_cfl */

boolean lte_rrc_meas_test_stub__csp_cap_tdscdma_is_supported
(
  void
)
{
  return lte_rrc_meas_test_tdscdma_supported;
}

boolean lte_rrc_meas_test_stub__csp_cap_wcdma_is_supported
(
  void
)
{
  return lte_rrc_meas_test_wcdma_supported;
}

/*===========================================================================

  FUNCTION:  lte_rrc_meas_test_init

===========================================================================*/
/*!
  @brief
  Initializes the tf proto stubs.

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_meas_test_init
(
  void
)
{
  /*-----------------------------------------------------------------------
             MEAS STUB FUNCTIONS
  -----------------------------------------------------------------------*/
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = lte_rrc_meas_test_stub__csp_get_serving_cell_params;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_meas_test_stub__sib_get_cell_sib;
  TF_STUB(lte_rrc_csp_get_camped_status) = lte_rrc_meas_test_stub__csp_get_camped_status;
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_meas_test_stub__controller_get_state;
  TF_STUB(lte_rrc_cep_get_state) = lte_rrc_meas_test_stub__cep_get_state;
  TF_STUB(lte_rrc_csp_lte_band_is_supported) = lte_rrc_meas_test_stub__csp_lte_band_is_supported;
  //TF_STUB(lte_rrc_csp_wcdma_band_is_supported) = lte_rrc_meas_test_stub__csp_wcdma_band_is_supported;
  //TF_STUB(lte_rrc_csp_tdscdma_band_is_supported) = lte_rrc_meas_test_stub__csp_tdscdma_band_is_supported;
 // TF_STUB(lte_rrc_csp_gsm_band_is_supported) = lte_rrc_meas_test_stub__csp_gsm_band_is_supported;
  //TF_STUB(lte_rrc_csp_cdma_1x_band_is_supported) = lte_rrc_meas_test_stub__csp_cdma_1x_band_is_supported;
  //TF_STUB(lte_rrc_csp_cdma_do_band_is_supported) = lte_rrc_meas_test_stub__csp_cdma_do_band_is_supported;
  TF_STUB(lte_rrc_cap_inter_freq_is_supported) = lte_rrc_cap_test_stub__inter_freq_is_supported;
  //TF_STUB(lte_rrc_cap_hrpd_rx_config) = lte_rrc_meas_test_stub__cap_hrpd_rx_config;
  //TF_STUB(lte_rrc_cap_1x_rx_config) = lte_rrc_meas_test_stub__cap_1x_rx_config;
  TF_STUB(lte_rrc_get_revoke_priority) = lte_rrc_meas_test_stub__get_revoke_priority_success;
  TF_STUB(lte_rrc_get_revoke_priority) = lte_rrc_meas_test_stub__get_revoke_priority_success;
  TF_STUB(lte_rrc_csp_get_selected_plmn) = lte_rrc_meas_test_stub__get_selected_plmn;
  TF_STUB(lte_rrc_csp_get_nas_req_info) = lte_rrc_meas_test_stub__get_nas_req_info;
 // TF_STUB(lte_rrc_get_wcdma_band_from_uarfcn) = lte_rrc_meas_test_stub__get_wcdma_band_from_uarfcn;
 // TF_STUB(lte_rrc_get_tdscdma_band_from_uarfcn) = lte_rrc_meas_test_stub__get_tdscdma_band_from_uarfcn;
  TF_STUB(lte_rrc_sib_b12_to_b17_conversion_is_needed) = 
    lte_rrc_meas_test_stub__b12_to_b17_conversion_is_not_needed;
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_meas_test_stub__get_fgi_bit;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel11;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_is_rel_10_ca_enabled) = lte_rrc_is_rel_10_ca_enabled_false;
#endif

  //TF_STUB(lte_rrc_get_gsm_band_from_arfcn) = lte_rrc_meas_test_stub__get_gsm_band_from_arfcn_valid;
  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_meas_test_stub__config_ho_in_progress;
  TF_STUB(lte_rrc_ctlr_irat_cgi_in_progress) = lte_rrc_meas_test_stub__ctlr_irat_cgi_in_progress;
  ////TF_STUB(lte_rrc_irat_proc_is_in_progress) = lte_rrc_meas_test_stub__irat_proc_is_in_progress_false;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_meas_test_stub__get_mode_fdd_for_last_camped_cell;
  TF_STUB(lte_rrc_esmgr_earfcn_in_cfl) = lte_rrc_meas_test_stub__earfcn_in_cfl;
#ifdef FEATURE_FEMTO_CSG
  TF_STUB(lte_rrc_csg_asf_check_csg_in_whitelist) = lte_rrc_meas_test_stub__csg_in_whitelist;
  #endif
 // TF_STUB(lte_rrc_controller_get_cphy_rf_status_info) = lte_rrc_meas_test_cphy_rf_status_available;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_llc_check_freq_match_with_pcell_scell_freq) = lte_rrc_meas_test_freq_match_with_pcell_scell;
#endif

 // TF_STUB(lte_rrc_cap_get_si_req_ho_utra) = lte_rrc_cap_test_stub__si_req_ho_utra_supported;
  //TF_STUB(lte_rrc_cap_get_si_req_ho_eutra) = lte_rrc_cap_test_stub__si_req_ho_eutra_supported;
  TF_STUB(lte_rrc_irat_get_tds_freq_from_irat_db) = lte_rrc_irat_to_tds_mgr_irat_acq_db_stub;
  //TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_meas_test_stub__csp_cap_tdscdma_is_supported;
  //TF_STUB(lte_rrc_csp_cap_wcdma_is_supported) = lte_rrc_meas_test_stub__csp_cap_wcdma_is_supported;
} /* lte_rrc_meas_test_init() */

boolean wcdma_convert_uarfcn
(
  uint16 source_uarfcn,
  sys_sband_e_type band_num,
  uint16 *target_uarfcn,
  sys_plmn_id_s_type camped_plmn_id
)
{
  *target_uarfcn = source_uarfcn;
  return TRUE;
}

boolean wcdma_mfbi_is_supported()
{
  return TRUE;
}

void lte_rrc_meas_test_set_wcdma_rat_support
(
  boolean val
)
{
  lte_rrc_meas_test_wcdma_supported = val;
}

void lte_rrc_meas_test_set_tdscdma_rat_support
(
  boolean val
)
{
  lte_rrc_meas_test_tdscdma_supported = val;
}
