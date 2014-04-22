/*! \file rr_sim.c

  This module contains the SIM data communicated from NAS

                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_sim.c#5 $ */
/* $DateTime: 2019/02/27 11:00:33 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_sim.h"
#include "rr_gprs_defs.h"
#include "mm_as_types.h"
#include "rr_ps_domain.h"
#include "geran_multi_sim.h"
#include "rr_test.h"
#include "rr_mac_send.h"
#include "rr_control.h"
#include <stringl/stringl.h>
#include "rr_grr.h"  
#include "rr_l1_send.h"
#include "rr_static_general.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Calculates IMSImod1000.
 *
 *  IMSImod1000 is used by GL1, and included in messages such as MPH_START_IDLE_MODE_REQ,
 *  MPH_UPDATE_IDLE_MODE_REQ, etc.
 *  To save re-calculating this each time, it is done once and stored when the IMSI is stored.
 *
 */
static void rr_calc_imsi_mod_1000( void )
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.imsi_valid)
  {
    uint8  index;
    byte  *imsi_ptr = (byte *)rr_sim_data_ptr->sim_data.imsi;

    /* Consider the short IMSI and the long IMSI */
    if ((imsi_ptr[1] & 0x08) != 0)
    {
      /* The length of the IMSI is odd */
      index = imsi_ptr[0];

      rr_sim_data_ptr->imsi_mod_1000 =
      (uint16)((byte)(imsi_ptr[index - 1] & 0xF0) >> 4) * (uint16)100 +
      (uint16)(imsi_ptr[index] & 0x0F) * (uint16)10 +
      (uint16)((byte) (imsi_ptr[index] & 0xF0) >> 4);
    }
    else
    {
      /* The length of the IMSI is even */
      index = imsi_ptr[0];

      rr_sim_data_ptr->imsi_mod_1000 =
      (uint16)((byte) (imsi_ptr[index - 1] & 0x0F) * (uint16)100) +
      (uint16)((byte) (imsi_ptr[index - 1] & 0xF0) >> 4) * (uint16)10 +
      (uint16)(imsi_ptr[index] & 0x0F);
    }
  }

} /* end rr_calc_imsi_mod_1000 */

#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) 

/*!
 * \brief Calculates IMSImod10000000.
 *
 *  IMSImod10000000 is used by GL1, and included in messages such as MPH_START_IDLE_MODE_REQ,
 *  MPH_UPDATE_IDLE_MODE_REQ, etc.
 *  To save re-calculating this each time, it is done once and stored when the IMSI is stored.
 *
 */
static void rr_calc_imsi_mod_10000000( void )
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.imsi_valid)
  {
    uint8  index;
    byte  *imsi_ptr = (byte *)rr_sim_data_ptr->sim_data.imsi;

    /* Consider the short IMSI and the long IMSI */
    if ((imsi_ptr[1] & 0x08) != 0)
    {
      /* The length of the IMSI is odd */
      index = imsi_ptr[0];

      rr_sim_data_ptr->imsi_mod_10000000=
      (uint16)((byte)(imsi_ptr[index - 3] & 0xF0) >> 4) * (uint32)1000000 +                       
      (uint16)((byte)(imsi_ptr[index - 2] & 0x0F))      * (uint32)100000 +                       
      (uint16)((byte)(imsi_ptr[index - 2] & 0xF0) >> 4) * (uint16)10000 +                        
      (uint16)((byte)(imsi_ptr[index - 1] & 0x0F))      * (uint16)1000 +                       
      (uint16)((byte)(imsi_ptr[index - 1] & 0xF0) >> 4) * (uint16)100 +                       
      (uint16)(imsi_ptr[index] & 0x0F)                  * (uint16)10 +                       
      (uint16)((byte) (imsi_ptr[index] & 0xF0) >> 4);
      MSG_GERAN_HIGH_1("imsi_mod_crore ODD  = %d", rr_sim_data_ptr->imsi_mod_10000000);
    }
    else
    {
      /* The length of the IMSI is even */
      index = imsi_ptr[0];

      rr_sim_data_ptr->imsi_mod_10000000=
      (uint16)((byte) (imsi_ptr[index - 3] & 0x0F)) * (uint32)1000000 +
      (uint16)((byte)(imsi_ptr[index - 3] & 0xF0) >> 4) * (uint32)100000 + 
      (uint16)((byte) (imsi_ptr[index - 2] & 0x0F))     * (uint16)10000 + 
      (uint16)((byte)(imsi_ptr[index - 2] & 0xF0) >> 4) * (uint16)1000 +
      (uint16)((byte) (imsi_ptr[index - 1] & 0x0F)      * (uint16)100) +
      (uint16)((byte) (imsi_ptr[index - 1] & 0xF0) >> 4)* (uint16)10 +
      (uint16)(imsi_ptr[index] & 0x0F);
       MSG_GERAN_HIGH_1("imsi_mod_crore Even = %d", rr_sim_data_ptr->imsi_mod_10000000);

    }
  }

} /* end rr_calc_imsi_mod_1000 */

