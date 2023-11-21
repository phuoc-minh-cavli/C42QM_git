/*! \file rr_init.c

  This module contains RR initialisaton.

                Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_init.c#8 $ */
/* $DateTime: 2023/02/27 00:36:51 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_init.h"
#include "aconfig.h"
#include "rr_cell_selection.h"
#include "rr_general.h"
#include "rr_static_general.h"
#include "gs.h"

#include "rr_mm_send.h"
#include "rr_shared_cell_selection.h"
#include "rr_inter_ho.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "rr_control.h"
#include "rr_l2_l1.h"
#include "rr_mac_send.h"
#include "rr_acq_db.h"
#include "rr_cipher.h"
#include "rr_channel_mode.h"
#include "rr_conn_establish.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_sys_info_si.h"
#include "rr_hz.h"

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
#include "gprs_odm_info.h"
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
#include "rr_callback.h"
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/

#include "rr_sys_info_pcell.h"
#include "rr_candidate_cell_db.h"
#include "rr_select_bcch.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
#include "rr_x2g_resel.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_FEMTO_GSM_NL
#include "rr_femto.h"
#endif /* FEATURE_FEMTO_GSM_NL */

#ifdef FEATURE_WCDMA
#include "rr_g2w.h"
#include "rr_g2w_handover.h"
#endif /* FEATURE_WCDMA */


#include "rr_l1_send.h"
#include "rr_cell_reestablish.h"
#include "rr_gprs_meas_rpt_control.h"
#include "rr_main.h"
#include "fs_lib.h"
#include "rr_ded_meas_reporting.h"
#include "rr_resel.h"
#include "rr_gprs_conn_establish.h"
#include "rr_sys_info_pacch.h"
#include "rr_process_pmo_pcco.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif // FEATURE_GSM_EDTM
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/
#include "rr_ded_config.h"
#include "rr_plmn_list.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
#include "rr_resel_g2w.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE */

#include "rr_grr.h"
#include "rr_cell_acq.h"
#include "rr_l1_idle_mode.h"
#include "rr_ps_domain.h"
#include "rr_ps_access.h"

#include "rr_resel_nacc.h"

#include "rr_gprs_main.h"
#include "rr_thermal.h"
#include "rr_msc.h"
#include "rr_pscan.h"
#include "rr_ce_paging.h"
#include "rr_conn_release.h"
#include "rr_multi_sim.h"
#include "rr_public_store.h"
#include "rr_test.h"
#include "rr_log.h"
#include "rr_gprs_debug.h"
#include "geran_grr_dyn_api.h"
#ifdef FEATURE_GPRS_PS_HANDOVER
#include "rr_psho.h"
#endif // FEATURE_GPRS_PS_HANDOVER

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_cgps.h"
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

#include "geran_nv.h"

#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) 
#include "rr_reduced_mobility.h"
#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef ie_descriptor_T  ie_store_t[MAXNO_RR_IE_DESCS];

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static ie_store_t        ie_store;
static rr_nv_recovery_restart_type_T rr_nv_recovery_restart;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param 
 *
 * \return ie_descriptor_T*
 */
static ie_descriptor_T *rr_get_ie_store_ptr( void )
{
  return ie_store;
}

/*!
 * \brief Store the message parameter in the message_list store.
 *
 * \param message_type
 * \param imperative_length
 * \param l2_channel
 * \param first_IE
 */
static void store_msg_params(
  byte message_type,
  byte imperative_length,
  l2_channel_type_T l2_channel,
  ie_descriptor_T *first_IE
   
)
{
  message_list_T  *message_list_ptr = rr_get_message_list_ptr();
  byte            *max_message_index_ptr = rr_get_max_message_index_ptr();

  if (*max_message_index_ptr >= rr_elements_in_message_list())
  {
    if (geran_get_nv_recovery_restart_enabled_dyn())
    {
      MSG_GERAN_ERROR_1_G("RR init failed index overflow, max_message_index:0x%x,Triggering PANIC RESET",*max_message_index_ptr);
      geran_initiate_recovery_restart_dyn(GERAN_CLIENT_GRR);
    }
    else
    {
      ERR_GERAN_FATAL_1_G("RR init failed index overflow, max_message_index:0x%x",*max_message_index_ptr);
    }
  }

  message_list_ptr[*max_message_index_ptr].message_type      = message_type;
  message_list_ptr[*max_message_index_ptr].imperative_length = imperative_length;
  message_list_ptr[*max_message_index_ptr].l2_channel        = l2_channel;
  message_list_ptr[*max_message_index_ptr].first_IE          = first_IE;

  (*max_message_index_ptr)++;
}

