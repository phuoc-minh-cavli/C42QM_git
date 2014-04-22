/*!
  @file lte_rrc_mdt_test.cpp


  @brief
  Unit test code for MDT .

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_mdt_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/08   dd     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_ueinfo.h"
#include "__lte_rrc_mdt.h"
#include "lte_rrc_ueinfo.h"
#include "lte_rrc_capabilities.h"
#include "__lte_rrc_controller.h"
#include "stm2.h"
#include "lte_rrc_controller.h"
#include "lte_mac_msg.h"
#include "lte_mac_rrc.h"
#include "lte_rrc_cep_test_asn1_common.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_mdti.h"
#include "lte_rrc_ueinfoi.h"
#include <dsm_item.h>
#include <dsm_init.h>
}

#ifdef FEATURE_LTE_MDT

#include "lte_rrc_cep_test_common.h"
#include "lte_rrc_test_conn_setup_basic.h"
#include "lte_rrc_test_reestablishment.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_cep_test_transitions.cpp
*/

static lte_rrc_handover_started_indi_s      handover_started_indi;
static lte_rrc_handover_completed_ind_s     handover_completed_ind;
static lte_rrc_conn_mode_failure_indi_s     config_failure_ind;
static lte_rrc_conn_released_indi_s         connection_released_indi;
static lte_rrc_deactivate_req_s deactivate_req;
static lte_cphy_rf_available_ind_s available_req;
static lte_cphy_rf_unavailable_ind_s unavailable_req;

extern "C"
{

  //TF_DECLARE_STUB( stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_camped_cell_identity,lte_rrc_global_cell_id_t *);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(stm_state_t,lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(boolean, lte_rrc_config_ho_in_progress, void);
  TF_PROTO_STUB(boolean, lte_rrc_cep_rach_is_in_progress, void);
  TF_PROTO_STUB(uint32, lte_rrc_config_get_t304_rem_time, void);
  TF_PROTO_STUB(uint32, lte_rrc_config_get_t304_value, void);
  TF_PROTO_STUB(void, lte_rrc_loc_services_get_loc_info, lte_rrc_osys_LocationInfo_r10 *);       
	TF_DECLARE_STUB( boolean, lte_rrc_cap_is_mdt_supported, void);

  static lte_errno_e lte_rrc_test_mdt_get_camped_cell_identity(lte_rrc_global_cell_id_t *cell_info)
  {
    return E_SUCCESS;
  }
  static stm_state_t lte_rrc_test_controller_get_state_connected()
  {
    return LTE_RRC_CONTROLLER_SM__CONNECTED;
  }
	static boolean lte_rrc_test_cap_is_mdt_supported()
  {
    return TRUE;
  }
	static lte_3gpp_release_ver_e lte_rrc_test_get_3gpp_release_version_for_MDT_rel10
	(
	  void
	)
	{
	  return LTE_3GPP_REL10;
	}

        	static lte_3gpp_release_ver_e lte_rrc_test_get_3gpp_release_version_for_MDT_rel11
	(
	  void
	)
	{
	  return LTE_3GPP_REL11;
	}
}

static uint8 lte_rrc_test_default_mdt_cfg_msg[] = 
{
0x50, 0xAA, 0x87, 0x21, 0x00, 0x40, 0x00, 0x40, 0x3F, 0xF0, 0x20, 0x00, 0x00, 0x40, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
};

