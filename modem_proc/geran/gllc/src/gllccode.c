/*****************************************************************************
***
*** TITLE   gllccode.c
***
***  GPRS LLC Layer Frame FCS and Cipher Coding Functions
***
***
*** DESCRIPTION
***
***  Provides a suite of performance critical functions for cipher
***  coding/decoding and frame check sequence (FCS) calculation.
***
***  For use during LLC frame assembly/disassembly activities.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gllc_ass_frame_header_fcs()
***  gllc_ass_frame_info_fcs()
***  gllc_ass_frame_info_fcs_and_encipher()
***
***  gllc_dis_frame_header_fcs()
***  gllc_dis_frame_info_fcs()
***  gllc_dis_frame_info_fcs_and_decipher()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gllc/src/gllccode.c#3 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/12/13   ra      CR563511
*** 21/08/13   npt     CR531633 - Remove FEATURE_DIME_MODEM dependencies
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "gprs_pdu.h"
#include "gllci.h"
#include "msg.h"
#include "rr_public_store.h"
#include "geran_nv.h"

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
#include "gl1_hw_gsac.h"
#include "mcs_hwio.h"
#endif

#include "gl1_hw_clk_ctl.h"

#include "gllcswgea34.h"

/*
#if defined TEST_FRAMEWORK
#include <qtf_stub.h>
  TF_PROTO_STUB( void, rr_read_public_store, rr_public_store_id_t);
  #define rr_read_public_store TF_STUB(rr_read_public_store)
#endif
*/

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

#define GLLC_FCS_SEED     0xFFFFFF00UL

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

#ifdef DEBUG_GSM_GPRS_GEA_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_GEA_TEST */


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/* Pre-computed FCS transformation table using generator polynomial:

   G(x) = x24+x23+x21+x20+x19+x17+x16+x15+x13+x8+x7+x5+x4+x2+1 = 0x01BBA1B5.

   NOTE - This table assumes a reversed octet presentation. */

LOCAL const uint32 gllc_fcs_transform[256] =
{
  0x00000000UL, 0xD6A77600UL, 0xF6455700UL, 0x20E22100UL,
  0xB7811500UL, 0x61266300UL, 0x41C44200UL, 0x97633400UL,
  0x34099100UL, 0xE2AEE700UL, 0xC24CC600UL, 0x14EBB000UL,
  0x83888400UL, 0x552FF200UL, 0x75CDD300UL, 0xA36AA500UL,
  0x68132200UL, 0xBEB45400UL, 0x9E567500UL, 0x48F10300UL,
  0xDF923700UL, 0x09354100UL, 0x29D76000UL, 0xFF701600UL,
  0x5C1AB300UL, 0x8ABDC500UL, 0xAA5FE400UL, 0x7CF89200UL,
  0xEB9BA600UL, 0x3D3CD000UL, 0x1DDEF100UL, 0xCB798700UL,
  0xD0264400UL, 0x06813200UL, 0x26631300UL, 0xF0C46500UL,
  0x67A75100UL, 0xB1002700UL, 0x91E20600UL, 0x47457000UL,
  0xE42FD500UL, 0x3288A300UL, 0x126A8200UL, 0xC4CDF400UL,
  0x53AEC000UL, 0x8509B600UL, 0xA5EB9700UL, 0x734CE100UL,
  0xB8356600UL, 0x6E921000UL, 0x4E703100UL, 0x98D74700UL,
  0x0FB47300UL, 0xD9130500UL, 0xF9F12400UL, 0x2F565200UL,
  0x8C3CF700UL, 0x5A9B8100UL, 0x7A79A000UL, 0xACDED600UL,
  0x3BBDE200UL, 0xED1A9400UL, 0xCDF8B500UL, 0x1B5FC300UL,
  0xFB473300UL, 0x2DE04500UL, 0x0D026400UL, 0xDBA51200UL,
  0x4CC62600UL, 0x9A615000UL, 0xBA837100UL, 0x6C240700UL,
  0xCF4EA200UL, 0x19E9D400UL, 0x390BF500UL, 0xEFAC8300UL,
  0x78CFB700UL, 0xAE68C100UL, 0x8E8AE000UL, 0x582D9600UL,
  0x93541100UL, 0x45F36700UL, 0x65114600UL, 0xB3B63000UL,
  0x24D50400UL, 0xF2727200UL, 0xD2905300UL, 0x04372500UL,
  0xA75D8000UL, 0x71FAF600UL, 0x5118D700UL, 0x87BFA100UL,
  0x10DC9500UL, 0xC67BE300UL, 0xE699C200UL, 0x303EB400UL,
  0x2B617700UL, 0xFDC60100UL, 0xDD242000UL, 0x0B835600UL,
  0x9CE06200UL, 0x4A471400UL, 0x6AA53500UL, 0xBC024300UL,
  0x1F68E600UL, 0xC9CF9000UL, 0xE92DB100UL, 0x3F8AC700UL,
  0xA8E9F300UL, 0x7E4E8500UL, 0x5EACA400UL, 0x880BD200UL,
  0x43725500UL, 0x95D52300UL, 0xB5370200UL, 0x63907400UL,
  0xF4F34000UL, 0x22543600UL, 0x02B61700UL, 0xD4116100UL,
  0x777BC400UL, 0xA1DCB200UL, 0x813E9300UL, 0x5799E500UL,
  0xC0FAD100UL, 0x165DA700UL, 0x36BF8600UL, 0xE018F000UL,
  0xAD85DD00UL, 0x7B22AB00UL, 0x5BC08A00UL, 0x8D67FC00UL,
  0x1A04C800UL, 0xCCA3BE00UL, 0xEC419F00UL, 0x3AE6E900UL,
  0x998C4C00UL, 0x4F2B3A00UL, 0x6FC91B00UL, 0xB96E6D00UL,
  0x2E0D5900UL, 0xF8AA2F00UL, 0xD8480E00UL, 0x0EEF7800UL,
  0xC596FF00UL, 0x13318900UL, 0x33D3A800UL, 0xE574DE00UL,
  0x7217EA00UL, 0xA4B09C00UL, 0x8452BD00UL, 0x52F5CB00UL,
  0xF19F6E00UL, 0x27381800UL, 0x07DA3900UL, 0xD17D4F00UL,
  0x461E7B00UL, 0x90B90D00UL, 0xB05B2C00UL, 0x66FC5A00UL,
  0x7DA39900UL, 0xAB04EF00UL, 0x8BE6CE00UL, 0x5D41B800UL,
  0xCA228C00UL, 0x1C85FA00UL, 0x3C67DB00UL, 0xEAC0AD00UL,
  0x49AA0800UL, 0x9F0D7E00UL, 0xBFEF5F00UL, 0x69482900UL,
  0xFE2B1D00UL, 0x288C6B00UL, 0x086E4A00UL, 0xDEC93C00UL,
  0x15B0BB00UL, 0xC317CD00UL, 0xE3F5EC00UL, 0x35529A00UL,
  0xA231AE00UL, 0x7496D800UL, 0x5474F900UL, 0x82D38F00UL,
  0x21B92A00UL, 0xF71E5C00UL, 0xD7FC7D00UL, 0x015B0B00UL,
  0x96383F00UL, 0x409F4900UL, 0x607D6800UL, 0xB6DA1E00UL,
  0x56C2EE00UL, 0x80659800UL, 0xA087B900UL, 0x7620CF00UL,
  0xE143FB00UL, 0x37E48D00UL, 0x1706AC00UL, 0xC1A1DA00UL,
  0x62CB7F00UL, 0xB46C0900UL, 0x948E2800UL, 0x42295E00UL,
  0xD54A6A00UL, 0x03ED1C00UL, 0x230F3D00UL, 0xF5A84B00UL,
  0x3ED1CC00UL, 0xE876BA00UL, 0xC8949B00UL, 0x1E33ED00UL,
  0x8950D900UL, 0x5FF7AF00UL, 0x7F158E00UL, 0xA9B2F800UL,
  0x0AD85D00UL, 0xDC7F2B00UL, 0xFC9D0A00UL, 0x2A3A7C00UL,
  0xBD594800UL, 0x6BFE3E00UL, 0x4B1C1F00UL, 0x9DBB6900UL,
  0x86E4AA00UL, 0x5043DC00UL, 0x70A1FD00UL, 0xA6068B00UL,
  0x3165BF00UL, 0xE7C2C900UL, 0xC720E800UL, 0x11879E00UL,
  0xB2ED3B00UL, 0x644A4D00UL, 0x44A86C00UL, 0x920F1A00UL,
  0x056C2E00UL, 0xD3CB5800UL, 0xF3297900UL, 0x258E0F00UL,
  0xEEF78800UL, 0x3850FE00UL, 0x18B2DF00UL, 0xCE15A900UL,
  0x59769D00UL, 0x8FD1EB00UL, 0xAF33CA00UL, 0x7994BC00UL,
  0xDAFE1900UL, 0x0C596F00UL, 0x2CBB4E00UL, 0xFA1C3800UL,
  0x6D7F0C00UL, 0xBBD87A00UL, 0x9B3A5B00UL, 0x4D9D2D00UL
};


