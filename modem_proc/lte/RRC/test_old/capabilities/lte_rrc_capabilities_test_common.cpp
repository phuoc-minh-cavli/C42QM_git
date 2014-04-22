/*!
  @file lte_rrc_capabilities_test_common.cpp

  @brief
  Common utilities for Capabilities test

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
09/11/09   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_csp.h>
#include <lte_static_rrc_mem_util.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_controller.h>
}

#include <tf_stub.h>
#include <TestFramework.h>
#include "lte_rrc_capabilities_test_common.h"

extern "C"
{
TF_PROTO_STUB(sys_lte_band_mask_e_type,lte_rrc_csp_get_lte_band_preference,void);
//TF_PROTO_STUB(sys_band_mask_type,lte_rrc_csp_get_do_band_preference,void);
//TF_PROTO_STUB(sys_band_mask_type,lte_rrc_csp_get_1x_band_preference,void);
//TF_PROTO_STUB(sys_band_mask_type,lte_rrc_csp_get_wcdma_band_preference,void);
//TF_PROTO_STUB(sys_band_mask_type,lte_rrc_csp_get_tdscdma_band_preference,void);
//TF_PROTO_STUB(sys_band_mask_type,lte_rrc_csp_get_gsm_band_preference,void);
//TF_PROTO_STUB(boolean,lte_rrc_csp_cdma_do_is_supported,void);
//TF_PROTO_STUB(boolean,lte_rrc_csp_cdma_1x_is_supported,void);
//TF_PROTO_STUB(boolean,lte_rrc_csp_cap_wcdma_is_supported,void);
////TF_PROTO_STUB(boolean,lte_rrc_csp_cap_tdscdma_is_supported,void);
//TF_PROTO_STUB(boolean,lte_rrc_csp_gsm_is_supported,void);
TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
}

/*! @brief LTE band preference for unit test
*/
static sys_lte_band_mask_e_type lte_rrc_capabilities_test_lte_band_preference;

/*! @brief CDMA band preference for unit test
*/
static sys_band_mask_type lte_rrc_capabilities_test_do_band_preference;
static sys_band_mask_type lte_rrc_capabilities_test_1x_band_preference;
static sys_band_mask_type lte_rrc_capabilities_test_wcdma_band_preference;
static sys_band_mask_type lte_rrc_capabilities_test_tdscdma_band_preference;
static sys_band_mask_type lte_rrc_capabilities_test_gsm_band_preference;

static boolean cdma_1x_supported=FALSE;

static boolean cdma_do_supported = FALSE;
static boolean w_supported = FALSE;
static boolean tds_supported = FALSE;
static boolean gsm_supported = FALSE;

static stm_state_t lte_rrc_capabilities_controller_state = LTE_RRC_CONTROLLER_SM__CONNECTED; 



/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_lte_band_preference

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_get_lte_band_preference 

    @return
    Returns the LTE band preference (in the Capabilities test framework)

*/
/*=========================================================================*/
static sys_lte_band_mask_e_type 
lte_rrc_capabilities_test_stub_csp_get_lte_band_preference(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_capabilities_test_lte_band_preference);
}/* lte_rrc_capabilities_test_stub_csp_get_lte_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_controller_get_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_controller_get_state

    @return
    Returns the Controller state (in Capabilities test framework) 

*/
/*=========================================================================*/
static stm_state_t 
lte_rrc_capabilities_test_stub_controller_get_state(void)
{
  TF_MSG("lte_rrc_capabilities_test_stub_controller_get_state: Stub was invoked");

  return (lte_rrc_capabilities_controller_state);
}/* lte_rrc_capabilities_test_stub_csp_get_lte_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_set_controller_state

===========================================================================*/
/*!
    @brief
    Function to set controller state in Capabilities test framework 

    @return

*/
/*=========================================================================*/
void 
lte_rrc_capabilities_test_set_controller_state(stm_state_t controller_state)
{
  TF_MSG("lte_rrc_capabilities_test_set_controller_state: Stub was invoked = %d",
         controller_state );

  lte_rrc_capabilities_controller_state = controller_state;
}

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_do_band_preference

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_get_do_band_preference 

    @return
    Returns the CDMA band preference (in the Capabilities test framework)

*/
/*=========================================================================*/
static sys_band_mask_type 
lte_rrc_capabilities_test_stub_csp_get_do_band_preference(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_capabilities_test_do_band_preference);
}/* lte_rrc_capabilities_test_stub_csp_get_do_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_1x_band_preference

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_get_1x_band_preference 

    @return
    Returns the CDMA band preference (in the Capabilities test framework)

*/
/*=========================================================================*/
static sys_band_mask_type 
lte_rrc_capabilities_test_stub_csp_get_1x_band_preference(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_capabilities_test_1x_band_preference);
}/* lte_rrc_capabilities_test_stub_csp_get_1x_band_preference */