static uint8 lte_rrc_test_default_mdt_cfg_msg2[] = 
{
0x50, 0xaa, 0x87, 0x21, 0x00, 0x40, 0x00, 0x40, 0x3f, 0xf0, 0x20, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x2a, 0x86, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint32 lte_rrc_test_config_get_t304_value_100
(
  void
)
{
   return 100;
}

static boolean lte_rrc_test_cep_rach_is_in_progress_TRUE
(
  void
)
{
   return TRUE;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/



/*===========================================================================

                                UNIT TEST

=======================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMDTCFGTest);

void LteRrcMDTCFGTest::Setup()
{  
  int ext_count = 0;

  lte_rrc_cep_test.ext_umid_list[0] = LTE_RRC_T330_TMRI;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  
  lte_rrc_utf_setup(LTE_RRC_MDT_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
	TF_STUB(lte_rrc_cap_is_mdt_supported) = lte_rrc_test_cap_is_mdt_supported;
	TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_test_get_3gpp_release_version_for_MDT_rel11;
	TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_controller_get_state_connected;

              TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void LteRrcMDTCFGTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_LoggedMeasurementConfiguration_r10_IEs * mdt_cfg_ptr;
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr = NULL;
  int decode_status = 0, i=0;
	  /* DLM to be sent */
  lte_rrc_mh_dlm_s dlm;		
  encoded_pdu.length = sizeof(lte_rrc_test_default_mdt_cfg_msg);
  encoded_pdu.value = lte_rrc_test_default_mdt_cfg_msg;
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
	/* Initialize header and populate fields in the DLM to be sent */
  lte_rrc_init_default_hdr(&dlm.msg_hdr, LTE_RRC_REL10_LOGGED_MEAS_CFG_REQUEST_DLM);
  dlm.dl_msg_ptr = dl_msg_ptr;
	dlm.pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;

  /* Send the DLM */
  msgr_send(&dlm.msg_hdr, sizeof(dlm));

	TF_SLEEP(90);

	dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_msg_ptr != NULL);
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	dl_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_messageClassExtension;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	dl_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_c1;
	dl_msg_ptr->message.u.c1->t = T_lte_rrc_osys_DL_DCCH_MessageType_c1_ueInformationRequest_r9;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	dl_msg_ptr->message.u.c1->t =	T_lte_rrc_osys_DL_DCCH_MessageType_c1_loggedMeasurementConfiguration_r10;
	dl_msg_ptr->message.u.c1->u.loggedMeasurementConfiguration_r10->criticalExtensions.u.c1->t =
				T_lte_rrc_osys_LoggedMeasurementConfiguration_r10_criticalExtensions_c1_spare3;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	dl_msg_ptr->message.u.c1->u.loggedMeasurementConfiguration_r10->criticalExtensions.u.c1->t = 
		T_lte_rrc_osys_LoggedMeasurementConfiguration_r10_criticalExtensions_c1_loggedMeasurementConfiguration_r10;
	dl_msg_ptr->message.u.c1->u.loggedMeasurementConfiguration_r10->criticalExtensions.t = 0;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	dl_msg_ptr->message.u.c1->u.loggedMeasurementConfiguration_r10->criticalExtensions.t = 
		T_lte_rrc_osys_LoggedMeasurementConfiguration_r10_criticalExtensions_criticalExtensionsFuture;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	dl_msg_ptr->message.u.c1->u.loggedMeasurementConfiguration_r10->criticalExtensions.t = 
		T_lte_rrc_osys_LoggedMeasurementConfiguration_r10_criticalExtensions_c1;

	mdt_cfg_ptr = 
			dl_msg_ptr->message.u.c1->u.loggedMeasurementConfiguration_r10->criticalExtensions.u.c1->u.loggedMeasurementConfiguration_r10;
	mdt_cfg_ptr->m.nonCriticalExtensionPresent = 1;

	mdt_cfg_ptr->traceReference_r10.plmn_Identity_r10.mcc.n = LTE_RRC_MAX_MCC_DIGITS+1;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	mdt_cfg_ptr->traceReference_r10.plmn_Identity_r10.mcc.n = LTE_RRC_MAX_MCC_DIGITS;

	mdt_cfg_ptr->m.areaConfiguration_r10Present = 0;
	
	for(i=0;i<6;i++)
	{
		mdt_cfg_ptr->loggingDuration_r10 = i;
		TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	}
	mdt_cfg_ptr->loggingDuration_r10 = i;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	mdt_cfg_ptr->loggingDuration_r10 = 0;
	for(i=0;i<8;i++)
	{
		mdt_cfg_ptr->loggingInterval_r10 = i;
		TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	}
	mdt_cfg_ptr->loggingInterval_r10 = i;
	TF_ASSERT(FALSE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	mdt_cfg_ptr->loggingInterval_r10 = 0;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	

	mdt_cfg_ptr->m.areaConfiguration_r10Present = 1;
	mdt_cfg_ptr->areaConfiguration_r10.t = 3;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	
	mdt_cfg_ptr->areaConfiguration_r10.t = T_lte_rrc_osys_AreaConfiguration_r10_trackingAreaCodeList_r10;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	
	mdt_cfg_ptr->areaConfiguration_r10.u.trackingAreaCodeList_r10->n = 0;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	mdt_cfg_ptr->areaConfiguration_r10.u.trackingAreaCodeList_r10->n = LTE_RRC_MDT_AREA_CFG_MAX_NUM_TAC+1;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	
	mdt_cfg_ptr->areaConfiguration_r10.u.trackingAreaCodeList_r10->n = 1;	
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	
	
  /*Free ConnectionSetupComplete (unencoded) message*/
  lte_rrc_osys_asn1_free_pdu(dl_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);  

	encoded_pdu.length = sizeof(lte_rrc_test_default_mdt_cfg_msg2);
  encoded_pdu.value = lte_rrc_test_default_mdt_cfg_msg2;
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_msg_ptr != NULL);
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));	

	mdt_cfg_ptr = 
			dl_msg_ptr->message.u.c1->u.loggedMeasurementConfiguration_r10->criticalExtensions.u.c1->u.loggedMeasurementConfiguration_r10;

	mdt_cfg_ptr->areaConfiguration_r10.u.cellGlobalIdList_r10->n = LTE_RRC_MDT_AREA_CFG_MAX_NUM_CGI+1;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	mdt_cfg_ptr->areaConfiguration_r10.u.cellGlobalIdList_r10->n = 0;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	mdt_cfg_ptr->areaConfiguration_r10.u.cellGlobalIdList_r10->n = 1;

	mdt_cfg_ptr->areaConfiguration_r10.u.cellGlobalIdList_r10->elem[0].plmn_Identity.mcc.n = LTE_RRC_MAX_MCC_DIGITS+1;
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));

	 /*Free ConnectionSetupComplete (unencoded) message*/
  lte_rrc_osys_asn1_free_pdu(dl_msg_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);  

 //Expire the T330 timer
 TF_SLEEP(900000);

  lte_rrc_utf_reset();
}

