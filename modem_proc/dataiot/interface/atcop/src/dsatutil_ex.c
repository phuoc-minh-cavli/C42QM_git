/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D
                        
                        U T I L I T Y   S E R V I C E S

GENERAL DESCRIPTION
  This module contains utility functions that are used in executing/processing
  AT commands.

EXTERNALIZED FUNCTIONS
  dsatutil_strncmp_ig_sp_case
    This function compares two strings for <n> characters, ignoring case.
    Lower case letters are converted to upper case before the 
    difference is taken.

  dsatutil_itoa
    This function converts an integer to ASCII, using a specified
    radix. The resulting string is null terminated.

  dsatutil_atoi
    This function converts an ASCII string to an integer, using a specified
    radix.  Spaces are ignored. Letters used for radices above 10 may be of
    either case.

  dsatutil_strcmp_ig_sp_case
    This function compares two null terminated strings, ignoring case.

  dsatutil_reset_password
    Clear the password variable or specified length.
    The password string is set to 0xFF, null terminated.  
    
  dsatutil_convert_tuple
    This function checks for the validity of known tuple formats and converts
    between string and number representations.  For IPv6 addresses, the number
    pointer is assumed to be an array of uint32 values.

  dsatutil_init_timers_ex
    This function registers timers with the opeating system.

  dsatutil_dispatch_timer_handler
    This function dispatches the common timer expiry event to the
    correct timer handler.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2021 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatutil_ex.c#3 $ $DateTime: 2021/02/01 05:46:44 $ $Author: pwbldsvc $


when       who     what, where, why
--------   ---     -----------------------------------------------------------------
01/25/21   ks      Added support for FSK related AT commands.
11/09/20   ks      Added support for preserving URC indices across PSM cycles.
09/19/17   rk      Added support to read edrx values from NV 73871.
06/13/17   rk      Added support for +CEDRXS & +CEDRXRDP command.
05/01/17   skc     Featurizing PBM Code.
04/13/17   rg      Added rau and gprs ready timer for CPSMS
1/19/17    skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
09/26/16   skc     Changes made for PSM timer input to spec compliant.
06/13/16   skc     Added support for AT+CPSMS.
12/10/15   skc     Removing usage of enum TUPLE_TYPE_IPv4_254
11/24/14   pg      Fixed ws46 featurization issue.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#include "dstaski.h"
#include "nv.h"
#include "dsati.h"
#include "err.h"
#include "msg.h"
#include "amssassert.h"
#include "ps_utils.h"
#include <string.h>
#include <stdio.h>
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatvoice.h"

#include "dsatact.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#include "fs_public.h"
#include "fs_sys_types.h"
#include "fs_errno.h"

#include "ds_Utils_DebugMsg.h"
#include "ds_pdn_psm_hdlr.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
  These macros used to be in dsumtspdpreg.h but had to removed from there 
  due to compilation issues seen with pdp reg rpc stub code.
-------------------------------------------------------------------------*/
#define pdp_addr_ipv4 pdp_addr.pdp_addr_ipv4
#define pdp_addr_ipv6 pdp_addr.pdp_addr_ipv6

/*-------------------------------------------------------------------------
            Import External Variables
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
            Local Variables
-------------------------------------------------------------------------*/
/* To get whether the originated call is in dialing or alerting state. */
boolean dsat_mo_dailing_state;

