#ifndef __AUDIO_RESOURCE_MANAGER_H__
#define __AUDIO_RESOURCE_MANAGER_H__
/*============================================================================

	FILE:        audio_resource_manager.h

	DESCRIPTION: 
	
    Copyright © 2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.

==========================================================================*/

/*================================================================================
	EDIT HISTORY FOR MODULE

	$Header: //components/rel/audio.tx/2.0/audio/adie/audio_codec_resource_manager/src/audio_resource_manager.h#2 $
	$Author: pwbldsvc $

 when			who		what, where, why
-----------		---		------------------------------------------------------
03/07/13		SSP		Driver Unification Update  
08/16/12		SP		Audio Resource Manager Feature Parity

==================================================================================*/

#include "wcd_types.h"
#ifdef WCD_MBHC_ENABLE
  #include "mbhc_codec.h"
#endif

#define ADDR_CDC_CTL					( 0x80 )
	#define MASK_CDC_ANA_RST_N						( 0x01 )	// bit 0
	#define MASK_CDC_DIG_RST_N						( 0x02 )	// bit 1
#define ADDR_LEAKAGE_CTL				( 0x88  )
	#define MASK_GDFS_EN_FEW						( 0x01 )	// bit 0
	#define ENABLE_GDFS_EN_FEW						( 0x01 )	// bit 0
	#define MASK_GDFS_EN_REST						( 0x02 )	// bit 1
	#define ENABLE_GDFS_EN_REST						( 0x02 )	// bit 1
	#define MASK_GDFS_CLAMP_EN						( 0x04 )	// bit 2
	#define DISABLE_GDFS_CLAMP_EN					( 0x0  )	// bit 2
#define ADDR_BIAS_CURR_CTL_2			( 0x104 )
#define ADDR_BIAS_OSC_BG_CTL			( 0x105 )
	#define MASK_BIAS_OSC_BG_CTL_BG_EN				( 0x01)		// bit 0
	#define ENABLE_BIAS_OSC_BG_CTL_BG_EN			( 0x01)		// bit 0
	#define DISABLE_BIAS_OSC_BG_CTL_BG_EN			( 0x0 )		// bit 0
#define ADDR_CLK_BUFF_EN1				( 0x108 )
	#define MASK_CLK_BUFF_EN						( 0x01 )	// bit 0
	#define ENABLE_CLK_BUFF_EN						( 0x01 )	// bit 0
	#define DISABLE_CLK_BUFF_EN						( 0x0  )	// bit 0
	#define MASK_PWR_MODE_SEL						( 0x02 )	// bit 1
	#define MASK_CLK_BUFF_REF_SEL					( 0x04 )	// bit 2
	#define MASK_CLK_SOURCE_SEL						( 0x08 )	// bit 3
	#define ENABLE_CLK_SOURCE_SEL_RC				( 0x08 )	// bit 3
	#define ENABLE_CLK_SOURCE_EXT_CLK				( 0x0  )	// bit 3
	#define MASK_CLK_BUFF_SEL_SRC					( 0x09 )	// bit 3 and 0
	#define ENABLE_CLK_BUFF_EN_EXT_CLK				( 0x01 )	// bit 0
	#define DISABLE_CLK_BUFF_EN_EXT_CLK				( 0x09 )	// bit 0
	#define ENABLE_CLK_BUFF_EN_RC_OSC				( 0x09 )	// bit 0
	#define DISABLE_CLK_BUFF_EN_RC_OSC				( 0x0  )	// bit 0
	#define MASK_CLK_DIV_SEL						( 0x10 )	// bit 4
	#define MASK_CLK_POLARITY_SEL					( 0x20 )	// bit 5
