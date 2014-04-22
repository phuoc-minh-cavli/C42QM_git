/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                P B M _ C O M M O N _ U T I L S.C

GENERAL DESCRIPTION
  This file contains the common utility functions used by PBM.

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbm_common_utils.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/16   ar      Add check for cache variable NULL
12/19/16   ar      Return error if cache is not available
06/23/16   nr      ECC optimizations for IoE devices
05/26/16   sp      Fix KW warning
05/26/16   nr      Logging improvements
03/10/16   nr      Logging improvements
03/10/16   nr      Fix to convert compressed fileds to uncompressed while performing excat match
02/02/16   nr      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "cmutil.h"
#include "uim_msg.h"
#include "pbm.h"
#include "pbmutils.h"
#include "pbmgsdi.h"
#include "stringl/stringl.h"
#include "uim_common_efs.h"
#include "pbm_conversion.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/

uint8 pbm_number_type_to_gsmton[4] = {
0, CM_TON_INTERNATIONAL, CM_TON_NATIONAL, CM_TON_NETWORK_SPECIFIC,
};

char pbm_bcd_to_char_mapping[] = {
'0', '1', '2', '3',
'4', '5', '6', '7',
'8', '9', '*', '#',
/* Time Pause         Wild           Expansion,        Endmark.*/
PBM_TIMEPAUSE_CHAR, PBM_WILD_CHAR,  PBM_EXPANSION_CHAR, '\0'
};

/*===========================================================================
                              STATIC DATA
===========================================================================*/

/* represent other groups like ecc, call-hist ,sdl */
pbm_g_other_field_info_s_type g_ecc_field_info[2] =
{
  /* Emergency Group */
  {
    6,
    {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,                 1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_HARDCODED_ECC,        1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_NV_ECC,               1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_SIM_ECC,              1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_NETWORK_ECC,          1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_ECC_SERVICE_CATEGORY, 1, PBM_BYTE_SIZE,       PBM_FT_BYTE,     FALSE, TRUE}
    }
  },

  /* Hardcoded and NV Emergency Group */
  {
    3,
    {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,                 1, NV_MAX_LTRS,         PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_HARDCODED_ECC,        1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_NV_ECC,               1, NV_MAX_DIAL_DIGITS,  PBM_FT_PHONE,    TRUE,  TRUE}
    }
  },
};

/* The following tables maps the 256 characters of PBM8 to the corresponding
 * unicode characters. */
const CONVERSION_TABLE  pbm8_to_unicode[256] = {
{0x00, 0x0000}, /* Null */
{0x01, 0x0001}, /* Start of heading */
{0x02, 0x0002}, /* Start of text */
{0x03, 0x0003}, /* End of text */
{0x04, 0x0004}, /* End of transmission */
{0x05, 0x0005}, /* Inquiry */
{0x06, 0x0006}, /* ACK */
{0x07, 0x0007}, /* Bell */
{0x08, 0x0008}, /* Backspace */
{0x09, 0x0009}, /* Tab */
{0x0A, 0x000A}, /* Line feed */
{0x0B, 0x000B}, /* Vertical tab */
{0x0C, 0x000C}, /* Form feed */
{0x0D, 0x000D}, /* Carriage return */
{0x0E, 0x000E}, /* Shift out */
{0x0F, 0x000F}, /* Shift in */
{0x10, 0x0010}, /* Data link escape */
{0x11, 0x0011}, /* Device control one */
{0x12, 0x0012}, /* Device control two */
{0x13, 0x0013}, /* Device control three */
{0x14, 0x0014}, /* Device control four */
{0x15, 0x0015}, /* NAK */
{0x16, 0x0016}, /* Synch */
{0x17, 0x0017}, /* End of block */
{0x18, 0x0018}, /* Cancel */
{0x19, 0x0019}, /* End of medium */
{0x1A, 0x001A}, /* Substitute */
{0x1B, 0x001B}, /* ESC */
{0x1C, 0x001C}, /* File separator */
{0x1D, 0x001D}, /* Group separator */
{0x1E, 0x001E}, /* Record separator */
{0x1F, 0x001F}, /* Unit separator */
{0x20, 0x0020}, /* Space */
{0x21, 0x0021}, /* Exclamation mark */
{0x22, 0x0022}, /* Quotation mark */
{0x23, 0x0023}, /* Number sign */
{0x24, 0x0024}, /* Dollar sign */
{0x25, 0x0025}, /* Percent sign */
{0x26, 0x0026}, /* Ampersand */
{0x27, 0x0027}, /* Apostrophe */
{0x28, 0x0028}, /* Left parenthesis */
{0x29, 0x0029}, /* Right parenthesis */
{0x2A, 0x002A}, /* Asterisk */
{0x2B, 0x002B}, /* Plus */
{0x2C, 0x002C}, /* Comma */
{0x2D, 0x002D}, /* Hyphen */
{0x2E, 0x002E}, /* Full stop */
{0x2F, 0x002F}, /* Solidus */
{0x30, 0x0030}, /* Zero */
{0x31, 0x0031}, /* One */
{0x32, 0x0032}, /* Two */
{0x33, 0x0033}, /* Three */
{0x34, 0x0034}, /* Four */
{0x35, 0x0035}, /* Five */
{0x36, 0x0036}, /* Six */
{0x37, 0x0037}, /* Seven */
{0x38, 0x0038}, /* Eight */
{0x39, 0x0039}, /* Nine */
{0x3A, 0x003A}, /* Colon */
{0x3B, 0x003B}, /* Semicolon */
{0x3C, 0x003C}, /* < (Less than) */
{0x3D, 0x003D}, /* = Equals */
{0x3E, 0x003E}, /* > greater than */
{0x3F, 0x003F}, /* Question mark */
{0x40, 0x0040}, /* AT Sign */
{0x41, 0x0041}, /* Latin A */
{0x42, 0x0042}, /* Latin B */
{0x43, 0x0043}, /* Latin C */
{0x44, 0x0044}, /* Latin D */
{0x45, 0x0045}, /* Latin E */
{0x46, 0x0046}, /* Latin F */
{0x47, 0x0047}, /* Latin G */
{0x48, 0x0048}, /* Latin H */
{0x49, 0x0049}, /* Latin I */
{0x4A, 0x004A}, /* Latin J */
{0x4B, 0x004B}, /* Latin K */
{0x4C, 0x004C}, /* Latin L */
{0x4D, 0x004D}, /* Latin M */
{0x4E, 0x004E}, /* Latin N */
{0x4F, 0x004F}, /* Latin O */
{0x50, 0x0050}, /* Latin P */
{0x51, 0x0051}, /* Latin Q */
{0x52, 0x0052}, /* Latin R */
{0x53, 0x0053}, /* Latin S */
{0x54, 0x0054}, /* Latin T */
{0x55, 0x0055}, /* Latin U */
{0x56, 0x0056}, /* Latin V */
{0x57, 0x0057}, /* Latin W */
{0x58, 0x0058}, /* Latin X */
{0x59, 0x0059}, /* Latin Y */
{0x5A, 0x005A}, /* Latin Z */
{0x5B, 0x005B}, /* Left square bracket */
{0x5C, 0x005C}, /* Reverse solidus */
{0x5D, 0x005D}, /* Right square bracket */
{0x5E, 0x005E}, /* Circumflex accent */
{0x5F, 0x005F}, /* Low line */
{0x60, 0x0060}, /* Grave accent */
{0x61, 0x0061}, /* Latin a */
{0x62, 0x0062}, /* Latin b */
{0x63, 0x0063}, /* Latin c */
{0x64, 0x0064}, /* Latin d */
{0x65, 0x0065}, /* Latin e */
{0x66, 0x0066}, /* Latin f */
{0x67, 0x0067}, /* Latin g */
{0x68, 0x0068}, /* Latin h */
{0x69, 0x0069}, /* Latin i */
{0x6A, 0x006A}, /* Latin j */
{0x6B, 0x006B}, /* Latin k */
{0x6C, 0x006C}, /* Latin l */
{0x6D, 0x006D}, /* Latin m */
{0x6E, 0x006E}, /* Latin n */
{0x6F, 0x006F}, /* Latin o */
{0x70, 0x0070}, /* Latin p */
{0x71, 0x0071}, /* Latin q */
{0x72, 0x0072}, /* Latin r */
{0x73, 0x0073}, /* Latin s */
{0x74, 0x0074}, /* Latin t */
{0x75, 0x0075}, /* Latin u */
{0x76, 0x0076}, /* Latin v */
{0x77, 0x0077}, /* Latin w */
{0x78, 0x0078}, /* Latin x */
{0x79, 0x0079}, /* Latin y */
{0x7A, 0x007A}, /* Latin z */
{0x7B, 0x007B}, /* Left curly bracket */
{0x7C, 0x007C}, /* Vertical line */
{0x7D, 0x007D}, /* Right curly bracket */
{0x7E, 0x007E}, /* Tilde */
{0x7F, 0x007F}, /* DEL */
{0x80, 0x0080}, /* control character 0x80 */
{0x81, 0x0081}, /* control character 0x81 */
{0x82, 0x0082}, /* control character 0x82 */
{0x83, 0x0083}, /* control character 0x83 */
{0x84, 0x0084}, /* control character 0x84 */
{0x85, 0x0085}, /* control character 0x85 */
{0x86, 0x0086}, /* control character 0x86 */
{0x87, 0x0087}, /* control character 0x87 */
{0x88, 0x0088}, /* control character 0x88 */
{0x89, 0x0089}, /* control character 0x89 */
{0x8A, 0x008A}, /* control character 0x8A */
{0x8B, 0x008B}, /* control character 0x8B */
{0x8C, 0x008C}, /* control character 0x8C */
{0x8D, 0x008D}, /* control character 0x8D */
{0x8E, 0x008E}, /* control character 0x8E */
{0x8F, 0x008F}, /* control character 0x8F */
{0x90, 0x0394}, /* Greek capital letter delta */
{0x91, 0x20AC}, /* Euro sign */
{0x92, 0x03A6}, /* Greek capital letter phi */
{0x93, 0x0393}, /* Greek capital letter gamma */
{0x94, 0x039B}, /* Greek capital letter lamda */
{0x95, 0x03A9}, /* Greek capital letter omega */
{0x96, 0x03A0}, /* Greek capital letter pi */
{0x97, 0x03A8}, /* Greek capital letter psi */
{0x98, 0x03A3}, /* Greek capital letter sigma */
{0x99, 0x0398}, /* Greek capital letter theta */
{0x9A, 0x039E}, /* Greek capital letter xi */
{0x9B, 0x009B}, /* control character 0x9B */
{0x9C, 0x009C}, /* control character 0x9C */
{0x9D, 0x009D}, /* control character 0x9D */
{0x9E, 0x009E}, /* control character 0x9E */
{0x9F, 0x009F}, /* control character 0x9F */
{0xA0, 0x00A0}, /* Non-Block Space */
{0xA1, 0x00A1}, /* Inverted exclamation mark */
{0xA2, 0x00A2}, /* Cent sign */
{0xA3, 0x00A3}, /* Pound sign */
{0xA4, 0x00A4}, /* Currency sign */
{0xA5, 0x00A5}, /* Yen sign */
{0xA6, 0x00A6}, /* Broken Vertical bar */
{0xA7, 0x00A7}, /* Section sign */
{0xA8, 0x00A8}, /* Diaeresis */
{0xA9, 0x00A9}, /* Copyright sign */
{0xAA, 0x00AA}, /* Feminine ordinal indicator */
{0xAB, 0x00AB}, /* Left-pointing double angle quotation mark */
{0xAC, 0x00AC}, /* Not sign */
{0xAD, 0x00AD}, /* Soft hyphen */
{0xAE, 0x00AE}, /* Registered sign */
{0xAF, 0x00AF}, /* Macron */
{0xB0, 0x00B0}, /* Degree sign */
{0xB1, 0x00B1}, /* Plus-minus sign */
{0xB2, 0x00B2}, /* Superscript two */
{0xB3, 0x00B3}, /* Superscript three */
{0xB4, 0x00B4}, /* Acute accent */
{0xB5, 0x00B5}, /* Micro sign */
{0xB6, 0x00B6}, /* Pilcrow sign */
{0xB7, 0x00B7}, /* Middle dot */
{0xB8, 0x00B8}, /* Cedilla */
{0xB9, 0x00B9}, /* Superscript one */
{0xBA, 0x00BA}, /* Masculine ordinal indicator */
{0xBB, 0x00BB}, /* Right-pointing double angle quotation mark */
{0xBC, 0x00BC}, /* Fraction one quarter */
{0xBD, 0x00BD}, /* Fraction one half */
{0xBE, 0x00BE}, /* Fraction three quarters */
{0xBF, 0x00BF}, /* Inverted question mark */
{0xC0, 0x00C0}, /* Latin A With grave */
{0xC1, 0x00C1}, /* Latin A With acute */
{0xC2, 0x00C2}, /* Latin A With circumflex */
{0xC3, 0x00C3}, /* Latin A With tilde */
{0xC4, 0x00C4}, /* Latin A With diaeresis */
{0xC5, 0x00C5}, /* Latin A With ring above */
{0xC6, 0x00C6}, /* Latin AE */
{0xC7, 0x00C7}, /* Latin C with cedilla */
{0xC8, 0x00C8}, /* Latin E with grave */
{0xC9, 0x00C9}, /* Latin E with acute */
{0xCA, 0x00CA}, /* Latin E with circumflex */
{0xCB, 0x00CB}, /* Latin E with diaeresis */
{0xCC, 0x00CC}, /* Latin I with grave */
{0xCD, 0x00CD}, /* Latin I with acute */
{0xCE, 0x00CE}, /* Latin I with circumflex */
{0xCF, 0x00CF}, /* Latin I with diaeresis */
{0xD0, 0x00D0}, /* Latin CAPITAL LETTER ETH (Icelandic) */
{0xD1, 0x00D1}, /* Latin CAPITAL LETTER N WITH TILDE */
{0xD2, 0x00D2}, /* Latin CAPITAL LETTER O WITH GRAVE */
{0xD3, 0x00D3}, /* Latin CAPITAL LETTER O WITH ACUTE */
{0xD4, 0x00D4}, /* Latin CAPITAL LETTER O WITH CIRCUMFLEX */
{0xD5, 0x00D5}, /* Latin CAPITAL LETTER O WITH TILDE */
{0xD6, 0x00D6}, /* Latin CAPITAL LETTER O WITH DIAERESIS */
{0xD7, 0x00D7}, /* MULTIPLICATION SIGN */
{0xD8, 0x00D8}, /* Latin CAPITAL LETTER O WITH STROKE */
{0xD9, 0x00D9}, /* Latin CAPITAL LETTER U WITH GRAVE */
{0xDA, 0x00DA}, /* Latin CAPITAL LETTER U WITH ACUTE */
{0xDB, 0x00DB}, /* Latin CAPITAL LETTER U WITH CIRCUMFLEX */
{0xDC, 0x00DC}, /* Latin CAPITAL LETTER U WITH DIAERESIS */
{0xDD, 0x00DD}, /* Latin CAPITAL LETTER Y WITH ACUTE */
{0xDE, 0x00DE}, /* Latin CAPITAL LETTER THORN (Icelandic) */
{0xDF, 0x00DF}, /* Latin SHARP S (German) */
{0xE0, 0x00E0}, /* Latin A WITH GRAVE */
{0xE1, 0x00E1}, /* Latin A WITH ACUTE */
{0xE2, 0x00E2}, /* Latin A WITH CIRCUMFLEX */
{0xE3, 0x00E3}, /* Latin A WITH TILDE */
{0xE4, 0x00E4}, /* Latin A WITH DIAERESIS */
{0xE5, 0x00E5}, /* Latin A WITH RING ABOVE */
{0xE6, 0x00E6}, /* Latin AE */
{0xE7, 0x00E7}, /* Latin C WITH CEDILLA */
{0xE8, 0x00E8}, /* Latin E WITH GRAVE */
{0xE9, 0x00E9}, /* Latin E WITH ACUTE */
{0xEA, 0x00EA}, /* Latin E WITH CIRCUMFLEX */
{0xEB, 0x00EB}, /* Latin E WITH DIAERESIS */
{0xEC, 0x00EC}, /* Latin I WITH GRAVE */
{0xED, 0x00ED}, /* Latin I WITH ACUTE */
{0xEE, 0x00EE}, /* Latin I WITH CIRCUMFLEX */
{0xEF, 0x00EF}, /* Latin I WITH DIAERESIS */
{0xF0, 0x00F0}, /* Latin ETH (Icelandic) */
{0xF1, 0x00F1}, /* Latin N WITH TILDE */
{0xF2, 0x00F2}, /* Latin O WITH GRAVE */
{0xF3, 0x00F3}, /* Latin O WITH ACUTE */
{0xF4, 0x00F4}, /* Latin O WITH CIRCUMFLEX */
{0xF5, 0x00F5}, /* Latin O WITH TILDE */
{0xF6, 0x00F6}, /* Latin O WITH DIAERESIS */
{0xF7, 0x00F7}, /* DIVISION SIGN */
{0xF8, 0x00F8}, /* Latin O WITH STROKE */
{0xF9, 0x00F9}, /* Latin U WITH GRAVE */
{0xFA, 0x00FA}, /* Latin U WITH ACUTE */
{0xFB, 0x00FB}, /* Latin U WITH CIRCUMFLEX */
{0xFC, 0x00FC}, /* Latin U WITH DIAERESIS */
{0xFD, 0x00FD}, /* Latin Y WITH ACUTE */
{0xFE, 0x00FE}, /* Latin THORN (Icelandic) */
{0xFF, 0x00FF}, /* Latin Y WITH DIAERESIS */
};