/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_wcdma_band_preference

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_get_wcdma_band_preference 

    @return
    Returns the WCDMA band preference (in the Capabilities test framework)

*/
/*=========================================================================*/
static sys_band_mask_type 
lte_rrc_capabilities_test_stub_csp_get_wcdma_band_preference(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_capabilities_test_wcdma_band_preference);
}/* lte_rrc_capabilities_test_stub_csp_get_wcdma_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_tdscdma_band_preference

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_get_tdscdma_band_preference 

    @return
    Returns the TDSCDMA band preference (in the Capabilities test framework)

*/
/*=========================================================================*/
static sys_band_mask_type 
lte_rrc_capabilities_test_stub_csp_get_tdscdma_band_preference(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_capabilities_test_tdscdma_band_preference);
}/* lte_rrc_capabilities_test_stub_csp_get_tdscdma_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_gsm_band_preference

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_get_gsm_band_preference 

    @return
    Returns the GSM band preference (in the Capabilities test framework)

*/
/*=========================================================================*/
static sys_band_mask_type 
lte_rrc_capabilities_test_stub_csp_get_gsm_band_preference(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_capabilities_test_gsm_band_preference);
}/* lte_rrc_capabilities_test_stub_csp_get_gsm_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_cdma_do_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_cdma_do_is_supported

    @return
    Is DO supported ? (in the Capabilities test framework)

*/
/*=========================================================================*/
static boolean 
lte_rrc_capabilities_test_stub_csp_cdma_do_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (cdma_do_supported);
}/* lte_rrc_capabilities_test_stub_csp_cdma_do_is_supported */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_set_cdma_do_is_supported

===========================================================================*/
/*!
    @brief
    Function for setting cdma_do_supported

    @return

*/
/*=========================================================================*/
void 
lte_rrc_capabilities_test_set_cdma_do_is_supported(boolean my_flag)
{
  TF_MSG("Stub was invoked");

  cdma_do_supported = my_flag;
}/* lte_rrc_capabilities_test_set_cdma_do_is_supported */


/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_w_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_w_is_supported

    @return
    Is W supported ? (in the Capabilities test framework)

*/
/*=========================================================================*/
static boolean 
lte_rrc_capabilities_test_stub_csp_w_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (w_supported);
}/* lte_rrc_capabilities_test_stub_csp_cdma_do_is_supported */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_tds_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_tds_is_supported

    @return
    Is W supported ? (in the Capabilities test framework)

*/
/*=========================================================================*/
static boolean 
lte_rrc_capabilities_test_stub_csp_tds_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (tds_supported);
}/* lte_rrc_capabilities_test_stub_csp_tdscdma_do_is_supported */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_set_w_is_supported

===========================================================================*/
/*!
    @brief
    Function for setting w_supported

    @return

*/
/*=========================================================================*/
void 
lte_rrc_capabilities_test_set_w_is_supported(boolean my_flag)
{
  TF_MSG("Stub was invoked");

  w_supported = my_flag;
}/* lte_rrc_capabilities_test_set_cdma_do_is_supported */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_set_tds_is_supported

===========================================================================*/
/*!
    @brief
    Function for setting tds_supported

    @return

*/
/*=========================================================================*/
void 
lte_rrc_capabilities_test_set_tds_is_supported(boolean my_flag)
{
  TF_MSG("Stub was invoked");

  tds_supported = my_flag;
}/* lte_rrc_capabilities_test_set_tds_is_supported */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_gsm_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_gsm_is_supported

    @return
    Is GSM supported ? (in the Capabilities test framework)

*/
/*=========================================================================*/
static boolean 
lte_rrc_capabilities_test_stub_csp_gsm_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (gsm_supported);
}/* lte_rrc_capabilities_test_stub_csp_cdma_gsm_is_supported */


/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_set_gsm_is_supported

===========================================================================*/
/*!
    @brief
    Function for setting gsm_supported

    @return

*/
/*=========================================================================*/
void 
lte_rrc_capabilities_test_set_gsm_is_supported(boolean my_flag)
{
  TF_MSG("Stub was invoked");

  gsm_supported = my_flag;
}/* lte_rrc_capabilities_test_set_gsm_is_supported */


/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_set_cdma_1x_is_supported

===========================================================================*/
/*!
    @brief
    Function for setting cdma_do_supported

    @return

*/
/*=========================================================================*/
void 
lte_rrc_capabilities_test_set_cdma_1x_is_supported(boolean my_flag)
{
  TF_MSG("Stub was invoked");

  cdma_1x_supported = my_flag;
}/* lte_rrc_capabilities_test_set_cdma_do_is_supported */