/*!
 * \brief Inserts an IE descriptor into the list of IE
 *
 * \param new_descriptor
 * \param IEI
 * \param presence
 * \param IE_length
 * \param IE_format
 * \param start_IE
 * \param next_IE
 *
 * \return ie_descriptor_T*
 */
static ie_descriptor_T *insert_ie_descriptor(
  ie_descriptor_T *new_descriptor,
  byte             IEI,
  byte             presence,
  byte             IE_length,
  format_T         IE_format,
  byte            *start_IE,
  ie_descriptor_T *next_IE
)
{
  new_descriptor->IEI       = IEI;
  new_descriptor->presence  = presence;
  new_descriptor->IE_length = IE_length;
  new_descriptor->IE_format = IE_format;
  new_descriptor->start_IE  = start_IE;
  new_descriptor->next_IE   = next_IE;

  return(new_descriptor);
}

/*!
 * \brief Initialise the message list. For each message, insert the description of it.
 *
 * \param 
 */
static void rr_initialise_msg_list( void )
{
  ie_descriptor_T *previous_IE = NULL;
  ie_descriptor_T *new_IE;
  word             ie_count;
  byte            *max_message_index_ptr = rr_get_max_message_index_ptr();
  ie_descriptor_T *ie_store_ptr = rr_get_ie_store_ptr();

  /****************************************/
  /* Initialise the message and IE counts.*/
  /****************************************/
  *max_message_index_ptr = 0;
  ie_count          = 0;

  /*********************************************/
  /* Defines the message ADDITIONAL_ASSIGNMENT */
  /*********************************************/

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x7C, OPTIONAL_IE, 0 , TV,  NULL, previous_IE);


  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x72, CONDITIONAL_IE, 0, TLV, NULL, previous_IE);


  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00,  MANDATORY_IE, 3, V,   NULL, previous_IE);

  store_msg_params(ADDITIONAL_ASSIGNMENT, 5, DCCH, new_IE);

  /***********************************************/
  /* Defines the message APPLICATION INFORMATION */
  /***********************************************/
  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0 , LV,  NULL, previous_IE);


  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x0, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(APPLICATION_INFORMATION, 5, DCCH, new_IE);

  /******************************************/
  /* Defines the message ASSIGNMENT_COMMAND */
  /******************************************/

  previous_IE = NULL;

  /* Multi-rate Configuration */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x03, OPTIONAL_IE, 8,  TLV,  NULL, previous_IE);

  /*********************************************************************************/
  /* VGCS Target Mode Indication IE is marked as comprehension required and should */
  /* trigger an exception if present for a mobile not supporting VGCS talking      */
  /* see 04.18 9.1.2.9... therefore, don't include in message description          */
  /* new_IE = &ie_store_ptr[ie_count];                                                 */
  /* ie_count++;                                                                   */
  /* insert_ie_descriptor(new_IE, 0x01, OPTIONAL_IE, 3,  TLV,  NULL, previous_IE); */
  /* previous_IE = new_IE;                                                         */
  /*********************************************************************************/

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x90, OPTIONAL_IE, 1,  TV,  NULL, previous_IE);

  /* Mobile Allocation, before time - Max length 10*/
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x21, CONDITIONAL_IE, 10,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x1E, CONDITIONAL_IE, 10, TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x1D, OPTIONAL_IE, 4,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x1C, OPTIONAL_IE, 4,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x19, CONDITIONAL_IE, 0,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x7C, OPTIONAL_IE, 3,  TV,  NULL, previous_IE);

  /* Mobile Allocation, before time - Max length 10*/
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x72, CONDITIONAL_IE, 10,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x66, OPTIONAL_IE, 2,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x64, OPTIONAL_IE, 4,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x63, OPTIONAL_IE, 2,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x62, OPTIONAL_IE, 17, TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x05, CONDITIONAL_IE, 0,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1,  V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3,  V,   NULL, previous_IE);

  store_msg_params( ASSIGNMENT_COMMAND, 6, DCCH, new_IE);

  /*******************************************/
  /* Defines the message CHANNEL_MODE_MODIFY */
  /*******************************************/

  previous_IE = NULL;

  /* Multi-rate Configuration*/
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x03, OPTIONAL_IE, 8,  TLV,  NULL, previous_IE);

  /*********************************************************************************/
  /* VGCS Target Mode Indication IE is marked as comprehension required and should */
  /* trigger an exception if present for a mobile not supporting VGCS talking      */
  /* see 04.18 9.1.5.2... therefore, don't include in message description          */
  /* new_IE = &ie_store_ptr[ie_count];                                                 */
  /* ie_count++;                                                                   */
  /* insert_ie_descriptor(new_IE, 0x01, OPTIONAL_IE, 3,  TLV,  NULL, previous_IE); */
  /* previous_IE = new_IE;                                                         */
  /*********************************************************************************/

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  store_msg_params( CHANNEL_MODE_MODIFY, 6, DCCH, new_IE);

  /***************************************/
  /* Defines the message CHANNEL_RELEASE */
  /***************************************/

  previous_IE = NULL;

  /* Individual Priorities */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, INDIVIDUAL_PRIORITIES_IEI, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* Enhanced DTM CS Release */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0xA0, OPTIONAL_IE, 1, TV, NULL, previous_IE);

  /* Cell Selection Indicator */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x77, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* Cell Channel Description */
  /* 3GPP 44.018, 9.1.7.4:
     Mobile stations not supporting VGCS talking or VGCS originating shall
     ignore this information element if present. */

  /* UTRAN Freq List */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x76, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* BA List Pref */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x75, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* GPRS Resumption */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0xC0, OPTIONAL_IE, 1, TV, NULL, previous_IE);

  /* Group Cipher Key Number */
  /* 3GPP 44.018 section 9.1.7.2:
     This IE may be present only if the Group channel description IE or the
     Group Channel Description 2 IE is provided.

     As neither Group Channel Description nor Group Channel Description 2 IEs
     are supported, this IE can be ignored. */

  /* Group Channel Description */
  /* 3GPP 44.018 section 9.1.7.1:
     Mobile stations not supporting VGCS listening or VBS listening shall
     ignore the group channel description (or group channel description 2)
     information element if present. */

  /* BA Range */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x73, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* RR Cause */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,   NULL, previous_IE);

  store_msg_params(CHANNEL_RELEASE, 3, DCCH, new_IE);

  /**********************************************/
  /* Defines the message CIPHERING_MODE_COMMAND */
  /**********************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(CIPHERING_MODE_COMMAND, 3, DCCH, new_IE);

  /*****************************************/
  /* Defines the message CLASSMARK_ENQUIRY */
  /*****************************************/
  /* IE 0x10:CLASSMARK_ENQUIRY_MASK */
  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x10, OPTIONAL_IE, 3, TLV, NULL, previous_IE);

  store_msg_params(CLASSMARK_ENQUIRY, 2, DCCH, previous_IE);

  /**********************************************/
  /* Defines the message FREQUENCY_REDEFINITION */
  /**********************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x62, OPTIONAL_IE, 17, TV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 2,  V,  NULL, previous_IE);

  /* mobile allocation */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 9,  LV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3,  V,  NULL, previous_IE);

  store_msg_params(FREQUENCY_REDEFINITION, 8, DCCH, previous_IE);

  /****************************************/
  /* Defines the message HANDOVER_COMMAND */
  /****************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;

  /* Multi-rate Configuration - Max length allowed = 8 */
  previous_IE = insert_ie_descriptor(new_IE, 0x03, OPTIONAL_IE, 8,  TLV,  NULL, previous_IE);

  /*********************************************************************************/
  /* VGCS Target Mode Indication IE is marked as comprehension required and should */
  /* trigger an exception if present for a mobile not supporting VGCS talking      */
  /* see 04.18 9.1.15.11... therefore, don't include in message description        */
  /* new_IE = &ie_store_ptr[ie_count];                                                 */
  /* ie_count++;                                                                   */
  /* insert_ie_descriptor(new_IE, 0x01, OPTIONAL_IE, 3,  TLV,  NULL, previous_IE); */
  /* previous_IE = new_IE;                                                         */
  /*********************************************************************************/

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x90, OPTIONAL_IE, 1,  TV,  NULL, previous_IE);

  /* Mobile Allocation, before time */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x21, CONDITIONAL_IE, 10,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x1E, CONDITIONAL_IE, 10, TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x1D, OPTIONAL_IE, 4,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x1C, OPTIONAL_IE, 4,  TV,  NULL, previous_IE);

  /* Frequency List, Before Time Max Length: 131*/
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x19, CONDITIONAL_IE, 131,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x12, CONDITIONAL_IE, 10, TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x7D, CONDITIONAL_IE, 2,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x7B, CONDITIONAL_IE, 3,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x7C, OPTIONAL_IE, 3,  TV,  NULL, previous_IE);

  /* Mobile Allocation, after time */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x72, CONDITIONAL_IE, 10,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x69, CONDITIONAL_IE, 10, TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x66, OPTIONAL_IE, 2,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x64, OPTIONAL_IE, 4,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x63, OPTIONAL_IE, 2,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x62, CONDITIONAL_IE, 17, TV,  NULL, previous_IE);

  /* Frequency List, After Time  Max Length: 131*/
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x05, CONDITIONAL_IE, 131,  TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x02, CONDITIONAL_IE, 10, TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0xD0, OPTIONAL_IE, 1,  TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1,  V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1,  V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3,  V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 2,  V,   NULL, previous_IE);

  store_msg_params(HANDOVER_COMMAND, 9, DCCH, previous_IE);

  /********************************************/
  /* Defines the message IMMEDIATE_ASSIGNMENT */
  /********************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x7C, OPTIONAL_IE, 3, TV, NULL, previous_IE);

  /* Mobile Allocation */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 9, LV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,  NULL, previous_IE);

  store_msg_params(IMMEDIATE_ASSIGNMENT, 23, CCCH, previous_IE);

  /*****************************************************/
  /* Defines the message IMMEDIATE_ASSIGNMENT_EXTENDED */
  /*****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x7C, OPTIONAL_IE, 3, TV, NULL, previous_IE);

  /* mobile allocation */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 5, LV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,  NULL, previous_IE);

  store_msg_params(IMMEDIATE_ASSIGNMENT_EXTENDED, 23, CCCH, previous_IE);

  /****************************************************/
  /* Defines the message  IMMEDIATE_ASSIGNMENT_REJECT */
  /****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(IMMEDIATE_ASSIGNMENT_REJECT, 23, CCCH, previous_IE);

  /*********************************************/
  /* Defines the message PAGING_REQUEST_TYPE_1 */
  /*********************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x17, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, LV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,   NULL, previous_IE);

  store_msg_params(PAGING_REQUEST_TYPE_1, 23, CCCH, previous_IE);

  /*********************************************/
  /* Defines the message PAGING_REQUEST_TYPE_2 */
  /*********************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x17, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V,   NULL, previous_IE);

  store_msg_params(PAGING_REQUEST_TYPE_2, 23, CCCH, previous_IE);

  /*********************************************/
  /* Defines the message PAGING_REQUEST_TYPE_3 */
  /*********************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(PAGING_REQUEST_TYPE_3, 23, CCCH, previous_IE);

  /***************************************/
  /* Defines the message PARTIAL RELEASE */
  /***************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  store_msg_params(PARTIAL_RELEASE, 5, DCCH, previous_IE);

  /*********************************************/
  /* Defines the message  PHYSICAL_INFORMATION */
  /*********************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(PHYSICAL_INFORMATION, 3, DCCH, previous_IE);

  /*********************************/
  /* Defines the message RR_STATUS */
  /*********************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  /*
  04.18 does not explicitly state that the RR_STATUS message may only be
  received on the DCCH, but RR_STATUS is only sent by the network if an RR
  RR connection exists so this seems appropriate.
  */
  store_msg_params(RR_STATUS, 3, DCCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_1 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1,  V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3,  V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 16, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_1, 23, BCCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_2 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3,  V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1,  V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 16, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_2, 23, BCCH, previous_IE);

  /*****************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_2_BIS */
  /*****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1,  V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3,  V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 16, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_2_BIS, 23, BCCH, previous_IE);

  /*****************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_2_TER */
  /*****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4,  V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 16, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_2_TER, 23, BCCH, previous_IE);

  /*****************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_2_QUATER */
  /*****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 20,  V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_2_QUATER, 23, BCCH, previous_IE);

  /*****************************************************/
  /* Defines the message MEASUREMENT_INFORMATION_TYPE */
  /* MI has a short header of 1 octet and a rest octet of
    20 */
  /*****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 21,  V, NULL, previous_IE);

  store_msg_params(MEASUREMENT_INFORMATION_TYPE, 21, SACCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_3 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 2, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 5, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 2, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_3, 23, BCCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_4 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, V,   NULL, previous_IE);

  /* CBCH Mobile Allocation */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x72, CONDITIONAL_IE, 6, TLV, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x64, OPTIONAL_IE, 4, TV,  NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 2, V,   NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 5, V,   NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_4, 23, BCCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_5 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 16, V,   NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_5, 18, SACCH, previous_IE);

  /*****************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_5_BIS */
  /*****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 16, V,   NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_5_BIS, 18, SACCH, previous_IE);

  /*****************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_5_TER */
  /*****************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 16, V,   NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_5_TER, 18, SACCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_6 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 7, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 5, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 2, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_6, 11, SACCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_7 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 20, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_7, 23, BCCH, previous_IE);

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_8 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 20, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_8, 23, BCCH, previous_IE);

#ifdef FEATURE_GERAN_EAB
  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_9 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 17, V, NULL, previous_IE);

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 3, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_9, 22, BCCH, previous_IE);
#endif /* FEATURE_GERAN_EAB */

  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_13 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 20, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_13, 0, BCCH, previous_IE);