/* ----------------------------------------------------------------------- */

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif

/* ----------------------------------------------------------------------- */

/* Cipher Stream Debug Utilities */

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

/* ======================================================================== */
/* GPRS GEA Hardware Accelerator Access Macros                              */
/* ======================================================================== */

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST

/* ----------------------------------------------------------------------- */

/* GSAC Clock Control */

#define GLLC_GSAC_CLOCK_IS_REQUIRED()  \
        gl1_hw_gsac_clock_is_required( GL1_HW_GSAC_CLOCK_GLLC_MASK )

#define GLLC_GSAC_CLOCK_NOT_REQUIRED() \
        gl1_hw_gsac_clock_not_required( GL1_HW_GSAC_CLOCK_GLLC_MASK )

/* ----------------------------------------------------------------------- */

/* GSAC Algorithm Selection */

#ifdef FEATURE_GLLC_EXPORT_COMPLIANT_CIPHERING
#define GLLC_GSAC_MODE( algo )                                               \
        (                                                                    \
          ( ((algo) == GMMLLC_GEA_1) ? 0x0:0x0 ) |                           \
          ( ((algo) == GMMLLC_GEA_2) ? 0x1:0x0 ) |                           \
          ( ((algo) == GMMLLC_GEA_3) ? 0x2:0x0 )                             \
        )
#else
#define GLLC_GSAC_MODE( algo )                                               \
        (                                                                    \
          ( ((algo) == GMMLLC_GEA_1) ? 0x0:0x0 ) |                           \
          ( ((algo) == GMMLLC_GEA_2) ? 0x1:0x0 ) |                           \
          ( ((algo) == GMMLLC_GEA_3) ? 0x2:0x0 ) |                           \
          ( ((algo) == GMMLLC_GEA_4) ? 0x3:0x0 )                             \
        )
#endif /* FEATURE_GLLC_EXPORT_COMPLIANT_CIPHERING */

/* ----------------------------------------------------------------------- */

/* GSAC Busy Waiting for GEA1/2 Algorithms */

#define GLLC_GSAC_WAIT_WHILE_BUSY( gea_is_3_or_4 )                           \
        {                                                                    \
          if ( !gea_is_3_or_4 )                                              \
          {                                                                  \
            while (HWIO_IN(AMBA_KEY_STRM_STATUS)) { /* Wait */ }             \
          }                                                                  \
        }

/* ----------------------------------------------------------------------- */

#else
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_GSAC_GET_CIPHER_STREAM()
===
===  DESCRIPTION
===
===    Obtains the next 4 octets of keystream from the GSAC hardware,
===    as a function of whether GEA1/2 or GEA3/4 ciphering is in operation.
===    The hardware register address for reading the next keystream word is
===    passed as a parameter. For GEA1/2 hardware, the hardware register
===    address is updated by indirection as the function returns.
===
===    GEA3 Keystream Acquisition Notes-
===
===      No GEA3 Status checking is made as keystream generation is quicker
===      than the time taken for the ARM to read it.
===
===    Per CT1 Clarification Notes -
===
===      The GEA3 keystream presentation to data is octet-wise with the
===      first bit of generated keystream XOR'd with bit (1,8) of the frame
===      format. This contrasts with GEA1 and GEA2 where the first bit of
===      generated keystream must be XOR'd with bit (1,1) of the frame.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Next four octets of keystream as a 32bit word
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

