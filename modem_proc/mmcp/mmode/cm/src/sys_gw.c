/*===========================================================================

              S Y S T E M   G W   M O D U L E  

DESCRIPTION
  This header file contains functions  that are shared between Call Manager,
  Call Manager clients and the GSM/WCDMA protocol stacks.

Copyright (c) 2002 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
02/20/09   rm      Lint clean post CMI changes
07/01/08   rm      Mainlining of MBMS changes
03/30/07   rp     Initial revision
===========================================================================*/


#include "mmcp_variation.h"
#include "sys.h"
#include "sys_v.h"
/*lint -esym(766,sys_gw.h)*/
#include "sys_gw.h"
/*lint -esym(766,sys_gw_v.h)*/
#include "sys_gw_v.h"
#include "string.h"
#include "cmidbg.h" 
#include "cmi.h"

#ifdef CM_GW_SUPPORTED
#if defined (FEATURE_MBMS)
/*===========================================================================
FUNCTION      sys_gw_mbms_conv_tmgi_to_tmgi_uint64

DESCRIPTION   Given the TMGI in array format, converts it to TMGI in uint64 
              format.

              Index 0 in the array becomes the MSB of the uint64 value.
              For eg:-
              
              Input: tmgi->value[0] = 0x01, tmgi->value[1] = 0x02, ...
              Output: tmgi_uint->tmgi = 0x0102...
 
              
PARAMETERS    tmgi      -- TMGI from Network. TMGI is character array of digits.
              tmgi_uint -- TMGI in uint64 format. 


DEPENDENCIES  NONE

RETURN VALUE  TRUE- if TMGI passed is correct.
              FALSE - if TMGI passed is incorrect.

SIDE EFFECTS  None.
===========================================================================*/

boolean sys_gw_mbms_conv_tmgi_to_tmgi_uint64
(
  sys_gw_mbms_tmgi_s_type *tmgi,
    /* TMGI in array format*/

  sys_gw_mbms_tmgi_uint_s_type *tmgi_uint
    /* Converted TMGI in uint64 format */
)
/*lint -esym(818, tmgi) */
{

  uint64 tmgi_temp = 0 ;      /* Variable to hold 15 digits TMGI*/
  int i=0;


  /*-----------------------------------------------------------------------*/
  /* Input validation */

  if (tmgi == NULL || tmgi_uint == NULL ) 
  {
    CM_ERR_0("mbms_conv_tmgi_to_tmgi_uint64: Null pointer passed  ");
    return FALSE;
  }

  if(tmgi->present == FALSE || tmgi->length > ARR_SIZE(tmgi->value))
  {
    CM_ERR_0("mbms_conv_tmgi_to_tmgi_uint64: Invalid TMGI passed from SDP\n");
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/

  tmgi_temp = 0;

  for (i = 0; i < tmgi->length ; i++ ) 
  {
    tmgi_temp = tmgi_temp << 8;
    tmgi_temp = tmgi->value[i] | tmgi_temp;
  }
  tmgi_uint->present = TRUE;
  tmgi_uint->tmgi = tmgi_temp;

  CM_MSG_HIGH_2("mbms_conv_tmgi_to_tmgi_uint64: Converted TMGI 0x%x%x", 
           QWORD_HIGH(tmgi_uint->tmgi),QWORD_LOW(tmgi_uint->tmgi));
  return TRUE;
}
/*lint +esym(818, tmgi) */

/*===========================================================================
MACRO         sys_gw_mbms_conv_tmgi_uint64_to_tmgi

DESCRIPTION   Given the TMGI in the UINT64 format , converts it to an array 
              of bytes
              MSB of the input is stored in index 0 of the output array. 
              For eg:-

              Input: tmgi_uint->tmgi = 0x0102...
              Output: tmgi->value[0] = 0x01, tmgi->value[1] = 0x02 and so on.

PARAMETERS    tmgi      -- TMGI in uint64 format
              tmgi_uint -- TMGI in array format


DEPENDENCIES  

RETURN VALUE  TRUE- if TMGI passed is correct.
              FALSE - if TMGI passed is incorrect.

SIDE EFFECTS  None.
===========================================================================*/

boolean sys_gw_mbms_conv_tmgi_uint64_to_tmgi
(
  sys_gw_mbms_tmgi_uint_s_type *tmgi_uint,
    /* TMGI in uint64 format */

  sys_gw_mbms_tmgi_s_type *tmgi
    /* TMGI in array format*/
)
/*lint -esym(818, tmgi_uint) */
/*lint -esym(771, tmgi_temp) */
{
  
  int i=0;
  uint64 tmgi_temp = 0;
  uint8  local_tmgi[SYS_GW_MBMS_MAX_TMGI_LENGTH]= {0};
  int    index=0;

  /*-----------------------------------------------------------------------*/
  /* Input validation */
  if (tmgi_uint == NULL || tmgi == NULL) 
  {
    CM_ERR_0("mbms_conv_tmgi_uint64_to_tmgi: NULL pointer passed");
    return FALSE;
  }

  if (! tmgi_uint->present) 
  {
    CM_ERR_0("mbms_conv_tmgi_uint64_to_tmgi: tmgi_uint->present = FALSE");
    return FALSE;
  }
  /*-----------------------------------------------------------------------*/

  tmgi_temp = tmgi_uint->tmgi ;
  tmgi->length  = (uint8)0;
  tmgi->present =  FALSE;

  while (tmgi_temp > 0 && index < (int)ARR_SIZE(local_tmgi)) 
  {
    local_tmgi[index] = (uint8) tmgi_temp & 0xFF;
    tmgi_temp >>= 8;
    index++;
  }

  CM_MSG_HIGH_1("mbms_conv_tmgi_uint64_to_tmgi: Length of TMGI array %d",index);

  if ( index == 0 ) 
  {
    return FALSE;
  }
  for (i=0; i < index; i++ ) 
  {
    tmgi->value[i] = local_tmgi[ ((index - 1) - i) ];
  }
  tmgi->length = (uint8)index ;
  tmgi->present = TRUE;

  return TRUE;  

}
/*lint -esym(818, tmgi_uint) */
/*lint -esym(771, tmgi_temp) */
#endif /*#if defined (FEATURE_MBMS)*/
#endif /* #ifdef CM_GW_SUPPORTED*/
