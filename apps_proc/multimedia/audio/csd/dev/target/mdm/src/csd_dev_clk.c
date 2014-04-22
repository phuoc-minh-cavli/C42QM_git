/*==============================================================================
  FILE:         csd_dev_tgt_clk.c

  OVERVIEW:     The objective of this module is to manage the clock present.

  Dependencies:

                Copyright (c) 2008-12, 2016 QUALCOMM Technologies Incorporated.
                All rights reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to this module.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/mdm/src/csd_dev_clk.c#4 $
 $DateTime: 2019/06/11 06:14:45 $
 $Author: pwbldsvc $
 $Change: 19490074 $
 $Revision: #4 $

 when       who       what, where, why
 --------   ---       --------------------------------------------------------
 01/24/18   pm         Update clk_id before calling csd_dev_afe_set_clock
 11/15/16   rk         Abstracting OSAL layer
 07/03/13   vk         Changes to remove compiler warnings
 02/19/13   unni       clean up
                         -Removed clock frequency hard-coding
                         -Simplify logics
 02/15/13   aim        Added LPASS clock control support.
 07/24/12   unni       PCM Support
 07/13/12   rp         Added 9x25 AFE support.
 09/20/11   oa         added fix for SIF mux hw issue
 06/22/11   oa         created 9x15 version
 04/27/10   suns       Initial Version
 =============================================================================*/

#include "csd_q6msg.h"
#include "csd_dev_clk.h"
#include "csd_debug_msg.h"
#include "csd_dev_ioctl.h"
#include "csd_dev_afe.h"
#include "DALSys.h"
#include "msmhwiobase.h"
#include "csd_os_dependencies.h"

#define ENABLE                          1
#define DISABLE                         0
/* MAX clock frequency avaiable in the chip */
#define MAX_PCM_CLK_FREQ_4096_KHZ       4096000
/* Known Frequencies */
#define CSD_DEV_CODEC_INVALID_FREQ      0xFFFFFFFF
/* HW mux address */
#define LPASS_CSR_GP_LPAIF_PRI_PCM_PRI_MODE_MUXSEL  ( ULTAUDIO_CORE_BASE + 0x00002008 )
#define LPASS_CSR_GP_LPAIF_SEC_PCM_SEC_MODE_MUXSEL  ( ULTAUDIO_CORE_BASE + 0x0000200C )

#define I2S_PCM_SEL_MASK                            ( 0x00000001 )
#define LPASS_CSR_GP_IO_MUX_MIC_CTL                 ( ULTAUDIO_CORE_BASE + 0x00002000 )
#define TLMM_SCLK_EN_MASK_MIC                       ( 0x00020002 )  // change bit from 2 to 3E

#define LPASS_CSR_GP_IO_MUX_SPKR_CTL                ( ULTAUDIO_CORE_BASE + 0x00002004 )
#define TLMM_SCLK_EN_MASK                           ( 0x00000006 )  // change bit from 2 to 3E

/* SW workaround for MCLK routing issue at HW level in MDM9607
  * Set below registers to route
  * MCLK source (gcc_ultaudio_ext_mclk2)
  * to PLL test pad
 */
#define GCC_CLK_CTL_REG 0x01800000
#define GCC_GCC_DEBUG_CLK_CTL (GCC_CLK_CTL_REG + 0x74000)
#define GCC_PLLTEST_PAD_CFG (GCC_CLK_CTL_REG + 0x7400C)

#define GCC_DEBUG_CLK_CTL_EN_VALUE 0x100ec
#define GCC_PLLTEST_PAD_CFG_EN_VALUE 0x1200