uint32 gllc_gsac_get_cipher_stream( uint32 **key_p, boolean gea_is_3_or_4 )
{
  uint32 keystream = 0;
  uint32 *ptr = NULL;

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
  /* --------------------------------------------------------------------- */

  if ( gea_is_3_or_4 )
  {
    /* Provide 4 octets of cipher key stream. Maximum PDU size as per spec is 1523 octets.
       Eight octet alignment, make it 1528. Then 1528/8 = 191. */
    if ( gllc_llme.key_stream_index < 191 )
    {
      keystream = gllc_llme.key_stream[ gllc_llme.key_stream_index ].uw[ gllc_llme.key_stream_index_sub++ ];

      if ( gllc_llme.key_stream_index_sub >= 2 )
      {
        gllc_llme.key_stream_index_sub = 0;
        gllc_llme.key_stream_index++;
      }
    }
    else
    {
      keystream = 0;
      MSG_GERAN_ERROR_2_G("GLC GSAC: Wrong index:  %d  %d", gllc_llme.key_stream_index, gllc_llme.key_stream_index_sub);
    }
  }

  /* --------------------------------------------------------------------- */

  else
  {
    if ( key_p != NULL )
    {
      ptr = *key_p;
    }

    if ( (ptr != NULL) && (ptr > (uint32 *)HWIO_ADDRI(AMBA_KEY_STRM_WORDn_RD, 7)) )
    {
      ptr = (uint32 *)HWIO_ADDRI(AMBA_KEY_STRM_WORDn_RD, 0);
    }

    if ( ptr != NULL )
    {
      keystream = in_dword( ptr++ );
    }

    if ( ( ptr != NULL ) && ( key_p != NULL ) )
    {
      *key_p = ptr;
    }
  }

  /* --------------------------------------------------------------------- */
#else
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
  /* --------------------------------------------------------------------- */

  return ( keystream );

} /* end of gllc_gsac_get_cipher_stream() */


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_IS_GEA_SUPPORTED()
===
===  DESCRIPTION
===
===    Determines whether the indicated GPRS Encryption Algorithm (GEA) is
===    supported.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if given GEA is supported, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

boolean gllc_is_gea_supported( gmmllc_gea_e_type gea )
{
  boolean gea_is_supported = FALSE;
  uint8   gea_nv_support   = 0;

  /* Obtain supported GEA algorithms as a bitmask via NV provisioning */
  rr_read_public_store( RRPS_MS_GPRS_GEA_SUPPORT, &gea_nv_support );

  switch ( gea )
  {
  /* --------------------------------------------------------------------- */

  case GMMLLC_GEA_NONE:
  case GMMLLC_GEA_UNDEFINED:

    gea_is_supported = TRUE;
    break;

  /* --------------------------------------------------------------------- */

  case GMMLLC_GEA_1:

    /* GEA1 support is identified by means of bitmask bit0 */
    gea_is_supported = ( gea_nv_support & 0x01 ) ? TRUE:FALSE;
    break;

  /* --------------------------------------------------------------------- */

  case GMMLLC_GEA_2:

    /* GEA2 support is identified by means of bitmask bit1 */
    gea_is_supported = ( gea_nv_support & 0x02 ) ? TRUE:FALSE;
    break;

  /* --------------------------------------------------------------------- */

  case GMMLLC_GEA_3:

    /* GEA3 support is identified by means of bitmask bit2 */
    gea_is_supported = ( gea_nv_support & 0x04 ) ? TRUE:FALSE;
    break;

  /* --------------------------------------------------------------------- */

  case GMMLLC_GEA_4:

    /* GEA3 support is identified by means of bitmask bit3 */
    gea_is_supported = ( gea_nv_support & 0x08 ) ? TRUE:FALSE;
    break;

  /* --------------------------------------------------------------------- */

  case GMMLLC_GEA_5:
  case GMMLLC_GEA_6:
  case GMMLLC_GEA_7:
  default:
    gea_is_supported = FALSE;

  /* --------------------------------------------------------------------- */
  }

  return ( gea_is_supported );

} /* end of gllc_is_gea_supported() */


/*===========================================================================
===
===  FUNCTION      GLLC_ASS_FRAME_HEADER_FCS
===
===  DESCRIPTION
===
===    Performs a partial FCS calculation over the given frame header and
===    length. The partial result is stored in the frame assembly working
===    structure.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

void gllc_ass_frame_header_fcs
(
  uint8   *hdr_p,
  uint16  hdr_len
)
{
  register uint32   ctr     = (uint32)hdr_len;
  register uint32   fcs     = GLLC_FCS_SEED;
  register uint8    *data_p = hdr_p;

  while ( ctr )
  {
    fcs = gllc_fcs_transform[((fcs >> 8) ^ *data_p++) & 0xFF] ^ (fcs >> 8);
    ctr--;
  }
  gllc_assy_info.fr_fcs = fcs;

} /* end of gllc_ass_frame_header_fcs() */


