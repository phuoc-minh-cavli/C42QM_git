/*-------------------------------------------------------------------------------
     Copyright (c) 2013 QUALCOMM Technologies Incorporated.
         All rights reserved.
         Qualcomm Technologies Incorporated Confidential and Proprietary
   -----------------------------------------------------------------------------*/
/*=============================================================================
                           Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_exposed_registers.c#1 $

when        who      what, where, why
--------    ---      ------------------------------------------------------------
02/14/13    OA       Initial revision
===============================================================================*/
#include "wcd_intf.h"
#include "wcd_exposed_registers.h"

#ifdef SPL_LOG_SUPPORT
	#include "wcd_exposed_registers.tmh"
#endif

#define SB_OFFSET 0x800

/* ******************************************** */
/*            TOMTOM related data               */
/* ******************************************** */
//uint8 enumeration_address_pgd[]  = { 0x00, 0x01, 0x30, 0x01, 0x17, 0x02 };
static const wcd_codec_enumeration_address tomtom_v2_EA = {
	1,            //version
	0x01300100,   //device_enum_addr_lsw, 32bit lsw little endian
	0x00000217,   //device_enum_addr_msw, pad + 16bit msw little endian
	0,
	16
};

/* ********************************************* */
/*             AANC related data                 */
/* ********************************************* */

//AANC v1
#define CDC_ANCn_LPF_B1_CTL     0x206
#define CDC_ANCn_LPF_B2_CTL     0x207
#define CDC_ANCn_LPF_B3_CTL     0x208

//AANC v2
#define CDC_ANCn_IIR_B1_CTL     0x202
#define   CDC_ANCn_GAIN_CTL     0x20C

static const wcd_register_info_v2 wcd_aanc_v2_register_table[] = {
//                        reg_addr,                         type, bmask, width, offset
  {CDC_ANCn_IIR_B1_CTL + SB_OFFSET,  E_WCD_AANC_FF_GAIN_ADAPTIVE,  0x04,  0x08, 0x80},
  {CDC_ANCn_IIR_B1_CTL + SB_OFFSET, E_WCD_ANC_FFGAIN_ADAPTIVE_EN,  0x08,  0x08, 0x80},
  {  CDC_ANCn_GAIN_CTL + SB_OFFSET,       E_WCD_ANC_GAIN_CONTROL,  0xFF,  0x08, 0x80} 
};

static const wcd_register_block_definition WCD_AANC_V2_BLOCK = {
  2,             //version;  
  sizeof(wcd_aanc_v2_register_table)/sizeof(wcd_register_info_v2), //num_elements_in_table;  
  wcd_aanc_v2_register_table,  //register_table[n]
  2              // HW version of the AANC block. 
};


/* ********************************************* */
/*              MAD related data                 */
/* ********************************************* */

#define CDC_MAD_MAIN_CTL_1      0xE0

#define CDC_MAD_AUDIO_CTL_3     0xE4
#define CDC_MAD_AUDIO_CTL_4     0xE5
#define CDC_MAD_ULTR_CTL_3      0xEE
#define CDC_MAD_ULTR_CTL_4      0xEF
#define CDC_MAD_BEACON_CTL_3    0xF5
#define CDC_MAD_BEACON_CTL_4    0xF6

#define INTR_MASK3              0x097
#define INTR_STATUS3            0x09B
#define INTR_CLEAR3             0x09F
#define INTR_DESTN3             0x090
//#define INTR_DESTN3             0x0AF


