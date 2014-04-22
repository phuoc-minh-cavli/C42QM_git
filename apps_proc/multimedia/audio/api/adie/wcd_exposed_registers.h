/*-------------------------------------------------------------------------------
    Copyright (c) 2013 QUALCOMM Technologies Incorporated.
    All rights reserved.
    Qualcomm Technologies Incorporated Confidential and Proprietary
   -----------------------------------------------------------------------------*/
/*=============================================================================
                           Edit History
$Id: //components/rel/audio.tx/2.0/audio/api/adie/wcd_exposed_registers.h#1 $

when        who      what, where, why
--------    ---      ------------------------------------------------------------
02/27/13    OA       Initial revision
===============================================================================*/

#ifndef __WCD_EXPOSED_REGISTER_UTILITY___
#define __WCD_EXPOSED_REGISTER_UTILITY___

#include "wcd_intf.h"


typedef enum {
  E_WCD_AANC          = 0,
  E_WCD_MAD           = 1,
  E_WCD_VBAT          = 2,
  E_WCD_CLIP_DET      = 3,
  E_WCD_PGD_PORT      = 4
} wcd_exposed_blocks;



/* ********************************************* */
/*          AANC related data types              */
/* ********************************************* */


typedef enum {
  E_WCD_AANC_LPF_FF_FB                 = 1,  /** CDC_ANCn_LPF_B1_CTL codec register. */
  E_WCD_AANC_LPF_COEFF_MSB             = 2,  /** CDC_ANCn_LPF_B2_CTL codec register. */
  E_WCD_AANC_LPF_COEFF_LSB             = 3,  /** CDC_ANCn_LPF_B3_CTL codec register. */
  E_WCD_HW_MAD_AUDIO_ENABLE            = 4,
  E_WCD_HW_MAD_ULTR_ENABLE             = 5,
  E_WCD_HW_MAD_BEACON_ENABLE           = 6,
  E_WCD_HW_MAD_AUDIO_SLEEP_TIME        = 7,
  E_WCD_HW_MAD_ULTR_SLEEP_TIME         = 8,
  E_WCD_HW_MAD_BEACON_SLEEP_TIME       = 9,
  E_WCD_HW_MAD_TX_AUDIO_SWITCH_OFF     = 10,
  E_WCD_HW_MAD_TX_ULTR_SWITCH_OFF      = 11,
  E_WCD_HW_MAD_TX_BEACON_SWITCH_OFF    = 12,
  E_WCD_MAD_AUDIO_INT_DEST_SELECT_REG  = 13,
  E_WCD_MAD_ULT_INT_DEST_SELECT_REG    = 14,
  E_WCD_MAD_BEACON_INT_DEST_SELECT_REG = 15,
  E_WCD_CLIP_INT_DEST_SELECT_REG       = 16,
  E_WCD_VBAT_INT_DEST_SELECT_REG       = 17,
  E_WCD_MAD_AUDIO_INT_MASK_REG         = 18,
  E_WCD_MAD_ULT_INT_MASK_REG           = 19,
  E_WCD_MAD_BEACON_INT_MASK_REG        = 20,
  E_WCD_CLIP_INT_MASK_REG              = 21,
  E_WCD_VBAT_INT_MASK_REG              = 22,
  E_WCD_MAD_AUDIO_INT_STATUS_REG       = 23,
  E_WCD_MAD_ULT_INT_STATUS_REG         = 24,
  E_WCD_MAD_BEACON_INT_STATUS_REG      = 25,
  E_WCD_CLIP_INT_STATUS_REG            = 26,
  E_WCD_VBAT_INT_STATUS_REG            = 27,
  E_WCD_MAD_AUDIO_INT_CLEAR_REG        = 28,
  E_WCD_MAD_ULT_INT_CLEAR_REG          = 29,
  E_WCD_MAD_BEACON_INT_CLEAR_REG       = 30,
  E_WCD_CLIP_INT_CLEAR_REG             = 31,
  E_WCD_VBAT_INT_CLEAR_REG             = 32,
  E_WCD_SB_PGD_PORT_TX_WATERMARK_N     = 33,
  E_WCD_SB_PGD_PORT_TX_ENABLE_N        = 34,
  E_WCD_SB_PGD_PORT_RX_WATERMARK_N     = 35,
  E_WCD_SB_PGD_PORT_RX_ENABLE_N        = 36,
  E_WCD_SB_PGD_TX_PORTn_MULTI_CHNL_0   = 37,
  E_WCD_SB_PGD_TX_PORTn_MULTI_CHNL_1   = 38,
  E_WCD_SB_PGD_RX_PORTn_MULTI_CHNL_0   = 39,
  E_WCD_SB_PGD_RX_PORTn_MULTI_CHNL_1   = 40,
  E_WCD_AANC_FF_GAIN_ADAPTIVE          = 41, /** CDC_ANCn_IIR_B1_CTL.*/
  E_WCD_ANC_FFGAIN_ADAPTIVE_EN         = 42, /** CDC_ANCn_IIR_B1_CTL*/
  E_WCD_ANC_GAIN_CONTROL               = 43, /** CDC_ANCn_GAIN_CTL*/
  E_WCD_CLIP_PIPE_BANK_SEL             = 44, /** CDC_SPKR_CLIPDET_B1_CTL*/
  E_WCD_CDC_SPKR_CLIPDET_VAL0          = 45,
  E_WCD_CDC_SPKR_CLIPDET_VAL1          = 46,
  E_WCD_CDC_SPKR_CLIPDET_VAL2          = 47,
  E_WCD_CDC_SPKR_CLIPDET_VAL3          = 48,
  E_WCD_CDC_SPKR_CLIPDET_VAL4          = 49,
  E_WCD_CDC_SPKR_CLIPDET_VAL5          = 50,
  E_WCD_CDC_SPKR_CLIPDET_VAL6          = 51,
  E_WCD_CDC_SPKR_CLIPDET_VAL7          = 52,
  E_WCD_SVA_ENGINE_INT_DEST_SELECT_REG = 53,
  E_WCD_SVA_ENGINE_INT_MASK_REG        = 54,
  E_WCD_SVA_ENGINE_INT_STATUS_REG      = 55,
  E_WCD_SVA_ENGINE_INT_CLEAR_REG       = 56,
  E_WCD_CLIP_INT_2_MASK_REG            = 57,
  E_WCD_CLIP_INT_2_STATUS_REG          = 58,
  E_WCD_CLIP_INT_2_CLEAR_REG           = 59,
  E_WCD_INTR2_PIN_DEST_SELECT_REG      = 60,
  E_WCD_REG_TYPE_32B_ANCHOR   = 0x7FFFFFFF
} wcd_reg_type;

