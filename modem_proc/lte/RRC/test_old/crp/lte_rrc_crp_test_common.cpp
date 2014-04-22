/*!
  @file lte_rrc_crp_test_common.cpp

  @brief
  Common utilities for CRP test

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
09/13/08   sureshs Renamed some functions
09/12/08   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_controller.h>
#include <lte_rrc_csp.h>
#include <lte_rrc_utils.h>
#include <geran_grr_msg.h>
}

#include <tf_stub.h>
#include <TestFramework.h>
#include <lte_rrc_meas_test_msg.h>

extern "C"
{
TF_PROTO_STUB(stm_state_t,lte_rrc_controller_get_state,void);
TF_PROTO_STUB(stm_state_t,lte_rrc_cep_get_state,void);
TF_PROTO_STUB(boolean,lte_rrc_csp_lte_band_is_supported,sys_sband_lte_e_type);
//TF_PROTO_STUB(boolean,lte_rrc_csp_cdma_do_is_supported,void);
//TF_PROTO_STUB(boolean,lte_rrc_csp_cdma_1x_is_supported,void);
//TF_PROTO_STUB(boolean,lte_rrc_csp_gsm_is_supported,void);
////TF_PROTO_STUB(boolean,lte_rrc_csp_cdma_do_band_is_supported,sys_band_class_e_type);
//TF_PROTO_STUB(boolean,lte_rrc_csp_gsm_band_is_supported,sys_sband_e_type);
TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_selected_plmn, lte_rrc_plmn_s *, uint32 *);
TF_PROTO_STUB(boolean, lte_rrc_sib_b12_to_b17_conversion_is_needed, void);
//TF_PROTO_STUB(sys_sband_e_type, lte_rrc_get_gsm_band_from_arfcn, geran_grr_band_ind_e, uint16, boolean *);
TF_PROTO_STUB( lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);

}

/*! @brief Controller state for unit test
*/
static stm_state_t lte_rrc_crp_test_controller_state;

/*! @brief Cep state for unit test
*/
static stm_state_t lte_rrc_crp_test_cep_state;

/*! @brief CSP DO is supported or not
*/
static boolean lte_rrc_crp_test_do_is_supported;

/*! @brief CSP 1x is supported or not
*/
static boolean lte_rrc_crp_test_1x_is_supported;

/*! @brief CSP GSM is supported or not
*/
static boolean lte_rrc_crp_test_gsm_is_supported;

/*! @brief CSP WCDMA is supported or not
*/
static boolean lte_rrc_crp_test_wcdma_is_supported;

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_controller_get_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_controller_get_state 

    @return
    Returns the RRC controller state (in the CRP test framework)

*/
/*=========================================================================*/
static stm_state_t lte_rrc_crp_test_stub_controller_get_state(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_crp_test_controller_state);
}/*lte_rrc_crp_test_stub_controller_get_state*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_cep_get_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_cep_get_state 

    @return
    Returns the RRC cep state (in the CRP test framework)

*/
/*=========================================================================*/
static stm_state_t lte_rrc_crp_test_stub_cep_get_state(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_crp_test_cep_state);
}/*lte_rrc_crp_test_stub_cep_get_state*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_do_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_cdma_do_is_supported 

    @return
    Returns whether DO is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_do_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_crp_test_do_is_supported);
}/*lte_rrc_crp_test_stub_csp_do_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_1x_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_cdma_1x_is_supported 

    @return
    Returns whether 1X is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_1x_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_crp_test_1x_is_supported);
}/*lte_rrc_crp_test_stub_csp_1x_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_gsm_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_gsm_is_supported 

    @return
    Returns whether GSM is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_gsm_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_crp_test_gsm_is_supported);
}/*lte_rrc_crp_test_stub_csp_gsm_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_wcdma_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_wcdma_is_supported 

    @return
    Returns whether WCDMA is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_wcdma_is_supported(void)
{
  TF_MSG("Stub was invoked");

  return (lte_rrc_crp_test_wcdma_is_supported);
}/*lte_rrc_crp_test_stub_csp_wcdma_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_lte_band_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_lte_band_is_supported 

    @return
    Returns whether LTE band is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_lte_band_is_supported
(
  sys_sband_lte_e_type band /*!< Band */
)
{
  LTE_RRC_UNUSED(band);

  TF_MSG("Stub was invoked");

  return (TRUE);
}/*lte_rrc_crp_test_stub_csp_lte_band_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_cdma_1x_band_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_cdma_1x_band_is_supported 

    @return
    Returns whether CDMA 1X band is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_cdma_1x_band_is_supported
(
  sys_band_class_e_type band /*!< Band */
)
{
  LTE_RRC_UNUSED(band);

  TF_MSG("Stub was invoked");

  return (TRUE);
}/*lte_rrc_crp_test_stub_csp_cdma_1x_band_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_cdma_do_band_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_cdma_do_band_is_supported 

    @return
    Returns whether CDMA DO band is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_cdma_do_band_is_supported
(
  sys_band_class_e_type band /*!< Band */
)
{
  LTE_RRC_UNUSED(band);

  TF_MSG("Stub was invoked");

  return (TRUE);
}/*lte_rrc_crp_test_stub_csp_cdma_do_band_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_gsm_band_is_supported

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_csp_gsm_band_is_supported 

    @return
    Returns whether GSM band is supported

*/
/*=========================================================================*/
static boolean lte_rrc_crp_test_stub_csp_gsm_band_is_supported
(
  sys_sband_e_type band /*!< Band */
)
{
  LTE_RRC_UNUSED(band);

  TF_MSG("Stub was invoked");

  return (TRUE);
}/*lte_rrc_crp_test_stub_csp_gsm_band_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_controller_set_state

===========================================================================*/
/*!
    @brief
    Set the desired output of lte_rrc_controller_get_state 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_stub_controller_set_state
(
  stm_state_t state /*!< Desired state */
)
{
  lte_rrc_crp_test_controller_state = state;
}/*lte_rrc_crp_test_stub_controller_set_state*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_cep_set_state

===========================================================================*/
/*!
    @brief
    Set the desired output of lte_rrc_cep_get_state 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_stub_cep_set_state
(
  stm_state_t state /*!< Desired state */
)
{
  lte_rrc_crp_test_cep_state = state;
}/*lte_rrc_crp_test_stub_cep_set_state*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_set_do_is_upported

===========================================================================*/
/*!
    @brief
    Set whether DO is supported or not

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_stub_csp_set_do_is_supported
(
  boolean do_is_supported /*!< Supported or not */
)
{
  lte_rrc_crp_test_do_is_supported = do_is_supported;
}/*lte_rrc_crp_test_stub_csp_set_do_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_set_1x_is_upported

===========================================================================*/
/*!
    @brief
    Set whether 1x is supported or not

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_stub_csp_set_1x_is_supported
(
  boolean cdma_1x_is_supported /*!< Supported or not */
)
{
  lte_rrc_crp_test_1x_is_supported = cdma_1x_is_supported;
}/*lte_rrc_crp_test_stub_csp_set_1x_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_set_gsm_is_upported

===========================================================================*/
/*!
    @brief
    Set whether GSM is supported or not

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_stub_csp_set_gsm_is_supported
(
  boolean gsm_is_supported /*!< Supported or not */
)
{
  lte_rrc_crp_test_gsm_is_supported = gsm_is_supported;
}/*lte_rrc_crp_test_stub_csp_set_gsm_is_supported*/

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_csp_set_wcdma_is_upported