/* True if card error has been recieved after the LPM mode was turned On */
uint32 pbm_session_lpm_status = 0;

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_INIT

DESCRIPTION
  This function initializes enumeration of a complex field. If there are
  complex fields within complex fields, all of them are converted into
  individual PBM fields.

  If the input field is a simple field, the simple field is returned as
  enumeration.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_complex_init(
  const pbm_field_s_type      *field,
  int                         *pnum_fields,
  pbm_complex_result_s_type  **presult
)
{
  int                        num_sub_fields;
  int                        returned_val;
  pbm_return_type            ret;
  pbm_complex_result_s_type *result = NULL;
  uint32                    *sizes=NULL;

  PBM_CHECK_PTR2_RET(field, presult, PBM_ERROR);

  *presult = NULL;

  PBM_MEM_ALLOC(result, sizeof(pbm_complex_result_s_type));
  PBM_CHECK_PTR_RET(result, PBM_ERROR);
  memset(result, 0, sizeof(pbm_complex_result_s_type));

  /* Get number of fields */
  if (field->field_type == PBM_FT_COMPLEX)
  {
    num_sub_fields = pbm_complex_num_sub_fields(
                                                 (pbm_complex_field_s_type *) pbm_get_field_data_ptr(field,0));
    PBM_MEM_ALLOC(sizes, (uint32) num_sub_fields * sizeof(uint32));
    if (sizes == NULL)
    {
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }
    returned_val = pbm_complex_fill_sizes(sizes,
                                          num_sub_fields,
                                          (pbm_complex_field_s_type *)
                                          pbm_get_field_data_ptr(field,0));
    if (returned_val != num_sub_fields)
    {
      UIM_MSG_ERR_2("pbm_complex_fill_sizes() didn't fill all subfields %d %d",
                    returned_val, num_sub_fields);
      PBM_MEM_FREEIF(sizes);
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }
  } else
  {
    num_sub_fields = 1;

    PBM_MEM_ALLOC(sizes, (uint32) num_sub_fields * sizeof(uint32));
    if (sizes == NULL)
    {
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }

    *sizes = field->data_len;
  }

  result->num_fields = num_sub_fields;

  /* Allocate memory */
  if (!pbm_allocate_fields_internal(&result->fields, (uint32) num_sub_fields,
                                     sizes))
  {
    PBM_MEM_FREEIF(sizes);
    pbm_enum_complex_free(result);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF(sizes);

  /* Read fields */
  if (field->field_id == PBM_FIELD_COMPLEX)
  {
    ret = pbm_complex_unwrap((pbm_complex_field_s_type *)
                              pbm_get_field_data_ptr(field,0), result);
    if (ret != PBM_SUCCESS)
    {
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }
  } else
  {
    pbm_fill_field(&result->fields[result->enumerator++],
                   field->field_id,
                   field->field_type,
                   pbm_get_field_data_ptr(field,0),
                   field->data_len);
  }

  if (pnum_fields)
    *pnum_fields = num_sub_fields;

  result->enumerator = 0;

  *presult = result;

  return PBM_SUCCESS;
}/* pbm_enum_complex_init */


/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_NEXT

DESCRIPTION
  This function obtains the next sub field within a complex field, converts it
  to a normal PBM field and returns it.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_s_type *pbm_enum_complex_next(
  pbm_complex_result_s_type       *result
)
{
  if (!result)
    return NULL;

  if (result->enumerator >= result->num_fields)
    return NULL;
  else
    return &result->fields[result->enumerator++];
} /* pbm_enum_complex_next */


/*===========================================================================
FUNCTION pbm_util_query_to_match_numbers

DESCRIPTION
 this function tells if the passed field is TON or number type
 and returns TRUE if the TON is international.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_util_query_to_match_numbers(
  pbm_field_id_e_type   field_id, 
  boolean               queried_number_has_plus,
  boolean               cache_number_contains_ton,
  boolean               cache_number_contains_plus,
  boolean               valid_ss_string
)
{
  boolean result = FALSE;

  if (field_id == PBM_FIELD_PHONE_GENERIC)
  {
    /* consider the TON as well in conditions */
    if (  ((queried_number_has_plus) && (!cache_number_contains_ton) && (valid_ss_string))
        || ((queried_number_has_plus) && (cache_number_contains_ton) && (valid_ss_string))
        || ((queried_number_has_plus) && (!valid_ss_string) && (cache_number_contains_ton ||cache_number_contains_plus))
        ||  ((!queried_number_has_plus) && (!cache_number_contains_ton && !cache_number_contains_plus))
       )
    {
      result = TRUE;
    }
  }
  else
  {
    /* other phone field no need to consider the ton */
    if (   ((queried_number_has_plus) && (!cache_number_contains_plus) && (valid_ss_string))
        || ((queried_number_has_plus) && (!valid_ss_string) && (cache_number_contains_plus))
        ||  ((!queried_number_has_plus) && (!cache_number_contains_plus) )
       )
    {
      result = TRUE;
    }
  }
  return result;
}/* pbm_util_query_to_match_numbers */


/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_FREE

DESCRIPTION
  This function frees the memory allocated to store the search result.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_enum_complex_free(
  pbm_complex_result_s_type  *result
)
{
  PBM_CHECK_PTR_RET(result, VOID);

  PBM_MEM_FREEIF(result->fields);
  result->num_fields = 0;
  result->enumerator = 0;

  PBM_MEM_FREEIF(result);
} /* pbm_enum_complex_free */


/*===========================================================================
FUNCTION pbm_get_internal_field_info
DESCRIPTION
  This function is used to return the exact field info structure among .
g_rest_field_info,g_sim_field_info,g_efs_field_info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_internal_field_info(
  pbm_phonebook_type        pb_id,
  pbm_internal_field_info  *field_info
)
{
  pbm_session_enum_type   session_type = PBM_SESSION_MAX;
  pbm_return_type         ret          = PBM_ERROR;
  PB_CACHE               *cache        = NULL;
  int                     field_group  = pbm_pb_id_to_fieldgroup(pb_id);

  PBM_CHECK_PTR_RET(field_info,PBM_ERROR);
  if (!(field_group < NUM_GROUPS && field_group > INVALID_GROUP))
  {
    UIM_MSG_ERR_2("Invalid group: phonebook %d and category %d", 
                   pb_id.device_type,pb_id.pb_category);
    return PBM_ERROR;
  }
  session_type = pbm_mapping_to_session(pb_id.pb_category, (pbm_slot_type) pb_id.slot_id,pb_id.prov_type);
  if (( field_group != ECC_GROUP) && ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS) )
  {
    return ret;
  }
  if ( (session_type < PBM_SESSION_MAX)  || (!pbm_action_requires_sim(pb_id))
         || (pb_id.device_type == PBM_ECC))
  {
    if(field_group == ECC_GROUP ||
       field_group == ECC_OTHER_GROUP)
    {
      memscpy(field_info, sizeof(pbm_g_efs_field_info_s_type),
              &(g_ecc_field_info[field_group - 3]), sizeof(pbm_g_other_field_info_s_type));
      cache = pbm_pb_id_to_cache(pb_id);
      if(cache != NULL)
      {
        if(cache->text_length[0])
        {
          field_info->info[0].max_length = cache->text_length[0] + 2;
        }
      }
      ret = PBM_SUCCESS;
    }
    else
    {
      ret = pbm_get_internal_non_ecc_field_info(pb_id, field_info, field_group, session_type);
    }
  }

  return ret;
}/* pbm_get_internal_field_info */