#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

/*!
 * \brief Stores EF-KC from the SIM..
 *
 * \param cipher_key (in)
 * \param rr_active (in)
 */
void rr_siminfo_store_cipher_keys(
  byte *cipher_key,
  byte *cipher_key_128,
  const boolean rr_active
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_active)
  {
    MSG_GERAN_HIGH_0("RR is active, storing new cipher_key");

    memscpy(
      rr_sim_data_ptr->sim_data.cipher_key,
      sizeof(rr_sim_data_ptr->sim_data.cipher_key),
      cipher_key,
      MM_SIM_EFKC_SIZE
    );

    rr_sim_data_ptr->sim_data.cipher_key_valid = TRUE;

    memscpy(
      rr_sim_data_ptr->sim_data.gsm_kc_128bit,
      sizeof(rr_sim_data_ptr->sim_data.gsm_kc_128bit),
      cipher_key_128,
      MM_GSM_128BIT_KC_SIZE
    );

    rr_sim_data_ptr->sim_data.cipher_key_128_valid = TRUE;

    /* Discard any old buffered Cipher Key that was never activated. */
    if (rr_sim_data_ptr->sim_data.new_cipher_key_valid)
    {
      MSG_GERAN_HIGH_0("Clearing old buffered cipher_key");
      rr_sim_data_ptr->sim_data.new_cipher_key_valid = FALSE;
    }

    /* Discard any old buffered Cipher Key that was never activated. */
    if (rr_sim_data_ptr->sim_data.new_cipher_key_128_valid)
    {
      MSG_GERAN_HIGH_0("Clearing old buffered 128 bit cipher key");
      rr_sim_data_ptr->sim_data.new_cipher_key_128_valid = FALSE;
    }
  }
  else
  {
    MSG_GERAN_HIGH_0("RR is inactive, buffering new cipher key");

    memscpy(
      rr_sim_data_ptr->sim_data.new_cipher_key,
      sizeof(rr_sim_data_ptr->sim_data.new_cipher_key),
      cipher_key,
      MM_SIM_EFKC_SIZE
    );

    rr_sim_data_ptr->sim_data.new_cipher_key_valid = TRUE;

    memscpy(
      rr_sim_data_ptr->sim_data.new_gsm_kc_128bit,
      sizeof(rr_sim_data_ptr->sim_data.new_gsm_kc_128bit),
      cipher_key_128,
      MM_GSM_128BIT_KC_SIZE
    );

    rr_sim_data_ptr->sim_data.new_cipher_key_128_valid = TRUE;
  }
}

/*!
 * \brief Stores EF-KCSN (Cipher Key Sequence Number) from the SIM.
 *
 * \param cipher_key_seq (in)
 * \param rr_active (in)
 */