void LteRrcMDTCFGTest::Teardown()
{  
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(NO_RUN, LteRrcMDTHandleReportTest);

void LteRrcMDTHandleReportTest::Setup()
{  
  int ext_count = 0;

  lte_rrc_cep_test.ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[1] = LTE_RRC_CONN_EST_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
  lte_rrc_cep_test.ext_umid_list[2] = LTE_RRC_UL_DATA_CNF;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;
    lte_rrc_cep_test.ext_umid_list[3] = LTE_RRC_BARRING_UPDATE_IND;
  lte_rrc_cep_test.ext_umid_count = ++ext_count;

  lte_rrc_utf_setup(LTE_RRC_MDT_SM, 
                    &(lte_rrc_cep_test.ext_umid_list[0]), 
                    ext_count);
	TF_STUB(lte_rrc_cap_is_mdt_supported) = lte_rrc_test_cap_is_mdt_supported;
	TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_test_get_3gpp_release_version_for_MDT_rel11;
	TF_STUB(lte_rrc_controller_get_state) = lte_rrc_test_controller_get_state_connected;

              TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void LteRrcMDTHandleReportTest::Test()
{
  lte_rrc_mdt_s i_ptr;
  lte_cphy_ueinfo_mdt_report_ind_s rpt;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_LoggedMeasurementConfiguration_r10_IEs * mdt_cfg_ptr;
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr = NULL;
  int decode_status = 0, i=0;
  lte_errno_e status;
  encoded_pdu.length = sizeof(lte_rrc_test_default_mdt_cfg_msg);
  encoded_pdu.value = lte_rrc_test_default_mdt_cfg_msg;
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_msg_ptr != NULL);
	TF_ASSERT(TRUE == lte_rrc_mdt_process_dlm(dl_msg_ptr));
	i_ptr.dd_ptr = (lte_rrc_mdt_dd_s *)lte_rrc_mem_priv_data_calloc(sizeof(lte_rrc_mdt_dd_s));

	  /* BUild MDT report to be sent from ML1*/
  rpt.rsrq_info_present = TRUE;
  rpt.serving_rsrp = 1;
  rpt.serving_rsrq = 2;
  rpt.neigh_results.eutra_size = 1;
  rpt.neigh_results.cdma2k_size = 0;
  rpt.neigh_results.wcdma_size = 1;
  rpt.neigh_results.wcdma_neigh[0].carrierFreq = 10;
  rpt.neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  rpt.neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 15;
  rpt.neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = 10;
  rpt.neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 20;  
  rpt.neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  rpt.neigh_results.eutra_neigh[0].carrierFreq = 2500;
  rpt.neigh_results.eutra_neigh[0].eutra_list.size = 1;
  rpt.neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 200;
  rpt.neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 10;
  rpt.neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 20;
  rpt.neigh_results.gsm_size = 1;
  rpt.neigh_results.gsm_neigh[0].freq = 1020;
  rpt.neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_EGSM_900;
  rpt.neigh_results.gsm_neigh[0].phy_cell_id.ncc = 100;
  rpt.neigh_results.gsm_neigh[0].phy_cell_id.bcc = 20;
  rpt.neigh_results.gsm_neigh[0].rssi = 10;
  TF_STUB(lte_rrc_csp_get_camped_cell_identity) = lte_rrc_test_mdt_get_camped_cell_identity;

	TF_ASSERT(E_SUCCESS == lte_rrc_mdt_handle_rpt(&i_ptr, &rpt));
	rpt.serving_rsrp = -1;
	TF_ASSERT(E_DATA_INVALID == lte_rrc_mdt_handle_rpt(&i_ptr, &rpt));	
	rpt.serving_rsrp = 97+1;
	TF_ASSERT(E_DATA_INVALID == lte_rrc_mdt_handle_rpt(&i_ptr, &rpt));
  rpt.rsrq_info_present = FALSE;
  rpt.serving_rsrp = 1;
	TF_ASSERT(E_SUCCESS == lte_rrc_mdt_handle_rpt(&i_ptr, &rpt));
  rpt.rsrq_info_present = TRUE;
	rpt.serving_rsrq = -1;
	TF_ASSERT(E_DATA_INVALID == lte_rrc_mdt_handle_rpt(&i_ptr, &rpt));	
	rpt.serving_rsrq = 34 +1;
	TF_ASSERT(E_DATA_INVALID == lte_rrc_mdt_handle_rpt(&i_ptr, &rpt));
  rpt.serving_rsrq = 2;
	
//	lte_rrc_mem_free(i_ptr.dd_ptr);

        lte_rrc_mem_priv_data_free(i_ptr.dd_ptr);
        i_ptr.dd_ptr = NULL;

        lte_rrc_osys_asn1_free_pdu(dl_msg_ptr, 
                                   lte_rrc_osys_DL_DCCH_Message_PDU
                                   );
  TF_SLEEP(172800000);
        // Prepare deactivate_req with reason LPM to send to RRC
  lte_rrc_init_default_hdr(&deactivate_req.msg_hdr, LTE_RRC_DEACTIVATE_REQ);
  deactivate_req.deactivate_reason = SYS_STOP_MODE_REASON_POWER_OFF;

  // Send deactivate_req to RRC
  status = msgr_send(&deactivate_req.msg_hdr, sizeof(lte_rrc_deactivate_req_s));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();


  lte_rrc_utf_reset();




}

void LteRrcMDTHandleReportTest::Teardown()
{  
   lte_rrc_test_cep_teardown();
  lte_rrc_utf_teardown();
}


stm_state_t lte_rrc_ueinfo_test_get_controller_state
(
  void
)
{
  return LTE_RRC_CONTROLLER_SM__CONNECTED;
}

stm_state_t lte_rrc_ueinfo_test_get_controller_state_connecting
(
  void
)
{
  return LTE_RRC_CONTROLLER_SM__CONNECTING;
}

static lte_3gpp_release_ver_e lte_rrc_test_get_3gpp_release_version_for_UEINFO
(
   void
)
{	 
    return LTE_3GPP_REL11;
}

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_hof) ;

void lte_rrc_test_ueinfo_hof::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;

            TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void lte_rrc_test_ueinfo_hof::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;
  lte_rrc_osys_UEInformationResponse_r9_IEs ueinfo_response_ie;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    TF_MSG("Sending Handover Started Indication");
    lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
    lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

    lte_rrc_utf_wait_for_done();
    
     /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);

    /* Setting the header for config failure indication */
  lte_rrc_init_default_hdr(&config_failure_ind.msg_hdr, LTE_RRC_CONN_MODE_FAILURE_INDI);
  config_failure_ind.conn_mode_failure_cause = LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO;
  config_failure_ind.ho_failure_cause = LTE_RRC_HO_FAILURE_CAUSE_RACH;
  lte_rrc_utf_send_msg((byte*)&config_failure_ind, sizeof(config_failure_ind));
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(10000);