/*=============================================================================

  FUNCTION      csd_dev_clk_cfg_is_req

  DESCRIPTION   static helper function to check if device requires a clock
                to be configured

  DEPENDENCIES  n/a

  PARAMETERS    @param dev : pointer to a device record

  RETURN VALUE  TRUE is clock configuration is required, else FALSE

  SIDE EFFECTS  none

=============================================================================*/
static bool_t csd_dev_clk_cfg_is_req(const struct csd_dev_record* dev)
{
  bool_t rc = FALSE;
  switch( dev->dev_info.interface_type )
  {
    case ACDB_DEVICE_TYPE_I2S_PARAM:
    case ACDB_DEVICE_TYPE_PCM_PARAM:
    case ACDB_DEVICE_TYPE_DIGITAL_MIC_PARAM:
      rc = TRUE;
      break;
    case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
    case ACDB_DEVICE_TYPE_INT_BTFM_PARAM:
    case ACDB_DEVICE_TYPE_HDMI_PARAM:
    case ACDB_DEVICE_TYPE_VIRTUAL_PARAM:
    case ACDB_DEVICE_TYPE_RT_PROXY_PARAM:
    default:
      rc = FALSE;
    break;
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_clk_get_src

  DESCRIPTION   static helper function to check if interface is configured
                as master, i.e. lpass is master for this interface

  DEPENDENCIES  @see adsp_afe_service_commands.h

  PARAMETERS    @param dev : pointer to a device record

  RETURN VALUE  AFE_PORT_LPACLK_CLK_SRC_EXTERNAL
                AFE_PORT_LPACLK_CLK_SRC_INTERNAL

  SIDE EFFECTS  none

=============================================================================*/
static uint16_t csd_dev_clk_get_attr(const struct csd_dev_record* dev)
{
  uint16_t clk_src = AFE_CLOCK_SET_CLOCK_ATTRIBUTE_INVALID;

    switch( dev->dev_info.interface_type )
    {
      case ACDB_DEVICE_TYPE_I2S_PARAM:
        /*
         *AFE_PORT_LPACLK_CLK_SRC_INTERNAL = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL
         *AFE_PORT_LPACLK_CLK_SRC_EXTERNAL = AFE_PORT_CONFIG_I2S_WS_SRC_EXTERNAL
         */
      clk_src = AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO;
        break;
      case ACDB_DEVICE_TYPE_PCM_PARAM:
        /*
         *AFE_PORT_PCM_SYNC_SRC_EXTERNAL = AFE_PORT_LPACLK_CLK_SRC_EXTERNAL
         *AFE_PORT_PCM_SYNC_SRC_INTERNAL = AFE_PORT_LPACLK_CLK_SRC_INTERNAL
         */
      clk_src = AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO;
        break;
      case ACDB_DEVICE_TYPE_DIGITAL_MIC_PARAM:
      case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
      case ACDB_DEVICE_TYPE_INT_BTFM_PARAM:
      case ACDB_DEVICE_TYPE_HDMI_PARAM:
      case ACDB_DEVICE_TYPE_VIRTUAL_PARAM:
      case ACDB_DEVICE_TYPE_RT_PROXY_PARAM:
      default:
      CSD_MSG(CSD_PRIO_ERROR,"Unknown clock source type");
        break;
    }
  return clk_src;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_clk_get_i2s_freq

  DESCRIPTION   Codec master clock is fixed = OSR of LPASS, calculate
                bit clock from param and verify against osr clock

  DEPENDENCIES  n/a

  PARAMETERS    @param bit_clk : reference to return bit clock frequency
                @param osr_clk : reference to return osr clock frequency
                @param dev : reference to device record

  RETURN VALUE  none

  SIDE EFFECTS  none

=============================================================================*/
static int32_t csd_dev_clk_get_i2s_freq( uint32_t *bit_clk, const struct csd_dev_record* dev )
{
  uint32_t freq = 0;

  
  freq = 2 * dev->curr_attrib.bits_per_sample * dev->curr_attrib.sample_rate;
  if(0)
  {
    CSD_MSG( CSD_PRIO_ERROR,"csd_dev_clk_get_i2s_freq: bit clk[%d] not integer divide of OSR[%d]",
             freq, CSD_DEV_CODEC_MCLK_FREQ);
	*bit_clk = 0;
    return CSD_EFAILED;
  }
  else
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_get_i2s_freq: I2S SCLK[%d]", freq);
    *bit_clk = freq;
    return 0;
  }
  
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_clk_get_pcm_freq

  DESCRIPTION   Frequency = sampling rate * slots per frame * bits per slot

  DEPENDENCIES  n/a

  PARAMETERS    @param bit_clk : reference to return bit clock frequency
                @param dev : reference to device record

  RETURN VALUE  @csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static int32_t csd_dev_tgt_clk_get_pcm_freq(uint32_t* bit_clk,
                                         const struct csd_dev_record* dev)
{
  uint32_t freq = 0;
  uint32_t frame_setting = 0;
  uint32_t sample_bitwidth = 0;
  uint32_t num_slots = 0;
  uint32_t sampling_rate = 0;
  int32_t rc = 0;

  if( !bit_clk )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_tgt_clk_get_pcm_freq: "
            "Null reference passed bit_clk[%x]", bit_clk);
    return CSD_EBADPARAM;
  }
 
  frame_setting = dev->dev_info.u.pcm_dev_info.afe_frame_setting;
  sample_bitwidth = dev->curr_attrib.bits_per_sample;
  sampling_rate = dev->curr_attrib.sample_rate;

  /* ulAfeFrameSettign :bits width per frame
   * 8bits (0)
   * 16bits (1)
   * 32bits (2)
   * 64bits (3)
   * 128bits (4)
   * 256bits (5) */
  num_slots = (8 << frame_setting) / sample_bitwidth;

  /* Frequency = sampling rate * slots per frame * bits per slot */
  freq = sampling_rate * num_slots * sample_bitwidth;

  /* Check for Max Frequency */
  if( freq > MAX_PCM_CLK_FREQ_4096_KHZ )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_tgt_clk_get_pcm_freq: "
            "PCM Clk frequency > Max (%d)",
        MAX_PCM_CLK_FREQ_4096_KHZ);
    freq = 0;
    rc = CSD_EFAILED;
  }
  
  *bit_clk = freq;
  CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_tgt_clk_get_pcm_freq: pcm clk freq [%d]",freq);
  
  return rc;  
}