/* Local Variable */
LOCAL dsat_string_item_type  csmp_fo_val[CSMP_FO_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  csmp_vp_val[CSMP_VP_STR_MAX_LEN+1];
#ifdef FEATURE_DSAT_ATCOP_VEND

LOCAL dsat_string_item_type  qcpdpp_passw_val[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+4];
LOCAL dsat_string_item_type  qcpdpp_uname_val[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+4];
#endif
LOCAL dsat_string_item_type  cusd_str_val[CUSD_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cusd_dcs_val[CUSD_DCS_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cgans_l2p_type_val[8];
LOCAL dsat_string_item_type  cpin_pin_val[MAX_PIN_STRING_LEN+1];
LOCAL dsat_string_item_type  cpin_newpin_val[MAX_PIN_STRING_LEN+1];
LOCAL dsat_string_item_type  cops_oper_val[COPS_OPER_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cpol_oper_val[CPOL_OPER_STR_MAX_LEN+1];
/* CPSMS */
//For NULL Terminated string already included and +2 for quotes
LOCAL dsat_string_item_type  cpsms_periodic_rau_str_val[CPSMS_TIMER_MAX_LEN+2];
LOCAL dsat_string_item_type  cpsms_gprs_ready_timer_str_val[CPSMS_TIMER_MAX_LEN+2];
LOCAL dsat_string_item_type  cpsms_periodic_tau_str_val[CPSMS_TIMER_MAX_LEN+2];
LOCAL dsat_string_item_type  cpsms_active_time_str_val[CPSMS_TIMER_MAX_LEN+2];
/*  CGDCONT  */
LOCAL dsat_string_item_type  cgdcont_pdptype_val[8];
LOCAL dsat_string_item_type  cgdcont_apn_val[DS_PDN_APM_MAX_APN_STRING_LEN+2];
LOCAL dsat_string_item_type  cgdcont_pdpaddr_val[DS_PDN_APM_MAX_APN_STRING_LEN+2];
/*  Value items: "+3" to allow for quotes and NULL  */
LOCAL dsat_string_item_type  cscb_mids_val[CSCB_MIDS_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cscb_dcss_val[CSCB_DCSS_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_es_orig_rqst_val[ES_ORGI_RQST_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_es_orig_fbk_val[ES_ORIG_FBK_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_es_ans_fbk_val[ES_ANS_FBK_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_trans_idle_val[ESA_FRAMED_IDLE_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_framed_un_ov_val[ESA_FRAMED_UN_OV_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_hd_auto_val[ESA_HD_AUTO_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_syn2_val[ESA_SYN2_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_csca_sca_val[CSCA_SCA_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_csca_tosca_val[CSCA_TOSCA_STR_MAX_LEN+1];
#if defined(FEATURE_DSAT_SP_CMDS)
LOCAL dsat_string_item_type  spc_val[NV_SEC_CODE_SIZE + 1];
LOCAL dsat_string_item_type  unlock_val[UNLOCK_STRING_LEN + 1];
LOCAL dsat_string_item_type  lock_val[NV_LOCK_CODE_SIZE + 1];
LOCAL dsat_string_item_type  old_lock_val[LOCK_OLD_STRING_LEN + 1];
LOCAL dsat_string_item_type  new_lock_val[NV_LOCK_CODE_SIZE + 1];
#endif /*defined(FEATURE_DSAT_SP_CMDS)*/
#ifdef FEATURE_DSAT_EDRX
LOCAL dsat_string_item_type  cedrxs_req_edrx_str_val[7];
#endif /* FEATURE_DSAT_EDRX */

/*Command table Val structure pointers*/
dsat_basic_table_val_struct_type               dsat_basic_vals;
dsat_basic_action_table_val_struct_type        dsat_basic_act_vals;
dsat_basic_sreg_table_val_struct_type          dsat_basic_sreg_vals;
dsat_basic_ext_table_val_struct_type           dsat_basic_ext_vals;
dsat_basic_vend_table_val_struct_type          dsat_basic_vend_vals;
dsat_etsi_ext_act_table_val_struct_type        dsat_etsi_ext_act_vals;

dsat_etsi_ext_table_val_struct_type            dsat_etsi_ext_vals;
dsat_basic_etsi_table_common_val               dsat_basic_etsi_cmn_vals;
/*--------------------------------------------------------------*/
/*ATCoP CM interface state vairables*/
dsatcmif_servs_state_info         dsatcmif_ph_val;

dsatetsicall_call_state_info      dsatcmif_call_val;

dsatetsicmif_sups_state_info      dsatcmif_sups_val;

dsatme_mmgsdi_state_info          dsatme_mmgsdi_val;

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
dsatme_pbm_state_info             dsatme_pbm_val;
#endif /* (FEATURE_ETSI_PBM) || (FEATURE_DSAT_CDMA_PBM) */

dsat_common_state_info            dsat_common_val;

dsatetsipkt_gprs_msg_info         dsatetsipkt_gprs_val;

dsat_psm_atcop_urc_state_info     dsat_psm_urc_param_val;

  dsat_num_item_type  dsat_cgauto_val;
/*Removed till here*/

/*---------------------------------------------------------------------------
     Operating system timers
---------------------------------------------------------------------------*/
LOCAL void dsatutil_timer_cb(void *cb_data);

dsat_timer_s_type dsatutil_timer_table[] =
{
  {DSAT_TIMER_VOICE_RING, dsatutil_timer_cb, dsatvoice_timer_expired_handler,
   0, FALSE, &dsatvoice_sys_mode, 0}  /* Duration varies, set in cmd handler */

  ,{DSAT_TIMER_VOICE_CALL_TIMEOUT, dsatutil_timer_cb, dsatvoice_timer_expired_handler,
   0, FALSE, &dsatvoice_voice_call_id, 0}/* Duration is the S7 value */

   ,{DSAT_TIMER_DATA_CALL_TIMEOUT, dsatutil_timer_cb, dsatvoice_timer_expired_handler,
   0, FALSE, &dsatvoice_data_call_id, 0}/* Duration is the S7 value */

#ifdef FEATURE_DSAT_ETSI_MODE
  ,{DSAT_INBAND_DTMF_RING, dsatutil_timer_cb, dsatetsicall_inband_dtmf_timer_expired_handler,
    0, FALSE,NULL,0}
#endif /* FEATURE_DSAT_ETSI_MODE */

  ,{DSAT_TIMER_FREE_ITEM, dsatutil_timer_cb, NULL, /* Initialized in dsatutil_init_timers_ex() */
   DSAT_FREE_ITEM_DLY, FALSE, NULL, 0}  
};

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL nv_stat_enum_type util_get_nv_item
(
  nv_items_enum_type        item,           /* Which item */
  nv_item_type             *data_ptr        /* Pointer to space for item */
);


LOCAL nv_stat_enum_type util_put_nv_item
(
  nv_items_enum_type       item,           /* Which item */
  nv_item_type            *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type   subs_id
);

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================
  Utility Functions
===========================================================================*/

/*===========================================================================

FUNCTION DSAT_IS_ETSI_MODE

DESCRIPTION
   Check if a particular mode passed is 3GPP based opertating mode or not
 
DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_etsi_mode(dsati_mode_e_type mode)
{
  if((mode == DSAT_MODE_GSM) || 
     (mode == DSAT_MODE_LTE))
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION DSAT_IS_VALID_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid

DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_valid_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index
)
{
  if( (tok_ptr!= NULL &&
       token_index < tok_ptr->args_found) && 
      (NULL != tok_ptr->arg[token_index]) && 
      ('\0' != *tok_ptr->arg[token_index]) )
  {
    return TRUE;
  }
  DS_AT_MSG1_HIGH("Empty Argument:  %d", token_index);
  return FALSE;
}

/*===========================================================================

FUNCTION DSAT_IS_VALID_NUM_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid and also converts the string to integer. It checks
   if the token passed is in range.

DEPENDENCIES

RETURN VALUE
  dsat_err_e_type

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_err_e_type dsat_is_valid_num_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index,
  dsat_num_item_type        range_min,
  dsat_num_item_type        range_max,
  void                     *output_buf
)
{

  dsat_num_item_type    *temp = NULL;

  if (tok_ptr == NULL || output_buf == NULL)
  {
    DS_AT_MSG1_HIGH("Invalid Argument:  %d", token_index);
    return DSAT_ERR_INVALID_TOKENS;
  }

  temp = (dsat_num_item_type *)output_buf;

  if( (token_index < tok_ptr->args_found) && 
      (NULL != tok_ptr->arg[token_index]) && 
      ('\0' != *tok_ptr->arg[token_index]) )
  {
    if(ATOI_OK == dsatutil_atoi(temp,tok_ptr->arg[token_index],10) &&
       (*temp <= range_max) &&(*temp >= range_min))
    {
      return DSAT_ERR_NO_ERROR;
    }
    DS_AT_MSG1_ERROR("[INT] Conversion Fail or Out of Range token: %d", token_index);
    return DSAT_ERR_ATOI_CONVERSION_FAILURE;
  }

  DS_AT_MSG1_HIGH("Empty Argument:  %d", token_index);
  return DSAT_ERR_INVALID_TOKENS;
}

/*===========================================================================

FUNCTION DSAT_IS_VALID_STRING_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid and also strip the quotes from the token_ptr passed.

DEPENDENCIES

RETURN VALUE
  dsat_err_e_type

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_err_e_type dsat_is_valid_string_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index,
  void                     *input_buf,
  void                     *output_buf,
  word                      str_len
)
{
  const byte *input_temp  = NULL;
  byte       *output_temp = NULL;

  if (tok_ptr == NULL || output_buf == NULL || input_buf == NULL || str_len <= 0)
  {
    DS_AT_MSG1_HIGH("Invalid Argument:  %d", token_index);
    return DSAT_ERR_INVALID_TOKENS;
  }

  input_temp  = (const byte *) input_buf;
  output_temp = (byte *) output_buf;

  if( (token_index < tok_ptr->args_found) && 
      (NULL != tok_ptr->arg[token_index]) && 
      ('\0' != *tok_ptr->arg[token_index]) )
  {
    if ( dsatutil_strip_quotes_out(input_temp,
                                   output_temp,
                                   str_len))
    {
      return DSAT_ERR_NO_ERROR;
    }
    DS_AT_MSG1_ERROR("[String] Conversion Fail or Out of Range token: %d", token_index);
    return DSAT_ERR_QUOTE_STRIP_FAILURE;
  }

  DS_AT_MSG1_HIGH("Empty Argument:  %d", token_index);
  return DSAT_ERR_INVALID_TOKENS;
}

/*===========================================================================

FUNCTION DSAT_IS_PROFILE_ID_IN_RANGE

DESCRIPTION
   Check if a particular profile_id being passed is in valid range or not
 
DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_profile_id_in_range(ds_profile_num_type profile_id)
{
  if((profile_id >= DSAT_LEGACY_PROFILE_NUM_MIN )&&( profile_id <= DSAT_LEGACY_PROFILE_NUM_MAX))
  {
    return TRUE;
  }
  return FALSE;

}
/*===========================================================================

FUNCTION DSATUTIL_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFFFFFF and a radix of 2,
  which returns 33 bytes (32 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
byte * dsatutil_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
)
{
  byte buf[MAX_ITOA_LEN], c;
  int n;

  n = sizeof(buf) - 1;

  buf[n] = '\0';


  do
  {
    c = (byte) (v % r);

    if (n <= 0)
    {
      DS_AT_MSG0_HIGH("OVERFLOW");
      break;
    }

    buf[--n] = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');

  } while ( (v /= r) > 0);

  while ( (*s++ = buf[n++]) != 0)
      ;

  return (s-1);
} /*  dsatutil_itoa */


/*===========================================================================

FUNCTION DSATUTIL_STRCMP_IG_SP_CASE

DESCRIPTION
  This function compares two null terminated strings, ignoring case.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical, except for case,
            otherwise, it returns the difference between the first
            characters to disagree. The difference is first arg minus
            second arg.  Lower case letters are converted to upper
            case before the difference is taken.

SIDE EFFECTS
  None.

===========================================================================*/
int dsatutil_strcmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2              /*  Pointer to string 2 */
)
{
  byte c1, c2;
  int d;

  do
  {
    c1 = *s1++;
    c2 = *s2++;

    d = UPCASE(c1) - UPCASE(c2);
  }
  while ( d == 0 && c1 != '\0' && c2 != '\0');

  return d;
} /*  dsatutil_strcmp_ig_sp_case  */

/*===========================================================================

FUNCTION DSATUTIL_STRNCMP_IG_SP_CASE

DESCRIPTION
  This function compares two strings for <n> characters, ignoring case.
  Lower case letters are converted to upper case before the 
  difference is taken.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical till <n> characters, except for 
            case otherwise, it returns the difference between 
            the first characters to disagree. The difference is first arg 
            minus second arg.  

SIDE EFFECTS
  None.

===========================================================================*/
int dsatutil_strncmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2,             /*  Pointer to string 2 */
  uint32      n               /* Number of characters to compare */
)
{
  byte c1, c2;
  int d;

  if ( n== 0 )
  {
    return -1;
  }
  do
  {
    c1 = *s1++;
    c2 = *s2++;

    d = UPCASE(c1) - UPCASE(c2);
    --n;
  }
  while ( d == 0 && c1 != '\0' && c2 != '\0' && n != 0 );

  return d;
}/* dsatutil_strncmp_ig_sp_case */

/*===========================================================================

FUNCTION DSATUTIL_ATOI

DESCRIPTION
  This function converts an ASCII string to an integer, using a specified
  radix.  Spaces are ignored. Letters used for radices above 10 may be of
  either case.

DEPENDENCIES


RETURN VALUE
  Returns 
    ATOI_OK           : for a correct conversion,
    ATOI_NO_ARG       : 1 if no argument was found,
    ATOI_OUT_OF_RANGE : if a character or the total value is out of range.

SIDE EFFECTS

===========================================================================*/

atoi_enum_type dsatutil_atoi
(
  dsat_num_item_type *val_arg_ptr,      /*  value returned  */
  const byte *s,                        /*  points to string to eval  */
  unsigned int r                        /*  radix */
)
{
  atoi_enum_type err_ret = ATOI_NO_ARG;
  byte c;
  dsat_num_item_type val, val_lim, dig_lim;

  val = 0;
  val_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM / r);
  dig_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM % r);

  while ( (c = *s++) != '\0')
  {
    if (c != ' ')
    {
      c = (byte) UPCASE (c);
      if (c >= '0' && c <= '9')
      {
        c -= '0';
      }
      else if (c >= 'A')
      {
        c -= 'A' - 10;
      }
      else
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too small */
        break;
      }

      if (c >= r || val > val_lim
          || (val == val_lim && c > dig_lim))
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too large */
        break;
      }
      else
      {
        err_ret = ATOI_OK;            /*  arg found: OK so far*/
        val = (dsat_num_item_type) (val * r + c);
      }
    }
    *val_arg_ptr =  val;
  }
  
  return err_ret;

} /*  dsatutil_atoi */



/*===========================================================================

FUNCTION DSATUTIL64_ATOI

DESCRIPTION
  This function converts an ASCII string to an integer, using a specified
DEPENDENCIES


RETURN VALUE
  Returns 
    ATOI_OK           : for a correct conversion,
    ATOI_NO_ARG       : 1 if no argument was found,
    ATOI_OUT_OF_RANGE : if a character or the total value is out of range.

SIDE EFFECTS

===========================================================================*/

atoi_enum_type dsatutil64_atoi
(
  uint64       *val_arg_ptr,      /*  value returned  */
  const byte    *s                /*  points to string to eval  */
)
{
  atoi_enum_type err_ret = ATOI_NO_ARG;
  byte c;
  uint64  val;
 #define MAX_VAL64_NUM_ITEM 0xFFFFFFFFFFFFFFFF /*  Max value of a numeric AT parm     */

  val = 0;

  while ( (c = *s++) != '\0')
  {
    if (c != ' ')
    {
      c = (byte) UPCASE (c);
      if (c >= '0' && c <= '9')
      {
        c -= '0';
      }
      else if (c >= 'A')
      {
        c -= 'A' - 10;
      }
      else
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too small */        
        break;
      }

      if(val > MAX_VAL64_NUM_ITEM)
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too large */        
        break;
      }
      else
      {
        err_ret = ATOI_OK;            /*  arg found: OK so far*/
        val = (uint64) (val * 10 + c);
      }
    }
    *val_arg_ptr =  val;
  }
  
  return err_ret;

} /*  dsatutil_atoi */

/*===========================================================================

FUNCTION DSATUTIL_RESET_PASSWORD

DESCRIPTION
  This function clears the password variable of specified length.
  The password string is set to 0xFF, null terminated.  

  The provided 'length' should be the total length of the buffer pointed to by 'password' including allocation 
  for the trailing NULL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatutil_reset_password
(
  byte * password,            /* String containing password */
  word length                 /* Maximum length of storage pointed to by "password" */
)
{
  int index;

  /* Do nothing if null pointer passed */
  if (NULL == password) return; 

  /* init the password */
  for (index = 0; index < length-1; index++)
  {
    password[index] = (char) 0xFF;
  }
  password[index] = 0;
  return;
} /* dsatutil_reset_password */

/*===========================================================================

FUNCTION DSATUTIL_DSM_NEW_BUFFER

DESCRIPTION
  This function is a wrapper for dsm_new_buffer() API.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsatutil_dsm_new_buffer
(
  dsm_mempool_id_type  pool_id,
  const char          *filename,
  uint32               line,
  boolean              graceful
)
{
  dsm_item_type *dsm_item_ptr = NULL;

  dsm_item_ptr = dsmi_new_buffer(pool_id, filename, line);
  if (NULL == dsm_item_ptr)
  {
    if (TRUE == graceful)
    {
      DS_AT_MSG_SPRINTF_3_ERROR("DSM items exhausted in pool 0x%x (%s, %d)",
                                pool_id, filename, line);
    }
    else
    {
      ERR_FATAL("DSM items exhausted in pool 0x%x", pool_id, 0, 0);
    }
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated DSM item 0x%x in pool 0x%x (%s, %d)",
                            pool_id, dsm_item_ptr, filename, line);
  }

  return dsm_item_ptr;
} /* dsatutil_dsm_new_buffer */

/*===========================================================================

FUNCTION DSATUTIL_DSM_PUSHDOWN_TAIL

DESCRIPTION
  This function is a wrapper for dsm_pushdown_tail() API.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the pushdown requires an additional data item, the pkt_ptr field of the 
  last item in the passed packet chain may be changed.

===========================================================================*/
void dsatutil_dsm_pushdown_tail
(
  dsm_item_type       **pkt_head_ptr,
  const void           *buf,
  uint16                size,
  dsm_mempool_id_type   pool_id,
  const char           *filename,
  uint32                line,
  boolean               graceful
)
{
  uint16 bytes_copied;

  bytes_copied = dsmi_pushdown_tail(pkt_head_ptr, buf, size, pool_id, filename, line);
  if (bytes_copied != size)
  {
    if (NULL != *pkt_head_ptr)
    {
      dsmi_free_packet(pkt_head_ptr, filename, line);
      *pkt_head_ptr = NULL;
    }
  }

  if (NULL == *pkt_head_ptr)
  {
    if (TRUE == graceful)
    {
      DS_AT_MSG_SPRINTF_5_ERROR("Failed to pushdown data of size %d on DSM packet "
                                "in pool 0x%x and bytes copied %d (%s, %d)",
                                size, pool_id, bytes_copied, filename, line);
    }
    else
    {
      ERR_FATAL("Failed to pushdown data of size %d on DSM packet in pool 0x%x and"
                " bytes copied %d", size, pool_id, bytes_copied);
    }
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Pushed down data of size %d on DSM packet in pool "
                            "0x%x (%s, %d)", size, pool_id, filename, line);
  }

  return;
} /* dsatutil_dsm_pushdown_tail */

/*===========================================================================

FUNCTION DSATUTIL_DSM_CREATE_PACKET

DESCRIPTION
  This function creates a DSM packet.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsatutil_dsm_create_packet
(
  const void *buf,
  uint16      size,
  const char *filename,
  uint32      line,
  boolean     graceful
)
{
  dsm_item_type *dsm_item_ptr = NULL;

  dsm_item_ptr = dsatutil_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL,
                                         filename, line, graceful);
  if (NULL == dsm_item_ptr)
  {
    dsm_item_ptr = dsatutil_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL,
                                           filename, line, graceful);
  }

  if (NULL != dsm_item_ptr)
  {
    (void) dsatutil_dsm_pushdown_tail(&dsm_item_ptr, buf, size,
                                      DSM_ITEM_POOL(dsm_item_ptr),
                                      filename, line, graceful);
  }

  return dsm_item_ptr;
} /* dsatutil_dsm_create_packet */

/*===========================================================================

FUNCTION DSATUTIL_INIT_OS_TIMERS

DESCRIPTION
  This function registers timers with the opeating system.  Starting
  and clearing timers is done in individual command handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_init_timers_ex ( void )
{
  dsat_timer_s_type *t_ptr = NULL;  
  dsat_timer_id_e_type index;

  for(index = 0; index < DSAT_TIMER_MAX; index++)
  {
    t_ptr = &dsatutil_timer_table[index];

    if ( !t_ptr->is_defined )
    {
      /* Register each timer */
      t_ptr->timer_handle = ps_timer_alloc(t_ptr->cb_func, (void*)(&t_ptr->timer_id));
      t_ptr->is_defined = TRUE;
    }
  }
} /* dsatutil_init_timers_ex */

/*===========================================================================

FUNCTION DSATUTIL_DISPATCH_TIMER_HANDLER

DESCRIPTION
  This function dispatches the common timer expiry event to the
  correct timer handler.  If the timer is not found, an error message
  is generated.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNCH_EVENT : asynch event has been dispatched

SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsatutil_dispatch_timer_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type  result       = DSAT_ASYNC_EVENT;
  boolean                timer_found  = FALSE;
  uint8                  i            = 0;
  dsat_timer_id_e_type  *timer_id_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  /* Loop over timer table */
  for(i = 0; i < (uint8)DSAT_TIMER_MAX; i++)
  {
    /* Check for match of timer ID */
    if ( ((unsigned int)dsatutil_timer_table[i].timer_id == *timer_id_ptr) &&
         (NULL != dsatutil_timer_table[i].hdlr_func))
    {
      timer_found = TRUE;
      /* Invoke the timer handler function */
      result = dsatutil_timer_table[i].hdlr_func (cmd_ptr);
    }
  }

  /* Report error if unknown timer */
  if( FALSE == timer_found )
  {
    DS_AT_MSG1_ERROR("Timer ID or handler not found in table: %d",
         *timer_id_ptr);
  }

  return result;
} /* dsatutil_dispatch_timers */

