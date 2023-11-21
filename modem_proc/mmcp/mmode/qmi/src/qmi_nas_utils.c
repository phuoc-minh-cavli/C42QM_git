/*===========================================================================

                        D S _ Q M I _ N A S _ U T I L S . C 

DESCRIPTION

 The Qualcomm Network Access Services MSM Interface utility source file.

EXTERNALIZED FUNCTIONS
     
  qmi_nas_binary_to_ascii()
    Convert integer value to ASCII string

Copyright (c) 2010-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_nas_utils.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/04/10    hs     Initial version
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_SERVICE_SPLIT

#include "sys.h"
#include "qmi_nas_msg.h"
#include "cm.h"
#include "qmi_nas.h"
#include "qmi_nas_internal.h"


/*===========================================================================
  FUNCTION QMI_NAS_BINARY_TO_ASCII()

  DESCRIPTION
    This function is used to convert a binary value to an ASCII string 
    
  PARAMETERS
    value        : binary value to convert
    buffer       : buffer to put value in
    buf_len      : length of buffer

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_nas_binary_to_ascii(uint32 value, char* buffer, uint16 buf_len)
{
  int count;

  for(count = buf_len - 1; count >= 0; count--)
  {
    buffer[count] = (char)(value%10 + '0');
    value /= 10;
  }
}

/*===========================================================================
  FUNCTION QMI_NAS_ASCII_TO_BINARY()

  DESCRIPTION
    This function is used to convert an ASCII string to binary values
    
  PARAMETERS
    value        : binary value to convert
    buffer       : buffer to put value in
    buf_len      : length of buffer

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

uint32 qmi_nas_ascii_to_binary(uint8* buffer, uint16 buf_len)
{
  int count;
  uint32 temp = 0;

  for(count = 0; count < buf_len; count++)
  {
    temp *= 10;
    temp += buffer[count] - '0';
  }

  return temp;
}

/*===========================================================================
  FUNCTION QMI_NAS_STRING_IS_NUMERIC()

  DESCRIPTION
    This function is used to verify a character string is composed entirely
    of ASCII characters
    
  PARAMETERS
    value        : binary value to convert
    buffer       : buffer to put value in
    buf_len      : length of buffer

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_string_is_numeric(uint8* buffer, uint16 buf_len)
{
  int count;

  for(count = 0; count < buf_len; count++)
  {
    if(buffer[count] < '0' || buffer[count] > '9')
    {
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
  FUNCTION qmi_nas_byte_array_compare()

  DESCRIPTION
    comparison function for qsort() used for char array.
    
  PARAMETERS
    check stdlib.h

  RETURN VALUE
    -1 if *p_a < *p_b, 0 if *p_a == *p_b, 1 if *p_a > *p_b
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_nas_byte_array_compare( const void *p_a, const void *p_b )
{
  return *(char*)p_a < *(char*)p_b ? -1 : *(char*)p_a == *(char*)p_b ? 0 : 1 ;
}

/*===========================================================================
  FUNCTION qmi_nas_int16_array_compare()

  DESCRIPTION
    comparison function for qsort() used for (signed) int16 array.
    
  PARAMETERS
    check stdlib.h

  RETURN VALUE
    -1 if *p_a < *p_b, 0 if *p_a == *p_b, 1 if *p_a > *p_b
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_nas_int16_array_compare( const void *p_a, const void *p_b )
{
  return *(int16*)p_a < *(int16*)p_b ? -1 : *(int16*)p_a == *(int16*)p_b ? 0 : 1 ;
}


/*===========================================================================
  FUNCTION qmi_nas_sys_mode_2_radio_if()

  DESCRIPTION
    convert sys_mode enum to radio_if enum
    
  PARAMETERS
    cm_mm_msim_ss_info_s_type, sys_sys_mode_e_type, if the stack needs to be in service

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_sys_mode_2_radio_if( cm_mm_ss_info_s_type *p_info, boolean needs_to_be_in_service )
{
  enum qmi_nas_radio_if_e status = QMI_NAS_RADIO_IF_NONE;

  if ( needs_to_be_in_service && p_info->srv_status != SYS_SRV_STATUS_SRV )
      {
        status = QMI_NAS_RADIO_IF_NONE;
      }
      else
      {
    switch ( p_info->sys_mode )
        {
          #if defined(FEATURE_GSM) 
          case SYS_SYS_MODE_GSM:   status = QMI_NAS_RADIO_IF_GSM;            break;
          #endif

          #if defined(FEATURE_WCDMA)
          case SYS_SYS_MODE_WCDMA: status = QMI_NAS_RADIO_IF_UMTS;           break;
          #endif

          #if defined(FEATURE_LTE)
          case SYS_SYS_MODE_LTE:   status = QMI_NAS_RADIO_IF_LTE;            break;
          #endif

          #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
          case SYS_SYS_MODE_CDMA:  status = QMI_NAS_RADIO_IF_CDMA20001X;     break;
          #endif

          #if defined(FEATURE_HDR)
          case SYS_SYS_MODE_HDR:   status = QMI_NAS_RADIO_IF_CDMA20001XEVDO; break;
          #endif
          
          #if defined(FEATURE_TDSCDMA)
          case SYS_SYS_MODE_TDS:   status = QMI_NAS_RADIO_IF_TDSCDMA;        break;
          #endif
          
          default:                 break;
            // not thinking of any other case than listed above
        }
      }
  return status;

}


/*===========================================================================
  FUNCTION qmi_nas_sys_mode_2_radio_if_ciot()

  DESCRIPTION
    convert sys_radio_access_tech enum to radio if enum and ciot op mode if RAT is LTE
    
  PARAMETERS
    sys_radio_access to translate

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_sys_mode_2_radio_if_ciot
(
  sys_sys_mode_e_type sys_radio_access,

  nas_camped_ciot_lte_op_mode_enum_type_v01 *ciot_lte_op_mode
)
{
  enum qmi_nas_radio_if_e radio_if = QMI_NAS_RADIO_IF_NONE;

  switch( sys_radio_access )
  {
    #if defined(FEATURE_GSM) 
    case SYS_SYS_MODE_GSM:  radio_if = QMI_NAS_RADIO_IF_GSM;     break;
    #endif

    #if defined(FEATURE_WCDMA)
    case SYS_SYS_MODE_WCDMA: radio_if = QMI_NAS_RADIO_IF_UMTS;    break;
    #endif

    #if defined(FEATURE_LTE)
    case SYS_SYS_MODE_LTE:  
      radio_if = QMI_NAS_RADIO_IF_LTE;
      *ciot_lte_op_mode = NAS_CIOT_SYS_MODE_LTE_WB_V01;
      break;
    case SYS_SYS_MODE_LTE_M1:
      radio_if = QMI_NAS_RADIO_IF_LTE;
      *ciot_lte_op_mode = NAS_CIOT_SYS_MODE_LTE_M1_V01;
      break;
    case SYS_SYS_MODE_LTE_NB1:
      radio_if = QMI_NAS_RADIO_IF_LTE;
      *ciot_lte_op_mode = NAS_CIOT_SYS_MODE_LTE_NB1_V01;
      break;
    #endif

    #if defined(FEATURE_TDSCDMA)
    case SYS_SYS_MODE_TDS:  radio_if = QMI_NAS_RADIO_IF_TDSCDMA; break;
    #endif

    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
    case SYS_SYS_MODE_CDMA:  radio_if = QMI_NAS_RADIO_IF_CDMA20001X; break;
    #endif

    #if defined(FEATURE_HDR)
    case SYS_SYS_MODE_HDR:  radio_if = QMI_NAS_RADIO_IF_CDMA20001XEVDO; break;
    #endif

    case SYS_SYS_MODE_NO_SRV:  radio_if = QMI_NAS_RADIO_IF_NONE; break;

    default:
      QM_MSG_ERROR_1("unsupported sys_radio_access %d", sys_radio_access);
      break; 
  }

  return radio_if;
}


/*===========================================================================
  FUNCTION qmi_nas_radio_if_2_sys_radio_access()

  DESCRIPTION
    convert radio_if enum to sys_radio_access_tech enum
    
  PARAMETERS
    radio_if to translate

  RETURN VALUE
    sys_radio_access_tech_e_type
===========================================================================*/
sys_radio_access_tech_e_type qmi_nas_radio_if_2_sys_radio_access( nas_radio_if_enum_v01 radio_if )
{
  sys_radio_access_tech_e_type radio_access_tech = SYS_RAT_NONE;

  switch( radio_if )
  {
    #if defined(FEATURE_GSM) 
    case NAS_RADIO_IF_GSM_V01:     radio_access_tech = SYS_RAT_GSM_RADIO_ACCESS;  break;
    #endif

    #if defined(FEATURE_WCDMA)
    case NAS_RADIO_IF_UMTS_V01:    radio_access_tech = SYS_RAT_UMTS_RADIO_ACCESS; break;
    #endif

    #if defined(FEATURE_LTE)
    case NAS_RADIO_IF_LTE_V01:     radio_access_tech = SYS_RAT_LTE_RADIO_ACCESS;  break;
    #endif

    #if defined(FEATURE_TDSCDMA)
    case NAS_RADIO_IF_TDSCDMA_V01: radio_access_tech = SYS_RAT_TDS_RADIO_ACCESS;  break;
    #endif

	default:
      QM_MSG_ERROR_1("unsupported radio if %d", radio_if);
      break; 
  }

  return radio_access_tech;
}

/*===========================================================================
  FUNCTION qmi_nas_radio_if_2_sys_radio_access_ciot()

  DESCRIPTION
    convert radio_if enum to sys_radio_access_tech enum
    
  PARAMETERS
    radio_if to translate

  RETURN VALUE
    sys_radio_access_tech_e_type
===========================================================================*/
sys_radio_access_tech_e_type qmi_nas_radio_if_2_sys_radio_access_ciot
(
  nas_radio_if_enum_v01 radio_if,

  nas_camped_ciot_lte_op_mode_enum_type_v01 ciot_lte_op_mode
)
{
  sys_radio_access_tech_e_type radio_access_tech = SYS_RAT_NONE;

  switch( radio_if )
  {
    #if defined(FEATURE_GSM) 
    case NAS_RADIO_IF_GSM_V01:     radio_access_tech = SYS_RAT_GSM_RADIO_ACCESS;  break;
    #endif

    #if defined(FEATURE_WCDMA)
    case NAS_RADIO_IF_UMTS_V01:    radio_access_tech = SYS_RAT_UMTS_RADIO_ACCESS; break;
    #endif

    #if defined(FEATURE_LTE)
    case NAS_RADIO_IF_LTE_V01:

    #ifdef FEATURE_MMODE_M1
      if(ciot_lte_op_mode == NAS_CIOT_SYS_MODE_LTE_M1_V01)
      {
        radio_access_tech = SYS_RAT_LTE_M1_RADIO_ACCESS;  
      }
      else 
    #endif
	  if(ciot_lte_op_mode == NAS_CIOT_SYS_MODE_LTE_NB1_V01)
      {
        radio_access_tech = SYS_RAT_LTE_NB1_RADIO_ACCESS;  
      }
      else
      {
        radio_access_tech = SYS_RAT_LTE_RADIO_ACCESS;  
      }
      break;
    #endif

    #if defined(FEATURE_TDSCDMA)
    case NAS_RADIO_IF_TDSCDMA_V01: radio_access_tech = SYS_RAT_TDS_RADIO_ACCESS;  break;
    #endif

	default:
      QM_MSG_ERROR_1("unsupported radio if %d", radio_if);
      break; 
  }

  return radio_access_tech;
}

