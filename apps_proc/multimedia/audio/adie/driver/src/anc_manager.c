/**
@file anc_manager.c
@brief 
Implementation and internal functions of WCD ANC Manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/anc_manager.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/01/13   KNM		Refactored bus manager i/f and replaced E_WCD_PGD with
                    E_WCD_CODEC_MAIN to suit both I2C and SB PGD i/f.
12/15/10	SK		Initial revision
===============================================================================*/

#include "anc_manager.h"
#include "usecase_manager.h"
#include "wcd_gen_id_defines.h"
#include "anc_manager_target.h"
#include "bus_manager.h"

#ifdef SPL_LOG_SUPPORT
	#include "anc_manager.tmh"
#endif


/*

removing unused code from compilation but maintaining it in case it is needed

int32 FP_abs(int32 value)
{
  if (value >= 0)
  {
    return value;
  }
  else
  {
    return -value;
  }
}

uint16 twosComp(int16 value, int16 bits)
{
  uint16 res = 0;
  uint32 width = bits + 1;
  if (value >= 0)
  {
    res = (uint16)value;
  }
  else
  {
    res = -((-value) - (1<<width));
  }
  return res;
}

int16 inverseTwosComp(uint32 value, int16 bits)
{
  int16 res = 0;
  uint32 full_bits = 0;
  uint32 width = bits + 1;

  full_bits  = (1 << width) - 1;

  if (0 == (1 & (value >> (bits - 1))))
  {
    res = (int16)value;
  }
  else
  {
    res = -( (int16)((full_bits ^ value) + 1) );
  }
  return res;
}

// Fixed point multiplication of signed 32 bit numbers
int32 FP_mult(int32 val1,
              int32 val2)
{
  int32 prod = 0;
  if ((val1 > 0 && val2 > 0) ||
      (val1 < 0 && val2 < 0))
  {
    if (FP_abs(val1) > (int32)(MAX_INT/FP_abs(val2)))
    {
      prod = MAX_INT;
    }
  }
  else if ((val1 > 0 && val2 < 0) ||
           (val1 < 0 && val2 > 0))
  {
    if (FP_abs(val1) > (int32)(MAX_INT/FP_abs(val2)))
    {
      prod = -(int32)MAX_INT;
    }
  }
  if (prod == 0)
  {
    prod = val1 * val2;
  }
  return prod;
}

// Fixed point shift of signed 32 bit numbers
int32 FP_shift(int32 value,
               int32 shift)
{
  int32 rnd = 1 << (FP_abs(shift) - 1);
  int32 val_s = value;

  //underflow -> rounding errors
  if (shift < 0)
  {
    val_s = FP_abs(val_s) + rnd;
    val_s = val_s >> FP_abs(shift);
    val_s = (value > 0 ) ? val_s : -val_s;
  }
  // overflow -> saturation
  else if (shift > 0)
  {
    if (FP_abs(value) > (int32)((MAX_INT >> FP_abs(shift))))
    {
      if (value < 0)
      {
        val_s = -(int32)MAX_INT;
      }
      else
      {
        val_s = (int32)MAX_INT;
      }
    }
    else
    {
      val_s = value << FP_abs(shift);
    }
  }
  return val_s;
}

// Format fixed point value to register FP twos comp format
int32 FP_format(int32 value,
                int32 intb,
                int32 fracb,
                int32 max_val)
{
  value = FP_shift(value, -(WCD_ANC_COEFF_FRAC_BITS - fracb));

  //check for saturation
  if (value > max_val)
  {
    value = max_val;
  }
  else if (value < -max_val)
  {
     value = -max_val;
  }
  //convert to 2s compliment
  value = twosComp((uint16)value, (uint16)(intb + fracb));

  return value;
}
*/