#if 0
TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
             LTE_RRC_UEINFO_SM__ACTIVE); 
#endif        

   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(TRUE == lte_rrc_ueinfo_populate_rlf_rpt(i_ptr,&ueinfo_response_ie));

   TF_ASSERT(ueinfo_response_ie.rlf_Report_r9.m.timeConnFailure_r10Present == 1);
   TF_ASSERT(ueinfo_response_ie.rlf_Report_r9.timeConnFailure_r10 == 10);

  lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_hof::Teardown()
{
  lte_rrc_utf_teardown();
}


uint32 lte_rrc_test_config_get_t304_rem_time_90 (void)
{
   return 90;
}

uint32 lte_rrc_test_config_get_t304_rem_time_60 (void)
{
   return 60;
}

uint32 lte_rrc_test_config_get_t304_rem_time_50 (void)
{
   return 50;
}

uint32 lte_rrc_test_config_get_t304_rem_time_40 (void)
{
   return 40;
}

uint32 lte_rrc_test_config_get_t304_rem_time_20 (void)
{
   return 20;
}

uint32 lte_rrc_test_config_get_t304_rem_time_100(void)
{
   return 100;
}

boolean lte_rrc_test_config_ho_in_progress_true (void)
{
   return TRUE;
}
boolean lte_rrc_test_config_ho_in_progress_false (void)
{
   return FALSE;
}
TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_hof_rf_avail_unavail) ;

void lte_rrc_test_ueinfo_hof_rf_avail_unavail::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;
      
            TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
         
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_false;

    lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
    TF_MSG("\nSending RF UNAVAILABLE IND\n");
    lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
    lte_rrc_utf_wait_for_done();

     TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
     TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_100;
    TF_MSG("Sending Handover Started Indication");
    lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
    lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

    lte_rrc_utf_wait_for_done();
    
     /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();

      TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
      TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_90;

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_get_t304_value) = lte_rrc_test_config_get_t304_value_100;

    /* Setting the header for config failure indication */
  lte_rrc_init_default_hdr(&config_failure_ind.msg_hdr, LTE_RRC_CONN_MODE_FAILURE_INDI);
  config_failure_ind.conn_mode_failure_cause = LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO;
  config_failure_ind.ho_failure_cause = LTE_RRC_HO_FAILURE_CAUSE_RACH;
  lte_rrc_utf_send_msg((byte*)&config_failure_ind, sizeof(config_failure_ind));
  lte_rrc_utf_wait_for_done();

  
   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(TRUE == i_ptr->sd_ptr->is_rpt_present);
   TF_ASSERT(LTE_RRC_UEINFO_FAILURE_HOF == i_ptr->sd_ptr->failure);
   lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_hof_rf_avail_unavail2) ;

void lte_rrc_test_ueinfo_hof_rf_avail_unavail2::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;
      
      TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
         
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail2::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_false;

    lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
    TF_MSG("\nSending RF UNAVAILABLE IND\n");
    lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
    lte_rrc_utf_wait_for_done();

     TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
     TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_100;
    TF_MSG("Sending Handover Started Indication");
    lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
    lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

    lte_rrc_utf_wait_for_done();
    
     /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();

      TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
      TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_90;

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
  TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_60;

  lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
  TF_MSG("\nSending RF UNAVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
  TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_20;

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_get_t304_value) = lte_rrc_test_config_get_t304_value_100;

    /* Setting the header for config failure indication */
  lte_rrc_init_default_hdr(&config_failure_ind.msg_hdr, LTE_RRC_CONN_MODE_FAILURE_INDI);
  config_failure_ind.conn_mode_failure_cause = LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO;
  config_failure_ind.ho_failure_cause = LTE_RRC_HO_FAILURE_CAUSE_RACH;
  lte_rrc_utf_send_msg((byte*)&config_failure_ind, sizeof(config_failure_ind));
  lte_rrc_utf_wait_for_done();

  
   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(FALSE == i_ptr->sd_ptr->is_rpt_present);
   TF_ASSERT(LTE_RRC_UEINFO_FAILURE_MAX == i_ptr->sd_ptr->failure);
   lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail2::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_hof_rf_avail_unavail3) ;

void lte_rrc_test_ueinfo_hof_rf_avail_unavail3::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;
      
      TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
         
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail3::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;


     TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
     TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_100;
    TF_MSG("Sending Handover Started Indication");
    lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
    lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

    lte_rrc_utf_wait_for_done();
    
     /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();

      TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
      TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_90;

      lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
      TF_MSG("\nSending RF UNAVAILABLE IND\n");
      lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
      lte_rrc_utf_wait_for_done();

      TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
      TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_60;

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_40;

  lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
  TF_MSG("\nSending RF UNAVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
  TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_20;

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_get_t304_value) = lte_rrc_test_config_get_t304_value_100;

    /* Setting the header for config failure indication */
  lte_rrc_init_default_hdr(&config_failure_ind.msg_hdr, LTE_RRC_CONN_MODE_FAILURE_INDI);
  config_failure_ind.conn_mode_failure_cause = LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO;
  config_failure_ind.ho_failure_cause = LTE_RRC_HO_FAILURE_CAUSE_RACH;
  lte_rrc_utf_send_msg((byte*)&config_failure_ind, sizeof(config_failure_ind));
  lte_rrc_utf_wait_for_done();

  
   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(FALSE == i_ptr->sd_ptr->is_rpt_present);
   TF_ASSERT(LTE_RRC_UEINFO_FAILURE_MAX == i_ptr->sd_ptr->failure);
   lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail3::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_hof_rf_avail_unavail4) ;