/*===========================================================================
  FUNCTION qmi_nas_sys_radio_access_2_radio_if()

  DESCRIPTION
    convert sys_radio_access_tech enum to radio if enum
    
  PARAMETERS
    sys_radio_access to translate

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_sys_radio_access_2_radio_if( sys_radio_access_tech_e_type sys_radio_access )
{
  enum qmi_nas_radio_if_e radio_if = QMI_NAS_RADIO_IF_NONE;

  switch( sys_radio_access )
  {
    #if defined(FEATURE_GSM) 
    case SYS_RAT_GSM_RADIO_ACCESS:  radio_if = QMI_NAS_RADIO_IF_GSM;     break;
    #endif

    #if defined(FEATURE_WCDMA)
    case SYS_RAT_UMTS_RADIO_ACCESS: radio_if = QMI_NAS_RADIO_IF_UMTS;    break;
    #endif

    #if defined(FEATURE_LTE)
    case SYS_RAT_LTE_RADIO_ACCESS:  radio_if = QMI_NAS_RADIO_IF_LTE;     break;
	#ifdef FEATURE_MMODE_M1
    case SYS_RAT_LTE_M1_RADIO_ACCESS:  radio_if = QMI_NAS_RADIO_IF_LTE_M1; break;
	#endif
    case SYS_RAT_LTE_NB1_RADIO_ACCESS:  radio_if = QMI_NAS_RADIO_IF_LTE_NB1; break;
    #endif

    #if defined(FEATURE_TDSCDMA)
    case SYS_RAT_TDS_RADIO_ACCESS:  radio_if = QMI_NAS_RADIO_IF_TDSCDMA; break;
    #endif

    default:
      QM_MSG_ERROR_1("unsupported sys_radio_access %d", sys_radio_access);
      break; 
  }

  return radio_if;
}

/*===========================================================================
  FUNCTION qmi_nas_sys_radio_access_2_radio_if()

  DESCRIPTION
    convert sys_radio_access_tech enum to radio if enum
    
  PARAMETERS
    sys_radio_access to translate

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_sys_radio_access_2_radio_if_ciot
( 
  sys_radio_access_tech_e_type sys_radio_access,

  nas_camped_ciot_lte_op_mode_enum_type_v01 *ciot_lte_op_mode
)
{
  enum qmi_nas_radio_if_e radio_if = QMI_NAS_RADIO_IF_NONE;

  switch( sys_radio_access )
  {
    #if defined(FEATURE_GSM) 
    case SYS_RAT_GSM_RADIO_ACCESS:  radio_if = QMI_NAS_RADIO_IF_GSM;     break;
    #endif

    #if defined(FEATURE_WCDMA)
    case SYS_RAT_UMTS_RADIO_ACCESS: radio_if = QMI_NAS_RADIO_IF_UMTS;    break;
    #endif

    #if defined(FEATURE_LTE)
    case SYS_RAT_LTE_RADIO_ACCESS:  
      radio_if = QMI_NAS_RADIO_IF_LTE;
      *ciot_lte_op_mode = NAS_CIOT_SYS_MODE_LTE_WB_V01;
      break;
	#ifdef FEATURE_MMODE_M1
    case SYS_RAT_LTE_M1_RADIO_ACCESS:
      radio_if = QMI_NAS_RADIO_IF_LTE;
      *ciot_lte_op_mode = NAS_CIOT_SYS_MODE_LTE_M1_V01;
      break;
	#endif
    case SYS_RAT_LTE_NB1_RADIO_ACCESS:
      radio_if = QMI_NAS_RADIO_IF_LTE;
      *ciot_lte_op_mode = NAS_CIOT_SYS_MODE_LTE_NB1_V01;
      break;
    #endif

    #if defined(FEATURE_TDSCDMA)
    case SYS_RAT_TDS_RADIO_ACCESS:  radio_if = QMI_NAS_RADIO_IF_TDSCDMA; break;
    #endif

    default:
      QM_MSG_ERROR_1("unsupported sys_radio_access %d", sys_radio_access);
      break; 
  }

  return radio_if;
}

/*===========================================================================
  FUNCTION qmi_nas_get_supported_technology_from_nv_pref_mode()

  DESCRIPTION
    split up nv pref_mode into access technologies
    
  PARAMETERS
    nv_pref_mode, access_tech_s container to hold split information

  RETURN VALUE
    TRUE if something understood, FALSE otherwise
===========================================================================*/
boolean qmi_nas_get_supported_technology_from_nv_pref_mode( nv_mode_enum_type pref_mode, struct access_tech_s *p )
{
  boolean ret_val = TRUE;

  switch ( pref_mode )
  {
    #if defined(FEATURE_MMODE_QMI_LTE_ONLY)
    case NV_MODE_LTE_ONLY:                p->c = 0; p->h = 0; p->g = 0; p->w = 0; p->l = 1; p->t = 0; break;
    #elif defined(FEATURE_MMODE_QMI_GPRS_LTE)
    case NV_MODE_LTE_ONLY:                p->c = 0; p->h = 0; p->g = 0; p->w = 0; p->l = 1; p->t = 0; break;
    case NV_MODE_GSM_ONLY:                p->c = 0; p->h = 0; p->g = 1; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_GSM_LTE_ONLY:            p->c = 0; p->h = 0; p->g = 1; p->w = 0; p->l = 1; p->t = 0; break;
    #else
    case NV_MODE_DIGITAL_ONLY:            p->c = 1; p->h = 1; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_AUTOMATIC:               p->c = 1; p->h = 1; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_CDMA_ONLY:               p->c = 1; p->h = 0; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_HDR_ONLY:                p->c = 0; p->h = 1; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_AMPS_ONLY:          p->c = 1; p->h = 0; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_GPS_ONLY:                p->c = 0; p->h = 0; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_GSM_ONLY:                p->c = 0; p->h = 0; p->g = 1; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_WCDMA_ONLY:              p->c = 0; p->h = 0; p->g = 0; p->w = 1; p->l = 0; p->t = 0; break;
    case NV_MODE_ANY_BUT_HDR:             p->c = 1; p->h = 0; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_GSM_WCDMA_ONLY:          p->c = 0; p->h = 0; p->g = 1; p->w = 1; p->l = 0; p->t = 0; break;
    case NV_MODE_DIGITAL_LESS_HDR_ONLY:   p->c = 1; p->h = 0; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_CDMA_HDR_ONLY:           p->c = 1; p->h = 1; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_AMPS_HDR_ONLY:      p->c = 1; p->h = 1; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_WLAN_ONLY:               p->c = 0; p->h = 0; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_WLAN_ONLY:          p->c = 1; p->h = 0; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_HDR_WLAN_ONLY:           p->c = 0; p->h = 1; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_HDR_WLAN_ONLY:      p->c = 1; p->h = 1; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_GSM_WLAN_ONLY:           p->c = 0; p->h = 0; p->g = 1; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_WCDMA_WLAN_ONLY:         p->c = 0; p->h = 0; p->g = 0; p->w = 1; p->l = 0; p->t = 0; break;
    case NV_MODE_GW_WLAN_ONLY:            p->c = 0; p->h = 0; p->g = 1; p->w = 1; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_AMPS_HDR_WLAN_ONLY: p->c = 1; p->h = 1; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_AMPS_WLAN_ONLY:     p->c = 1; p->h = 0; p->g = 0; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_ANY_BUT_HDR_WLAN:        p->c = 1; p->h = 0; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_LTE_ONLY:                p->c = 0; p->h = 0; p->g = 0; p->w = 0; p->l = 1; p->t = 0; break;
    case NV_MODE_GWL:                     p->c = 0; p->h = 0; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_HDR_LTE_ONLY:            p->c = 0; p->h = 1; p->g = 0; p->w = 0; p->l = 1; p->t = 0; break;
    case NV_MODE_CDMA_HDR_LTE_ONLY:       p->c = 1; p->h = 1; p->g = 0; p->w = 0; p->l = 1; p->t = 0; break;
    case NV_MODE_CDMA_HDR_GSM_WCDMA:      p->c = 1; p->h = 1; p->g = 1; p->w = 1; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_GSM_WCDMA:          p->c = 1; p->h = 0; p->g = 1; p->w = 1; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_LTE_ONLY:           p->c = 1; p->h = 0; p->g = 0; p->w = 0; p->l = 1; p->t = 0; break;
    case NV_MODE_CDMA_HDR_GSM :           p->c = 1; p->h = 1; p->g = 1; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_GSM  :              p->c = 1; p->h = 0; p->g = 1; p->w = 0; p->l = 0; p->t = 0; break;
    case NV_MODE_CDMA_HDR_GSM_WCDMA_LTE : p->c = 1; p->h = 1; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_ANY_BUT_WLAN:            p->c = 1; p->h = 1; p->g = 1; p->w = 1; p->l = 1; p->t = 0; break;
    case NV_MODE_TDS_ONLY:                p->c = 0; p->h = 0; p->g = 0; p->w = 0; p->l = 0; p->t = 1; break;
    case NV_MODE_TDS_GSM:                 p->c = 0; p->h = 0; p->g = 1; p->w = 0; p->l = 0; p->t = 1; break;
    case NV_MODE_TDS_GSM_LTE:             p->c = 0; p->h = 0; p->g = 1; p->w = 0; p->l = 1; p->t = 1; break;
    case NV_MODE_TDS_LTE:                 p->c = 0; p->h = 0; p->g = 0; p->w = 0; p->l = 1; p->t = 1; break;
    case NV_MODE_TDS_GSM_WCDMA_LTE:       p->c = 0; p->h = 0; p->g = 1; p->w = 1; p->l = 1; p->t = 1; break;
    case NV_MODE_TDS_GSM_WCDMA:           p->c = 0; p->h = 0; p->g = 1; p->w = 1; p->l = 0; p->t = 1; break;
    case NV_MODE_TDS_WCDMA:               p->c = 0; p->h = 0; p->g = 0; p->w = 1; p->l = 0; p->t = 1; break;
    case NV_MODE_TDS_WCDMA_LTE:           p->c = 0; p->h = 0; p->g = 0; p->w = 1; p->l = 1; p->t = 1; break;
    case NV_MODE_CDMA_HDR_GSM_TDS_LTE:    p->c = 1; p->h = 1; p->g = 1; p->w = 0; p->l = 1; p->t = 1; break;
    case NV_MODE_WCDMA_LTE_ONLY:          p->c = 0; p->h = 0; p->g = 0; p->w = 1; p->l = 1; p->t = 0; break;
    #endif

    default:
      QM_MSG_ERROR_1("unsupported NV value %d", pref_mode);
      ret_val = FALSE;
      break;
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION qmi_nas_cm_bc_2_qmi_bc()

  DESCRIPTION
    convert sys_band_class_e_type to enum nas_active_band_enum_v01
    
  PARAMETERS
    cm band class enum

  RETURN VALUE
    corresponding qmi band class enum value or;
    NAS_ACTIVE_BAND_ENUM_MAX_ENUM_VAL_V01 if error
===========================================================================*/
nas_active_band_enum_v01 qmi_nas_cm_bc_2_qmi_bc( sys_band_class_e_type cm_bc )
{
  nas_active_band_enum_v01 qmi_bc = NAS_ACTIVE_BAND_ENUM_MAX_ENUM_VAL_V01;
  switch ( cm_bc )
  {
    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
        case SYS_BAND_BC0:
      qmi_bc = NAS_ACTIVE_BAND_BC_0_V01;
      break;
    case SYS_BAND_BC1:
      qmi_bc = NAS_ACTIVE_BAND_BC_1_V01;
      break;
    case SYS_BAND_BC3:
      qmi_bc = NAS_ACTIVE_BAND_BC_3_V01;
      break;
    case SYS_BAND_BC4:
      qmi_bc = NAS_ACTIVE_BAND_BC_4_V01;
      break;
    case SYS_BAND_BC5:
      qmi_bc = NAS_ACTIVE_BAND_BC_5_V01;
      break;
    case SYS_BAND_BC6:
      qmi_bc = NAS_ACTIVE_BAND_BC_6_V01;
      break;
    case SYS_BAND_BC7:
      qmi_bc = NAS_ACTIVE_BAND_BC_7_V01;
      break;
    case SYS_BAND_BC8:
      qmi_bc = NAS_ACTIVE_BAND_BC_8_V01;
      break;
    case SYS_BAND_BC9:
      qmi_bc = NAS_ACTIVE_BAND_BC_9_V01;
      break;
    case SYS_BAND_BC10:
      qmi_bc = NAS_ACTIVE_BAND_BC_10_V01;
      break;
    case SYS_BAND_BC11:
      qmi_bc = NAS_ACTIVE_BAND_BC_11_V01;
      break;
    case SYS_BAND_BC12:
      qmi_bc = NAS_ACTIVE_BAND_BC_12_V01;
      break;
    case SYS_BAND_BC13:
      qmi_bc = NAS_ACTIVE_BAND_BC_13_V01;
      break;
    case SYS_BAND_BC14:
      qmi_bc = NAS_ACTIVE_BAND_BC_14_V01;
      break;
    case SYS_BAND_BC15:
      qmi_bc = NAS_ACTIVE_BAND_BC_15_V01;
      break;
    case SYS_BAND_BC16:
      qmi_bc = NAS_ACTIVE_BAND_BC_16_V01;
      break;
    case SYS_BAND_BC17:
      qmi_bc = NAS_ACTIVE_BAND_BC_17_V01;
      break;
    case SYS_BAND_BC18:
      qmi_bc = NAS_ACTIVE_BAND_BC_18_V01;
      break;
    case SYS_BAND_BC19:
      qmi_bc = NAS_ACTIVE_BAND_BC_19_V01;
      break;
    #endif

    #if defined(FEATURE_GSM) 
    case SYS_BAND_GSM_450:
      qmi_bc = NAS_ACTIVE_BAND_GSM_450_V01;
      break;
    case SYS_BAND_GSM_480:
      qmi_bc = NAS_ACTIVE_BAND_GSM_480_V01;
      break;
    case SYS_BAND_GSM_750:
      qmi_bc = NAS_ACTIVE_BAND_GSM_750_V01;
      break;
    case SYS_BAND_GSM_850:
      qmi_bc = NAS_ACTIVE_BAND_GSM_850_V01;
      break;
    case SYS_BAND_GSM_EGSM_900:
      qmi_bc = NAS_ACTIVE_BAND_GSM_900_EXTENDED_V01;
      break;
    case SYS_BAND_GSM_PGSM_900:
      qmi_bc = NAS_ACTIVE_BAND_GSM_900_PRIMARY_V01;
      break;
    case SYS_BAND_GSM_RGSM_900:
      qmi_bc = NAS_ACTIVE_BAND_GSM_900_RAILWAYS_V01;
      break;
    case SYS_BAND_GSM_DCS_1800:
      qmi_bc = NAS_ACTIVE_BAND_GSM_1800_V01;
      break;
    case SYS_BAND_GSM_PCS_1900:
      qmi_bc = NAS_ACTIVE_BAND_GSM_1900_V01;
      break;
    #endif

    #if defined(FEATURE_WCDMA)
    case SYS_BAND_WCDMA_I_IMT_2000:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_2100_V01;
      break;
    case SYS_BAND_WCDMA_II_PCS_1900:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_PCS_1900_V01;
      break;
    case SYS_BAND_WCDMA_III_1700:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_DCS_1800_V01;
      break;
    case SYS_BAND_WCDMA_IV_1700:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_1700_US_V01;
      break;
    case SYS_BAND_WCDMA_V_850:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_850_V01;
      break;
    case SYS_BAND_WCDMA_VI_800:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_800_V01;
      break;
    case SYS_BAND_WCDMA_VII_2600:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_2600_V01;
      break;
    case SYS_BAND_WCDMA_VIII_900:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_900_V01;
      break;
    case SYS_BAND_WCDMA_IX_1700:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_1700_JAPAN_V01;
      break;
    case SYS_BAND_WCDMA_XI_1500:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_1500_JAPAN_V01;
      break;
    case SYS_BAND_WCDMA_XIX_850:
      qmi_bc = NAS_ACTIVE_BAND_WCDMA_850_JAPAN_V01;
      break;
    #endif

    #if defined(FEATURE_LTE)
    case SYS_BAND_LTE_EUTRAN_BAND1:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_1_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND2:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_2_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND3:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_3_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND4:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_4_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND5:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_5_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND6:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_6_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND7:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_7_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND8:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_8_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND9:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_9_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND10:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_10_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND11:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_11_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND12:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_12_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND13:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_13_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND14:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_14_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND17:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_17_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND18:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_18_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND19:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_19_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND20:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_20_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND21:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_21_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND23:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_23_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND24:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_24_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND25:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_25_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND26:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_26_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND27:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_27_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND28:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_28_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND29:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_29_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND30:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_30_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND31:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_31_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND32:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_32_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND33:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_33_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND34:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_34_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND35:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_35_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND36:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_36_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND37:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_37_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND38:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_38_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND39:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_39_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND40:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND41:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_41_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND42:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_42_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND43:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_43_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND66:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_66_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND71:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_71_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND72:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_72_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND73:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_73_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND85:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_85_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND86:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_86_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND87:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_87_V01;
      break;
    case SYS_BAND_LTE_EUTRAN_BAND88:
      qmi_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_88_V01;
      break;
    #endif

    #if defined(FEATURE_TDSCDMA)
    case SYS_BAND_TDS_BANDA:
      qmi_bc = NAS_ACTIVE_BAND_TDSCDMA_BAND_A_V01;
      break;
    case SYS_BAND_TDS_BANDB:
      qmi_bc = NAS_ACTIVE_BAND_TDSCDMA_BAND_B_V01;
      break;
    case SYS_BAND_TDS_BANDC:
      qmi_bc = NAS_ACTIVE_BAND_TDSCDMA_BAND_C_V01;
      break;
    case SYS_BAND_TDS_BANDD:
      qmi_bc = NAS_ACTIVE_BAND_TDSCDMA_BAND_D_V01;
      break;
    case SYS_BAND_TDS_BANDE:
      qmi_bc = NAS_ACTIVE_BAND_TDSCDMA_BAND_E_V01;
      break;
    case SYS_BAND_TDS_BANDF:
      qmi_bc = NAS_ACTIVE_BAND_TDSCDMA_BAND_F_V01;
      break;
    #endif
    
    default:
      QM_MSG_ERROR_1("invalid cm bc %d", cm_bc);
      break;
  }

  return qmi_bc;
}

#if defined(FEATURE_HDR)
/*===========================================================================
  FUNCTION qmi_nas_cm_2_qmi_hdr_personality()

  DESCRIPTION
    convert sys_personality_e_type to enum qmi_nas_hdr_personality_e
    
  PARAMETERS
    sys_personality_e_type

  RETURN VALUE
    corresponding qmi_nas_hdr_personality_e
===========================================================================*/
enum qmi_nas_hdr_personality_e qmi_nas_cm_2_qmi_hdr_personality( sys_personality_e_type cm_val )
{
  enum qmi_nas_hdr_personality_e qmi_val;
  
  switch ( cm_val )
  {
    case SYS_PERSONALITY_HRPD:  qmi_val = QMI_NAS_HDR_PERSONALITY_HRPD;    break;
    case SYS_PERSONALITY_EHRPD: qmi_val = QMI_NAS_HDR_PERSONALITY_EHRPD;   break;
    default:
      qmi_val = QMI_NAS_HDR_PERSONALITY_UNKNOWN;
      break;
  }

  return qmi_val;
}
#endif

/*===========================================================================
  FUNCTION qmi_nas_strnlen()

  DESCRIPTION
    QMI_NAS local version of strnlen
    
  PARAMETERS
    - src_ptr  : pointer to start of the source buffer to calculate str len
    - nax_len  : maximum length to check for

  RETURN VALUE
   length of the string in the src buffer
===========================================================================*/
uint32 qmi_nas_strnlen ( const char *src_ptr, uint32 max_len)
{
  uint32 len = 0;

  if( src_ptr != NULL )
  {
    for( len = 0; len < max_len && src_ptr[len] != '\0'; len++ );
  }
  else
  {
    QM_MSG_ERROR("Invalid buffer pointer.");
  }

  return len;
}

/*===========================================================================
  FUNCTION convert_endianness16()

  DESCRIPTION
    Convert endianness of 16bit character string by swapping first and second
    byte of each character

  PARAMETERS
    buf: ptr to buffer that will be converted.
    size: size of 'buf' in terms of number of bytes. 'size' is expected to be
          an even number. if not, last byte at the end will be left untouched
===========================================================================*/
void convert_endianness16( unsigned char *buf, unsigned int size )
{
  unsigned char tmp;
  unsigned int  i;

  if ( buf )
  {
    for ( i=0; i<size-1; i+=2 )
    {
      tmp        = *(buf+i);
      *(buf+i)   = *(buf+i+1);
      *(buf+i+1) = tmp;
    }
  }
}

/*===========================================================================
  FUNCTION qmi_nas_eutra_status_cm_2_qmi()

  DESCRIPTION
    convert sys_eutra_status_enum_type to nas_eutra_cell_status_enum_type_v01
    
  PARAMETERS
    cm eutra status enum

  RETURN VALUE
    corresponding qmi eutra status enum value
===========================================================================*/
nas_eutra_cell_status_enum_type_v01 qmi_nas_eutra_status_cm_2_qmi
( 
  sys_eutra_status_enum_type cm_eutra_status 
)
{
  nas_eutra_cell_status_enum_type_v01 qmi_val;
  
  switch ( cm_eutra_status )
  {
    case SYS_EUTRA_CELL_PRESENT          : qmi_val = NAS_EUTRA_CELL_PRESENT_V01;               break;
    case SYS_EUTRA_CELL_NOT_PRESENT      : qmi_val = NAS_EUTRA_CELL_NOT_PRESENT_V01;           break;
    case SYS_EUTRA_DETECTION_UNKNOWN     : qmi_val = NAS_EUTRA_CELL_PRESENCE_UNKNOWN_V01;      break;
    case SYS_EUTRA_DETECTION_UNSUPPORTED : qmi_val = NAS_EUTRA_CELL_DETECTION_UNSUPPORTED_V01; break;
    default:
      QM_MSG_HIGH_1("unknown eutra status %d", cm_eutra_status);
      qmi_val = NAS_EUTRA_CELL_PRESENCE_UNKNOWN_V01;
      break;
  }

  return qmi_val;
} /* END_FUNCTION qmi_nas_eutra_status_cm_2_qmi() */

/*===========================================================================
  FUNCTION qmi_nas_validate_cmd_buf_p()

  DESCRIPTION
    Function to validate returned cmd_buf_p is still allocated in client.

  PARAMETERS
    cmd_buf_p: pointer to queue element for comparison
    nas_state: pointer to qmi_nas state
===========================================================================*/
boolean qmi_nas_validate_cmd_buf_p( qmi_mmode_qcsi_transaction_type *cmd_buf_p, qmi_mmode_svc_info_type *qmi_nas_state )
{

  if( cmd_buf_p == NULL || qmi_nas_state == NULL )
  {
    return FALSE;
  }
  
  if ( NULL != q_linear_search( &qmi_nas_state->client_q,
                                 qmi_mmode_qcsi_compare_client_handle,
                                 cmd_buf_p->clnt_info_ptr) )
  {
    if ( NULL != q_linear_search( &qmi_nas_state->pending_req,
                                 qmi_mmode_qcsi_compare_pending_trans,
                                 cmd_buf_p->req_handle) )
    {
      return TRUE;
    }
  }

  QM_MSG_HIGH("Buffer does not match existing client transaction");
  return FALSE;
}

/*===========================================================================
  FUNCTION qmi_nas_rat_qmi_to_mmgsdi()

  DESCRIPTION
    Translate QMI_NAS RAT to MMGSDI RAT enum type

  PARAMETERS
    qmi_rat: QMI_NAS Radio access tech
===========================================================================*/
mmgsdi_rat_enum_type qmi_nas_rat_qmi_to_mmgsdi( enum qmi_nas_radio_if_e qmi_rat )
{
  mmgsdi_rat_enum_type mmgsdi_val = MMGSDI_RAT_NONE;
  
  switch ( qmi_rat )
  {
    #if defined(FEATURE_GSM) 
    case QMI_NAS_RADIO_IF_GSM:     mmgsdi_val = MMGSDI_RAT_GSM;  break;
    #endif
    #if defined(FEATURE_WCDMA)
    case QMI_NAS_RADIO_IF_UMTS:    mmgsdi_val = MMGSDI_RAT_UMTS; break;
    #endif
    #if defined(FEATURE_LTE)
    case QMI_NAS_RADIO_IF_LTE:     mmgsdi_val = MMGSDI_RAT_LTE;  break;
    #endif
    #if defined(FEATURE_TDSCDMA)
    case QMI_NAS_RADIO_IF_TDSCDMA: mmgsdi_val = MMGSDI_RAT_TDS;  break;
    #endif
    default:
      QM_MSG_HIGH_1("unknown rat %d", qmi_rat);
      break;
  }

  return mmgsdi_val;
}

/*===========================================================================
  FUNCTION qmi_nas_rat_mmgsdi_to_qmi()

  DESCRIPTION
    Translate MMGSDI RAT enum type to QMI_NAS RAT

  PARAMETERS
    mmgsdi_rat: MMGSDI Radio access tech
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_rat_mmgsdi_to_qmi( mmgsdi_rat_enum_type mmgsdi_rat )
{
  enum qmi_nas_radio_if_e qmi_val = QMI_NAS_RADIO_IF_NONE;
  
  switch ( mmgsdi_rat )
  {
    #if defined(FEATURE_GSM) 
    case MMGSDI_RAT_GSM:  qmi_val = QMI_NAS_RADIO_IF_GSM;      break;
    #endif
    #if defined(FEATURE_WCDMA)
    case MMGSDI_RAT_UMTS: qmi_val = QMI_NAS_RADIO_IF_UMTS;     break;
    #endif
    #if defined(FEATURE_LTE)
    case MMGSDI_RAT_LTE:  qmi_val = QMI_NAS_RADIO_IF_LTE;      break;
    #endif
    #if defined(FEATURE_TDSCDMA)
    case MMGSDI_RAT_TDS:  qmi_val = QMI_NAS_RADIO_IF_TDSCDMA;  break;
    #endif
    default:
      QM_MSG_HIGH_1("unknown rat %d", mmgsdi_rat);
      break;
  }

  return qmi_val;
}

#ifdef FEATURE_MMODE_REPORT_POWERSAVE
/*===========================================================================
  FUNCTION qmi_nas_get_srv_status_from_cmss_event()

  DESCRIPTION
    Function to get srv_status for different RATs by looking up CMSS Event.

  PARAMETERS
    p_info: pointer to latest cm ss event
    stack_index: stack to look into in latest cm ss event
    sys_mode: sys_mode/RAT for which we need to determine service status
===========================================================================*/
nas_service_status_enum_type_v01 qmi_nas_get_srv_status_from_cmss_event
(
  cm_mm_ss_info_s_type *p_info, 
  sys_sys_mode_e_type sys_mode
)
{
   sys_srv_status_e_type srv_status;
   switch(sys_mode)
   {
     #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
     case SYS_SYS_MODE_CDMA:
       switch(p_info->asubs_id)
       {
         case SYS_MODEM_AS_ID_2:
#ifdef FEATURE_TRIPLE_SIM
         case SYS_MODEM_AS_ID_3:
#endif
           srv_status = SYS_SRV_STATUS_NONE;
           break;

         default:
           if(p_info->stack_info[stack_index].sys_mode == sys_mode || p_info->stack_info[stack_index].sys_mode == SYS_SYS_MODE_NO_SRV)
           {
             srv_status = p_info->stack_info[stack_index].srv_status;
           }
           else
           {
             srv_status = SYS_SRV_STATUS_NO_SRV;
           }
           break;
       }
       break;
     #endif

     #if defined(FEATURE_HDR)
     case SYS_SYS_MODE_HDR:
       switch(p_info->asubs_id)
       {
         case SYS_MODEM_AS_ID_2:
#ifdef FEATURE_TRIPLE_SIM
         case SYS_MODEM_AS_ID_3:
#endif
           srv_status = SYS_SRV_STATUS_NONE;
           break;

         default:
           if( stack_index != STACK_INDEX_MAX && p_info->stack_info[stack_index].sys_mode == sys_mode )
           {
             srv_status = p_info->stack_info[stack_index].srv_status;
           }
           else
           {
             if( p_info->stack_info[STACK_INDEX_0].sys_mode == SYS_SYS_MODE_NO_SRV )
           {
               srv_status = p_info->stack_info[STACK_INDEX_0].srv_status;
           }
             else if( p_info->number_of_stacks > 1 && p_info->stack_info[STACK_INDEX_1].sys_mode == SYS_SYS_MODE_NO_SRV )
           {
               srv_status = p_info->stack_info[STACK_INDEX_1].srv_status;
           }
           else
           {
             srv_status = SYS_SRV_STATUS_NO_SRV;
           }
           }
           break;
       }
       break;
     #endif

     #if defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)
     case SYS_SYS_MODE_TDS:
     case SYS_SYS_MODE_WCDMA:
     #endif
     #if defined(FEATURE_GSM)
     case SYS_SYS_MODE_GSM:
     #endif
     case SYS_SYS_MODE_LTE:
       {
         if( p_info->sys_mode == sys_mode )
         {
           srv_status = p_info->srv_status;
         }
         #if (defined(FEATURE_CDMA) || defined(FEATURE_HDR))
         else if( p_info->number_of_stacks > 1 && p_info->stack_info[STACK_INDEX_1].sys_mode == sys_mode )
         {
           srv_status = p_info->stack_info[STACK_INDEX_1].srv_status; 
         }
         #endif
         else
         {
           if( p_info->sys_mode == SYS_SYS_MODE_NO_SRV )
           {
             srv_status = p_info->srv_status;
           }
           #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE)) && defined(FEATURE_HDR)
           else if( p_info->number_of_stacks > 1 && p_info->stack_info[STACK_INDEX_1].sys_mode == SYS_SYS_MODE_NO_SRV )
           {
             srv_status = p_info->stack_info[STACK_INDEX_1].srv_status; 
           }
           #endif
           else
           {
             srv_status = SYS_SRV_STATUS_NO_SRV;
           }
         }
       }
       break;

     default:
       srv_status = SYS_SRV_STATUS_NONE;
       break;
   }

   return (nas_service_status_enum_type_v01)srv_status;
}
#endif // FEATURE_MMODE_REPORT_POWERSAVE

/*===========================================================================
  FUNCTION QMI_NAS_CHECK_SYS_INFO_CHG_AGAINST_RPT_LIMITS()

  DESCRIPTION
    Function to determine if fields under limited reporting changed.

  PARAMETERS

===========================================================================*/
boolean qmi_nas_check_sys_info_chg_against_rpt_limits(
  nas_sys_info_ind_msg_v01 * new_ind,
  nas_sys_info_ind_msg_v01 * old_ind,
  uint64                     sys_info_limits
)
{
  boolean                    retval = FALSE;

  if( sys_info_limits & QMI_NAS_LIMIT_BY_SRV_STATUS )
  {
    // If both TLVs are valid, and the field differs, or if only one of the TLVs is valid
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
         #ifndef FEATURE_MMODE_QMI_LTE_ONLY
         #ifdef FEATURE_MMODE_3GPP2
         ( new_ind->cdma_srv_status_info_valid         &&
           old_ind->cdma_srv_status_info_valid         &&
           new_ind->cdma_srv_status_info.srv_status    != old_ind->cdma_srv_status_info.srv_status    ) ||
         ( new_ind->hdr_srv_status_info_valid          &&
           old_ind->hdr_srv_status_info_valid          &&
           new_ind->hdr_srv_status_info.srv_status     != old_ind->hdr_srv_status_info.srv_status     ) ||
           #endif
         ( new_ind->gsm_srv_status_info_valid          &&
           old_ind->gsm_srv_status_info_valid          &&
           new_ind->gsm_srv_status_info.srv_status     != old_ind->gsm_srv_status_info.srv_status     ) ||
           #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_srv_status_info_valid        &&
           old_ind->wcdma_srv_status_info_valid        &&
           new_ind->wcdma_srv_status_info.srv_status   != old_ind->wcdma_srv_status_info.srv_status   ) ||
           #endif
         ( new_ind->lte_srv_status_info_valid          &&
           old_ind->lte_srv_status_info_valid          &&
           new_ind->lte_srv_status_info.srv_status     != old_ind->lte_srv_status_info.srv_status     ) ||
           #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_srv_status_info_valid      &&
           old_ind->tdscdma_srv_status_info_valid      &&
           new_ind->tdscdma_srv_status_info.srv_status != old_ind->tdscdma_srv_status_info.srv_status ) ||
           #endif
           #ifdef FEATURE_MMODE_3GPP2
         QMI_NAS_XOR( new_ind->cdma_srv_status_info_valid   , old_ind->cdma_srv_status_info_valid     ) ||
         QMI_NAS_XOR( new_ind->hdr_srv_status_info_valid    , old_ind->hdr_srv_status_info_valid      ) ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_srv_status_info_valid    , old_ind->gsm_srv_status_info_valid      ) ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_srv_status_info_valid  , old_ind->wcdma_srv_status_info_valid    ) ||
         #endif
         QMI_NAS_XOR( new_ind->lte_srv_status_info_valid    , old_ind->lte_srv_status_info_valid      ) 
         #ifdef FEATURE_TDSCDMA
         || QMI_NAS_XOR( new_ind->tdscdma_srv_status_info_valid, old_ind->tdscdma_srv_status_info_valid   )  
         #endif
         #else
         ( new_ind->lte_srv_status_info_valid          &&
           old_ind->lte_srv_status_info_valid          &&
           new_ind->lte_srv_status_info.srv_status     != old_ind->lte_srv_status_info.srv_status     ) ||
         QMI_NAS_XOR( new_ind->lte_srv_status_info_valid    , old_ind->lte_srv_status_info_valid      )
         #endif
    )
    {
      retval = TRUE;
    }
  }
  
  if( sys_info_limits & QMI_NAS_LIMIT_BY_SRV_DOMAIN )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if (
          #ifndef FEATURE_MMODE_QMI_LTE_ONLY
          #ifdef FEATURE_MMODE_3GPP2
          ( new_ind->cdma_sys_info_valid                         &&
           old_ind->cdma_sys_info_valid                         &&
           new_ind->cdma_sys_info.common_sys_info.srv_domain    != old_ind->cdma_sys_info.common_sys_info.srv_domain    ) ||
         ( new_ind->hdr_sys_info_valid                          &&
           old_ind->hdr_sys_info_valid                          &&
           new_ind->hdr_sys_info.common_sys_info.srv_domain     != old_ind->hdr_sys_info.common_sys_info.srv_domain     ) ||
           #endif
         ( new_ind->gsm_sys_info_valid                          &&
           old_ind->gsm_sys_info_valid                          &&
           new_ind->gsm_sys_info.common_sys_info.srv_domain     != old_ind->gsm_sys_info.common_sys_info.srv_domain     ) ||
           #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid                        &&
           old_ind->wcdma_sys_info_valid                        &&
           new_ind->wcdma_sys_info.common_sys_info.srv_domain   != old_ind->wcdma_sys_info.common_sys_info.srv_domain   ) ||
           #endif
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.common_sys_info.srv_domain     != old_ind->lte_sys_info.common_sys_info.srv_domain     ) ||
           #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid                      &&
           old_ind->tdscdma_sys_info_valid                      &&
           new_ind->tdscdma_sys_info.common_sys_info.srv_domain != old_ind->tdscdma_sys_info.common_sys_info.srv_domain ) ||
           #endif
         #ifdef FEATURE_MMODE_3GPP2
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid   , old_ind->cdma_sys_info_valid    )                                  ||
         QMI_NAS_XOR( new_ind->hdr_sys_info_valid    , old_ind->hdr_sys_info_valid     )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info_valid    , old_ind->gsm_sys_info_valid     )                                  ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )                                  
         #ifdef FEATURE_TDSCDMA
         || QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )       
         #endif
         #else
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.common_sys_info.srv_domain     != old_ind->lte_sys_info.common_sys_info.srv_domain     ) ||
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )
         #endif
    )
  {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_PLMN_ID )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
         #ifndef FEATURE_MMODE_QMI_LTE_ONLY
         #ifdef FEATURE_MMODE_3GPP2
         ( new_ind->cdma_sys_info_valid    &&
           old_ind->cdma_sys_info_valid    &&
           ( memcmp( new_ind->cdma_sys_info.cdma_specific_sys_info.network_id.mcc,
                     old_ind->cdma_sys_info.cdma_specific_sys_info.network_id.mcc,
                     sizeof(old_ind->cdma_sys_info.cdma_specific_sys_info.network_id.mcc)        ) ||
             memcmp( new_ind->cdma_sys_info.cdma_specific_sys_info.network_id.mnc,
                     old_ind->cdma_sys_info.cdma_specific_sys_info.network_id.mnc,
                     sizeof(old_ind->cdma_sys_info.cdma_specific_sys_info.network_id.mnc)        )  )  )  ||
           #endif
         ( new_ind->gsm_sys_info_valid     &&
           old_ind->gsm_sys_info_valid     &&
           ( memcmp( new_ind->gsm_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     old_ind->gsm_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     sizeof(old_ind->gsm_sys_info.threegpp_specific_sys_info.network_id.mcc)     ) ||
             memcmp( new_ind->gsm_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     old_ind->gsm_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     sizeof(old_ind->gsm_sys_info.threegpp_specific_sys_info.network_id.mnc)     )  )  )  ||
          #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid   &&
           old_ind->wcdma_sys_info_valid   &&
           ( memcmp( new_ind->wcdma_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     old_ind->wcdma_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     sizeof(old_ind->wcdma_sys_info.threegpp_specific_sys_info.network_id.mcc)   ) ||
             memcmp( new_ind->wcdma_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     old_ind->wcdma_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     sizeof(old_ind->wcdma_sys_info.threegpp_specific_sys_info.network_id.mnc)   )  )  )  ||
          #endif
         ( new_ind->lte_sys_info_valid     &&
           old_ind->lte_sys_info_valid     &&
           ( memcmp( new_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     sizeof(old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mcc)     ) ||
             memcmp( new_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     sizeof(old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mnc)     )  )  )  ||
          #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid &&
           old_ind->cdma_sys_info_valid    &&
           ( memcmp( new_ind->tdscdma_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     old_ind->tdscdma_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     sizeof(old_ind->tdscdma_sys_info.threegpp_specific_sys_info.network_id.mcc) ) ||
             memcmp( new_ind->tdscdma_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     old_ind->tdscdma_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     sizeof(old_ind->tdscdma_sys_info.threegpp_specific_sys_info.network_id.mnc) )  )  ) ||
         #endif
         #ifdef FEATURE_MMODE_3GPP2
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid   , old_ind->cdma_sys_info_valid    )                 ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info_valid    , old_ind->gsm_sys_info_valid     )                 ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )                 ||
         #endif
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )                 
         #ifdef FEATURE_TDSCDMA
         || QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )
         #endif
         #else
         ( new_ind->lte_sys_info_valid     &&
           old_ind->lte_sys_info_valid     &&
           ( memcmp( new_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mcc,
                     sizeof(old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mcc)     ) ||
             memcmp( new_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mnc,
                     sizeof(old_ind->lte_sys_info.threegpp_specific_sys_info.network_id.mnc)     )  )  )  ||
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     ) 
         #endif
      )
    {
      retval = TRUE;
    }
}

  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
  if( sys_info_limits & QMI_NAS_LIMIT_BY_SID_NID )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->cdma_sys_info_valid                                    &&
           old_ind->cdma_sys_info_valid                                    &&
           ( new_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.sid != old_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.sid ||
             new_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.nid != old_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.nid  )  ) ||
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid, old_ind->cdma_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }
  #endif

  if( sys_info_limits & QMI_NAS_LIMIT_BY_ROAM_STATUS )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
         #ifndef FEATURE_MMODE_QMI_LTE_ONLY
         #ifdef FEATURE_MMODE_3GPP2
         ( new_ind->cdma_sys_info_valid                          &&
           old_ind->cdma_sys_info_valid                          &&
           new_ind->cdma_sys_info.common_sys_info.roam_status    != old_ind->cdma_sys_info.common_sys_info.roam_status    ) ||
         ( new_ind->hdr_sys_info_valid                           &&
           old_ind->hdr_sys_info_valid                           &&
           new_ind->hdr_sys_info.common_sys_info.roam_status     != old_ind->hdr_sys_info.common_sys_info.roam_status     ) ||
         #endif
         ( new_ind->gsm_sys_info_valid                           &&
           old_ind->gsm_sys_info_valid                           &&
           new_ind->gsm_sys_info.common_sys_info.roam_status     != old_ind->gsm_sys_info.common_sys_info.roam_status     ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid                         &&
           old_ind->wcdma_sys_info_valid                         &&
           new_ind->wcdma_sys_info.common_sys_info.roam_status   != old_ind->wcdma_sys_info.common_sys_info.roam_status   ) ||
         #endif
         ( new_ind->lte_sys_info_valid                           &&
           old_ind->lte_sys_info_valid                           &&
           new_ind->lte_sys_info.common_sys_info.roam_status     != old_ind->lte_sys_info.common_sys_info.roam_status     ) ||
         #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid                       &&
           old_ind->tdscdma_sys_info_valid                       &&
           new_ind->tdscdma_sys_info.common_sys_info.roam_status != old_ind->tdscdma_sys_info.common_sys_info.roam_status ) ||
         #endif
         #ifdef FEATURE_MMODE_3GPP2
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid   , old_ind->cdma_sys_info_valid    )                                    ||
         QMI_NAS_XOR( new_ind->hdr_sys_info_valid    , old_ind->hdr_sys_info_valid     )                                    ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info_valid    , old_ind->gsm_sys_info_valid     )                                    ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )                                    ||
         #endif
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )                                    
         #ifdef FEATURE_TDSCDMA
         ||
         QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )  
         #endif
         #else
         ( new_ind->lte_sys_info_valid                           &&
           old_ind->lte_sys_info_valid                           &&
           new_ind->lte_sys_info.common_sys_info.roam_status     != old_ind->lte_sys_info.common_sys_info.roam_status     ) ||
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     ) 
         #endif
      )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_SRV_CAPABILITY )
    {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
         #ifndef FEATURE_MMODE_QMI_LTE_ONLY
         #ifdef FEATURE_MMODE_3GPP2
         ( new_ind->cdma_sys_info_valid                         &&
           old_ind->cdma_sys_info_valid                         &&
           new_ind->cdma_sys_info.common_sys_info.srv_capability != old_ind->cdma_sys_info.common_sys_info.srv_capability ) ||
         ( new_ind->hdr_sys_info_valid                          &&
           old_ind->hdr_sys_info_valid                          &&
           new_ind->hdr_sys_info.common_sys_info.srv_capability != old_ind->hdr_sys_info.common_sys_info.srv_capability ) ||
         #endif
         ( new_ind->gsm_sys_info_valid                          &&
           old_ind->gsm_sys_info_valid                          &&
           new_ind->gsm_sys_info.common_sys_info.srv_capability != old_ind->gsm_sys_info.common_sys_info.srv_capability ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid                        &&
           old_ind->wcdma_sys_info_valid                        &&
           new_ind->wcdma_sys_info.common_sys_info.srv_capability != old_ind->wcdma_sys_info.common_sys_info.srv_capability ) ||
         #endif
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.common_sys_info.srv_capability != old_ind->lte_sys_info.common_sys_info.srv_capability ) ||
         #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid                      &&
           old_ind->tdscdma_sys_info_valid                      &&
           new_ind->tdscdma_sys_info.common_sys_info.srv_capability != old_ind->tdscdma_sys_info.common_sys_info.srv_capability ) ||
         #endif
         #ifdef FEATURE_MMODE_3GPP2
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid   , old_ind->cdma_sys_info_valid    )                                  ||
         QMI_NAS_XOR( new_ind->hdr_sys_info_valid    , old_ind->hdr_sys_info_valid     )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info_valid    , old_ind->gsm_sys_info_valid     )                                  ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )                                  
         #ifdef FEATURE_TDSCDMA
         ||
         QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )  
         #endif
         #else
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.common_sys_info.srv_capability != old_ind->lte_sys_info.common_sys_info.srv_capability ) ||
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     ) 
         #endif
     )
  {
      retval = TRUE;
    }
  }
  
  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
  if( sys_info_limits & QMI_NAS_LIMIT_BY_PACKET_ZONE )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->cdma_sys_info_valid                                    &&
           old_ind->cdma_sys_info_valid                                    &&
           ( new_ind->cdma_sys_info.cdma_specific_sys_info.packet_zone != old_ind->cdma_sys_info.cdma_specific_sys_info.packet_zone )  ) ||
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid, old_ind->cdma_sys_info_valid )  )
  {
      retval = TRUE;
    }
  }
  #endif

  #if defined(FEATURE_HDR)
  if( sys_info_limits & QMI_NAS_LIMIT_BY_IS856_SYS_ID )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->hdr_sys_info_valid                                    &&
           old_ind->hdr_sys_info_valid                                    &&
           memcmp( new_ind->hdr_sys_info.hdr_specific_sys_info.is856_sys_id,
                   old_ind->hdr_sys_info.hdr_specific_sys_info.is856_sys_id,
                   sizeof(old_ind->hdr_sys_info.hdr_specific_sys_info.is856_sys_id) )  
          ) || QMI_NAS_XOR( new_ind->hdr_sys_info_valid, old_ind->hdr_sys_info_valid )  )
    {
      retval = TRUE;
    }
}
  #endif

  if( sys_info_limits & QMI_NAS_LIMIT_BY_CELL_ID )
    {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
         #ifndef FEATURE_MMODE_QMI_LTE_ONLY
         ( new_ind->gsm_sys_info_valid                          &&
           old_ind->gsm_sys_info_valid                          &&
           new_ind->gsm_sys_info.threegpp_specific_sys_info.cell_id != old_ind->gsm_sys_info.threegpp_specific_sys_info.cell_id ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid                        &&
           old_ind->wcdma_sys_info_valid                        &&
           new_ind->wcdma_sys_info.threegpp_specific_sys_info.cell_id != old_ind->wcdma_sys_info.threegpp_specific_sys_info.cell_id ) ||
         #endif
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.threegpp_specific_sys_info.cell_id != old_ind->lte_sys_info.threegpp_specific_sys_info.cell_id ) ||
         #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid                      &&
           old_ind->tdscdma_sys_info_valid                      &&
           new_ind->tdscdma_sys_info.threegpp_specific_sys_info.cell_id != old_ind->tdscdma_sys_info.threegpp_specific_sys_info.cell_id ) ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info_valid    , old_ind->gsm_sys_info_valid     )                                  ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )                                
         #ifdef FEATURE_TDSCDMA
         ||
         QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )   
         #endif
         #else
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.threegpp_specific_sys_info.cell_id != old_ind->lte_sys_info.threegpp_specific_sys_info.cell_id ) ||
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     ) 
         #endif
     )
  {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_LAC )
    {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
         #ifndef FEATURE_MMODE_QMI_LTE_ONLY    
         ( new_ind->gsm_sys_info_valid                          &&
           old_ind->gsm_sys_info_valid                          &&
           new_ind->gsm_sys_info.threegpp_specific_sys_info.lac != old_ind->gsm_sys_info.threegpp_specific_sys_info.lac ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid                        &&
           old_ind->wcdma_sys_info_valid                        &&
           new_ind->wcdma_sys_info.threegpp_specific_sys_info.lac != old_ind->wcdma_sys_info.threegpp_specific_sys_info.lac ) ||
         #endif
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.threegpp_specific_sys_info.lac != old_ind->lte_sys_info.threegpp_specific_sys_info.lac ) ||
         #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid                      &&
           old_ind->tdscdma_sys_info_valid                      &&
           new_ind->tdscdma_sys_info.threegpp_specific_sys_info.lac != old_ind->tdscdma_sys_info.threegpp_specific_sys_info.lac ) ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info_valid    , old_ind->gsm_sys_info_valid     )                                  ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )                                  
         #ifdef FEATURE_TDSCDMA
         ||
         QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )      
         #endif
         #else
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.threegpp_specific_sys_info.lac != old_ind->lte_sys_info.threegpp_specific_sys_info.lac ) ||
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     ) 
         #endif

      )
  {
      retval = TRUE;
  }
}


  #ifndef FEATURE_MMODE_QMI_LTE_ONLY    
  if( sys_info_limits & QMI_NAS_LIMIT_BY_RAC )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->gsm_rac_valid                                    &&
           old_ind->gsm_rac_valid                                    &&
           new_ind->gsm_rac != old_ind->gsm_rac ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_rac_valid                                    &&
           old_ind->wcdma_rac_valid                                    &&
           new_ind->wcdma_rac != old_ind->wcdma_rac ) ||
         #endif
          QMI_NAS_XOR( new_ind->gsm_rac_valid, old_ind->gsm_rac_valid )  
          #ifdef FEATURE_WCDMA
          ||
          QMI_NAS_XOR( new_ind->wcdma_rac_valid, old_ind->wcdma_rac_valid )
          #endif
          )
{
      retval = TRUE;
    }
  }
  #endif
    
  if( sys_info_limits & QMI_NAS_LIMIT_BY_TAC )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->lte_sys_info_valid                                    &&
           old_ind->lte_sys_info_valid                                    &&
           new_ind->lte_sys_info.lte_specific_sys_info.tac != old_ind->lte_sys_info.lte_specific_sys_info.tac 
          ) || QMI_NAS_XOR( new_ind->lte_sys_info_valid, old_ind->lte_sys_info_valid )  )
{
      retval = TRUE;
    }
  }

  #ifndef FEATURE_MMODE_QMI_LTE_ONLY    
  #if !defined(FEATURE_MMODE_QMI_GPRS_LTE)
  #if defined(FEATURE_TDSCDMA) || defined(FEATURE_WCDMA) 
  if( sys_info_limits & QMI_NAS_LIMIT_BY_HS_CALL_STATUS )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid                        &&
           old_ind->wcdma_sys_info_valid                        &&
           new_ind->wcdma_sys_info.wcdma_specific_sys_info.hs_call_status != old_ind->wcdma_sys_info.wcdma_specific_sys_info.hs_call_status ) ||
         #endif
         #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid                      &&
           old_ind->tdscdma_sys_info_valid                      &&
           new_ind->tdscdma_sys_info.tdscdma_specific_sys_info.hs_call_status != old_ind->tdscdma_sys_info.tdscdma_specific_sys_info.hs_call_status ) ||
         #endif
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )  ||
         #endif
         #ifdef FEATURE_TDSCDMA
         QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )
         #endif
         )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_HS_IND )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->wcdma_sys_info_valid                        &&
           old_ind->wcdma_sys_info_valid                        &&
           new_ind->wcdma_sys_info.wcdma_specific_sys_info.hs_ind != old_ind->wcdma_sys_info.wcdma_specific_sys_info.hs_ind ) ||
         ( new_ind->tdscdma_sys_info_valid                      &&
           old_ind->tdscdma_sys_info_valid                      &&
           new_ind->tdscdma_sys_info.tdscdma_specific_sys_info.hs_ind != old_ind->tdscdma_sys_info.tdscdma_specific_sys_info.hs_ind ) ||
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )  ||
         QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )    )
    {
      retval = TRUE;
    }
  }