#define ADDR_CLK_BUFF_EN2				( 0x109 )
	#define MASK_CLK_BUFF_RESET						( 0x01 )	// bit 0
	#define MASK_ASYNC_RESET_BLANK_CKT				( 0x02 )	// bit 1
	#define ENABLE_RESET_BLANK_CKT					( 0x02 )	// bit 1
	#define DISABLE_RESET_BLANK_CKT					( 0x0  )	// bit 1
	#define MASK_CLK_BUFF_EN2_MCLK_EN				( 0x04 )	// bit 2
	#define ENABLE_CLK_BUFF_EN2_MCLK_EN				( 0x04 )	// bit 2
	#define DISABLE_CLK_BUFF_EN2_MCLK_EN			( 0x0  )	// bit 2
// TAIKO
#define ADDR_TX_COM_BIAS				( 0x14C )
	#define MASK_AAF_FILT_MODE						( 0x10 )	// bit 4
	#define MASK_AAF_FILT_MODE_9P6					( 0x10 )	// bit 4
	#define MASK_AAF_FILT_MODE_12P288				( 0x0  )	// bit 4
// TAIKO
#define ADDR_RX_COM_TIMER_DIV			( 0x19E )
	#define MASK_RX_COM_TIMER_DIV_MCLK_FSEL			( 0x01 )	// bit 0
	#define ENABLE_F_9P6MHZ							( 0x0  )	// bit 0
	#define ENABLE_F_12P288MHZ						( 0x01 )	// bit 0
// TAIKO
#define ADDR_RC_OSC_FREQ				( 0x1FA )
	#define MASK_REG_OSC_FREQ						( 0x0F )	// bit 3:0
	#define MASK_FREQ_CTL_SEL						( 0x10 )	// bit 4
	#define MASK_RC_TUNER_START						( 0x20 )	// bit 5
	#define MASK_RC_TUNER_RELEASE					( 0x40 )	// bit 6
	#define MASK_RC_OSC_EN							( 0x80 )	// bit 7
	#define ENABLE_RC_OSC_EN						( 0x80 )	// bit 7
	#define DISABLE_RC_OSC_EN						( 0x0  )	// bit 7
// TAIKO
#define ADDR_RC_OSC_TEST				( 0x1FB )
	#define MASK_SCHT_EN							( 0x80 )	// bit 7
	#define ENABLE_SCHT_EN							( 0x80 )	// bit 7
	#define DISABLE_SCHT_EN							( 0x0  )	// bit 7
// TAIKO
#define ADDR_CDC_CLK_RX_I2S_CTL			( 0x306 )
#define ADDR_CDC_CLK_TX_I2S_CTL			( 0x307 )
// TAIKO
#define ADDR_CDC_CLK_MCLK_CTL			( 0x311 )
	#define MASK_CDC_CLK_MCLK_CTL_MCLK_EN			( 0x01 )	// bit 0
	#define ENABLE_CDC_CLK_MCLK_CTL_MCLK_EN			( 0x01 )	// bit 0
	#define DISABLE_CDC_CLK_MCLK_CTL_MCLK_EN		( 0x0  )	// bit 0

#ifndef WCD_MBHC_ENABLE

#define ADDR_BIAS_CENTRAL_BG_CTL		( 0x101 )
	#define MASK_BG_EN								( 0x01 )	// bit 0
	#define ENABLE_BG_EN							( 0x01 )	// bit 0
	#define DISABLE_BG_EN							( 0x0  )	// bit 0
	#define MASK_BG_MODE							( 0x02 )	// bit 1
	#define MASK_ENABLE_MIC_BIAS_BG_MODE			( 0x03 )	// bit 1:0
	#define ENABLE_MIC_BIAS_BG_FAST_MODE			( 0x02 )	// bit 1:0
	#define ENABLE_MIC_BIAS_BG_SLOW_MODE			( 0x01 )	// bit 1:0
	#define MASK_DC_START_UP_EN						( 0x04 )	// bit 2
	#define ENABLE_DC_START_UP_EN					( 0x04 )	// bit 2
	#define DISABLE_DC_START_UP_EN					( 0x00 )	// bit 2
	#define MASK_TRAN_START_UP_EN					( 0x08 )	// bit 3
	#define MASK_OTA_BIAS_CTL						( 0x10 )	// bit 4
	#define MASK_ATEST_CTL							( 0x20 )	// bit 5
	#define MASK_BYPASS_COUNTER_LOGIC				( 0x40 )	// bit 6
	#define MASK_REGISTER_PRECHARGE					( 0x80 )	// bit 7
	#define ENABLE_REGISTER_PRECHARGE				( 0x80 )	// bit 7
	#define DISABLE_REGISTER_PRECHARGE				( 0x0  )	// bit 7
	#define MASK_ALL_BIAS_CENTRAL_BG_CTL			( 0xFF )	// bit 7:0