===========================================================================*/
/*!
    @brief
    Set whether WCDMA is supported or not

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_stub_csp_set_wcdma_is_supported
(
  boolean wcdma_is_supported /*!< Supported or not */
)
{
  lte_rrc_crp_test_wcdma_is_supported = wcdma_is_supported;
}/*lte_rrc_crp_test_stub_csp_set_wcdma_is_supported*/

/*===========================================================================

  FUNCTION:  lte_rrc_crp_test_stub_csp_get_selected_plmn

===========================================================================*/
/*!
  @brief
  Stub func for getting selected PLMN

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_crp_test_stub_csp_get_selected_plmn
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

} /* lte_rrc_crp_test_stub_csp_get_selected_plmn */

/*===========================================================================

  FUNCTION:  lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_needed

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_needed
(
  void
)
{
  return TRUE;

} /* lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_needed() */

/*===========================================================================

  FUNCTION:  lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_not_needed

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_not_needed
(
  void
)
{
  return FALSE;

} /* lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_not_needed() */

/*===========================================================================

  FUNCTION:  lte_rrc_crp_test_stub__get_3gpp_release_version

===========================================================================*/
/*!
    @brief
    This function simulates the get 3gpp release version function.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_crp_test_stub__get_3gpp_release_version
(
  void
)
{
  return LTE_3GPP_REL10;

} /* lte_rrc_crp_test_stub__get_3gpp_release_version */

/*===========================================================================

   FUNCTION:  lte_rrc_crp_test_stub_init

===========================================================================*/
/*!
    @brief
    Initializes the stub required for CRP unit test 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_stub_init()
{
  TF_STUB(lte_rrc_controller_get_state) = 
  lte_rrc_crp_test_stub_controller_get_state;
  TF_STUB(lte_rrc_cep_get_state) = 
  lte_rrc_crp_test_stub_cep_get_state;
  //TF_STUB(lte_rrc_csp_cdma_do_is_supported) = 
  //lte_rrc_crp_test_stub_csp_do_is_supported;
  //TF_STUB(lte_rrc_csp_cdma_1x_is_supported) = 
  //lte_rrc_crp_test_stub_csp_1x_is_supported;
 // TF_STUB(lte_rrc_csp_gsm_is_supported) = 
  //lte_rrc_crp_test_stub_csp_gsm_is_supported;
  //TF_STUB(lte_rrc_csp_wcdma_is_supported) = 
  //lte_rrc_crp_test_stub_csp_wcdma_is_supported;
  TF_STUB(lte_rrc_csp_lte_band_is_supported) = 
  lte_rrc_crp_test_stub_csp_lte_band_is_supported;
  //TF_STUB(lte_rrc_csp_cdma_1x_band_is_supported) = 
  //lte_rrc_crp_test_stub_csp_cdma_1x_band_is_supported;
 // TF_STUB(lte_rrc_csp_cdma_do_band_is_supported) = 
  //lte_rrc_crp_test_stub_csp_cdma_do_band_is_supported;
  //TF_STUB(lte_rrc_csp_gsm_band_is_supported) = 
  //lte_rrc_crp_test_stub_csp_gsm_band_is_supported;
  TF_STUB(lte_rrc_sib_b12_to_b17_conversion_is_needed) =
  lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_not_needed;
  //TF_STUB(lte_rrc_get_gsm_band_from_arfcn) = 
    //lte_rrc_meas_test_stub__get_gsm_band_from_arfcn;
  TF_STUB(lte_rrc_get_3gpp_release_version) = 
    lte_rrc_crp_test_stub__get_3gpp_release_version;
}/*lte_rrc_crp_test_stub_init*/