/*===========================================================================
FUNCTION UTIL_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL nv_stat_enum_type util_get_nv_item
(
  nv_items_enum_type         item,           /* Which item */
  nv_item_type              *data_ptr        /* Pointer to space for item */
)
{
  nv_cmd_type  nv_command;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset((void*)&nv_command, 0x0, sizeof(nv_cmd_type));

 
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  if (rex_self() == rcinit_lookup_rextask("ds"))
  {
    //nv_command.status = ds_pdn_nv_manager_read_legacy_nv( item, data_ptr);

      nv_command.status = ds_get_nv_item(item,data_ptr);

  }
#ifdef FEATURE_ATCOP_TEMP_NV_ALT
  else if (rex_self() == rcinit_lookup_rextask("ps")) 
  {
        nv_command.status = ps_get_nv_item (item, data_ptr);
  }
#endif
#else
  if (rex_self() == &ds_tcb)
  {
    //nv_command.status = ds_pdn_nv_manager_read_legacy_nv( item, data_ptr);
    
    nv_command.status = ds_get_nv_item(item,data_ptr);
  }
#ifdef FEATURE_ATCOP_TEMP_NV_ALT
  else if (rex_self() == &ps_tcb) 
  {
    //@SANJAYKC: TODO
    nv_command.status = ps_get_nv_item (item, data_ptr);
  }
#endif
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

  if (NV_DONE_S != nv_command.status)
  {
    DS_AT_MSG2_ERROR("Failed to get NV item %d, status = %d",
                     item, nv_command.status);
  }

  return( nv_command.status );
}/* util_get_nv_item */

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  return util_get_nv_item(item, data_ptr);
}

/*===========================================================================
FUNCTION UTIL_PUT_NV_ITEM_WRAPPER

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL nv_stat_enum_type util_put_nv_item
(
  nv_items_enum_type     item,           /* Which item */
  nv_item_type           *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type subs_id
)
{
  nv_cmd_type  nv_command;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset((void*)&nv_command, 0x0, sizeof(nv_cmd_type));

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  if (rex_self() == rcinit_lookup_rextask("ds"))
  {
    /*nv_command.status = ds3gcfgmgr_write_legacy_nv_ex ( item, 
                                                        data_ptr,
                                                        (sys_modem_as_id_e_type)subs_id);*/      
      nv_command.status = ds_put_nv_item(item,data_ptr);
  }
  
#ifdef FEATURE_ATCOP_TEMP_NV_ALT
  else if (rex_self() == rcinit_lookup_rextask("ps")) 
  {
        nv_command.status = ps_put_nv_item (item, data_ptr);
  }
#endif
#else
  if (rex_self() == &ds_tcb)
  {
   /* nv_command.status = ds3gcfgmgr_write_legacy_nv_ex ( item, 
                                                        data_ptr,
                                                        (sys_modem_as_id_e_type)subs_id);*/
       
     nv_command.status = ds_put_nv_item(item,data_ptr);
  }  
#ifdef FEATURE_ATCOP_TEMP_NV_ALT
  else if (rex_self() == &ps_tcb) 
  {
        nv_command.status = ps_put_nv_item (item, data_ptr);
  }
#endif
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
 if (NV_DONE_S != nv_command.status)
  {
    DS_AT_MSG2_ERROR("Failed to put NV item %d, status = %d",
                     item, nv_command.status);
  }

  return( nv_command.status );
}/* util_put_nv_item */