#endif
#if defined(FEATURE_HDR)
  if( sys_info_limits & QMI_NAS_LIMIT_BY_HDR_ACTIVE_PROTOCOL )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->hdr_sys_info_valid                                    &&
           old_ind->hdr_sys_info_valid                                    &&
           ( new_ind->hdr_sys_info.hdr_specific_sys_info.hdr_active_prot != old_ind->hdr_sys_info.hdr_specific_sys_info.hdr_active_prot )
          ) || QMI_NAS_XOR( new_ind->hdr_sys_info_valid, old_ind->hdr_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }
#endif
  #endif

  if( sys_info_limits & QMI_NAS_LIMIT_BY_EGPRS_SUPPORT_IND )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->gsm_sys_info_valid                                    &&
           old_ind->gsm_sys_info_valid                                    &&
           ( new_ind->gsm_sys_info.gsm_specific_sys_info.egprs_supp != old_ind->gsm_sys_info.gsm_specific_sys_info.egprs_supp )
          ) || QMI_NAS_XOR( new_ind->gsm_sys_info_valid, old_ind->gsm_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }

#if defined(FEATURE_HDR)
  #if !defined(FEATURE_MMODE_QMI_GPRS_LTE)
  if( sys_info_limits & QMI_NAS_LIMIT_BY_HDR_PERSONALITY )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->hdr_sys_info_valid                                    &&
           old_ind->hdr_sys_info_valid                                    &&
           ( new_ind->hdr_sys_info.hdr_specific_sys_info.hdr_personality != old_ind->hdr_sys_info.hdr_specific_sys_info.hdr_personality )
          ) || QMI_NAS_XOR( new_ind->hdr_sys_info_valid, old_ind->hdr_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }
#endif

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  if( sys_info_limits & QMI_NAS_LIMIT_BY_CCS_SUPPORTED )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->cdma_sys_info_valid                                    &&
           old_ind->cdma_sys_info_valid                                    &&
           ( new_ind->cdma_sys_info.cdma_specific_sys_info.ccs_supported != old_ind->cdma_sys_info.cdma_specific_sys_info.ccs_supported )
          ) || QMI_NAS_XOR( new_ind->cdma_sys_info_valid, old_ind->cdma_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }
#endif
#endif

  if( sys_info_limits & QMI_NAS_LIMIT_BY_DTM_SUPPORTED )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->gsm_sys_info_valid                                    &&
           old_ind->gsm_sys_info_valid                                    &&
           ( new_ind->gsm_sys_info.gsm_specific_sys_info.dtm_supp != old_ind->gsm_sys_info.gsm_specific_sys_info.dtm_supp )
          ) || QMI_NAS_XOR( new_ind->gsm_sys_info_valid, old_ind->gsm_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_CS_BAR_STATUS )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->gsm_sys_info3_valid                        &&
           old_ind->gsm_sys_info3_valid                        &&
           new_ind->gsm_sys_info3.cs_bar_status != old_ind->gsm_sys_info3.cs_bar_status ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info3_valid                      &&
           old_ind->wcdma_sys_info3_valid                      &&
           new_ind->wcdma_sys_info3.cs_bar_status != old_ind->wcdma_sys_info3.cs_bar_status ) ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info3_valid  , old_ind->gsm_sys_info3_valid   )  
         #ifdef FEATURE_WCDMA
         ||
         QMI_NAS_XOR( new_ind->wcdma_sys_info3_valid, old_ind->wcdma_sys_info3_valid )
         #endif
         )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_PS_BAR_STATUS )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->gsm_sys_info3_valid                        &&
           old_ind->gsm_sys_info3_valid                        &&
           new_ind->gsm_sys_info3.ps_bar_status != old_ind->gsm_sys_info3.ps_bar_status ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info3_valid                      &&
           old_ind->wcdma_sys_info3_valid                      &&
           new_ind->wcdma_sys_info3.ps_bar_status != old_ind->wcdma_sys_info3.ps_bar_status ) ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info3_valid  , old_ind->gsm_sys_info3_valid   )  
         #ifdef FEATURE_WCDMA
         ||
         QMI_NAS_XOR( new_ind->wcdma_sys_info3_valid, old_ind->wcdma_sys_info3_valid )
         #endif
         )
    {
      retval = TRUE;
    }
  }
  #endif

  if( sys_info_limits & QMI_NAS_LIMIT_BY_VOICE_SUPPORT_ON_LTE )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->voice_support_on_lte_valid                                    &&
           old_ind->voice_support_on_lte_valid                                    &&
           ( new_ind->voice_support_on_lte != old_ind->voice_support_on_lte )
          ) || QMI_NAS_XOR( new_ind->voice_support_on_lte_valid, old_ind->voice_support_on_lte_valid )  )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_SYS_INFO_NO_CHANGE )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->sys_info_no_change_valid                                    &&
           old_ind->sys_info_no_change_valid                                    &&
           ( new_ind->sys_info_no_change != old_ind->sys_info_no_change )
          ) || QMI_NAS_XOR( new_ind->sys_info_no_change_valid, old_ind->sys_info_no_change_valid )  )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_IS_SYS_FORBIDDEN )
  {
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( 
        #ifdef FEATURE_MMODE_3GPP2
         ( new_ind->cdma_sys_info_valid                         &&
           old_ind->cdma_sys_info_valid                         &&
           new_ind->cdma_sys_info.common_sys_info.is_sys_forbidden != old_ind->cdma_sys_info.common_sys_info.is_sys_forbidden ) ||
         ( new_ind->hdr_sys_info_valid                          &&
           old_ind->hdr_sys_info_valid                          &&
           new_ind->hdr_sys_info.common_sys_info.is_sys_forbidden != old_ind->hdr_sys_info.common_sys_info.is_sys_forbidden ) ||
         #endif
         ( new_ind->gsm_sys_info_valid                          &&
           old_ind->gsm_sys_info_valid                          &&
           new_ind->gsm_sys_info.common_sys_info.is_sys_forbidden != old_ind->gsm_sys_info.common_sys_info.is_sys_forbidden ) ||
         #ifdef FEATURE_WCDMA
         ( new_ind->wcdma_sys_info_valid                        &&
           old_ind->wcdma_sys_info_valid                        &&
           new_ind->wcdma_sys_info.common_sys_info.is_sys_forbidden != old_ind->wcdma_sys_info.common_sys_info.is_sys_forbidden ) ||
         #endif
         ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.common_sys_info.is_sys_forbidden != old_ind->lte_sys_info.common_sys_info.is_sys_forbidden ) ||
         #ifdef FEATURE_TDSCDMA
         ( new_ind->tdscdma_sys_info_valid                      &&
           old_ind->tdscdma_sys_info_valid                      &&
           new_ind->tdscdma_sys_info.common_sys_info.is_sys_forbidden != old_ind->tdscdma_sys_info.common_sys_info.is_sys_forbidden ) ||
         #endif
         #ifdef FEATURE_MMODE_3GPP2
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid   , old_ind->cdma_sys_info_valid    )                                  ||
         QMI_NAS_XOR( new_ind->hdr_sys_info_valid    , old_ind->hdr_sys_info_valid     )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->gsm_sys_info_valid    , old_ind->gsm_sys_info_valid     )                                  ||
         #ifdef FEATURE_WCDMA
         QMI_NAS_XOR( new_ind->wcdma_sys_info_valid  , old_ind->wcdma_sys_info_valid   )                                  ||
         #endif
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )                                  
         #ifdef FEATURE_TDSCDMA
         ||
         QMI_NAS_XOR( new_ind->tdscdma_sys_info_valid, old_ind->tdscdma_sys_info_valid )      
         #endif
         )
    #else
    if ( ( new_ind->lte_sys_info_valid                          &&
           old_ind->lte_sys_info_valid                          &&
           new_ind->lte_sys_info.common_sys_info.is_sys_forbidden != old_ind->lte_sys_info.common_sys_info.is_sys_forbidden ) ||
         QMI_NAS_XOR( new_ind->lte_sys_info_valid    , old_ind->lte_sys_info_valid     )            )
    #endif
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_LTE_EMBMS_COVERAGE )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->lte_embms_coverage_valid                        &&
           old_ind->lte_embms_coverage_valid                        &&
           new_ind->lte_embms_coverage != old_ind->lte_embms_coverage ) ||
         ( new_ind->embms_coverage_status_valid                      &&
           old_ind->embms_coverage_status_valid                      &&
           new_ind->embms_coverage_status != old_ind->embms_coverage_status ) ||
         QMI_NAS_XOR( new_ind->lte_embms_coverage_valid  , old_ind->lte_embms_coverage_valid   )  ||
         QMI_NAS_XOR( new_ind->embms_coverage_status_valid, old_ind->embms_coverage_status_valid )    )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_LTE_VOICE_STATUS )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->lte_voice_status_valid                                    &&
           old_ind->lte_voice_status_valid                                    &&
           ( new_ind->lte_voice_status != old_ind->lte_voice_status )
          ) || QMI_NAS_XOR( new_ind->lte_voice_status_valid, old_ind->lte_voice_status_valid )  )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_LTE_SMS_STATUS )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->lte_sms_status_valid                                    &&
           old_ind->lte_sms_status_valid                                    &&
           ( new_ind->lte_sms_status != old_ind->lte_sms_status )
          ) || QMI_NAS_XOR( new_ind->lte_sms_status_valid, old_ind->lte_sms_status_valid )  )
    {
      retval = TRUE;
    }
  }

  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
  if( sys_info_limits & QMI_NAS_LIMIT_BY_IS_SYS_PRL_MATCH )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->cdma_sys_info_valid                        &&
           old_ind->cdma_sys_info_valid                        &&
           new_ind->cdma_sys_info.cdma_hdr_only_sys_info.is_sys_prl_match != old_ind->cdma_sys_info.cdma_hdr_only_sys_info.is_sys_prl_match ) ||
         ( new_ind->hdr_sys_info_valid                      &&
           old_ind->hdr_sys_info_valid                      &&
           new_ind->hdr_sys_info.cdma_hdr_only_sys_info.is_sys_prl_match != old_ind->hdr_sys_info.cdma_hdr_only_sys_info.is_sys_prl_match ) ||
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid  , old_ind->cdma_sys_info_valid   )  ||
         QMI_NAS_XOR( new_ind->hdr_sys_info_valid, old_ind->hdr_sys_info_valid )    )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_P_REV_IN_USE )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->cdma_sys_info_valid                                    &&
           old_ind->cdma_sys_info_valid                                    &&
           ( new_ind->cdma_sys_info.cdma_specific_sys_info.p_rev_in_use != old_ind->cdma_sys_info.cdma_specific_sys_info.p_rev_in_use )
          ) || QMI_NAS_XOR( new_ind->cdma_sys_info_valid, old_ind->cdma_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_CDMA_SYS_ID )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->cdma_sys_info_valid                                    &&
           old_ind->cdma_sys_info_valid                                    &&
           ( new_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.sid != old_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.sid ||
             new_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.nid != old_ind->cdma_sys_info.cdma_specific_sys_info.cdma_sys_id.nid  )  ) ||
         QMI_NAS_XOR( new_ind->cdma_sys_info_valid, old_ind->cdma_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }
  #endif

  #if defined(FEATURE_WCDMA)
  if( sys_info_limits & QMI_NAS_LIMIT_BY_PSC )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->wcdma_sys_info_valid                                    &&
           old_ind->wcdma_sys_info_valid                                    &&
           ( new_ind->wcdma_sys_info.wcdma_specific_sys_info.psc != old_ind->wcdma_sys_info.wcdma_specific_sys_info.psc )
          ) || QMI_NAS_XOR( new_ind->wcdma_sys_info_valid, old_ind->wcdma_sys_info_valid )  )
    {
      retval = TRUE;
    }
  }
  #endif

  if( sys_info_limits & QMI_NAS_LIMIT_BY_SIM_REJ_INFO )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->sim_rej_info_valid                                    &&
           old_ind->sim_rej_info_valid                                    &&
           ( new_ind->sim_rej_info != old_ind->sim_rej_info )
          ) || QMI_NAS_XOR( new_ind->sim_rej_info_valid, old_ind->sim_rej_info_valid )  )
    {
      retval = TRUE;
    }
  }

  #if defined(FEATURE_WCDMA)
  if( sys_info_limits & QMI_NAS_LIMIT_BY_WCDMA_EUTRA_STATUS )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->wcdma_eutra_status_valid                                    &&
           old_ind->wcdma_eutra_status_valid                                    &&
           ( new_ind->wcdma_eutra_status != old_ind->wcdma_eutra_status )
          ) || QMI_NAS_XOR( new_ind->wcdma_eutra_status_valid, old_ind->wcdma_eutra_status_valid )  )
    {
      retval = TRUE;
    }
  }
  #endif

  #ifdef FEATURE_FEMTO_CSG
  #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))

  if( sys_info_limits & QMI_NAS_LIMIT_BY_WCDMA_CSG_INFO )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->wcdma_csg_info_valid                                    &&
           old_ind->wcdma_csg_info_valid                                    &&
           ( new_ind->wcdma_csg_info.id != old_ind->wcdma_csg_info.id ||
             new_ind->wcdma_csg_info.name_len != old_ind->wcdma_csg_info.name_len ||
             memcmp(new_ind->wcdma_csg_info.name, 
                    old_ind->wcdma_csg_info.name,
                    sizeof(old_ind->wcdma_csg_info.name)))
          ) || QMI_NAS_XOR( new_ind->wcdma_csg_info_valid, old_ind->wcdma_csg_info_valid )  )
    {
      retval = TRUE;
    }
  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_LTE_CSG_INFO )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if ( ( new_ind->lte_csg_info_valid                                    &&
           old_ind->lte_csg_info_valid                                    &&
           ( new_ind->lte_csg_info.id != old_ind->lte_csg_info.id ||
             new_ind->lte_csg_info.name_len != old_ind->lte_csg_info.name_len ||
             memcmp(new_ind->lte_csg_info.name, 
                    old_ind->lte_csg_info.name,
                    sizeof(old_ind->lte_csg_info.name)))
          ) || QMI_NAS_XOR( new_ind->lte_csg_info_valid, old_ind->lte_csg_info_valid )  )
    {
      retval = TRUE;
    }
  }
  #endif
  #endif

  //QMI_NAS_LIMIT_BY_SRV_STATUS_TOGGLE & QMI_NAS_LIMIT_BY_SRV_RAT_TECH_CHANGE
  {
    boolean is_prev_service_avail = FALSE;
    boolean is_new_service_avail  = FALSE;
    boolean is_prev_service_3gpp2 = FALSE;
    boolean is_new_service_3gpp2  = FALSE;
    boolean is_prev_service_3gpp = FALSE;
    boolean is_new_service_3gpp  = FALSE;

   #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
    if( (old_ind->cdma_srv_status_info_valid &&  (old_ind->cdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) ||
         (old_ind->hdr_srv_status_info_valid &&  (old_ind->hdr_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) 
      )
    { //Is service available on any RAT?
      is_prev_service_avail = TRUE;
      is_prev_service_3gpp2 = TRUE;
    }
    #endif
    
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    if( (old_ind->gsm_srv_status_info_valid &&  (old_ind->gsm_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) ||
        #ifdef FEATURE_WCDMA
         (old_ind->wcdma_srv_status_info_valid &&  (old_ind->wcdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) ||
         #endif
         (old_ind->lte_srv_status_info_valid &&  (old_ind->lte_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) 
         #ifdef FEATURE_TDSCDMA
         ||
         (old_ind->tdscdma_srv_status_info_valid &&  (old_ind->tdscdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) 
         #endif
      )
    #else
    if( (old_ind->lte_srv_status_info_valid &&  (old_ind->lte_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01))
      )
    #endif
    { //Is service available on any RAT?
      is_prev_service_avail = TRUE;
      is_prev_service_3gpp  = TRUE;
    }
    
    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
    if( (new_ind->cdma_srv_status_info_valid &&  (new_ind->cdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) ||
         (new_ind->hdr_srv_status_info_valid &&  (new_ind->hdr_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01))
      )
    { //Is service available on any RAT?
      is_new_service_avail = TRUE;
      is_new_service_3gpp2 = TRUE;
    }
    #endif
    
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    if( (new_ind->gsm_srv_status_info_valid &&  (new_ind->gsm_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) ||
        #ifdef FEATURE_WCDMA
         (new_ind->wcdma_srv_status_info_valid &&  (new_ind->wcdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) ||
         #endif
         (new_ind->lte_srv_status_info_valid &&  (new_ind->lte_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) 
         #ifdef FEATURE_TDSCDMA
         ||
         (new_ind->tdscdma_srv_status_info_valid &&  (new_ind->tdscdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) 
         #endif
      )
    #else
    if( (new_ind->lte_srv_status_info_valid &&  (new_ind->lte_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)) 
      )
    #endif
    { //Is service available on any RAT?
      is_new_service_avail = TRUE;
      is_new_service_3gpp  = TRUE;
    }
    QM_MSG_HIGH_2("service_avail(prev:%d, now:%d), 3gpp2", is_prev_service_avail, is_new_service_avail);
    if(( sys_info_limits & QMI_NAS_LIMIT_BY_SRV_STATUS_TOGGLE )&& 
        QMI_NAS_XOR(is_prev_service_avail, is_new_service_avail))
    {
      retval = TRUE;
    }

     QM_MSG_HIGH_4("3gpp2 srv (prev:%d, now:%d), 3gpp (prev:%d, now:%d)",
                   is_prev_service_3gpp2, is_new_service_3gpp2,
                   is_prev_service_3gpp, is_new_service_3gpp);
    //1. 3gpp2 full service => 3gpp full service Or
    //2. 3gpp full service => 3gpp2 full service
    if(( sys_info_limits & QMI_NAS_LIMIT_BY_SRV_RAT_TECH_CHANGE )&& 
       ( ( is_prev_service_3gpp2 && !is_new_service_3gpp2 && is_new_service_3gpp ) ||
         ( !is_prev_service_3gpp2 && is_prev_service_3gpp && is_new_service_3gpp2 ) )
      )
    {
      retval = TRUE;
    }

  }

  if( sys_info_limits & QMI_NAS_LIMIT_BY_CIOT_LTE_CAP )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if (
         ( new_ind->nas_supp_ciot_optimizations_valid                          &&
           old_ind->nas_supp_ciot_optimizations_valid                          &&
           ( new_ind->nas_supp_ciot_optimizations.cp_ciot_supported != old_ind->nas_supp_ciot_optimizations.cp_ciot_supported ||
             new_ind->nas_supp_ciot_optimizations.up_ciot_supported != old_ind->nas_supp_ciot_optimizations.up_ciot_supported 
           )
         ) ||
         QMI_NAS_XOR( new_ind->nas_supp_ciot_optimizations_valid    , old_ind->nas_supp_ciot_optimizations_valid  )
    )
    {
      retval = TRUE;
    }
  }
  
  if( sys_info_limits & QMI_NAS_LIMIT_BY_CAMPED_CIOT_LTE_OP_MODE )
  {
    // Paranthesis required around && inside || to prevent compiler warning, they're unnecessary.  Sigh...
    if(
         ( new_ind->nas_camped_ciot_lte_op_mode_valid                          &&
           old_ind->nas_camped_ciot_lte_op_mode_valid                          &&
           ( new_ind->nas_camped_ciot_lte_op_mode     != old_ind->nas_camped_ciot_lte_op_mode )) ||
         QMI_NAS_XOR( new_ind->nas_camped_ciot_lte_op_mode_valid    , old_ind->nas_camped_ciot_lte_op_mode_valid  )
      )
    {
      retval = TRUE;
    }
  }


  return retval;
} // END_FUNCTION QMI_NAS_CHECK_SYS_INFO_CHG_AGAINST_RPT_LIMITS()

/*===========================================================================
  FUNCTION qmi_nas_rat_cm_scan_to_mmgsdi()

  DESCRIPTION
    Translate CM Scan Result RAT to MMGSDI RAT enum type

  PARAMETERS
    cm_rat: CM Radio access tech
===========================================================================*/
mmgsdi_rat_enum_type qmi_nas_rat_cm_scan_to_mmgsdi( sys_radio_access_tech_e_type cm_rat )
{
  mmgsdi_rat_enum_type mmgsdi_val = MMGSDI_RAT_NONE;
  
  switch ( cm_rat )
  {
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    case SYS_RAT_GSM_RADIO_ACCESS:  mmgsdi_val = MMGSDI_RAT_GSM;  break;
    #ifdef FEATURE_WCDMA
    case SYS_RAT_UMTS_RADIO_ACCESS: mmgsdi_val = MMGSDI_RAT_UMTS; break;
    #endif
    case SYS_RAT_LTE_RADIO_ACCESS:  mmgsdi_val = MMGSDI_RAT_LTE;  break;
    #ifdef FEATURE_TDSCDMA
    case SYS_RAT_TDS_RADIO_ACCESS:  mmgsdi_val = MMGSDI_RAT_TDS;  break;
    #endif
    #else
    case SYS_RAT_LTE_RADIO_ACCESS:  mmgsdi_val = MMGSDI_RAT_LTE;  break;
    #endif
    default:
      QM_MSG_HIGH_1("unknown rat %d", cm_rat);
      break;
  }

  return mmgsdi_val;
}

/*===========================================================================
  FUNCTION qmi_nas_rat_cm_mode_to_mmgsdi()

  DESCRIPTION
    Translate CM sys mode RAT to MMGSDI RAT enum type

  PARAMETERS
    cm_rat: CM Radio access tech
===========================================================================*/
mmgsdi_rat_enum_type qmi_nas_rat_cm_mode_to_mmgsdi( sys_sys_mode_e_type cm_rat )
{
  mmgsdi_rat_enum_type mmgsdi_val = MMGSDI_RAT_NONE;
  
  switch ( cm_rat )
  {
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    case SYS_SYS_MODE_GSM:   mmgsdi_val = MMGSDI_RAT_GSM;  break;
    #ifdef FEATURE_WCDMA
    case SYS_SYS_MODE_WCDMA: mmgsdi_val = MMGSDI_RAT_UMTS; break;
    #endif
    case SYS_SYS_MODE_LTE:   mmgsdi_val = MMGSDI_RAT_LTE;  break;
    #ifdef FEATURE_TDSCDMA
    case SYS_SYS_MODE_TDS:   mmgsdi_val = MMGSDI_RAT_TDS;  break;
    #endif
    #else
    case SYS_SYS_MODE_LTE:   mmgsdi_val = MMGSDI_RAT_LTE;  break;
    #endif
    default:
      QM_MSG_HIGH_1("unknown rat %d", cm_rat);
      break;
  }

  return mmgsdi_val;
}

/*===========================================================================
  FUNCTION qmi_nas_radio_if_2_sys_mode()

  DESCRIPTION
    Converts nas_radio_if_enum_v01 type to sys_sys_mode_e_type
 
  PARAMETERS
    radio_if: RAT to convert
 
  RETURN VALUE
    corresponding sys_sys_mode_e_type
===========================================================================*/
sys_sys_mode_e_type qmi_nas_radio_if_2_sys_mode( nas_radio_if_enum_v01 radio_if )
{
  sys_sys_mode_e_type cm_val;
  
  switch ( radio_if )
  {
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    #ifdef FEATURE_MMODE_3GPP2
    case NAS_RADIO_IF_CDMA_1XEVDO_V01: cm_val = SYS_SYS_MODE_HDR;   break;
    case NAS_RADIO_IF_CDMA_1X_V01:     cm_val = SYS_SYS_MODE_CDMA;  break;
    #endif
    case NAS_RADIO_IF_LTE_V01:         cm_val = SYS_SYS_MODE_LTE;   break;
    case NAS_RADIO_IF_GSM_V01:         cm_val = SYS_SYS_MODE_GSM;   break;
    #ifdef FEATURE_WCDMA
    case NAS_RADIO_IF_UMTS_V01:        cm_val = SYS_SYS_MODE_WCDMA; break;
    #endif
    #ifdef FEATURE_TDSCDMA
    case NAS_RADIO_IF_TDSCDMA_V01:     cm_val = SYS_SYS_MODE_TDS;   break;
    #endif
    #else
    case NAS_RADIO_IF_LTE_V01:         cm_val = SYS_SYS_MODE_LTE;   break;
    #endif
    default:
      QM_MSG_HIGH_1("unknown radio if %d", radio_if);
      cm_val = SYS_SYS_MODE_NO_SRV;
      break;
  }

  return cm_val;
}

/*===========================================================================
  FUNCTION qmi_nas_convert_pref_to_mask()

  DESCRIPTION
    Converts cm ims preference to qmi preference mask
 
  PARAMETERS
    old_mask: old mask value to add on to
    call_type_mask: call type for preference
    is_ims_pref: whether IMS is preferred
===========================================================================*/
uint64 qmi_nas_convert_pref_to_mask( uint64 old_mask, cm_call_type_mask_type call_type_mask, cm_call_type_mask_type ims_pref_mask )
{
  uint64 mask = old_mask;

  if ( call_type_mask & CM_CALL_TYPE_VOICE_MASK )
  {
    // save SMS bit, and set VOICE bit based on ims pref mask
    mask = (mask & NAS_CALL_TYPE_B_SMS_V01) | 
             ((ims_pref_mask & CM_CALL_TYPE_VOICE_MASK) ? NAS_CALL_TYPE_B_VOICE_V01 : 0);
  }

  if ( call_type_mask & CM_CALL_TYPE_SMS_MASK )
  {
    // save VOICE bit, and set SMS bit based on ims pref mask
    mask = (mask & NAS_CALL_TYPE_B_VOICE_V01) | 
             ((ims_pref_mask & CM_CALL_TYPE_SMS_MASK) ? NAS_CALL_TYPE_B_SMS_V01 : 0);
  }

  return mask;
}

/*===========================================================================
  FUNCTION qmi_nas_map_voice_sms_status_cm_to_qmi()

  DESCRIPTION
    Maps CM domain status to QMI_NAS enum type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_voice_sms_status_e_type qmi_nas_map_voice_sms_status_cm_to_qmi( sys_cm_domain_sel_domain_e_type domain )
{
  nas_voice_sms_status_e_type retval = NAS_VOICE_SMS_STATUS_NONE;

  switch( domain )
  {
    case SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE: retval = NAS_VOICE_SMS_STATUS_NONE;   break;
    case SYS_CM_DOMAIN_SEL_DOMAIN_NO_SMS:   retval = NAS_VOICE_SMS_STATUS_NONE;   break;
    case SYS_CM_DOMAIN_SEL_DOMAIN_IMS     : retval = NAS_VOICE_SMS_STATUS_IMS;    break; 
    #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
    case SYS_CM_DOMAIN_SEL_DOMAIN_1X      : retval = NAS_VOICE_SMS_STATUS_1X;     break;
    case SYS_CM_DOMAIN_SEL_DOMAIN_3GPP    : retval = NAS_VOICE_SMS_STATUS_3GPP;   break;
    #endif

    default:
      retval = NAS_VOICE_SMS_STATUS_NONE;
      break;
  }

  return retval;
}

/*===========================================================================
  FUNCTION qmi_nas_update_global_ims_pref()

  DESCRIPTION
    Update global ims preference information based on SRV_CHANGED event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_update_global_ims_pref( cm_mm_ss_info_s_type *p_ss_info )
{
  boolean no_lte_full_srv_on_stack1 = FALSE;
#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  boolean no_lte_full_srv_on_stack2 = FALSE;
#endif
  if( p_ss_info->sys_mode != SYS_SYS_MODE_LTE || p_ss_info->srv_status != SYS_SRV_STATUS_SRV ||
       ( p_ss_info->srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->srv_domain != SYS_SRV_DOMAIN_CAMPED ) )
  {
     no_lte_full_srv_on_stack1 = TRUE;
  }
  #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE)) && defined(FEATURE_HDR)
  if( !(p_ss_info->number_of_stacks >1) ||  !p_ss_info->stack_info[STACK_INDEX_1].is_operational || 
      p_ss_info->stack_info[STACK_INDEX_1].sys_mode != SYS_SYS_MODE_LTE || p_ss_info->stack_info[STACK_INDEX_1].srv_status != SYS_SRV_STATUS_SRV ||
      ( p_ss_info->stack_info[STACK_INDEX_1].srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->stack_info[STACK_INDEX_1].srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CAMPED ) 
    ) 
  {
     no_lte_full_srv_on_stack2 = TRUE;
  }
  #endif
  //No LTE full PS service on both the stacks
  if( no_lte_full_srv_on_stack1 
      #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
      && no_lte_full_srv_on_stack2 
      #endif
      )
  {
    qmi_nasi_global.ims_pref_cache.lte_mask_valid = FALSE;
    qmi_nasi_global.ims_pref_cache.lte_call_mask = 0;
  }

  #ifndef FEATURE_MMODE_QMI_LTE_ONLY
  #if !defined(FEATURE_MMODE_QMI_GPRS_LTE) && defined(FEATURE_HDR)
  if ( ( p_ss_info->number_of_stacks == 1 && 
         ( p_ss_info->stack_info[STACK_INDEX_0].sys_mode != SYS_SYS_MODE_HDR || p_ss_info->stack_info[STACK_INDEX_0].srv_status != SYS_SRV_STATUS_SRV || 
           ( p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CAMPED ) ) ) || 
       ( p_ss_info->number_of_stacks > 1 && 
         ( p_ss_info->stack_info[STACK_INDEX_1].sys_mode != SYS_SYS_MODE_HDR || p_ss_info->stack_info[STACK_INDEX_1].srv_status != SYS_SRV_STATUS_SRV ||
           ( p_ss_info->stack_info[STACK_INDEX_1].srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->stack_info[STACK_INDEX_1].srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CAMPED ) ) ) ) 
  {
    qmi_nasi_global.ims_pref_cache.hdr_mask_valid = FALSE;
    qmi_nasi_global.ims_pref_cache.hdr_call_mask = 0;
  }
  #endif
#if ( defined(FEATURE_LTE) && defined(FEATURE_GSM))
  if ( p_ss_info->sys_mode != SYS_SYS_MODE_GSM || p_ss_info->srv_status != SYS_SRV_STATUS_SRV ||
       ( p_ss_info->srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->srv_domain != SYS_SRV_DOMAIN_CAMPED ) )
  {
    qmi_nasi_global.ims_pref_cache.gsm_mask_valid = FALSE;
    qmi_nasi_global.ims_pref_cache.gsm_call_mask = 0;
  }
#endif
  #ifdef FEATURE_WCDMA
  if ( p_ss_info->sys_mode != SYS_SYS_MODE_WCDMA || p_ss_info->srv_status != SYS_SRV_STATUS_SRV ||
       ( p_ss_info->srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->srv_domain != SYS_SRV_DOMAIN_CAMPED ) )
  {
    qmi_nasi_global.ims_pref_cache.wcdma_mask_valid = FALSE;
    qmi_nasi_global.ims_pref_cache.wcdma_call_mask = 0;
  }
  #endif

  #ifdef FEATURE_MMODE_3GPP2
  if ( p_ss_info->stack_info[STACK_INDEX_0].sys_mode != SYS_SYS_MODE_CDMA || p_ss_info->stack_info[STACK_INDEX_0].srv_status != SYS_SRV_STATUS_SRV ||
       ( p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CAMPED ) )
  {
    qmi_nasi_global.ims_pref_cache.cdma_mask_valid = FALSE;
    qmi_nasi_global.ims_pref_cache.cdma_call_mask = 0;
  }
  #endif

  #ifdef FEATURE_TDSCDMA
  if ( p_ss_info->stack_info[STACK_INDEX_0].sys_mode != SYS_SYS_MODE_TDS || p_ss_info->stack_info[STACK_INDEX_0].srv_status != SYS_SRV_STATUS_SRV ||
       ( p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CS_PS && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_PS_ONLY && p_ss_info->stack_info[STACK_INDEX_0].srv_domain != SYS_SRV_DOMAIN_CAMPED ) )
  {
    qmi_nasi_global.ims_pref_cache.tds_mask_valid = FALSE;
    qmi_nasi_global.ims_pref_cache.tds_call_mask = 0;
  }
  #endif
  #endif
}

/*===========================================================================
  FUNCTION qmi_nas_qmi_lte_bc_2_sys_lte_bc()

  DESCRIPTION
    convert nas_active_band_enum_v01 to sys_sband_lte_e_type 
    
  PARAMETERS
    qmi band class enum

  RETURN VALUE
    corresponding sys band class enum value or;
    SYS_SBAND_LTE_EUTRAN_BAND_MAX if error
===========================================================================*/
sys_sband_lte_e_type qmi_nas_qmi_lte_bc_2_sys_lte_bc( nas_active_band_enum_v01 qmi_lte_bc )
{

  sys_sband_lte_e_type sys_bc = SYS_SBAND_LTE_EUTRAN_BAND_MAX;
  
  switch ( qmi_lte_bc )
  {
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_1_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND1;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_2_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND2;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_3_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND3;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_4_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND4;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_5_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND5;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_6_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND6;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_7_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND7;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_8_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND8;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_9_V01:  sys_bc = SYS_SBAND_LTE_EUTRAN_BAND9;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_10_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND10;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_11_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND11;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_12_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND12;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_13_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND13;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_14_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND14;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_17_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND17;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_18_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND18;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_19_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND19;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_20_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND20;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_21_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND21;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_23_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND23;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_24_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND24;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_25_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND25;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_26_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND26;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_27_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND27;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_33_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND33;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_34_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND34;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_35_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND35;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_36_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND36;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_37_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND37;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_38_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND38;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_39_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND39;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND40;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_41_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND41;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_42_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND42;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_43_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND43;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_66_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND66;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_71_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND71;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_85_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND85;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_86_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND86;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_87_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND87;  break;
    case NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_88_V01: sys_bc = SYS_SBAND_LTE_EUTRAN_BAND88;  break;
    default:
      QM_MSG_ERROR_1("invalid qmi bc %d", qmi_lte_bc);
      break;
  }

  return sys_bc;
}

/*===========================================================================
  FUNCTION qmi_nas_qmi_sys_lte_bc_2_lte_bc()

  DESCRIPTION
    convert sys_sband_lte_e_type to nas_active_band_enum_v01
    
  PARAMETERS
    sys sband enum

  RETURN VALUE
    corresponding sys band class enum value or;
    NAS_ACTIVE_BAND_ENUM_MAX_ENUM_VAL_V01 if error
===========================================================================*/
nas_active_band_enum_v01  qmi_nas_qmi_sys_lte_bc_2_lte_bc(sys_sband_lte_e_type  sys_lte_bc )
{

  nas_active_band_enum_v01 nas_lte_bc = NAS_ACTIVE_BAND_ENUM_MAX_ENUM_VAL_V01;
  
  switch ( sys_lte_bc )
  {
    case SYS_SBAND_LTE_EUTRAN_BAND1 :  nas_lte_bc = NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_1_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND2 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_2_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND3 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_3_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND4 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_4_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND5 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_5_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND6 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_6_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND7 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_7_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND8 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_8_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND9 :  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_9_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND10:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_10_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND11:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_11_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND12:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_12_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND13:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_13_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND14:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_14_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND17:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_17_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND18:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_18_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND19:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_19_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND20:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_20_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND21:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_21_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND23:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_23_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND24:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_24_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND25:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_25_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND26:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_26_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND27:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_27_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND33:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_33_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND34:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_34_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND35:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_35_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND36:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_36_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND37:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_37_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND38:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_38_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND39:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_39_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND40:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_40_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND41:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_41_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND42:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_42_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND43:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_43_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND66:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_66_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND71:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_71_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND85:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_85_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND86:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_86_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND87:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_87_V01;  break;
    case SYS_SBAND_LTE_EUTRAN_BAND88:  nas_lte_bc =  NAS_ACTIVE_BAND_E_UTRA_OPERATING_BAND_88_V01;  break;
    default:
      QM_MSG_ERROR_1("invalid qmi bc %d", sys_lte_bc);
      break;
  }

  QM_MSG_MED_2("Recieved sys_lte_bc =%d, mapped nas_lte_bc= %d", sys_lte_bc, nas_lte_bc);

  return nas_lte_bc;
}

/*===========================================================================
  FUNCTION qmi_nas_map_display_bit_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI display bit value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_tri_state_boolean_type_v01 qmi_nas_map_display_bit_mmgsdi_to_qmi( mmgsdi_eons_rplmn_display_bit_enum_type display_bit )
{
  nas_tri_state_boolean_type_v01 qmi_disp_bit = NAS_TRI_UNKNOWN_V01;
  QM_MSG_MED_1("Translating mmgsdi display bit %d", display_bit);

  switch ( display_bit )
  {
    case MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_0:
      qmi_disp_bit = NAS_TRI_FALSE_V01;
      break;
      
    case MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_1:
      qmi_disp_bit = NAS_TRI_TRUE_V01;
      break;

    default:
      qmi_disp_bit = NAS_TRI_UNKNOWN_V01;
      break;
  }

  return qmi_disp_bit;
}

/*===========================================================================
  FUNCTION qmi_nas_map_home_status_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI home status value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_tri_state_boolean_type_v01 qmi_nas_map_home_status_mmgsdi_to_qmi( mmgsdi_eons_roaming_status_enum_type home_status )
{
  nas_tri_state_boolean_type_v01 qmi_home = NAS_TRI_UNKNOWN_V01;
  QM_MSG_MED_1("Translating mmgsdi home status %d", home_status);

  switch ( home_status )
  {
    case MMGSDI_EONS_IN_HOME_NW:
      qmi_home = NAS_TRI_TRUE_V01;
      break;
      
    case MMGSDI_EONS_IN_ROAMING_NW:
      qmi_home = NAS_TRI_FALSE_V01;
      break;

    default:
      qmi_home = NAS_TRI_UNKNOWN_V01;
      break;
  }

  return qmi_home;
}

/*===========================================================================
  FUNCTION qmi_nas_map_lang_id_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI language ID value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_lang_id_enum_v01 qmi_nas_map_lang_id_mmgsdi_to_qmi( mmgsdi_plmn_lang_type plmn_lang )
{
  nas_lang_id_enum_v01 lang_id = NAS_LANG_ID_UNKNOWN_V01;

  if ( !strncmp( "zh_hant", (char *)plmn_lang.lang_id, plmn_lang.lang_len ) )
  {
    lang_id = NAS_LANG_ID_ZH_TRAD_V01;
  }
  else if ( !strncmp( "zh_hans", (char *)plmn_lang.lang_id, plmn_lang.lang_len ) )
  {
    lang_id = NAS_LANG_ID_ZH_SIMP_V01;
  }

  return lang_id;
}

/*===========================================================================
  FUNCTION qmi_nas_map_mmgsdi_session_to_cm_asubs()

  DESCRIPTION
    Map MMGSDI session to CM asubs id

  RETURN VALUE
    asubs id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sys_modem_as_id_e_type qmi_nas_map_mmgsdi_session_to_cm_asubs( mmgsdi_session_type_enum_type session )
{
  sys_modem_as_id_e_type asubs_id = SYS_MODEM_AS_ID_NONE;

  if( session == MMGSDI_GW_PROV_PRI_SESSION 
    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
    || session == MMGSDI_1X_PROV_PRI_SESSION 
    #endif
    )
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  #ifdef FEATURE_DUAL_SIM
  else if( session == MMGSDI_GW_PROV_SEC_SESSION || session == MMGSDI_1X_PROV_SEC_SESSION )
  {
    asubs_id = SYS_MODEM_AS_ID_2;
  }
  #endif

  return asubs_id;
}

/*===========================================================================
  FUNCTION qmi_nas_map_cm_asubs_to_mmgsdi_session()

  DESCRIPTION
    Map CM asubs id to MMGSDI session

  RETURN VALUE
    asubs id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_session_type_enum_type qmi_nas_map_cm_asubs_to_mmgsdi_session( sys_modem_as_id_e_type asubs_id )
{
  mmgsdi_session_type_enum_type session = MMGSDI_GW_PROV_PRI_SESSION;

  if ( asubs_id == SYS_MODEM_AS_ID_1 )
  {
    session = MMGSDI_GW_PROV_PRI_SESSION;
  }
  #ifdef FEATURE_DUAL_SIM
  else if ( asubs_id == SYS_MODEM_AS_ID_2 )
  {
    session = MMGSDI_GW_PROV_SEC_SESSION;
  }
  #endif

  return session;
}

/*===========================================================================
  FUNCTION qmi_nas_sys_rat_2_radio_if()

  DESCRIPTION
    convert radio access tech enum to radio_if enum
    
  PARAMETERS
    

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_rat_2_radio_if( sys_radio_access_tech_e_type rat )
{
  enum qmi_nas_radio_if_e status = QMI_NAS_RADIO_IF_NONE;

  switch ( rat )
  {
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    case SYS_RAT_GSM_RADIO_ACCESS:   status = QMI_NAS_RADIO_IF_GSM;            break;
    #ifdef FEATURE_WCDMA
    case SYS_RAT_UMTS_RADIO_ACCESS:  status = QMI_NAS_RADIO_IF_UMTS;           break;
    #endif
    case SYS_RAT_LTE_RADIO_ACCESS:   status = QMI_NAS_RADIO_IF_LTE;            break;
    #ifdef FEATURE_TDSCDMA
    case SYS_RAT_TDS_RADIO_ACCESS:   status = QMI_NAS_RADIO_IF_TDSCDMA;        break;
    #endif
    #else
    case SYS_RAT_LTE_RADIO_ACCESS:   status = QMI_NAS_RADIO_IF_LTE;            break;
    #endif
    default:
     QM_MSG_ERROR_1( "unsupported rat to radio if %d", rat );
     break;
  }

  return status;
}

/*===========================================================================
  FUNCTION qmi_nas_map_nw_name_source_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI network name source value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_nw_name_source_enum_type_v01 qmi_nas_map_nw_name_source_mmgsdi_to_qmi( mmgsdi_eons_name_source_enum_type nw_name_source )
{
  nas_nw_name_source_enum_type_v01 name_source = NAS_NW_NAME_SOURCE_UNKNOWN_V01;

  switch ( nw_name_source )
  {
    case MMGSDI_EONS_NAME_SOURCE_OPL_PNN:
      name_source = NAS_NW_NAME_SOURCE_OPL_PNN_V01;
      break;

    case MMGSDI_EONS_NAME_SOURCE_CPHS_ONS:
      name_source = NAS_NW_NAME_SOURCE_CPHS_ONS_V01;
      break;

    case MMGSDI_EONS_NAME_SOURCE_NITZ:
      name_source = NAS_NW_NAME_SOURCE_NITZ_V01;
      break;

    case MMGSDI_EONS_NAME_SOURCE_SE13:
      name_source = NAS_NW_NAME_SOURCE_SE13_V01;
      break;

    case MMGSDI_EONS_NAME_SOURCE_MCC_MNC:
      name_source = NAS_NW_NAME_SOURCE_MCC_MNC_V01;
      break;

    default:
      name_source = NAS_NW_NAME_SOURCE_UNKNOWN_V01;
      break;
  }

  QM_MSG_HIGH_2("Translating nw name source %d, QMI nw name source %d",
                nw_name_source, name_source);
  return name_source;
}

#endif // FEATURE_QMI_SERVICE_SPLIT