static const wcd_register_info_v2 wcd_mad_v1_register_table[] = {
//                          reg_addr,                                 type, bmask, width, offset
  { CDC_MAD_MAIN_CTL_1   + SB_OFFSET,            E_WCD_HW_MAD_AUDIO_ENABLE,  0x01,  0x08, 0x00},
  { CDC_MAD_MAIN_CTL_1   + SB_OFFSET,             E_WCD_HW_MAD_ULTR_ENABLE,  0x02,  0x08, 0x00},
  { CDC_MAD_MAIN_CTL_1   + SB_OFFSET,           E_WCD_HW_MAD_BEACON_ENABLE,  0x04,  0x08, 0x00},
  { CDC_MAD_AUDIO_CTL_3  + SB_OFFSET,        E_WCD_HW_MAD_AUDIO_SLEEP_TIME,  0x1F,  0x08, 0x00},
  { CDC_MAD_ULTR_CTL_3   + SB_OFFSET,         E_WCD_HW_MAD_ULTR_SLEEP_TIME,  0x1F,  0x08, 0x00},
  { CDC_MAD_BEACON_CTL_3 + SB_OFFSET,       E_WCD_HW_MAD_BEACON_SLEEP_TIME,  0x1F,  0x08, 0x00},
  { CDC_MAD_AUDIO_CTL_4  + SB_OFFSET,     E_WCD_HW_MAD_TX_AUDIO_SWITCH_OFF,  0x01,  0x08, 0x00},
  { CDC_MAD_ULTR_CTL_4   + SB_OFFSET,      E_WCD_HW_MAD_TX_ULTR_SWITCH_OFF,  0x01,  0x08, 0x00},
  { CDC_MAD_BEACON_CTL_4 + SB_OFFSET,    E_WCD_HW_MAD_TX_BEACON_SWITCH_OFF,  0x01,  0x08, 0x00},  
  { INTR_DESTN3          + SB_OFFSET, E_WCD_SVA_ENGINE_INT_DEST_SELECT_REG,  0x04,  0x08, 0x00},
  { INTR_DESTN3          + SB_OFFSET,  E_WCD_MAD_AUDIO_INT_DEST_SELECT_REG,  0x04,  0x08, 0x00},
  { INTR_DESTN3          + SB_OFFSET,    E_WCD_MAD_ULT_INT_DEST_SELECT_REG,  0x04,  0x08, 0x00},
  { INTR_DESTN3          + SB_OFFSET, E_WCD_MAD_BEACON_INT_DEST_SELECT_REG,  0x04,  0x08, 0x00},  
  { INTR_MASK3           + SB_OFFSET,        E_WCD_SVA_ENGINE_INT_MASK_REG,  0x01,  0x08, 0x00},
  { INTR_MASK3           + SB_OFFSET,         E_WCD_MAD_AUDIO_INT_MASK_REG,  0x02,  0x08, 0x00},
  { INTR_MASK3           + SB_OFFSET,           E_WCD_MAD_ULT_INT_MASK_REG,  0x04,  0x08, 0x00},
  { INTR_MASK3           + SB_OFFSET,        E_WCD_MAD_BEACON_INT_MASK_REG,  0x08,  0x08, 0x00},
  { INTR_STATUS3         + SB_OFFSET,      E_WCD_SVA_ENGINE_INT_STATUS_REG,  0x01,  0x08, 0x00},
  { INTR_STATUS3         + SB_OFFSET,       E_WCD_MAD_AUDIO_INT_STATUS_REG,  0x02,  0x08, 0x00},
  { INTR_STATUS3         + SB_OFFSET,         E_WCD_MAD_ULT_INT_STATUS_REG,  0x04,  0x08, 0x00},
  { INTR_STATUS3         + SB_OFFSET,      E_WCD_MAD_BEACON_INT_STATUS_REG,  0x08,  0x08, 0x00},  
  { INTR_CLEAR3          + SB_OFFSET,       E_WCD_SVA_ENGINE_INT_CLEAR_REG,  0x01,  0x08, 0x00},
  { INTR_CLEAR3          + SB_OFFSET,        E_WCD_MAD_AUDIO_INT_CLEAR_REG,  0x02,  0x08, 0x00},
  { INTR_CLEAR3          + SB_OFFSET,          E_WCD_MAD_ULT_INT_CLEAR_REG,  0x04,  0x08, 0x00},
  { INTR_CLEAR3          + SB_OFFSET,       E_WCD_MAD_BEACON_INT_CLEAR_REG,  0x08,  0x08, 0x00}
};