/*===========================================================================
FUNCTION pbm_validate_pb_set

DESCRIPTION
 this function validates the pb_set value 

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_validate_pb_set(
  pbm_pb_set_type     pb_set
)
{
  if ((pb_set < PBM_USIM_PB_ONE) || (pb_set >= PBM_MAX_NUM_PB_SETS)) 
  {																	   
    UIM_MSG_ERR_2("PB_SET %d out of bounds , num sets %d", pb_set,		   
    	           PBM_MAX_NUM_PB_SETS);
    return FALSE;													   
  }

  return TRUE;
} /* pbm_validate_pb_set */


/*===========================================================================
FUNCTION pbm_util_is_session_provision

DESCRIPTION
  This function whether the session is provisioning or not

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_util_is_session_provision(
  pbm_provision_enum_type   prov_type
)
{
  switch ( prov_type )
  {
    case PBM_PROVISION_GW:
    case PBM_PROVISION_1X:
    case PBM_PROVISION_GW_SECONDARY:
    case PBM_PROVISION_1X_SECONDARY:
#ifdef FEATURE_TRIPLE_SIM
    case PBM_PROVISION_GW_TERTIARY :
    case PBM_PROVISION_1X_TERTIARY :
#endif /* FEATURE_TRIPLE_SIM */
      return TRUE ;

    default :
      return FALSE;
  }
} /* pbm_util_is_session_provision */


/*===========================================================================
FUNCTION pbm_mmgsdi_session_to_pbm_session

DESCRIPTION
retrieves session id from record info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_mmgsdi_session_to_pbm_session (
  mmgsdi_session_id_type session_id
)
{
  pbm_session_enum_type   pb_session = PBM_SESSION_DEFAULT;

  for (pb_session = PBM_SESSION_DEFAULT; pb_session < PBM_SESSION_MAX; pb_session++ )
  {
    if (session_id == pbm_session_info[pb_session].session_id)
    {
      return pb_session;
    }
  }
  UIM_MSG_ERR_0("Session out of range, picking default");
  return PBM_SESSION_DEFAULT;
} /* pbm_mmgsdi_session_to_pbm_session */


/*===========================================================================

FUNCTION: pbm_bcd_to_ascii

DESCRIPTION
  Convert the phone number from BCD to ASCII, but checks to make sure
  we don't overwrite memory.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, if we can do this.

SIDE EFFECTS
  None

===========================================================================*/
boolean pbm_bcd_to_ascii(
  PACKED const byte *bcd_number,
  uint8              bcd_len,
  uint8              ton_npi,
  byte              *ascii_number
)
{
  int   bcd_index   = 0;
  int   ascii_index = 0;
  uint8 asc_1 = 0;
  uint8 asc_2 = 0;

  PBM_CHECK_PTR2_RET(bcd_number, ascii_number, FALSE);

  if (bcd_len > (PBM_MAX_NUM_LENGTH / 2))
  {
    UIM_MSG_ERR_1("BCD_LENGTH 0x%x maybe corrupt.", bcd_len);
    return FALSE;
  }

  /*****************************/
  /* International call or not */
  /*****************************/
  if ((ton_npi & CM_TON_MASK) == CM_TON_INTERNATIONAL)
  {
    ascii_number[ascii_index++] = '+';
  }

  /*************************/
  /* Decode BCD into ascii */
  /*************************/
  for ( bcd_index = 0; bcd_index < bcd_len; bcd_index++)
  {
    asc_1 = (bcd_number[bcd_index] & 0x0F);
    asc_2 = (bcd_number[bcd_index] & 0xF0) >> 4;

    ascii_number[ascii_index++] = pbm_bcd_to_char_mapping[asc_1];
    ascii_number[ascii_index++] = pbm_bcd_to_char_mapping[asc_2];
  }

  /* Null terminate the ascii string */
  if (asc_2 != 0x0f)
  {
    ascii_number[ascii_index] = '\0';
  }
  return TRUE;
}/* pbm_bcd_to_ascii */


/*==============================================================================

FUNCTION NAME pbm_action_requires_sim

DESCRIPTION
   This function is called to determine if an action on a given pb_id
   requires the SIM to be fully initialized.  The purpose of this call
   is to prohibit reads and writes to memory while the SIM is still
   initializing.

RETURN VALUE
  TRUE: If the action would require the SIM.
  FALSE: The pb_id does not touch the UIM.

DEPENDENCIES
  None
==============================================================================*/
boolean pbm_action_requires_sim(pbm_phonebook_type pb_id)
{
  switch (pb_id.device_type)
  {
    case PBM_DEFAULT:
    case PBM_ADN:                 /* Abbreviated Dialing Number list */
    case PBM_FDN:                 /* Fixed Dialing Number list */
    case PBM_LND:                 /* SIM/RUIM Last Number Dialed list */
    case PBM_SDN:                 /* Service Dialling Number */
    case PBM_MSISDN:              /* MSISDN */
    case PBM_MBN:                 /* MBN */
    case PBM_ECC:                 /* Emergency Call Code (Emergency number list) */
    case PBM_GRPNAME:
    case PBM_AAS:
    case PBM_OCI:                 /* USIM Outgoing Call Information file.  */
    case PBM_ICI:                 /* USIM Incoming Call Information file.  */
    case PBM_MBDN:                /* MBDN */
      return TRUE;

	default:
      return FALSE;
  }
}/* pbm_action_requires_sim */


/*===========================================================================
FUNCTION pbm_uncompress_fields

DESCRIPTION
  Converts the number from BCD format to ascii and text to UCS2 to send to 
  the clients.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_uncompress_fields(
  const pbm_field_s_type  *fields,
  int                      num_fields,
  pbm_field_s_type       **new_fields,
  int                     *total_fields,
  boolean                  skip_text_fields
)
{
  pbm_field_s_type              *my_data_buf_with_complex_fields=NULL;
  pbm_field_s_type              *uncompressed_fields=NULL;
  int                            num_fields_with_complex_fields;
  uint32                        *new_rec_sizes = NULL;
  boolean                        converted_to_complex_anr = FALSE;
  uint8                          cnt = 0,valid_field_cnt = 0;
  pbm_return_type                ret = PBM_SUCCESS;
  pbm_field_type_e_type          uncompressed_field_type;
  boolean                        is_sups_string = FALSE ;
  uint8                         *uncompressed_data = NULL;

  for(cnt=0;cnt<num_fields;cnt++)
  {
    /* need to compute the size for extra simple anr,
     * since legacy clients would still be expecting
     * simple additional number field
     */
    if (fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
    {
      /* if atleast EXT anr field , then convert it
       * into complex anr and simple ANR before sending the record to clinet
       */
      if(PBM_SUCCESS != pbm_convert_to_complex_anr((const pbm_field_s_type *)fields,
                                                    num_fields,&my_data_buf_with_complex_fields,
                                                    &num_fields_with_complex_fields))
      {
        UIM_MSG_ERR_0("Can't copy fields");
        ret = PBM_ERROR;
        goto END_PBM_FUNC;
      }
      else
      {
        converted_to_complex_anr = TRUE;
        break;
      }
    }
  }

  if(converted_to_complex_anr)
  {
    fields = my_data_buf_with_complex_fields;
    num_fields = num_fields_with_complex_fields;
  }

  PBM_MEM_ALLOC(new_rec_sizes, num_fields * sizeof(uint32));
  if(new_rec_sizes == NULL)
  {
    UIM_MSG_ERR_1("no memory %d ", num_fields * sizeof(uint32));
    goto END_PBM_FUNC;
  }

  PBM_MEM_ALLOC(uncompressed_data, PBM_TEXT_SIZE_BYTES * sizeof(uint8));
  if(uncompressed_data == NULL)
  {
    UIM_MSG_ERR_1("no memory %d ", PBM_TEXT_SIZE_BYTES * sizeof(uint8));
    goto END_PBM_FUNC;
  }

  for(cnt=0;cnt<num_fields;cnt++)
  {
    const pbm_field_s_type     *sfield = &fields[cnt];
    uint8                      *sfield_data_ptr = pbm_get_field_data_ptr(fields,cnt);

    if( sfield_data_ptr == NULL ) 
    {
      ret = PBM_ERROR;
      goto END_PBM_FUNC;
    }

    memset(uncompressed_data,0,PBM_TEXT_SIZE_BYTES);
    if (sfield->field_type == PBM_FT_BCD)
    {
      if(!pbm_bcd_to_ascii((byte *)&sfield_data_ptr[2],
                            sfield_data_ptr[0]-1,
                              sfield_data_ptr[1],
                              (byte *)uncompressed_data))
      {
        UIM_MSG_ERR_2("BCD to ASCII erro %d %d",sfield_data_ptr[0],sfield_data_ptr[1]);
        continue;
      }

      //append NULL char
      new_rec_sizes[valid_field_cnt] = 1 + strlen((const char *)uncompressed_data);
      valid_field_cnt++;
    }
    else if (sfield->field_type == PBM_FT_UCS2_COMPRESSED && !skip_text_fields )
    {
      (void) pbmconvert_sim_to_string(sfield_data_ptr,
                                      sfield->data_len,
                                      (uint16 *)uncompressed_data,
                                      PBM_TEXT_SIZE_BYTES);

      //append NULL char
      new_rec_sizes[valid_field_cnt] = 2*(1 + pbmutils_wstrlen((uint16*)uncompressed_data));
      valid_field_cnt++;
    }
    else
    {
      new_rec_sizes[valid_field_cnt] = sfield->data_len;
      valid_field_cnt++;
    }
  }

  if (!pbm_allocate_fields_internal(&uncompressed_fields,valid_field_cnt,new_rec_sizes))
  {
    ret = PBM_ERROR;
    goto END_PBM_FUNC;
  }

  valid_field_cnt = 0;
  for(cnt=0;cnt<num_fields;cnt++)
  {
    const pbm_field_s_type       *sfield = &fields[cnt];
    uint8                        *sfield_data_ptr = pbm_get_field_data_ptr(fields,cnt);

    memset(uncompressed_data,0,PBM_TEXT_SIZE_BYTES);
    if (sfield->field_type == PBM_FT_BCD)
    {
      int pos_plus,i;
      if(!pbm_bcd_to_ascii((byte *)&sfield_data_ptr[2],
                             sfield_data_ptr[0]-1,
                             sfield_data_ptr[1],
                             (byte *)uncompressed_data))
      {
        UIM_MSG_ERR_2("BCD to ASCII erro %d %d",sfield_data_ptr[0],sfield_data_ptr[1]);
        continue;
      }

      if( uncompressed_data[0] == '+' )
      {
        if ( pbm_util_is_valid_sups_string( (char *)(uncompressed_data+1),
                                            strlen((const char *)(uncompressed_data+1)),
                                            &pos_plus,
                                            FALSE) )
        {
          /* if the number is valid SS string */
          if( pbm_util_is_valid_sups_string((char *)(uncompressed_data+1),
                                             strlen((const char *)(uncompressed_data+1)),
                                             &pos_plus, 
                                             TRUE)  )
          {
            is_sups_string = TRUE ;
            /* Place + in the correct position in the string */ 
            for (i=0;i<pos_plus;i++)
            {
              uncompressed_data[i] = uncompressed_data[i+1];
            }
            uncompressed_data[i] = '+';
          }
          else
          {
            UIM_MSG_ERR_0("Could not find position of plus in sups string");
          }
        }
      }
      uncompressed_field_type = PBM_FT_PHONE;
    }
    else if (sfield->field_type == PBM_FT_UCS2_COMPRESSED && !skip_text_fields)
    {
      (void) pbmconvert_sim_to_string(sfield_data_ptr,
                                      sfield->data_len,
                                      (uint16*)uncompressed_data,
                                      PBM_TEXT_SIZE_BYTES);
      uncompressed_field_type = PBM_FT_UCS2;
    }
    else
    {
      memscpy(uncompressed_data,PBM_TEXT_SIZE_BYTES,sfield_data_ptr,sfield->data_len);
      uncompressed_field_type = sfield->field_type;
    }

    pbm_fill_field(&uncompressed_fields[valid_field_cnt], 
                   sfield->field_id,
                   uncompressed_field_type,
                   uncompressed_data,new_rec_sizes[valid_field_cnt]);
    valid_field_cnt++;
  }

  if ( is_sups_string ) 
  {
    UIM_MSG_HIGH_0(" Found sup string in record ");
    for ( cnt = 0 ; cnt < num_fields ; cnt++ )
    {
      uint8 *sfield_data_ptr = pbm_get_field_data_ptr(uncompressed_fields,cnt) ; 
      if( sfield_data_ptr != NULL )
      {
        if ( uncompressed_fields[cnt].field_id ==  PBM_FIELD_NUMBER_TYPE ) 
        {
          *sfield_data_ptr =  0; 
        }
        if ( uncompressed_fields[cnt].field_id ==  PBM_FIELD_GSM_TONNPI ) 
        {
          *sfield_data_ptr &= 0x8F ; 
        }
      }
    }
  }

  *new_fields   = uncompressed_fields;
  *total_fields = valid_field_cnt;

  END_PBM_FUNC:
  if(converted_to_complex_anr)
    PBM_MEM_FREEIF(my_data_buf_with_complex_fields);
  PBM_MEM_FREEIF(new_rec_sizes);
  PBM_MEM_FREEIF(uncompressed_data);
  return ret;
}/* pbm_uncompress_fields */