/*===========================================================================

  FUNCTION      csd_dev_tgt_set_clk_param

  DESCRIPTION   Sets clock related params.

  DEPENDENCIES  n/a

  PARAMETERS    @param bit_clk : bit clock (optional)
                @param osr_clk : osr clock (optional)
                @param clk_src : AFE_PORT_LPACLK_CLK_SRC_EXTERNAL/
                                 AFE_PORT_LPACLK_CLK_SRC_INTERNAL .
                                 @see adsp_afe_serivce.h
                @param lpaif_clk : return
 
  RETURN VALUE  void

  SIDE EFFECTS  none

=============================================================================*/
static void csd_dev_tgt_set_clk_param( uint32_t clk_in_freq, uint16_t clk_attri,
                                       afe_param_id_clock_set_t* lpaif_clk )
{
  if( !lpaif_clk )
  {
    CSD_MSG( CSD_PRIO_ERROR,"csd_dev_tgt_set_clk_param: Null param [lpaif_clk]" );
    return;
  }
  
  csd_memset(lpaif_clk,0,sizeof(afe_param_id_clock_set_t));

  lpaif_clk->clk_set_minor_version = AFE_API_VERSION_CLOCK_SET;
  lpaif_clk->clk_attri = clk_attri ;
  if( clk_in_freq != CSD_DEV_CODEC_INVALID_FREQ )
  {
    lpaif_clk->clk_freq_in_hz = clk_in_freq;
    lpaif_clk->enable = (clk_in_freq == 0 ) ? 0 : 1;
  }
  lpaif_clk->clk_root = AFE_PORT_LPACLK_CLK_ROOT_DEFAULT;
}
/*=============================================================================

  FUNCTION      csd_dev_clk_config_i2s_pcm_mux

  DESCRIPTION   Selects the I2S/PCM mux.

  DEPENDENCIES  n/a

  PARAMETERS    @param afe_port_id : afe port id
 
  RETURN VALUE  void

  SIDE EFFECTS  none

=============================================================================*/
static int32_t csd_dev_clk_config_i2s_pcm_mux( uint32_t afe_port_id )
{
  uint32_t mux_value = 0;
  uint32_t mux_address = 0;  
  uint32_t curr_value = 0;
  uint32_t rc = CSD_EOK;  

  switch( afe_port_id )
  {
    case AFE_PORT_ID_PRIMARY_MI2S_RX:
    case AFE_PORT_ID_PRIMARY_MI2S_TX:
      mux_address = LPASS_CSR_GP_LPAIF_PRI_PCM_PRI_MODE_MUXSEL;
      mux_value = 0x00000000;
    break;
    
    case AFE_PORT_ID_SECONDARY_MI2S_RX:
    case AFE_PORT_ID_SECONDARY_MI2S_TX:
      mux_address = LPASS_CSR_GP_LPAIF_SEC_PCM_SEC_MODE_MUXSEL;
      mux_value = 0x00000000;
    break;
    
    case AFE_PORT_ID_PRIMARY_PCM_RX:
    case AFE_PORT_ID_PRIMARY_PCM_TX:
      mux_address = LPASS_CSR_GP_LPAIF_PRI_PCM_PRI_MODE_MUXSEL;
      mux_value = 0x00000001;
    break;
    
    case AFE_PORT_ID_SECONDARY_PCM_RX:
    case AFE_PORT_ID_SECONDARY_PCM_TX:
      mux_address = LPASS_CSR_GP_LPAIF_SEC_PCM_SEC_MODE_MUXSEL;
      mux_value = 0x00000001;
    break;
    
    default:
      rc = CSD_EBADPARAM;
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_clk_config_i2s_pcm_mux: no mux configuration for afe_port[0x%x]", afe_port_id );
    break;
  }
  
  if( CSD_EOK == rc )
  {
    curr_value = inpdw( mux_address );
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: mux[0x%x] has value[%d]", mux_address, curr_value );
    
    if( curr_value != mux_value )
    {      
      outpdw( mux_address, mux_value );
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: write value[%d] to mux[0x%x]", mux_value, mux_address );
    }
  }
  
  // Enable Speaker MUX
  if( rc == CSD_EOK )
  {
    mux_address = LPASS_CSR_GP_IO_MUX_SPKR_CTL;    
    mux_value = 0x00000006; //changed bit from 0x6 to 0x3E
    curr_value = inpdw( mux_address );    
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: mux[0x%x] has value[%d]", mux_address, curr_value );
        
    if( (curr_value&TLMM_SCLK_EN_MASK) !=  mux_value )
    { // Bit 2(TLMM_SCLK_EN) need to be 1 for BCLK going out from MDM
      mux_value = curr_value | TLMM_SCLK_EN_MASK;
      outpdw( mux_address, mux_value );
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: write value[%d] to mux[0x%x]", mux_value, mux_address );
    } 
    curr_value = inpdw( mux_address );    
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux_outside: mux[0x%x] has value[%d]", mux_address, curr_value );

  }  
  
  // Enable Mic MUX
  if( rc == CSD_EOK )
  {
    mux_address = LPASS_CSR_GP_IO_MUX_MIC_CTL;    
    mux_value = 0x00020002;  //changed bit from 0x2 to 0x3E
    curr_value = inpdw( mux_address );    
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: mux[0x%x] has value[%d]", mux_address, curr_value );
        
    if( (curr_value&TLMM_SCLK_EN_MASK_MIC) !=  mux_value )
    { // Bit 1(TLMM_SCLK_EN) need to be 1 for BCLK going out from MDM
      mux_value = curr_value | TLMM_SCLK_EN_MASK_MIC;
      outpdw( mux_address, mux_value );
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: write value[%d] to mux[0x%x]", mux_value, mux_address );
    }

    curr_value = inpdw( mux_address );    
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux_outside: mux[0x%x] has value[%d]", mux_address, curr_value );
    
  }
  // MCLK SW workaround for 9206

  if( rc == CSD_EOK )    //change make the whole function as comment
  {
    mux_address = GCC_GCC_DEBUG_CLK_CTL;    
    mux_value = 0x100ec;
    outpdw( mux_address, mux_value );
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: PLL_PAD write value[%d] to mux[0x%x]", mux_value, mux_address );

    mux_address = GCC_PLLTEST_PAD_CFG;	  
    mux_value = 0x1200;
    outpdw( mux_address, mux_value );
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_config_i2s_pcm_mux: PLL_PAD write value[%d] to mux[0x%x]", mux_value, mux_address );
  } 

  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_set_clk

  DESCRIPTION   Configures the clock

  DEPENDENCIES  n/a

  PARAMETERS    @param dev : device record.
                @param enable : Enable or Disable the clock

  RETURN VALUE  void

  SIDE EFFECTS  need to configure

=============================================================================*/
static int32_t csd_dev_set_clk( const struct csd_dev_record* dev, bool_t enable )
{
  uint32_t bit_clk = CSD_DEV_CODEC_INVALID_FREQ;
  int32_t rc = CSD_EOK;
  afe_param_id_clock_set_t lpaif_clk;  
   
  csd_memset(&lpaif_clk, 0, sizeof(afe_param_id_clock_set_t));
  
  // Steps
  // 1) Get the required clocks based on AFE port type
  // 2) Get the source of the clock (internal/external)
  // 3) Send the config to AFE
  
  switch( dev->dev_info.afe_port_id )
  {
    case AFE_PORT_ID_PRIMARY_MI2S_RX:
    case AFE_PORT_ID_PRIMARY_MI2S_TX:
    case AFE_PORT_ID_SECONDARY_MI2S_RX:
    case AFE_PORT_ID_SECONDARY_MI2S_TX:
      if( enable )
      {
        rc = csd_dev_clk_get_i2s_freq( &bit_clk, dev );
      }
      else
      {
        bit_clk = 0;
      }
    break;
    
    case AFE_PORT_ID_PRIMARY_PCM_RX:
    case AFE_PORT_ID_PRIMARY_PCM_TX:
    case AFE_PORT_ID_SECONDARY_PCM_RX:
    case AFE_PORT_ID_SECONDARY_PCM_TX:
      if( enable )
      {
        rc = csd_dev_tgt_clk_get_pcm_freq( &bit_clk, dev );
      }
      else
      {
        bit_clk = 0;
      }
    break;
    
    default:
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_set_clk: Unknown afe port id [0x%x] ", dev->dev_info.afe_port_id );
      rc = CSD_EBADPARAM;
    break;
  }

  if( rc == CSD_EOK )
  {
    uint16_t clk_attr = csd_dev_clk_get_attr(dev);    
    csd_dev_tgt_set_clk_param(bit_clk, clk_attr, &lpaif_clk);
    lpaif_clk.clk_id = AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT;
    rc = csd_dev_afe_set_clock( dev->dev_info.afe_port_id, &lpaif_clk );
    if( CSD_EOK != rc )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_set_clk: set clock failed for AFE port(0x%x), rc[0x%x]", dev->dev_info.afe_port_id, rc );
    }
  }
  
  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_set_shared_clk_param

  DESCRIPTION   Configures the clock

  DEPENDENCIES  n/a

  PARAMETERS    @param dev : device record.
                @param enable : Enable or Disable the clock

  RETURN VALUE  void

  SIDE EFFECTS  need to configure

=============================================================================*/
static int32_t csd_dev_set_shared_clk( uint16_t afe_port_id, bool_t enable )
{
  int32_t rc = CSD_EOK;
  afe_param_id_lpass_core_shared_clk_cfg_t lpass_shared_clk;
  
  csd_memset(&lpass_shared_clk, 0, sizeof(afe_param_id_lpass_core_shared_clk_cfg_t));       

  lpass_shared_clk.lpass_core_shared_clk_cfg_minor_version = AFE_API_VERSION_LPASS_CORE_SHARED_CLK_CONFIG;
  lpass_shared_clk.enable = enable;   
  
  rc = csd_dev_afe_set_lpass_shared_clock( afe_port_id, &lpass_shared_clk );
  if( rc != CSD_EOK )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_set_shared_clk: csd_dev_afe_set_lpass_shared_clock failed, rc[0x%x], enable[%d]", rc, enable );
  }
  
  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_config_lpass_mux

  DESCRIPTION   Configures the clock

  DEPENDENCIES  n/a

  PARAMETERS    @param dev : device record.
                @param lpaif_clk : clks for DSP to configure

  RETURN VALUE  void

  SIDE EFFECTS  need to configure

=============================================================================*/
static int32_t csd_dev_config_lpass_mux( uint16_t afe_port_id )
{  
  int32_t rc = CSD_EOK;  
    
  // Config required LPASS MUX
  // 1) Enabled LPASS shared clock to access the MUX registers
  // 2) Read/Write to the registers
  // 3) Disable LPASS shared clock
  rc = csd_dev_set_shared_clk( afe_port_id, ENABLE );
  
  if( rc == CSD_EOK )
  {
    rc = csd_dev_clk_config_i2s_pcm_mux( afe_port_id );
    rc |= csd_dev_set_shared_clk( afe_port_id, DISABLE );
  }
  
  return rc;
}                                                                               
/*=============================================================================

  FUNCTION      csd_dev_tgt_clk_enable

  DESCRIPTION   Enable clocks for a particular device. If the clocks are already
                enabled, just increase the reference counter.

  DEPENDENCIES  

  PARAMETERS    @param a_dev : device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_clk_enable(const struct csd_dev_record* dev)
{
  int32_t rc = CSD_EOK;
  uint16_t afe_port_id = 0;
   
  if( !dev )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_clk_enable: Unexpected NULL param[dev]" );
    return CSD_EBADPARAM;
  }
  
  if( dev->dev_info.is_virtual )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_enable: clk config not required for virtual device" );
    return CSD_EOK;
  }
  
  if( !csd_dev_clk_cfg_is_req(dev) )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_enable: clk config not required for interface[0x%x]", dev->dev_info.interface_type );
    return CSD_EOK;
  }
    
  // Steps:
  // 1) Config LPASS MUX
  // 2) Enable Clock    
  afe_port_id = dev->dev_info.afe_port_id;  
  rc = csd_dev_config_lpass_mux( afe_port_id );      
    
  if( rc == CSD_EOK )
  {
    rc = csd_dev_set_clk( dev, ENABLE );
  }  
  
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_clk_disable

  DESCRIPTION   Decrement the reference count for the number of device sharing
                a clock. If the number of device using a clock is 0, then
                disable that clock

  DEPENDENCIES  n/a

  PARAMETERS    @param a_dev : referenc to a device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_clk_disable(const struct csd_dev_record* dev)
{
  int32_t rc = CSD_EOK;  
  afe_param_id_clock_set_t lpaif_clk;
  
  csd_memset(&lpaif_clk, 0, sizeof(lpaif_clk));  

  if( !dev )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_clk_disable: Unexpected NULL param[dev]" );
    return CSD_EBADPARAM;
  }
  
  if( dev->dev_info.is_virtual )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_disable: clk config not required for virtual device" );
    return CSD_EOK;
  }
  
  if( !csd_dev_clk_cfg_is_req(dev) )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_clk_disable: clk config not required for interface[0x%x]", dev->dev_info.interface_type );
    return CSD_EOK;
  }
    
  rc = csd_dev_set_clk( dev, DISABLE );  
                
  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_tgt_clk_init

  DESCRIPTION   Initialize the clk records

  DEPENDENCIES  

  PARAMETERS    n/a


  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_clk_init(void)
{  
  return CSD_EOK;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_clk_deinit

  DESCRIPTION   

  DEPENDENCIES  

  PARAMETERS    n\a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_clk_deinit(void)
{
  return CSD_EOK;
}

/*=============================================================================

  FUNCTION      csd_dev_clk_codec_mclk_control

  DESCRIPTION   Enable/Disable MCLK Source

  DEPENDENCIES

  PARAMETERS    @param enable : enable/disable flag


  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_clk_codec_mclk_control(bool_t enable)
{
  afe_param_id_clock_set_t lpaif_clk = {0};
  int32_t rc = CSD_EOK;

  //CSD_MSG(CSD_PRIO_HIGH, "Entering csd_dev_clk_codec_mclk_control: "
          //  "MCLK Control [%d] ", enable);

  if( enable )
  {        
    csd_dev_tgt_set_clk_param(CSD_DEV_CODEC_MCLK_FREQ, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, &lpaif_clk);
  }
  else
  {       
    csd_dev_tgt_set_clk_param(0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, &lpaif_clk);
  }

  lpaif_clk.clk_id = AFE_CLOCK_SET_CLOCK_ID_MCLK_3;

  rc = csd_dev_afe_set_clock(AFE_PORT_ID_PRIMARY_MI2S_RX, &lpaif_clk);
  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_clk_codec_mclk_control: "
            "MCLK Control [%d] failed, rc[0x%x]", enable, rc);
  }

   //CSD_MSG(CSD_PRIO_ERROR, "Custom Message : Leaving csd_dev_clk_codec_mclk_control  rc value [0x%x] ", rc); //added a print message.

  return rc;
}