/*===========================================================================
FUNCTION DSATUTIL_PUT_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  return util_put_nv_item(item, data_ptr,SYS_MODEM_AS_ID_1);
}

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_TUPLE

DESCRIPTION
  This function checks for the validity of known tuple formats and converts
  between string and number representations.  
  For IPV6 addresses only the preferred text representation of RFC2373 is 
  supported: quantity 8, 16 bit HEX numbers, separated by colons and without
  zero compressing double colon pairs "::".
    
      For IPv6 addresses, the number
  pointer is assumed to be an array of uint32 values.
  
  Tuples supported:
     IPv4       range: 0.0.0.0 - 255.255.255.255
     IPv4_254   range: 0.0.0.0 - 254.254.254.254
     PORT       range: 0.0 - 65535.65535
     TMASK      range: 0.0 - 255.255
     IPv6       range: 0:0:0:0:0:0:0:0 - 
                         FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF

DEPENDENCIES
  For IPv6 addresses, the number pointer is assumed to be an array[4] of
  uint32 values.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if validation successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_convert_tuple 
(
   tuple_convert_enum_type mode,             /* Mode of conversion      */
   tuple_type_enum_type ttype,               /* Type of tuple           */
   uint32* num_ptr,                          /* Integer representation */
   byte* buf_ptr,                            /* String representation   */
   uint8 buf_len,                            /* String length           */
   dsat_ip_mask_enum_type ip_mask            /* IP or Mask Convertion   */
)
{
  #define MAX_TOKENS 16             /* Max num of delimiter separated tokens */
  #define MAX_TOKEN_LEN 5           /* Max num chars in a token */

  /* Only RFC2732 preferred text representation for IPV6 addresses uses ':'
     separators */
  char delimiter = 
    (ttype == TUPLE_TYPE_IPv6 ? IPV6_DELIMITER : IPV4_DELIMITER);
  /* When parsing IPV6 addresses, how many tokens per uint32 number */
  uint8 max_tokens_per_number = (ttype == TUPLE_TYPE_IPv6_OCTETS ? 4 : 2);
  atoi_enum_type result;
  /* Only RFC2732 preferred text representation for IPV6 addresses uses hex
     characters */
  byte radix = (ttype == TUPLE_TYPE_IPv6 ? 16 : 10);
  uint8  num_tokens;
  uint8  token_bits;
  uint32 token_mask;
  uint32 token_limit;
  dsat_num_item_type val[MAX_TOKENS]={0};
  uint32 *num_ptr_orig = num_ptr; 
  uint8 num_ptr_max_len=1;

  /* Setup conversion paramaters */
  switch (ttype)
  {
  case TUPLE_TYPE_IPv4:
    token_bits = 8;
    token_mask = token_limit = (1 << token_bits) - 1;
    max_tokens_per_number = num_tokens = 4;
    break;

  case TUPLE_TYPE_PORT:
    token_bits = 16;
    token_mask = token_limit = (1 << token_bits) - 1;
    max_tokens_per_number = num_tokens = 2;
    break;
    
  case TUPLE_TYPE_TMASK:
    token_bits = 8;
    token_mask = token_limit = (1 << token_bits) - 1;
    max_tokens_per_number = num_tokens = 2;
    break;
    
  case TUPLE_TYPE_IPv6:
    token_bits = 16;
    token_mask = token_limit = (1 << token_bits) - 1;
    num_tokens = 8;
    max_tokens_per_number = 2;
    num_ptr_max_len = 4;
    break;
    
  case TUPLE_TYPE_IPv6_OCTETS:
    token_bits = 8;
    token_mask = token_limit = (1 << token_bits) - 1;
    num_tokens = 16;
    max_tokens_per_number = 4;
    num_ptr_max_len = 4;
    break;
    
  default:
    DS_AT_MSG1_ERROR("Unsupported tuple type: %d", ttype);
    return DSAT_ERROR;
  }

  /* Perform conversion */
  if (STRING_TO_INT == mode)
  { 
#define NUM_BITS_VAL 32

    int cpi=0, vali=0;
    byte cp[MAX_TOKEN_LEN+1];
    byte * c = buf_ptr;
    int i,j;
    boolean loop = TRUE;
    memset((void*)cp,0,sizeof(cp));

    /*---------------------------------------------------------------
      The following loop processes the string representing an tuple.
      Each token is converted in to decimal integer and stored in
      a temporary array.
      If the value is greater than the limit, we return an error.
    ---------------------------------------------------------------*/
    while (loop && (num_tokens > vali))
    {
      /* Skip quotes */
      if ('"' != *c)
      {
        /* Build token until delimiter or end of buffer */
        loop = (('\0' != *c)? TRUE : FALSE );
        if ( (delimiter != *c) && loop )
        {
          cp[cpi++] = *c;

          if (MAX_TOKEN_LEN < cpi)
          {
            DS_AT_MSG2_ERROR("Token exceeds max length: %d < %d",
                       MAX_TOKEN_LEN,cpi);
            return DSAT_ERROR;
          }
        }
        else 
        {
          /* Convert to integer */
          result = dsatutil_atoi( (val+vali), cp, radix);
          if (ATOI_OK != result)
          {
            return DSAT_ERROR;
          }

          /* Test token against limit */
          if((*(val+vali)) > token_limit)
          {
            DS_AT_MSG2_MED("Token %d larger than limit %d",*(val+vali),token_limit);
            return DSAT_ERROR;
          }

          /* Reinitialize buffer */
          memset((void*)cp,0,sizeof(cp));
          cpi = 0;         
          vali++;
        }
      }
      c++;
    }
    
    /* Verify expected tokens found */
    if (num_tokens != vali)
    {
      DS_AT_MSG2_ERROR("Expected number of tokens violated: %d != %d",
                num_tokens,vali);
      return DSAT_ERROR;
    }

    /* Convert to number, processing tokens in order encountered */
    *num_ptr = 0;
    j = 0;
    for (i = num_tokens - 1; i >= 0; i--)
    {
      /* Check for number array element change (IPv6 only) */
      if (max_tokens_per_number <= j)
      {
        num_ptr++;
        if ((num_ptr - num_ptr_orig) >= num_ptr_max_len)
        {
          DS_AT_MSG1_ERROR("exceeded num_ptr_max_len %d",num_ptr_max_len);
          return DSAT_ERROR;
        }

        *num_ptr = 0;
        j = 0;
      }

      *num_ptr = (*num_ptr >> token_bits) | 
                 (val[i] << (NUM_BITS_VAL - token_bits));
      j++;

      /* If all the tokens per this 32 bit number have been processed and
         it didn't use up all 32 bits of number:
         right shift number down by quantity of unused bits.
         (this currently only applies to case  of TOS/mask) */
      if ( j == max_tokens_per_number &&
           NUM_BITS_VAL > (token_bits * max_tokens_per_number) ) 
      {
        *num_ptr = (*num_ptr >> (NUM_BITS_VAL - 
                                 (token_bits * max_tokens_per_number)));
      }
    }
  }
  else /* INT_TO_STRING */
  {
    uint32 num;
    int i,j;
    dsat_num_item_type   cgpiaf_add_format        = 0;
    dsat_num_item_type   cgpiaf_LeadingZero_format = 0;
    dsat_num_item_type   cgpiaf_CompressZero_format = 0;
    byte num_tokens_per_word = 
      ((ttype == TUPLE_TYPE_IPv6 || ttype == TUPLE_TYPE_IPv6_OCTETS) ? 
        (num_tokens >> 2) : num_tokens);
    byte delim_str[2];

    delim_str [0] = delimiter;
    delim_str [1] = '\0';

    memset ((void*)buf_ptr, 0, buf_len);
    j = num_tokens_per_word-1;

    /* uint32 from DSUMTS looks like this: 
       byte0="4", byte1="3", byte2="2", byte 3="1" for IP address like
       "1.2.3.4" so parse off token bits from bottom of uint32s
       inserting into val array starting from top */
    for (i = num_tokens - 1; i >= 0; i--)
    {
      if ((num_ptr - num_ptr_orig) >= num_ptr_max_len)
      {
        DS_AT_MSG1_ERROR("exceeded num_ptr_max_len %d",num_ptr_max_len);
        return DSAT_ERROR;
      }
      num = *num_ptr;
      num >>= (token_bits * ((num_tokens_per_word-1) - j));

      val[i] = (dsat_num_item_type)(num & token_mask);
      j--;
      
      /* Check for number array element change (IPv6 only) */
      if (0 > j)
      {
        num_ptr++;
        j = num_tokens_per_word-1;
      }
    }
    
    /* then go through val array of token integers creating delimited 
       string */
    /* Decides IPv6 address representation based on +CGPIAF setting*/
     cgpiaf_add_format          = (dsat_num_item_type)dsatutil_get_val(
                                      DSATETSI_EXT_CGPIAF_IDX,0,NUM_TYPE);

    if( (DSAT_CONVERT_IP == ip_mask)&&
        (ttype == TUPLE_TYPE_IPv6)&&
        (1 == cgpiaf_add_format))
    {
      int last_count = 0;
      int count = 0;
      int index = 0;
      int last_token = 0;
      byte work_buffer[6];

      cgpiaf_CompressZero_format  = (dsat_num_item_type)dsatutil_get_val(
                                        DSATETSI_EXT_CGPIAF_IDX,3,NUM_TYPE);
      if(1 == cgpiaf_CompressZero_format)
      {
        for(i = 0 ; i < num_tokens ; i++)
        {
          if(val[i] == 0)
          {   
            count++;
            if(last_count <= count)
            {
               last_count = count;
               index = i;
            }
          }
          else
          {
              count = 0;
          }
        }
      }
      DS_AT_MSG3_HIGH("MAX count %d last index %d first index %d \n",
                                last_count,index,index-last_count+1);
      
      cgpiaf_LeadingZero_format  = (dsat_num_item_type)dsatutil_get_val(
                                        DSATETSI_EXT_CGPIAF_IDX,2,NUM_TYPE);

      for( i = 0 ; i < num_tokens ; i++)
      {
        if (i == (num_tokens - 1))
          last_token = 1;

        if(i < index-last_count+1 || i > index)
        {
          if(1 == cgpiaf_LeadingZero_format)
          {
            snprintf((char *)work_buffer, sizeof(work_buffer),
                     "%04X%c",
                     val[i], (last_token ? '\0' : delimiter));
          }
          else
          {
            snprintf((char *)work_buffer, sizeof(work_buffer),
                     "%X%c",
                     val[i], (last_token ? '\0' : delimiter));
          }
          strlcat((char *)buf_ptr, (char *)work_buffer, buf_len);
        }
        else
        {
          if(i == index)
          {
            strlcat((char *)buf_ptr, (char *)delim_str, buf_len);
          }
          if(0 == index-last_count+1 && i == 0)
          {
            strlcat((char *)buf_ptr, (char *)delim_str, buf_len);
          }
        }
      }
    }
    else
    {
      /* This size is guaranteed not to overflow with any radix */
      byte work_buf[MAX_ITOA_LEN];
      for (i=0; i < num_tokens; i++)
      {
        dsatutil_itoa (val[i], work_buf, radix);
        strlcat((char *)buf_ptr, (char *)work_buf, buf_len);
        if (i < (num_tokens - 1))
          strlcat((char *)buf_ptr, (char *)delim_str, buf_len);
      }
    }    
  }

  return DSAT_OK;
} /* dsatutil_convert_tuple */


/*===========================================================================

FUNCTION DSATUTIL_TIMER_CB

DESCRIPTION
  This function is called when timer expires.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void dsatutil_timer_cb(void *cb_data)
{
  /* submit dstask a cmd */
  ds_command_type               * cmd_ptr     = NULL;

  /*-------------------------------------------------------------------------
    Send a DS_TIMER_EXPIRED_CMD to the DS task, and indicate which timer
    expired.
  -------------------------------------------------------------------------*/
  cmd_ptr = dsat_get_cmd_buf(sizeof(dsat_timer_id_e_type), TRUE);

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_TIMER_EXPIRED_CMD;
    cmd_ptr->cmd_payload_ptr = cb_data;
    ds_put_cmd( cmd_ptr );
  }
} /* dsatutil_timer_cb */

#ifdef FEATURE_DSAT_ETSI_DATA

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_IPADDR

DESCRIPTION
  This function converts between IP V4/V6 addresses and ds_umts_pdp_addr_types.
  In identifies the type of conversion to be done and passes the rest of 
  the work off to the convert tuple function. In string to number mode the 
  presence of a colon in string identifies an IPV6 conversion else IPV4 is 
  assumed. In number to string mode the setting of 
  ds_umts_pdp_addr_type.ip_vsn identifies conversion type.

DEPENDENCIES
  Buffer pointed to by buf_ptr should be at least MAX_IPADDR_STR_LEN bytes 
  in length to hold NULL terminated, maximum length IPV6 address for 
  INT_TO_STRING conversion.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_OK : if validation successful
    DSAT_ERROR : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_convert_ipaddr
(
   tuple_convert_enum_type mode,             /* Mode of conversion      */
   dsat_ip_addr_type* addr_ptr,          /* Number representation   */
   byte* buf_ptr                             /* NULL terminated string  */
)
{
  /* word aligned address */
  uint32 addr32[4];
  dsat_num_item_type   cgpiaf_add_format = 0;
  tuple_type_enum_type tuple_type = TUPLE_TYPE_MAX;

  memset( (void *)addr32, 0, (sizeof(uint32) * 4) );

  if ( INT_TO_STRING == mode )  
  {
    /* copy to local word aligned address */
    if ( DS_PROFILE_IP_V6 == addr_ptr->ip_vsn )
    {
          addr32[0] = addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[0];
          addr32[1] = addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[1];
          addr32[2] = addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[2];
          addr32[3] = addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[3];
    }
    else
    {
          addr32[0] = addr_ptr->addr.pdp_addr_ipv4;
    }
  }

  /* For IPV6 conversion either mode... */
  if ( (STRING_TO_INT == mode && strchr((char *)buf_ptr, IPV6_DELIMITER)) ||
           (INT_TO_STRING == mode && DS_PROFILE_IP_V6 == addr_ptr->ip_vsn) )
  {
    /* PDP address parm may not be given but IPV6 delimiter has already 
       been found in string, so no need to test for NULL string as
       in else block below */
       
    /* Decides IPv6 address representation based on +CGPIAF setting*/
    tuple_type = TUPLE_TYPE_IPv6;
    if(INT_TO_STRING == mode)
    {
      cgpiaf_add_format = (dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_CGPIAF_IDX,0,NUM_TYPE);
      /* "1" Use IPv6-like colon-notation. IP address */
      if(1 == cgpiaf_add_format)
      {
         tuple_type = TUPLE_TYPE_IPv6;
      }
      else
      {
         tuple_type = TUPLE_TYPE_IPv6_OCTETS;
      }
    }
    if ( DSAT_OK != dsatutil_convert_tuple(
                                     mode,
                                     tuple_type,
                                     addr32,
                                     buf_ptr,
                                     MAX_IPADDR_STR_LEN+1,
                                     DSAT_CONVERT_IP) )
    {
      DS_AT_MSG0_ERROR("Failed IPV6 address conversion");
      return DSAT_ERROR;
    }
    if ( STRING_TO_INT == mode )
    {
          addr_ptr->ip_vsn = DS_PROFILE_IP_V6;
    }
  }
  else /* Assume IPV4 conversion */
  {
    if ( DSAT_OK != dsatutil_convert_tuple(mode,
                                           TUPLE_TYPE_IPv4,
                                           addr32,
                                           buf_ptr,
                                           MAX_IPADDR_STR_LEN+1,
                                           DSAT_CONVERT_IP) )
    {
      DS_AT_MSG0_ERROR("Failed IPV4 address conversion");
      return DSAT_ERROR;
    }
    if ( STRING_TO_INT == mode )
    {
          addr_ptr->ip_vsn = DS_PROFILE_IP_V4;
    }
  }

  if ( STRING_TO_INT == mode )
  {
    /* copy from local word aligned address */
    //@sanjaykc: check functionality
    if ( DS_PROFILE_IP_V6 == addr_ptr->ip_vsn )
    {
      addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[0] = addr32[0];
      addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[1] = addr32[1];
      addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[2] = addr32[2];
      addr_ptr->addr.pdp_addr_ipv6.in6_u.u6_addr32[3] = addr32[3];
    }
    else
    {
      addr_ptr->addr.pdp_addr_ipv4 = addr32[0];
    }
  }

  return DSAT_OK;
} /* dsatutil_convert_ipaddr */

#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================
FUNCTION  DSATUTIL_VALIDATE_MEMORY_MSG_ERROR

DESCRIPTION
  Validates an allocated block of memory from Modem Heap memory
  and calls an error F3 if invalid.

DEPENDENCIES
  None

RETURNS
  The Data Pointer to the allocated Memory.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_validate_memory_msg_error