/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_cdma_1x_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_cdma_1x_is_supported

    @return
    Is 1X supported ? (in the Capabilities test framework)

*/
/*=========================================================================*/
boolean 
lte_rrc_capabilities_test_stub_csp_cdma_1x_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (cdma_1x_supported);
}/* lte_rrc_capabilities_test_stub_csp_get_cdma_band_preference */



/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_set_do_band_preference

===========================================================================*/
/*!
    @brief
    Set the desired output of do_band_preference 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_stub_csp_set_do_band_preference
(
  sys_band_mask_type do_band_preference /*!< Desired LTE band preference */
)
{
  lte_rrc_capabilities_test_do_band_preference = do_band_preference;
}/* lte_rrc_capabilities_test_stub_csp_set_do_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_set_1x_band_preference

===========================================================================*/
/*!
    @brief
    Set the desired output of 1x_band_preference 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_stub_csp_set_1x_band_preference
(
  sys_band_mask_type band_preference /*!< Desired LTE band preference */
)
{
  lte_rrc_capabilities_test_1x_band_preference = band_preference;
}/* lte_rrc_capabilities_test_stub_csp_set_1x_band_preference */


/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_set_wcdma_band_preference

===========================================================================*/
/*!
    @brief
    Set the desired output of wcdma_band_preference 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_stub_csp_set_wcdma_band_preference
(
  sys_band_mask_type wcdma_band_preference /*!< Desired LTE band preference */
)
{
  lte_rrc_capabilities_test_wcdma_band_preference = wcdma_band_preference;
}/* lte_rrc_capabilities_test_stub_csp_set_wcdma_band_preference */

/*===========================================================================*/
/*!
    @brief
    Set the desired output of tdscdma_band_preference 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_stub_csp_set_tdscdma_band_preference
(
  sys_band_mask_type tdscdma_band_preference /*!< Desired LTE band preference */
)
{
  lte_rrc_capabilities_test_tdscdma_band_preference = tdscdma_band_preference;
}/* lte_rrc_capabilities_test_stub_csp_set_tdscdma_band_preference */

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_set_gsm_band_preference

===========================================================================*/
/*!
    @brief
    Set the desired output of gsm_band_preference 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_stub_csp_set_gsm_band_preference
(
  sys_band_mask_type gsm_band_preference /*!< Desired GSM band preference */
)
{
  lte_rrc_capabilities_test_gsm_band_preference = gsm_band_preference;
}/* lte_rrc_capabilities_test_stub_csp_set_gsm_band_preference */



/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_csp_set_cdma_band_preference

===========================================================================*/
/*!
    @brief
    Set the desired output of cdma_band_preference 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_stub_csp_set_lte_band_preference
(
  sys_lte_band_mask_e_type lte_band_preference /*!< Desired LTE band preference */
)
{
  lte_rrc_capabilities_test_lte_band_preference = lte_band_preference;
}/* lte_rrc_capabilities_test_stub_csp_set_lte_band_preference */

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_stub_rohc_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns if ROHC is supported

    @return
    boolean, TRUE means support is present.

*/
/*=========================================================================*/
boolean lte_rrc_capabilities_test_stub_rohc_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_capabilities_test_stub_rohc_is_supported */
#if 0
/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_stub_tds_rat_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns if TDS is supported

    @return
    boolean, TRUE means support is present.

*/
/*=========================================================================*/
boolean lte_rrc_capabilities_test_stub_tds_rat_is_supported
(
  sys_plmn_id_s_type camped_plmn
)
{
  LTE_RRC_UNUSED(camped_plmn);
  return TRUE;

} /* lte_rrc_capabilities_test_stub_tds_rat_is_supported */ 
#endif 