#ifdef FEATURE_GERAN_EAB
  /*************************************************/
  /* Defines the message SYSTEM_INFORMATION_TYPE_21 */
  /*************************************************/

  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 20, V, NULL, previous_IE);

  store_msg_params(SYSTEM_INFORMATION_TYPE_21, 22, BCCH, previous_IE);
#endif /* FEATURE_GERAN_EAB */

  /**********************************************/
  /* Defines the message DTM_ASSIGNMENT_COMMAND */
  /**********************************************/
  previous_IE = NULL;

  /* Ciphering Mode Setting */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x90, OPTIONAL_IE, 1,  TV,  NULL, previous_IE);

  /* Multi-Rate configuration */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x17, OPTIONAL_IE, 8, TLV, NULL, previous_IE);

  /* Description of the Downlink Packet Channel Assignment */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x16, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* Description of the Uplink Packet Channel Assignment */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x15, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* Mobile Allocation */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x13, CONDITIONAL_IE, 10, TLV, NULL, previous_IE);

  /* Frequency List */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x12, CONDITIONAL_IE, 0, TLV, NULL, previous_IE);

  /* Channel Mode */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x11, OPTIONAL_IE, 2, TV, NULL, previous_IE);

  /* Cell Channel Description */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x10, OPTIONAL_IE, 17, TV, NULL, previous_IE);

  /* GPRS broadcast information */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x0, MANDATORY_IE, 0, LV, NULL, previous_IE);

  /* Description of the CS Channel */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x0, MANDATORY_IE, 3, V, NULL, previous_IE);

  /* CS Power Command */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x0, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(DTM_ASSIGNMENT_COMMAND, 13, DCCH, new_IE);

  /**********************************************/
  /* Defines the message DTM_ASSIGNMENT_FAILURE */
  /**********************************************/
  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(DTM_ASSIGNMENT_FAILURE, 3, DCCH, new_IE);

  /**********************************/
  /* Defines the message DTM_REJECT */
  /**********************************/
  previous_IE = NULL;

  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 1, V, NULL, previous_IE);

  store_msg_params(DTM_REJECT, 3, DCCH, new_IE);

  /***************************************/
  /* Defines the message DTM_INFORMATION */
  /***************************************/
  previous_IE = NULL;

  /* DTM Information Details */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x0, MANDATORY_IE, 4, LV, NULL, previous_IE);

  /* Routeing Area Identification */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x0, MANDATORY_IE, 6, V, NULL, previous_IE);

  store_msg_params(DTM_INFORMATION, 12, DCCH, new_IE);

  /*****************************************/
  /* Defines the message PACKET_ASSIGNMENT */
  /*****************************************/
  previous_IE = NULL;

  /* Description of the Downlink Packet Channel Assignment */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x23, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* Description of the Uplink Packet Channel Assignment */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x22, OPTIONAL_IE, 0, TLV, NULL, previous_IE);

  /* GPRS broadcast information */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x0, MANDATORY_IE, 0, LV, NULL, previous_IE);

  store_msg_params(PACKET_ASSIGNMENT, 9, DCCH, new_IE);

  /*******************************************/
  /* Defines the message PACKET_NOTIFICATION */
  /*******************************************/
  previous_IE = NULL;

  /* Mobile Identity */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x11, CONDITIONAL_IE, 0, TLV, NULL, previous_IE);

  /* Packet TMSI */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x10, CONDITIONAL_IE, 5, TV, NULL, previous_IE);

  store_msg_params(PACKET_NOTIFICATION, 2, DCCH, new_IE);

  /**********************************************/
  /* Defines the message GPRS_INFORMATION       */
  /**********************************************/
  previous_IE = NULL;

  /* LLC PDU Container */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 0, LV, NULL, previous_IE);

  /* TLLI */
  new_IE = &ie_store_ptr[ie_count];
  ie_count++;
  previous_IE = insert_ie_descriptor(new_IE, 0x00, MANDATORY_IE, 4, V, NULL, previous_IE);

  store_msg_params(GPRS_INFORMATION, 8, DCCH, new_IE);

  if ( *max_message_index_ptr > NUMBER_OF_MT_RR_MESSAGES )
  {
    MSG_GERAN_ERROR_2_G("ERROR: Too many RR MT descriptors (%d vs %d)", *max_message_index_ptr, NUMBER_OF_MT_RR_MESSAGES);
  }

  /*lint -e{774}*/
  if ( ie_count > MAXNO_RR_IE_DESCS )
  {
    MSG_GERAN_ERROR_2_G("ERROR: Too many RR IE descriptors (%d vs %d)", ie_count, MAXNO_RR_IE_DESCS);
  }
}