/*===========================================================================
===
===  FUNCTION      GLLC_ASS_FRAME_INFO_FCS
===
===  DESCRIPTION
===
===    Completes a frame FCS calculation over the remaining frame
===    information field and appends the FCS to the end of the information
===    field.
===
===  DEPENDENCIES
===
===    Performance requirements have tailored this algorithm to a little
===    endian target.
===
===    It is expected that the frame assembly working structure contains
===    partial FCS result which has already been calculated from the header.
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

void gllc_ass_frame_info_fcs
(
  dsm_item_type **pdu_p,
  uint16        fcs_len
)
{
  register uint32         fcs          = gllc_assy_info.fr_fcs;
  register dsm_item_type  *item_p      = *pdu_p;
  register dsm_item_type  *last_item_p = *pdu_p;
  register uint32         fcs_end_ctr  = fcs_len;

  register uint32         data;
  register uint32         *dat32_p;
  register uint8          *dat8_p;
  register uint32         ctr4;
  register uint32         ctr1;

  /* --------------------------------------------------------------------- */

  while ( item_p && fcs_end_ctr )
  {
    ctr1   = (uint32)item_p -> used;
    dat8_p = item_p -> data_ptr;

    if ( fcs_end_ctr < ctr1 )
      ctr1 = fcs_end_ctr;

    fcs_end_ctr -= ctr1;

    /* Align to long word address before commencing long word access. */

    while ( ((uint32)dat8_p & 0x3) && ctr1 )
    {
      fcs = gllc_fcs_transform[((fcs >> 8) ^ *dat8_p++) & 0xFF] ^ (fcs >> 8);
      ctr1--;
    }

    /* Now commence FCS calculation using long word little endian data
       access. */

    ctr4     = ctr1 >> 2;         /* # of whole long words in payload */
    ctr1    &= 0x3;               /* # of remaining octets in payload */
    dat32_p  = (uint32 *)dat8_p;  /* Set up long word pointer         */

    while ( ctr4 )
    {
      data = *dat32_p++;

      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      data >>= 8;
      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      data >>= 8;
      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      data >>= 8;
      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      ctr4--;
    }

    if ( ctr1 )
    {
      data = *dat32_p;

      do
      {
        fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
        data >>= 8;
        ctr1--;
      }
      while ( ctr1 );
    }

    last_item_p = item_p;
    item_p      = item_p -> pkt_ptr;
  }

  /* --------------------------------------------------------------------- */

  /* Find the end of the source chain if not already there. */

  while ( item_p )
  {
    last_item_p = item_p;
    item_p      = item_p -> pkt_ptr;
  }

  /* --------------------------------------------------------------------- */

  /* Prepare the FCS in a new DSM item, and append it to the tail of the
     source chain. */

  item_p = gprs_pdu_alloc(DSM_DS_SMALL_ITEM_POOL);

  if (item_p)
  {
    item_p -> used = 3;
    dat8_p         = item_p -> data_ptr;

    if ( last_item_p )
      last_item_p -> pkt_ptr = item_p;
    else
      *pdu_p = item_p;

    fcs         = (~fcs) >> 8;
    dat8_p[0]   = (uint8)(fcs & 0xFF);
    fcs       >>= 8;
    dat8_p[1]   = (uint8)(fcs & 0xFF);
    fcs       >>= 8;
    dat8_p[2]   = (uint8)(fcs & 0xFF);
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_ass_frame_info_fcs() */


/*===========================================================================
===
===  FUNCTION      GLLC_ASS_FRAME_INFO_FCS_AND_ENCIPHER
===
===  DESCRIPTION
===
===    Completes a frame FCS calculation over the remaining frame
===    information field, and generates an enciphered copy to which the
===    FCS is appended.
===
===  DEPENDENCIES
===
===    It is expected that the frame assembly working structure contains
===    partial FCS result which has already been calculated from the header.
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

void gllc_ass_frame_info_fcs_and_encipher
(
  dsm_item_type *src_pdu_p,
  dsm_item_type **des_pdu_p,
  uint16        frame_info_len,
  uint16        fcs_len,
  uint32        gea_input_seed
)
{
  register uint32         fcs             = gllc_assy_info.fr_fcs;
  register uint32         data;
  register uint32         ctr;
  register uint32         gea_is_3_or_4   = (uint32)FALSE;
  register uint32         keystream       = 0;
  register uint32         kstrm_ctr       = 0;
  register uint8          *in_dat_p;
  register uint8          *out_dat_p;
           uint32         *keystream_p;

           dsm_item_type  *in_item_p      = src_pdu_p;
           dsm_item_type  *last_item_p    = *des_pdu_p;
           dsm_item_type  *out_item_p     = NULL;

           uint16         encipher_and_fcs_count;
           uint16         encipher_only_count;

                          /* Point keystream_p at this dummy in cases
                             where ciphering is unsupported to prevent
                             compiler warnings. */

           uint32         dummy_keystream = 0;
  GfwGea34ControlStruct   control;

           vect64         key_ptr[2];

  /* --------------------------------------------------------------------- */

  NOTUSED(frame_info_len);

  /* Added blanket initialisation */
  keystream_p = &dummy_keystream;

  /* --------------------------------------------------------------------- */

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
  #error code not present
#endif

  /* --------------------------------------------------------------------- */

  /* For sharing a single cipher HW block by multiple GLLC tasks. */

  GLLC_CIPHER_LOCK();

  /* --------------------------------------------------------------------- */

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST

  /* Enable clock only for GEA1 and GEA2. */

  if ( (gllc_llme.gea == GMMLLC_GEA_1) ||
       (gllc_llme.gea == GMMLLC_GEA_2)    )
  {
    GLLC_GSAC_CLOCK_IS_REQUIRED();
  }

#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  /* --------------------------------------------------------------------- */

  /* Initialise the GEA hardware accelerator block to be used for
     generating the encipher keystream. Load message 'input', Kc and
     algorithm selection. Enable automatic stream generation. */
  {
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
    if ( (gllc_llme.gea == GMMLLC_GEA_1) ||
         (gllc_llme.gea == GMMLLC_GEA_2)    )
    {

      HWIO_OUT ( GEA_INPUT,   gea_input_seed      );
      HWIO_OUTI( GEA_KC_n, 1, gllc_llme.key_Kc[1] );
      HWIO_OUTI( GEA_KC_n, 0, gllc_llme.key_Kc[0] );

      HWIO_OUT(
      GEA_CONFIG_0,
        (
          ( 0   << HWIO_SHFT(GEA_CONFIG_0, GEA12_BIG_ENDIAN)         ) | ( 0 << HWIO_SHFT(GEA_CONFIG_0, GEA12_PREPEND_X_ZEROS)  ) |
          ( 0   << HWIO_SHFT(GEA_CONFIG_0, GEA_DIRECTION_DATA_BIT)   ) | ( 1 << HWIO_SHFT(GEA_CONFIG_0, GEA_SHIFT_INTO_MSB_ENA) ) |
          ( 0   << HWIO_SHFT(GEA_CONFIG_0, GEA12_KEY_STREAM_GEN_CMD) ) | ( 1 << HWIO_SHFT(GEA_CONFIG_0, GEA12_AUTOGEN_ENA)      ) |
          ( 1   << HWIO_SHFT(GEA_CONFIG_0, GEA12_32OCTETS_MODE)      ) | ( 1 << HWIO_SHFT(GEA_CONFIG_0, GEA_TRIGGER)            ) |
          ( GLLC_GSAC_MODE(gllc_llme.gea) << HWIO_SHFT(GEA_CONFIG_0, GEA_MODE) )
        )
      );

      keystream_p = (uint32 *)HWIO_ADDRI(AMBA_KEY_STRM_WORDn_RD, 0);
    }

    /* ..................................................................... */

    else if ( (gllc_llme.gea == GMMLLC_GEA_3) ||
              (gllc_llme.gea == GMMLLC_GEA_4)    )
    {
      gea_is_3_or_4 = (uint32)TRUE;


      /* Reset all parameters. */

      // memset( &control, 0, sizeof(GfwGea34ControlStruct) );


      /* Initialise parameters for this PDU. */

      control.bFirst             = TRUE;
      control.nBytesTotal        = (frame_info_len+3);
      control.nBytesPerCall      = (frame_info_len+3);


      /* Initialise keystream buffer. */

      gllc_llme.key_stream_index     = 0;
      gllc_llme.key_stream_index_sub = 0;
      memset( gllc_llme.key_stream, 0, sizeof(gllc_llme.key_stream));


      /* New function call here which provides all key stream octets required for this PDU. */

      if (gllc_llme.gea == GMMLLC_GEA_3)
      {

        key_ptr[0].ub[0] = (( gllc_llme.key_Kc[1] >>  24 ) & 0xFF );
        key_ptr[0].ub[1] = (( gllc_llme.key_Kc[1] >>  16 ) & 0xFF );
        key_ptr[0].ub[2] = (( gllc_llme.key_Kc[1] >>   8 ) & 0xFF );
        key_ptr[0].ub[3] = (( gllc_llme.key_Kc[1] >>   0 ) & 0xFF );
        key_ptr[0].ub[4] = (( gllc_llme.key_Kc[0] >>  24 ) & 0xFF );
        key_ptr[0].ub[5] = (( gllc_llme.key_Kc[0] >>  16 ) & 0xFF );
        key_ptr[0].ub[6] = (( gllc_llme.key_Kc[0] >>   8 ) & 0xFF );
        key_ptr[0].ub[7] = (( gllc_llme.key_Kc[0] >>   0 ) & 0xFF );

        gfw_gea_34( &key_ptr[0],
                    64,
                    gea_input_seed,
                    0,
                    &gllc_llme.key_stream[0],
                    &control);
      }
      else
      {


        key_ptr[1].ub[0] = (( gllc_llme.key_Kc128[1] >>  24 ) & 0xFF );
        key_ptr[1].ub[1] = (( gllc_llme.key_Kc128[1] >>  16 ) & 0xFF );
        key_ptr[1].ub[2] = (( gllc_llme.key_Kc128[1] >>   8 ) & 0xFF );
        key_ptr[1].ub[3] = (( gllc_llme.key_Kc128[1] >>   0 ) & 0xFF );
        key_ptr[1].ub[4] = (( gllc_llme.key_Kc128[0] >>  24 ) & 0xFF );
        key_ptr[1].ub[5] = (( gllc_llme.key_Kc128[0] >>  16 ) & 0xFF );
        key_ptr[1].ub[6] = (( gllc_llme.key_Kc128[0] >>   8 ) & 0xFF );
        key_ptr[1].ub[7] = (( gllc_llme.key_Kc128[0] >>   0 ) & 0xFF );

        key_ptr[0].ub[0] = (( gllc_llme.key_Kc128[3] >>  24 ) & 0xFF );
        key_ptr[0].ub[1] = (( gllc_llme.key_Kc128[3] >>  16 ) & 0xFF );
        key_ptr[0].ub[2] = (( gllc_llme.key_Kc128[3] >>   8 ) & 0xFF );
        key_ptr[0].ub[3] = (( gllc_llme.key_Kc128[3] >>   0 ) & 0xFF );
        key_ptr[0].ub[4] = (( gllc_llme.key_Kc128[2] >>  24 ) & 0xFF );
        key_ptr[0].ub[5] = (( gllc_llme.key_Kc128[2] >>  16 ) & 0xFF );
        key_ptr[0].ub[6] = (( gllc_llme.key_Kc128[2] >>   8 ) & 0xFF );
        key_ptr[0].ub[7] = (( gllc_llme.key_Kc128[2] >>   0 ) & 0xFF );

        gfw_gea_34( &key_ptr[0],
                    128,
                    gea_input_seed,
                    0,
                    &gllc_llme.key_stream[0],
                    &control);
      }

    }

    /* ..................................................................... */

    else
    {
      MSG_GERAN_LOW_1_G("GLC GSAC: GEA %d Unsupported ", gllc_llme.gea);
      keystream_p = &dummy_keystream;
    }
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* --------------------------------------------------------------------- */

    while ( in_item_p )
    {
      /* Acquire a new DSM item to receive the enciphered result of this
         DSM item. */

      out_item_p = gprs_pdu_alloc(DSM_DS_POOL_SIZE(in_item_p->used));

      if ( out_item_p == NULL )
      {
        GPRS_PDU_FREE( des_pdu_p );

        if ( (gllc_llme.gea == GMMLLC_GEA_1) ||
             (gllc_llme.gea == GMMLLC_GEA_2)    )
        {
        GLLC_GSAC_CLOCK_NOT_REQUIRED();
        }

        return;                                                /*** RETURN ***/
      }

      in_dat_p           = in_item_p -> data_ptr;
      out_dat_p          = out_item_p -> data_ptr;
      out_item_p -> used = in_item_p  -> used;

      if ( fcs_len >= in_item_p -> used )
      {
        encipher_and_fcs_count  = in_item_p -> used;
        encipher_only_count     = 0;
        fcs_len                -= encipher_and_fcs_count;
      }
      else
      {
        encipher_and_fcs_count  = fcs_len;
        encipher_only_count     = in_item_p -> used - fcs_len;
        fcs_len                 = 0;
      }

      /* ................................................................... */

      /* Perform combined enciphering and FCS calculation. */

      ctr = encipher_and_fcs_count;

      while ( ctr )
      {
        if ( !kstrm_ctr )
        {
          GLLC_GSAC_WAIT_WHILE_BUSY( gea_is_3_or_4 );
          keystream = gllc_gsac_get_cipher_stream( &keystream_p, gea_is_3_or_4 );
          kstrm_ctr = 4;

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
          #error code not present
#endif
        }

        data = (uint32)*in_dat_p++;
        fcs  = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);

        *out_dat_p++ = (uint8)(data ^ keystream);
        keystream  >>= 8;
        kstrm_ctr--;
        ctr--;
      }

      /* ................................................................... */

      /* Perform enciphering over any remainder. */

      ctr = encipher_only_count;

      while ( ctr )
      {
        if ( !kstrm_ctr )
        {
          GLLC_GSAC_WAIT_WHILE_BUSY( gea_is_3_or_4 );
          keystream = gllc_gsac_get_cipher_stream( &keystream_p, gea_is_3_or_4 );
          kstrm_ctr = 4;

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
          #error code not present
#endif
        }

        *out_dat_p++ = (uint8)((uint32)*in_dat_p++ ^ keystream);
        keystream  >>= 8;
        kstrm_ctr--;
        ctr--;
      }

      /* ................................................................... */

      /* Prepare for next item. */

      if ( last_item_p )
        last_item_p -> pkt_ptr = out_item_p;
      else
        *des_pdu_p = out_item_p;

      last_item_p = out_item_p;
      in_item_p   = in_item_p -> pkt_ptr;
    }

    /* --------------------------------------------------------------------- */

    /* Prepare the FCS in a new DSM item, encipher it, and append it to the
       tail of the copy chain. */

    out_item_p = gprs_pdu_alloc(DSM_DS_SMALL_ITEM_POOL);

    if ( out_item_p )
    {
      out_item_p -> used = 3;
      out_dat_p          = out_item_p -> data_ptr;

      if ( last_item_p )
        last_item_p -> pkt_ptr = out_item_p;
      else
        *des_pdu_p = out_item_p;

      fcs            = (~fcs) >> 8;
      out_dat_p[0]   = (uint8)(fcs & 0xFF);
      fcs          >>= 8;
      out_dat_p[1]   = (uint8)(fcs & 0xFF);
      fcs          >>= 8;
      out_dat_p[2]   = (uint8)(fcs & 0xFF);

      ctr = 3;
      while ( ctr )
      {
        if ( !kstrm_ctr )
        {
          GLLC_GSAC_WAIT_WHILE_BUSY( gea_is_3_or_4 );
          keystream = gllc_gsac_get_cipher_stream( &keystream_p, gea_is_3_or_4 );
          kstrm_ctr = 4;

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
          #error code not present
#endif
        }

        *out_dat_p++  ^= (uint8)keystream;
        keystream    >>= 8;
        kstrm_ctr--;
        ctr--;
      }
    }
    else
    {
      GPRS_PDU_FREE( des_pdu_p );
    }
  }

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST

  if ( (gllc_llme.gea == GMMLLC_GEA_1) ||
       (gllc_llme.gea == GMMLLC_GEA_2)    )
  {
    GLLC_GSAC_CLOCK_NOT_REQUIRED();
  }

