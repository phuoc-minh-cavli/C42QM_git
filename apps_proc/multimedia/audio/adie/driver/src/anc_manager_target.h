#ifndef WCD_ANC_MANAGER_TARGET_INTF_
#define WCD_ANC_MANAGER_TARGET_INTF_

/**
@file anc_manager_target.h
@brief 
External WCD ANC Manager Target file
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/anc_manager_target.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/01/13   KNM	    Initial revision
===============================================================================*/

#include "wcd_utility.h"
#include "wcd_types.h"

/******************************************************************************
 **************               ANC DEFINITIONS                   ***************
 *****************************************************************************/

#define WCD_ANC_NUM_IIR_FF_A_COEFFS           7
#define WCD_ANC_NUM_IIR_FF_B_COEFFS           8

#define WCD_ANC_NUM_IIR_FB_A_COEFFS           6
#define WCD_ANC_NUM_IIR_FB_B_COEFFS           7

#define WCD_ANC_IIR_COEFFS_REG_INT_BITS       2
#define WCD_ANC_IIR_COEFFS_REG_FRAC_BITS      9
#define WCD_ANC_IIR_COEFFS_MAX_VAL            511

#define WCD_MAX_ANC_SHIFT                     6
#define WCD_MIN_ANC_SHIFT                     -4

#define WCD_MAX_ANC_SMLPF_SHIFT               7
#define WCD_MIN_ANC_SMLPF_SHIFT               0
#define WCD_MAX_ANC_DCFLT_SHIFT               7
#define WCD_MIN_ANC_DCFLT_SHIFT               0

#define WCD_MAX_ANC_LPF_SHIFT                 12
#define WCD_MIN_ANC_LPF_SHIFT                 0

#define WCD_MAX_ANC_GAIN                      255
#define WCD_MIN_ANC_GAIN                      0

wcd_result wcd_set_anc_connections(wcd_adie_codec_db_anc_config *anc_config);

wcd_result wcd_disable_anc_connections(void);

/******************************************************************************
 **************           ANC REGISTER DEFINITIONS              ***************
 *****************************************************************************/

#define WCD_ANC_LR_MIX_ENABLE_R1              0x200
#define WCD_ANC_LR_MIX_ENABLE_R2              0x280
#define WCD_ANC_LR_MIX_ENABLE_M               0x04
#define WCD_ANC_LR_MIX_ENABLE_S               2
#define WCD_ANC_LR_MIX_ENABLE_TRUE            1
#define WCD_ANC_LR_MIX_ENABLE_FALSE           0

#define WCD_ANC_SMLPF_ENABLE_R1               0x200
#define WCD_ANC_SMLPF_ENABLE_R2               0x280
#define WCD_ANC_SMLPF_ENABLE_M                0x40
#define WCD_ANC_SMLPF_ENABLE_S                6
#define WCD_ANC_SMLPF_ENABLE_TRUE             1
#define WCD_ANC_SMLPF_ENABLE_FALSE            0

#define WCD_ANC_SMLPF_SHIFT_R1                0x20A
#define WCD_ANC_SMLPF_SHIFT_R2                0x28A
#define WCD_ANC_SMLPF_SHIFT_M                 0x7
#define WCD_ANC_SMLPF_SHIFT_S                 0

#define WCD_ANC_DCFLT_ENABLE_R1               0x200
#define WCD_ANC_DCFLT_ENABLE_R2               0x280
#define WCD_ANC_DCFLT_ENABLE_M                0x20
#define WCD_ANC_DCFLT_ENABLE_S                5
#define WCD_ANC_DCFLT_ENABLE_TRUE             1
#define WCD_ANC_DCFLT_ENABLE_FALSE            0

#define WCD_ANC_DCFLT_SHIFT_R1                0x20B
#define WCD_ANC_DCFLT_SHIFT_R2                0x28B
#define WCD_ANC_DCFLT_SHIFT_M                 0x7
#define WCD_ANC_DCFLT_SHIFT_S                 0

#define WCD_ANC_SHIFT_R1                      0x201
#define WCD_ANC_SHIFT_R2                      0x281
#define WCD_ANC_SHIFT_M                       0xF0 | 0x0F
#define WCD_ANC_SHIFT_S                       0
#define WCD_ANC_SHIFT_FF_S                    4
#define WCD_ANC_SHIFT_FB_S                    0