wcd_result set_lr_mix_enable(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint16  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_LR_MIX_ENABLE_R1 : WCD_ANC_LR_MIX_ENABLE_R2;
  uint8   mask = WCD_ANC_LR_MIX_ENABLE_M;
  uint8   shift = WCD_ANC_LR_MIX_ENABLE_S;
  uint8   value = WCD_ANC_LR_MIX_ENABLE_FALSE;
  wcd_result rc = E_WCD_SUCCESS;

  //set LR enable to true in CDC_ANC_CTL for the appropriate channel if the output
  //channel is the current channel we are configuring AND if LR mix enable is set to TRUE

  if (anc_config.anc_lr_mix_enable == WCD_TRUE)
  {
    if (anc_config.anc_lr_mix_output_channel == channel)
    {
      value = WCD_ANC_LR_MIX_ENABLE_TRUE;
    }
  }

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  return rc;
}

wcd_result set_smlpf(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint16  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_SMLPF_ENABLE_R1 : WCD_ANC_SMLPF_ENABLE_R2;
  uint8   mask = WCD_ANC_SMLPF_ENABLE_M;
  uint8   shift = WCD_ANC_SMLPF_ENABLE_S;
  uint8   value = WCD_ANC_SMLPF_ENABLE_FALSE;
  wcd_result rc = E_WCD_SUCCESS;

  if (anc_config.smlpf_enable == WCD_TRUE)
  {
    value = WCD_ANC_SMLPF_ENABLE_TRUE;
  }

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  if (rc != E_WCD_SUCCESS)
  {
    return rc;
  }

  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_SMLPF_SHIFT_R1 : WCD_ANC_SMLPF_SHIFT_R2;
  mask = WCD_ANC_SMLPF_SHIFT_M;
  shift = WCD_ANC_SMLPF_SHIFT_S;
  value = (uint8)anc_config.smlpf_shift;

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  return rc;
}

wcd_result set_dcflt(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint16  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_DCFLT_ENABLE_R1 : WCD_ANC_DCFLT_ENABLE_R2;
  uint8   mask = WCD_ANC_DCFLT_ENABLE_M;
  uint8   shift = WCD_ANC_DCFLT_ENABLE_S;
  uint8   value = WCD_ANC_DCFLT_ENABLE_FALSE;
  wcd_result rc = E_WCD_SUCCESS;

  if (anc_config.dcflt_enable == WCD_TRUE)
  {
    value = WCD_ANC_DCFLT_ENABLE_TRUE;
  }

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  if (rc != E_WCD_SUCCESS)
  {
    return rc;
  }

  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_DCFLT_SHIFT_R1 : WCD_ANC_DCFLT_SHIFT_R2;
  mask = WCD_ANC_DCFLT_SHIFT_M;
  shift = WCD_ANC_DCFLT_SHIFT_S;
  value = (uint8)anc_config.dcflt_shift;

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  return rc;
}

wcd_result set_shift(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint16  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_SHIFT_R1 : WCD_ANC_SHIFT_R2;
  uint8   mask = WCD_ANC_SHIFT_M;
  uint8   shift = WCD_ANC_SHIFT_S;
  uint8   value = 0;
  wcd_result rc = E_WCD_SUCCESS;

  value |= (uint8)(anc_config.anc_ff_shift << WCD_ANC_SHIFT_FF_S);
  value |= (uint8)(anc_config.anc_fb_shift << WCD_ANC_SHIFT_FB_S);
  
  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  return rc;
}

wcd_result set_anc_gain(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint16  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_GAIN_CTL_R1 : WCD_ANC_GAIN_CTL_R2;
  uint8   mask = WCD_ANC_GAIN_CTL_M;
  uint8   shift = WCD_ANC_GAIN_CTL_S;
  uint8   value = (uint8)anc_config.adaptive_gain;
  wcd_result rc = E_WCD_SUCCESS;
  
  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  return rc;
}