typedef struct {
  uint32            reg_addr;       /**< Register address (as shown on SWI). */
  wcd_reg_type      type;           /**< Register type. */
  uint32            bitmask;        /**< Bitmask of the usable bits. */
  uint16            register_width; /**< Register width. */
  uint16            offset;         /**< Register offset. */
} wcd_register_info_v2;


/* wcd_register_block_definition version 2 is NOT binary compatible with v1 (A family) */
typedef struct {
  uint32     version;
  uint32     num_elements_in_table;
  const wcd_register_info_v2*  register_table;
  uint32     hw_version;           /**< HW version of the requested block. */
}wcd_register_block_definition;


typedef struct {
  uint32    version;
  uint32    device_enum_addr_lsw; /**< Codec enumeration address LSW. */
  uint32    device_enum_addr_msw; /**< Codec enumeration address MSW. */
  uint16    tx_slave_port_offset; /**< Valid offset as per Codec hardware*/
  uint16    rx_slave_port_offset; /**< Valid offset as per Codec hardware*/
}wcd_codec_enumeration_address;

/* ************************************************ */
/*                 Public API                       */
/* ************************************************ */

/**
  \brief  Gets a table containing a list of registers definitions for different hw 
          blocks
  
  This function's purpose is to provide a table containing the information required
  to configure certain codec hw blocks, this information is required by some 
  components with the need of immediate control of the hw.
  
  \param block[in]:   Selects one of the available tables
  \return:  Returns 
                    NULL if error.
                    otherwise a pointer to a constant structure containing the 
                    register related information, the structure version needs to
                    be verified by the caller to ensure the version received is 
                    expected.
*/
const wcd_register_block_definition* wcd_exposed_register_block( wcd_exposed_blocks block );

/**
  \brief Gets the enumeration address for the codec used on the current build.
  
  \param None.
  \return: Returns
                  NULL if error.
                  Otherwise a pointer to a constant structure containing
                  the enumeration address.
*/
const wcd_codec_enumeration_address* wcd_get_codec_enumeration_address( void );


/**
  \brief Sets the default codec information to be retrieved.
  
  This function sets the default codec information to be retrieved. this is 
  used in cases where the codec cannot be detected.
  
  \param codec: wcd_codec type value used for default initialization.
  \return wcd_result: E_WCD_SUCCESS if no error was found.
*/
wcd_result wcd_set_default_codec( wcd_codec_info codec_info );

#endif