void lte_rrc_test_ueinfo_hof_rf_avail_unavail4::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;         
      TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail4::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;


     TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
     TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_100;
    TF_MSG("Sending Handover Started Indication");
    lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
    lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

    lte_rrc_utf_wait_for_done();
    
     /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();

      TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
      TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_90;

      lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
      TF_MSG("\nSending RF UNAVAILABLE IND\n");
      lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
      lte_rrc_utf_wait_for_done();

      TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
      TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_90;

      lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
      TF_MSG("\nSending RF UNAVAILABLE IND\n");
      lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
      lte_rrc_utf_wait_for_done();

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_40;

  lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
  TF_MSG("\nSending RF UNAVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_test_config_ho_in_progress_true;
  TF_STUB(lte_rrc_config_get_t304_rem_time) = lte_rrc_test_config_get_t304_rem_time_40;

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_STUB(lte_rrc_config_get_t304_value) = lte_rrc_test_config_get_t304_value_100;

    /* Setting the header for config failure indication */
  lte_rrc_init_default_hdr(&config_failure_ind.msg_hdr, LTE_RRC_CONN_MODE_FAILURE_INDI);
  config_failure_ind.conn_mode_failure_cause = LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO;
  config_failure_ind.ho_failure_cause = LTE_RRC_HO_FAILURE_CAUSE_RACH;
  lte_rrc_utf_send_msg((byte*)&config_failure_ind, sizeof(config_failure_ind));
  lte_rrc_utf_wait_for_done();

  
   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(TRUE == i_ptr->sd_ptr->is_rpt_present);
   TF_ASSERT(LTE_RRC_UEINFO_FAILURE_HOF == i_ptr->sd_ptr->failure);
   lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_hof_rf_avail_unavail4::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_ho_success_rlf ) ;

void lte_rrc_test_ueinfo_ho_success_rlf::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

            TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void lte_rrc_test_ueinfo_ho_success_rlf::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
    lte_rrc_ueinfo_s *i_ptr_1; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt, asn_rlf_rpt_1;
  lte_rrc_osys_UEInformationResponse_r9_IEs ueinfo_response_ie;
  lte_rrc_osys_UEInformationResponse_r9_IEs ueinfo_response_ie_1;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    TF_STUB(lte_rrc_config_get_t304_value) = 
       lte_rrc_test_config_get_t304_value_100;


    TF_MSG("Sending Handover Started Indication");
    lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
    lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

    lte_rrc_utf_wait_for_done();
    
     /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();


  //  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
    //        LTE_RRC_UEINFO_SM__INACTIVE);
    
    TF_MSG("Sending Handover Completed Indication");
    lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                             LTE_RRC_HANDOVER_COMPLETED_IND);
    handover_completed_ind.phy_cell_id = 2;
    handover_completed_ind.dl_freq = 1;
    handover_completed_ind.dl_bandwidth = LTE_BW_NRB_6;
    lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                         sizeof(handover_completed_ind));

    TF_SLEEP(102301);
    lte_rrc_utf_wait_for_done();
        /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_NORMAL;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

 /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

   TF_SLEEP(1000);
   lte_rrc_utf_wait_for_done(); 


   TF_MSG("Sent UEINFO_RSP "); 

   TF_MSG("Sending Connection Released Indication");
   lte_rrc_init_default_hdr(&connection_released_indi.msg_hdr, 
                            LTE_RRC_CONN_RELEASED_INDI);
   connection_released_indi.rel_reason = LTE_RRC_CONN_REL_NORMAL;
   connection_released_indi.cell_barring_is_reqd = false;

   lte_rrc_utf_send_msg((byte*)&connection_released_indi, 
                        sizeof(connection_released_indi));

   TF_SLEEP(1000);
   lte_rrc_utf_wait_for_done(); 

     // Prepare deactivate_req with reason LPM to send to RRC
  lte_rrc_init_default_hdr(&deactivate_req.msg_hdr, LTE_RRC_DEACTIVATE_REQ);
  deactivate_req.deactivate_reason = SYS_STOP_MODE_REASON_IRAT;

  // Send deactivate_req to RRC
  status = msgr_send(&deactivate_req.msg_hdr, sizeof(lte_rrc_deactivate_req_s));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);

   i_ptr_1 = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(TRUE == lte_rrc_ueinfo_populate_rlf_rpt(i_ptr_1,&ueinfo_response_ie_1));

   TF_ASSERT(ueinfo_response_ie_1.rlf_Report_r9.m.timeConnFailure_r10Present == 1);
   TF_ASSERT(ueinfo_response_ie_1.rlf_Report_r9.timeConnFailure_r10 == 1023);

    lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_ho_success_rlf::Teardown()
{
    lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_ho_success_deactivate_rlf ) ;