#define ADDR_MICB_CFILT_2_CTL			( 0x12E )
	#define MASK_MICBIAS2_OFF						( 0x80 )	// bit 7
	#define MASK_CFILT_2_EN							( 0xC0 )	// bit 7:6
	#define ENABLE_CFILT2_CTL						( 0x80 )
	#define DISABLE_CFILT2_CTL						( 0x0  )
	#define MASK_MICBIAS_CFILT_2_OFF				( 0xF0 )	// bit 7:4
	#define MASK_INT_EXT_PRECHRG					( 0x30 )	// bit 5:4
	#define MASK_CFILT_MODE_CAP_EN					( 0x40 )	// bit 6
	#define ENABLE_CFILT_MODE_CAP_EN				( 0x40 )	// bit 6
	#define DISABLE_CFILT_MODE_CAP_EN				( 0x0  )	// bit 6
#define ADDR_MICB_2_CTL					( 0x131 )
	#define MASK_PULLDOWN_EN						( 0x01 )	// bit 0
	#define MASK_OPA_STG3_TAIL_CURR					( 0x0E )	// bit 3:1
	#define MASK_CAP_MODE							( 0x10 )	// bit 4
	#define MASK_CFILT_SEL							( 0x60 )	// bit 6:5
	#define ENABLE_CFILT_2							( 0x20 )    // bit 6:5
	#define MASK_MICB_OPA_EN						( 0x80 )	// bit 7
	#define MASK_MICB_ALL							( 0xFF )	// bit 7:1
	#define ENABLE_MICB_CTL							( 0xBA )
	#define DISABLE_MICB_CTL						( 0x3A )
#define ADDR_MBHC_SCALING_MUX_1			( 0x14E )
	#define MASK_VMIC_GND_PATH_EN					( 0x01 )	// bit 0
	#define MASK_MIC_BIAS_PATH_EN					( 0x02 )	// bit 1
	#define MASK_VMIC_PATH_EN						( 0x04 )	// bit 2
	#define MASK_TX_IN_PATH_EN						( 0x08 )	// bit 3
	#define MASK_HPH_GND_PATH_EN					( 0x10 )	// bit 4
	#define MASK_HPH_R_PATH_EN						( 0x20 )	// bit 5
	#define MASK_HPH_L_PATH_EN						( 0x40 )	// bit 6
	#define MASK_SCALING_MUX_BIAS_BLOCK_EN			( 0x80 )	// bit 7
	#define ENABLE_SCALING_MUX_BIAS_BLOCK_EN		( 0x80 )	// bit 7
	#define DISABLE_SCALING_MUX_BIAS_BLOCK_EN		( 0x0  )	// bit 7
	#define MASK_ALL_MBHC_SCALING_MUX_1				( 0xFF )
	#define MASK_ALL_BUT_EN_MBHC_SCALING_MUX_1		( 0x7F )
#define RX_HPH_CNP_EN					( 0x1AB )
	#define MASK_RIGHT_RX_HPH_CNP_EN				( 0x10 )	// bit 4
	#define MASK_LEFT_RX_HPH_CNP_EN					( 0x20 )	// bit 5
	#define MASK_LR_RX_HPH_CNP_EN					( 0x30 )
	#define ENABLE_LR_RX_HPH_CNP					( 0x30 )
	#define DISABLE_LR_RX_HPH_CNP					( 0x0  )
	#define MASK_FSM_RESET							( 0x40 )	// bit 6
	#define MASK_FSM_CLK_EN							( 0x80 )	// bit 7