/*
 * Discard all RR temporary data
 */
void discard_all_rr_temporary_data( )
{

  rr_psi_discard_pmo_data();
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_ded_meas_reporting_discard_data();
#endif /* FEATURE_GERAN_CS_SUPPORT */
  rr_tbf_est_discard_data();
#ifdef FEATURE_WCDMA
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_g2w_handover_discard_data();
#endif /* FEATURE_GERAN_CS_SUPPORT */
#endif // FEATURE_WCDMA
#ifdef FEATURE_GSM_DTM
  rr_dtm_discard_data();
#endif // FEATURE_GSM_DTM
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  rr_x2g_resel_deinit();
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_ded_config_discard_data();
#endif /* FEATURE_GERAN_CS_SUPPORT */

  /*Discards heap for current subscriber */
  rr_data_discard_data();
  rr_data_free_gprs_system_info_database_data();

#if defined(TEST_FRAMEWORK) && defined(FEATURE_RR_SI_CACHE)
  #error code not present
#endif /* TEST_FRAMEWORK && FEATURE_RR_SI_CACHE */

  /*Discards heap for other subscriber who is in RR_INACTIVE state*/
  {
    {
      if (rr_is_control_state(RR_INACTIVE)) 
      {
        rr_data_free_gprs_system_info_database_data(); 
        rr_data_discard_data();
      }
    }
   
  }

} /* end discard_all_rr_temporary_data() */