(
  void       *data_ptr,
  uint32      data_size,
  const char *filename,
  uint32      line
)
{
  if (NULL == data_ptr)
  {
    DS_AT_MSG_SPRINTF_3_ERROR("Failed to allocate heap memory block of size %d (%s, %d)",
                              data_size, filename, line);
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated heap memory block 0x%x of size %d (%s, %d)",
                            data_ptr, data_size, filename, line);

    memset(data_ptr, 0x00, data_size);
  }

  return data_ptr;
} /* dsatutil_validate_memory_msg_error */

/*===========================================================================
FUNCTION  DSATUTIL_VALIDATE_MEMORY_ERR_FATAL

DESCRIPTION
  Validates an allocated block of memory from Modem Heap memory
  and calls ERR_FATAL if invalid.

DEPENDENCIES
  None

RETURNS
  The Data Pointer to the allocated Memory.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_validate_memory_err_fatal
(
  void       *data_ptr,
  uint32      data_size,
  const char *filename,
  uint32      line
)
{
  if (NULL == data_ptr)
  {
    ERR_FATAL("Failed to allocate heap memory block of size %d", data_size, 0, 0);
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated heap memory block 0x%x of size %d (%s, %d)",
                            data_ptr, data_size, filename, line);

    memset(data_ptr, 0x00, data_size);
  }

  return data_ptr;
} /* dsatutil_validate_memory_err_fatal */

/*===========================================================================
FUNCTION  DSATUTIL_FREE_MEMORY

DESCRIPTION
  Frees a block of memory from Modem Heap memory

DEPENDENCIES
  FEATURE_DATA_MODEM_HEAP

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_free_memory
(
  void *data_ptr
)
{
  DS_AT_MSG1_MED("modem_mem_free 0x%x:",(unsigned int)data_ptr);
  if (NULL != data_ptr)
  {
    dsat_modem_mem_free(data_ptr,MODEM_MEM_CLIENT_DATA);
    data_ptr = NULL;
  }
} /* dsatutil_free_memory() */

/*===========================================================================
FUNCTION  DSATUTIL_FREE_MEMORY_EXT

DESCRIPTION
  Frees a block of memory from Modem Heap memory and makes the ptr to NULL

DEPENDENCIES
  FEATURE_DATA_MODEM_HEAP

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_free_memory_ext
(
  void **data_ptr
)
{
  if (data_ptr != NULL && *data_ptr != NULL)
  {
    DS_AT_MSG1_MED("modem_mem_free 0x%x:",(unsigned int)*data_ptr);
    dsat_modem_mem_free(*data_ptr,MODEM_MEM_CLIENT_DATA);
    *data_ptr = NULL;
  }
} /* dsatutil_free_memory_ext() */

/*===========================================================================

FUNCTION DSATUTIL_GET_CMD_BUF

DESCRIPTION
  This function is a wrapper for ds_get_cmd_buf() API. It calls an error F3 in case of
  graceful handing and ERR_FATAL otherwise. The caller of this function must check
  for a NULL return value in case of graceful handing.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer if avaiable or NULL in case of graceful handing
  if no command buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

ds_command_type *dsatutil_get_cmd_buf
(
  size_t      payload_size, /* Number of bytes to allocate for cmd payload */
  const char *filename,
  uint32      line,
  boolean     graceful
)
{
  ds_command_type *cmd_buf = NULL;

  cmd_buf = ds_allocate_cmd_buf(payload_size);

  if ((payload_size > 0) && (NULL != cmd_buf) && (NULL == cmd_buf->cmd_payload_ptr))
  {
    ds_release_cmd_buf(&cmd_buf);
    cmd_buf = NULL;
  }

  if (NULL == cmd_buf)
  {
    if (TRUE == graceful)
    {
      DS_AT_MSG_SPRINTF_3_ERROR("Failed to get DS command buffer of payload size %d (%s, %d)",
                                payload_size, filename, line);
    }
    else
    {
      ERR_FATAL("Failed to get DS command buffer of payload size %d", payload_size, 0, 0);
    }
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated DS command buffer 0x%x of payload size %d (%s, %d)",
                            cmd_buf, payload_size, filename, line);

    if (NULL != cmd_buf->cmd_payload_ptr)
    {
      memset(cmd_buf->cmd_payload_ptr, 0x0, payload_size);
    }
  }
  
  return cmd_buf;
} /* dsatutil_get_cmd_buf() */

/*===========================================================================

FUNCTION DSATUTIL_GET_CMD_PAYLOAD_PTR

DESCRIPTION
  This function is used to get command payload pointer using command pointer.
  It calls ERR_FATAL when either command pointer or command payload pointer is NULL.

DEPENDENCIES
  None

RETURN VALUE
  Returns command payload pointer.

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_cmd_payload_ptr
(
  ds_command_type *cmd_ptr
)
{
  void *cmd_payload_ptr = NULL;

  if (NULL != cmd_ptr)
  {
    cmd_payload_ptr = cmd_ptr->cmd_payload_ptr;
  }

  if (NULL == cmd_payload_ptr)
  {
    ERR_FATAL("Invalid pointers: command = %p, payload = %p",
              cmd_ptr, cmd_payload_ptr, 0);
  }

  return cmd_payload_ptr;
} /* dsatutil_get_cmd_payload_ptr() */

/*===========================================================================
FUNCTION  DSATUTIL_MEMSCPY

DESCRIPTION
  
 This function will validate and copy data from src pointer to dest pointer.
  
DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

size_t dsatutil_memscpy
(
  void        *dst,
  size_t      dst_size,
  const void  *src,
  size_t      src_size

)
{
  if(dst_size == 0 || src_size == 0 || dst == NULL || src == NULL)
  {
    return 0;
  }
  else
  {
    return memscpy( dst,dst_size, src,src_size);
  }
} /* dsatutil_free_memory() */

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_VALS

DESCRIPTION
  This function will take basic table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_vals
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT_BASIC_N_C_IDX:
      resp =  &dsat_basic_vals.dsat_amp_c_val;
      break;
    case DSAT_BASIC_N_D_IDX:
      resp = &dsat_basic_vals.dsat_amp_d_val;
      break;
   case DSAT_BASIC_N_F_IDX:
     resp = &dsat_basic_vals.dsat_amp_f_val;
     break;
   case DSAT_BASIC_N_S_IDX:
     resp = &dsat_basic_vals.dsat_amp_s_val;
     break;
   case DSAT_BASIC_E_IDX:
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
     resp = &dsat_basic_vals.dsat_e_val_multi_port[0];
#else
     resp = &dsat_basic_vals.dsat_e_val;
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
     break;
   case DSAT_BASIC_L_IDX:
     resp = &dsat_basic_vals.dsat_l_val;
     break;
   case DSAT_BASIC_M_IDX:
     resp = &dsat_basic_vals.dsat_m_val;
     break;
   case DSAT_BASIC_Q_IDX :
     resp = &dsat_basic_vals.dsat_q_val;
     break;
   case DSAT_BASIC_V_IDX:
     resp = &dsat_basic_vals.dsat_v_val;
     break;
   case DSAT_BASIC_X_IDX:
     resp =  &dsat_basic_vals.dsat_x_val;
     break;
   case DSAT_BASIC_Z_IDX:
     resp = &dsat_basic_vals.dsat_z_val;
     break;
   case DSAT_BASIC_DS_V_IDX :
     resp = &dsat_basic_vals.dsat_slash_v_val;
     break;
   default:
    resp = NULL;
    break;
 }
 return resp;
}/*dsatutil_get_basic_vals*/
/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_ACTION_VALS

DESCRIPTION
  This function will take basic action table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_action_vals
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  if(cmd_id == DSAT_BASIC_ACT_O_IDX)
  {
    resp = &dsat_basic_act_vals.dsat_o_val;
  }
  return resp;
}/*dsatutil_get_basic_action_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_SREG_VALS

DESCRIPTION
  This function will take basic sreg table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_sreg_vals
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT_SREG_S0_IDX:
      resp =  &dsat_basic_sreg_vals.dsat_s0_val;
      break;
    case DSAT_SREG_S2_IDX:
      resp = &dsat_basic_sreg_vals.dsat_s2_val;
      break;
    case DSAT_SREG_S3_IDX:
      resp = &dsat_basic_sreg_vals.dsat_s3_val;
      break;
   case DSAT_SREG_S4_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s4_val;
     break;
   case DSAT_SREG_S5_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s5_val;
     break;
   case DSAT_SREG_S6_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s6_val;
     break;
   case DSAT_SREG_S7_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s7_val;
     break;
   case DSAT_SREG_S8_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s8_val;
     break;
   case DSAT_SREG_S10_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s10_val;
     break;
   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_basic_sreg_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_EXT_VALS

DESCRIPTION
  This function will take basic extended table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_ext_vals
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT_EXT_FCLASS_IDX:
      resp =  &dsat_basic_ext_vals.dsat_fclass_val;
      break;
    case DSAT_EXT_ICF_IDX:
      resp = &dsat_basic_ext_vals.dsat_icf_val[0];
      break;
    case DSAT_EXT_IFC_IDX:
      resp = &dsat_basic_ext_vals.dsat_ifc_val[0];
      break;
   case DSAT_EXT_IPR_IDX:
     resp = &dsat_basic_ext_vals.dsat_ipr_val;
     break;
   case DSAT_EXT_CMEE_IDX:
     resp = &dsat_basic_ext_vals.dsat_cmee_val;
     break;
#if defined(FEATURE_DSAT_ETSI_MODE)
   case DSAT_EXT_WS46_IDX:
     resp = &dsat_basic_ext_vals.dsat_ws46_val;
     break;
#endif /* defined(FEATURE_DSAT_ETSI_MODE)*/
   case DSAT_EXT_CFUN_IDX:
     resp = &dsat_basic_ext_vals.dsat_cfun_val[0];
     break;

   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_basic_ext_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_VEND_VALS

DESCRIPTION
  This function will take basic vender table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_vend_vals
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_ATCOP_VEND
    case DSAT_VENDOR_QCSIMSTAT_IDX:
      resp =  &dsat_basic_vend_vals.dsat_qcsimstat_val;
      break;
#endif
#endif /*FEATURE_DSAT_ETSI_MODE*/
#ifdef FEATURE_DSAT_LTE
#ifdef FEATURE_DSAT_ATCOP_VEND
   case DSAT_VENDOR_QCRATSTATE_IDX:
     resp = &dsat_basic_vend_vals.dsat_qcratstate_val;
     break;
   case DSAT_VENDOR_QCEXTCLTSTATE_IDX:
     resp = &dsat_basic_vend_vals.dsat_qcextcltstate_val;
     break;
#endif
#endif /* FEATURE_DSAT_LTE */
#ifdef FEATURE_DSAT_ATCOP_VEND
   case DSAT_VENDOR_QCPDPP_IDX:
      resp = &dsat_basic_vend_vals.dsat_qcpdpp_val[0];
      break;
   case DSAT_VENDOR_QCCELLSCAN_IDX:
     resp = &dsat_basic_vend_vals.dsat_qccellscan_mode_val;
     break;
   case DSAT_VENDOR_QCJDCFG:
     resp = &dsat_basic_vend_vals.dsat_qcjdcfg_val;
     break;   
  case DSAT_VENDOR_QCJDSTATE:
     resp = &dsat_basic_vend_vals.dsat_qcjdstate_val;    
     break;
   case DSAT_VENDOR_QCCEINFO_MODE:
      resp = &dsat_basic_vend_vals.dsat_qcceinfo_val;	
      break;
#ifdef FEATURE_FSK_TX
   case DSAT_VENDOR_QCFSKDBG_IDX:
      resp = &dsat_basic_vend_vals.dsat_qcfskdbg_val[0];	
      break;
   case DSAT_VENDOR_QCFSKDATA_IDX:
      resp = &dsat_basic_vend_vals.dsat_qcfskdata_val;	
      break;
   case DSAT_VENDOR_QCFSKSTART_IDX:
      resp = &dsat_basic_vend_vals.dsat_qcfskstart_val[0];	
      break;
   case DSAT_VENDOR_QCFSKSTOP_IDX:
      resp = &dsat_basic_vend_vals.dsat_qcfskstop_val;
      break;