#define ADDR_CDC_CLK_OTHER_CTL			( 0x30C )
	#define MASK_CP_CLK_EN							( 0x01 )	// bit 0 
	#define MASK_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN	( 0x04 )	// bit 2
	#define ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN	( 0x04 )	// bit 2
	#define DISABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN	( 0x0  )	// bit 2
    #define MASK_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN    ( 0x20 )    // bit 5
    #define ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN  ( 0x20 )    // bit 5
    #define DISABLE_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN ( 0x0  )    // bit 5

#endif

/* Audio Resource Manager Commands */
#define RM_CMD_MIN						( 0x00000000 )
#define RM_CMD_INIT						( 0x00000001 )
#define RM_CMD_MANAGE_RESOURCE			( 0x00000002 )
#define RM_CMD_STATUS_NOTIFICATION		( 0x00000004 )
#define RM_CMD_SET_CB					( 0x00000008 )
#define RM_CMD_DEINIT					( 0x00000010 )
#define RM_CMD_MBHC_PLUG_STATUS			( 0x00000020 )
#define RM_CMD_MAX						( 0x00000040 )

// TAIKO
#define MBHC_MCLK_12288_n_ready			96
#define MBHC_MCLK_9600_n_ready			96
#define	MBHC_MCLK_12288_n_poll			3 
#define MBHC_MCLK_9600_n_poll			3
#define MBHC_MCLK_12288_bc_wait			30
#define MBHC_MCLK_9600_bc_wait			30
#define MBHC_MCLK_12288_n_cic			60
#define MBHC_MCLK_9600_n_cic			47
#define MBHC_MCLK_12288_gain			11
#define MBHC_MCLK_9600_gain				14

typedef enum
{
	RM_MBHC_PLUG_NONE = -1,
	RM_MBHC_HEADPHONE,
	RM_MBHC_HEADSET,
	RM_MBHC_SPECIAL_HEADSET 
} rm_mbhc_plug_type;

typedef enum
{
	RM_MBHC_BLOCK_NONE = 0,
	RM_MBHC_BLOCK_HP_START,
	RM_MBHC_BLOCK_HS_START,
	RM_MBHC_BLOCK_HP_COMPLETE,
	RM_MBHC_BLOCK_HS_COMPLETE
} rm_mbhc_block_state;

typedef enum
{
	RM_CLIENT_MIN = 0,
	RM_MBHC_MODULE,
	RM_WCD_MODULE,
	RM_CLIENT_MAX
} rm_audio_resource_manager_client;

typedef enum
{
	SHARED_RESOURCE_NONE = 0,
	SHARED_RESOURCE_MASTER_CLOCK,
	SHARED_RESOURCE_CLOCK_BLOCK,
	SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC,
	SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK,
	SHARED_RESOURCE_CLOCK_BLOCK_NONE,
	SHARED_RESOURCE_CENTRAL_BANDGAP,
	SHARED_RESOURCE_LDOH,
	SHARED_RESOURCE_MICB_CFILT2_CTL,
	SHARED_RESOURCE_MICB2_CTL,
	SHARED_RESOURCE_HEADSET_PA_CTL,
	SHARED_RESOURCE_MAX
} rm_audio_shared_resource;

typedef enum
{
	RESOURCE_DISABLED = 0,
	RESOURCE_ENABLED
} rm_audio_shared_resource_status;

typedef enum
{
	RM_DISABLE = 0,
	RM_ENABLE
} rm_audio_shared_resource_ctl;

typedef enum
{
	CFILT_FAST = 0,
	CFILT_SLOW
} rm_audio_shared_resource_bg_mode;