/*===========================================================================
FUNCTION pbm_record_id_to_session_type

DESCRIPTION
Converts a record ID to a session ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_record_id_to_session_type (
  pbm_record_id_type          record_id
)
{
  pbm_phonebook_type pb_id = {PBM_GPB};

  pb_id = pbm_session_record_id_to_phonebook_type(record_id);
  return pbm_mapping_to_session (pb_id.pb_category, (pbm_slot_type)pb_id.slot_id, pb_id.prov_type);
} /* pbm_record_id_to_session_type */


/*===========================================================================
FUNCTION PBM_MAPPING_TO_SESSION

DESCRIPTION
retrieves session id from record info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_mapping_to_session(
  pbm_pb_category_enum_type   pb_category,
  pbm_slot_type               slot_id,
  pbm_provision_enum_type     prov_type
)
{
  pbm_session_enum_type i     = PBM_SESSION_DEFAULT;
  boolean               found = FALSE;

  if ( PBM_GPB == pb_category )
  {
    switch(slot_id)
    {
      case PBM_SLOT_1:
      {
        i = PBM_SESSION_GPB_1;
        found = TRUE;
      }
      break;

      case PBM_SLOT_2:
      {
        i = PBM_SESSION_GPB_2;
        found = TRUE;
      }
      break;

#ifdef FEATURE_TRIPLE_SIM
      case PBM_SLOT_3:
      {
        i = PBM_SESSION_GPB_3;
        found = TRUE;
      }
      break;
#endif /* FEATURE_TRIPLE_SIM */

      default:
        break;
    }
  }
  else
  {
    for ( i = (int) PBM_SESSION_DEFAULT; i < (int) PBM_SESSION_MAX; i++ )
    {
      /* local phonebook, check only mandatory prov info */
      if (((int) PBM_SESSION_LPB_GW == i &&
            PBM_PROVISION_GW == prov_type) ||
           ((int) PBM_SESSION_LPB_1X == i &&
            PBM_PROVISION_1X == prov_type)
#ifdef FEATURE_DUAL_SIM
          ||( (int) PBM_SESSION_LPB_GW_SECONDARY == i &&
               PBM_PROVISION_GW_SECONDARY == prov_type) ||
              ((int)PBM_SESSION_LPB_1X_SECONDARY == i &&
               PBM_PROVISION_1X_SECONDARY == prov_type)
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
          ||( (int) PBM_SESSION_LPB_GW_TERTIARY == i &&
             PBM_PROVISION_GW_TERTIARY == prov_type) ||
            ((int)PBM_SESSION_LPB_1X_TERTIARY == i &&
             PBM_PROVISION_1X_TERTIARY == prov_type)
#endif /* FEATURE_TRIPLE_SIM */
         )
      {
        found = TRUE;
        break;
      }
    }
  }

  if ( TRUE == found && i < (int) MAX_UIM_APPS )
  {
    /* success - return session id*/
    return i;
  }

  UIM_MSG_ERR_3("Invalid parameters %d %d %d",pb_category,slot_id,prov_type);
  return PBM_SESSION_MAX;
}/* pbm_mapping_to_session */


/*===========================================================================
FUNCTION CHECK_PB_READY

DESCRIPTION
Checks if the phonebook is ready or not and returns the appropriate PBM
return value. Any value other than PBM_SUCCESS means the phonebook is not
available for use.

Checking for PBM_DEFAULT returns an ERROR

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type check_pb_ready(
  pbm_phonebook_type             pb_id
)
{
  pbm_return_type     ret;
  PB_CACHE           *cache;

  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
    return PBM_ERROR;

  switch (cache->status)
  {
    case PBM_STATUS_READY:
      ret = PBM_SUCCESS;
      break;

    case PBM_STATUS_NOT_INIT:
      if (pbm_action_requires_sim(pb_id))
        ret = PBM_ERROR_NO_SIM;
      else
        ret = PBM_ERROR_NOT_READY;
      break;

    case PBM_STATUS_INIT_SYNC:
    case PBM_STATUS_WRITE_SYNC:
      if (rex_self() == PBM_TCB)
        ret = PBM_SUCCESS;
      else
        ret = PBM_ERROR_SYNC;
      break;

    case PBM_STATUS_PIN_REQ: ret = PBM_ERROR_SIM_PIN_REQ; break;
    case PBM_STATUS_PUC_REQ: ret = PBM_ERROR_SIM_PUC_REQ; break;
    case PBM_STATUS_PIN2_REQ: ret = PBM_ERROR_PIN2_REQ; break;
    case PBM_STATUS_PUC2_REQ: ret = PBM_ERROR_PUC2_REQ; break;
    case PBM_STATUS_SIM_FAIL: ret = PBM_ERROR_SIM_FAIL; break;
    case PBM_STATUS_NOT_AVAILABLE: ret = PBM_ERROR_NOT_AVAILABLE; break;
    case PBM_STATUS_RESTRICTED: ret = PBM_ERROR_RESTRICTED; break;

    case PBM_STATUS_NOT_READY:
    default:
      ret = PBM_ERROR_NOT_READY;
      break;
  }

  return ret;
}/* check_pb_ready */


/*===========================================================================
FUNCTION pbm_util_get_cache_number_status_of_ton

DESCRIPTION
 this function tells if the passed field is TON or number type
 and returns TRUE if the TON is international.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_util_get_cache_number_status_of_ton(
  pbm_field_s_type     *sfield_ptr
)
{
  boolean        result = FALSE;
  void          *temp   = pbm_get_field_data_ptr(sfield_ptr,0);

  PBM_CHECK_PTR_RET(temp , FALSE);

  if (sfield_ptr->data_len != 0)
  {
    if ( sfield_ptr->field_id == PBM_FIELD_GSM_TONNPI )
    {
      /* check if the cache number has TON as international */
      if((*(byte *)temp & CM_TON_MASK) == CM_TON_INTERNATIONAL)
      {
        result = TRUE;
      }
    }
    if (sfield_ptr->field_id == PBM_FIELD_NUMBER_TYPE)
    {
      if (*(uint8 *)temp < ARR_SIZE(pbm_number_type_to_gsmton))
      {
        if(CM_TON_INTERNATIONAL & pbm_number_type_to_gsmton[*(uint8 *)temp])
        {
          result = TRUE;
        }
      }
    }
    if (sfield_ptr->field_id == PBM_FIELD_PHONE_GENERIC
        && sfield_ptr->field_type == PBM_FT_BCD)
    {
      /* check if the cache number has TON as international */
      if((((byte *)temp)[1] & CM_TON_MASK) == CM_TON_INTERNATIONAL)
      {
        result = TRUE;
      }
    }
  } /* if data_len != 0 */
  return result;
}/* pbm_util_get_cache_number_status_of_ton */


/* ============================================================================
FUNCTION PBM_UTIL_GET_SLOT_INDEX

DESCRIPTION
  This function based on the PBM slot id passed in, populates the slot index

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  PBM_SUCCESS if a valid slot index is available else return  PBM_ERROR

SIDE EFFECTS
  None
============================================================================*/
pbm_return_type pbm_util_get_slot_index(
  pbm_slot_type             pbm_slot,
  uint8                    *slot_index
)
{
  pbm_return_type   pbm_status = PBM_SUCCESS;

  switch(pbm_slot)
  {
    case PBM_SLOT_1:
      *slot_index = PBM_SLOT_1_INDEX;
      break;
    case PBM_SLOT_2:
      *slot_index = PBM_SLOT_2_INDEX;
      break;
    case PBM_SLOT_3:
      *slot_index = PBM_SLOT_3_INDEX;
      break;
    default:
      pbm_status = PBM_ERROR;
      UIM_MSG_ERR_1("Could not retrieve slot index for slot id 0x%x",
                    pbm_slot);
      break;
  }
  return pbm_status;
} /* pbm_util_get_slot_index */


/*===========================================================================
FUNCTION pbm_mapping_as_id_to_pbm_session

DESCRIPTION
 Conversion of AS_ID to PBM session 
 This maps AS_ID only to GW sessions for now 
 Usually  AS_ID_1 maps to GW_PRI and 1x_PRI sessions 
 but from CM we always get OTA for GW sessions and hence the mapping 


DEPENDENCIES

SIDE EFFECTS

===========================================================================*/

pbm_session_enum_type   pbm_mapping_as_id_to_pbm_session( 
  sys_modem_as_id_e_type    as_id
)
{
  switch(as_id) 
  {
    case SYS_MODEM_AS_ID_1 :
	  return PBM_SESSION_LPB_GW ;

#ifdef FEATURE_DUAL_SIM
	case SYS_MODEM_AS_ID_2 : 
	  return PBM_SESSION_LPB_GW_SECONDARY; 
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
    case SYS_MODEM_AS_ID_3 : 
	  return PBM_SESSION_LPB_GW_TERTIARY ; 
#endif /* FEATURE_TRIPLE_SIM */
    default : 
      return PBM_SESSION_DEFAULT; 
  }
}/* pbm_mapping_as_id_to_pbm_session */


/*===========================================================================
FUNCTION pbm_device_type_to_phonebook_type

DESCRIPTION
DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_device_type_to_phonebook_type(
  pbm_session_enum_type  session_type,
  pbm_device_type        device_type
)
{
  pbm_phonebook_type pb_id = {PBM_GPB};

  pb_id.prov_type = pbm_session_type_to_provision_type(session_type);
  pb_id.slot_id = pbm_session_info[session_type].slot_id;
  pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
  pb_id.device_type = device_type;

  return pb_id;
} /* pbm_device_type_to_phonebook_type */


/*===========================================================================
FUNCTION PBM_FREE_FIELDS_INTERNAL

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields_internal.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields_internal( 
  pbm_field_s_type     **fields,
  int                    num_fields
)
{
  PBM_CHECK_PTR_RET(fields, VOID);

  if ((num_fields > 0) && (*fields != NULL))
    PBM_MEM_FREEIF(*fields);
  *fields = NULL;
} /* pbm_free_fields_internal */