/*!
 * \brief Initialise the Recovery Restart mechanism.
 *
 * \param 
 */
static void rr_init_recovery_restart( void )
{
#if defined(PERLUTF) || defined(TEST_FRAMEWORK)
  #error code not present
#else
  if(geran_efs_read_primary("/nv/item_files/gsm/gl1/gsm_commercial_recovery_restart",
                            &rr_nv_recovery_restart.recovery_restart_enabled, sizeof(rr_nv_recovery_restart.recovery_restart_enabled))
                           == sizeof(rr_nv_recovery_restart.recovery_restart_enabled))
  {
    rr_nv_recovery_restart.valid = TRUE;
  }
  else
  {
    rr_nv_recovery_restart.valid = FALSE;
  }
#endif /* !PERLUTF */

  return;
}

/*!
 * \brief Initialise all RR data.
 *
 * \param 
 */
static void rr_initialise( void )
{
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info();
  gprs_scell_info_t             *pcell_info_ptr = rr_gprs_get_pcell_info();
  rr_measured_signal_data_t     *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr();

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  wcdma_ncell_list_data_t       *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr();
  surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr();
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_GSM_TDS
  rr_ded_irat_measurements_t   *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr();
#endif /*FEATURE_GSM_TDS*/

  /* Initialise recovery mechanism parameters */
  rr_init_recovery_restart();
  
  /**
   * Initialise cell information for RR_SERVICE_IND/GRR_MAC_PS_ACCESS_REQ.
   */
  rr_mm_send_reset_last_cell_info();
  rr_mac_send_init();

  /* Setup the main state machine module */
  rr_control_init();

  rr_acq_db_init();

#ifdef FEATURE_GERAN_X2G_ACQ_DB_SCAN
  rr_irat_acq_db_init();
#endif /*FEATURE_GERAN_X2G_ACQ_DB_SCAN*/

  /* Initialise PLMN select request info */
  rr_init_plmn_select_req_info();

  /* Initialise PLMN list request info */
  rr_init_plmn_list_req_info();

  /***********************************************/
  /* Initialise the system_information_database  */
  /***********************************************/
  rr_clear_system_information(&scell_info_ptr->gsm);

  /*******************************************************/
  /* Initialise the surrounding_system_information_base  */
  /*******************************************************/
  rr_reset_surrounding_database();

  /***************************************/
  /* Initialise the pending_channel_data */
  /***************************************/
  rr_clear_pending_channel_data();

  /* Initialise the connection establishment module */
  rr_initialise_connection_establishment();

  rr_initialise_msg_list();

  rr_initialise_forbidden_cell_list();

  /*
   * initialise current signal strength and signal quality
   */

  rr_measured_signal_data_ptr->current_signal_strength = 0;
  rr_measured_signal_data_ptr->current_signal_quality  = RR_INVALID_RXQUAL_VALUE;


  /* Initialise RR's multiband database band support information */
  rr_init_band_support_info();

  /* reset the current band */
  rr_reset_current_band();


#ifdef FEATURE_LTE
  /* Memset the memory to zero first to ensure that any pointers are set
  to NULL. */
  memset(rr_get_instances_ptr(SI2Q_INSTANCE), 0, sizeof(instances_T));
#endif /* FEATURE_LTE */

  rr_initialize_SI2q_instances();

  rr_initialize_MI_instances();
  rr_initialize_SI2ter_instances();

#ifdef FEATURE_GERAN_EAB
  rr_initialize_SI21_instances();
#endif /* FEATURE_GERAN_EAB */

  rr_initialize_nas_message_buffer();

  rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE);
  rr_l1_gsm_deact_reason_set(RR_L1_MODE_CHANGE);

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_g2w_init();
  rr_g2x_init();

  /*************************************************************/
  /* Initialise the wcdma surrounding cell database            */
  /* Also reset the rejected wcdma cell database &             */
  /* the threshold Store.                                      */
  /*************************************************************/
  rr_g2wr_reset_wcdma_surrounding_database();

  rr_initialize_wcdma_idle_neighbor_list (&scell_info_ptr->gsm.utran_neighbor_list);
  rr_initialize_wcdma_idle_neighbor_list (&pcell_info_ptr->gsm.utran_neighbor_list);
  rr_initialize_wcdma_ded_neighbor_list(&scell_info_ptr->gsm.wcdma_ded_neighbor_list);
  rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);
  rr_initialize_wcdma_neighbor_cell_list(wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list);