#endif /* FEATURE_FSK_TX */
#endif
   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_basic_vend_vals*/


/*===========================================================================
FUNCTION  DSATUTIL_GET_ETSI_EXT_ACT_VALS

DESCRIPTION
  This function will take etsi ext action table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_etsi_ext_act_vals
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSATETSI_EXT_ACT_CIND_ETSI_IDX:
        resp = &dsat_etsi_ext_act_vals.dsat_cind_val[0];
      break;
#ifdef FEATURE_DATA_ETSI_PIN 
    case DSATETSI_EXT_ACT_CPIN_ETSI_IDX:
        resp =  &dsat_etsi_ext_act_vals.dsat_cpin_val[0];
      break;
#endif /*FEATURE_DATA_ETSI_PIN*/

    

   case DSATETSI_EXT_ACT_CMER_ETSI_IDX:
     resp = &dsat_etsi_ext_act_vals.dsat_cmer_val[0];
     break;
   
#ifdef FEATURE_DSAT_ETSI_DATA
   case DSATETSI_EXT_ACT_CGATT_ETSI_IDX:
        resp = &dsat_etsi_ext_act_vals.dsat_cgatt_val;
     break;
   case DSATETSI_EXT_ACT_CGACT_ETSI_IDX:
     resp = &dsat_etsi_ext_act_vals.dsat_cgact_val;
     break;
#endif /*#FEATURE_DSAT_ETSI_DATA*/

#ifdef FEATURE_ETSI_PBM
   case DSATETSI_EXT_ACT_CPBS_ETSI_IDX:
     resp = &dsat_etsi_ext_act_vals.dsat_cpbs_val;
     break;
#endif/* FEATURE_ETSI_PBM*/

#ifdef FEATURE_ETSI_SMS
   case DSATETSI_EXT_ACT_CPMS_ETSI_IDX:
       resp = &dsat_etsi_ext_act_vals.dsat_cpms_val[0];
      break;
     /* CNMI belongs into CTAB and ETSICTAB,Both have same variable and it is already allocated 
        in vender Structure */
   case DSATETSI_EXT_ACT_CNMI_ETSI_IDX:
       resp =  &dsat_basic_etsi_cmn_vals.dsat_cnmi_val[0];
     break;
   case DSATETSI_EXT_ACT_CMMS_ETSI_IDX:
       resp = &dsat_etsi_ext_act_vals.dsat_cmms_val;
     break;
#endif /* FEATURE_ETSI_SMS*/

   case DSATETSI_EXT_ACT_COPS_ETSI_IDX:
       resp = &dsat_etsi_ext_act_vals.dsat_cops_val[0];
     break;

   case DSATETSI_EXT_ACT_CUSD_ETSI_IDX:
     resp = &dsat_etsi_ext_act_vals.dsat_cusd_val[0];
     break;
   
#ifdef FEATURE_MMGSDI
   case DSATETSI_EXT_ACT_CPOL_ETSI_IDX:
       resp = &dsat_etsi_ext_act_vals.dsat_cpol_val[0];
     break;
#endif /* FEATURE_MMGSDI*/
  case DSATETSI_EXT_ACT_CPLS_ETSI_IDX:
       resp = &dsat_etsi_ext_act_vals.dsat_cpls_val;
     break;

  case DSATETSI_EXT_ACT_CTZR_ETSI_IDX:
     resp = &dsat_etsi_ext_act_vals.dsat_ctzr_val;
     break;

   case DSATETSI_EXT_ACT_CTZU_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_vals.dsat_ctzu_val;
     break;
   
   
   case DSATETSI_EXT_ACT_CSDF_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_vals.dsat_csdf_val;
     break;
 
   case DSATETSI_EXT_ACT_CPSMS_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_vals.dsat_cpsms_val[0];
     break;

#ifdef FEATURE_DSAT_EDRX
   case DSATETSI_EXT_ACT_CEDRXS_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_vals.dsat_cedrxs_val[0];
     break;
#endif /* FEATURE_DSAT_EDRX */
   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_etsi_ext_act_vals*/


/*===========================================================================
FUNCTION  DSATUTIL_GET_ETSI_EXT_VALS

DESCRIPTION
  This function will take etsi ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_etsi_ext_vals
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSATETSI_EXT_CREG_IDX:
        resp = &dsat_etsi_ext_vals.dsat_creg_val;
      break;
   case DSATETSI_EXT_CGREG_IDX:
       resp = &dsat_etsi_ext_vals.dsat_cgreg_val;
     break;
#ifdef FEATURE_DSAT_LTE
   case DSATETSI_EXT_CEREG_IDX:
       resp = &dsat_etsi_ext_vals.dsat_cereg_val;
     break;
#endif /*FEATURE_DSAT_LTE*/
   case DSATETSI_EXT_CSCS_IDX:
     resp = &dsat_etsi_ext_vals.dsat_cscs_val;
     break;

   case DSATETSI_EXT_CRC_IDX:
       resp = &dsat_etsi_ext_vals.dsat_crc_val;
       break;
#ifdef FEATURE_DSAT_ETSI_DATA
   case DSATETSI_EXT_CGDCONT_IDX:
     resp = &dsat_etsi_ext_vals.dsat_cgdcont_val[0];
     break;
    case  DSATETSI_EXT_CGEREP_IDX:
      resp = &dsat_etsi_ext_vals.dsat_cgerep_val[0];
      break;
    case  DSATETSI_EXT_CGPIAF_IDX:
      resp = &dsat_etsi_ext_vals.dsat_cgpiaf_val[0];
      break;
#endif /* FEATURE_DSAT_ETSI_DATA*/

#ifdef FEATURE_ETSI_SMS
#ifdef FEATURE_ETSI_SMS_PS
    case  DSATETSI_EXT_CGSMS_IDX:
        resp = &dsat_etsi_ext_vals.dsat_cgsms_val;
      break;
#endif  /*FEATURE_ETSI_SMS_PS*/
    case  DSATETSI_EXT_CSMS_IDX:
         resp = &dsat_etsi_ext_vals.dsat_csms_val;
       break;
    case  DSATETSI_EXT_CMGF_IDX:
       resp = &dsat_etsi_ext_vals.dsat_cmgf_val;
       break;
    case  DSATETSI_EXT_CSAS_IDX:
       resp = &dsat_etsi_ext_vals.dsat_csas_val;
       break;
    case  DSATETSI_EXT_CRES_IDX:
       resp = &dsat_etsi_ext_vals.dsat_cres_val;
       break;
    case  DSATETSI_EXT_CSCA_IDX:
         resp = &dsat_etsi_ext_vals.dsat_csca_val[0];
       break;
    case  DSATETSI_EXT_CSMP_IDX:
         resp = &dsat_etsi_ext_vals.dsat_csmp_val[0];
       break;
    case  DSATETSI_EXT_CSDH_IDX:
       resp = &dsat_etsi_ext_vals.dsat_csdh_val;
       break;
#endif /*FEATURE_ETSI_SMS*/
#ifdef FEATURE_DSAT_LTE
    case  DSATETSI_EXT_CEMODE_IDX:
       resp = &dsat_etsi_ext_vals.dsat_cemode_val;
       break;
    case  DSATETSI_EXT_CRTDCP_IDX:
       resp = &dsat_etsi_ext_vals.dsat_crtdcp_val;
       break;

#endif /*FEATURE_DSAT_LTE*/
    case  DSATETSI_EXT_CVHU_IDX:
     resp = &dsat_etsi_ext_vals.dsat_cvhu_val;
     break;
   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_etsi_ext_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_VAL_FROM_CMD_ID

DESCRIPTION
  This function will take valid command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_val_from_cmd_id
(
  dsat_num_item_type       cmd_id
)
{
  void *resp = NULL;

  if( DSAT_BASIC_N_C_IDX <= cmd_id && 
           cmd_id < DSAT_BASIC_MAX_IDX)
  {
    resp = dsatutil_get_basic_vals(cmd_id);
  }
  else if( DSAT_BASIC_ACT_D_IDX <= cmd_id && 
                cmd_id < DSAT_BASIC_ACT_MAX_IDX)
  {
    resp = dsatutil_get_basic_action_vals(cmd_id);
  }
  else if(  DSAT_SREG_S0_IDX <= cmd_id && 
                 cmd_id < DSAT_SREG_MAX_IDX )
  {
    resp = dsatutil_get_basic_sreg_vals(cmd_id);
  }
  else if( DSAT_EXT_FCLASS_IDX <= cmd_id && 
                cmd_id < DSAT_EXT_MAX_IDX)
  {
    resp = dsatutil_get_basic_ext_vals(cmd_id);
  }
  else if( DSAT_VENDOR_QCSIMSTAT_IDX <= cmd_id &&
                cmd_id < DSAT_VENDOR_MAX_IDX)
  {
    resp = dsatutil_get_basic_vend_vals(cmd_id);
  }
#ifdef FEATURE_DSAT_ETSI_MODE
  /* ETSI command tables*/
  else if( DSATETSI_EXT_ACT_CSQ_ETSI_IDX <= cmd_id && 
                    cmd_id < DSATETSI_EXT_ACT_MAX_ETSI_IDX)
  {
    resp = dsatutil_get_etsi_ext_act_vals(cmd_id);
  }
  else if( DSATETSI_EXT_CBST_IDX <= cmd_id && 
                    cmd_id < DSATETSI_EXT_MAX_IDX )
  {
    resp = dsatutil_get_etsi_ext_vals(cmd_id);
  }
#endif /* FEATURE_DSAT_ETSI_MODE */
  else
  {
    DS_AT_MSG1_ERROR("Table not found for %d index",cmd_id);
    DS_ASSERT(0);
  }
  return resp;
}/*dsatutil_get_val_from_cmd_id*/
/*===========================================================================
FUNCTION  DSATUTIL_DEFAULT_CONSTRUCTOR

DESCRIPTION
  This function will initialize the global variables.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_default_constructor(void)
{
  /* Initilize val pointers to default values */
#ifdef FEATURE_DSAT_ATCOP_VEND

  dsat_basic_vend_vals.dsat_qcpdpp_val[2].string_item = qcpdpp_passw_val;
  dsat_basic_vend_vals.dsat_qcpdpp_val[3].string_item = qcpdpp_uname_val;
#endif 
  /* Initilize val pointers to static memory */
  dsat_etsi_ext_act_vals.dsat_cusd_val[1].string_item = cusd_str_val;
  dsat_etsi_ext_act_vals.dsat_cusd_val[2].string_item = cusd_dcs_val;
  dsat_etsi_ext_act_vals.dsat_csdf_val = 1;
  dsat_etsi_ext_act_vals.dsat_cpsms_val[1].string_item =
                            cpsms_periodic_rau_str_val;
  dsat_etsi_ext_act_vals.dsat_cpsms_val[2].string_item = 
                            cpsms_gprs_ready_timer_str_val;
  dsat_etsi_ext_act_vals.dsat_cpsms_val[3].string_item = 
                            cpsms_periodic_tau_str_val;
  dsat_etsi_ext_act_vals.dsat_cpsms_val[4].string_item = 
                            cpsms_active_time_str_val;

  /* Initilize val pointers to static memory */
  dsat_etsi_ext_vals.dsat_cgdcont_val[1].string_item = cgdcont_pdptype_val;
  dsat_etsi_ext_vals.dsat_cgdcont_val[2].string_item = cgdcont_apn_val;
  dsat_etsi_ext_vals.dsat_cgdcont_val[3].string_item = cgdcont_pdpaddr_val;
   
  dsat_etsi_ext_vals.dsat_cscb_val[1].string_item= cscb_mids_val;
  dsat_etsi_ext_vals.dsat_cscb_val[2].string_item= cscb_dcss_val;