void rr_siminfo_store_cipher_key_seq(
  byte cipher_key_seq,
  const boolean rr_active
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_active)
  {
    MSG_GERAN_HIGH_0("RR is active, storing new cipher_key_seq_no");

    rr_sim_data_ptr->sim_data.cipher_key_seq_no = cipher_key_seq;
    rr_sim_data_ptr->sim_data.cipher_key_seq_no_valid = TRUE;

    /* Discard any old buffered Cipher Key Sequence Number that was never activated. */
    if (rr_sim_data_ptr->sim_data.new_cipher_key_seq_no_valid)
    {
      MSG_GERAN_HIGH_0("Clearing old buffered cipher_key_seq_no");
      rr_sim_data_ptr->sim_data.new_cipher_key_seq_no_valid = FALSE;
    }
  }
  else
  {
    MSG_GERAN_HIGH_0("RR is inactive, buffering new cipher_key_seq_no");
    rr_sim_data_ptr->sim_data.new_cipher_key_seq_no = cipher_key_seq;
    rr_sim_data_ptr->sim_data.new_cipher_key_seq_no_valid = TRUE;
  }
}

void rr_siminfo_activate_new_cipher_keys( void )
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.new_cipher_key_valid)
  {
    memscpy(
      rr_sim_data_ptr->sim_data.cipher_key,
      sizeof(rr_sim_data_ptr->sim_data.cipher_key),
      rr_sim_data_ptr->sim_data.new_cipher_key,
      MM_SIM_EFKC_SIZE
    );
    memscpy(
      rr_sim_data_ptr->sim_data.gsm_kc_128bit,
      sizeof(rr_sim_data_ptr->sim_data.gsm_kc_128bit),
      rr_sim_data_ptr->sim_data.new_gsm_kc_128bit,
      sizeof(rr_sim_data_ptr->sim_data.new_gsm_kc_128bit)
    );
    rr_sim_data_ptr->sim_data.cipher_key_valid = TRUE;
    rr_sim_data_ptr->sim_data.cipher_key_128_valid = TRUE;

    /* Next cipher key is no longer required. */
    rr_sim_data_ptr->sim_data.new_cipher_key_valid = FALSE;
    rr_sim_data_ptr->sim_data.new_cipher_key_128_valid = FALSE;
  }
  if (rr_sim_data_ptr->sim_data.new_cipher_key_seq_no_valid)
  {
    rr_sim_data_ptr->sim_data.cipher_key_seq_no = rr_sim_data_ptr->sim_data.new_cipher_key_seq_no;
    rr_sim_data_ptr->sim_data.cipher_key_seq_no_valid = TRUE;

    /* Next cipher key sequence number is no longer required. */
    rr_sim_data_ptr->sim_data.new_cipher_key_seq_no_valid = FALSE;
  }
}

/*!
 * \brief Stores EF-ACC (Access Class) from the SIM.
 *
 * \param access_class (in)
 */
static void rr_siminfo_store_access_class(
  byte *access_class
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(access_class);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);
  memscpy(rr_sim_data_ptr->sim_data.access_class,
  	      sizeof(rr_sim_data_ptr->sim_data.access_class),
  	      access_class,
  	      MM_SIM_EFACC_SIZE
  	      );
  rr_sim_data_ptr->sim_data.access_class_valid = TRUE;
}

/*!
 * \brief Stores the IMSI from the SIM.
 *
 * \param imsi (in)
 */
static void rr_siminfo_store_imsi(
  byte *imsi
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(imsi);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);
  memscpy(rr_sim_data_ptr->sim_data.imsi,sizeof(rr_sim_data_ptr->sim_data.imsi),imsi, MM_SIM_IMSI_SIZE);
  rr_sim_data_ptr->sim_data.imsi_valid = TRUE;
}

/*!
 * \brief Stores the TMSI from the SIM.
 *
 * \param tmsi (in)
 */
static void rr_siminfo_store_tmsi(
  byte *tmsi
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(tmsi);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);
  memscpy(rr_sim_data_ptr->sim_data.tmsi,sizeof(rr_sim_data_ptr->sim_data.tmsi),tmsi,MM_SIM_TMSI_SIZE);
  rr_sim_data_ptr->sim_data.tmsi_valid = TRUE;
}