wcd_result set_anc_ctl_enables(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint16 reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_B1_CTL_ENABLES_R1 : WCD_ANC_B1_CTL_ENABLES_R2;
  uint8  mask = WCD_ANC_B1_CTL_ENABLES_M;
  uint8  shift = WCD_ANC_B1_CTL_ENABLES_S;
  uint8  value = 0;
  wcd_result rc = E_WCD_SUCCESS;

  value |= anc_config.ff_in_enable << WCD_ANC_B1_CTL_ENABLES_FF_IN_ENABLE_S;
  value |= anc_config.hybrid_enable << WCD_ANC_B1_CTL_ENABLES_HYBRID_ENABLE_S;
  value |= anc_config.ff_out_enable << WCD_ANC_B1_CTL_ENABLES_FF_OUT_ENABLE_S;
  value |= anc_config.adaptive_gain_enable << WCD_ANC_B1_CTL_ENABLES_FFGAIN_ENABLE_S;

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);


  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_B2_CTL_ENABLES_R1 : WCD_ANC_B2_CTL_ENABLES_R2;
  mask = WCD_ANC_B2_CTL_ENABLES_M;
  shift = WCD_ANC_B2_CTL_ENABLES_S;

  value = 0;
  value |= anc_config.flex_enable << WCD_ANC_B2_CTL_ENABLES_FFLE_ENABLE_S;
  value |= anc_config.dmic_x2_enable << WCD_ANC_B2_CTL_ENABLES_DMIC_X2_ENABLE_S;
  
  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);

  return rc;
}

wcd_result set_anc_lpf_shift(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint16 reg;
  uint8  mask;
  uint8  shift;
  uint8  value = 0;

  wcd_result rc = E_WCD_SUCCESS;

  //first for FF
  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_LPF_FF_SHIFT_R1 : WCD_ANC_LPF_FF_SHIFT_R2;
  mask = WCD_ANC_LPF_SHIFT_M;
  shift = WCD_ANC_LPF_SHIFT_S;
  value = 0;

  value |= anc_config.anc_ff_lpf_shift[0] << WCD_ANC_LPF_S1_S;
  value |= anc_config.anc_ff_lpf_shift[1] << WCD_ANC_LPF_S2_S;

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);


  //now for FB
  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_LPF_FB_SHIFT_R1 : WCD_ANC_LPF_FB_SHIFT_R2;
  mask = WCD_ANC_LPF_SHIFT_M;
  shift = WCD_ANC_LPF_SHIFT_S;
  value = 0;

  value |= anc_config.anc_fb_lpf_shift[0] << WCD_ANC_LPF_S1_S;
  value |= anc_config.anc_fb_lpf_shift[1] << WCD_ANC_LPF_S2_S;

  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  return rc;
}