void lte_rrc_test_ueinfo_ho_success_deactivate_rlf::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;

            TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void lte_rrc_test_ueinfo_ho_success_deactivate_rlf::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
    lte_rrc_ueinfo_s *i_ptr_1; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt, asn_rlf_rpt_1;
  lte_rrc_osys_UEInformationResponse_r9_IEs ueinfo_response_ie, ueinfo_response_ie_1;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    TF_MSG("Sending Handover Started Indication");
    lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
    lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

    lte_rrc_utf_wait_for_done();
    
     /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();


  //  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
    //        LTE_RRC_UEINFO_SM__INACTIVE);
    
    TF_MSG("Sending Handover Completed Indication");
    lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                             LTE_RRC_HANDOVER_COMPLETED_IND);
    handover_completed_ind.phy_cell_id = 2;
    handover_completed_ind.dl_freq = 1;
    handover_completed_ind.dl_bandwidth = LTE_BW_NRB_6;
    lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                         sizeof(handover_completed_ind));

    TF_SLEEP(102301);
    lte_rrc_utf_wait_for_done();

    TF_MSG("Sending Connection Released Indication");
    lte_rrc_init_default_hdr(&connection_released_indi.msg_hdr, 
                             LTE_RRC_CONN_RELEASED_INDI);
    connection_released_indi.rel_reason = LTE_RRC_CONN_REL_NORMAL;
    connection_released_indi.cell_barring_is_reqd = false;

    lte_rrc_utf_send_msg((byte*)&connection_released_indi, 
                         sizeof(connection_released_indi));

    TF_SLEEP(1000);
    lte_rrc_utf_wait_for_done(); 

      // Prepare deactivate_req with reason LPM to send to RRC
   lte_rrc_init_default_hdr(&deactivate_req.msg_hdr, LTE_RRC_DEACTIVATE_REQ);
   deactivate_req.deactivate_reason = SYS_STOP_MODE_REASON_IRAT;

   // Send deactivate_req to RRC
   status = msgr_send(&deactivate_req.msg_hdr, sizeof(lte_rrc_deactivate_req_s));
   TF_ASSERT(status == E_SUCCESS);
   lte_rrc_utf_wait_for_done();

   TF_SLEEP(1000);


        /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_NORMAL;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

 /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

   TF_SLEEP(1000);
   lte_rrc_utf_wait_for_done(); 


   TF_MSG("Sent UEINFO_RSP "); 

   i_ptr_1 = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(TRUE == lte_rrc_ueinfo_populate_rlf_rpt(i_ptr_1,&ueinfo_response_ie_1));

   TF_ASSERT(ueinfo_response_ie_1.rlf_Report_r9.m.timeConnFailure_r10Present == 0);

    lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_ho_success_deactivate_rlf::Teardown()
{
    lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_rlf ) ;

void lte_rrc_test_ueinfo_rlf::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void lte_rrc_test_ueinfo_rlf::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;
  lte_rrc_osys_UEInformationResponse_r9_IEs ueinfo_response_ie;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_NORMAL;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

 /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
  cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
  cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
  cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
  cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
  cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
  cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    
  
  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__ACTIVE);
   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(TRUE == lte_rrc_ueinfo_populate_rlf_rpt(i_ptr,&ueinfo_response_ie));

   TF_ASSERT(ueinfo_response_ie.rlf_Report_r9.m.timeConnFailure_r10Present == 0);

         TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;

   TF_MSG("Sending Handover Started Indication");
   lte_rrc_init_default_hdr(&handover_started_indi.msg_hdr, LTE_RRC_HANDOVER_STARTED_INDI);
   lte_rrc_utf_send_msg((byte*)&handover_started_indi, sizeof(handover_started_indi));

   lte_rrc_utf_wait_for_done();

    /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
 lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

 TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
 TF_ASSERT(cphy_rpt_req != NULL);
 TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
 TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
 lte_rrc_utf_wait_for_done();

 TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
           LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


 lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
 cphy_ueinfo_rpt_rsp.status = E_SUCCESS;

 //Populate basic report as response
 cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;
 cphy_rpt_req->rlf_rpt->serving_rsrp = 97;
 cphy_rpt_req->rlf_rpt->rsrq_info_present = 1;
 cphy_rpt_req->rlf_rpt->serving_rsrq = 34;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_size = 3;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_size = 3;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_size = 3;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_size = 3;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].carrierFreq = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].carrierFreq = 2000;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].carrierFreq = 65535;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.size=1;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.size=1;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.size=1;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].pci = 2;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrp = 34;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[0].eutra_list.eutra[0].rsrq = 22;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].pci = 34;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrp = 34;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[1].eutra_list.eutra[0].rsrq = 22;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].pci = 503;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrp = 97;
 cphy_rpt_req->rlf_rpt->neigh_results.eutra_neigh[2].eutra_list.eutra[0].rsrq = 34;

 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].carrierFreq = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].carrierFreq = 16383;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].carrierFreq = 2000;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.size = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.size = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.size = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_ecno = 34;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].cpich_rscp = -2;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].psc = 511;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[0].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_ecno = 34;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].cpich_rscp = 33;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].psc = 111;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[1].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_ecno = 34;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].cpich_rscp = 91;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].psc = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.wcdma_neigh[2].wcdma_list.wcdma[0].wcdma_mode = LTE_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD;

 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].arfcn = 2047;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].arfcn = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].arfcn = 45;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].band_class = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].band_class = 5;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].band_class = 7;  
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.size = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.size = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.size = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].phy_cell_id = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_pn_phase = 32767;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[0].cdma2k_list.cdma2k[0].pilot_strength = 63;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].phy_cell_id = 511;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_pn_phase = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[1].cdma2k_list.cdma2k[0].pilot_strength = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].phy_cell_id = 44;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_pn_phase = 345;
 cphy_rpt_req->rlf_rpt->neigh_results.cdma2k_neigh[2].cdma2k_list.cdma2k[0].pilot_strength = 22;

 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].freq = 1023;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].gsm_band = LTE_L1_GSM_BAND_DCS1800;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.bcc = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].phy_cell_id.ncc = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[0].rssi = 63;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].freq = 0;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].gsm_band = LTE_L1_GSM_BAND_PCS1900;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.bcc = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].phy_cell_id.ncc = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[1].rssi = 63;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].freq = 500;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].gsm_band = LTE_L1_GSM_BAND_PGSM_900;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.bcc = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].phy_cell_id.ncc = 1;
 cphy_rpt_req->rlf_rpt->neigh_results.gsm_neigh[2].rssi = 0;    

 // Send cphy_ueinfo_rpt_rsp to RRC
 status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
 LTE_ASSERT(status == E_SUCCESS);  

 lte_rrc_utf_wait_for_done();


 //  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
   //        LTE_RRC_UEINFO_SM__INACTIVE);

   TF_MSG("Sending Handover Completed Indication");
   lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                            LTE_RRC_HANDOVER_COMPLETED_IND);
   handover_completed_ind.phy_cell_id = 2;
   handover_completed_ind.dl_freq = 1;
   handover_completed_ind.dl_bandwidth = LTE_BW_NRB_6;
   lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                        sizeof(handover_completed_ind));

   TF_SLEEP(1000);

   TF_MSG("Sending Connection Released Indication");
   lte_rrc_init_default_hdr(&connection_released_indi.msg_hdr, 
                            LTE_RRC_CONN_RELEASED_INDI);
   connection_released_indi.rel_reason = LTE_RRC_CONN_REL_NORMAL;
   connection_released_indi.cell_barring_is_reqd = false;

   lte_rrc_utf_send_msg((byte*)&connection_released_indi, 
                        sizeof(connection_released_indi));

   TF_SLEEP(1000);
   lte_rrc_utf_wait_for_done(); 

     // Prepare deactivate_req with reason LPM to send to RRC
  lte_rrc_init_default_hdr(&deactivate_req.msg_hdr, LTE_RRC_DEACTIVATE_REQ);
  deactivate_req.deactivate_reason = SYS_STOP_MODE_REASON_IRAT;

  // Send deactivate_req to RRC
  status = msgr_send(&deactivate_req.msg_hdr, sizeof(lte_rrc_deactivate_req_s));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();


   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

   TF_ASSERT(TRUE == lte_rrc_ueinfo_populate_rlf_rpt(i_ptr,&ueinfo_response_ie));

   TF_ASSERT(ueinfo_response_ie.rlf_Report_r9.m.timeConnFailure_r10Present == 0);