static const wcd_register_block_definition WCD_MAD_V1_BLOCK = {
  2,             //version;  
  sizeof(wcd_mad_v1_register_table)/sizeof(wcd_register_info_v2), //num_elements_in_table;
  wcd_mad_v1_register_table,  //register_table[n]
  1              // HW version of the MAD block. 
};



/* ********************************************* */
/*              VBAT related data                */
/* ********************************************* */

static const wcd_register_info_v2 wcd_vbat_v1_register_table[] = {
//                  reg_addr,                           type, bmask, width, offset
  { INTR_DESTN3  + SB_OFFSET, E_WCD_VBAT_INT_DEST_SELECT_REG,  0x04,  0x08, 0x00},  
  { INTR_MASK3   + SB_OFFSET,        E_WCD_VBAT_INT_MASK_REG,  0xC0,  0x08, 0x00},
  { INTR_STATUS3 + SB_OFFSET,      E_WCD_VBAT_INT_STATUS_REG,  0xC0,  0x08, 0x00},  
  { INTR_CLEAR3  + SB_OFFSET,       E_WCD_VBAT_INT_CLEAR_REG,  0xC0,  0x08, 0x00},
};

  
static const wcd_register_block_definition WCD_VBAT_V1_BLOCK = {
  2,             //version;
  sizeof(wcd_vbat_v1_register_table)/sizeof(wcd_register_info_v2), //num_elements_in_table;
  wcd_vbat_v1_register_table,  //register_table[n]
  1              // HW version of the vbat block. 
};


/* ********************************************* */
/*            SPKR CLIP related data             */
/* ********************************************* */

#define CDC_SPKR_CLIPDET_B1_CTL     0x365
#define CDC_SPKR2_CLIPDET_B1_CTL    0x366
#define CDC_SPKR_CLIPDET_VAL0       0x270
#define CDC_SPKR_CLIPDET_VAL1       0x271
#define CDC_SPKR_CLIPDET_VAL2       0x272
#define CDC_SPKR_CLIPDET_VAL3       0x273
#define CDC_SPKR_CLIPDET_VAL4       0x274
#define CDC_SPKR_CLIPDET_VAL5       0x275
#define CDC_SPKR_CLIPDET_VAL6       0x276
#define CDC_SPKR_CLIPDET_VAL7       0x277
#define CDC_SPKR2_CLIPDET_VAL0      0x3E8
#define CDC_SPKR2_CLIPDET_VAL1      0x3E9
#define CDC_SPKR2_CLIPDET_VAL2      0x3EA
#define CDC_SPKR2_CLIPDET_VAL3      0x3EB
#define CDC_SPKR2_CLIPDET_VAL4      0x3EC
#define CDC_SPKR2_CLIPDET_VAL5      0x3ED
#define CDC_SPKR2_CLIPDET_VAL6      0x3EE
#define CDC_SPKR2_CLIPDET_VAL7      0x3EF

static const wcd_register_info_v2 wcd_clip_v2_register_table[] = {
//                  reg_addr,                                   type, bmask, width, offset
  { INTR_DESTN3  + SB_OFFSET,          E_WCD_CLIP_INT_DEST_SELECT_REG,  0x04,  0x08, 0x00},
  { INTR_MASK3   + SB_OFFSET,                 E_WCD_CLIP_INT_MASK_REG,  0x10,  0x08, 0x00},
  { INTR_STATUS3 + SB_OFFSET,               E_WCD_CLIP_INT_STATUS_REG,  0x10,  0x08, 0x00},
  { INTR_CLEAR3  + SB_OFFSET,                E_WCD_CLIP_INT_CLEAR_REG,  0x10,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_B1_CTL  + SB_OFFSET,    E_WCD_CLIP_PIPE_BANK_SEL,  0x03,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL0    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL0,  0xFF,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL1    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL1,  0xFF,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL2    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL2,  0xFF,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL3    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL3,  0xFF,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL4    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL4,  0xFF,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL5    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL5,  0xFF,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL6    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL6,  0xFF,  0x08, 0x00},
  { CDC_SPKR_CLIPDET_VAL7    + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL7,  0xFF,  0x08, 0x00},
  { INTR_MASK3               + SB_OFFSET,   E_WCD_CLIP_INT_2_MASK_REG,  0x20,  0x08, 0x00},
  { INTR_STATUS3             + SB_OFFSET, E_WCD_CLIP_INT_2_STATUS_REG,  0x20,  0x08, 0x00},
  { INTR_CLEAR3              + SB_OFFSET,  E_WCD_CLIP_INT_2_CLEAR_REG,  0x20,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_B1_CTL + SB_OFFSET,    E_WCD_CLIP_PIPE_BANK_SEL,  0x03,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL0   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL0,  0xFF,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL1   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL1,  0xFF,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL2   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL2,  0xFF,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL3   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL3,  0xFF,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL4   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL4,  0xFF,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL5   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL5,  0xFF,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL6   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL6,  0xFF,  0x08, 0x00},
  { CDC_SPKR2_CLIPDET_VAL7   + SB_OFFSET, E_WCD_CDC_SPKR_CLIPDET_VAL7,  0xFF,  0x08, 0x00}
};