#define round(val) ((val>0) ?(val+0.5):(val - 0.5))
wcd_result set_anc_iir_coeff(wcd_adie_codec_db_anc_config anc_config, uint32 channel)
{
  uint32  i = 0;
  uint16  reg = 0;
  uint8   mask = 0;
  uint8   shift = 0;
  uint8   value = 0;
  int32   coeff = 0;
  uint32  u_coeff = 0;
  uint8   val_MSB = 0;
  uint8   val_LSB = 0;
  double  cal_gain = ((double)(anc_config.anc_gain))/8192;
  wcd_result rc = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_1("=WCD_93XX: <set_anc_iir_coeff: anc_config.anc_gain = [%d].>\n", anc_config.anc_gain);
  //set coeff ptr to the first coeff
  //ordering:
    //A[2] - A[8] -- FF denominators
    //B[1] - B[8] -- FF numerators
    //V[2] - V[7] -- FB denominators
    //W[1] - W[7] -- FB numerators
  
  //set PTR = 0
  reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_IIR_COEFF_PTR_R1 : WCD_ANC_IIR_COEFF_PTR_R2;
  mask = WCD_ANC_IIR_COEFF_PTR_M;
  shift = WCD_ANC_IIR_COEFF_PTR_S;
  value = WCD_ANC_IIR_COEFF_PTR_FIRST_COEFF;
  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);
  if (rc != E_WCD_SUCCESS)
  {
    return rc;
  }

  //write FF coeffs
  for(i = 0; i < WCD_ANC_NUM_IIR_FF_A_COEFFS + WCD_ANC_NUM_IIR_FF_B_COEFFS; i++)
  {
    coeff = anc_config.anc_ff_coeff[i];
    //if we are in the B coefficients have to do some GAIN multiplication
    if (i < WCD_ANC_NUM_IIR_FF_A_COEFFS)
    {
      coeff = (int32)round((double)(coeff)/16);
    }
    else
    {
		coeff = (int32)round((((double)coeff * cal_gain) / 16));
    }
	u_coeff = (uint32)coeff;
	WCD_DEBUG_INFO_2("=WCD_93XX: <set_anc_iir_coeff: anc_ff_coeff[%u] = [%d].>\n", i, coeff);

    val_MSB = (uint8) (WCD_ANC_IIR_COEFF_MSB_M & (u_coeff >> WCD_REGISTER_DEPTH));
    val_LSB = (uint8) (WCD_ANC_IIR_COEFF_LSB_M & u_coeff);

    //ordering is LSB 8bit followed by MSB 4bit
    reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_IIR_COEFF_R1 : WCD_ANC_IIR_COEFF_R2;
    mask = WCD_ANC_IIR_COEFF_LSB_M;
    shift = WCD_ANC_IIR_COEFF_S;
    value = val_LSB;
    rc = wcd_register_write(E_WCD_CODEC_MAIN, reg,mask,shift,value);
    if (rc != E_WCD_SUCCESS)
    {
      break;
    }
    mask = WCD_ANC_IIR_COEFF_MSB_M;
    shift = WCD_ANC_IIR_COEFF_S;
    value = val_MSB;
    rc = wcd_register_write(E_WCD_CODEC_MAIN, reg,mask,shift,value);
    if (rc != E_WCD_SUCCESS)
    {
      break;
    }
  }

  if (rc != E_WCD_SUCCESS)
  {
    return rc;
  }
  
  //write FB coeffs
  for(i = 0; i < WCD_ANC_NUM_IIR_FB_A_COEFFS + WCD_ANC_NUM_IIR_FB_B_COEFFS; i++)
  {
    coeff = anc_config.anc_fb_coeff[i];
    coeff = (int32)round((double)(coeff)/16);
    u_coeff = (uint32)coeff;
    val_MSB = (uint8) (WCD_ANC_IIR_COEFF_MSB_M & (u_coeff >> WCD_REGISTER_DEPTH));
    val_LSB = (uint8) (WCD_ANC_IIR_COEFF_LSB_M & u_coeff);

    WCD_DEBUG_INFO_2("=WCD_93XX: <set_anc_iir_coeff: anc_fb_coeff[%u] = [%u].>\n", i, u_coeff);
    //ordering is LSB 8 bit, followed by MSB 4bit
    
    reg = (channel == WCD_ANC_PRIMARY_CHANNEL) ? WCD_ANC_IIR_COEFF_R1 : WCD_ANC_IIR_COEFF_R2;
    mask = WCD_ANC_IIR_COEFF_LSB_M;
    shift = WCD_ANC_IIR_COEFF_S;
    value = val_LSB;
    rc = wcd_register_write(E_WCD_CODEC_MAIN, reg,mask,shift,value);
    if (rc != E_WCD_SUCCESS)
    {
      break;
    }
    mask = WCD_ANC_IIR_COEFF_MSB_M;
    shift = WCD_ANC_IIR_COEFF_S;
    value = val_MSB;
    rc = wcd_register_write(E_WCD_CODEC_MAIN, reg,mask,shift,value);
    if (rc != E_WCD_SUCCESS)
    {
      break;
    }
  }
  return rc;
}