#ifdef FEATURE_DSAT_EDRX
  dsat_etsi_ext_act_vals.dsat_cedrxs_val[2].string_item = 
                            cedrxs_req_edrx_str_val;
#endif /* FEATURE_DSAT_EDRX */

  /* CM interface Variable */
  /* SS event and PH event */
  dsatcmif_initialize_to_default(DSAT_PH_VALS, &dsatcmif_ph_val);

  /* Call Info */
  dsatcmif_initialize_to_default(DSAT_CALL_VALS, &dsatcmif_call_val);

  /* SUPs Info */
  dsatcmif_initialize_to_default(DSAT_SUPS_VALS, &dsatcmif_sups_val);


  /* MMGSDI Info */
  dsatme_initialize_to_default(DSAT_MMGSDI_VALS, &dsatme_mmgsdi_val);

  /* Common info */
  dsatme_initialize_to_default(DSAT_COMMON_VALS, &dsat_common_val);

  dsat_etsi_ext_act_vals.dsat_cpin_val[0].string_item = cpin_pin_val;
  dsat_etsi_ext_act_vals.dsat_cpin_val[1].string_item = cpin_newpin_val;
  dsat_etsi_ext_act_vals.dsat_cops_val[2].string_item = cops_oper_val;
  dsat_etsi_ext_act_vals.dsat_cpol_val[2].string_item = cpol_oper_val;
  dsat_etsi_ext_vals.dsat_csca_val[0].string_item = dsat_csca_sca_val;
  dsat_etsi_ext_vals.dsat_csca_val[1].string_item = dsat_csca_tosca_val;
  dsat_etsi_ext_vals.dsat_csmp_val[0].string_item =  csmp_fo_val;
  dsat_etsi_ext_vals.dsat_csmp_val[1].string_item =  csmp_vp_val;


#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  dsatme_initialize_to_default(DSAT_PBM_VALS, &dsatme_pbm_val);
#endif /* (FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

  dsatme_initialize_to_default(DSAT_MD_VALS, &dsatetsipkt_gprs_val);
  dsatme_initialize_to_default(DSAT_PSM_URC_VALS, &dsat_psm_urc_param_val);

   return;
}

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory for mem_type
  which are subscription/slot/session independent.
  
DEPENDENCIES
  None

RETURNS


SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr
(
  dsat_mem_type           mem_type,
  boolean                 graceful
)
{
  void  *addr = NULL;

  DS_AT_MSG1_MED("mem_type:%d ", mem_type);
  
  switch(mem_type)
  {
    case DSAT_PH_VALS:
    {
        addr = &dsatcmif_ph_val;
    }
    break;

    case DSAT_CALL_VALS:
     {
       addr = &dsatcmif_call_val;
     }
    break;

    case DSAT_SUPS_VALS:
      {
        addr = &dsatcmif_sups_val;
      }
    break;
    
    case DSAT_MMGSDI_VALS:
      {
        addr = &dsatme_mmgsdi_val;
      }
    break;
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
    case DSAT_PBM_VALS:
    {
      addr = &dsatme_pbm_val;
    }
    break;
#endif /* (FEATURE_ETSI_PBM) || (FEATURE_DSAT_CDMA_PBM) */
    
	case DSAT_MD_VALS:
	{
	  addr = &dsatetsipkt_gprs_val;
	}
	break;
    
    case DSAT_COMMON_VALS:
      {
        addr = &dsat_common_val;
      }
    break;

    case DSAT_PSM_URC_VALS:
    {
      addr = &dsat_psm_urc_param_val;
    }
    break;

    default:
    break;
  }

  if (addr == NULL)
  {
    DS_AT_MSG1_ERROR("Invalid addr for mem %d", mem_type);

    if (FALSE == graceful)
    {
      DS_ASSERT(0);
    }
  }

  return addr;
} /*dsatutil_get_base_addr*/

/*===========================================================================

FUNCTION DSATUTIL_VALUE_GET

DESCRIPTION
  This FUNCTION copy value from source pointer into destination pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void * dsatutil_value_get
(
  const void                    *dest,                           /*  Destination pointer*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
)
{
  dsat_mixed_param_val_type * val_list;
  dsat_string_item_type     * val_string ;
  dsat_num_item_type *element;

  switch(value_info->val_type)
  {
    /* Single number Type*/
  case NUM_TYPE:
    element = (dsat_num_item_type *)dest;
    return (void *)(*(element + (value_info ->index)));
    
    /* Array Type or Struct Type */
  case STR_TYPE:
    return ((void *)dest);

    /* Array of string*/
  case ARRAY_OF_STR_TYPE :
    /* TO DO*/   
    break;
    
  case MIX_NUM_TYPE :
    val_list =  (dsat_mixed_param_val_type *) dest;
    return (void *)(val_list[value_info->index].num_item);

    /* Mixed string type*/
  case MIX_STR_TYPE :
    val_list =  (dsat_mixed_param_val_type *) dest;
    val_string = val_list[value_info->index].string_item;
    return ((void *)val_string);

  case MIX_INDEX_TYPE:
    val_list =  (dsat_mixed_param_val_type *) dest;
    return (void *)( val_list + value_info->index );

  default:
    break;
  }
  return NULL;
} /*dsatutil_value_get*/
/*===========================================================================

FUNCTION DSATUTIL_GET_PTR_TO_VALUE

DESCRIPTION
  This FUNCTION take entry index of parse table and extract base address of table .
  according base table it will  search entry in the table, if record found then return
  value_ptr otherwise return NULL.

DEPENDENCIES
  None.
RETURN VALUE
  Return value pointer . 

SIDE EFFECTS
 None.
===========================================================================*/
 void * dsatutil_get_ptr_to_value
(
  dsat_num_item_type             cmd_id,            /*  entry index in parse table  */
  dsati_value_info_struct_type  *value_info         /*  value information*/
)
{
  void * ret_val = NULL;
  ret_val = dsatutil_get_val_from_cmd_id(cmd_id);
  if(ret_val !=  NULL)
  {
    ret_val = dsatutil_value_get(ret_val ,value_info);
  }
  return ret_val;
} /*  dsatutil_get_ptr_to_value  */
/*===========================================================================

FUNCTION DSATUTIL_VALUE_COPY

DESCRIPTION
  This FUNCTION copy value from source pointer into destination pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsatutil_value_copy
(
  const void                    *dest,                           /*  Destination pointer*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
)
{
  dsat_mixed_param_val_type * val_list;
  dsat_string_item_type     * val_string ;
  switch(value_info->val_type)
  {
    /* Single number Type*/
    case NUM_TYPE:
      ((dsat_num_item_type *)dest)[value_info->index]=(dsat_num_item_type)value_info->val_ptr;
      break;
    /* Array Type or Struct Type */
    case STR_TYPE:
      (void) dsatutil_memscpy((void*)dest, value_info->size,(void*)value_info->val_ptr,value_info->size); 
      break;
    /* Array of string*/
    case ARRAY_OF_STR_TYPE :
       /*Currently leave it implement is later*/   
      break;
    /* Mixed number type*/
    case MIX_NUM_TYPE :
       val_list =  (dsat_mixed_param_val_type *) dest;
       val_list[value_info->index].num_item = (dsat_num_item_type)value_info->val_ptr;
       break;
    /* Mixed string type*/
    case MIX_STR_TYPE :
      val_list =  (dsat_mixed_param_val_type *) dest;
      val_string = val_list[value_info->index].string_item;
      (void) dsatutil_memscpy((void*)val_string, value_info->size,(void*)value_info->val_ptr,value_info->size);
      break;
    default:
      DS_AT_MSG0_HIGH("Default case");
      break;
   }
} /*dsatutil_value_copy*/
/*===========================================================================

FUNCTION DSATUTIL_SET_VALUE

DESCRIPTION
  This FUNCTION take entry index of parse table and extract base address of table .
  according base table it will  search entry in  table, if record found then typecast
  value_ptr into value_type and set it into table value pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsatutil_set_value
(
  dsat_num_item_type             cmd_id,           /*  entry index in parse table  */
  dsati_value_info_struct_type  *value_info        /*  value information*/
)
{
  void * ret_val = NULL;

  ret_val = dsatutil_get_val_from_cmd_id(cmd_id);
  if(ret_val !=  NULL)
  {
    dsatutil_value_copy(ret_val ,value_info);
    return ;
  }
} /*  dsatutil_set_value  */
 /*===========================================================================
 
 FUNCTION dsatutil_get_val
 
 DESCRIPTION
   This FUNCTION take command index and return corresponding value stored in table.  
 
 DEPENDENCIES
   None.
 RETURN VALUE
   None
 
 SIDE EFFECTS
   None.
 ===========================================================================*/

 void * dsatutil_get_val
(
  dsat_num_item_type       cmd_id,            /*  entry index in parse table  */
  dsat_num_item_type       index,
  value_type_enum_type     val_type
)
{
  dsati_value_info_struct_type val_info;
  void                        *return_val = NULL;
  memset((void *)&val_info,0,sizeof(val_info));
  val_info.index = index;
  val_info.val_type = val_type;
  return_val = ((void *)dsatutil_get_ptr_to_value( cmd_id, &val_info));
  if((val_type == STR_TYPE) || 
     (val_type == MIX_STR_TYPE) || 
     (val_type == MIX_INDEX_TYPE) )
  {
    if(return_val == NULL)
    {
       DS_ERR_FATAL("String Type return value NULL");
    }
  }
  return return_val;
}/* dsatutil_get_val*/

 /*===========================================================================
  
  FUNCTION dsatutil_get_val_ext
  
  DESCRIPTION
    This FUNCTION take command index and return corresponding value stored in table.  
  
  DEPENDENCIES
    None.
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None.
  ===========================================================================*/
 
  void * dsatutil_get_val_ext
 (
   dsat_num_item_type       cmd_id,            /*  entry index in parse table  */
   dsat_num_item_type       index,
   value_type_enum_type     val_type
 )
 {
   dsati_value_info_struct_type val_info;
   void                        *return_val = NULL;
   memset((void *)&val_info,0,sizeof(val_info));
   val_info.index = index;
   val_info.val_type = val_type;
   return_val = ((void *)dsatutil_get_ptr_to_value( cmd_id, &val_info));
   if((val_type == STR_TYPE) || 
      (val_type == MIX_STR_TYPE) || 
      (val_type == MIX_INDEX_TYPE) )
   {
     if(return_val == NULL)
     {
        DS_AT_MSG0_HIGH("String Type return value NULL");
     }
   }
   return return_val;
 }/* dsatutil_get_val_ext*/

 /*===========================================================================
 FUNCTION	   dsat_get_cbst_export_val
 
 DESCRIPTION   This function returns pointer to a prticular index in array 
			   dsat_cbst_export_val. 
 
 PARAMETERS    int index - index of the array
 
 DEPENDENCIES  None
 
 RETURN VALUE  dsat_num_item_type*
 
 SIDE EFFECTS  None
 ===========================================================================*/
 dsat_num_item_type* dsat_get_cbst_export_val(int index)
 {
   return &dsat_cbst_export_val[index];
 }

