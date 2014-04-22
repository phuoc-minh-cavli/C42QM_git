#ifndef RR_MSC_H
#define RR_MSC_H

/*!
 * \file rr_msc.h 
 *  
 * This module contains functionality to manage the advertised multi-slot class. 
 *  
 *              Copyright (c) 2014 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_msc.h#3 $ */
/* $DateTime: 2018/10/16 07:03:13 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "comdef.h"
#include "geran_multi_sim.h"
#include "rr_l1.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
typedef enum
{
#ifdef FEATURE_RR_THERMAL_MITIGATION
  RR_MSC_CLIENT_THERMAL,      // Thermal Mitigation
#endif
#ifdef FEATURE_DUAL_SIM
  RR_MSC_CLIENT_L1_COEX,      // L1 coexistence management
#endif
  RR_MSC_CLIENT_MAX_COUNT     // Number of instances of modified MSC data
} rr_msc_client_e_t;
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#ifdef FEATURE_GSM_DTM
/*!
 * \brief Converts DTM multislot subclass value to a DTM multislot class value.
 * 
 * \param dtm_multislot_subclass (in)
 * 
 * \return uint8 - DTM multislot class value
 */
extern uint8 rr_msc_conv_dtm_multislot_subclass_to_class(uint8 dtm_multislot_subclass);

/*!
 * \brief Converts DTM multislot class value to a DTM multislot subclass value.
 * 
 * \param dtm_multislot_class (in)
 * 
 * \return uint8 - DTM multislot subclass value
 */
extern uint8 rr_msc_conv_dtm_multislot_class_to_subclass(uint8 dtm_multislot_class);
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_DUAL_SIM
/*!
 * \brief Used to set a specific number of TX slots.
 * 
 * \param msc_client (in)
 * \param tx_slots (in)
 * \param rx_slots (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
extern boolean rr_msc_set_tx_rx_slots(
  rr_msc_client_e_t msc_client,
  uint8 tx_slots,
  uint8 rx_slots
);
#endif /* FEATURE_DUAL_SIM */

#ifdef FEATURE_RR_THERMAL_MITIGATION
/*!
 * \brief Used to reduce the number of TX slots.
 * 
 * \param msc_client (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
extern boolean rr_msc_reduce_tx_slots(rr_msc_client_e_t msc_client);
#endif /* FEATURE_RR_THERMAL_MITIGATION */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
/*!
 * \brief Used to revert the MSC data to the original NV values.
 * 
 * \param msc_client (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
extern boolean rr_msc_revert_to_orig(rr_msc_client_e_t msc_client);

/*!
 * \brief Called when the NV data becomes available.
 * 
 */
extern void rr_msc_nv_ready(void);

/*!
 * \brief Returns the MSC values.
 * 
 * \param egprs_multislot_class_ptr (out)
 * \param dtm_multislot_class_ptr (out)
 * \param gprs_multislot_class_ptr (out)
 */
extern void rr_msc_get_multislot_class(
  uint8 *gprs_multislot_class,
  uint8 *egprs_multislot_class
#ifdef FEATURE_GSM_DTM
  ,uint8 *dtm_multislot_subclass
#endif
);

#ifdef FEATURE_DUAL_SIM
/*!
 * \brief Process function for MPH_COEX_MSC_CHANGE_IND
 * 
 * \param msg_ptr (in)
 */
extern void rr_msc_process_l1_coex_msc_change_ind(mph_coex_msc_change_ind_T *msg_ptr  );
#endif

/*!
 * \brief Updates MM with new multi-slot class parameters (if required). 
 *        Note: Also calls into the RR-THERMAL module which will update the service capability (if required). 
 * 
 * \param can_send_service_ind 
 */
extern void rr_msc_update(boolean can_send_service_ind );

/*!
 * \brief Sends RR_IMSG_THERMAL_MITIGATION_REQ.
 * 
 */
extern void rr_msc_send_rr_imsg_msc_change_ind(void);

/*!
 * \brief Initialisation function called once at RR task start.
 */
extern void rr_msc_task_start_init(void);
#endif /* FEATURE_DUAL_SIM || FEATURE_RR_THERMAL_MITIGATION */

#endif /* #ifndef RR_MSC_H */