wcd_result enable_anc_clk(wcd_adie_codec_db_anc_config *anc_config)
{
  wcd_result rc = E_WCD_SUCCESS;
  uint16  reg = WCD_ANC_CLK_R;
  uint8   mask =  WCD_ANC_CLK_M;
  uint8   shift = WCD_ANC_CLK_S;
  uint8   value = 0;

  //anc channel 1
  if (anc_config[0].default_enable != WCD_FALSE)
  {
    value |= WCD_TRUE << WCD_ANC1_CLK_FF_EN_S;
    if (anc_config[0].anc_feedback_enable == WCD_TRUE)
    {
      value |= WCD_TRUE << WCD_ANC1_CLK_FB_EN_S;
    }
  }

  //anc channel 2
  if (anc_config[1].default_enable != WCD_FALSE)
  {
    value |= WCD_TRUE << WCD_ANC2_CLK_FF_EN_S;
    if (anc_config[1].anc_feedback_enable == WCD_TRUE)
    {
      value |= WCD_TRUE << WCD_ANC2_CLK_FB_EN_S;
	  }
  }
  
  rc = wcd_register_write(E_WCD_CODEC_MAIN, reg,mask,shift,value);
  return rc;

}

wcd_result set_anc_connections(wcd_adie_codec_db_anc_config *anc_config)
{
  //target specific...this can change depending on how many input
  //devices are available per codec
  return wcd_set_anc_connections(anc_config);                                        
}

wcd_result validate_anc_configuration(wcd_adie_codec_db_anc_config *anc_config)
{
  uint32 iter = 0;
  
  for (iter = 0; iter < WCD_ANC_MAX_CHANNELS; iter++)
  {
    if ((int32)anc_config[iter].anc_ff_shift > WCD_MAX_ANC_SHIFT || 
        (int32)anc_config[iter].anc_ff_shift < WCD_MIN_ANC_SHIFT ||
        (int32)anc_config[iter].anc_fb_shift > WCD_MAX_ANC_SHIFT ||
        (int32)anc_config[iter].anc_fb_shift < WCD_MIN_ANC_SHIFT )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid ANC configuration. ANC FF/FB shift is not in range.>\n");
      return E_WCD_INVALID_ANC_CONFIGURATION;
    }

    if ((int32)anc_config[iter].smlpf_shift > WCD_MAX_ANC_SMLPF_SHIFT ||
        (int32)anc_config[iter].smlpf_shift < WCD_MIN_ANC_SMLPF_SHIFT ||
        (int32)anc_config[iter].dcflt_shift > WCD_MAX_ANC_DCFLT_SHIFT ||
        (int32)anc_config[iter].dcflt_shift < WCD_MIN_ANC_DCFLT_SHIFT )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid ANC configuration. ANC SMLPF/DCFLT shift is not in range.>\n");
      return E_WCD_INVALID_ANC_CONFIGURATION;
    }

    if ((int32)anc_config[iter].anc_fb_lpf_shift[0] > WCD_MAX_ANC_LPF_SHIFT ||
        (int32)anc_config[iter].anc_fb_lpf_shift[0] < WCD_MIN_ANC_LPF_SHIFT ||
        (int32)anc_config[iter].anc_fb_lpf_shift[1] > WCD_MAX_ANC_LPF_SHIFT ||
        (int32)anc_config[iter].anc_fb_lpf_shift[1] < WCD_MIN_ANC_LPF_SHIFT ||
        (int32)anc_config[iter].anc_ff_lpf_shift[0] > WCD_MAX_ANC_LPF_SHIFT ||
        (int32)anc_config[iter].anc_ff_lpf_shift[0] < WCD_MIN_ANC_LPF_SHIFT ||
        (int32)anc_config[iter].anc_ff_lpf_shift[1] > WCD_MAX_ANC_LPF_SHIFT ||
        (int32)anc_config[iter].anc_ff_lpf_shift[1] < WCD_MIN_ANC_LPF_SHIFT )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid ANC configuration. ANC FF/FB LPF shift is not in range.>\n");
      return E_WCD_INVALID_ANC_CONFIGURATION;
    }

    if (anc_config[iter].adaptive_gain > WCD_MAX_ANC_GAIN ||
        anc_config[iter].adaptive_gain < WCD_MIN_ANC_GAIN )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid ANC configuration. ANC adaptive gain is not in range.>\n");
      return E_WCD_INVALID_ANC_CONFIGURATION;
    }
  }

  return E_WCD_SUCCESS;
}