/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS_INTERNAL

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the PBM heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields_internal(
  pbm_field_s_type       **fields_buf_ptr,
  uint32                   num_fields,
  const uint32            *sizes
)
{
  uint32                 i;
  uint32                 total_size = 0;
  uint32                 mem_offset = 0;
  const uint32           size_one_field = sizeof(pbm_field_s_type);

  PBM_CHECK_PTR2_RET(fields_buf_ptr, sizes, FALSE);
  if( num_fields == 0 )
  {
    UIM_MSG_ERR_0("Invalid num_fields for allocate_internal ");
    return FALSE;
  }

  *fields_buf_ptr = NULL;
  total_size = ALIGNED_SIZE (num_fields * size_one_field);
  for (i = 0; i < num_fields; i++)
  {
    total_size += ALIGNED_SIZE (sizes[i]);
  }

  if (total_size > 0)
  {
    PBM_MEM_ALLOC(*fields_buf_ptr, total_size);
    if (*fields_buf_ptr == NULL)
    {
      return FALSE;
    }
    mem_offset = ALIGNED_SIZE(num_fields * size_one_field);
    for (i = 0; i < num_fields; i++)
    {
      // Each offset is from the start of the particular field
      // to the data associated with this field.
      // Thus, the first offset points to the start of the
      // data part of the buffer, and then each subsequent offset
      // is incremented by the size of the previous items data minus
      // the size of the previous items field header.  By doing
      // this, each field can be handled in a self contained way, and
      // accessing the data does not require knowledge of the entire
      // field memory buffer.
      (*fields_buf_ptr)[i].buffer_offset = mem_offset;
      mem_offset += ALIGNED_SIZE(sizes[i]);
      mem_offset -= size_one_field; /* take #bytes from next field to its data */
    }
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}/* pbm_allocate_fields_internal */


/*===========================================================================
  FUNCTION PBM_UTIL_GET_COMMON_EFS_ITEM_CONTEXT

  DESCRIPTION
    Returns the index of the common EFS item context based on the index.

  PARAMETERS
    index

  RETURN VALUE
    uim_common_efs_context_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uim_common_efs_context_type pbm_util_get_common_efs_item_context(
  uint8 index
)
{
  switch (index)
  {
    case PBM_SLOT_1_INDEX:
      return UIM_COMMON_EFS_CONTEXT_0;
    case PBM_SLOT_2_INDEX:
      return UIM_COMMON_EFS_CONTEXT_1;
    case PBM_SLOT_3_INDEX:
      return UIM_COMMON_EFS_CONTEXT_2;
    default:
      return UIM_COMMON_EFS_CONTEXT_MAX;
  }
}/* pbm_util_get_common_efs_item_context */


/*===========================================================================
FUNCTION PBM_EFS_CACHE_STRUCTURE_INIT

DESCRIPTION
  Initializes a new cache to contain a given number of records.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_efs_cache_structure_init(
  PB_CACHE             *cache,
  pbm_phonebook_type    pb_id,
  int                   num_of_records,
  boolean               circular
)
{
  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  rex_enter_crit_sect(&pbm_crit_sect);
  cache->pb_id          = pb_id;
  if(num_of_records > 0)
  {
    cache->status         = PBM_STATUS_NOT_INIT;
  }
  else
  {
    cache->status         = PBM_STATUS_NOT_AVAILABLE;
  }
  cache->circular          = circular;
  cache->text_length[0]    = 255;
  cache->record_length[0]  = 65535;
  cache->file_length       = 0;
  cache->num_of_records    = num_of_records;
  cache->records_in_use    = 0;
  cache->start             = 1;
  cache->pb_cache_array    = NULL;

  PBM_ALLOC_MEM_W_ERR_RET(cache->pb_cache_array,
                          (uint32) (cache->num_of_records+1) *
                                    sizeof(pbm_addr_cache_s_type *),
                          rex_leave_crit_sect(&pbm_crit_sect));

  rex_leave_crit_sect(&pbm_crit_sect);
  return PBM_SUCCESS;
} /* pbm_efs_cache_structure_init */


/*===========================================================================
FUNCTION MATCH_FIELDS

DESCRIPTION
Local utility function used by match_found() to determine whether the
field id to be searched matches the cache field id.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean match_fields(
  pbm_field_id_e_type      data_field,
  pbm_field_id_e_type      cache_field,
  uint32                   flags,
  pbm_field_type_e_type    field_type
)
{
  if ((flags & PBM_FIELDID_MASK) != PBM_FIELDID_PHONE)
  {
    if ((data_field == PBM_FIELD_NONE) || (data_field == cache_field))
      return TRUE;
    else
      return FALSE;
  } else
  {
    if (field_type == PBM_FT_PHONE || field_type == PBM_FT_BCD)
      return TRUE;
    else
      return FALSE;
  }
} /* match_fields */


/*===========================================================================
FUNCTION PBM_UTILS_IS_PHONE_FIELD

DESCRIPTION
  This API tells if the passed field is a phone number type field.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
static boolean pbm_utils_is_phone_field(
  pbm_field_id_e_type      field_id
)
{
  if((field_id == PBM_FIELD_PHONE_BUSINESS_FAX)
      ||(field_id ==PBM_FIELD_PHONE_WORK)
      ||(field_id ==PBM_FIELD_PHONE_HOME)
      ||(field_id ==PBM_FIELD_PHONE_FAX)
      ||(field_id ==PBM_FIELD_PHONE_VOICE)
      ||(field_id ==PBM_FIELD_PHONE_PREF)
      ||(field_id ==PBM_FIELD_PHONE_CELL)
      ||(field_id ==PBM_FIELD_PHONE_PAGER)
      ||(field_id ==PBM_FIELD_PHONE_GENERIC)
      ||(field_id ==PBM_FIELD_PHONE_OTHER)
      ||(field_id ==PBM_FIELD_NUMBER_TYPE)
      ||(field_id ==PBM_FIELD_PHONE_BUSINESS_FAX)
      ||(field_id ==PBM_FIELD_DEFAULT_PHONE)
      )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* pbm_utils_is_phone_field */


/*===========================================================================
FUNCTION pbmconvert_pbm8_source

DESCRIPTION
  uint16 pbmconvert_pbm8_source(const void **in_ptr)

  in_ptr: [in/out] Source pointer that is moved after we get the octet.

  The utilities in this file use Unicode as the basis for the conversion.
  When presented with a pbm8 string, this input function is used to get
  each octet, convert it quickly to unicode, and return that.

DEPENDENCIES
  None.

RETURN VALUE
  Unicode value associated with the pbm8 value..

SIDE EFFECTS
  None.
===========================================================================*/
uint16 pbmconvert_pbm8_source(const void **in_ptr)
{
   uint8  *input;
   uint8   pbm8_char;
   uint16  ret_val = 0;

   /* Check input parameters. */
   PBM_CHECK_PTR2_RET(in_ptr, *in_ptr, ret_val);

   input = (uint8 *) *in_ptr;

   /* Take the in_ptr as a character. */
   pbm8_char = *input;

   /* Move in_ptr one octet. */
   input++;

   /* Assign the in_ptr to be the new pointer value. */
   *in_ptr = (void *) input;

   /* Convert what we got to Unicode. */
   ret_val = pbm8_to_unicode[pbm8_char].Unicode;

   return ret_val;
} /* pbmconvert_pbm8_source */


/*===========================================================================
FUNCTION PBM_STRNCMP

DESCRIPTION
String compare function.  This function returns a number greater
than 0 if s1 is greater than s2, a number less than 0 if s1 is less than
s2, and 0 if s1 is the same as s2.  The comparison continues until both
strings reach the null character or n characters are compared. If
case_sensitive is true, the case of the characters must be identical.
If it is false, then a case-insensitive comparison is performed.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static int pbm_strncmp(
  const void       *s1,
  const void       *s2,
  size_t            n,
  boolean           case_sensitive,
  boolean           s1_is_wstr,
  boolean           s2_is_wstr
)
{
  pfsource_fn     src_fn1  = NULL;
  pfsource_fn     src_fn2  = NULL;
  int             ret_val  = 0;
  uint16          c1       = 0;
  uint16          c2       = 0; /* Characters used in comparison */

   /* Check input parameters. */
   PBM_CHECK_PTR2_RET(s1, s2, ret_val);

  /* While there are still characters to test AND
   * *s1 and *s2 are not both NULL characters AND
   * *s1 and *s2 are the same character (when in upper case)
   * check the next character.
   * Otherwise, return the result from the last comparison which has the
   * difference or, if there is no difference, it is set to the value 0.
   */
  if (s1_is_wstr)
  {
    src_fn1 = pbmconvert_return_input_string_first_ucs2_char;
  } else
  {
    src_fn1 = pbmconvert_pbm8_source;
  }
  if (s2_is_wstr)
  {
    src_fn2 = pbmconvert_return_input_string_first_ucs2_char;
  } else
  {
    src_fn2 = pbmconvert_pbm8_source;
  }

  while (n-- && (ret_val == 0))
  {
    c1 = ((*src_fn1)(&s1));
    c2 = ((*src_fn2)(&s2));
    if ((c1 == 0) && (c2 == 0))
      break;
    if (!case_sensitive)
    {
      c1 = pbm_wstr_tolower(c1);
      c2 = pbm_wstr_tolower(c2);
    }

    if ((c1 == c2) || (c1 == pbm8_to_unicode[PBM_WILD_CHAR].Unicode) ||
        (c2 == pbm8_to_unicode[PBM_WILD_CHAR].Unicode))
      ret_val = 0;
    else
      ret_val = (int) c1 - (int) c2;
  }

  return ret_val;
}/* pbm_strncmp */


/*===========================================================================
FUNCTION wstr_tolower

DESCRIPTION
  uint16 wstr_tolower(uint16 c)

  c: [in] UCS2 character to convert to lower case.  Currently, the
          only characters that are converted are 'A' - 'Z'.

  This function allows for a case insensitive match of strings.
  The current implementation only understands the LATIN character
  set, characters 'A' - 'Z', however, any character set may be added
  to this function.

RETURN VALUE
  Either the input character, or the lower case version of the same.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_wstr_tolower(uint16 c)
{
   if (c >= pbm8_to_unicode['A'].Unicode && c <= pbm8_to_unicode['Z'].Unicode)
   {
      return((c - pbm8_to_unicode['A'].Unicode) + pbm8_to_unicode['a'].Unicode);
   }

   return c;
} /* pbm_wstr_tolower */


