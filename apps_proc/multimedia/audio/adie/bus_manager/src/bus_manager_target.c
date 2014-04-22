/**
@file bus_manager_target.c
@brief
Implementation of codec specific functions of WCD bus manager.
 */
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
         All rights reserved.
         Qualcomm Confidential and Proprietary
   -----------------------------------------------------------------------------*/
/*=============================================================================
                           Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/bus_manager/src/bus_manager_target.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/13   KNM      Updated non-cacheable register list as per codec status
                    and read only register.
08/11/13   KNM      Updated non-cacheable register list to make 0x1B3 register
                    as non-cacheable. 
05/31/13   KNM      Adopted wcd9306/02 2.0 hw revision specific changes and
                    common code specific logging message change.
03/02/13   KNM      Initial revision.
===============================================================================*/

#include "bus_manager_target.h"
#include "bus_manager.h"

#ifdef SPL_LOG_SUPPORT
	#include "bus_manager_target.tmh"
#endif

extern wcd_bool WCD_G_FALSE_BOOLEAN;

/**
 * \brief Initializes the shadow register to the default values upon reset
 */
wcd_result wcd_initialize_shadow_register(uint8* shadow_register, wcd_codec_info codec_info)
{
  if (shadow_register == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing shadow register.>\n");
    return E_WCD_INVALID_HANDLE;
  }

  shadow_register[0x000] = 0x02; // CHIP_CTL
  shadow_register[0x001] = 0x00; // CHIP_STATUS
  shadow_register[0x005] = 0x00; // CHIP_ID_BYTE_1
  shadow_register[0x006] = 0x03; // CHIP_ID_BYTE_2
  shadow_register[0x007] = 0x01; // CHIP_ID_BYTE_3
  shadow_register[0x009] = 0x00; // CHIP_DEBUG_CTL
  shadow_register[0x00C] = 0x77; // SLAVE_ID_1
  shadow_register[0x00D] = 0x66; // SLAVE_ID_2
  shadow_register[0x00E] = 0x55; // SLAVE_ID_3
  shadow_register[0x010] = 0x00; // PIN_CTL_OE0
  shadow_register[0x012] = 0x00; // PIN_CTL_DATA0
  shadow_register[0x018] = 0x00; // HDRIVE_GENERIC
  shadow_register[0x019] = 0x08; // HDRIVE_OVERRIDE
  shadow_register[0x020] = 0x44; // ANA_CSR_WAIT_STATE
  shadow_register[0x040] = 0x80; // PROCESS_MONITOR_CTL0
  shadow_register[0x041] = 0x00; // PROCESS_MONITOR_CTL1
  shadow_register[0x042] = 0x00; // PROCESS_MONITOR_CTL2
  shadow_register[0x043] = 0x01; // PROCESS_MONITOR_CTL3
  shadow_register[0x048] = 0x00; // QFUSE_CTL
  shadow_register[0x049] = 0x00; // QFUSE_STATUS
  shadow_register[0x04A] = 0x00; // QFUSE_DATA_OUT0
  shadow_register[0x04B] = 0x00; // QFUSE_DATA_OUT1
  shadow_register[0x04C] = 0x00; // QFUSE_DATA_OUT2
  shadow_register[0x04D] = 0x00; // QFUSE_DATA_OUT3
  shadow_register[0x04E] = 0x00; // QFUSE_DATA_OUT4
  shadow_register[0x04F] = 0x00; // QFUSE_DATA_OUT5
  shadow_register[0x050] = 0x00; // QFUSE_DATA_OUT6
  shadow_register[0x051] = 0x00; // QFUSE_DATA_OUT7
  shadow_register[0x080] = 0x00; // CDC_CTL
  shadow_register[0x088] = 0x04; // LEAKAGE_CTL
  shadow_register[0x090] = 0x00; // INTR_MODE
  shadow_register[0x094] = 0xFF; // INTR_MASK0
  shadow_register[0x095] = 0xFF; // INTR_MASK1
  shadow_register[0x096] = 0x3F; // INTR_MASK2
  shadow_register[0x097] = 0x3F; // INTR_MASK3
  shadow_register[0x098] = 0x00; // INTR_STATUS0
  shadow_register[0x099] = 0x00; // INTR_STATUS1
  shadow_register[0x09A] = 0x00; // INTR_STATUS2
  shadow_register[0x09B] = 0x00; // INTR_STATUS3
  shadow_register[0x09C] = 0x00; // INTR_CLEAR0
  shadow_register[0x09D] = 0x00; // INTR_CLEAR1
  shadow_register[0x09E] = 0x00; // INTR_CLEAR2
  shadow_register[0x09F] = 0x00; // INTR_CLEAR3
  shadow_register[0x0A0] = 0x01; // INTR_LEVEL0
  shadow_register[0x0A1] = 0x00; // INTR_LEVEL1
  shadow_register[0x0A2] = 0x00; // INTR_LEVEL2
  shadow_register[0x0A3] = 0x00; // INTR_LEVEL3
  shadow_register[0x0A4] = 0x00; // INTR_TEST0
  shadow_register[0x0A5] = 0x00; // INTR_TEST1
  shadow_register[0x0A6] = 0x00; // INTR_TEST2
  shadow_register[0x0A7] = 0x00; // INTR_TEST3
  shadow_register[0x0A8] = 0x00; // INTR_SET0
  shadow_register[0x0A9] = 0x00; // INTR_SET1
  shadow_register[0x0AA] = 0x00; // INTR_SET2
  shadow_register[0x0AB] = 0x00; // INTR_SET3
  shadow_register[0x0AC] = 0x00; // INTR_DESTN0
  shadow_register[0x0AD] = 0x00; // INTR_DESTN1
  shadow_register[0x0AE] = 0x00; // INTR_DESTN2
  shadow_register[0x0AF] = 0x00; // INTR_DESTN3
  shadow_register[0x0C0] = 0x00; // CDC_DMIC_DATA0_MODE
  shadow_register[0x0C1] = 0x00; // CDC_DMIC_CLK0_MODE
  shadow_register[0x0C2] = 0x00; // CDC_DMIC_DATA1_MODE
  shadow_register[0x0C3] = 0x00; // CDC_DMIC_CLK1_MODE
  shadow_register[0x0C4] = 0x00; // CDC_INTR_MODE
  shadow_register[0x100] = 0x1C; // BIAS_REF_CTL
  shadow_register[0x101] = 0x50; // BIAS_CENTRAL_BG_CTL
  shadow_register[0x102] = 0x07; // BIAS_PRECHRG_CTL
  shadow_register[0x103] = 0x52; // BIAS_CURR_CTL_1
  shadow_register[0x104] = 0x00; // BIAS_CURR_CTL_2
  shadow_register[0x105] = 0x16; // BIAS_OSC_BG_CTL
  shadow_register[0x108] = 0x04; // CLK_BUFF_EN1
  shadow_register[0x109] = 0x02; // CLK_BUFF_EN2
  shadow_register[0x110] = 0x65; // LDO_H_MODE_1
  shadow_register[0x111] = 0xA8; // LDO_H_MODE_2
  shadow_register[0x112] = 0x6B; // LDO_H_LOOP_CTL
  shadow_register[0x113] = 0x84; // LDO_H_COMP_1
  shadow_register[0x114] = 0xE0; // LDO_H_COMP_2
  shadow_register[0x115] = 0x6D; // LDO_H_BIAS_1
  shadow_register[0x116] = 0xA5; // LDO_H_BIAS_2
  shadow_register[0x117] = 0x60; // LDO_H_BIAS_3
  shadow_register[0x128] = 0x40; // MICB_CFILT_1_CTL
  shadow_register[0x129] = 0x80; // MICB_CFILT_1_VAL
  shadow_register[0x12A] = 0x00; // MICB_CFILT_1_PRECHRG
  shadow_register[0x12B] = 0x02; // MICB_1_CTL
  shadow_register[0x12C] = 0x24; // MICB_1_INT_RBIAS
  shadow_register[0x12D] = 0x01; // MICB_1_MBHC
  shadow_register[0x12E] = 0x40; // MICB_CFILT_2_CTL
  shadow_register[0x12F] = 0x80; // MICB_CFILT_2_VAL
  shadow_register[0x130] = 0x00; // MICB_CFILT_2_PRECHRG
  shadow_register[0x131] = 0x12; // MICB_2_CTL
  shadow_register[0x132] = 0x24; // MICB_2_INT_RBIAS
  shadow_register[0x133] = 0x02; // MICB_2_MBHC
  shadow_register[0x134] = 0x40; // MICB_CFILT_3_CTL
  shadow_register[0x135] = 0x80; // MICB_CFILT_3_VAL
  shadow_register[0x136] = 0x00; // MICB_CFILT_3_PRECHRG
  shadow_register[0x137] = 0x02; // MICB_3_CTL
  shadow_register[0x138] = 0x24; // MICB_3_INT_RBIAS
  shadow_register[0x139] = 0x00; // MICB_3_MBHC
  shadow_register[0x14A] = 0x00; // MBHC_INSERT_DETECT
  shadow_register[0x14B] = 0x00; // MBHC_INSERT_DET_STATUS
  shadow_register[0x14C] = 0xE0; // TX_COM_BIAS
  shadow_register[0x14E] = 0x00; // MBHC_SCALING_MUX_1
  shadow_register[0x14F] = 0x80; // MBHC_SCALING_MUX_2
  shadow_register[0x150] = 0xF1; // RESERVED_MAD_ANA_CTRL
  shadow_register[0x151] = 0x00; // TX_SUP_SWITCH_CTRL_1
  shadow_register[0x152] = 0x80; // TX_SUP_SWITCH_CTRL_2
  shadow_register[0x153] = 0x02; // TX_1_EN
  shadow_register[0x154] = 0x02; // TX_2_EN
  shadow_register[0x155] = 0x44; // TX_1_2_ADC_CH1
  shadow_register[0x156] = 0x44; // TX_1_2_ADC_CH2
  shadow_register[0x157] = 0x00; // TX_1_2_ATEST_REFCTRL
  shadow_register[0x158] = 0x00; // TX_1_2_TEST_CTL
  shadow_register[0x159] = 0x00; // TX_1_2_TEST_BLOCK_EN
  shadow_register[0x15A] = 0x55; // TX_1_2_TXFE_CLKDIV
  shadow_register[0x15B] = 0x00; // TX_1_2_SAR_ERR_CH1
  shadow_register[0x15C] = 0x00; // TX_1_2_SAR_ERR_CH2
  shadow_register[0x15D] = 0x00; // TX_3_EN
  shadow_register[0x15E] = 0xCC; // TX_1_2_TEST_EN
  shadow_register[0x15F] = 0x00; // TX_4_5_TXFE_SC_CTL
  shadow_register[0x160] = 0xCC; // TX_4_5_TEST_EN
  shadow_register[0x167] = 0x02; // TX_4_EN
  shadow_register[0x168] = 0x02; // TX_5_EN
  shadow_register[0x169] = 0x44; // TX_4_5_ADC_CH4
  shadow_register[0x16A] = 0x44; // TX_4_5_ADC_CH5
  shadow_register[0x16B] = 0x00; // TX_4_5_ATEST_REFCTRL
  shadow_register[0x16C] = 0x38; // TX_4_5_TEST_CTL
  shadow_register[0x16D] = 0xFC; // TX_4_5_TEST_BLOCK_EN
  shadow_register[0x16E] = 0x55; // TX_4_5_TXFE_CKDIV
  shadow_register[0x16F] = 0x00; // TX_4_5_SAR_ERR_CH4
  shadow_register[0x170] = 0x00; // TX_4_5_SAR_ERR_CH5
  shadow_register[0x172] = 0x00; // TX_7_MBHC_ATEST_REFCTRL
  shadow_register[0x173] = 0x44; // TX_7_MBHC_ADC
  shadow_register[0x174] = 0x38; // TX_7_MBHC_TEST_CTL
  shadow_register[0x175] = 0x00; // TX_7_MBHC_SAR_ERR
  shadow_register[0x176] = 0x0B; // TX_7_TXFE_CLKDIV
  shadow_register[0x181] = 0x21; // BUCK_MODE_1    //changed from 21 to A1
  shadow_register[0x182] = 0xFF; // BUCK_MODE_2
  shadow_register[0x183] = 0xCE; // BUCK_MODE_3
  shadow_register[0x184] = 0x3A; // BUCK_MODE_4
  shadow_register[0x185] = 0x00; // BUCK_MODE_5
  shadow_register[0x186] = 0x08; // BUCK_CTRL_VCL_1
  shadow_register[0x187] = 0xA3; // BUCK_CTRL_VCL_2
  shadow_register[0x188] = 0x82; // BUCK_CTRL_VCL_3
  shadow_register[0x189] = 0x5B; // BUCK_CTRL_CCL_1
  shadow_register[0x18A] = 0xDC; // BUCK_CTRL_CCL_2
  shadow_register[0x18B] = 0x6A; // BUCK_CTRL_CCL_3
  shadow_register[0x18D] = 0x50; // BUCK_CTRL_PWM_DRVR_1
  shadow_register[0x18E] = 0x64; // BUCK_CTRL_PWM_DRVR_2
  shadow_register[0x18F] = 0x77; // BUCK_CTRL_PWM_DRVR_3
  shadow_register[0x190] = 0x00; // BUCK_TMUX_A_D
  shadow_register[0x191] = 0x00; // NCP_BUCKREF
  shadow_register[0x192] = 0xFE; // NCP_EN
  shadow_register[0x193] = 0x94; // NCP_CLK
  shadow_register[0x194] = 0x28; // NCP_STATIC
  shadow_register[0x195] = 0x88; // NCP_VTH_LOW
  shadow_register[0x196] = 0xA0; // NCP_VTH_HIGH
  shadow_register[0x197] = 0x00; // NCP_ATEST
  shadow_register[0x198] = 0x10; // NCP_DTEST
  shadow_register[0x199] = 0x06; // NCP_DLY1
  shadow_register[0x19A] = 0x06; // NCP_DLY2
  shadow_register[0x19B] = 0x00; // RX_AUX_SW_CTL
  shadow_register[0x19C] = 0x00; // RX_PA_AUX_IN_CONN
  shadow_register[0x19E] = 0xE8; // RX_COM_TIMER_DIV
  shadow_register[0x19F] = 0x1F; // RX_COM_OCP_CTL
  shadow_register[0x1A0] = 0x77; // RX_COM_OCP_COUNT
  shadow_register[0x1A1] = 0x00; // RX_COM_DAC_CTL
  shadow_register[0x1A2] = 0x00; // RX_COM_BIAS
  shadow_register[0x1A4] = 0x38; // RX_HPH_AUTO_CHOP
  shadow_register[0x1A5] = 0xA4; // RX_HPH_CHOP_CTL
  shadow_register[0x1A6] = 0x7A; // RX_HPH_BIAS_PA
  shadow_register[0x1A7] = 0x87; // RX_HPH_BIAS_LDO
  shadow_register[0x1A8] = 0x8A; // RX_HPH_BIAS_CNP
  shadow_register[0x1A9] = 0x2A; // RX_HPH_BIAS_WG_OCP
  shadow_register[0x1AA] = 0x69; // RX_HPH_OCP_CTL
  shadow_register[0x1AB] = 0x80; // RX_HPH_CNP_EN
  shadow_register[0x1AD] = 0x15; // RX_HPH_CNP_WG_TIME
  shadow_register[0x1AE] = 0x00; // RX_HPH_L_GAIN
  shadow_register[0x1AF] = 0x00; // RX_HPH_L_TEST
  shadow_register[0x1B0] = 0x40; // RX_HPH_L_PA_CTL
  shadow_register[0x1B1] = 0x00; // RX_HPH_L_DAC_CTL
  shadow_register[0x1B2] = 0x00; // RX_HPH_L_ATEST
  shadow_register[0x1B3] = 0x00; // RX_HPH_L_STATUS
  shadow_register[0x1B4] = 0x00; // RX_HPH_R_GAIN
  shadow_register[0x1B5] = 0x00; // RX_HPH_R_TEST
  shadow_register[0x1B6] = 0x40; // RX_HPH_R_PA_CTL
  shadow_register[0x1B7] = 0x00; // RX_HPH_R_DAC_CTL
  shadow_register[0x1B8] = 0x00; // RX_HPH_R_ATEST
  shadow_register[0x1B9] = 0x00; // RX_HPH_R_STATUS
  shadow_register[0x1BA] = 0x76; // RX_EAR_BIAS_PA
  shadow_register[0x1BB] = 0xA0; // RX_EAR_BIAS_CMBUFF
  shadow_register[0x1BC] = 0x00; // RX_EAR_EN
  shadow_register[0x1BD] = 0x02; // RX_EAR_GAIN
  shadow_register[0x1BE] = 0x05; // RX_EAR_CMBUFF
  shadow_register[0x1BF] = 0x40; // RX_EAR_ICTL
  shadow_register[0x1C0] = 0x08; // RX_EAR_CCOMP
  shadow_register[0x1C1] = 0x03; // RX_EAR_VCM
  shadow_register[0x1C3] = 0x00; // RX_EAR_DAC_CTL_ATEST
  shadow_register[0x1C5] = 0x04; // RX_EAR_STATUS
  shadow_register[0x1C6] = 0x78; // RX_LINE_BIAS_PA
  shadow_register[0x1C7] = 0x42; // RX_BUCK_BIAS1
  shadow_register[0x1C8] = 0x84; // RX_BUCK_BIAS2
  shadow_register[0x1C9] = 0x80; // RX_LINE_COM
  shadow_register[0x1CA] = 0x00; // RX_LINE_CNP_EN
  shadow_register[0x1CB] = 0x00; // RX_LINE_CNP_WG_CTL
  shadow_register[0x1CC] = 0x04; // RX_LINE_CNP_WG_TIME
  shadow_register[0x1CD] = 0x00; // RX_LINE_1_GAIN
  shadow_register[0x1CF] = 0x00; // RX_LINE_1_DAC_CTL
  shadow_register[0x1D0] = 0x00; // RX_LINE_1_STATUS
  shadow_register[0x1D1] = 0x00; // RX_LINE_2_GAIN
  shadow_register[0x1D3] = 0x00; // RX_LINE_2_DAC_CTL
  shadow_register[0x1D4] = 0x00; // RX_LINE_2_STATUS
  shadow_register[0x1DD] = 0x00; // RX_LINE_CNP_DBG
  shadow_register[0x1DF] = 0x6F; // SPKR_DRV_EN
  shadow_register[0x1E0] = 0x00; // SPKR_DRV_GAIN
  shadow_register[0x1E1] = 0x04; // SPKR_DRV_DAC_CTL
  shadow_register[0x1E5] = 0xE5; // SPKR_DRV_DBG_DAC
  shadow_register[0x1E6] = 0x18; // SPKR_DRV_DBG_PA
  shadow_register[0x1E7] = 0x00; // SPKR_DRV_DBG_PWRSTG
  shadow_register[0x1E8] = 0x45; // SPKR_DRV_BIAS_LDO
  shadow_register[0x1E9] = 0xA5; // SPKR_DRV_BIAS_INT
  shadow_register[0x1EA] = 0x55; // SPKR_DRV_BIAS_PA
  shadow_register[0x1EB] = 0x00; // SPKR_DRV_STATUS_OCP
  shadow_register[0x1EC] = 0x00; // SPKR_DRV_STATUS_PA
  shadow_register[0x1FA] = 0x46; // RC_OSC_FREQ
  shadow_register[0x1FB] = 0x0A; // RC_OSC_TEST
  shadow_register[0x1FC] = 0x18; // RC_OSC_STATUS
  shadow_register[0x1FD] = 0x00; // RC_OSC_TUNER
  shadow_register[0x1FE] = 0x44; // MBHC_HPH
  shadow_register[0x200] = 0x00; // CDC_ANC1_B1_CTL
  shadow_register[0x201] = 0x00; // CDC_ANC1_SHIFT
  shadow_register[0x202] = 0x00; // CDC_ANC1_IIR_B1_CTL
  shadow_register[0x203] = 0x00; // CDC_ANC1_IIR_B2_CTL
  shadow_register[0x204] = 0x00; // CDC_ANC1_IIR_B3_CTL
  shadow_register[0x206] = 0x00; // CDC_ANC1_LPF_B1_CTL
  shadow_register[0x207] = 0x00; // CDC_ANC1_LPF_B2_CTL
  shadow_register[0x209] = 0x00; // CDC_ANC1_SPARE
  shadow_register[0x20A] = 0x00; // CDC_ANC1_SMLPF_CTL
  shadow_register[0x20B] = 0x00; // CDC_ANC1_DCFLT_CTL
  shadow_register[0x20C] = 0x00; // CDC_ANC1_GAIN_CTL
  shadow_register[0x20D] = 0x00; // CDC_ANC1_B2_CTL
  shadow_register[0x220] = 0x00; // CDC_TX1_VOL_CTL_TIMER
  shadow_register[0x221] = 0x00; // CDC_TX1_VOL_CTL_GAIN
  shadow_register[0x222] = 0x00; // CDC_TX1_VOL_CTL_CFG
  shadow_register[0x223] = 0x08; // CDC_TX1_MUX_CTL
  shadow_register[0x224] = 0x03; // CDC_TX1_CLK_FS_CTL
  shadow_register[0x225] = 0x00; // CDC_TX1_DMIC_CTL
  shadow_register[0x228] = 0x00; // CDC_TX2_VOL_CTL_TIMER
  shadow_register[0x229] = 0x00; // CDC_TX2_VOL_CTL_GAIN
  shadow_register[0x22A] = 0x00; // CDC_TX2_VOL_CTL_CFG
  shadow_register[0x22B] = 0x08; // CDC_TX2_MUX_CTL
  shadow_register[0x22C] = 0x03; // CDC_TX2_CLK_FS_CTL
  shadow_register[0x22D] = 0x00; // CDC_TX2_DMIC_CTL
  shadow_register[0x230] = 0x00; // CDC_TX3_VOL_CTL_TIMER
  shadow_register[0x231] = 0x00; // CDC_TX3_VOL_CTL_GAIN
  shadow_register[0x232] = 0x00; // CDC_TX3_VOL_CTL_CFG
  shadow_register[0x233] = 0x08; // CDC_TX3_MUX_CTL
  shadow_register[0x234] = 0x03; // CDC_TX3_CLK_FS_CTL
  shadow_register[0x235] = 0x00; // CDC_TX3_DMIC_CTL
  shadow_register[0x238] = 0x00; // CDC_TX4_VOL_CTL_TIMER
  shadow_register[0x239] = 0x00; // CDC_TX4_VOL_CTL_GAIN
  shadow_register[0x23A] = 0x00; // CDC_TX4_VOL_CTL_CFG
  shadow_register[0x23B] = 0x08; // CDC_TX4_MUX_CTL
  shadow_register[0x23C] = 0x03; // CDC_TX4_CLK_FS_CTL
  shadow_register[0x23D] = 0x00; // CDC_TX4_DMIC_CTL
  shadow_register[0x278] = 0x00; // CDC_DEBUG_B1_CTL
  shadow_register[0x279] = 0x00; // CDC_DEBUG_B2_CTL    //changed from 0x00 to 0x10 for loopback testing
  shadow_register[0x27A] = 0x00; // CDC_DEBUG_B3_CTL
  shadow_register[0x27B] = 0x00; // CDC_DEBUG_B4_CTL
  shadow_register[0x27C] = 0x00; // CDC_DEBUG_B5_CTL
  shadow_register[0x27D] = 0x00; // CDC_DEBUG_B6_CTL
  shadow_register[0x27E] = 0x00; // CDC_DEBUG_B7_CTL
  shadow_register[0x280] = 0x00; // CDC_ANC2_B1_CTL 
  shadow_register[0x281] = 0x00; // CDC_ANC2_SHIFT
  shadow_register[0x282] = 0x00; // CDC_ANC2_IIR_B1_CTL
  shadow_register[0x283] = 0x00; // CDC_ANC2_IIR_B2_CTL
  shadow_register[0x284] = 0x00; // CDC_ANC2_IIR_B3_CTL
  shadow_register[0x286] = 0x00; // CDC_ANC2_LPF_B1_CTL
  shadow_register[0x287] = 0x00; // CDC_ANC2_LPF_B2_CTL
  shadow_register[0x289] = 0x00; // CDC_ANC2_SPARE
  shadow_register[0x28A] = 0x00; // CDC_ANC2_SMLPF_CTL
  shadow_register[0x28B] = 0x00; // CDC_ANC2_DCFLT_CTL
  shadow_register[0x28C] = 0x00; // CDC_ANC2_GAIN_CTL
  shadow_register[0x28D] = 0x00; // CDC_ANC2_B2_CTL
  shadow_register[0x2A0] = 0x00; // CDC_SRC1_PDA_CFG
  shadow_register[0x2A1] = 0x1B; // CDC_SRC1_FS_CTL
  shadow_register[0x2A8] = 0x00; // CDC_SRC2_PDA_CFG
  shadow_register[0x2A9] = 0x1B; // CDC_SRC2_FS_CTL
  shadow_register[0x2B0] = 0x00; // CDC_RX1_B1_CTL
  shadow_register[0x2B1] = 0x00; // CDC_RX1_B2_CTL
  shadow_register[0x2B2] = 0x00; // CDC_RX1_B3_CTL
  shadow_register[0x2B3] = 0x00; // CDC_RX1_B4_CTL
  shadow_register[0x2B4] = 0x78; // CDC_RX1_B5_CTL
  shadow_register[0x2B5] = 0x80; // CDC_RX1_B6_CTL
  shadow_register[0x2B6] = 0x00; // CDC_RX1_VOL_CTL_B1_CTL
  shadow_register[0x2B7] = 0x00; // CDC_RX1_VOL_CTL_B2_CTL
  shadow_register[0x2B8] = 0x00; // CDC_RX2_B1_CTL
  shadow_register[0x2B9] = 0x00; // CDC_RX2_B2_CTL
  shadow_register[0x2BA] = 0x00; // CDC_RX2_B3_CTL
  shadow_register[0x2BB] = 0x00; // CDC_RX2_B4_CTL
  shadow_register[0x2BC] = 0x78; // CDC_RX2_B5_CTL
  shadow_register[0x2BD] = 0x80; // CDC_RX2_B6_CTL
  shadow_register[0x2BE] = 0x00; // CDC_RX2_VOL_CTL_B1_CTL
  shadow_register[0x2BF] = 0x00; // CDC_RX2_VOL_CTL_B2_CTL
  shadow_register[0x2C0] = 0x00; // CDC_RX3_B1_CTL
  shadow_register[0x2C1] = 0x00; // CDC_RX3_B2_CTL
  shadow_register[0x2C2] = 0x00; // CDC_RX3_B3_CTL
  shadow_register[0x2C3] = 0x00; // CDC_RX3_B4_CTL
  shadow_register[0x2C4] = 0x78; // CDC_RX3_B5_CTL
  shadow_register[0x2C5] = 0x80; // CDC_RX3_B6_CTL
  shadow_register[0x2C6] = 0x00; // CDC_RX3_VOL_CTL_B1_CTL
  shadow_register[0x2C7] = 0x00; // CDC_RX3_VOL_CTL_B2_CTL
  shadow_register[0x2C8] = 0x00; // CDC_RX4_B1_CTL
  shadow_register[0x2C9] = 0x00; // CDC_RX4_B2_CTL
  shadow_register[0x2CA] = 0x00; // CDC_RX4_B3_CTL
  shadow_register[0x2CB] = 0x00; // CDC_RX4_B4_CTL
  shadow_register[0x2CC] = 0x78; // CDC_RX4_B5_CTL
  shadow_register[0x2CD] = 0x80; // CDC_RX4_B6_CTL      //changed from 0x80 to 0xC0 
  shadow_register[0x2CE] = 0x00; // CDC_RX4_VOL_CTL_B1_CTL
  shadow_register[0x2CF] = 0x00; // CDC_RX4_VOL_CTL_B2_CTL
  shadow_register[0x300] = 0x00; // CDC_CLK_ANC_RESET_CTL
  shadow_register[0x301] = 0x00; // CDC_CLK_RX_RESET_CTL
  shadow_register[0x302] = 0x00; // CDC_CLK_TX_RESET_B1_CTL
  shadow_register[0x303] = 0x00; // CDC_CLK_TX_RESET_B2_CTL
  shadow_register[0x304] = 0x00; // CDC_CLK_DMIC_B1_CTL
  shadow_register[0x305] = 0x00; // CDC_CLK_DMIC_B2_CTL
  shadow_register[0x306] = 0x00; // CDC_CLK_I2S_CTL RX
  shadow_register[0x307] = 0x00; // CDC_CLK_I2S_CTL TX
  shadow_register[0x308] = 0x00; // CDC_CLK_OTHR_RESET_B1_CTL
  shadow_register[0x309] = 0x00; // CDC_CLK_OTHR_RESET_B2_CTL
  shadow_register[0x30A] = 0x00; // CDC_CLK_TX_CLK_EN_B1_CTL
  shadow_register[0x30B] = 0x00; // CDC_CLK_TX_CLK_EN_B2_CTL
  shadow_register[0x30C] = 0x00; // CDC_CLK_OTHR_CTL
  shadow_register[0x30D] = 0x00; // CDC_CLK_RDAC_CLK_EN_CTL  //SET THE VALUE FROM 0 TO 20
  shadow_register[0x30E] = 0x00; // CDC_CLK_ANC_CLK_EN_CTL
  shadow_register[0x30F] = 0x00; // CDC_CLK_RX_B1_CTL
  shadow_register[0x310] = 0x00; // CDC_CLK_RX_B2_CTL
  shadow_register[0x311] = 0x00; // CDC_CLK_MCLK_CTL
  shadow_register[0x312] = 0x00; // CDC_CLK_PDM_CTL
  shadow_register[0x313] = 0x01; // CDC_CLK_SD_CTL     //changed to 0x03 from 0x00 to enable sidetone
  shadow_register[0x314] = 0x00; // CDC_CLK_POWER_CTL
  shadow_register[0x324] = 0x12; // CDC_CLSH_IDLE_HPH_THSD
  shadow_register[0x325] = 0x0C; // CDC_CLSH_IDLE_EAR_THSD
  shadow_register[0x326] = 0x18; // CDC_CLSH_FCLKONLY_HPH_THSD
  shadow_register[0x327] = 0x23; // CDC_CLSH_FCLKONLY_EAR_THSD
  shadow_register[0x328] = 0x00; // CDC_CLSH_K_ADDR
  shadow_register[0x329] = 0xA4; // CDC_CLSH_K_DATA
  shadow_register[0x32A] = 0xD7; // CDC_CLSH_I_PA_FACT_HPH_L
  shadow_register[0x32B] = 0x05; // CDC_CLSH_I_PA_FACT_HPH_U
  shadow_register[0x32C] = 0x60; // CDC_CLSH_I_PA_FACT_EAR_L
  shadow_register[0x32D] = 0x09; // CDC_CLSH_I_PA_FACT_EAR_U
  shadow_register[0x340] = 0x00; // CDC_IIR1_GAIN_B1_CTL     //changed to 0x08 from 0x00 to enable sidetone
  shadow_register[0x341] = 0x00; // CDC_IIR1_GAIN_B2_CTL
  shadow_register[0x342] = 0x00; // CDC_IIR1_GAIN_B3_CTL
  shadow_register[0x343] = 0x00; // CDC_IIR1_GAIN_B4_CTL
  shadow_register[0x344] = 0x00; // CDC_IIR1_GAIN_B5_CTL
  shadow_register[0x345] = 0x00; // CDC_IIR1_GAIN_B6_CTL
  shadow_register[0x346] = 0x00; // CDC_IIR1_GAIN_B7_CTL
  shadow_register[0x347] = 0x00; // CDC_IIR1_GAIN_B8_CTL
  shadow_register[0x348] = 0x40; // CDC_IIR1_CTL
  shadow_register[0x349] = 0x00; // CDC_IIR1_GAIN_TIMER_CTL
  shadow_register[0x34A] = 0x00; // CDC_IIR1_COEF_B1_CTL
  shadow_register[0x34B] = 0x00; // CDC_IIR1_COEF_B2_CTL
  shadow_register[0x350] = 0x00; // CDC_IIR2_GAIN_B1_CTL
  shadow_register[0x351] = 0x00; // CDC_IIR2_GAIN_B2_CTL
  shadow_register[0x352] = 0x00; // CDC_IIR2_GAIN_B3_CTL
  shadow_register[0x353] = 0x00; // CDC_IIR2_GAIN_B4_CTL
  shadow_register[0x354] = 0x00; // CDC_IIR2_GAIN_B5_CTL
  shadow_register[0x355] = 0x00; // CDC_IIR2_GAIN_B6_CTL
  shadow_register[0x356] = 0x00; // CDC_IIR2_GAIN_B7_CTL
  shadow_register[0x357] = 0x00; // CDC_IIR2_GAIN_B8_CTL
  shadow_register[0x358] = 0x40; // CDC_IIR2_CTL
  shadow_register[0x359] = 0x00; // CDC_IIR2_GAIN_TIMER_CTL
  shadow_register[0x35A] = 0x00; // CDC_IIR2_COEF_B1_CTL
  shadow_register[0x35B] = 0x00; // CDC_IIR2_COEF_B2_CTL
  shadow_register[0x360] = 0x00; // CDC_TOP_GAIN_UPDATE
  shadow_register[0x368] = 0x30; // CDC_COMP0_B1_CTL
  shadow_register[0x369] = 0xB5; // CDC_COMP0_B2_CTL
  shadow_register[0x36A] = 0x28; // CDC_COMP0_B3_CTL
  shadow_register[0x36B] = 0x37; // CDC_COMP0_B4_CTL
  shadow_register[0x36C] = 0x7F; // CDC_COMP0_B5_CTL
  shadow_register[0x36D] = 0x00; // CDC_COMP0_B6_CTL
  shadow_register[0x36E] = 0x03; // CDC_COMP0_SHUT_DOWN_STATUS
  shadow_register[0x36F] = 0x03; // CDC_COMP0_FS_CFG
  shadow_register[0x370] = 0x30; // CDC_COMP1_B1_CTL
  shadow_register[0x371] = 0xB5; // CDC_COMP1_B2_CTL
  shadow_register[0x372] = 0x28; // CDC_COMP1_B3_CTL
  shadow_register[0x373] = 0x37; // CDC_COMP1_B4_CTL
  shadow_register[0x374] = 0x7F; // CDC_COMP1_B5_CTL
  shadow_register[0x375] = 0x00; // CDC_COMP1_B6_CTL
  shadow_register[0x376] = 0x03; // CDC_COMP1_SHUT_DOWN_STATUS
  shadow_register[0x377] = 0x03; // CDC_COMP1_FS_CFG
  shadow_register[0x378] = 0x30; // CDC_COMP2_B1_CTL
  shadow_register[0x379] = 0xB5; // CDC_COMP2_B2_CTL
  shadow_register[0x37A] = 0x28; // CDC_COMP2_B3_CTL
  shadow_register[0x37B] = 0x37; // CDC_COMP2_B4_CTL
  shadow_register[0x37C] = 0x7F; // CDC_COMP2_B5_CTL
  shadow_register[0x37D] = 0x00; // CDC_COMP2_B6_CTL
  shadow_register[0x37E] = 0x03; // CDC_COMP2_SHUT_DOWN_STATUS
  shadow_register[0x37F] = 0x03; // CDC_COMP2_FS_CFG
  shadow_register[0x380] = 0x00; // CDC_CONN_RX1_B1_CTL
  shadow_register[0x381] = 0x00; // CDC_CONN_RX1_B2_CTL
  shadow_register[0x382] = 0x00; // CDC_CONN_RX1_B3_CTL
  shadow_register[0x383] = 0x00; // CDC_CONN_RX2_B1_CTL
  shadow_register[0x384] = 0x00; // CDC_CONN_RX2_B2_CTL
  shadow_register[0x385] = 0x00; // CDC_CONN_RX2_B3_CTL
  shadow_register[0x386] = 0x00; // CDC_CONN_RX3_B1_CTL
  shadow_register[0x387] = 0x00; // CDC_CONN_RX3_B2_CTL
  shadow_register[0x388] = 0x00; // CDC_CONN_RX4_B1_CTL
  shadow_register[0x389] = 0x00; // CDC_CONN_RX4_B2_CTL
  shadow_register[0x38A] = 0x00; // CDC_CONN_RX4_B3_CTL
  shadow_register[0x391] = 0x00; // CDC_CONN_ANC_B1_CTL
  shadow_register[0x392] = 0x00; // CDC_CONN_ANC_B2_CTL
  shadow_register[0x393] = 0x00; // CDC_CONN_TX_B1_CTL
  shadow_register[0x394] = 0x00; // CDC_CONN_TX_B2_CTL
  shadow_register[0x395] = 0x00; // CDC_CONN_TX_B3_CTL
  shadow_register[0x396] = 0x00; // CDC_CONN_TX_B4_CTL
  shadow_register[0x397] = 0x01; // CDC_CONN_EQ1_B1_CTL
  shadow_register[0x398] = 0x00; // CDC_CONN_EQ1_B2_CTL
  shadow_register[0x399] = 0x00; // CDC_CONN_EQ1_B3_CTL
  shadow_register[0x39A] = 0x00; // CDC_CONN_EQ1_B4_CTL
  shadow_register[0x39B] = 0x00; // CDC_CONN_EQ2_B1_CTL
  shadow_register[0x39C] = 0x00; // CDC_CONN_EQ2_B2_CTL
  shadow_register[0x39D] = 0x00; // CDC_CONN_EQ2_B3_CTL
  shadow_register[0x39E] = 0x00; // CDC_CONN_EQ2_B4_CTL
  shadow_register[0x39F] = 0x00; // CDC_CONN_SRC1_B1_CTL
  shadow_register[0x3A0] = 0x00; // CDC_CONN_SRC1_B2_CTL
  shadow_register[0x3A1] = 0x00; // CDC_CONN_SRC2_B1_CTL
  shadow_register[0x3A2] = 0x00; // CDC_CONN_SRC2_B2_CTL
  shadow_register[0x3A3] = 0x00; // CDC_CONN_TX_SB_B1_CTL
  shadow_register[0x3A4] = 0x00; // CDC_CONN_TX_SB_B2_CTL
  shadow_register[0x3A5] = 0x00; // CDC_CONN_TX_SB_B3_CTL
  shadow_register[0x3A6] = 0x00; // CDC_CONN_TX_SB_B4_CTL
  shadow_register[0x3A7] = 0x00; // CDC_CONN_TX_SB_B5_CTL
  shadow_register[0x3AD] = 0x00; // CDC_CONN_TX_SB_B11_CTL
  shadow_register[0x3AE] = 0x00; // CDC_CONN_RX_SB_B1_CTL
  shadow_register[0x3AF] = 0x00; // CDC_CONN_RX_SB_B2_CTL
  shadow_register[0x3B0] = 0x00; // CDC_CONN_CLSH_CTL
  shadow_register[0x3B1] = 0x01; // CDC_CONN_MISC
  shadow_register[0x3C0] = 0x00; // CDC_MBHC_EN_CTL
  shadow_register[0x3C1] = 0x00; // CDC_MBHC_FIR_B1_CFG
  shadow_register[0x3C2] = 0x06; // CDC_MBHC_FIR_B2_CFG
  shadow_register[0x3C3] = 0x03; // CDC_MBHC_TIMER_B1_CTL
  shadow_register[0x3C4] = 0x09; // CDC_MBHC_TIMER_B2_CTL
  shadow_register[0x3C5] = 0x1E; // CDC_MBHC_TIMER_B3_CTL
  shadow_register[0x3C6] = 0x45; // CDC_MBHC_TIMER_B4_CTL
  shadow_register[0x3C7] = 0x04; // CDC_MBHC_TIMER_B5_CTL
  shadow_register[0x3C8] = 0x78; // CDC_MBHC_TIMER_B6_CTL
  shadow_register[0x3C9] = 0x00; // CDC_MBHC_B1_STATUS
  shadow_register[0x3CA] = 0x00; // CDC_MBHC_B2_STATUS 
  shadow_register[0x3CB] = 0x00; // CDC_MBHC_B3_STATUS
  shadow_register[0x3CC] = 0x00; // CDC_MBHC_B4_STATUS
  shadow_register[0x3CD] = 0x00; // CDC_MBHC_B5_STATUS
  shadow_register[0x3CE] = 0xC0; // CDC_MBHC_B1_CTL
  shadow_register[0x3CF] = 0x5D; // CDC_MBHC_B2_CTL
  shadow_register[0x3D0] = 0x00; // CDC_MBHC_VOLT_B1_CTL
  shadow_register[0x3D1] = 0x00; // CDC_MBHC_VOLT_B2_CTL
  shadow_register[0x3D2] = 0x00; // CDC_MBHC_VOLT_B3_CTL
  shadow_register[0x3D3] = 0x00; // CDC_MBHC_VOLT_B4_CTL
  shadow_register[0x3D4] = 0x00; // CDC_MBHC_VOLT_B5_CTL
  shadow_register[0x3D5] = 0x00; // CDC_MBHC_VOLT_B6_CTL
  shadow_register[0x3D6] = 0xFF; // CDC_MBHC_VOLT_B7_CTL
  shadow_register[0x3D7] = 0x07; // CDC_MBHC_VOLT_B8_CTL
  shadow_register[0x3D8] = 0xFF; // CDC_MBHC_VOLT_B9_CTL
  shadow_register[0x3D9] = 0x7F; // CDC_MBHC_VOLT_B10_CTL
  shadow_register[0x3DA] = 0x00; // CDC_MBHC_VOLT_B11_CTL
  shadow_register[0x3DB] = 0x80; // CDC_MBHC_VOLT_B12_CTL
  shadow_register[0x3DC] = 0x00; // CDC_MBHC_CLK_CTL
  shadow_register[0x3DD] = 0x00; // CDC_MBHC_INT_CTL
  shadow_register[0x3DE] = 0x00; // CDC_MBHC_DEBUG_CTL
  shadow_register[0x3DF] = 0x00; // CDC_MBHC_SPARE

  switch( codec_info.version )
  {
    case E_WCD_VER_1P0:
      shadow_register[0x004] = 0x00; //CHIP_ID_BYTE_0
      shadow_register[0x008] = 0x20; //CHIP_VERSION
      shadow_register[0x171] = 0x0C; //TX_7_MBHC_EN
      shadow_register[0x18C] = 0x50; //BUCK_CTRL_CCL_4
      shadow_register[0x1AC] = 0xDE; //RX_HPH_CNP_WG_CTL
      shadow_register[0x1C2] = 0xF2; //RX_EAR_CNP
      shadow_register[0x1CE] = 0x00; //RX_LINE_1_TEST
      shadow_register[0x1D2] = 0x00; //RX_LINE_2_TEST
      shadow_register[0x1E2] = 0x98; //SPKR_DRV_OCP_CTL
      shadow_register[0x1E3] = 0x48; //SPKR_DRV_CLIP_DET
      shadow_register[0x1E4] = 0x28; //SPKR_DRV_IEC
      shadow_register[0x320] = 0x22; //CDC_CLSH_B1_CTL
      shadow_register[0x321] = 0x35; //CDC_CLSH_B2_CTL
      shadow_register[0x322] = 0x3B; //CDC_CLSH_B3_CTL
      shadow_register[0x323] = 0x04; //CDC_CLSH_BUCK_NCP_VARS
      shadow_register[0x32E] = 0x0D; //CDC_CLSH_V_PA_HD_EAR
      shadow_register[0x32F] = 0x0D; //CDC_CLSH_V_PA_HD_HPH
      shadow_register[0x330] = 0x3A; //CDC_CLSH_V_PA_MIN_EAR
      shadow_register[0x331] = 0x1D; //CDC_CLSH_V_PA_MIN_HPH
      break;
    case E_WCD_VER_2P0:
      shadow_register[0x004] = 0x01; //CHIP_ID_BYTE_0
      shadow_register[0x008] = 0x21; //CHIP_VERSION
      shadow_register[0x171] = 0x6C; //TX_7_MBHC_EN
      shadow_register[0x18C] = 0x51; //BUCK_CTRL_CCL_4
      shadow_register[0x1AC] = 0xDA; //RX_HPH_CNP_WG_CTL
      shadow_register[0x1C2] = 0xC0; //RX_EAR_CNP
      shadow_register[0x1CE] = 0x02; //RX_LINE_1_TEST
      shadow_register[0x1D2] = 0x02; //RX_LINE_2_TEST
      shadow_register[0x1E2] = 0x97; //SPKR_DRV_OCP_CTL
      shadow_register[0x1E3] = 0x01; //SPKR_DRV_CLIP_DET
      shadow_register[0x1E4] = 0x00; //SPKR_DRV_IEC
      shadow_register[0x320] = 0xE4; //CDC_CLSH_B1_CTL
      shadow_register[0x321] = 0x00; //CDC_CLSH_B2_CTL
      shadow_register[0x322] = 0x00; //CDC_CLSH_B3_CTL
      shadow_register[0x323] = 0x00; //CDC_CLSH_BUCK_NCP_VARS
      shadow_register[0x32E] = 0x00; //CDC_CLSH_V_PA_HD_EAR
      shadow_register[0x32F] = 0x00; //CDC_CLSH_V_PA_HD_HPH
      shadow_register[0x330] = 0x00; //CDC_CLSH_V_PA_MIN_EAR
      shadow_register[0x331] = 0x00; //CDC_CLSH_V_PA_MIN_HPH
      break;
  }

  HANDLE_UNUSED_PARAMETER(codec_info); 

  return E_WCD_SUCCESS;
}