/*!
 * \brief Stores the PTMSI from the SIM.
 *
 * \param ptmsi (in)
 */
static void rr_siminfo_store_ptmsi(
  byte *ptmsi
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(ptmsi);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);
  memscpy(rr_sim_data_ptr->sim_data.ptmsi,sizeof(rr_sim_data_ptr->sim_data.ptmsi),ptmsi,MM_SIM_PTMSI_SIZE);   
  rr_sim_data_ptr->sim_data.ptmsi_valid = TRUE;
}

/*!
 * \brief Stores EF-AD (Admin Data) from the SIM.
 *
 * \param admin_data (in)
 */
static void rr_siminfo_store_admin_data(
  mm_sim_efad_T admin_data
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  rr_sim_data_ptr->sim_data.ms_opmode = admin_data;
  rr_sim_data_ptr->sim_data.ms_opmode_valid = TRUE;
}

/*!
 * \brief Validates the Access Class from the SIM.
 *
 * \param access_class (in)
 *
 * \return boolean
 */
static boolean rr_access_class_is_valid(
  const byte *access_class
)
{
  boolean bValid;

#ifdef FEATURE_ALLOW_ACCESS_WITHOUT_LOW_CLASSES
   bValid = TRUE;
#else
  /* 3GPP 22.011 specification states that:
   * "All UEs are members of one out of ten randomly allocated mobile populations,
   * defined as Access Classes 0 to 9".
   * Therefore this function considers an access class bitmap to be valid if
   * and only if AT LEAST 1 of the Normal access class bits is set.
   */
  if (access_class != NULL)
  {
    /* First, check whether any bits are set in normal classes 0..7 (2nd octet) */
    bValid = (boolean)(access_class[1] != 0x00);

    if (bValid == FALSE)
    {
      /* Not a member of classes 0..7, so try the lower 2 bits of the 1st octet */
      if ((access_class[0] & 0x03) != 0x00) /* i.e. class 8 or class 9 bits */
      {
        bValid = TRUE;
      }
      else
      {
        MSG_GERAN_ERROR_2("Invalid SIM [%02X%02X]: UE must belong to a normal access class",
                  (int)access_class[0], (int)access_class[1]);
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("Unable to validate access class, NULL pointer");
    bValid = FALSE;
  }
#endif /* FEATURE_ALLOW_ACCESS_WITHOUT_LOW_CLASSES */

  return(bValid);
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/*!
 * \brief Updates the SIM info (IMSI + TMSI + PTMSI) in the Public Store for MAC
 *
 */
void rr_sim_update_public_store( void )
{
  rrps_store_update_data_t *store_update_ptr = rr_get_rrps_store_update();

  if (store_update_ptr != NULL)
  {
    // Need to update IMSI + TMSI + PTMSI

    rr_siminfo_get_imsi(
      store_update_ptr->imsi,
      sizeof(store_update_ptr->imsi)
    );

    rr_siminfo_get_tmsi(
      store_update_ptr->tmsi,
      sizeof(store_update_ptr->tmsi)
    );

    rr_siminfo_get_ptmsi(
      store_update_ptr->ptmsi,
      sizeof(store_update_ptr->ptmsi)
    );

    rr_write_public_store(
      RRPS_UPD_SIM_IMSI | RRPS_UPD_SIM_TMSI | RRPS_UPD_SIM_PTMSI,   // param_id
      store_update_ptr                                             // param_copy
    );

    rr_free_rrps_store_update(
      store_update_ptr   // ptr
    );
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrps_store_update_ptr is NULL");
  }
}

/*!
 * \brief Called when RR_INVALIDATE_SIM_DATA_REQ received. Clears the stored SIM data.
 *
 * \param msg_ptr (in) - pointer to the RR_INVALIDATE_SIM_DATA_REQ message
 */
void rr_sim_process_invalidate_sim_data_req(
  rr_invalidate_sim_data_req_T *msg_ptr
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Sanity-check the input
  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  // Clear the local SIM data
  memset(&rr_sim_data_ptr->sim_data, 0, sizeof(rr_sim_data_from_nas_t));

  // Update the Public Store SIM data
  rr_sim_update_public_store();

  // Test module needs to know that SIM is removed
  rr_handle_test_messages(
    msg_ptr    // new_message
  );

} /* rr_sim_process_invalidate_sim_data_req */

/*!
 * \brief Called when RR_SIM_INSERTED_REQ received. Stores the SIM data.
 *
 * \param msg_ptr (in) - pointer to the RR_SIM_INSERTED_REQ message
 */
void rr_sim_process_sim_inserted_req(
  rr_sim_inserted_req_T *msg_ptr
   
)
{
  // Sanity-check the input
  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Update the local SIM data
  rr_siminfo_store_cipher_keys(
    msg_ptr->ciphering_key,   // cipher_key
    msg_ptr->gsm_kc_128bit,   // cipher_key_128
    TRUE                     // rr_active
  );

  rr_siminfo_store_cipher_key_seq(msg_ptr->ciphering_key_seq_no, TRUE);
  rr_siminfo_store_access_class(msg_ptr->access_control);
  rr_siminfo_store_imsi(msg_ptr->imsi);
  rr_siminfo_store_ptmsi(msg_ptr->ptmsi);
  rr_siminfo_store_tmsi(msg_ptr->tmsi);
  rr_siminfo_store_admin_data(msg_ptr->admin_data);

  // Calculate IMSImod1000 required by GL1 later
  rr_calc_imsi_mod_1000();
#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) 
  // Calculate IMSImod10000000 required by GL1 later
  rr_calc_imsi_mod_10000000();
#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

  // Update the Public Store SIM data
  // Note: The SIM inserted indication is also used as a trigger to update the Public Store NV data
  rr_public_store_update_nv_data();
  rr_sim_update_public_store();

  // Test module needs to know that SIM is inserted
  rr_handle_test_messages(
    msg_ptr    // new_message
  );
}


/*!
 * \brief Called when RR_SIM_UPDATE_REQ received. Updates the stored the SIM data.
 *
 * \param msg_ptr (in) - pointer to the RR_SIM_UPDATE_REQ message
 */
void rr_sim_process_sim_update_req(
  rr_sim_update_req_T *msg_ptr
   
)
{

  // Update the local SIM data
  if (rr_is_inactive())
  {
    rr_siminfo_store_cipher_keys(
      msg_ptr->ciphering_key,   // cipher_key
      msg_ptr->gsm_kc_128bit,   // cipher_key_128
      FALSE                    // rr_active
    );

    rr_siminfo_store_cipher_key_seq(
      msg_ptr->ciphering_key_seq_no,   // ciphering_key_seq
      FALSE                           // rr_active
    );
  }
  else
  {
    rr_siminfo_store_cipher_keys(
      msg_ptr->ciphering_key,   // cipher_key
      msg_ptr->gsm_kc_128bit,   // cipher_key_128
      TRUE                     // rr_active
    );

    rr_siminfo_store_cipher_key_seq(
      msg_ptr->ciphering_key_seq_no,   // ciphering_key_no
      TRUE                            // rr_active
    );
  }

  rr_siminfo_store_ptmsi(msg_ptr->ptmsi);
  rr_siminfo_store_tmsi(msg_ptr->tmsi);

  // Update the Public Store SIM data
  rr_sim_update_public_store();

#ifdef FEATURE_GERAN_CS_SUPPORT
  /*
  registered_rai.location_area_code:
    The LAC is 16 bits, split over 2 octets
    Bit 8 of octet 0 is the MSB and bit 1 of octet 1 the LSB
    (see 24.008 Table 10.5.148)
  */
  grr_set_ota_gprs_suspension_parameters(
    msg_ptr->registered_rai    // registered_rai
  );
#endif /* FEATURE_GERAN_CS_SUPPORT */

#ifndef PERLUTF
  if (grr_is_idle_camped()) 
  { 
    rr_send_mph_xmsi_update_ind(); 
  } 
#endif /**/
} /* rr_sim_process_sim_update_req */

/*!
 * \brief Gets the EF-KC (Cipher Key) from the SIM.
 *
 * \param cipher_key (out) - Updated with the value of EF-KC (if available).
 *
 * \return boolean - TRUE if available; FALSE otherwise.
 */
boolean rr_siminfo_get_cipher_key(
  byte *cipher_key,
  byte destsize_64,
  byte *cipher_key_128,
  byte destsize_128
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);
 
  if ((rr_sim_data_ptr->sim_data.cipher_key_valid) && (rr_sim_data_ptr->sim_data.cipher_key_128_valid))
  {
    if (cipher_key != NULL)
    {
      memscpy(cipher_key,destsize_64,rr_sim_data_ptr->sim_data.cipher_key,MM_SIM_EFKC_SIZE);
    }

    if (cipher_key_128 != NULL)
    {
      memscpy(cipher_key_128,destsize_128,rr_sim_data_ptr->sim_data.gsm_kc_128bit,MM_GSM_128BIT_KC_SIZE);
    }
    return TRUE;
  }

  return FALSE;
}

/*!
 * \brief Gets the EF-KCSN (Cipher Key Sequence Number) from the SIM.
 *
 * \param cipher_key_seq (out) - Updated with the value of EF-KCSN (if available).
 *
 * \return boolean - TRUE if available; FALSE otherwise.
 */
boolean rr_siminfo_get_cipher_key_seq(
  byte *cipher_key_seq
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.cipher_key_seq_no_valid)
  {
    if (cipher_key_seq != NULL)
    {
      *cipher_key_seq = rr_sim_data_ptr->sim_data.cipher_key_seq_no;
    }
    return TRUE;
  }

  return FALSE;
}

/*!
 * \brief Gets the EF-ACC (Access Class) from the SIM.
 *
 * \param access_class (out) - Updated with the Access Class (if available).
 *
 * \return boolean - TRUE if available; FALSE otherwise.
 */
boolean rr_siminfo_get_access_class(
  byte *access_class,
  byte destsize
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_PARAM(access_class, FALSE);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.access_class_valid)
  {
   memscpy(access_class,destsize,rr_sim_data_ptr->sim_data.access_class,MM_SIM_EFACC_SIZE);

    return rr_access_class_is_valid((const byte *)access_class);
  }

  return FALSE;
}