#else
  scell_info_ptr->gsm.utran_neighbor_list.valid_data = FALSE;
  pcell_info_ptr->gsm.utran_neighbor_list.valid_data = FALSE;
  scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data = FALSE;
#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/

#ifdef FEATURE_GSM_TDS
  rr_initialize_tds_ded_measurements(tds_ded_meas_ptr);
#endif /*FEATURE_GSM_TDS*/

  rr_init_hz_info_dyn();
#ifdef PERLUTF
  #error code not present
#endif /* PERLUTF */

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
  gprs_odm_set_plmn_lac_list(NULL, LAC_DIRECT);
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

  rr_clear_cell_selection_details();
  rr_cell_selection_clear_decode_bcch_retry_status();

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  rr_init_cell_change_cb_data();
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/

  rr_candidate_cell_db_init();
  rr_select_bcch_init();

#ifdef FEATURE_FEMTO_GSM_NL
  rr_femto_init();
#endif /* FEATURE_FEMTO_GSM_NL */

  {
    eng_mode_scell_info_t scell_info = {0};

    geran_eng_mode_data_write(
      ENG_MODE_SCELL_INFO,    // geran_eng_mode_info_type
      (void*)&scell_info,     // param_copy
      FALSE                  // valid_flag
    );
  }


  rr_gprs_initialise();

  rr_grr_control_init();
  rr_l1_idle_init();
  rr_l1_send_init();
  rr_ce_paging_init();