/**
 * \brief Applies the ANC configuration from ACDB. The anc configuration consists 
 * of 2 wcd_adie_codec_db_anc_config structures. Where the first is the first channel
 * and the second is the second channel
 *
 */
wcd_result wcd_apply_anc_configuration(wcd_adie_codec_db_anc_config *anc_config)
{
  wcd_result  return_code = E_WCD_SUCCESS;
  uint16      reg = 0;
  uint8       mask = 0;
  uint8       shift = 0;
  uint8       value = 0;
  uint32      iter = 0;
  
  do
  {
    return_code = validate_anc_configuration(anc_config);

    if(return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //reset ANC
    reg = WCD_ANC_RESET_R;
    mask = WCD_ANC_RESET_M;
    shift = WCD_ANC_RESET_S;
    value = WCD_ANC_RESET_ENABLE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift,value);

    if(return_code != E_WCD_SUCCESS)
    {
      break;
    }

    for(iter = 1; iter <= WCD_ANC_MAX_CHANNELS; iter++)
    {
      if (anc_config[iter-1].default_enable == WCD_FALSE)
      {
        continue;
      }

      return_code = set_lr_mix_enable(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = set_smlpf(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = set_dcflt(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = set_shift(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = set_anc_iir_coeff(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = set_anc_lpf_shift(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = set_anc_gain(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }
    }
    
    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //set ANC connections
    return_code = set_anc_connections(anc_config);
    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }
	
    //if ANC 1 is enabled, set it to HPHL/EAROUT
    if (anc_config[0].default_enable != WCD_FALSE)
    {
	    return_code = wcd_register_write(E_WCD_CODEC_MAIN, WCD_ANC_ANC1_OUTPUT_SEL_R, 
                                  WCD_ANC_ANC1_OUTPUT_SEL_M, 
                                  WCD_ANC_ANC1_OUTPUT_SEL_S, 
                                  WCD_ANC_ANC1_OUTPUT_SEL_EAR_HPHL);
    }

    //enable ANC (clock)
    return_code = enable_anc_clk(anc_config);
    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //unreset ANC
    value = WCD_ANC_RESET_DISABLE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift,value);
    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //turn on FF/FB ANC output
    for(iter = 1; iter <= WCD_ANC_MAX_CHANNELS; iter++)
    {
      if (anc_config[iter-1].default_enable == WCD_FALSE)
      {
        continue;
      }

      return_code = set_anc_ctl_enables(anc_config[iter-1], iter);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }
    }

  } while(WCD_G_FALSE_BOOLEAN);

  wcd_bus_flush(E_WCD_CODEC_MAIN);
  //we are done
  return return_code;
}

/**
 * \brief Disable ANC. Just Reset + disable clock and disable connections
 *
 */
wcd_result wcd_disable_anc_configuration(void)
{
  wcd_result  return_code = E_WCD_SUCCESS;
  uint16      reg = 0;
  uint8       mask = 0;
  uint8       shift = 0;
  uint8       value = 0;

  //reset ANC
  reg = WCD_ANC_RESET_R;
  mask = WCD_ANC_RESET_M;
  shift = WCD_ANC_RESET_S;
  value = WCD_ANC_RESET_ENABLE;
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift,value);
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //disable clock for both ANC channels
  reg = WCD_ANC_CLK_R;
  mask =  WCD_ANC_CLK_M;
  shift = WCD_ANC_CLK_S;
  value = WCD_FALSE << WCD_ANC1_CLK_FB_EN_S;
  value |= WCD_FALSE << WCD_ANC1_CLK_FF_EN_S;
  value |= WCD_FALSE << WCD_ANC2_CLK_FB_EN_S;
  value |= WCD_FALSE << WCD_ANC2_CLK_FF_EN_S;
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift,value);
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //disable anc connections
  return_code = wcd_disable_anc_connections();
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //reset this to 0...doesnt matter, disabling anc anyways
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, WCD_ANC_ANC1_OUTPUT_SEL_R, 
                                WCD_ANC_ANC1_OUTPUT_SEL_M, 
                                WCD_ANC_ANC1_OUTPUT_SEL_S, 
                                WCD_ANC_ANC1_OUTPUT_SEL_ZERO);
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //unreset ANC -- saves power
  reg = WCD_ANC_RESET_R;
  mask = WCD_ANC_RESET_M;
  shift = WCD_ANC_RESET_S;
  value = WCD_ANC_RESET_DISABLE;
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift,value);

  wcd_bus_flush(E_WCD_CODEC_MAIN);
  return return_code;
}