typedef enum
{
	AUDIO_STATUS_PLAYBACK_ENABLE_START = 0,
	AUDIO_STATUS_PLAYBACK_ENABLE_COMPLETE,
	AUDIO_STATUS_PLAYBACK_DISABLE_START,
	AUDIO_STATUS_PLAYBACK_DISABLE_COMPLETE,
	AUDIO_STATUS_RECORDING_ENABLE_START,
	AUDIO_STATUS_RECORDING_ENABLE_COMPLETE,
	AUDIO_STATUS_RECORDING_DISABLE_START,
	AUDIO_STATUS_RECORDING_DISABLE_COMPLETE
} rm_audio_status;

#define AUDIO_MIN_SHARED_RESOURCE			SHARED_RESOURCE_NONE
#define AUDIO_MAX_SHARED_RESOURCE			SHARED_RESOURCE_MAX
#define AUDIO_SHARED_RESOURCE_COUNT 		AUDIO_MAX_SHARED_RESOURCE
#define AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE	3

//uint8_t	rm_shared_resource_count[AUDIO_SHARED_RESOURCE_COUNT] = {0};

struct rm_shared_resource_list
{
	uint8		rm_shared_resource_register_status[AUDIO_SHARED_RESOURCE_COUNT][AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE];
	uint8		rm_shared_resource_active_client[AUDIO_SHARED_RESOURCE_COUNT][AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE];
	uint16		rm_shared_resource_registers[AUDIO_SHARED_RESOURCE_COUNT][AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE];
	uint8		rm_shared_resource_masks[AUDIO_SHARED_RESOURCE_COUNT][AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE];
	uint8		rm_shared_resource_enable_values[AUDIO_SHARED_RESOURCE_COUNT][AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE];
	uint8		rm_shared_resource_disable_values[AUDIO_SHARED_RESOURCE_COUNT][AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE];
};

#define AUDIO_CBG_DISABLED_FAST_MODE		RM_MBHC_MODULE
#define AUDIO_CBG_ENABLED_SLOW_MODE			RM_WCD_MODULE
#define AUDIO_CFILT_FAST_MODE				RM_MBHC_MODULE
#define AUDIO_CFILT_SLOW_MODE				RM_WCD_MODULE

/* Callback function proto type */
typedef uint32 (*rm_cb_fn)(uint32 evt_id, void* payload, uint32 payload_size, void* client_data);

/* This structure defines the payload */
struct rm_cb
{
   rm_cb_fn	cb;
   void*	cb_data;
};

typedef struct 
{
   rm_audio_shared_resource                 resource_id;
   rm_audio_shared_resource_ctl             action;
   uint16                                   default_headset_voltage;
   uint8                                    ext_byp_cap_mode; 
} rm_params;

struct rm_status_notification
{
   rm_audio_status	status;
};

struct rm_mbhc_plug_status
{
   rm_mbhc_plug_type	plug_type;
};

wcd_result rm_audio_sr_init(void);
wcd_result rm_audio_sr_deinit(void);
wcd_result rm_audio_shared_resources_initialize(void);
wcd_result rm_audio_sr_ioctl(rm_audio_resource_manager_client client, uint32 cmd, void* params);
wcd_result rm_clock_block_control(rm_audio_resource_manager_client client, rm_audio_shared_resource shared_res, wcd_bool enable);
wcd_result rm_codec_set_cfilt_mode(uint8 bg_mode);

#ifdef WCD_MBHC_ENABLE
wcd_result rm_mbhc_block_control(wcd_bool enable);
wcd_result rm_mbhc_enable_headset_micbias(wcd_bool enable, uint16 hs_micbias_voltage);
wcd_result rm_mbhc_enable_vddio_switch(wcd_bool enable);
wcd_result rm_mbhc_set_VDDIO(wcd_bool enable);
wcd_result rm_mbhc_pause_polling(wcd_bool enable);
wcd_result rm_mbhc_setup_intr(wcd_bool enable);
// TAIKO
wcd_result rm_mbhc_setup_clk_freq_dependent_params(wcd_bool enable);
#endif

#endif /* __AUDIO_RESOURCE_MANAGER__ */