#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */


  GLLC_CIPHER_UNLOCK();

  /* --------------------------------------------------------------------- */

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */

  /* --------------------------------------------------------------------- */
} /* end of gllc_ass_frame_info_fcs_and_encipher() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIS_FRAME_HEADER_FCS
===
===  DESCRIPTION
===
===    Performs a partial FCS calculation over the given frame header and
===    length. The partial result is stored in the frame disassembly working
===    structure.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

void gllc_dis_frame_header_fcs
(
  uint8   *hdr_p,
  uint16  hdr_len
)
{
  register uint32   ctr     = (uint32)hdr_len;
  register uint32   fcs     = GLLC_FCS_SEED;
  register uint8    *data_p = hdr_p;

  while ( ctr )
  {
    fcs = gllc_fcs_transform[((fcs >> 8) ^ *data_p++) & 0xFF] ^ (fcs >> 8);
    ctr--;
  }
  gllc_disassy_info.fr_fcs = fcs;

} /* end of gllc_dis_frame_header_fcs() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIS_FRAME_INFO_FCS
===
===  DESCRIPTION
===
===    Completes a frame FCS calculation over the remaining frame
===    information field contained in the given chain to the given length,
===    strips the FCS octets and tests for FCS error.
===
===  DEPENDENCIES
===
===    The given chain is expected to contain both the information and
===    FCS fields of the received frame, having had the header pre-stripped.
===
===    It is also expected that the frame disassembly working structure
===    contains partial FCS result which has already been calculated from
===    the header.
===
===    Performance requirements have tailored this algorithm to a little
===    endian target.
===
===  RETURN VALUE
===
===    TRUE if no FCS errors are detected, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

boolean gllc_dis_frame_info_fcs
(
  dsm_item_type **pdu_p,
  uint16        fcs_len
)
{
  register uint32         fcs           = gllc_disassy_info.fr_fcs;
  register dsm_item_type  *item_p       = *pdu_p;
  register uint32         fcs_end_ctr   = fcs_len;

  register dsm_item_type  *last_p       = NULL;
  register dsm_item_type  *last_but_2_p = NULL;
  register dsm_item_type  *last_but_3_p = NULL;
  register uint32         data;
  register uint32         *dat32_p;
  register uint8          *dat8_p;
  register uint32         ctr4;
  register uint32         ctr1;

           uint32         rcvd_fcs = 0;

  /* --------------------------------------------------------------------- */

  if ( item_p == NULL )
  {
    MSG_GERAN_ERROR_0_G("GLC GRR Dis FCS: No PDU Link");
    return ( FALSE );                                        /*** RETURN ***/
  }

  /* --------------------------------------------------------------------- */

  while ( item_p && fcs_end_ctr )
  {
    ctr1   = (uint32)item_p -> used;
    dat8_p = item_p -> data_ptr;

    if ( fcs_end_ctr < ctr1 )
      ctr1 = fcs_end_ctr;

    fcs_end_ctr -= ctr1;

    /* Align to long word address before commencing long word access. */

    while ( ((uint32)dat8_p & 0x3L) && ctr1 )
    {
      fcs = gllc_fcs_transform[((fcs >> 8) ^ *dat8_p++) & 0xFF] ^ (fcs >> 8);
      ctr1--;
    }

    /* Now commence FCS calculation using long word little endian data
       access. */

    ctr4     = ctr1 >> 2;         /* # of whole long words in payload */
    ctr1    &= 0x3;               /* # of remaining octets in payload */
    dat32_p  = (uint32 *)dat8_p;  /* Set up long word pointer         */

    while ( ctr4 )
    {
      data = *dat32_p++;

      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      data >>= 8;
      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      data >>= 8;
      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      data >>= 8;
      fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
      ctr4--;
    }

    if ( ctr1 )
    {
      data = *dat32_p;

      do
      {
        fcs    = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
        data >>= 8;
        ctr1--;
      }
      while ( ctr1 );
    }

    last_but_3_p = last_but_2_p;
    last_but_2_p = last_p;
    last_p       = item_p;
    item_p       = item_p -> pkt_ptr;
  }

  fcs = (~fcs) & 0xFFFFFF00UL;

  /* --------------------------------------------------------------------- */

  /* Find the end of the source chain if not already there. */

  while ( item_p )
  {
    last_but_3_p = last_but_2_p;
    last_but_2_p = last_p;
    last_p       = item_p;
    item_p       = item_p -> pkt_ptr;
  }

  /* --------------------------------------------------------------------- */

  /* Strip the received FCS contained in the last 3 octets of the chain.
     At worst case these 3 octets could be contained in the last three
     items of the chain (one in each). */

  if ( last_p )
  {
    dat8_p = (uint8 *)(last_p -> data_ptr + last_p -> used - 1);
    ctr1   = 3;

    while ( last_p && ctr1 )
    {
      rcvd_fcs <<= 8;
      rcvd_fcs  |= (*dat8_p-- << 8);

      if ( --(last_p -> used) == 0 )
      {
        (void)dsm_free_buffer( last_p );

        if ( last_p == *pdu_p )
          *pdu_p = NULL;

        last_p       = last_but_2_p;
        last_but_2_p = last_but_3_p;

        if ( last_p )
        {
          last_p -> pkt_ptr = NULL;
          dat8_p = (uint8 *)(last_p -> data_ptr + last_p -> used - 1);
        }
      }

      ctr1--;
    }
  }

  /* --------------------------------------------------------------------- */

  return ( fcs == rcvd_fcs );

  /* --------------------------------------------------------------------- */
} /* end of gllc_dis_frame_info_fcs() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIS_FRAME_INFO_FCS_AND_DECIPHER
===
===  DESCRIPTION
===
===    Deciphers a given frame's information and FCS fields, before
===    completing an FCS calculation to the given length, stripping the FCS
===    octets and testing for FCS error.
===
===  DEPENDENCIES
===
===    The given chain is expected to contain both the information and
===    FCS fields of the received frame, having had the header pre-stripped.
===
===    It is also expected that the frame disassembly working structure
===    contains a partial FCS result which has already been calculated from
===    the header.
===
===  RETURN VALUE
===
===    TRUE if no FCS errors are detected, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

boolean gllc_dis_frame_info_fcs_and_decipher
(
  dsm_item_type **pdu_p,
  uint16        frame_info_len,
  uint16        fcs_len,
  uint32        gea_input_seed
)
{
  register uint32         fcs             = gllc_disassy_info.fr_fcs;

  register uint8          *data_p;
  register uint32         data;
  register uint32         ctr;
  register uint32         gea_is_3_or_4   = (uint32)FALSE;
  register uint32         keystream       = 0;
  register uint32         kstrm_ctr       = 0;
           uint32         *keystream_p;

           dsm_item_type  *item_p         = *pdu_p;
           dsm_item_type  *last_p         = NULL;
           dsm_item_type  *last_but_2_p   = NULL;
           dsm_item_type  *last_but_3_p   = NULL;

           uint32         rcvd_fcs        = 0;

           uint16         decipher_and_fcs_count;
           uint16         decipher_only_count;

                          /* Point keystream_p at this dummy in cases
                             where ciphering is unsupported to prevent
                             compiler warnings. */

           uint32         dummy_keystream = 0;
  GfwGea34ControlStruct   control;

           vect64         key_ptr[2];

  /* --------------------------------------------------------------------- */

  NOTUSED(frame_info_len);

  /* --------------------------------------------------------------------- */

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
  #error code not present