/*===========================================================================

   FUNCTION:  lte_rrc_capabilities_test_stub_init

===========================================================================*/
/*!
    @brief
    Initializes the stub required for Capabilities unit test 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_stub_init()
{
  TF_STUB(lte_rrc_csp_get_lte_band_preference) = 
  lte_rrc_capabilities_test_stub_csp_get_lte_band_preference;

  //TF_STUB(lte_rrc_csp_get_do_band_preference) = 
  //lte_rrc_capabilities_test_stub_csp_get_do_band_preference;

  //TF_STUB(lte_rrc_csp_get_1x_band_preference) = 
  //lte_rrc_capabilities_test_stub_csp_get_1x_band_preference;

  //TF_STUB(lte_rrc_csp_get_wcdma_band_preference) = 
  //lte_rrc_capabilities_test_stub_csp_get_wcdma_band_preference;

  //TF_STUB(lte_rrc_csp_get_tdscdma_band_preference) = 
  //lte_rrc_capabilities_test_stub_csp_get_tdscdma_band_preference;

 // TF_STUB(lte_rrc_csp_get_gsm_band_preference) = 
 // lte_rrc_capabilities_test_stub_csp_get_gsm_band_preference;

  //TF_STUB(lte_rrc_csp_cdma_do_is_supported) =
  //lte_rrc_capabilities_test_stub_csp_cdma_do_is_supported;
 
 // TF_STUB(lte_rrc_csp_cdma_1x_is_supported) =
  //lte_rrc_capabilities_test_stub_csp_cdma_1x_is_supported;

  TF_STUB(lte_rrc_controller_get_state)=
  lte_rrc_capabilities_test_stub_controller_get_state;

  //TF_STUB(lte_rrc_csp_cap_wcdma_is_supported) =
  //lte_rrc_capabilities_test_stub_csp_w_is_supported;

 // TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) =
  //lte_rrc_capabilities_test_stub_csp_tds_is_supported;

 // TF_STUB(lte_rrc_csp_gsm_is_supported) =
 // lte_rrc_capabilities_test_stub_csp_gsm_is_supported;

//  TF_STUB(lte_rrc_rohc_is_supported) = 
  //lte_rrc_capabilities_test_stub_rohc_is_supported;

}/* lte_rrc_capabilities_test_stub_init */

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_construct_cap_enq_msg

===========================================================================*/
/*!
    @brief
    Constructs a Capability Enquiry DLM by dynamic allocation 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_construct_cap_enq_msg
(
  lte_rrc_mh_dlm_s *msg_ptr /*!< Pointer to message */
)
{
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr;
  lte_rrc_osys_UE_CapabilityRequest *cap_ptr;

  lte_rrc_init_default_hdr(&msg_ptr->msg_hdr,
                           LTE_RRC_UE_CAPABILITY_ENQUIRY_DLM);

  //msg_ptr->sfn = 100;
  msg_ptr->pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  memset(&msg_ptr->cell_id,0,sizeof(msg_ptr->cell_id));

  msg_ptr->dl_msg_ptr = lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_Message));
  TF_ASSERT(msg_ptr->dl_msg_ptr != NULL);
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *) msg_ptr->dl_msg_ptr;

  dl_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_c1;
  /* Allocate memory for lte_rrc_osys_DL_DCCH_MessageType_c1 */
  dl_msg_ptr->message.u.c1 = 
    (lte_rrc_osys_DL_DCCH_MessageType_c1 *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_MessageType_c1));
  dl_msg_ptr->message.u.c1->t = T_lte_rrc_osys_DL_DCCH_MessageType_c1_ueCapabilityEnquiry;

  /* Allocate memory for lte_rrc_osys_UECapabilityEnquiry */
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry = 
    (lte_rrc_osys_UECapabilityEnquiry *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_UECapabilityEnquiry));
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->rrc_TransactionIdentifier = 1;
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.t =
    T_lte_rrc_osys_UECapabilityEnquiry_criticalExtensions_c1;

  /* Allocate memory for lte_rrc_osys_UECapabilityEnquiry_criticalExtensions_c1 */
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.c1 = 
    (lte_rrc_osys_UECapabilityEnquiry_criticalExtensions_c1 *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_UECapabilityEnquiry_criticalExtensions_c1));
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.c1->t
    = T_lte_rrc_osys_UECapabilityEnquiry_criticalExtensions_c1_ueCapabilityEnquiry_r8;

  /* Allocate memory for lte_rrc_osys_UECapabilityEnquiry_r8_IEs */
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.c1->u.
    ueCapabilityEnquiry_r8 = 
    (lte_rrc_osys_UECapabilityEnquiry_r8_IEs *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_UECapabilityEnquiry_r8_IEs));
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.c1->u.
    ueCapabilityEnquiry_r8->m.nonCriticalExtensionPresent = 0;

  /* Allocate memory for lte_rrc_osys_RAT_Type */
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.c1->u.
    ueCapabilityEnquiry_r8->ue_CapabilityRequest.n = 1;
  dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.c1->u.
    ueCapabilityEnquiry_r8->ue_CapabilityRequest.elem = 
    (lte_rrc_osys_RAT_Type *)
      lte_rrc_asn1_osys_IE_malloc(
        dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.c1->u.
          ueCapabilityEnquiry_r8->ue_CapabilityRequest.n * 
        sizeof(lte_rrc_osys_RAT_Type));
  cap_ptr = &(dl_msg_ptr->message.u.c1->u.ueCapabilityEnquiry->criticalExtensions.u.
            c1->u.ueCapabilityEnquiry_r8->ue_CapabilityRequest);
  TF_ASSERT(cap_ptr != NULL);
  *(cap_ptr->elem) = lte_rrc_osys_RAT_Type_eutra;
} /* lte_rrc_capabilities_test_construct_cap_enq_msg */