//lte_rrc_utf_check_no_more_msgs();
  // Reset the UTF framework
  lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_rlf::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_rlf1 ) ;

void lte_rrc_test_ueinfo_rlf1::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
      LTE_CPHY_UEINFO_RPT_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;
      TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
 
}

void lte_rrc_test_ueinfo_rlf1::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
   
    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_TMR;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);
  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
          LTE_RRC_UEINFO_SM__INACTIVE);

      /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_RESET;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);
  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
          LTE_RRC_UEINFO_SM__INACTIVE);

      /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_RF_UNAVAILABLE;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);
  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
          LTE_RRC_UEINFO_SM__INACTIVE);


  //  lte_rrc_utf_check_no_more_msgs();

  // Reset the UTF framework
  lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_rlf1::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_rlf_dsds_inactivity ) ;

void lte_rrc_test_ueinfo_rlf_dsds_inactivity::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
      LTE_CPHY_UEINFO_RPT_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

       TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void lte_rrc_test_ueinfo_rlf_dsds_inactivity::Test()
{
  lte_mac_random_access_problem_ind_msg_s lte_mac_random_access_problem_ind_msg;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
   
    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    /* At this point, Send LTE_MAC_RANDOM_ACCESS_PROBLEM_IND */
  lte_rrc_init_default_hdr(&lte_mac_random_access_problem_ind_msg.hdr,LTE_MAC_RANDOM_ACCESS_PROBLEM_IND);
  lte_mac_random_access_problem_ind_msg.access_reason = LTE_MAC_ACCESS_REASON_DSDS_INACTIVITY_TMR_EXPIRY;
  status = msgr_send(&lte_mac_random_access_problem_ind_msg.hdr, sizeof(lte_mac_random_access_problem_ind_msg));
  TF_ASSERT(status == E_SUCCESS);
  TF_MSG("Sent LTE_MAC_RANDOM_ACCESS_PROBLEM_IND to RRC");
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);


  //UEINFO STM should continue to be in INACTIVE state as no RPT REQ to ML1
  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
          LTE_RRC_UEINFO_SM__INACTIVE);

    /* At this point, Send LTE_MAC_RANDOM_ACCESS_PROBLEM_IND */
  lte_rrc_init_default_hdr(&lte_mac_random_access_problem_ind_msg.hdr,LTE_MAC_RANDOM_ACCESS_PROBLEM_IND);
  lte_mac_random_access_problem_ind_msg.access_reason = LTE_MAC_ACCESS_REASON_DSDS_INACTIVITY_TMR_EXPIRY;
  status = msgr_send(&lte_mac_random_access_problem_ind_msg.hdr, sizeof(lte_mac_random_access_problem_ind_msg));
  TF_ASSERT(status == E_SUCCESS);
  TF_MSG("Sent LTE_MAC_RANDOM_ACCESS_PROBLEM_IND to RRC");
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);


 //UEINFO STM should continue to be in INACTIVE state as no RPT REQ to ML1
  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
          LTE_RRC_UEINFO_SM__INACTIVE);


  // Reset the UTF framework
  lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_rlf_dsds_inactivity::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_rlf_connecting_state ) ;

void lte_rrc_test_ueinfo_rlf_connecting_state::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state_connecting;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;
}

void lte_rrc_test_ueinfo_rlf_connecting_state::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;

    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_NORMAL;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);


  //UEINFO STM should continue to be in INACTIVE state as no RPT REQ to ML1
  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
          LTE_RRC_UEINFO_SM__INACTIVE);

  lte_rrc_utf_wait_for_done();
 
}

void lte_rrc_test_ueinfo_rlf_connecting_state::Teardown()
{
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_test_ueinfo_RLF_BadReportFromMl1_T300expiry ) ;