/**
 * \brief Reserves the ANC elements (input device + micbias manager) in the usecase.
 */
wcd_result wcd_reserve_anc_elements(wcd_usecase* usecase, wcd_adie_codec_db_anc_config* anc_configuration)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 anc_iterator = 0;

  wcd_element* anc_input_element = NULL;
  wcd_element* micbias_element = NULL;
  wcd_list* anc_element_list = NULL;

  do
  {
    if (usecase == NULL || anc_configuration == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Uscase or ANC config is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    //reserve the elements
    for(anc_iterator = 0; anc_iterator < WCD_ANC_MAX_CHANNELS; anc_iterator++)
    {
      WCD_DEBUG_INFO_1("=WCD_93XX: <ANC Iterator: %u.>\n", (unsigned int)anc_iterator);

      if (anc_configuration[anc_iterator].default_enable == WCD_FALSE || WCD_FALSE == anc_configuration[anc_iterator].input_device)
      {
        WCD_DEBUG_WARN_0("=WCD_93XX: anc_configuration[anc_iterator].input_device is NULL\n");
        continue;
      }
      anc_element_list = NULL;

      //set the anc element list here
      if (anc_iterator == 0)
      {
        anc_element_list = usecase->primary_anc_elements;
      }
      else
      {
        anc_element_list = usecase->secondary_anc_elements;
      }

      if (anc_element_list == NULL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not determine ANC element list.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
      }

      //map anc input device enum to wcd element
      return_code = wcd_map_anc_source(anc_configuration[anc_iterator].input_device,
                                        &anc_input_element);

      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      //figure out mic bias element
      if (anc_input_element->connectable_elements[0]->type == WCD_MICBIAS_MANAGER_TYPE_ID)
      {
        micbias_element = anc_input_element->connectable_elements[0];
      }
      else
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Micbias Manager is not first connectable element of the input device element.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
      }

      //push input
      return_code = wcd_push_to_list(anc_element_list, anc_input_element);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      //push mic bias element
      return_code = wcd_push_to_list(anc_element_list, micbias_element);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      //no need to reserve (set connected element)...this is the only place they should be used
      //except for micbias for internal micbiasing
      anc_input_element->connected_element = micbias_element;
    }

  } while (WCD_G_FALSE_BOOLEAN);

  return return_code;
}

/**
 * \brief Unreserves the ANC elements (input device + micbias manager) in the usecase.
 */
wcd_result wcd_unreserve_anc_elements(wcd_usecase* usecase)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_list_element* iterator = NULL;
  wcd_element* temp_element = NULL;
  do
  {
    if (usecase == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    //free primary anc elements - remove them from the list and set connected element to NULL
    if (usecase->primary_anc_elements != NULL)
    {
      iterator = usecase->primary_anc_elements->head; 

      while(iterator != NULL)
      {
        temp_element = (wcd_element*)iterator->data;
        iterator = iterator->next;

        temp_element->connected_element = NULL;

        return_code = wcd_remove_from_list(usecase->primary_anc_elements, (void*)temp_element);

        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }
      }
    }

    //repeat for secondary
    if (usecase->secondary_anc_elements != NULL)
    {
      iterator = usecase->secondary_anc_elements->head; 

      while(iterator != NULL)
      {
        temp_element = (wcd_element*)iterator->data;
        iterator = iterator->next;

        temp_element->connected_element = NULL;

        return_code = wcd_remove_from_list(usecase->secondary_anc_elements, (void*)temp_element);

        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }
      }
    }

  } while(WCD_G_FALSE_BOOLEAN);

  return return_code;
}