static const wcd_register_block_definition WCD_CLIP_V2_BLOCK = {
  2,             //version;
  sizeof(wcd_clip_v2_register_table)/sizeof(wcd_register_info_v2), //num_elements_in_table;
  wcd_clip_v2_register_table,  //register_table[n]
  2              // HW version of the speaker clip detection block. 
};


/* ********************************************* */
/*             PGD PORT related data             */
/* ********************************************* */

#define SB_PGD_PORT_RX_CFGn           0x040
#define SB_PGD_PORT_TX_CFGn           0x050
#define SB_PGD_TX_PORTn_MULTI_CHNL_0  0x100
#define SB_PGD_TX_PORTn_MULTI_CHNL_1  0x101
#define SB_PGD_RX_PORTn_MULTI_CHNL_0  0x180
#define SB_PGD_RX_PORTn_MULTI_CHNL_1  0x181

static const wcd_register_info_v2 wcd_pgd_port_v1_register_table[] = {
//                                   reg_addr,                               type, bmask, width, offset
  { SB_PGD_PORT_TX_CFGn           + SB_OFFSET,   E_WCD_SB_PGD_PORT_TX_WATERMARK_N,  0x1E,  0x08, 0x01},
  { SB_PGD_PORT_TX_CFGn           + SB_OFFSET,      E_WCD_SB_PGD_PORT_TX_ENABLE_N,  0x01,  0x08, 0x01},
  { SB_PGD_PORT_RX_CFGn           + SB_OFFSET,   E_WCD_SB_PGD_PORT_RX_WATERMARK_N,  0x1E,  0x08, 0x01},
  { SB_PGD_PORT_RX_CFGn           + SB_OFFSET,      E_WCD_SB_PGD_PORT_RX_ENABLE_N,  0x01,  0x08, 0x01},
  { SB_PGD_TX_PORTn_MULTI_CHNL_0  + SB_OFFSET, E_WCD_SB_PGD_TX_PORTn_MULTI_CHNL_0,  0xFF,  0x08, 0x04},
  { SB_PGD_TX_PORTn_MULTI_CHNL_1  + SB_OFFSET, E_WCD_SB_PGD_TX_PORTn_MULTI_CHNL_1,  0xFF,  0x08, 0x04},
  { SB_PGD_RX_PORTn_MULTI_CHNL_0  + SB_OFFSET, E_WCD_SB_PGD_RX_PORTn_MULTI_CHNL_0,  0xFF,  0x08, 0x04},
  { SB_PGD_RX_PORTn_MULTI_CHNL_1  + SB_OFFSET, E_WCD_SB_PGD_RX_PORTn_MULTI_CHNL_1,  0xFF,  0x08, 0x04},
};


//TODO verify if we need to return intf EA here (0x021700A00000);
static const wcd_register_block_definition WCD_PGD_PORT_V1_BLOCK = {
  2,             //version;
  sizeof(wcd_pgd_port_v1_register_table)/sizeof(wcd_register_info_v2), //num_elements_in_table;
  wcd_pgd_port_v1_register_table,  //register_table[n]
  1              // HW version of the pgd port block. 
};