void lte_rrc_test_ueinfo_RLF_BadReportFromMl1_T300expiry::Setup()
{
  /* Only add those external UMIDs that UEINFO will send out */
  uint32 umid_list[] = 
    { 
                LTE_CPHY_UEINFO_RPT_REQ,
             LTE_CPHY_UEINFO_CONN_FAILURE_RPT_REQ,
       LTE_MAC_RACH_RPT_REQ
		//LTE_CPHY_UEINFO_RPT_CNF,
		//LTE_CPHY_RL_FAILURE_IND
    };

  lte_rrc_utf_setup(LTE_RRC_UEINFO_SM, umid_list,
          sizeof(umid_list)/sizeof(umid_list[0]));

    TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_ueinfo_test_get_controller_state;

      TF_STUB(lte_rrc_get_3gpp_release_version) =
         lte_rrc_test_get_3gpp_release_version_for_UEINFO;

      TF_STUB(lte_rrc_loc_services_get_loc_info) = 
         lte_rrc_test_loc_services_get_loc_info;

      TF_STUB(lte_rrc_cep_rach_is_in_progress) = 
      lte_rrc_test_cep_rach_is_in_progress_TRUE;
}

void lte_rrc_test_ueinfo_RLF_BadReportFromMl1_T300expiry::Test()
{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_errno_e status;
  uint32 buf_len;
  lte_cphy_ueinfo_rpt_req_s  *cphy_rpt_req; 
  lte_cphy_ueinfo_rpt_rsp_s cphy_ueinfo_rpt_rsp;
  lte_rrc_tmri_s    t300_tmr_expiry;
  lte_rrc_ueinfo_s *i_ptr; /*! ueinfo priv data ptr*/ 
  lte_rrc_osys_RLF_Report_r9 asn_rlf_rpt;
  lte_cphy_ueinfo_conn_failure_report_req_s  *conn_est_failure_rpt_req;
  lte_mac_rach_rpt_req_msg_s *mac_rach_rpt_req;
    TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);

    /* At this point, Send RLF indication */
  lte_rrc_init_default_hdr(&rl_fail_ind.msg_hdr,LTE_CPHY_RL_FAILURE_IND);
  rl_fail_ind.rlf_reason = LTE_CPHY_RLF_REASON_NORMAL;
  status = msgr_send(&rl_fail_ind.msg_hdr, sizeof(rl_fail_ind));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

 /* Check for UEINFO_RPT_REQ sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_rpt_req_s));
  TF_ASSERT(cphy_rpt_req != NULL);
  TF_ASSERT(cphy_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_RPT_REQ);
  TF_ASSERT(cphy_rpt_req->type == LTE_CPHY_UEINFO_RPT_MASK_RLF);
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__WAIT_FOR_RPT_CNF);


  lte_rrc_init_default_hdr(&cphy_ueinfo_rpt_rsp.msgr_hdr, LTE_CPHY_UEINFO_RPT_CNF);
  cphy_ueinfo_rpt_rsp.status = E_FAILURE;

  //Populate basic report as response
  cphy_ueinfo_rpt_rsp.type = LTE_CPHY_UEINFO_RPT_MASK_RLF;

  // Send cphy_ueinfo_rpt_rsp to RRC
  status = msgr_send(&cphy_ueinfo_rpt_rsp.msgr_hdr, sizeof(cphy_ueinfo_rpt_rsp));
  LTE_ASSERT(status == E_SUCCESS);  

  lte_rrc_utf_wait_for_done();

  TF_SLEEP(1000);

   TF_ASSERT(stm_get_state(LTE_RRC_UEINFO_SM) == 
            LTE_RRC_UEINFO_SM__INACTIVE);
   i_ptr = (lte_rrc_ueinfo_s *)stm_get_user_data(LTE_RRC_UEINFO_SM);

         TF_STUB(lte_rrc_config_get_t304_value) = 
         lte_rrc_test_config_get_t304_value_100;

         TF_STUB(lte_rrc_controller_get_state) =
         lte_rrc_ueinfo_test_get_controller_state_connecting;

         lte_rrc_init_default_hdr(&t300_tmr_expiry.msg_hdr,LTE_RRC_T300_TMRI);
         
         status = msgr_send(&t300_tmr_expiry.msg_hdr, sizeof(t300_tmr_expiry));
         TF_ASSERT(status == E_SUCCESS);
         lte_rrc_utf_wait_for_done();

         /* Check for conn_est_failure sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&conn_est_failure_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_ueinfo_conn_failure_report_req_s));
  TF_ASSERT(conn_est_failure_rpt_req->msgr_hdr.id == LTE_CPHY_UEINFO_CONN_FAILURE_RPT_REQ);

  /* Check for mach RACH rpt req sent by UEINFO module*/
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_rach_rpt_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_mac_rach_rpt_req_msg_s));
  TF_ASSERT(mac_rach_rpt_req->hdr.id  == LTE_MAC_RACH_RPT_REQ);

  LteRrcTestConEstFailureRpt1__Test();
  lte_rrc_utf_wait_for_done();

       // Prepare deactivate_req with reason LPM to send to RRC
  lte_rrc_init_default_hdr(&deactivate_req.msg_hdr, LTE_RRC_DEACTIVATE_REQ);
  deactivate_req.deactivate_reason = SYS_STOP_MODE_REASON_POWER_OFF;

  // Send deactivate_req to RRC
  status = msgr_send(&deactivate_req.msg_hdr, sizeof(lte_rrc_deactivate_req_s));
  TF_ASSERT(status == E_SUCCESS);
  lte_rrc_utf_wait_for_done();

  // Reset the UTF framework
  lte_rrc_utf_reset();
}

void lte_rrc_test_ueinfo_RLF_BadReportFromMl1_T300expiry::Teardown()
{
  lte_rrc_utf_teardown();
}

#endif