#endif

  /* --------------------------------------------------------------------- */

  if ( item_p == NULL )
  {
    MSG_GERAN_ERROR_0_G("GLC GRR Dis FCS: No PDU Link");
    return ( FALSE );                                        /*** RETURN ***/
  }

  /* --------------------------------------------------------------------- */

  /* For sharing a single cipher HW block by multiple GLLC tasks. */

  GLLC_CIPHER_LOCK();

  /* --------------------------------------------------------------------- */

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST

  /* Enable clock only for GEA1 and GEA2. */

  if ( (gllc_llme.gea == GMMLLC_GEA_1) ||
       (gllc_llme.gea == GMMLLC_GEA_2)    )
  {
    GLLC_GSAC_CLOCK_IS_REQUIRED();
  }

#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  /* --------------------------------------------------------------------- */

  /* Initialise the GEA hardware accelerator block to be used for
     generating the decipher keystream. Load message 'input', Kc and
     algorithm selection. Enable automatic stream generation. */

  {
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
    if ( (gllc_llme.gea == GMMLLC_GEA_1) ||
         (gllc_llme.gea == GMMLLC_GEA_2)    )
    {
      HWIO_OUT ( GEA_INPUT,   gea_input_seed      );
      HWIO_OUTI( GEA_KC_n, 1, gllc_llme.key_Kc[1] );
      HWIO_OUTI( GEA_KC_n, 0, gllc_llme.key_Kc[0] );

      HWIO_OUT(
        GEA_CONFIG_0,
        (
          ( 0   << HWIO_SHFT(GEA_CONFIG_0, GEA12_BIG_ENDIAN)         ) | ( 0 << HWIO_SHFT(GEA_CONFIG_0, GEA12_PREPEND_X_ZEROS)  ) |
          ( 1   << HWIO_SHFT(GEA_CONFIG_0, GEA_DIRECTION_DATA_BIT)   ) | ( 1 << HWIO_SHFT(GEA_CONFIG_0, GEA_SHIFT_INTO_MSB_ENA) ) |
          ( 0   << HWIO_SHFT(GEA_CONFIG_0, GEA12_KEY_STREAM_GEN_CMD) ) | ( 1 << HWIO_SHFT(GEA_CONFIG_0, GEA12_AUTOGEN_ENA)      ) |
          ( 1   << HWIO_SHFT(GEA_CONFIG_0, GEA12_32OCTETS_MODE)      ) | ( 1 << HWIO_SHFT(GEA_CONFIG_0, GEA_TRIGGER)            ) |
            ( GLLC_GSAC_MODE(gllc_llme.gea) << HWIO_SHFT(GEA_CONFIG_0, GEA_MODE) )
        )
      );

      keystream_p = (uint32 *)HWIO_ADDRI(AMBA_KEY_STRM_WORDn_RD, 0);
    }

    /* ..................................................................... */

    else if ( (gllc_llme.gea == GMMLLC_GEA_3) ||
              (gllc_llme.gea == GMMLLC_GEA_4)    )
    {
      gea_is_3_or_4 = (uint32)TRUE;

      /* Reset all parameters. */
      // memset( &control, 0, sizeof(GfwGea34ControlStruct) );
      /* Initialise parameters for this PDU. */
      control.bFirst             = TRUE;
      control.nBytesTotal        = (frame_info_len+3);
      control.nBytesPerCall      = (frame_info_len+3);

      gllc_llme.key_stream_index     = 0;
      gllc_llme.key_stream_index_sub = 0;
      memset( gllc_llme.key_stream, 0, sizeof(gllc_llme.key_stream));

      /* New function call here which provides all key stream octets required for this PDU. */

      if (gllc_llme.gea == GMMLLC_GEA_3)
      {
        key_ptr[0].ub[0] = (( gllc_llme.key_Kc[1] >>  24 ) & 0xFF );
        key_ptr[0].ub[1] = (( gllc_llme.key_Kc[1] >>  16 ) & 0xFF );
        key_ptr[0].ub[2] = (( gllc_llme.key_Kc[1] >>   8 ) & 0xFF );
        key_ptr[0].ub[3] = (( gllc_llme.key_Kc[1] >>   0 ) & 0xFF );
        key_ptr[0].ub[4] = (( gllc_llme.key_Kc[0] >>  24 ) & 0xFF );
        key_ptr[0].ub[5] = (( gllc_llme.key_Kc[0] >>  16 ) & 0xFF );
        key_ptr[0].ub[6] = (( gllc_llme.key_Kc[0] >>   8 ) & 0xFF );
        key_ptr[0].ub[7] = (( gllc_llme.key_Kc[0] >>   0 ) & 0xFF );

        gfw_gea_34( &key_ptr[0],
                    64,
                    gea_input_seed,
                    1,
                    &gllc_llme.key_stream[0],
                    &control);
      }
      else
      {
        key_ptr[1].ub[0] = (( gllc_llme.key_Kc128[1] >>  24 ) & 0xFF );
        key_ptr[1].ub[1] = (( gllc_llme.key_Kc128[1] >>  16 ) & 0xFF );
        key_ptr[1].ub[2] = (( gllc_llme.key_Kc128[1] >>   8 ) & 0xFF );
        key_ptr[1].ub[3] = (( gllc_llme.key_Kc128[1] >>   0 ) & 0xFF );
        key_ptr[1].ub[4] = (( gllc_llme.key_Kc128[0] >>  24 ) & 0xFF );
        key_ptr[1].ub[5] = (( gllc_llme.key_Kc128[0] >>  16 ) & 0xFF );
        key_ptr[1].ub[6] = (( gllc_llme.key_Kc128[0] >>   8 ) & 0xFF );
        key_ptr[1].ub[7] = (( gllc_llme.key_Kc128[0] >>   0 ) & 0xFF );

        key_ptr[0].ub[0] = (( gllc_llme.key_Kc128[3] >>  24 ) & 0xFF );
        key_ptr[0].ub[1] = (( gllc_llme.key_Kc128[3] >>  16 ) & 0xFF );
        key_ptr[0].ub[2] = (( gllc_llme.key_Kc128[3] >>   8 ) & 0xFF );
        key_ptr[0].ub[3] = (( gllc_llme.key_Kc128[3] >>   0 ) & 0xFF );
        key_ptr[0].ub[4] = (( gllc_llme.key_Kc128[2] >>  24 ) & 0xFF );
        key_ptr[0].ub[5] = (( gllc_llme.key_Kc128[2] >>  16 ) & 0xFF );
        key_ptr[0].ub[6] = (( gllc_llme.key_Kc128[2] >>   8 ) & 0xFF );
        key_ptr[0].ub[7] = (( gllc_llme.key_Kc128[2] >>   0 ) & 0xFF );

        gfw_gea_34( &key_ptr[0],
                    128,
                    gea_input_seed,
                    1,
                    &gllc_llme.key_stream[0],
                    &control);
      }
    }

    /* ..................................................................... */

    else
    {
      MSG_GERAN_ERROR_1_G("GLC GSAC: GEA %d Unsupported", gllc_llme.gea);
      keystream_p = &dummy_keystream;
    }

#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* --------------------------------------------------------------------- */

    while ( item_p )
    {
      if ( fcs_len >= item_p -> used )
      {
        decipher_and_fcs_count  = item_p -> used;
        decipher_only_count     = 0;
        fcs_len                -= decipher_and_fcs_count;
      }
      else
      {
        decipher_and_fcs_count  = fcs_len;
        decipher_only_count     = item_p -> used - fcs_len;
        fcs_len                 = 0;
      }

      data_p = item_p -> data_ptr;

      /* ................................................................... */

      /* Perform combined deciphering and FCS calculation. */

      ctr = decipher_and_fcs_count;

      while ( ctr )
      {
        if ( !kstrm_ctr )
        {
          GLLC_GSAC_WAIT_WHILE_BUSY( gea_is_3_or_4 );
          keystream = gllc_gsac_get_cipher_stream( &keystream_p, gea_is_3_or_4 );
          kstrm_ctr = 4;

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
          #error code not present
#endif
        }

        data = (uint32)*data_p ^ keystream;
        fcs  = gllc_fcs_transform[((fcs >> 8) ^ data) & 0xFF] ^ (fcs >> 8);
        *data_p++ = (uint8)data;

        keystream >>= 8;
        kstrm_ctr--;
        ctr--;
      }

      /* ................................................................... */

      /* Perform deciphering over any remainder. */

      ctr = decipher_only_count;

      while ( ctr )
      {
        if ( !kstrm_ctr )
        {
          GLLC_GSAC_WAIT_WHILE_BUSY( gea_is_3_or_4 );
          keystream = gllc_gsac_get_cipher_stream( &keystream_p, gea_is_3_or_4 );
          kstrm_ctr = 4;

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
          #error code not present
#endif
        }

        *data_p = (uint8)((uint32)*data_p ^ keystream);
        data_p++;
        keystream >>= 8;
        kstrm_ctr--;
        ctr--;
      }

      /* ................................................................... */

      /* Prepare for next item. */

      last_but_3_p = last_but_2_p;
      last_but_2_p = last_p;
      last_p       = item_p;
      item_p       = item_p -> pkt_ptr;
    }

    fcs = (~fcs) & 0xFFFFFF00UL;

    /* --------------------------------------------------------------------- */

    /* Strip the received FCS contained in the last 3 octets of the chain.
       At worst case these 3 octets could be contained in the last three
       items of the chain (one in each). */

    if ( last_p ) /*lint !e774 Boolean within 'if' always evaluates to True  */
    {
      data_p = (uint8 *)(last_p -> data_ptr + last_p -> used - 1);
      ctr    = 3;

      while ( last_p && ctr )
      {
        rcvd_fcs <<= 8;
        rcvd_fcs  |= (*data_p-- << 8);

        if ( --(last_p -> used) == 0 )
        {
          (void)dsm_free_buffer( last_p );

          if ( last_p == *pdu_p )
            *pdu_p = NULL;

          last_p       = last_but_2_p;
          last_but_2_p = last_but_3_p;

          if ( last_p )
          {
            last_p -> pkt_ptr = NULL;
            data_p = (uint8 *)(last_p -> data_ptr + last_p -> used - 1);
          }
        }

        ctr--;
      }
    }
  }

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST

  if ( (gllc_llme.gea == GMMLLC_GEA_1) ||
       (gllc_llme.gea == GMMLLC_GEA_2)    )
  {
    GLLC_GSAC_CLOCK_NOT_REQUIRED();
  }

#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST*/

  /* --------------------------------------------------------------------- */

  GLLC_CIPHER_UNLOCK();

  /* --------------------------------------------------------------------- */

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */

  /* --------------------------------------------------------------------- */

  return ( fcs == rcvd_fcs );

  /* --------------------------------------------------------------------- */
} /* end of gllc_dis_frame_info_fcs_and_decipher() */


#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */


/*** EOF: don't remove! ***/