const wcd_register_block_definition* tomtom_v1_register_blocks[] =
{
  &WCD_AANC_V2_BLOCK,
  &WCD_MAD_V1_BLOCK,
  &WCD_VBAT_V1_BLOCK,
  &WCD_CLIP_V2_BLOCK,
  &WCD_PGD_PORT_V1_BLOCK
};

const wcd_register_block_definition* tomtom_v2_register_blocks[] =
{
  &WCD_AANC_V2_BLOCK,
  &WCD_MAD_V1_BLOCK,
  &WCD_VBAT_V1_BLOCK,
  &WCD_CLIP_V2_BLOCK,
  &WCD_PGD_PORT_V1_BLOCK
};



/* ********************************************* */
/*               CODEC detection                 */
/* ********************************************* */
#define WCD_INVALID  0xFFFFFFFF

wcd_codec_info* g_codec_info_p       = NULL;
wcd_codec_info  g_codec_info         = {(wcd_codec)WCD_INVALID, (wcd_codec_version)WCD_INVALID};
wcd_codec_info  g_codec_info_default = {(wcd_codec)WCD_INVALID, (wcd_codec_version)WCD_INVALID};


void wcd_register_block_get_codec_info()
{
  if(!g_codec_info_p)
  {
    wcd_result rc = E_WCD_SUCCESS;
    wcd_ioctl_cmd_codec_info param;    
    rc = wcd_ioctl( NULL, WCD_IOCTL_GET_CODEC_INFO, &param );

    if( rc == E_WCD_SUCCESS)
    {
      g_codec_info.codec    = param.codec;
      g_codec_info.version  = param.version;
      g_codec_info_p        = &g_codec_info;
    }
    else
    {
      if( g_codec_info_default.codec != WCD_INVALID )
      {
        g_codec_info_p      = &g_codec_info_default;
      }
    }
	
	if (g_codec_info_p)
	{
		WCD_DEBUG_INFO_2("=WCD_93XX: <wcd_register_block_get_codec_info, at the end, codec=(%d), version=(%d)> \n", g_codec_info_p->codec, g_codec_info_p->version);
	}
  }
}

/* ********************************************* */
/*             Exported functions                */
/* ********************************************* */
const wcd_register_block_definition* wcd_exposed_register_block( wcd_exposed_blocks block )
{
  const wcd_register_block_definition* rc = NULL;  
  
  if( (int32) block < E_WCD_AANC || (int32) block > E_WCD_PGD_PORT )
  {
    return NULL;
  }
  
  wcd_register_block_get_codec_info();
  
  if( g_codec_info_p )
  {
    WCD_DEBUG_INFO_2("=WCD_93XX: <wcd_exposed_register_block, codec=(%d), version=(%d)> \n", g_codec_info_p->codec, g_codec_info_p->version);

    //TODO: remove the non supported from here
    switch( g_codec_info_p->codec )
    {
      case E_WCD_WCD9330:
        {
          switch( g_codec_info_p->version )
          {
            case E_WCD_VER_1P0:
              {
                rc = tomtom_v1_register_blocks[block];
              }
              break;
            case E_WCD_VER_2P0:
              {
                rc = tomtom_v2_register_blocks[block];
              }
              break;
            default:
			  rc = NULL;
          }
        }
        break;
            default:
            rc = NULL;
    }
  }

  return rc;
}

const wcd_codec_enumeration_address* wcd_get_codec_enumeration_address( )
{
  const wcd_codec_enumeration_address* rc = NULL;  
  wcd_register_block_get_codec_info();
  
  //add different EAs if necesary
  rc = &tomtom_v2_EA;
  return rc;
}



wcd_result wcd_set_default_codec( wcd_codec_info codec_info )
{
  g_codec_info_default.codec    = codec_info.codec;
  g_codec_info_default.version  = codec_info.version;
  return E_WCD_SUCCESS;
}