#define WCD_ANC_B1_CTL_ENABLES_R1               0x200
#define WCD_ANC_B1_CTL_ENABLES_R2               0x280
#define WCD_ANC_B1_CTL_ENABLES_M                0x80 | 0x10 | 0x08 | 0x01
#define WCD_ANC_B1_CTL_ENABLES_S                0
#define WCD_ANC_B1_CTL_ENABLES_FF_IN_ENABLE_S   4
#define WCD_ANC_B1_CTL_ENABLES_HYBRID_ENABLE_S  3
#define WCD_ANC_B1_CTL_ENABLES_FF_OUT_ENABLE_S  0
#define WCD_ANC_B1_CTL_ENABLES_FFGAIN_ENABLE_S  7

#define WCD_ANC_B2_CTL_ENABLES_R1               0x20D
#define WCD_ANC_B2_CTL_ENABLES_R2               0x28D
#define WCD_ANC_B2_CTL_ENABLES_M                0x01 | 0x02
#define WCD_ANC_B2_CTL_ENABLES_S                0
#define WCD_ANC_B2_CTL_ENABLES_FFLE_ENABLE_S    1
#define WCD_ANC_B2_CTL_ENABLES_DMIC_X2_ENABLE_S 0

#define WCD_ANC_GAIN_CTL_R1                   0x20C
#define WCD_ANC_GAIN_CTL_R2                   0x28C
#define WCD_ANC_GAIN_CTL_M                    0xFF
#define WCD_ANC_GAIN_CTL_S                    0

#define WCD_ANC_LPF_FF_SHIFT_R1               0x206
#define WCD_ANC_LPF_FF_SHIFT_R2               0x286
#define WCD_ANC_LPF_FB_SHIFT_R1               0x207
#define WCD_ANC_LPF_FB_SHIFT_R2               0x287
#define WCD_ANC_LPF_SHIFT_M                   0xF0 | 0x0F
#define WCD_ANC_LPF_SHIFT_S                   0
#define WCD_ANC_LPF_S1_S                      0
#define WCD_ANC_LPF_S2_S                      4

#define WCD_ANC_IIR_COEFF_PTR_R1              0x203
#define WCD_ANC_IIR_COEFF_PTR_R2              0x283
#define WCD_ANC_IIR_COEFF_PTR_M               0x3F
#define WCD_ANC_IIR_COEFF_PTR_S               0
#define WCD_ANC_IIR_COEFF_PTR_FIRST_COEFF     0

#define WCD_ANC_IIR_COEFF_R1                  0x204
#define WCD_ANC_IIR_COEFF_R2                  0x284
#define WCD_ANC_IIR_COEFF_MSB_M               0x0F
#define WCD_ANC_IIR_COEFF_LSB_M               0xFF
#define WCD_ANC_IIR_COEFF_S                   0

#define WCD_ANC_CLK_R                         0x30E
#define WCD_ANC_CLK_M                         0x01 | 0x02 | 0x04 | 0x08
#define WCD_ANC_CLK_S                         0
#define WCD_ANC1_CLK_FF_EN_S                  0
#define WCD_ANC1_CLK_FB_EN_S                  1
#define WCD_ANC2_CLK_FF_EN_S                  2
#define WCD_ANC2_CLK_FB_EN_S                  3

#define WCD_ANC_RESET_R                       0x300
#define WCD_ANC_RESET_M                       0x01 | 0x02 | 0x04 | 0x08
#define WCD_ANC_RESET_S                       0
#define WCD_ANC_RESET_ENABLE                  0xFF
#define WCD_ANC_RESET_DISABLE                 0x00

#define WCD_ANC_CONN_R                        0x391
#define WCD_ANC_CONN_M                        0x0F | 0xF0
#define WCD_ANC_CONN_S                        0
#define WCD_ANC_CONN_ANC1_S                   0
#define WCD_ANC_CONN_ANC2_S                   4

#define WCD_ANC_ANC1_OUTPUT_SEL_R             0x392
#define WCD_ANC_ANC1_OUTPUT_SEL_M             0x3
#define WCD_ANC_ANC1_OUTPUT_SEL_S             0
#define WCD_ANC_ANC1_OUTPUT_SEL_ZERO          0x0
#define WCD_ANC_ANC1_OUTPUT_SEL_EAR_HPHL      0x1
//#define WCD_ANC_ANC1_OUTPUT_SEL_EAR_SPKR      0x2

#define WCD_ANC_ADC_DMIC_SEL_R1               0x200
#define WCD_ANC_ADC_DMIC_SEL_R2               0x280
#define WCD_ANC_ADC_DMIC_SEL_M                0x2
#define WCD_ANC_ADC_DMIC_SEL_S                1

/**
 * \brief Maps an ANC input device to the codec source element.
 */
wcd_result wcd_map_anc_source(wcd_adie_codec_db_anc_input_device anc_source,
                              wcd_element** source_element);

/**
 * \brief Check if ANC feature is supported or not
 */
wcd_bool wcd_is_anc_supported(void);

#endif
