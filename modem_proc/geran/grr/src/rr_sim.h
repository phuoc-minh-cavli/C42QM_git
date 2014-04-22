#ifndef RR_SIM_H
#define RR_SIM_H

/*! \file rr_sim.h
 
  This is the header file for rr_sim.c.
  This module contains SIM data communicated from NAS.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_sim.h#3 $ */
/* $DateTime: 2019/02/27 11:00:33 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "mm_rr.h"
#include "sys_cnst.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
KEYSIZE_64,
KEYSIZE_128
} keysize_e;
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*!
 * \brief Updates the SIM info (IMSI + TMSI + PTMSI) in the Public Store for MAC
 * 
 */
extern void rr_sim_update_public_store(
   
);

/*!
 * \brief Called when RR_INVALIDATE_SIM_DATA_REQ received. Clears the stored SIM data.
 * 
 * \param msg_ptr (in) - pointer to the RR_INVALIDATE_SIM_DATA_REQ message
 */
extern void rr_sim_process_invalidate_sim_data_req(
  rr_invalidate_sim_data_req_T *msg_ptr
   
);

/**
  @brief Gets the EF-KC (Cipher Key) from the SIM
 
  @param cipher_key Updated with the value of EF-KC, if available.
 
  @return TRUE if EF-KC is available; FALSE otherwise
*/

extern boolean rr_siminfo_get_cipher_key(
  byte *cipher_key,
  byte destsize_64,
  byte *cipher_key_128,
  byte destsize_128
   
);

/**
  @brief Stores the EF-KC (cipher key).

  @param cipher_key Pointer to the cipher key to store.
*/
extern void rr_siminfo_store_cipher_keys(
  byte *cipher_key,
  byte *cipher_key_128,
  const boolean rr_active
);

/**
  @brief Stores the EF-KCSN (Cipher Key Sequence Number)
 
  @param cipher_key_seq Updated with the value of EF-KCSN, if
                        available.
*/
extern void rr_siminfo_store_cipher_key_seq(
  byte cipher_key_seq,
  const boolean rr_active
);

/*!
 * \brief Called when RR_SIM_INSERTED_REQ received. Stores the SIM data.
 * 
 * \param msg_ptr (in) - pointer to the RR_SIM_INSERTED_REQ message
 */
extern void rr_sim_process_sim_inserted_req(
  rr_sim_inserted_req_T *msg_ptr
   
);

/*!
 * \brief Called when RR_SIM_UPDATE_REQ received. Updates the stored the SIM data.
 * 
 * \param rr_sim_update_req_ptr (in) - pointer to the RR_SIM_UPDATE_REQ message
 */
extern void rr_sim_process_sim_update_req(
  rr_sim_update_req_T *rr_sim_update_req_ptr
   
);


/*!
 * \brief Gets the EF-KCSN (Cipher Key Sequence Number) from the SIM.
 * 
 * \param cipher_key_seq (out) - Updated with the value of EF-KCSN (if available).
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_cipher_key_seq(
  byte *cipher_key_seq
   
);

/*!
 * \brief Activates buffered cipher key and cipher key sequence number.
 *
 * This function should called on receipt of RRC_CIPHER_SYNC_IND or
 * TDSRR_CIPHER_SYNC_IND.  It copies the buffered cipher key and cipher
 * key sequence number to the main store.
 */
extern void rr_siminfo_activate_new_cipher_keys(
   
);

/*!
 * \brief Gets the EF-ACC (Access Class) from the SIM.
 * 
 * \param access_class (out) - Updated with the Access Class (if available).
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_access_class(
  byte *access_class,
  byte destsize
   
);

/*!
 * \brief Gets the IMSI from the SIM.
 * 
 * \param imsi (out) - Updated with the IMSI (if available)
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_imsi(
  byte *imsi,
  byte destsize
   
);

/*!
 * \brief Gets the TMSI from the SIM.
 * 
 * \param tmsi (out) - Updated with the TMSI (if available).
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_tmsi(
  byte *tmsi,
  byte destsize
   
);

/*!
 * \brief Gets the P-TMSI from the SIM.
 * 
 * \param ptmsi (out) - Updated with the P-TMSI (if available).
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_ptmsi(
  byte *ptmsi,
  byte destsize
   
);

/*!
 * \brief Gets the MS Operational Mode from the SIM.
 * 
 * 
 * \return ms_operation_mode_T - the value of MS Operational Mode
 */
extern ms_operation_mode_T rr_siminfo_get_ms_opmode(
   
);

/*!
 * \brief Returns TRUE if the inserted SIM is a Type Approval SIM
 * 
 * 
 * \return boolean - TRUE if a Type Approval SIM, FALSE otherise
 */
extern boolean rr_sim_is_type_approval(
   
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Retrieves the MS Operational Mode for all subscriptions.
 *
 * \param op_mode_list (out) - array of operation modes.
 * \param num_entries  (in)  - number of entries in the op_mode_list
 */
void rr_ms_get_sim_operation_modes(byte *op_mode_list, int num_entries);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Gets the IMSImod1000 value from the SIM.
 * 
 * \param imsi_mod_1000 (out) - Updated with the value of IMSImod1000
 * 
 * \return boolean - TRUE if SIM information was available, FALSE otherwise
 */
extern boolean rr_get_imsi_mod_1000(
  word *imsi_mod_1000
   
);

#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) 

/*!
 * \brief Gets the IMSImod10000000 value from the SIM.
 * 
 * \param imsi_mod_10000000 (out) - Updated with the value of IMSImod10000000
 * 
 * \return boolean - TRUE if SIM information was available, FALSE otherwise
 */

boolean rr_get_imsi_mod_10000000(
  uint32 *imsi_mod_10000000
   
);

#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

#endif /* #ifndef RR_SIM_H */

/* EOF */