#ifdef FEATURE_IDLE_DRX_SCALING
  rr_idle_drx_init();
#endif /* FEATURE_IDLE_DRX_SCALING */
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  rr_set_rr_using_pseudo_si2q_data(FALSE);
#endif

#if defined(FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_SAVE_MODE)
  rr_init_psm_edrx_params();
#endif /*FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_SAVE_MODE*/

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
  rr_resel_rmc_init();
  rr_resel_c1_delta_init();
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/
  (void) discard_all_rr_temporary_data();
} /* rr_initialise */

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/*!
 * \brief This function is called from external tasks to determine if Recovery Restart is enabled.
 *
 * \param 
 *
 * \return boolean - TRUE if Recovery Restart is enabled; FALSE otherwise (i.e. should ERR_FATAL)
 */
boolean geran_get_nv_recovery_restart_enabled_dyn( void )
{
  boolean recovery_restart_enabled = FALSE;

  if (rr_nv_recovery_restart.valid == TRUE)
  {
    if (rr_nv_recovery_restart.recovery_restart_enabled == TRUE)
    {
      recovery_restart_enabled = TRUE;
    }
  }

  return recovery_restart_enabled;
}

/*!
 * \brief Initialise RR data (related to GPRS)
 *
 * \param init_reason
 * \param 
 */
void rr_gprs_initialise( void )
{
  rr_gprs_main_init();

  /* Init scell, pcell and ncell sys info data stores */
  rr_init_gprs_sys_info_database();

  /* Initialise PS domain & access modules */
  rr_ps_domain_init();
  rr_ps_access_init();

  /* Initialise cell reselection */
  rr_resel_init();

  /* Reset NACC event tracking */
  rr_gprs_nacc_notify_event(CCN_RESET);

#ifdef FEATURE_GSM_DTM
  rr_dtm_init();
#endif /* FEATURE_GSM_DTM */

  return;

} /* end rr_gprs_initialise() */

/*!
 * \brief Initialisation of RR operational data when GERAN started
 *
 * \param 
 */
void rr_init_geran_started(void )
{
  // TODO move initialisaion from rr_initialise to here
  ;
}

/*!
 * \brief Initialisation performed when GERAN stopped
 *
 * \param 
 */
void rr_init_geran_stopped( void )
{
  // Ensure all allocated memory is freed
  discard_all_rr_temporary_data();

  rr_gprs_initialise();

} /* end rr_init_geran_stopped() */