/*===========================================================================
FUNCTION MATCH_FOUND

DESCRIPTION
Local utility function used by pbm_enum_rec_init() to determine whether the
given cache entry is a match for the given search criteria. Assumes that
data is null terminated.

DEPENDENCIES

SIDE EFFECTS
=========================================================================*/
boolean match_found(
  pbm_cat_e_type               category,
  pbm_field_id_e_type          field_id,
  const void                  *data_ptr,
  uint16                       data_size,
  uint32                       flags,
  const pbm_addr_cache_s_type *cache_entry
)
{
  int                   j                          = 0;
  int                   data_size_temp             = 0;
  boolean               match                      = FALSE;
  /* queried_number_has_plus signifies if the queried number contains + sign */
  boolean               queried_number_has_plus    = FALSE;
  /* cache_number_contains_ton signifies if the number in PBM cache has GSM TON as international*/
  boolean               cache_number_contains_ton  = FALSE;
  /* valid_ss_string signifies if the queried number is a valid SS string */
  boolean               valid_ss_string            = FALSE;
  /* cache_number_contains_plus signifies if the number in pbm cache contains + sign */
  boolean               cache_number_contains_plus = FALSE;
  boolean               match_numbers              = FALSE;
  pbm_field_s_type     *sfield                     = NULL;
  pbm_return_type       ret                        = PBM_ERROR;
  uint8                 offset_for_cache_number    = 0;
  pbm_field_type_e_type cached_entry_data_type;
  /* PBM cache number is to be send for matching with this offset, it can be 0 or 1
  depending upon whether the number contains + sign or not*/
  byte                  local_number[PBM_MAX_NUM_LENGTH + 2] = {0};
  uint8                 cached_entry_data[PBM_TEXT_SIZE_BYTES];

  PBM_CHECK_PTR_RET(cache_entry, FALSE);
  if (cache_entry->fields == NULL)
  {
    /* We're looking at a dummy entry (deleted from cache, only there in
     * case write fails).  This can't match */
    return FALSE;
  }

  if((category == PBM_CAT_NONE) || (cache_entry->category == (uint16) category))
  {
    if((data_ptr == NULL) && (field_id == PBM_FIELD_NONE))
    {
      /* if data is NULL (and any field) , we only look at category */
      return TRUE;
    }

    if(pbm_utils_is_phone_field(field_id))
    {
      memset(local_number, 0, PBM_MAX_NUM_LENGTH + 2);

      /* max length supported for number field is PBM_MAX_LENGTH +1 is for "+" */
      if ( data_size > (PBM_MAX_NUM_LENGTH + 1))
      {
        return FALSE;
      }

      PBM_CHECK_PTR_RET(data_ptr, FALSE);
      memscpy(local_number, PBM_MAX_NUM_LENGTH + 1, data_ptr, data_size);

      /* passed number contains the + sign */
      if(strstr((char *)local_number,"+"))
      {
        data_size_temp = data_size;
        if(!pbm_util_get_ss_ton_status_of_queried_number(&valid_ss_string,
                                                         &queried_number_has_plus,
                                                         (int *)&data_size_temp,
                                                         local_number))
        {
          return FALSE;
        }
        data_size = data_size_temp;
      }
      else
      {
        /* + sign not present in the number queried,
          hence, match with numbers whose TON is not set */
        queried_number_has_plus = FALSE;
      }

      for (j = 0; j < cache_entry->num_fields; j++)
      {
        pbm_complex_result_s_type *result = NULL;

        ret = pbm_enum_complex_init(&cache_entry->fields[j], NULL, &result);
        if (ret != PBM_SUCCESS)
        {
          UIM_MSG_ERR_2("Failed complex field enumeration ret_val 0x%x for id 0x%x",
                         ret, cache_entry->record_id);
          return FALSE;
        }

        while ((sfield = pbm_enum_complex_next(result)) != NULL)
        {
          cache_number_contains_ton = pbm_util_get_cache_number_status_of_ton(sfield);
        }

        pbm_enum_complex_free(result);
        result = NULL;
      }/* for each field */
    }

    for (j = 0; j < cache_entry->num_fields && !match; j++)
    {
      pbm_complex_result_s_type *result           = NULL;
      uint16                     cached_entry_len = 0;

      offset_for_cache_number = 0;
      ret = pbm_enum_complex_init(&cache_entry->fields[j], NULL, &result);
      if (ret != PBM_SUCCESS)
      {
        UIM_MSG_ERR_2("Failed complex field enumeration 0x%x for id 0x%x",
                       ret, cache_entry->record_id);
        return FALSE;
      }

      while ((sfield = pbm_enum_complex_next(result)) != NULL)
      {
        /* reset the flags */
        cache_number_contains_plus = FALSE;
        match_numbers              = FALSE;

        if (match_fields(field_id,
                         sfield->field_id,
                         flags,
                         sfield->field_type))
        {
          if (data_ptr == NULL)
          {
            match = TRUE;
            break;
          }

          memset(cached_entry_data,0,PBM_TEXT_SIZE_BYTES);
          if (pbm_utils_is_phone_field(sfield->field_id) &&
              sfield->field_type == PBM_FT_BCD)
          {
            byte *bcd_num = (byte*)pbm_get_field_data_ptr(sfield,0);

            if (!pbm_bcd_to_ascii(&bcd_num[2],
                                  bcd_num[0]-1,
                                  bcd_num[1],
                                 (byte *) cached_entry_data))
            {
              UIM_MSG_ERR_1("SIM ERROR record 0x%x could not be converted to ascii", bcd_num[0]-1);
              return FALSE;
            }
            cached_entry_data_type = PBM_FT_PHONE;
            cached_entry_len       = strlen((const char *)cached_entry_data) + 1;
          }
          else if (sfield->field_type == PBM_FT_UCS2_COMPRESSED)
          {
            (void) pbmconvert_sim_to_string((uint8*)pbm_get_field_data_ptr(sfield,0),
                                             sfield->data_len,
                                            (uint16*)cached_entry_data,
                                            PBM_TEXT_SIZE_CHARS);
             cached_entry_data_type = PBM_FT_UCS2;
             cached_entry_len       = 2 * (pbmutils_wstrlen((uint16*)cached_entry_data) + 1);
          }
          else
          {
            memscpy(cached_entry_data,
                    PBM_TEXT_SIZE_BYTES,
                    (uint8*)pbm_get_field_data_ptr(sfield,0),
                    sfield->data_len);
            cached_entry_data_type = PBM_FT_STRING;
            cached_entry_len       = sfield->data_len;
          }

          if (sfield->data_len != 0)
          {
            /* place to check if this number from cache should be checked with the queried
               number or not. Move the big if else condition here */
            if(pbm_utils_is_phone_field(field_id))
            {
              /* since match fields has been done already and only number fields can come in this code leg */
              /* need check for + sign if it is just number field */
              if (cached_entry_data[0] == '+')
              {
                cache_number_contains_plus = TRUE;
                /* since cache number contains the + sign, while matching pbm cache number + 1 is to be send */
                offset_for_cache_number = 1;
              }

              match_numbers = pbm_util_query_to_match_numbers(sfield->field_id,
                                                              queried_number_has_plus,
                                                              cache_number_contains_ton,
                                                              cache_number_contains_plus,
                                                              valid_ss_string);
            }

            if(!pbm_utils_is_phone_field(field_id) ||
                match_numbers)
            {
              if ( (flags & PBM_DATATYPE_MASK) == PBM_DATATYPE_STR)
              {
                /* Number of chars in input */
                uint16 num_chars_given     = 0;
                /* Number of chars in record */
                uint16 num_chars_in_record = 0;
                /* PBM_DATATYPE_STR */
                boolean case_sensitive = ((flags & PBM_CASE_MASK) == PBM_CASE_SENSITIVE);

                /* If the type provided is not UCS2, we need to divide to
                 * get the number of characters in the input data. */
                if ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)
                {
                  num_chars_given = data_size/2 - 1;
                }
                else
                {
                  num_chars_given = data_size - 1;
                }

                /* If the record we found is FT_UCS2, divide to get the
                 * number of characters */
                if (sfield->field_type == PBM_FT_UCS2 || cached_entry_data_type == PBM_FT_UCS2)
                {
                  num_chars_in_record = ((cached_entry_len - offset_for_cache_number)/2) - 1;
                }
                else
                {
                  num_chars_in_record = cached_entry_len - offset_for_cache_number - 1;
                }

                if ((flags & PBM_SEARCHTYPE_MASK) == PBM_SEARCHTYPE_CONTAINS)
                {
                  /* PBM_SEARCHTYPE_CONTAINS */
                  if(pbm_utils_is_phone_field(field_id))
                  {
                    match = (pbm_strstr((void *)(cached_entry_data + offset_for_cache_number),
                                        local_number,
                                        case_sensitive,
                                        (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE,
                                        ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)? TRUE:FALSE) != NULL ?
                                        TRUE : FALSE);
                  }
                  else
                  {
                    match = (pbm_strstr((void*)cached_entry_data,
                                        data_ptr,
                                        case_sensitive,
                                        (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE,
                                        ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)? TRUE:FALSE) != NULL ?
                                        TRUE : FALSE);
                  }
                }
                else
                {
                  /* PBM_SEARCHTYPE_STARTSWITH or PBM_SEARCHTYPE_EXACT */
                  if ((flags & PBM_MATCHING_MASK) == PBM_MATCHING_NORMAL)
                  {
                    /* PBM_MATCHING_NORMAL */
                    if(pbm_utils_is_phone_field(field_id))
                    {
                      match = (pbm_strncmp(local_number,
                                          (void *) (cached_entry_data + offset_for_cache_number),
                                           num_chars_given,
                                           case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                    else
                    {
                      match = (pbm_strncmp(data_ptr,
                                           (void *)cached_entry_data,
                                           num_chars_given, case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                  }
                  else
                  {
                    /* PBM_MATCHING_GREATER */
                    if(pbm_utils_is_phone_field(field_id))
                    {
                      match = (pbm_strncmp(local_number,
                                           (void *)(cached_entry_data + offset_for_cache_number),
                                           num_chars_given,
                                           case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                    else
                    {
                      match = (pbm_strncmp(data_ptr,
                                           (void *)cached_entry_data,
                                           num_chars_given,
                                           case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                  }
                }

                if (match && ((flags & PBM_SEARCHTYPE_MASK) == PBM_SEARCHTYPE_EXACT))
                {
                  if (num_chars_in_record != num_chars_given)
                  {
                    UIM_MSG_MED_2("Excat match failed, num_chars_in_record 0x%x num_chars_given 0x%x",
                                   num_chars_in_record, num_chars_given);
                    match = FALSE;
                  }
                }
              }
              else
              {
                /* PBM_DATATYPE_BYTE, Always EXACT */
                PBM_CHECK_PTR_RET(sfield, FALSE);
                if(pbm_utils_is_phone_field(field_id))
                {
                  match = (memcmp(local_number,
                                  cached_entry_data + offset_for_cache_number,
                                  data_size) == 0 ? TRUE : FALSE);
                }
                else
                {
                  match = (memcmp(data_ptr,
                                  cached_entry_data,
                                  data_size) == 0 ? TRUE : FALSE);
                }
              }
            }/* match_numbers == TRUE */
          } /* if data_len != 0 */
        }  /* if field_id*/
      } /* for each sub field */
      pbm_enum_complex_free(result);
    }  /* for each field */
  }
  return match;
} /* match_found */


/*===========================================================================
FUNCTION pbm_get_active_app_type

DESCRIPTION
  This function returns the active session_type corresponding to this slot.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
mmgsdi_app_enum_type pbm_get_active_app_type(
  pbm_session_enum_type   session_type
)
{
  pbm_session_enum_type   i = PBM_SESSION_DEFAULT;
  pbm_slot_type           slot;

  rex_enter_crit_sect(&pbm_crit_sect);

  slot = (pbm_slot_type)pbm_session_info[session_type].slot_id;

  for ( i = PBM_SESSION_DEFAULT; i < PBM_SESSION_MAX; i++ )
  {
    if ( pbm_session_info[i].slot_id == (uint16) slot
          && pbm_session_info[i].app_type != MMGSDI_APP_NONE
          && pbm_session_info[i].app_type != MMGSDI_APP_UNKNOWN
          && (i == session_type || PBM_LPB != pbm_session_type_to_pb_category(session_type)))
    {
      rex_leave_crit_sect(&pbm_crit_sect);
      return pbm_session_info[i].app_type;
    }
  }

  rex_leave_crit_sect(&pbm_crit_sect);

  return MMGSDI_APP_UNKNOWN;
}/* pbm_get_active_app_type */


/*===========================================================================
FUNCTION PBM_CACHE_RECORD_NEW

DESCRIPTION
  This function allocates a new cache record, initializes it 0 and sets the
  number of reference to 1.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_addr_cache_s_type *pbm_cache_record_new(
  void
)
{
  pbm_addr_cache_s_type *record = NULL;

  PBM_MEM_ALLOC(record, sizeof(pbm_addr_cache_s_type));

  PBM_CHECK_PTR_RET(record, NULL);

  memset(record, 0, sizeof(pbm_addr_cache_s_type));

  record->nref = 1;

  return record;
}/* pbm_cache_record_new */


/*===========================================================================
FUNCTION PBM_PB_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given pb_id.
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
PB_CACHE *pbm_pb_id_to_cache(
  pbm_phonebook_type   pb_id
)
{
  PB_CACHE                *cache        = NULL;
  pbm_session_enum_type    session_type = PBM_SESSION_DEFAULT;

  if(pbm_action_requires_sim(pb_id))
  {
    //need to calculate the session-type only for SIM phonebooks
    session_type = pbm_mapping_to_session(pb_id.pb_category, (pbm_slot_type)pb_id.slot_id,
                                          pb_id.prov_type);
    PBM_VALIDATE_SESSION_RET(session_type,NULL);
  }
  cache = pbm_device_id_to_cache(session_type, pb_id.device_type);
  return cache;
}/* pbm_pb_id_to_cache */


/*===========================================================================
FUNCTION PBM_GET_FIRST_AVAILABLE

DESCRIPTION
Gets the first slot that is empty.  Returns the start index
for circular caches, or returns the index of first unused
area for non-circular.  Finally, returns 0 if not available

In case of deletion of circular cache, this functions returns the last location.
This is not valid for non-circular caches.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_get_first_available(
  PB_CACHE         *pb_cache,
  boolean           delete_only
)
{
  pbm_addr_cache_s_type **cache;
  int                     cache_size;
  uint16                  location;

  PBM_CHECK_PTR_RET(pb_cache, 0);

  if (!pb_cache->circular && delete_only)
  {
    UIM_MSG_ERR_1("PBM Deletion of first available not possible for cache 0x%x",
                  pb_cache);
    return 0;
  }

  cache = pb_cache->pb_cache_array;
  cache_size = pb_cache->num_of_records;

  if (pb_cache->circular)
  {
    if (delete_only)
      return(uint16) PBM_CIRC_CACHE_LASTLOC(pb_cache);
    else
      return(uint16) PBM_CIRC_CACHE_NEWLOC(pb_cache);
  }

  for (location = 1; location <= cache_size; location++)
  {
    pbm_record_id_type rec_id;

	rec_id = pbm_session_location_to_record_id(pb_cache->pb_id,location);
    //if this is location is a hidden location,then we should not treat it as a empty location
    if(pbm_is_rec_hidden(rec_id))
    {
      continue;
    }
    if (cache == NULL || (cache[location] == NULL) || (cache[location]->num_fields == 0))
      break;
  }

  if (location > cache_size)  /* No empty slot was found*/
  {
    return 0;
  }

  return location;
}/* pbm_get_first_available */


/*===========================================================================
FUNCTION PBM_CACHE_ADD

DESCRIPTION
Adds the given record to the given cache.  First allocates a new record
and copies over the info. from the given record.

If a record already exists at the location specified by the record ID, then
the existing record is first deleted before the new record is added at that
same location.

If the "location" part of the record ID is 0, then the record is placed in
the first available location and the record ID is updated accordingly.  (This
is a 1-based array.)

new_flag is used to support updating existing records.

DEPENDENCIES
  Must be called from within pbm critical section.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_add(
  PB_CACHE               *pb_cache,
  pbm_addr_cache_s_type  *record,
  boolean                 new_flag
)
{
  pbm_addr_cache_s_type **cache;
  int                     cache_size;
  uint16                  location;

  PBM_CHECK_PTR3_RET(pb_cache, pb_cache->pb_cache_array, record, PBM_ERROR);

  if (record->nref == 0)
  {
    UIM_MSG_ERR_1("Trying to add invalid record %d", record->record_id);
    return PBM_ERROR;
  }

  cache = pb_cache->pb_cache_array;
  cache_size = pb_cache->num_of_records;

  /* Where is it stored? */
  location = pbm_record_id_to_location(record->record_id);

  if (location > cache_size || location == 0)
  {
    UIM_MSG_ERR_1("PBM: Invalid record ID %d for phonebook.", location);
    return PBM_ERROR_INDEX_INVALID;
  }

  /* If that location is in use, delete entry before overwriting*/
  if (cache[location] != NULL)
  {
    if ( pbm_cache_delete(pb_cache, record->record_id) != PBM_SUCCESS )
    {
      return PBM_ERROR;
    }
  }

  /* Save the record in the cache location. */
  cache[location] = record;

  /* Update records in use only if we are deleting a non-dummy record */
  if (cache[location]->num_fields)
  {
    pb_cache->records_in_use++;
  }

  /* If new_flag is false, that means we are modifying an existing entry. */
  /* BREW supports this odd thing, but this flag makes it work. */
  if (pb_cache->circular && new_flag)
  {
    PBM_CIRC_CACHE_ADDSTART(pb_cache);       /* new_flag is true, so this
                                                changes where we point. */
  }

  return PBM_SUCCESS;
}/* pbm_cache_add */


/*===========================================================================
FUNCTION PBM_CACHE_RECORD_FREE

DESCRIPTION
  This function frees a cache record

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint8 pbm_cache_record_free(
  pbm_addr_cache_s_type     *rec
)
{
  if (!rec)
    return 0;

  if (--rec->nref)
    return rec->nref;

  pbm_free_fields_internal(&rec->fields, rec->num_fields);
  PBM_MEM_FREEIF(rec);

  return 0;
} /* pbm_cache_record_free */


/*===========================================================================
FUNCTION PBM_CACHE_DELETE

DESCRIPTION
  Deletes the record with the given record ID from the given cache.  The record
  is also free'ed.

DEPENDENCIES
  Must be called from within pbm critical section.

  Make sure that USIM fields are overwritten/deleted before the
  corresponding ADN fields. The field data resides as part of ADN record
  except after init.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_delete(
  PB_CACHE             *pb_cache,
  pbm_record_id_type    record_id
)
{
  pbm_addr_cache_s_type    **cache;
  int                        cache_size;
  int                        location;
  uint8                      ref;

   PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

  cache = pb_cache->pb_cache_array;
  cache_size = pb_cache->num_of_records;

  location = pbm_record_id_to_location(record_id);

  if ( (location > cache_size) || (location < 1) )
  {
    UIM_MSG_ERR_1("Could not delete a location from record_id 0x%x",
                  record_id);
    return PBM_ERROR_INDEX_INVALID;
  }

  if (!cache || !cache[location])
  {
    UIM_MSG_ERR_1("Record not found at location from record_id 0x%x",
                  record_id);
    return PBM_ERROR_NOT_FOUND;
  }

  /* Update records in use only if we are deleting a non-dummy record */
  if (cache[location]->num_fields)
  {
    /* If we are deleting the firt entry in a circular cache, "decrement" value
     * of start. Currently this can happen only while recovering from an Add
     * failure on first available location. */
    if (pb_cache->circular && location == PBM_CIRC_CACHE_FIRSTLOC(pb_cache))
        PBM_CIRC_CACHE_DELSTART(pb_cache);

    pb_cache->records_in_use--;
  }

  ref = pbm_cache_record_free(cache[location]);
  if (ref != 0)
  {
    UIM_MSG_MED_2("%d references remaining in record %d", ref, record_id);
  }

  cache[location] = NULL;

  return PBM_SUCCESS;
}/* pbm_cache_delete */


/*===========================================================================
FUNCTION PBM_FILL_PB_CACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB cache
  when info comes back from MMGSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SESSION are set.
===========================================================================*/
void pbm_fill_pb_cache_info(
  PB_CACHE                         *cache,
  pbm_session_enum_type             session_type,
  pbm_pb_set_type                   pb_set,
  int                               record_length,
  int                               min_record_length,
  pbm_file_type                     file_id,
  mmgsdi_file_structure_enum_type   file_type,
  boolean                           increment_num_files,
  int                               num_records,
  const mmgsdi_file_security_type  *write
)
{
  int                   i = 0;
  mmgsdi_pin_enum_type  pin_type;

  PBM_VALIDATE_SESSION_RET(session_type,VOID);
  if ( ! pbm_validate_pb_set(pb_set) )
  {
    return;
  }

  if (record_length >= min_record_length)
  {
    cache[session_type].file_ids[pb_set]  = file_id;
    cache[session_type].file_type         = file_type;

    if (increment_num_files)
    {
      cache[session_type].num_files++;
      cache[session_type].num_of_records += num_records;
    } else
    {
      cache[session_type].num_files = 1;
      cache[session_type].num_of_records = num_records;
    }

    if(pb_set != PBM_USIM_PB_INVALID)
    {
      cache[session_type].record_length[pb_set] = record_length;
      cache[session_type].records_in_pb_set[pb_set] = num_records;
      if (cache[session_type].pb_id.device_type != PBM_GRPNAME && cache[session_type].pb_id.device_type != PBM_AAS)
      {
        cache[session_type].text_length[pb_set] = record_length - min_record_length;
      } 
      else
      {
        cache[session_type].text_length[pb_set] = record_length;
      }
    }
      
    if (cache[session_type].num_of_records > 0)
    {
      cache[session_type].status = PBM_STATUS_NOT_READY;
    }
    else
      PBM_SET_PB_CACHE_READY(&cache[session_type]);

	/* add write protection related status here */
	if(write != NULL)
	{
	  cache[session_type].write.protection_method = write->protection_method;
	  cache[session_type].write.num_protection_pin = write->num_protection_pin;
	  for(i=0; i < write->num_protection_pin; i++)
	  {
		pin_type = write->protection_pin_ptr[i];
		if(pin_type == MMGSDI_PIN1)
		{
		  cache[session_type].write.pin_protection_mask |= (1 << PBM_PIN_PIN1);
		}
		else if(pin_type == MMGSDI_PIN2)
		{
		  cache[session_type].write.pin_protection_mask |= (1 << PBM_PIN_PIN2);
		}
		else if(pin_type == MMGSDI_ADM1)
		{
		  cache[session_type].write.pin_protection_mask |= (1 << PBM_PIN_ADM);
		} /* need to check what adm pin id to be used, currently using ADM1*/		
      }
	}
  } else
  {
    UIM_MSG_ERR_3("Bad data received from SIM 0x%x 0x%x 0x%x",
                  session_type,
                  file_id,
                  record_length);
    cache[session_type].status = PBM_STATUS_NOT_AVAILABLE;
  }
}/* pbm_fill_pb_cache_info */


/*===========================================================================
FUNCTION PBM_FILE_CIRCULAR

DESCRIPTION
  This function returns TRUE if the init file is circular, FALSE otherwise.

SIDE EFFECTS
===========================================================================*/
boolean pbm_file_circular(
  pbm_file_type    file_id
)
{
  switch (file_id)
  {
    case PBM_FILE_LND:
    case PBM_FILE_OCI:
    case PBM_FILE_ICI:
      return TRUE;

    default:
      return FALSE;
  }
} /* pbm_file_circular */


/*===========================================================================
FUNCTION       pbm_find_provioning_sesssion_in_queue

DESCRIPTION
This function handles session deactivation event from MMGSDI.

RETURN VALUE
  pbm_session_enum_type

DEPENDENCIES

===========================================================================*/
pbm_session_enum_type pbm_find_provioning_sesssion_in_queue (
  pbm_session_enum_type deactivate_session
)
{
  uint16 loop_session,slot_id;
  
  slot_id = pbm_session_info[deactivate_session].slot_id;
  for(loop_session = 0;loop_session< PBM_SESSION_MAX;loop_session++)
  {
    if((pbm_session_type_to_pb_category(loop_session) == PBM_LPB) && 
       (slot_id == pbm_session_info[loop_session].slot_id) && 
       (loop_session != deactivate_session))
    {
      UIM_MSG_HIGH_3("PBM match_found for deactivate_session %d slot_id %d deact_session_complement %d",
                      deactivate_session,slot_id,loop_session);
      break;
    }
  }

  return (pbm_session_enum_type)loop_session;
}/* pbm_find_provioning_sesssion_in_queue */


/*===========================================================================
FUNCTION PBM_FILE_ID_TO_FIELD_ID

DESCRIPTION
  Returns the field id stored within the given file device.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_id_e_type pbm_file_id_to_field_id(pbm_file_type file_id)
{
   switch (file_id)
   {
      case PBM_FILE_EMAIL:
      case PBM_FILE_EMAIL1:
      case PBM_FILE_EMAIL2:
      case PBM_FILE_EMAIL3:
         return PBM_FIELD_EMAIL;

      case PBM_FILE_SNE:
      case PBM_FILE_SNE1:
      case PBM_FILE_SNE2:
      case PBM_FILE_SNE3:
         return PBM_FIELD_OTHERNAME;

      case PBM_FILE_GRP:
      case PBM_FILE_GRP1:
      case PBM_FILE_GRP2:
      case PBM_FILE_GRP3:
         return PBM_FIELD_GRPNAME_ID;

      case PBM_FILE_ANR:
      case PBM_FILE_ANRA:
      case PBM_FILE_ANRB:
      case PBM_FILE_ANRC:
      case PBM_FILE_ANR1:
      case PBM_FILE_ANRA1:
      case PBM_FILE_ANRB1:
      case PBM_FILE_ANRC1:
      case PBM_FILE_ANR2:
      case PBM_FILE_ANRA2:
      case PBM_FILE_ANRB2:
      case PBM_FILE_ANRC2:
      case PBM_FILE_ANR3:
      case PBM_FILE_ANRA3:
      case PBM_FILE_ANRB3:
      case PBM_FILE_ANRC3:
         return PBM_FIELD_ADDITIONAL_NUMBER_EXT;
      case PBM_FILE_ECC:
         return PBM_FIELD_SIM_ECC;

      default:
         return PBM_FIELD_NONE;
   }
}


/*===========================================================================
FUNCTION PBM_FILE_ID_TO_PB_ID

DESCRIPTION
  This function will find the phonebook associated with a given init file name
  and sim id.
  It returns PBM_DEFAULT if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_file_id_to_pb_id(pbm_session_enum_type session_type,
                                        pbm_file_type file_id)
{
   pbm_phonebook_type pb_id = {PBM_GPB};

   pb_id.slot_id = pbm_session_info[session_type].slot_id;
   pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
   pb_id.prov_type = pbm_session_type_to_provision_type(session_type);

   switch (file_id)
   {
      case PBM_FILE_ADN:
      case PBM_FILE_ADN1:
      case PBM_FILE_ADN2:
      case PBM_FILE_ADN3:
         pb_id.device_type = PBM_ADN;
         break;

      case PBM_FILE_FDN:
         pb_id.device_type = PBM_FDN;
         break;

      case PBM_FILE_LND:
         pb_id.device_type = PBM_LND;
         break;

      case PBM_FILE_SDN:
         pb_id.device_type = PBM_SDN;
         break;

      case PBM_FILE_MSISDN:
         pb_id.device_type = PBM_MSISDN;
         break;

      case PBM_FILE_MBN:
         pb_id.device_type = PBM_MBN;
         break;

      case PBM_FILE_GAS:
         pb_id.device_type = PBM_GRPNAME;
         break;
      case PBM_FILE_AAS:
         pb_id.device_type = PBM_AAS;
         break;
      case PBM_FILE_ICI:
         pb_id.device_type = PBM_ICI;
         break;

      case PBM_FILE_OCI:
         pb_id.device_type = PBM_OCI;
         break;

      case PBM_FILE_MBDN:
         pb_id.device_type = PBM_MBDN;
         break;
      case PBM_FILE_ECC:
         pb_id.device_type = PBM_ECC;
         break;
      default:
         UIM_MSG_ERR_1("pbm_file_id_to_pb_id called for invalid file %d", file_id);
         pb_id.device_type = PBM_DEFAULT;
         break;
   }
   return pb_id;
}


/*===========================================================================
FUNCTION PBM_CAT_TO_PHONEBOOK

DESCRIPTION
  This function returns the phonebook associated with a given category.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_cat_to_phonebook(pbm_cat_e_type cat)
{
   switch (cat)
   {
      case PBM_CAT_BUSINESS:
      case PBM_CAT_PERSONAL:
         return PBM_EFS;

      case PBM_CAT_SIM_ADN:
         return PBM_ADN;

      case PBM_CAT_SIM_FDN:
         return PBM_FDN;

      case PBM_CAT_SIM_BDN:
         return PBM_BDN;

      case PBM_CAT_SDL:
         return PBM_SDL;

      case PBM_CAT_INCOMING:
         return PBM_RCVD;

      case PBM_CAT_OUTGOING:
         return PBM_DIALED;

      case PBM_CAT_MISSED:
         return PBM_MISS;

      case PBM_CAT_SIM_SDN:
         return PBM_SDN;

      case PBM_CAT_SIM_MSISDN:
         return PBM_MSISDN;

      case PBM_CAT_SIM_MBN:
         return PBM_MBN;

      case PBM_CAT_ECC:
         return PBM_ECC;

      case PBM_CAT_GRPNAME:
         return PBM_GRPNAME;
      case PBM_CAT_AAS:
         return PBM_AAS;
      case PBM_CAT_SIM_MBDN:
         return PBM_MBDN;


      default:
         UIM_MSG_ERR_1("Unexpected category %d", cat);
         return PBM_DEFAULT;
   }
}


/*===========================================================================
FUNCTION PBM_PB_ID_TO_FIELDGROUP

DESCRIPTION
  This function returns the field info group corresponding to this phonebook.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_pb_id_to_fieldgroup(pbm_phonebook_type pb_id)
{
   switch (pb_id.device_type)
   {
      case PBM_FDN:
         return FDN_GROUP;
      case PBM_BDN:
      case PBM_SDN:
      case PBM_MSISDN:
      case PBM_MBN:
      case PBM_MBDN:
         return SIM_GROUP;
      case PBM_GRPNAME:
         return GRPNAME_GROUP;
      case PBM_AAS:
         return AAS_GROUP;

      case PBM_ADN:
         return ADN_GROUP;

      case PBM_DIALED:
      case PBM_RCVD:
      case PBM_MISS:
      case PBM_LND:
      case PBM_OCI:
      case PBM_ICI:
         return CALLHISTORY_GROUP;

      case PBM_EFS:
      case PBM_DEFAULT:
         return EFS_GROUP;

      case PBM_SDL:
         return SDL_GROUP;

      case PBM_ECC:
         return ECC_GROUP;
      case PBM_ECC_OTHER:
        return ECC_OTHER_GROUP;

      default:
         UIM_MSG_ERR_1("Unexpected phonebook device %d", pb_id.device_type);
         return INVALID_GROUP;
   }
}


/*===========================================================================
FUNCTION pbm_pb_type_and_slot_to_session_type

DESCRIPTION
retrieves session id from record info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_pb_type_and_slot_to_session_type(
  pbm_slot_type                   slot_id,
  pbm_pb_category_enum_type       pb_type
)
{
  if ( PBM_GPB == pb_type )
  {
    if ( PBM_SLOT_1 == slot_id )
    {
      return PBM_SESSION_GPB_1;
    }
    else if ( PBM_SLOT_2 == slot_id )
    {
      return PBM_SESSION_GPB_2;
    }
#ifdef FEATURE_TRIPLE_SIM
    else if ( PBM_SLOT_3 == slot_id )
    {
      return PBM_SESSION_GPB_3;
    } 
#endif /* FEATURE_TRIPLE_SIM */
  }
  return PBM_SESSION_DEFAULT;
}/* pbm_pb_type_and_slot_to_session_type */


/*===========================================================================
FUNCTION PBM_SESSION_ID_TO_PROVISION_TYPE

DESCRIPTION
Converts a session ID to provision type

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_provision_enum_type pbm_session_type_to_provision_type(
  pbm_session_enum_type session_type
)
{
  pbm_provision_enum_type res = PBM_PROVISION_NONE;

  switch (session_type)
  {
    case PBM_SESSION_LPB_GW:
      res = PBM_PROVISION_GW;
      break;

    case PBM_SESSION_LPB_1X:
      res = PBM_PROVISION_1X;
      break;

#ifdef FEATURE_DUAL_SIM
    case PBM_SESSION_LPB_GW_SECONDARY:
      res = PBM_PROVISION_GW_SECONDARY;
      break;

    case PBM_SESSION_LPB_1X_SECONDARY:
      res = PBM_PROVISION_1X_SECONDARY;
      break;
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
    case PBM_SESSION_LPB_GW_TERTIARY:
      res = PBM_PROVISION_GW_TERTIARY;
      break;

    case PBM_SESSION_LPB_1X_TERTIARY:
      res = PBM_PROVISION_1X_TERTIARY;
      break;
#endif /* FEATURE_TRIPLE_SIM */

    case PBM_SESSION_GPB_1:
#ifdef FEATURE_DUAL_SIM
     case PBM_SESSION_GPB_2:
#endif  /*FEATURE_DUAL_SIM*/
#ifdef FEATURE_TRIPLE_SIM
     case PBM_SESSION_GPB_3:
#endif /* FEATURE_TRIPLE_SIM */
     default:
       res = PBM_PROVISION_NONE;
       break;
   }
   return res;
} /* pbm_session_type_to_provision_type */


/*===========================================================================
FUNCTION pbm_session_type_to_pb_category

DESCRIPTION
  Converts a session_type to PB category

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_category_enum_type pbm_session_type_to_pb_category(
  pbm_session_enum_type session_type
)
{
  pbm_pb_category_enum_type res = PBM_GPB;

  switch (session_type)
  {
     case PBM_SESSION_GPB_1:
#ifdef FEATURE_DUAL_SIM
     case PBM_SESSION_GPB_2:
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
     case PBM_SESSION_GPB_3:
#endif /* FEATURE_TRIPLE_SIM */
        res = PBM_GPB;
        break;
     case PBM_SESSION_LPB_GW:
     case PBM_SESSION_LPB_1X:
#ifdef FEATURE_DUAL_SIM
     case PBM_SESSION_LPB_GW_SECONDARY:
     case PBM_SESSION_LPB_1X_SECONDARY:
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
     case PBM_SESSION_LPB_GW_TERTIARY:
     case PBM_SESSION_LPB_1X_TERTIARY:
#endif /* FEATURE_TRIPLE_SIM */
        res = PBM_LPB;
        break;
     default:
        break;
   }
   return res;
} /* pbm_session_type_to_pb_category */


/*===========================================================================
FUNCTION pbm_uim_device_type_to_phonebook_type

DESCRIPTION
  This function is to derive UIM device type to phonebook type.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_phonebook_type pbm_uim_device_type_to_phonebook_type(
  pbm_uim_app_enum_type uim_type,
  pbm_device_type       device_type
)
{
   pbm_phonebook_type     pb_id         = {PBM_GPB};
   pbm_session_enum_type  session_type  = PBM_SESSION_DEFAULT;

   switch (uim_type)
   {
      case PBM_UIM_APP_LPB_GW_PROV:
         session_type = PBM_SESSION_LPB_GW;
      break;

      case PBM_UIM_APP_LPB_1X_PROV:
         session_type = PBM_SESSION_LPB_1X;
      break;

#ifdef FEATURE_DUAL_SIM
      case PBM_UIM_APP_GPB_SLOT_2:
         session_type = PBM_SESSION_GPB_2;
         break;

      case PBM_UIM_APP_LPB_GW_SECONDARY:
         session_type = PBM_SESSION_LPB_GW_SECONDARY;
         break;

      case PBM_UIM_APP_LPB_1X_SECONDARY:
         session_type = PBM_SESSION_LPB_1X_SECONDARY;
         break;
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
      case PBM_UIM_APP_GPB_SLOT_3:
         session_type = PBM_SESSION_GPB_3;
         break;

      case PBM_UIM_APP_LPB_GW_TERTIARY:
         session_type = PBM_SESSION_LPB_GW_TERTIARY;
         break;

      case PBM_UIM_APP_LPB_1X_TERTIARY:
         session_type = PBM_SESSION_LPB_1X_TERTIARY;
         break;
#endif /* FEATURE_TRIPLE_SIM */

      case PBM_UIM_APP_GPB_SLOT_1:
      default:
         session_type = PBM_SESSION_DEFAULT;
      break;
   }

   pb_id.prov_type = pbm_session_type_to_provision_type(session_type);
   if(session_type == PBM_SESSION_GPB_1)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_1;
   }
   else if(session_type == PBM_SESSION_GPB_2)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_2;
   }