/*!
 * \brief Gets the IMSI from the SIM.
 *
 * \param imsi (out) - Updated with the IMSI (if available)
 *
 * \return boolean - TRUE if available; FALSE otherwise.
 */
boolean rr_siminfo_get_imsi(
  byte *imsi,
  byte destsize
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_PARAM(imsi, FALSE);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.imsi_valid)
  {
    memscpy(imsi,destsize,rr_sim_data_ptr->sim_data.imsi,MM_SIM_IMSI_SIZE);
    
    /* validate the length octet: the IMSI buffer contains (1+n) octets, where n */
    /* is no more than 8 (3GPP 24.008 states the IMSI has maximum of 15 digits,  */
    /* and each digit is represented by one half-octet using a BCD-type format)  */
    if (imsi[0] >= MM_SIM_IMSI_SIZE) /* this is defined as 9 */
    {
      MSG_GERAN_ERROR_1_G("IMSI length octet is out of range (%d)", (int)imsi[0]);
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*!
 * \brief Gets the TMSI from the SIM.
 *
 * \param tmsi (out) - Updated with the TMSI (if available).
 *
 * \return boolean - TRUE if available; FALSE otherwise.
 */
boolean rr_siminfo_get_tmsi(
  byte *tmsi,
  byte destsize
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;
  boolean sim_data_valid = FALSE;

  //RR_NULL_CHECK_RETURN_PARAM(tmsi, FALSE);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.tmsi_valid)
  {
    memscpy(tmsi,destsize,rr_sim_data_ptr->sim_data.tmsi,MM_SIM_TMSI_SIZE);
    sim_data_valid = TRUE;
  }

  return sim_data_valid;
}

/*!
 * \brief Gets the P-TMSI from the SIM.
 *
 * \param ptmsi (out) - Updated with the P-TMSI (if available).
 *
 * \return boolean - TRUE if available; FALSE otherwise.
 */
boolean rr_siminfo_get_ptmsi(
  byte *ptmsi,
  byte destsize
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;
  boolean sim_data_valid = FALSE;

  //RR_NULL_CHECK_RETURN_PARAM(ptmsi, FALSE);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.ptmsi_valid)
  {
    memscpy(ptmsi,destsize,rr_sim_data_ptr->sim_data.ptmsi,MM_SIM_PTMSI_SIZE);
    
    sim_data_valid = TRUE;
  }

  return sim_data_valid;
}

/*!
 * \brief Gets the MS Operational Mode from the SIM.
 *
 *
 * \return ms_operation_mode_T - the value of MS Operational Mode
 */
ms_operation_mode_T rr_siminfo_get_ms_opmode(
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.ms_opmode_valid)
  {
    return (ms_operation_mode_T) rr_sim_data_ptr->sim_data.ms_opmode;
  }

  return NORMAL_OPERATION;
}

/*!
 * \brief Returns TRUE if the inserted SIM is a Type Approval SIM
 * 
 * 
 * \return boolean - TRUE if a Type Approval SIM, FALSE otherise
 */
boolean rr_sim_is_type_approval(
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();

  RR_NULL_CHECK_RETURN_PARAM(rr_sim_data_ptr, FALSE);

  if (rr_sim_data_ptr->sim_data.ms_opmode_valid)
  {
    // See 3GPP 51.011 10.3.18 EFAD (Administrative data)
    if (rr_sim_data_ptr->sim_data.ms_opmode & 0x80)
    {
      return TRUE;
    }
  }

  return FALSE;
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Retrieves the MS Operational Mode for all subscriptions.
 *
 * \param op_mode_list (out) - array of operation modes.
 */
void rr_ms_get_sim_operation_modes(byte *op_mode_list)
{
  if ((op_mode_list != NULL) )
  {
    rr_sim_data_t *rr_sim_data_ptr;
    byte          *op_mode_ptr = op_mode_list;

      rr_sim_data_ptr = rr_sim_get_data_ptr();
      if (rr_sim_data_ptr->sim_data.ms_opmode_valid)
      {
        *op_mode_ptr = (byte)(rr_sim_data_ptr->sim_data.ms_opmode);
      }
      else
      {
        /* default to zero, implies NORMAL_OPERATION */
        *op_mode_ptr = 0x00;
      }
  }

  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Gets the IMSImod1000 value from the SIM.
 *
 * \param imsi_mod_1000 (out) - Updated with the value of IMSImod1000
 *
 * \return boolean - TRUE if SIM information was available, FALSE otherwise
 */
boolean rr_get_imsi_mod_1000(
  word *imsi_mod_1000
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_PARAM(imsi_mod_1000, FALSE);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.imsi_valid)
  {
    *imsi_mod_1000 = rr_sim_data_ptr->imsi_mod_1000;
    return TRUE;
  }

  return FALSE;
}

#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) 

/*!
 * \brief Gets the IMSImod10000000 value from the SIM.
 *
 * \param imsi_mod_10000000 (out) - Updated with the value of IMSImod1000
 *
 * \return boolean - TRUE if SIM information was available, FALSE otherwise
 */
boolean rr_get_imsi_mod_10000000(
  uint32 *imsi_mod_10000000
   
)
{
  rr_sim_data_t *rr_sim_data_ptr;

  //RR_NULL_CHECK_RETURN_PARAM(imsi_mod_10000000, FALSE);

  // Obtain a pointer to the module data
  rr_sim_data_ptr = rr_sim_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sim_data_ptr);

  if (rr_sim_data_ptr->sim_data.imsi_valid)
  {
    *imsi_mod_10000000 = rr_sim_data_ptr->imsi_mod_10000000;
    return TRUE;
  }

  return FALSE;
}
#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/


/* EOF */