/*!
 * \brief External interface for RR to initialise itself
 *
 * \param 
 */
void rr_init_modem_online( void )
{
  rr_initialise();
}

/*!
 * \brief Initialisation perform when modem taken offline
 *
 * \param 
 *
 * Do not call anything in here that is not GAS ID specific
 */
void rr_init_modem_offline( void )
{
  rr_reset_software();
  rr_gprs_reset_software();

  // Ensure all allocated memory is freed
  discard_all_rr_temporary_data();

  return;
} /* end rr_init_modem_offline() */

/*!
 * \brief RR task start initisation.
 */
void rr_init_task_start_init(void)
{
  rr_tbf_est_task_start_init();
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_ded_meas_reporting_task_start_init();
#endif /* FEATURE_GERAN_CS_SUPPORT */
  rr_grr_control_task_start_init();
  rr_gprs_meas_rpt_control_task_start_init();
  rr_resel_task_start_init();
#ifdef FEATURE_WCDMA
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_g2w_handover_task_start_init();
#endif /* FEATURE_GERAN_CS_SUPPORT */
#endif // FEATURE_WCDMA
#ifdef FEATURE_GSM_DTM
  rr_dtm_task_start_init();
#endif // FEATURE_GSM_DTM
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_ded_config_task_start_init();
#endif /* FEATURE_GERAN_CS_SUPPORT */
  rr_plc_task_start_init();
  rr_sys_info_pacch_task_start_init();
  rr_process_pmo_pcco_task_start_init();
  rr_l1_idle_task_start_init();
  rr_ps_domain_task_start_init();
  rr_mac_send_task_start_init();
  rr_ps_access_task_start_init();
  rr_l1_send_task_start_init();
  rr_gprs_main_task_start_init();
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_cell_reest_task_start_init();
#endif /* FEATURE_GERAN_CS_SUPPORT */
#ifdef FEATURE_RR_THERMAL_MITIGATION
  rr_thermal_task_start_init();
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
  rr_msc_task_start_init();
#endif
  rr_mm_send_task_start_init();
  rr_pscan_task_start_init();
  rr_ce_paging_task_start_init();
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_ce_rel_task_start_init();
#endif /* FEATURE_GERAN_CS_SUPPORT */
  rr_shared_sel_task_start_init();
  rr_general_task_start_init();
  rr_test_task_start_init();
#ifdef FEATURE_GERAN_CS_SUPPORT
  rr_channel_mode_task_start_init();
#endif /* FEATURE_GERAN_CS_SUPPORT */
  rr_log_task_start_init();
#ifdef FEATURE_GPRS_PS_HANDOVER
  rr_psho_task_start_init();
#endif // FEATURE_GPRS_PS_HANDOVER
#ifdef FEATURE_CGPS_UMTS_CP_GSM
  rr_cgps_task_start_init();
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

  rr_init_modem_online();
#ifdef FEATURE_DUAL_SIM
  rr_init_modem_online();
#ifdef FEATURE_TRIPLE_SIM
  rr_init_modem_online();
#endif /*  FEATURE_TRIPLE_SIM */
#endif /*  FEATURE_DUAL_SIM */

  return;
} /* end rr_init_task_start_init() */

/*!
 * \brief Called when a REcovery Restart event occurs.
 *
 * \param 
 */
void rr_init_recovery_init( void )
{
  // Ensure all allocated memory is freed
  discard_all_rr_temporary_data();

  // Now re-initialise modules [ready for cell selection]
  // TODO: Need to work out what needs to be reset after Recovery Restart
  // This sequence was copied from rr_control and left as-is
  (void) grr_control( EV_RESET_SOFTWARE, NULL );

  rr_grr_control_init();

  (void) rr_resel_control(EV_RESET_SOFTWARE, NULL);

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
  (void) rr_reduced_mobility_control(EV_RESET_SOFTWARE, NULL);
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

  rr_cell_acq_control(EV_RESET_SOFTWARE, NULL);
  (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL);
  (void) rr_gprs_bsi_acquisition_control( EV_RESET_SOFTWARE, NULL);
  (void) rr_plmn_list_control( EV_RESET_SOFTWARE, NULL);

#ifdef FEATURE_GSM_DTM
  (void) rr_dtm_control( EV_RESET_SOFTWARE, NULL );
#ifdef FEATURE_GSM_EDTM
  (void) rr_gprs_edtm_control( EV_RESET_SOFTWARE, NULL );
#endif // FEATURE_GSM_EDTM
#endif // FEATURE_GSM_DTM
  rr_gprs_main_init();

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_resel_g2w_init();
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  rr_l1_idle_init();

} /* end rr_init_recovery_init() */

/* EOF */