/**
 * \brief Enables ANC using the supplied ANC configuration. Also reserves the input elements
 * along with the micbias managers (needed for ANC) inside the usecase
 */
wcd_result wcd_enable_anc(wcd_usecase* usecase, wcd_adie_codec_db_anc_config* anc_configuration)
{
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Enable ANC.>\n");

    if (usecase == NULL || anc_configuration == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Uscase or ANC config is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

	  // check whether ANC feature is supported in codec or not
	  if ( wcd_is_anc_supported() != WCD_TRUE)
	  {
	    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error ANC feature is not supported.\n");
	    return_code = E_WCD_ANC_NOT_SUPPORTED;
	    break;
	  }

    //initialize element lists
    return_code = wcd_create_and_initialize_list ( (void**) &usecase->primary_anc_elements );
    if (return_code != E_WCD_SUCCESS) {
      break;
    }

    return_code = wcd_create_and_initialize_list ( (void**) &usecase->secondary_anc_elements );
    if (return_code != E_WCD_SUCCESS) {
      break;
    }

    //if AANC is enabled, do not reserve ANC elements or enable the ANC profile
    if ( (anc_configuration[0].adaptive && anc_configuration[0].default_enable) ||
          (anc_configuration[1].adaptive && anc_configuration[1].default_enable) )
    {
      usecase->adaptive_anc = WCD_TRUE;
    }

    if (!usecase->adaptive_anc)
    {
      //reserve ANC elements
      return_code = wcd_reserve_anc_elements(usecase, anc_configuration);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error reserving ANC elements in usecase.>\n");
        break;
      }

      //enable the input + micbias using the anc commands now
      return_code = wcd_enable_anc_profile(usecase);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error enabling ANC profile.>\n");
        break;
      }
    }    

    return_code = wcd_apply_anc_configuration(anc_configuration);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error applying ANC configuration.>\n");
    }
  
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Enable ANC. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Disables ANC from the usecase
 */
wcd_result wcd_disable_anc(wcd_usecase* usecase)
{
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Disable ANC.>\n");

	  // check whether ANC feature is supported in codec or not
	  if ( wcd_is_anc_supported() != WCD_TRUE)
	  {
	    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error ANC feature is not supported.\n");
	    return_code = E_WCD_ANC_NOT_SUPPORTED;
	    break;
	  }

    //disable anc configuration applied
    return_code = wcd_disable_anc_configuration();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error disabling ANC configuration.>\n");
    }

    //if adaptive ANC is not enabled, disable the profile or unreserve any elements
    if (!usecase->adaptive_anc)
    {
      //disable microphones using profiles
      return_code = wcd_disable_anc_profile(usecase);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error disabling ANC profile.>\n");
      }

      //unreserve elements
      return_code = wcd_unreserve_anc_elements(usecase);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error unreserving ANC elements.>\n");
      }
    }

    //FREE all the ANC things inside of the usecase
    WCD_FREE(usecase->primary_anc_elements, WCD_MALLOC_START_TAG + 8);
    WCD_FREE(usecase->secondary_anc_elements, WCD_MALLOC_START_TAG + 8);
    WCD_FREE(usecase->anc_command_list, WCD_MALLOC_START_TAG + 6);

    usecase->secondary_anc_elements = NULL;
    usecase->primary_anc_elements = NULL;

    usecase->adaptive_anc = WCD_FALSE;
    usecase->anc_command_list = NULL;
    usecase->anc_command_list_size = 0;
    usecase->anc_current_command = 0;
    
  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Disable ANC. RC:%d.>\n", return_code);

  return E_WCD_SUCCESS;
}