/*
EA[0] = Instance Value, the instance number of the component within the SOC, usually 0x0
EA[1] = Device Index( Interface Device - 0x0, Framer Device - 0x1 ).
EA[3:2] = Product Code(PC) [0b[15:4]- Chip Identifier, 0b[3:0]- Chip Revision]
EA[5:4] = Manufacturer ID, the MI for Qualcomm is 0x0217

*/

uint8 enumeration_address_pgd[]  = { 0x00, 0x01, 0xE0, 0x00, 0x17, 0x02 };
uint8 enumeration_address_intf[] = { 0x00, 0x00, 0xE0, 0x00, 0x17, 0x02 };

/******************************************************************************
 **************                I2C BUS DEFINITIONS              ***************
 *****************************************************************************/
 uint8 wcd_I2C_slave_ID_reg[] = 
 {
  0x0C,
  0x0D,
  0x0E
 };

/**
 * \brief Sets the enumeration address for either I2C/SB PGD or SB INTF
 */
wcd_result wcd_set_enumeration_address(wcd_bus_type bus_type, uint8 bus_intf,
                                       uint8** enumeration_address, uint8* enum_addr_length)
{
  if (bus_type == E_WCD_SLIMBUS)
  {
    if (bus_intf == E_WCD_CODEC_MAIN)
    {
      *enumeration_address = enumeration_address_pgd;
      *enum_addr_length = sizeof(enumeration_address_pgd)/sizeof(uint8);
    }
    else if (bus_intf == E_WCD_INTF)
    {
      *enumeration_address = enumeration_address_intf;
      *enum_addr_length = sizeof(enumeration_address_intf)/sizeof(uint8);
    }
    else
    {
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }
  else if (bus_type == E_WCD_I2C)
  {
      *enumeration_address = wcd_I2C_slave_ID_reg;
      *enum_addr_length = sizeof(wcd_I2C_slave_ID_reg)/sizeof(uint8);
  }
  else
  {
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  return E_WCD_SUCCESS;
}

/**
 * \brief Initializes the cachable register array. Tells us which registers are cachable
 * or not. Each register is a bit in the array
 */
wcd_result 
wcd_initialize_noncachable_register(uint8* noncachable_registers, wcd_codec_info codec_info)
{
  HANDLE_UNUSED_PARAMETER(codec_info); 

  if (noncachable_registers == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing noncachable registers.>\n");
    return E_WCD_INVALID_HANDLE;
  }

  noncachable_registers[0] = 0xF2; // CHIP_ID_BYTE_3 | ... | CHIP_ID_BYTE_0 | CHIP_STATUS
  noncachable_registers[1] = 0x01; // CHIP_VERSION
  noncachable_registers[2] = 0x0;
  noncachable_registers[3] = 0x0;
  noncachable_registers[4] = 0x0;
  noncachable_registers[5] = 0x0;
  noncachable_registers[6] = 0x0;
  noncachable_registers[7] = 0x0;
  noncachable_registers[8] = 0x6;  // PROCESS_MONITOR_CTL2 | PROCESS_MONITOR_CTL1
  noncachable_registers[9] = 0xFE; // QFUSE_DATA_OUT7 | ...| QFUSE_DATA_OUT0 | QFUSE_STATUS
  noncachable_registers[10] = 0x3; // QFUSE_DATA_OUT7 | QFUSE_DATA_OUT6
  noncachable_registers[11] = 0x0;
  noncachable_registers[12] = 0x0;
  noncachable_registers[13] = 0x0;
  noncachable_registers[14] = 0x0;
  noncachable_registers[15] = 0x0;
  noncachable_registers[16] = 0x0;
  noncachable_registers[17] = 0x0;
  noncachable_registers[18] = 0x0;
  noncachable_registers[19] = 0xF; // INTR_STATUS3 |...|INTR_STATUS1
  noncachable_registers[20] = 0x0;
  noncachable_registers[21] = 0x0;
  noncachable_registers[22] = 0x0;
  noncachable_registers[23] = 0x0;
  noncachable_registers[24] = 0x1F; // TLMM Ctrl Registers 0xC0 to 0xC4
  noncachable_registers[25] = 0x0;
  noncachable_registers[26] = 0x0;
  noncachable_registers[27] = 0x0;
  noncachable_registers[28] = 0x0;
  noncachable_registers[29] = 0x0;
  noncachable_registers[30] = 0x0;
  noncachable_registers[31] = 0x0;
  noncachable_registers[32] = 0x0;
  noncachable_registers[33] = 0x0;
  noncachable_registers[34] = 0x0;
  noncachable_registers[35] = 0x0;
  noncachable_registers[36] = 0x0;
  noncachable_registers[37] = 0x0;
  noncachable_registers[38] = 0x0;
  noncachable_registers[39] = 0x0;
  noncachable_registers[40] = 0x0;
  noncachable_registers[41] = 0x08; // MBHC_INSERT_DET_STATUS
  noncachable_registers[42] = 0x0;
  noncachable_registers[43] = 0x18; // TX_1_2_SAR_ERR_CH2 | TX_1_2_SAR_ERR_CH1
  noncachable_registers[44] = 0x0;
  noncachable_registers[45] = 0x80; // TX_4_5_SAR_ERR_CH4
  noncachable_registers[46] = 0x21; // TX_7_MBHC_SAR_ERR | TX_4_5_SAR_ERR_CH5
  noncachable_registers[47] = 0x0;
  noncachable_registers[48] = 0x0;
  noncachable_registers[49] = 0x0;
  noncachable_registers[50] = 0x0;
  noncachable_registers[51] = 0x0;
  noncachable_registers[52] = 0x0;
  noncachable_registers[53] = 0x0;
  noncachable_registers[54] = 0x08; // RX_HPH_L_STATUS
  noncachable_registers[55] = 0x02; // RX_HPH_R_STATUS
  noncachable_registers[56] = 0x20; // RX_EAR_STATUS
  noncachable_registers[57] = 0x0;
  noncachable_registers[58] = 0x11; // RX_LINE_2_STATUS | RX_LINE_1_STATUS
  noncachable_registers[59] = 0x0;
  noncachable_registers[60] = 0x0;
  noncachable_registers[61] = 0x18; // SPKR_DRV_STATUS_PA | SPKR_DRV_STATUS_OCP
  noncachable_registers[62] = 0x0;
  noncachable_registers[63] = 0x70; // MBHC_HPH__VDD_FOR_CP_STATUS | RC_OSC_TUNER | RC_OSC_STATUS
  noncachable_registers[64] = 0xDF; // CDC_ANC2_LPF_B2_CTL | CDC_ANC2_LPF_B1_CTL | CDC_ANC2_IIR_B3_CTL
  noncachable_registers[65] = 0x0;
  noncachable_registers[66] = 0x0;
  noncachable_registers[67] = 0x0;
  noncachable_registers[68] = 0x04; // CDC_TX1_VOL_CTL_CFG
  noncachable_registers[69] = 0x04; // CDC_TX2_VOL_CTL_CFG
  noncachable_registers[70] = 0x04; // CDC_TX3_VOL_CTL_CFG
  noncachable_registers[71] = 0x04; // CDC_TX4_VOL_CTL_CFG
  noncachable_registers[72] = 0x0;
  noncachable_registers[73] = 0x0;
  noncachable_registers[74] = 0x0;
  noncachable_registers[75] = 0x0;
  noncachable_registers[76] = 0x0;
  noncachable_registers[77] = 0x0;
  noncachable_registers[78] = 0x0;
  noncachable_registers[79] = 0x0;
  noncachable_registers[80] = 0xDF; // CDC_ANC1_LPF_B2_CTL | CDC_ANC1_LPF_B1_CTL | CDC_ANC1_IIR_B3_CTL
  noncachable_registers[81] = 0x0;
  noncachable_registers[82] = 0x0;
  noncachable_registers[83] = 0x0;
  noncachable_registers[84] = 0x0;
  noncachable_registers[85] = 0x0;
  noncachable_registers[86] = 0x23; // CDC_RX1_B1_CTL
  noncachable_registers[87] = 0x23; // CDC_RX2_B1_CTL
  noncachable_registers[88] = 0x23; // CDC_RX3_B1_CTL
  noncachable_registers[89] = 0x23; // CDC_RX4_B1_CTL
  noncachable_registers[90] = 0x0;
  noncachable_registers[91] = 0x0;
  noncachable_registers[92] = 0x0;
  noncachable_registers[93] = 0x0;
  noncachable_registers[94] = 0x0;
  noncachable_registers[95] = 0x0;
  noncachable_registers[96] = 0x0;
  noncachable_registers[97] = 0x0;
  noncachable_registers[98] = 0x0;
  noncachable_registers[99] = 0x0;
  noncachable_registers[100] = 0x0;
  noncachable_registers[101] = 0x03; // CDC_CLSH_K_DATA | CDC_CLSH_K_ADDR
  noncachable_registers[102] = 0x0;
  noncachable_registers[103] = 0x0;
  noncachable_registers[104] = 0xF0; // CDC_IIR1_GAIN_B8_CTL |...|CDC_IIR1_GAIN_B5_CTL
  noncachable_registers[105] = 0x0C; // CDC_IIR1_COEF_B2_CTL | CDC_IIR1_COEF_B1_CTL
  noncachable_registers[106] = 0xF0; // CDC_IIR2_GAIN_B8_CTL |...|CDC_IIR2_GAIN_B5_CTL
  noncachable_registers[107] = 0x0C; // CDC_IIR2_COEF_B2_CTL | CDC_IIR2_COEF_B1_CTL
  noncachable_registers[108] = 0x01; // CDC_TOP_GAIN_UPDATE
  noncachable_registers[109] = 0x40; // COMP0_SHUT_DOWN_STATUS
  noncachable_registers[272] = 0x65; // 0x110
  noncachable_registers[111] = 0x40; // COMP2_SHUT_DOWN_STATUS
  noncachable_registers[112] = 0x0;
  noncachable_registers[113] = 0x0;
  noncachable_registers[114] = 0x0;
  noncachable_registers[115] = 0x0;
  noncachable_registers[116] = 0x0;
  noncachable_registers[117] = 0x0;
  noncachable_registers[118] = 0x0;
  noncachable_registers[119] = 0x0;
  noncachable_registers[120] = 0x4;  // CDC_MBHC_FIR_B2_CFG
  noncachable_registers[121] = 0x3E; // MBHC_B5_STATUS|...|CDC_MBHC_B1_STATUS
  noncachable_registers[122] = 0x0;
  noncachable_registers[123] = 0x0;
  noncachable_registers[124] = 0x0;
  noncachable_registers[332] = 0x0; //0x14c
  noncachable_registers[774] = 0x0; //0x306
  noncachable_registers[775] = 0x0; //0x307

  return E_WCD_SUCCESS;
}



/**
 * \brief All the writable registers in the codec including 
 * those one which are managed only by Hw. Register contents 
 * will be printed out for the register dump 
 *
 */
uint32 writable_registers[] = {
  0x000, 0x001, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00C, 0x00D, 0x00E,
  0x010, 0x012, 0x013, 0x018, 0x019, 0x020, 0x040, 0x041, 0x042, 0x043, 0x048,
  0x049, 0x04A, 0x04B, 0x04C, 0x04D, 0x04E, 0x04F, 0x050, 0x051, 0x080, 0x088,
  0x090, 0x094, 0x095, 0x096, 0x097, 0x098, 0x099, 0x09A, 0x09B, 0x09C, 0x09D, 
  0x09E, 0x09F, 0x0A0, 0x0A1, 0x0A2, 0x0A3, 0x0A4, 0x0A5, 0x0A6, 0x0A7, 0x0A8,
  0x0A9, 0x0AA, 0x0AB, 0x0AC, 0x0AD, 0x0AE, 0x0AF, 0x0C0, 0x0C1, 0x0C2, 0x0C3,
  0x0C4, 0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x108, 0x109, 0x110, 0x111,
  0x112, 0x113, 0x114, 0x115, 0x116, 0x117, 0x128, 0x129, 0x12A, 0x12B, 0x12C,
  0x12D, 0x12E, 0x12F, 0x130, 0x131, 0x132, 0x133, 0x134, 0x135, 0x136, 0x137,
  0x138, 0x139, 0x14A, 0x14B, 0x14C, 0x14E, 0x14F, 0x150, 0x151, 0x152, 0x153,
  0x154, 0x155, 0x156, 0x157, 0x158, 0x159, 0x15A, 0x15B, 0x15C, 0x15D, 0x15E,
  0x15F, 0x160, 0x167, 0x168, 0x169, 0x16A, 0x16B, 0x16C, 0x16D, 0x16E, 0x16F,
  0x170, 0x171, 0x172, 0x173, 0x174, 0x175, 0x176, 0x181, 0x182, 0x183, 0x184,
  0x185, 0x186, 0x187, 0x188, 0x189, 0x18A, 0x18B, 0x18C, 0x18D, 0x18E, 0x18F,
  0x190, 0x191, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197, 0x198, 0x199, 0x19A,
  0x19B, 0x19C, 0x19E, 0x19F, 0x1A0, 0x1A1, 0x1A2, 0x1A4, 0x1A5, 0x1A6, 0x1A7,
  0x1A8, 0x1A9, 0x1AA, 0x1AB, 0x1AC, 0x1AD, 0x1AE, 0x1AF, 0x1B0, 0x1B1, 0x1B2,
  0x1B3, 0x1B4, 0x1B5, 0x1B6, 0x1B7, 0x1B8, 0x1B9, 0x1BA, 0x1BB, 0x1BC, 0x1BD,
  0x1BE, 0x1BF, 0x1C0, 0x1C1, 0x1C2, 0x1C3, 0x1C5, 0x1C6, 0x1C7, 0x1C8, 0x1C9,
  0x1CA, 0x1CB, 0x1CC, 0x1CD, 0x1CE, 0x1CF, 0x1D0, 0x1D1, 0x1D2, 0x1D3, 0x1D4,
  0x1DD, 0x1DF, 0x1E0, 0x1E1, 0x1E2, 0x1E3, 0x1E4, 0x1E5, 0x1E6, 0x1E7, 0x1E8,
  0x1E9, 0x1EA, 0x1EB, 0x1EC, 0x1FA, 0x1FB, 0x1FC, 0x1FD, 0x1FE, 0x200, 0x201,
  0x202, 0x203, 0x204, 0x206, 0x207, 0x209, 0x20A, 0x20B, 0x20C, 0x20D, 0x220,
  0x221, 0x222, 0x223, 0x224, 0x225, 0x228, 0x229, 0x22A, 0x22B, 0x22C, 0x22D,
  0x230, 0x231, 0x232, 0x233, 0x234, 0x235, 0x238, 0x239, 0x23A, 0x23B, 0x23C,
  0x23D, 0x278, 0x279, 0x27A, 0x27B, 0x27C, 0x27D, 0x27E, 0x280, 0x281, 0x282, 
  0x283, 0x284, 0x286, 0x287, 0x289, 0x28A, 0x28B, 0x28C, 0x28D, 0x2A0, 0x2A1,
  0x2A8, 0x2A9, 0x2B0, 0x2B1, 0x2B2, 0x2B3, 0x2B4, 0x2B5, 0x2B6, 0x2B7, 0x2B8, 
  0x2B9, 0x2BA, 0x2BB, 0x2BC, 0x2BD, 0x2BE, 0x2BF, 0x2C0, 0x2C1, 0x2C2, 0x2C3,
  0x2C4, 0x2C5, 0x2C6, 0x2C7, 0x2C8, 0x2C9, 0x2CA, 0x2CB, 0x2CC, 0x2CD, 0x2CE,
  0x2CF, 0x300, 0x301, 0x302, 0x303, 0x304, 0x305, 0x306, 0x308, 0x309, 0x30A,
  0x30B, 0x30C, 0x30D, 0x30E, 0x30F, 0x310, 0x311, 0x312, 0x313, 0x314, 0x320,
  0x321, 0x322, 0x323, 0x324, 0x325, 0x326, 0x327, 0x328, 0x329, 0x32A, 0x32B, 
  0x32C, 0x32D, 0x32E, 0x32F, 0x330, 0x331, 0x340, 0x341, 0x342, 0x343, 0x344,
  0x345, 0x346, 0x347, 0x348, 0x349, 0x34A, 0x34B, 0x350, 0x351, 0x352, 0x353,
  0x354, 0x355, 0x356, 0x357, 0x358, 0x359, 0x35A, 0x35B, 0x360, 0x368, 0x369,
  0x36A, 0x36B, 0x36C, 0x36D, 0x36E, 0x36F, 0x370, 0x371, 0x372, 0x373, 0x374,
  0x375, 0x376, 0x377, 0x378, 0x379, 0x37A, 0x37B, 0x37C, 0x37D, 0x37E, 0x37F,
  0x380, 0x381, 0x382, 0x383, 0x384, 0x385, 0x386, 0x387, 0x388, 0x389, 0x38A,
  0x391, 0x392, 0x393, 0x394, 0x395, 0x396, 0x397, 0x398, 0x399, 0x39A, 0x39B,
  0x39C, 0x39D, 0x39E, 0x39F, 0x3A0, 0x3A1, 0x3A2, 0x3A3, 0x3A4, 0x3A5, 0x3A6,
  0x3A7, 0x3AD, 0x3AE, 0x3AF, 0x3B0, 0x3B1, 0x3B2, 0x3C0, 0x3C1, 0x3C2, 0x3C3, 
  0x3C4, 0x3C5, 0x3C6, 0x3C7, 0x3C8, 0x3C9, 0x3CA, 0x3CB, 0x3CC, 0x3CD, 0x3CE,
  0x3CF, 0x3D0, 0x3D1, 0x3D2, 0x3D3, 0x3D4, 0x3D5, 0x3D6, 0x3D7, 0x3D8, 0x3D9,
  0x3DA, 0x3DB, 0x3DC, 0x3DD, 0x3DE, 0x3DF };

/** 
 * \brief Dumps the registers for the target that are writable.
 */
wcd_result wcd_dump_target_registers(void)
{
  uint32 iterator = 0;
  uint8 value = 0;
  wcd_result return_code = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_0("=WCD_REG_DUMP <BEGIN> \n");

  return_code = wcd_activate_bus();
  
  for(iterator = 0; iterator < sizeof(writable_registers) / sizeof(uint32); iterator++)
  {
    return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)writable_registers[iterator], 0xFF, 0x0, &value);

    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    WCD_DEBUG_INFO_2("=WCD_REG_DUMP <[0x%x] = 0x%x>\n", (unsigned int)writable_registers[iterator], (unsigned int)value);
	WCD_SLEEP(1);// remove
  }

  if (return_code == E_WCD_SUCCESS)
  {
    WCD_DEBUG_INFO_0("=WCD_REG_DUMP <END>\n");
  }
  
  wcd_deactivate_bus();

  return return_code;

}