#ifdef FEATURE_WCDMA
 /*==============================================================================
 
 FUNCTION dsat_get_es_export_val
 
 DESCRIPTION This function returns the dsat_es_export_val global variable .  
 
 PARAMETERS index of array
 
 DEPENDENCIES None
 
 RETURN VALUE global variable
 
 SIDE EFFECTS None
 ===========================================================================*/
 dsat_num_item_type dsat_get_es_export_val(int index)
 {
   return dsat_es_export_val[index];
 }
 
  
 /*==============================================================================
 
 FUNCTION dsat_get_esa_export_val
 
 DESCRIPTION This function returns the dsat_esa_export_val global variable .  
 
 PARAMETERS index of array
 
 DEPENDENCIES None
 
 RETURN VALUE global variable
 
 SIDE EFFECTS None
 ===========================================================================*/
 dsat_num_item_type dsat_get_esa_export_val(int index)
 {
   return dsat_esa_export_val[index];
 }
#endif  /* FEATURE_WCDMA */

 /*===========================================================================
 
 FUNCTION DSAT_SET_VAL_INFO
 
 DESCRIPTION
       Set values into structure .  
 
 DEPENDENCIES
   None.
 RETURN VALUE
   None
 
 SIDE EFFECTS
   None.
 ===========================================================================*/

 void dsat_set_val_info
 (           
   dsat_num_item_type             index,
   dsat_num_item_type             size,
   void *                         val_ptr,
   value_type_enum_type           val_type,
   dsati_value_info_struct_type   *val_info
 )
 {
   val_info->index = index;
   val_info->size = size;
   val_info->val_ptr = val_ptr;
   val_info->val_type = val_type;

 } /*dsat_set_val_info*/
/*Other get set APIs for globals*/

boolean dsat_mo_dailing_state_get()
{
  return dsat_mo_dailing_state;
}

void dsat_mo_dailing_state_set
(
  boolean        value
)
{
  dsat_mo_dailing_state = value;
}

void dsat_value_set
(
  dsat_num_item_type cmd_id,
  dsat_num_item_type index,
  dsat_num_item_type size,
  dsat_num_item_type value_num,
  value_type_enum_type value_type
)
{
  dsati_value_info_struct_type  val_info; 
  memset((void *)&val_info,0,sizeof(val_info)); 
  dsat_set_val_info( index, size,(void *)value_num, value_type,&val_info);
  dsatutil_set_value( cmd_id,&val_info);
}
/*===========================================================================
 FUNCTION DSAT_IS_PENDING_CMD_STATE
 
 DESCRIPTION
   This function checks the pending state of a command being passed
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_is_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
)
{
  if(dsat_pending_state[PORT_ZERO].cmd_idx == cmd_id && 
     dsat_pending_state[PORT_ZERO].cmd_pending[apps_id] == pending) 
  {
    return TRUE;
  }
  return FALSE;
}/*dsat_is_pending_cmd_state*/

/*===========================================================================
 FUNCTION DSAT_IS_NOT_PENDING_CMD_STATE
 
 DESCRIPTION
   This function checks the pending state of a command being passed
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_is_not_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
)
{
  if(dsat_pending_state[PORT_ZERO].cmd_idx == cmd_id && 
     dsat_pending_state[PORT_ZERO].cmd_pending[apps_id] != pending) 
  {
    return TRUE;
  }
  return FALSE;
}/*dsat_is_not_pending_cmd_state*/

/*===========================================================================
 FUNCTION DSAT_SET_PENDING_CMD_STATE
 
 DESCRIPTION
   This function sets the pending state of a command being passed.
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
void dsat_set_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
)
{
  dsat_pending_state[PORT_ZERO].cmd_idx = cmd_id;
  dsat_pending_state[PORT_ZERO].cmd_pending[apps_id] = pending;
}/*dsat_set_pending_cmd_state*/

/*===========================================================================
 FUNCTION DSAT_BIN_CONVERSION
 
 DESCRIPTION
  This function converts a decimal number into its binary form.
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   None.
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
 void dsat_bin_conversion(uint32 num, byte* binaryForm, uint8 binaryForm_size, uint8 siz)
 {
   if(siz < binaryForm_size)
   {
     int cnt = siz - 1;
     while( num > 0 && cnt >= 0 )
     {
       binaryForm[cnt] = num%2 + 0x30;
       num = num/2;
       cnt--;
     }
     while(cnt >= 0)
     {
       binaryForm[cnt] = '0';
       cnt--;
     }
     binaryForm[siz] = '\0';
   }
 }

 /*===========================================================================
 FUNCTION DSAT_DETERMINE_RAT
 
 DESCRIPTION
  This function determines the rat type.
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   None.
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
 void dsat_determine_rat(dsat_edrx_config_s_type *edrx, uint8 rat, dsat_edrx_params_s_type *rat_edrx)
 {
   if(rat_edrx != NULL && edrx != NULL)
   {
     switch(rat)
     {
       case DSAT_EDRX_RAT_GSM_RADIO_ACCESS:
         *rat_edrx = edrx->gsm;
       break;
       case DSAT_EDRX_RAT_UMTS_RADIO_ACCESS:
         *rat_edrx = edrx->umts;
       break;
       case DSAT_EDRX_RAT_LTE_RADIO_ACCESS:
         if(edrx->M1.enabled)
           *rat_edrx = edrx->M1;
         else
           *rat_edrx = edrx->lte;
       break;
       case DSAT_EDRX_RAT_LTE_NB1_RADIO_ACCESS:
         *rat_edrx = edrx->NB1;
       break;
       default:
       break;
     }
   }
 }

/*===========================================================================
FUNCTION  DSAT_STORE_ATCOP_PSM_URC_INFO

DESCRIPTION
  This function fills the buffer with ATCOP URC related parameter info. Memory
  is allocated by ATCOP and caller should free the memory after using.

DEPENDENCIES
  None

RETURNS
  Returns pointer to the formatted ATCOP URC buffer.
  Returns an enum that describes the result of the command execution.
  Possible values:
    FALSE : if there was any problem in executing the command
    TRUE : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/

boolean dsat_store_atcop_psm_urc_info
(
  void   **buf_ptr,
  uint32  *buf_len
)
{
  char                          *temp_buf_ptr = NULL;
  uint32                         temp_len = 0;
  uint8                          index;
  dsat_psm_atcop_urc_state_info  *psm_urc_ptr = &dsat_psm_urc_param_val;

  /*-----------------------------------------------------------------------*/

  dsat_psm_urc_update_bit_mask(DSAT_PSM_URC_CRC_MODE, 
                               DSATETSI_EXT_CRC_IDX, 
                               0);
  dsat_psm_urc_update_bit_mask(DSAT_PSM_URC_CTZR_REPORTING, 
                               DSATETSI_EXT_ACT_CTZR_ETSI_IDX, 
                               0);

  if( psm_urc_ptr->bit_mask == 0 ||
      buf_len == NULL ||
      buf_ptr == NULL)
  {
    return FALSE;
  }
  
  DS_SYSTEM_HEAP_MEM_ALLOC(*buf_ptr, sizeof(dsat_psm_atcop_urc_state_info) ,char*);
  if(*buf_ptr == NULL)
  {
    ULOG_RT_PRINTF_0(ds_pdn_psm_get_ulog_handle(),
                     "dsat_store_atcop_psm_urc_info Memory allocation failed");
    return FALSE;
  }

  temp_buf_ptr = (char *)(*buf_ptr);
  temp_len = memscpy(temp_buf_ptr, 
                     sizeof(uint32), 
                     &(psm_urc_ptr->bit_mask), 
                     sizeof(uint32));

  temp_buf_ptr += temp_len;
  *buf_len += temp_len;

  DS_AT_MSG1_HIGH("Store bit_mask = %d", psm_urc_ptr->bit_mask);
  ULOG_RT_PRINTF_1(ds_pdn_psm_get_ulog_handle(),
                 "Store bit_mask = %d",psm_urc_ptr->bit_mask);

  for(index = 0; index < DSAT_PSM_URC_MAX; index++ )
  {
    if(psm_urc_ptr->bit_mask & DSAT_BM(index))
    {
      temp_len = memscpy(temp_buf_ptr, 
                         sizeof(uint8), 
                         &(psm_urc_ptr->psm_urc_params[index]), 
                         sizeof(uint8));
      temp_buf_ptr += temp_len;
      *buf_len += temp_len;
    }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION  DSAT_RESTORE_ATCOP_PSM_URC_INFO

DESCRIPTION
  This function restores the ATCOP URC PSM buffer. Memory
  is allocated by mode handler and ATCOP should free the memory after using.
  ATCOP needs to post the command to the DS task.

DEPENDENCIES
  None

RETURNS
  NONE

SIDE EFFECTS
  None

===========================================================================*/

void dsat_restore_atcop_psm_urc_info
(
  void    *buf_ptr,
  uint32   buf_len
)
{
  ds_command_type              *cmd_buf  = NULL;
  uint32                        bit_mask = 0;
  uint8                         set_bit_cnt = 0;

  /* buf_len should be at least the size of bit_mask*/
  if(buf_ptr == NULL ||
     buf_len < sizeof(uint32))
  {
    return;
  }

  /* Copy bit_mask*/
  memscpy(&bit_mask,
           sizeof(uint32),
           buf_ptr,
           sizeof(uint32));

  DS_AT_MSG2_HIGH("restore_atcop_psm_urc_info buf_len = %d, bit_mask = %d", buf_len, bit_mask);
  ULOG_RT_PRINTF_2(ds_pdn_psm_get_ulog_handle(),
                 "restore_atcop_psm_urc_info buf_len = %d, bit_mask = %d", buf_len, bit_mask);

  /* Count set bits*/
  while (bit_mask) 
  { 
    bit_mask &= (bit_mask - 1); 
    set_bit_cnt++; 
  } 

  /* buf_len excluding size of bit_mask must match with number of URCs registered.*/
  if((buf_len - sizeof(uint32)) != set_bit_cnt)
  {
    return;
  }

  cmd_buf = dsat_get_cmd_buf(buf_len, FALSE);
  if(cmd_buf == NULL)
  {
    ULOG_RT_PRINTF_0(ds_pdn_psm_get_ulog_handle(),
                 "dsat_restore_atcop_psm_urc_info dsat_get_cmd_buf failed");
    return;
  }
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_RESTORE_URC_INFO_IN_PSM_CMD;
  memscpy(cmd_buf->cmd_payload_ptr,
                   buf_len,
                   buf_ptr,
                   buf_len);

  ds_put_cmd(cmd_buf);
  DS_SYSTEM_HEAP_MEM_FREE(buf_ptr);
  return;
}

/*===========================================================================
FUNCTION DSAT_PSM_URC_UPDATE_BIT_MASK

DESCRIPTION
  This function will set or unset the bit mask for registration of URCs.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/

void dsat_psm_urc_update_bit_mask
(
  uint32                   psm_urc_idx,
  dsat_num_item_type       cmd_id,
  dsat_num_item_type       param_index
)
{
  dsat_psm_atcop_urc_state_info  *psm_urc_val = NULL;
  psm_urc_val = dsat_get_base_addr(DSAT_PSM_URC_VALS, FALSE);

  if(psm_urc_val == NULL || psm_urc_idx >= DSAT_PSM_URC_MAX)
  {
    return;
  }

  psm_urc_val->psm_urc_params[psm_urc_idx] = (dsat_num_item_type)dsatutil_get_val(
                                                             cmd_id,param_index,NUM_TYPE);

  DS_AT_MSG3_HIGH("update_bit_mask urc_idx = %d, cmd_id = %d, index = %d", psm_urc_idx, cmd_id, param_index);
  if( 0 < psm_urc_val->psm_urc_params[psm_urc_idx])
  {
    psm_urc_val->bit_mask |= DSAT_BM(psm_urc_idx);
  }
  else
  {
    psm_urc_val->bit_mask &= ~(DSAT_BM(psm_urc_idx));
  }
  return;
}

//#endif

