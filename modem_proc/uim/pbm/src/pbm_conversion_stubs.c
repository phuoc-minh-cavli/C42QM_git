/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          P B M C O N V E R S I O N _ S T U B S.c

GENERAL DESCRIPTION
  This file contains the STUB functions for some of the functions defined in pbmconvers.c

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbm_conversion_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/16   nr      Initial Revision
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "stdlib.h"
#include "pbm.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbm_conversion.h"

/*===========================================================================
FUNCTION pbmconvert_sim_to_string

DESCRIPTION
 This is stub function for pbmconvert_sim_to_string ()

DEPENDENCIES
  None.

RETURN VALUE
  Zero

SIDE EFFECTS
  None.
===========================================================================*/
uint32 pbmconvert_sim_to_string(uint8 *in_str, uint16 in_len,
                                uint16 *out_octets, uint16 out_len)
{
  (void)in_str;
  (void)in_len;
  (void)out_octets;
  (void)out_len;

  return 0;
} /* pbmconvert_sim_to_string */


/*===========================================================================
FUNCTION pbmconvert_pbm8_to_ucs2

DESCRIPTION
  This is stub function for pbmconvert_pbm8_to_ucs2 ()

DEPENDENCIES
  None.

RETURN VALUE
  FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean pbmconvert_pbm8_to_ucs2(const uint8 *pbm8_str,  uint16 pbm8_str_len,
                                uint16 *ucs2_str, uint16 ucs2_str_len)
{
  (void)pbm8_str;
  (void)pbm8_str_len;
  (void)ucs2_str;
  (void)ucs2_str_len;

  return FALSE;
} /* pbmconvert_pbm8_to_ucs2 */


/*===========================================================================
FUNCTION PBM_STRSTR

DESCRIPTION

  This is stub function for pbm_strstr ()

RETURNS
  NULL

SIDE EFFECTS
===========================================================================*/
void *pbm_strstr(const void *s1, const void *s2, boolean case_sensitive,
                 boolean s1_is_wstr, boolean s2_is_wstr)
{
  (void)s1;
  (void)s2;
  (void)case_sensitive;
  (void)s1_is_wstr;
  (void)s2_is_wstr;

  return NULL;
} /* pbm_strstr */


/*===========================================================================
FUNCTION pbmconvert_init_conversion

DESCRIPTION
  This is stub function for pbmconvert_init_conversion ().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void pbmconvert_init_conversion(void)
{
  return;
} /* pbmconvert_init_conversion */


/*===========================================================================
FUNCTION pbmconvert_return_input_string_first_ucs2_char

DESCRIPTION
  This is stub function for pbmconvert_return_input_string_first_ucs2_char ().

DEPENDENCIES
  None.

RETURN VALUE
  Zero

SIDE EFFECTS
  None.
===========================================================================*/
uint16 pbmconvert_return_input_string_first_ucs2_char(
  const void **in_ptr
)
{
  (void)in_ptr;

  return 0;
} /* pbmconvert_return_input_string_first_ucs2_char */