#ifdef FEATURE_TRIPLE_SIM
   else if(session_type == PBM_SESSION_GPB_3)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_3;
   }
#endif /* FEATURE_TRIPLE_SIM */
   pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
   pb_id.device_type = device_type;

   return pb_id;
} /* pbm_uim_device_type_to_phonebook_type */


/*===========================================================================
FUNCTION pbm_session_record_id_to_phonebook_type

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_session_record_id_to_phonebook_type(
  pbm_record_id_type    record_id
)
{
   uint32             phonebook = 0;
   pbm_phonebook_type pb_id     = {PBM_GPB};

   /* Get upper 5 bits of record ID*/
   phonebook = (record_id >> PBM_NUM_LOC_BITS) & PBM_PB_MASK;

   if(phonebook < ARR_SIZE(pbm_pb_bits_to_pb_id))
   {
     pb_id.device_type = pbm_pb_bits_to_pb_id[phonebook];
   }

   if (pb_id.device_type == PBM_RESERVED)
   {
      UIM_MSG_ERR_1("Unknown phonebook code 0x%x.", phonebook);
      pb_id.device_type = PBM_EFS;
   }

   /* retrieve provision */
   pb_id.prov_type = (pbm_provision_enum_type)((record_id >> (PBM_NUM_SLOT_BITS + PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) & PBM_PROV_MASK);

   /* retrieve slot */
   pb_id.slot_id = (record_id >> (PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) & PBM_SLOT_MASK;

   /* retrieve pb category */
   pb_id.pb_category = (pbm_pb_category_enum_type)((record_id >> (PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) & PBM_PB_TYPE_MASK);

   return pb_id;
} /* pbm_session_record_id_to_phonebook_type */


/*===========================================================================
FUNCTION PBM_CALCULATE_SIZE_FROM_FIELDS

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  passed in.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint32 pbm_calculate_size_from_fields(
  const pbm_field_s_type *fields,
  uint32                  num_fields
)
{
  uint32 total_size = 0;
  uint32 i          = 0;

  PBM_CHECK_PTR_RET(fields, 0);

  if (num_fields > 0)
  {
    total_size = (int) ALIGNED_SIZE ((uint32) num_fields * sizeof(pbm_field_s_type));
    for (i = 0; i < num_fields; i++)
    {
      total_size += ALIGNED_SIZE ((fields)[i].data_len);
    }
  }

  return total_size;
} /* pbm_calculate_size_from_fields */

