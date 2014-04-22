/*============================================================================

	FILE:        audio_resource_manager.c

	DESCRIPTION: 
	
    Copyright © 2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.

==========================================================================*/

/*================================================================================
	EDIT HISTORY FOR MODULE

	$Header: //components/rel/audio.tx/2.0/audio/adie/audio_codec_resource_manager/src/audio_resource_manager.c#2 $
	$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/21/14   AG      Fixed SAL warnings
09/12/13   KNM     Fix for stereo EU headset gets detected as mono headset.
                   Resolved issue with Vol+, Vol- & Middle button press detection
                   during audio playback.
08/07/13   KNM     Fixed compiler warnings.
03/07/13   SSP     Driver Unification Update 
08/16/12    SP     Audio Resource Manager Feature Parity
==================================================================================*/

#include "audio_resource_manager.h"
#include "init_manager.h"
#include "target_defs.h"
#include "wcd_osal.h"
#include "bus_manager.h"
#include "wcd_custom_functions.h"
#ifdef WCD_MBHC_ENABLE
	#include "mbhc_state_target_specific.h"
#endif
#ifdef SPL_LOG_SUPPORT
	#include "audio_resource_manager.tmh"
#endif

static uint8 audio_status, playback_usecase_count=0, recording_usecase_count=0;
rm_audio_status use_case_status;
struct rm_cb mbhc_rm_cb;
struct rm_shared_resource_list rm_sr_list;
static rm_audio_shared_resource current_shared_resource=SHARED_RESOURCE_NONE;
static uint8 RM_INITIALIZED = 0;
static uint8 MBHC_RM_INITIALIZED = 0;
static rm_mbhc_plug_type RM_MBHC_PLUG = RM_MBHC_PLUG_NONE;
static wcd_bool RM_WCD9320_V1P0 = FALSE;
static wcd_codec_info rm_codec_info;
static uint16 rm_default_headset_voltage = 1800;
extern uint8 HPH_PA_State;
static uint8 rm_headset_ext_cap_mode = 0;
#ifdef WCD_MBHC_ENABLE
static wcd_bool rm_vddio_state = FALSE;
#endif
extern void *ACRMLock;


wcd_result rm_audio_sr_init(void)
{
	wcd_result rc = E_WCD_SUCCESS;
	rc = rm_audio_shared_resources_initialize();
	WCD_DEBUG_INFO_1("=WCD_RM: <rm_audio_sr_init successful, rc=(0x%x)> \n", rc);
	return rc;
}

wcd_result rm_audio_sr_deinit(void)
{
	wcd_result rc = E_WCD_SUCCESS;
	return rc;
}

wcd_result rm_audio_shared_resources_initialize(void)
{
	wcd_result rc = E_WCD_SUCCESS;
	// Initialize Audio Shared Resource with values of shared registers, masks, enable_values, disable_values
	uint8 i, j;

	WCD_DEBUG_INFO_0("+WCD_RM: <rm_audio_shared_resources_initialize called> \n");
	for (i=0; i<AUDIO_SHARED_RESOURCE_COUNT; i++)
	{
		for (j=0; j<AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE; j++)
		{
			rm_sr_list.rm_shared_resource_registers[i][j] = 0x0;
			rm_sr_list.rm_shared_resource_register_status[i][j] = RESOURCE_DISABLED;
			rm_sr_list.rm_shared_resource_active_client[i][j] = 0;
		}
	}
	
	// Master Clock
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_MASTER_CLOCK][0] = ADDR_CDC_CLK_MCLK_CTL;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_MASTER_CLOCK][0] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_MASTER_CLOCK][0] = MASK_CDC_CLK_MCLK_CTL_MCLK_EN;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_MASTER_CLOCK][0] = ENABLE_CDC_CLK_MCLK_CTL_MCLK_EN;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_MASTER_CLOCK][0] = DISABLE_CDC_CLK_MCLK_CTL_MCLK_EN;

	// Clock Block RC Osc
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] = ADDR_RC_OSC_FREQ;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] = MASK_RC_OSC_EN;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] = ENABLE_RC_OSC_EN;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] = DISABLE_RC_OSC_EN;

	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][1] = ADDR_CLK_BUFF_EN1;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][1] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][1] = MASK_CLK_BUFF_SEL_SRC;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][1] = ENABLE_CLK_BUFF_EN_RC_OSC;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][1] = DISABLE_CLK_BUFF_EN_RC_OSC;

	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][2] = ADDR_CLK_BUFF_EN2;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][2] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][2] = MASK_CLK_BUFF_EN2_MCLK_EN;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][2] = ENABLE_CLK_BUFF_EN2_MCLK_EN;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][2] = DISABLE_CLK_BUFF_EN2_MCLK_EN;

	// Clock Block External Clock
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] = ADDR_CLK_BUFF_EN1;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] = MASK_CLK_BUFF_SEL_SRC;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] = ENABLE_CLK_BUFF_EN_EXT_CLK;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] = DISABLE_CLK_BUFF_EN_EXT_CLK;

	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][1] = ADDR_CLK_BUFF_EN2;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][1] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][1] = MASK_CLK_BUFF_EN2_MCLK_EN;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][1] = ENABLE_CLK_BUFF_EN2_MCLK_EN;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][1] = DISABLE_CLK_BUFF_EN2_MCLK_EN;

	// Central Bandgap
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_CENTRAL_BANDGAP][0] = ADDR_BIAS_CENTRAL_BG_CTL;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_CENTRAL_BANDGAP][0] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_CENTRAL_BANDGAP][0] = MASK_ENABLE_MIC_BIAS_BG_MODE;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_CENTRAL_BANDGAP][0] = ENABLE_MIC_BIAS_BG_SLOW_MODE;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_CENTRAL_BANDGAP][0] = ENABLE_MIC_BIAS_BG_FAST_MODE;

	// LDOH
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_LDOH][0] = 0x0;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_LDOH][0] = RESOURCE_DISABLED;
	
	// CFILT2
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_MICB_CFILT2_CTL][0] = ADDR_MICB_CFILT_2_CTL;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_MICB_CFILT2_CTL][0] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_MICB_CFILT2_CTL][0] = MASK_CFILT_2_EN;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_MICB_CFILT2_CTL][0] = ENABLE_CFILT2_CTL;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_MICB_CFILT2_CTL][0] = DISABLE_CFILT2_CTL;

	// MICB2
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_MICB2_CTL][0] = ADDR_MICB_2_CTL;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_MICB2_CTL][0] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_MICB2_CTL][0] = MASK_MICB_ALL;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_MICB2_CTL][0] = ENABLE_MICB_CTL;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_MICB2_CTL][0] = DISABLE_MICB_CTL;
	
	// Headset PA
	rm_sr_list.rm_shared_resource_registers[SHARED_RESOURCE_HEADSET_PA_CTL][0] = RX_HPH_CNP_EN;
	rm_sr_list.rm_shared_resource_register_status[SHARED_RESOURCE_HEADSET_PA_CTL][0] = RESOURCE_DISABLED;
	rm_sr_list.rm_shared_resource_masks[SHARED_RESOURCE_HEADSET_PA_CTL][0] = MASK_LR_RX_HPH_CNP_EN;
	rm_sr_list.rm_shared_resource_enable_values[SHARED_RESOURCE_HEADSET_PA_CTL][0] = ENABLE_LR_RX_HPH_CNP;
	rm_sr_list.rm_shared_resource_disable_values[SHARED_RESOURCE_HEADSET_PA_CTL][0] = DISABLE_LR_RX_HPH_CNP;
	
	// GET THE CODEC CHIP ID AND VERSION
	rc = wcd_get_codec_info(&rm_codec_info);
	if (rc == E_WCD_SUCCESS)
	{
		if ( (rm_codec_info.codec == E_WCD_WCD9320) && (rm_codec_info.version == E_WCD_VER_1P0) )
		{
			RM_WCD9320_V1P0 = TRUE;
		}
		else
		{
			RM_WCD9320_V1P0 = FALSE;
		}
		RM_INITIALIZED = 1;
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_shared_resources_initialize, Codec_id=(0x%x), Version=(0x%x), RM_WCD9320_V1P0=(0x%x)> \n", rm_codec_info.codec, rm_codec_info.version, RM_WCD9320_V1P0);
		WCD_DEBUG_INFO_0("-WCD_RM: <rm_audio_shared_resources_initialize successful> \n");
	}
	else
	{
		RM_INITIALIZED = 0;
		WCD_DEBUG_ERROR_0("=WCD_RM: <rm_audio_shared_resources_initialize, Could not get chip info> \n");
		WCD_DEBUG_ERROR_0("-WCD_RM: <rm_audio_shared_resources_initialize failure> \n");
	}

	return rc;
}

//uint32_t rm_audio_sr_ioctl(audio_resource_manager_client rm_client, audio_shared_resource shared_resource, bool_t enable)
wcd_result rm_audio_sr_ioctl(rm_audio_resource_manager_client client, uint32 cmd, void* params)
{
	wcd_result rc = E_WCD_SUCCESS;
	uint16 	reg;
	uint8 	mask, value, i;
	rm_audio_shared_resource new_shared_resource = SHARED_RESOURCE_NONE, resource = SHARED_RESOURCE_NONE;
	rm_audio_shared_resource_ctl enable = RM_DISABLE;
	rm_mbhc_plug_type RM_MBHC_PLUG_NEW = RM_MBHC_PLUG_NONE;
//#ifdef WCD_MBHC_ENABLE
	int mutexStatus = E_WCD_GENERIC_ERROR;
	uint32 mutex_timeout = WCD_INFINITE;
//#endif

	if( cmd <= RM_CMD_MIN || cmd >= RM_CMD_MAX ) {
		return E_WCD_GENERIC_WARNING;
	}

	WCD_DEBUG_INFO_2("+WCD_RM: <rm_audio_sr_ioctl called, client=(0x%x), cmd=(%u)> \n", client, (unsigned int)cmd);

// ACQUIRE A MUTEX
//#ifdef WCD_MBHC_ENABLE
	if ( ACRMLock != NULL )
	{
		mutexStatus = WCD_WAIT_FOR_MUTEX((void*)ACRMLock, mutex_timeout);
		WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl, After WCD_WAIT_FOR_MUTEX in Audio_Resource_Manager> \n");
		if(mutexStatus == WCD_WAIT_DONE)
		{
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl, Acquired the mutex in Audio_Resource_Manager> \n");
		}
		else
		{
			WCD_DEBUG_ERROR_0("=WCD_RM: <rm_audio_sr_ioctl, Failed to acquire the mutex in Audio_Resource_Manager> \n");
		}
	}
//#endif

	switch(cmd) 
	{
	 case RM_CMD_INIT:
	 {
		if ( (params != NULL) && (client == RM_MBHC_MODULE) )
		{
			rm_default_headset_voltage = ((rm_params*)params)->default_headset_voltage;
			rm_headset_ext_cap_mode    = ((rm_params*)params)->ext_byp_cap_mode;
			WCD_DEBUG_INFO_2("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_INIT, default_headset_voltage=(%u), headset_ext_cap_mode=(%u) > \n", rm_default_headset_voltage, rm_headset_ext_cap_mode);
		}
		WCD_DEBUG_INFO_1("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_INIT, client=%d> \n", client);

		if (client == RM_WCD_MODULE)
		{
			if (RM_INITIALIZED == 0)
			{							
				rc = rm_audio_sr_init();
				if (rc == E_WCD_SUCCESS) {
					WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_INIT successful> \n");
				}
				else
				{
					WCD_DEBUG_ERROR_1("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_INIT failed with status=(0x%x)> \n", rc);
					break;
				}
			}
			else
			{
				WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_INIT, RM Already Initialized> \n");
			}
		}
		else if (client == RM_MBHC_MODULE)
		{
			if (RM_INITIALIZED == 1)
			{
				MBHC_RM_INITIALIZED = 1;
			}
			else
			{
				rc = E_WCD_GENERIC_WARNING;
			}
		}

		WCD_DEBUG_INFO_2("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_INIT, RM_INITIALIZED=%u, MBHC_RM_INITIALIZED=%u> \n", RM_INITIALIZED, MBHC_RM_INITIALIZED);
		break;
	 }

	 case RM_CMD_DEINIT:
	 {
		WCD_DEBUG_INFO_1("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_DEINIT, client=%d> \n", client);
		if (client == RM_WCD_MODULE)
		{
			if (RM_INITIALIZED == 1)
			{
				rc = rm_audio_sr_deinit();
				if (rc != E_WCD_SUCCESS)
				{
					WCD_DEBUG_ERROR_1("=WCD_RM: <rm_audio_sr_ioctl-rm_audio_sr_deinit fail with status=(0x%x)> \n", rc);
				}
				RM_INITIALIZED = 0;
			}
		}
		else if (client == RM_MBHC_MODULE)
		{
			if (MBHC_RM_INITIALIZED == 1)
			{
				MBHC_RM_INITIALIZED = 0;
			}
		}
		WCD_DEBUG_INFO_2("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_DEINIT, RM_INITIALIZED=%u, MBHC_RM_INITIALIZED=%u> \n", RM_INITIALIZED, MBHC_RM_INITIALIZED);
		break;
	 }

	case RM_CMD_SET_CB:
	 {
		if (client == RM_MBHC_MODULE)
		{							
			mbhc_rm_cb.cb = ((struct rm_cb*)params)->cb;
			mbhc_rm_cb.cb_data = ((struct rm_cb*)params)->cb_data;
		}
		break;
	 }

	 case RM_CMD_STATUS_NOTIFICATION:
	 {
#if 0
		audio_status = ((struct rm_status_notification*)params)->status;
#else
		use_case_status = (rm_audio_status)(((struct rm_status_notification*)params)->status);
		audio_status = use_case_status;
		if ((use_case_status == AUDIO_STATUS_PLAYBACK_ENABLE_COMPLETE) && (HPH_PA_State == 1))
		{
			playback_usecase_count++;
		}
		else if ((use_case_status == AUDIO_STATUS_PLAYBACK_DISABLE_COMPLETE) && (HPH_PA_State == 2))
		{
			playback_usecase_count--;
			HPH_PA_State = 0;
		}
		else if (use_case_status == AUDIO_STATUS_RECORDING_ENABLE_START)
		{
			recording_usecase_count++;
		}
		else if (use_case_status == AUDIO_STATUS_RECORDING_DISABLE_START)
		{
			recording_usecase_count--;
		}

		WCD_DEBUG_INFO_2("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_STATUS_NOTIFICATION, audio_status=(0x%x), HPH_PA_State=(%u)> \n", audio_status, HPH_PA_State);
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_STATUS_NOTIFICATION, rec_count=(0x%x), playback_count=(0x%x), use_case_status=(0x%x)> \n", recording_usecase_count, playback_usecase_count, use_case_status);
		
/*		if (use_case_status==AUDIO_STATUS_RECORDING_ENABLE_START && recording_usecase_count==1)
		{
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_STATUS_NOTIFICATION, calling CFILT_SLOW> \n");
			rc = rm_codec_set_cfilt_mode(CFILT_SLOW);
		}
		else if (use_case_status==AUDIO_STATUS_RECORDING_DISABLE_START && recording_usecase_count==0)
		{
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_STATUS_NOTIFICATION, calling CFILT_FAST> \n");
			rc = rm_codec_set_cfilt_mode(CFILT_FAST);
		}
*/ 
#endif


/*	 	if ( (mbhc_rm_cb.cb != NULL) && (MBHC_RM_INITIALIZED == 1) )
		{
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_STATUS_NOTIFICATION, Calling MBHC CB> \n");
			rc = mbhc_rm_cb.cb(audio_status, NULL, 0, NULL);
		}		
*/
#ifdef WCD_MBHC_ENABLE
		if ( (MBHC_RM_INITIALIZED==1) && (RM_INITIALIZED==1) )
		{
			if ( (use_case_status==AUDIO_STATUS_PLAYBACK_ENABLE_START) || (use_case_status==AUDIO_STATUS_PLAYBACK_DISABLE_START) || (use_case_status==AUDIO_STATUS_RECORDING_ENABLE_START) || (use_case_status==AUDIO_STATUS_RECORDING_DISABLE_START) )
			{		
				rc = rm_mbhc_block_control(FALSE);
			}
			else if ( (use_case_status==AUDIO_STATUS_PLAYBACK_ENABLE_COMPLETE) || (use_case_status==AUDIO_STATUS_PLAYBACK_DISABLE_COMPLETE) || (use_case_status==AUDIO_STATUS_RECORDING_ENABLE_COMPLETE) || (use_case_status==AUDIO_STATUS_RECORDING_DISABLE_COMPLETE) )
			{
				rc = rm_mbhc_block_control(TRUE);
			}
		}
		else
		{
			WCD_DEBUG_ERROR_0("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_STATUS_NOTIFICATION, MBHC not initialized> \n");
		}
#endif
		break;
	 }
	 
	 case RM_CMD_MANAGE_RESOURCE:
	 {
 		if (RM_INITIALIZED == 0)
		{
			WCD_DEBUG_ERROR_1("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE failed, RM_INITIALIZED=%u> \n", RM_INITIALIZED);
			rc = E_WCD_GENERIC_WARNING;
			goto rm_cmd_manage_resource_end;
		}

		enable = ((rm_params*)params)->action;
		resource = ((rm_params*)params)->resource_id;
		new_shared_resource = resource;

		WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE called, enable=(0x%x), resource=(0x%x), client=(0x%x)> \n", enable, resource, client);
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE called, Ext_clk_client_no=(0x%x), RC_osc_client_no=(0x%x), No_clk_client_no=(0x%x)> \n", rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0], rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0], rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_NONE][0]);	 

		if (client <= RM_CLIENT_MIN || client >= RM_CLIENT_MAX)
		{
			rc = E_WCD_GENERIC_WARNING;
			goto rm_cmd_manage_resource_end;
		}

		if (resource<=SHARED_RESOURCE_NONE || resource>=SHARED_RESOURCE_MAX || resource==SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC || resource==SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK )
		{
			rc = E_WCD_GENERIC_WARNING;
			goto rm_cmd_manage_resource_end;
		}

		if (resource == SHARED_RESOURCE_CLOCK_BLOCK)
		{
			if (client == RM_WCD_MODULE)
			{
				if (enable)
				{
					new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK;
					if ( (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] > 0) && (current_shared_resource==new_shared_resource) ) 
					{
						rm_sr_list.rm_shared_resource_active_client[new_shared_resource][0]++;
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
				}
				else
				{
					if (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] > 1) 
					{
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK;
						rm_sr_list.rm_shared_resource_active_client[new_shared_resource][0]--;
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
					else if ( (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0]==1) && (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0]>0) )
					{
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC;
					}
					else if ( (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0]==1) && (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0]==0) )
					{
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_NONE;
					}
					else
					{
						WCD_DEBUG_ERROR_0("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE failed, External Clock is not active, hence can not be disabled> \n");
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
				}
			} 
			else if (client == RM_MBHC_MODULE)
			{
				if (enable)
				{
					if (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] > 0)
					{
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK;
						rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0]++;
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
					else  if (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] > 0)
					{
						rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0]++;
						WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE, RC Oscillator is already active> \n");
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
					else
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC;			
				}
				else
				{
					// Disable block for MBHC, Currently not needed
					if (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0] > 0)
					{
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK;
						rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0]--;
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
					else if (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] > 1)
					{
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC;
						rm_sr_list.rm_shared_resource_active_client[new_shared_resource][0]--;
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
					else if (rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0] == 1)
					{
						new_shared_resource = SHARED_RESOURCE_CLOCK_BLOCK_NONE;
					}
					else
					{
						// Invalid Request
						WCD_DEBUG_ERROR_0("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE error, RC OSC is not active, hence can not be disabled> \n");
						rc = E_WCD_SUCCESS;
						goto rm_cmd_manage_resource_end;
					}
				}
			}

			rc |= rm_clock_block_control(client, new_shared_resource, TRUE);
			
			if (enable)
			{
				rm_sr_list.rm_shared_resource_active_client[new_shared_resource][0]++;
				rm_sr_list.rm_shared_resource_register_status[new_shared_resource][0] = RESOURCE_ENABLED;
				if (current_shared_resource != 0)
				{
				//	rm_sr_list.rm_shared_resource_active_client[current_shared_resource][0]--;
					rm_sr_list.rm_shared_resource_register_status[current_shared_resource][0] = RESOURCE_DISABLED;
				}
				current_shared_resource = new_shared_resource;
			}
			else
			{
				rm_sr_list.rm_shared_resource_register_status[current_shared_resource][0] = RESOURCE_DISABLED;
				rm_sr_list.rm_shared_resource_active_client[current_shared_resource][0]--;
				if (new_shared_resource!=SHARED_RESOURCE_CLOCK_BLOCK_NONE)
					rm_sr_list.rm_shared_resource_register_status[new_shared_resource][0] = RESOURCE_ENABLED;
				current_shared_resource = new_shared_resource;
			}
		}
		else
		{
			for (i=0; i<AUDIO_NO_OF_SHARED_REG_PER_SHARED_RESOURCE; i++)
			{
				if (rm_sr_list.rm_shared_resource_registers[new_shared_resource][i] != 0x0)
				{
					reg = (uint16) rm_sr_list.rm_shared_resource_registers[new_shared_resource][i];
					mask = rm_sr_list.rm_shared_resource_masks[new_shared_resource][i];
					if (enable)
					{
						value = rm_sr_list.rm_shared_resource_enable_values[new_shared_resource][i];
						
						if (rm_sr_list.rm_shared_resource_active_client[new_shared_resource][i] == 0)
							rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, reg, mask, 0, value);
						rm_sr_list.rm_shared_resource_active_client[new_shared_resource][i]++;
						rm_sr_list.rm_shared_resource_register_status[new_shared_resource][i] = RESOURCE_ENABLED;
					}
					else
					{
						value = rm_sr_list.rm_shared_resource_disable_values[new_shared_resource][i];
						
						if (rm_sr_list.rm_shared_resource_active_client[new_shared_resource][i] == 1)
						{
							rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, reg, mask, 0, value);
							rm_sr_list.rm_shared_resource_register_status[new_shared_resource][i] = RESOURCE_DISABLED;
						}
						rm_sr_list.rm_shared_resource_active_client[new_shared_resource][i]--;
					}
				}
			}
		}

        WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE, At the end, new_shared_resource=(0x%x), active_client=(0x%x), enabled=(0x%x)> \n", new_shared_resource, rm_sr_list.rm_shared_resource_active_client[new_shared_resource][0], rm_sr_list.rm_shared_resource_register_status[new_shared_resource][0]);

rm_cmd_manage_resource_end:
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE, At the end, enable=(0x%x), resource=(0x%x), client=(0x%x)> \n", enable, resource, client);
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE, At the end, Ext_clk_client_no=(0x%x), RC_osc_client_no=(0x%x), No_clk_client_no=(0x%x)> \n", rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0], rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0], rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_NONE][0]);	 
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl - RM_CMD_MANAGE_RESOURCE, At the end, current_shared_resource=(0x%x), active_client=(0x%x), enabled=(0x%x)> \n", current_shared_resource, rm_sr_list.rm_shared_resource_active_client[current_shared_resource][0], rm_sr_list.rm_shared_resource_register_status[current_shared_resource][0]);

	    break;
	  }
	  case RM_CMD_MBHC_PLUG_STATUS:
	  {
			RM_MBHC_PLUG_NEW = ((struct rm_mbhc_plug_status*)params)->plug_type;
			WCD_DEBUG_INFO_3("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_MBHC_PLUG_STATUS, RM_MBHC_PLUG_NEW=(0x%x), RM_MBHC_PLUG=(0x%x), playback_usecase_count=(0x%x)> \n", RM_MBHC_PLUG_NEW, RM_MBHC_PLUG, playback_usecase_count);	 

#ifdef WCD_MBHC_ENABLE
			// When Headset is inserted, MBHC turns off bit 7 of 0x133
			// If playback is active, VDDIO needs to be turned ON here, ONLY for headsets
			if ( ( (RM_MBHC_PLUG_NEW == RM_MBHC_HEADSET) || (RM_MBHC_PLUG_NEW == RM_MBHC_SPECIAL_HEADSET) ) && (playback_usecase_count >= 1) )
			{
				RM_MBHC_PLUG = RM_MBHC_PLUG_NEW;
				rc |= rm_mbhc_set_VDDIO(TRUE);
				WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_MBHC_PLUG_STATUS, Enabled VDDIO> \n");
			}
			else if (RM_MBHC_PLUG_NEW == RM_MBHC_PLUG_NONE)
			{
				rc |= rm_mbhc_set_VDDIO(FALSE);
				RM_MBHC_PLUG = RM_MBHC_PLUG_NEW;
				WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_MBHC_PLUG_STATUS, Disabled VDDIO> \n");
			}
			else
			{
				RM_MBHC_PLUG = RM_MBHC_PLUG_NEW;
				WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl-RM_CMD_MBHC_PLUG_STATUS, No need to modify VDDIO state> \n");			
			}
#endif

			break;
	  }
	  default:
	  {
			WCD_DEBUG_ERROR_1("=WCD_RM: <rm_audio_sr_ioctl, Invalid command, cmd=(%u)> \n", (unsigned int)cmd);	 
			break;
	  }
	}
	
	// RELEASE THE MUTEX
//#ifdef WCD_MBHC_ENABLE
	if ( ACRMLock != NULL )
	{
		if (mutexStatus == WCD_WAIT_DONE)
		{
			mutexStatus = WCD_RELEASE_MUTEX(ACRMLock);
			if(mutexStatus == WCD_WAIT_DONE)
			{
				WCD_DEBUG_INFO_0("=WCD_RM: <rm_audio_sr_ioctl, Released the mutex in Audio_Resource_Manager> \n");
			}
			else
			{
				WCD_DEBUG_ERROR_0("=WCD_RM: <rm_audio_sr_ioctl, Failed to release the mutex in Audio_Resource_Manager> \n");
			}
		}
		else
		{
			WCD_DEBUG_ERROR_0("=WCD_RM: <rm_audio_sr_ioctl, Mutex was not acquired originally in Audio_Resource_Manager> \n");
		}
	}
//#endif

	WCD_DEBUG_INFO_1("-WCD_RM: <rm_audio_sr_ioctl, Exiting with rc=(%d)> \n", rc);	 

	return rc;
}


wcd_result rm_clock_block_control(rm_audio_resource_manager_client client, rm_audio_shared_resource shared_res, wcd_bool enable)
{
	wcd_result rc = E_WCD_SUCCESS;

	WCD_DEBUG_INFO_0("+WCD_RM: <rm_clock_block_control called> \n");
	WCD_DEBUG_INFO_3("=WCD_RM: <rm_clock_block_control called, client=(0x%x), shared_res=(0x%x), enable=(0x%x)> \n", client, shared_res, enable);
	WCD_DEBUG_INFO_1("=WCD_RM: <rm_clock_block_control called, current_shared_resource=(0x%x)> \n", current_shared_resource);
	WCD_DEBUG_INFO_3("=WCD_RM: <rm_clock_block_control called, Ext_clk_client_no=(0x%x), RC_osc_client_no=(0x%x), No_clk_client_no=(0x%x)> \n", rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK][0], rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC][0], rm_sr_list.rm_shared_resource_active_client[SHARED_RESOURCE_CLOCK_BLOCK_NONE][0]);	 

	HANDLE_UNUSED_PARAMETER(enable);
	HANDLE_UNUSED_PARAMETER(client);

	if (shared_res == SHARED_RESOURCE_CLOCK_BLOCK_EXT_CLK)
	{
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_clock_block_control: Bringup Ext Clk, client=(0x%x), shared_res=(0x%x), current_shared_resource=(0x%x)> \n", client, shared_res, current_shared_resource);

		// Setup External Clock as the clock resource
		
		// Initialize Clock block
		// 0x109; 0x04; 0x0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_CLK_BUFF_EN2_MCLK_EN, 0, DISABLE_CLK_BUFF_EN2_MCLK_EN);
		// WCD_SLEEP_MICRO(50);
		WCD_SLEEP(1);

		// Leakage controls, Now done as part of "wcd_reset_codec" in "target_defs.c"
		// set bit 1 of reg 0x088 to 1
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_LEAKAGE_CTL, MASK_GDFS_EN_REST, 0, ENABLE_GDFS_EN_REST);
		// set bit 0 of reg 0x088 to 1
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_LEAKAGE_CTL, MASK_GDFS_EN_FEW, 0, ENABLE_GDFS_EN_FEW);
		// set bit 2 of reg 0x88 to 0
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_LEAKAGE_CTL, MASK_GDFS_CLAMP_EN, 0, DISABLE_GDFS_CLAMP_EN);

		if (RM_WCD9320_V1P0 == FALSE)
		{
			// #BIAS_CURR_CTL_2__PROGRAM_THE_0P85V_VBG_REFERENCE = V_0P868V
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CURR_CTL_2, 0x0C, 0, 0x08);

			// Initialize Bandgap
			//set bit 7 of reg 0x101 to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_REGISTER_PRECHARGE, 0, ENABLE_REGISTER_PRECHARGE);
			//set bit 2 of reg 0x101 to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_DC_START_UP_EN, 0, ENABLE_DC_START_UP_EN);
			//set bit 1:0 of reg 0x101 to 01
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_ENABLE_MIC_BIAS_BG_MODE, 0, ENABLE_MIC_BIAS_BG_SLOW_MODE);
			WCD_SLEEP(1);
			//set bit 7 of reg 0x101 to 0
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_REGISTER_PRECHARGE, 0, DISABLE_REGISTER_PRECHARGE);
		}

		// 0x19E; 0x01; 0x00, MCLK FSEL = 9.6MHz
		// wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_RX_COM_TIMER_DIV, MASK_RX_COM_TIMER_DIV_MCLK_FSEL, 0, ENABLE_F_9P6MHZ);

		//set bit 0 of reg 0x105 to 0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_OSC_BG_CTL, MASK_BIAS_OSC_BG_CTL_BG_EN, 0, DISABLE_BIAS_OSC_BG_CTL_BG_EN);

/*		//set bit 6 of reg 0x12E to 1 for recording and set it to 0 for playback
		if (recording_usecase_count > 0)
			rc |= rm_codec_set_cfilt_mode(CFILT_SLOW);
		else
			rc |= rm_codec_set_cfilt_mode(CFILT_FAST);
*/
		// 0x109; 0x02; 0x02, Asserts the ckt
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_ASYNC_RESET_BLANK_CKT, 0, ENABLE_RESET_BLANK_CKT);

		// 0x1FA; 0x80; 0x00
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_RC_OSC_FREQ, MASK_RC_OSC_EN, 0, DISABLE_RC_OSC_EN);
		// 0x1FB; 0x80; 0x00
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_RC_OSC_TEST, MASK_SCHT_EN, 0, DISABLE_SCHT_EN);

		// Bit 3 Register 0x108 will control the clock source
		// 0: EXT_CLK_SEL
		// 1: RC_OSC_CLK_SEL
		// 0x108; 0x08; 0x0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN1, MASK_CLK_SOURCE_SEL, 0, ENABLE_CLK_SOURCE_EXT_CLK);
		// 0x108; 0x01; 0x01
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN1, MASK_CLK_BUFF_EN, 0, ENABLE_CLK_BUFF_EN);
		//#Delay_microS 50
		// WCD_SLEEP_MICRO(50);
		//#Delay_mS 1
		WCD_SLEEP(1);
				
		// Initialize clock power control, 0x314, 0x3, 0x3, Now done as part of "wcd_reset_codec" in "target_defs.c" 
		// #CDC_CLK_POWER_CTL__GRP_A = ENABLE
		// #CDC_CLK_POWER_CTL__GRP_B = ENABLE
  		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_POWER_CTL, MASK_CDC_CLK_POWER_CTL_GRP_A_B, 0, ENABLE_CDC_CLK_POWER_CTL_GRP_A_B);
		// Initialize Master clock, Now done as part of "wcd_reset_codec" in "target_defs.c" 
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_MCLK_CTL, MASK_CDC_CLK_MCLK_CTL_MCLK_EN, 0, ENABLE_CDC_CLK_MCLK_CTL_MCLK_EN);

		// 0x109; 0x02; 0x00
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_ASYNC_RESET_BLANK_CKT, 0, DISABLE_RESET_BLANK_CKT);
		//#CLK_BUFF_EN2__MCLK_EN = ENABLE  
		// 0x109; 0x04; 0x04
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_CLK_BUFF_EN2_MCLK_EN, 0, ENABLE_CLK_BUFF_EN2_MCLK_EN);
		// #Delay_mS 1
		WCD_SLEEP(1);	
	
		// Initialize Connection Clock - TX only
		if (RM_WCD9320_V1P0 == TRUE)
		{
			// set bit 2 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN, 0, ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN);
		}
		else if (RM_WCD9320_V1P0 == FALSE)
		{
			// set bit 5 of reg 0x30C to 1// removing this as this was setting clock to invert
			//rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN, 0, ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN);
			// set bit 2 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN, 0, ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN);
		}

		// TAIKO
		// set bit 4 of reg 0x14C to 1 for 9.6MHz Ext Mclk
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_TX_COM_BIAS, MASK_AAF_FILT_MODE, 0, MASK_AAF_FILT_MODE_9P6);

		if (wcd_init_options.bustype == E_WCD_I2C)
		{
			// set bit 4 of reg 0x306 to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_RX_I2S_CTL, 0x10, 0, 0x10);
			// set bit 4 of reg 0x307 to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_TX_I2S_CTL, 0x10, 0, 0x10);
		}

#ifdef WCD_MBHC_ENABLE
		if (RM_WCD9320_V1P0 == TRUE)
		{
			rc |= rm_mbhc_setup_clk_freq_dependent_params(TRUE);
		}
#endif

	}
	else if (shared_res == SHARED_RESOURCE_CLOCK_BLOCK_RC_OSC)
	{
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_clock_block_control: Bringup RC OSC, client=(0x%x), shared_res=(0x%x), current_shared_resource=(0x%x)> \n", client, shared_res, current_shared_resource);

		// Setup RC Oscillator as the clock resource
			
		// 0x109; 0x04; 0x0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_CLK_BUFF_EN2_MCLK_EN, 0, DISABLE_CLK_BUFF_EN2_MCLK_EN);
		// WCD_SLEEP_MICRO(50);
		WCD_SLEEP(1);

		// Leakage controls, Now done as part of "wcd_reset_codec" in "target_defs.c" 
		// set bit 1 of reg 0x088 to 1
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_LEAKAGE_CTL, MASK_GDFS_EN_REST, 0, ENABLE_GDFS_EN_REST);
		// set bit 0 of reg 0x088 to 1
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_LEAKAGE_CTL, MASK_GDFS_EN_FEW, 0, ENABLE_GDFS_EN_FEW);
		// set bit 2 of reg 0x88 to 0
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_LEAKAGE_CTL, MASK_GDFS_CLAMP_EN, 0, DISABLE_GDFS_CLAMP_EN);

		if (RM_WCD9320_V1P0 == FALSE)
		{
			// #BIAS_CURR_CTL_2__PROGRAM_THE_0P85V_VBG_REFERENCE = V_0P868V
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CURR_CTL_2, 0x0C, 0, 0x08);

			// Initialize Bandgap
			//set bit 7 of reg 0x101 to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_REGISTER_PRECHARGE, 0, ENABLE_REGISTER_PRECHARGE);
			//set bit 2 of reg 0x101 to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_DC_START_UP_EN, 0, ENABLE_DC_START_UP_EN);
			//set bit 1:0 of reg 0x101 to 10
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_ENABLE_MIC_BIAS_BG_MODE, 0, ENABLE_MIC_BIAS_BG_FAST_MODE);
			WCD_SLEEP(1);
			//WCD_SLEEP_MICRO(20);
			//set bit 7 of reg 0x101 to 0
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_REGISTER_PRECHARGE, 0, DISABLE_REGISTER_PRECHARGE);
		}

		//set bit 0 of reg 0x105 to 1
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_OSC_BG_CTL, MASK_BIAS_OSC_BG_CTL_BG_EN, 0, ENABLE_BIAS_OSC_BG_CTL_BG_EN);

		//set bit 6 of reg 0x12E to 0
		rc |= rm_codec_set_cfilt_mode(CFILT_FAST);

		//set bits 6:5 of reg 0x131 to 0b01
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_2_CTL, MASK_CFILT_SEL, 0, ENABLE_CFILT_2);

		// Initialize Clock block
		// 0x1FA; 0x80; 0x80
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_RC_OSC_FREQ, MASK_RC_OSC_EN, 0, ENABLE_RC_OSC_EN);
		// 0x1FB; 0x80; 0x80
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_RC_OSC_TEST, MASK_SCHT_EN, 0, ENABLE_SCHT_EN);
		// #Delay_mS 1
		WCD_SLEEP(1);
		// #Delay_microS 10
		// WCD_SLEEP_MICRO(10);
		// 0x1FB; 0x80; 0x00
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_RC_OSC_TEST, MASK_SCHT_EN, 0, DISABLE_SCHT_EN);
		// WCD_SLEEP_MICRO(20);
		WCD_SLEEP(1);

		// 0x108; 0x08; 0x08
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN1, MASK_CLK_SOURCE_SEL, 0, ENABLE_CLK_SOURCE_SEL_RC);
		// 0x109; 0x02; 0x00
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_ASYNC_RESET_BLANK_CKT, 0, DISABLE_RESET_BLANK_CKT);
		// 0x109; 0x02; 0x02
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_ASYNC_RESET_BLANK_CKT, 0, ENABLE_RESET_BLANK_CKT);
		// 0x108; 0x01; 0x0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN1, MASK_CLK_BUFF_EN, 0, DISABLE_CLK_BUFF_EN);

		// Initialize clock power control, 0x314, 0x3, 0x3, Now done as part of "wcd_reset_codec" in "target_defs.c" 
		// #CDC_CLK_POWER_CTL__GRP_A = ENABLE
		// #CDC_CLK_POWER_CTL__GRP_B = ENABLE
  		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_POWER_CTL, MASK_CDC_CLK_POWER_CTL_GRP_A_B, 0, ENABLE_CDC_CLK_POWER_CTL_GRP_A_B);
		// Initialize Master clock, Now done as part of "wcd_reset_codec" in "target_defs.c" 
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_MCLK_CTL, MASK_CDC_CLK_MCLK_CTL_MCLK_EN, 0, ENABLE_CDC_CLK_MCLK_CTL_MCLK_EN);

		// #Delay_mS 1
		WCD_SLEEP(1);
		// 0x109; 0x02; 0x00
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_ASYNC_RESET_BLANK_CKT, 0, DISABLE_RESET_BLANK_CKT);
		// 0x109; 0x04; 0x04
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_CLK_BUFF_EN2_MCLK_EN, 0, ENABLE_CLK_BUFF_EN2_MCLK_EN);

		// Initialize Connection Clock - TX only
		if (RM_WCD9320_V1P0 == TRUE)
		{
			// set bit 2 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN, 0, ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN);
		}
		else if (RM_WCD9320_V1P0 == FALSE)
		{
			// set bit 5 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN, 0, ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN);
			// set bit 2 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN, 0, ENABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN);
		}

		// TAIKO
		if (RM_WCD9320_V1P0 == TRUE)
		{
			// set bit 4 of reg 0x14C to 0 for 12.288MHz RC Osc clock
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_TX_COM_BIAS, MASK_AAF_FILT_MODE, 0, MASK_AAF_FILT_MODE_12P288);
		}
		else
		{
			// set bit 4 of reg 0x14C to 1 for 9.6MHz RC Osc clock
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_TX_COM_BIAS, MASK_AAF_FILT_MODE, 0, MASK_AAF_FILT_MODE_9P6);
		}

#ifdef WCD_MBHC_ENABLE
		if (RM_WCD9320_V1P0 == TRUE)
		{
			rc |= rm_mbhc_setup_clk_freq_dependent_params(FALSE);
		}
#endif

	}
	else if (shared_res == SHARED_RESOURCE_CLOCK_BLOCK_NONE)
	{
		WCD_DEBUG_INFO_3("=WCD_RM: <rm_clock_block_control: Bringup NO Clock, client=(0x%x), shared_res=(0x%x), current_shared_resource=(0x%x)> \n", client, shared_res, current_shared_resource);

		// Initialize Connection Clock - TX only
		if (RM_WCD9320_V1P0 == TRUE)
		{
			// set bit 2 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN, 0, DISABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN);
		}
		else if (RM_WCD9320_V1P0 == FALSE)
		{
			// set bit 5 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN, 0, DISABLE_CDC_CLK_OTHR_CTL_CONN_CLK_RX_EN);
			// set bit 2 of reg 0x30C to 1
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_OTHER_CTL, MASK_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN, 0, DISABLE_CDC_CLK_OTHR_CTL_CONN_CLK_TX_EN);
		}

		if (wcd_init_options.bustype == E_WCD_I2C)
		{
			// set bit 4 of reg 0x306 to 0
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_RX_I2S_CTL, 0x10, 0, 0x0);
			// set bit 4 of reg 0x307 to 0
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_CLK_TX_I2S_CTL, 0x10, 0, 0x0);
		}

		// 0x109; 0x04; 0x0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_CLK_BUFF_EN2_MCLK_EN, 0, DISABLE_CLK_BUFF_EN2_MCLK_EN);
		// #Delay_microS 50
		// WCD_SLEEP_MICRO(50);
		WCD_SLEEP(1);

		//set bit 0 of reg 0x105 to 0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_OSC_BG_CTL, MASK_BIAS_OSC_BG_CTL_BG_EN, 0, DISABLE_BIAS_OSC_BG_CTL_BG_EN);
		// 0x1FA; 0x80; 0x0
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_RC_OSC_FREQ, MASK_RC_OSC_EN, 0, DISABLE_RC_OSC_EN);

		// 0x109; 0x02; 0x02, Asserts the ckt
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN2, MASK_ASYNC_RESET_BLANK_CKT, 0, ENABLE_RESET_BLANK_CKT);
		// 0x108; 0x01; 0x00, De-asserts CLK_BUFF_EN
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CLK_BUFF_EN1, MASK_CLK_BUFF_EN, 0, DISABLE_CLK_BUFF_EN);

		if (RM_WCD9320_V1P0 == FALSE)
		{
			//set bit 0 of reg 0x101 to 0
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_BG_EN, 0, DISABLE_BG_EN);
			//set bit 2 of reg 0x101 to 0
			rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_BIAS_CENTRAL_BG_CTL, MASK_DC_START_UP_EN, 0, DISABLE_DC_START_UP_EN);
		}
	}

	if (rc == E_WCD_SUCCESS)
	{
		WCD_DEBUG_INFO_0("-WCD_RM: <rm_clock_block_control successful> \n");
	}
	else
	{
		WCD_DEBUG_INFO_1("-WCD_RM: <rm_clock_block_control error, rc=(0x%x)> \n", rc);
	}

	return rc;
}

wcd_result rm_codec_set_cfilt_mode(uint8 bg_mode)
{
	wcd_result rc = E_WCD_SUCCESS;

	if (bg_mode == CFILT_SLOW)
	{
		// rc = wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_CFILT_2_CTL, MASK_CFILT_MODE_CAP_EN, 0, ENABLE_CFILT_MODE_CAP_EN);
		rc = wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_CFILT_2_CTL, MASK_INT_EXT_PRECHRG, 0x0, 0x0);
		WCD_SLEEP(1);
	}
	else
	{
		// rc = wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_CFILT_2_CTL, MASK_CFILT_MODE_CAP_EN, 0, DISABLE_CFILT_MODE_CAP_EN);
		rc = wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_CFILT_2_CTL, MASK_INT_EXT_PRECHRG, 0x0, 0x30);
		WCD_SLEEP(1);
	}
	
	return rc;
}

#ifdef WCD_MBHC_ENABLE

wcd_result rm_mbhc_block_control(wcd_bool enable)
{
	// TRUE  = Enable / Disable Complete, Enable  INTR & Resume Polling
	// FALSE = Enable / Disable Start,    Disable INTR & Pause  Polling

	wcd_result rc = E_WCD_SUCCESS;
	uint8 cmd = RM_MBHC_BLOCK_NONE;

	if (enable) // COMPLETE
	{
		if (RM_MBHC_PLUG == RM_MBHC_HEADPHONE)
			cmd = RM_MBHC_BLOCK_HP_COMPLETE;
		else if ( (RM_MBHC_PLUG == RM_MBHC_HEADSET) || (RM_MBHC_PLUG == RM_MBHC_SPECIAL_HEADSET) )
			cmd = RM_MBHC_BLOCK_HS_COMPLETE;
	}
	else // START
	{
		if (RM_MBHC_PLUG == RM_MBHC_HEADPHONE)
			cmd = RM_MBHC_BLOCK_HP_START;
		else if ( (RM_MBHC_PLUG == RM_MBHC_HEADSET) || (RM_MBHC_PLUG == RM_MBHC_SPECIAL_HEADSET) )
			cmd = RM_MBHC_BLOCK_HS_START;
	}

	WCD_DEBUG_INFO_3("+WCD_RM: <rm_mbhc_block_control, enable=(0x%x), RM_MBHC_PLUG=(0x%x), cmd=(0x%x)> \n", enable, RM_MBHC_PLUG, cmd);

	switch(cmd)
	{
		case RM_MBHC_BLOCK_NONE:
		{
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_mbhc_block_control, Do nothing as nothing is plugged in> \n");
			break;
		}
		case RM_MBHC_BLOCK_HP_START:
		{
			rc |= rm_mbhc_setup_intr(FALSE);
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_mbhc_block_control, Disabled INTR for HP> \n");
			break;
		}
		case RM_MBHC_BLOCK_HS_START:
		{
			rc |= rm_mbhc_pause_polling(TRUE);
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_mbhc_block_control, Paused Polling for HS> \n");
			break;
		}
		case RM_MBHC_BLOCK_HP_COMPLETE:
		{
			rc |= rm_mbhc_setup_intr(TRUE);
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_mbhc_block_control, Enabled INTR for HP> \n");
			break;
		}
		case RM_MBHC_BLOCK_HS_COMPLETE:
		{
			if ((audio_status == AUDIO_STATUS_PLAYBACK_ENABLE_COMPLETE) && (playback_usecase_count == 1))
			{
				rc |= rm_mbhc_set_VDDIO(TRUE);
				WCD_DEBUG_INFO_2("=WCD_RM:<rm_mbhc_block_control, After enabling VDDIO, audio_status=(0x%x), playback_usecase_count=(0x%x)> \n", audio_status, playback_usecase_count);
			}
			else if ( (audio_status==AUDIO_STATUS_PLAYBACK_DISABLE_COMPLETE) && (playback_usecase_count==0) )
			{
				rc |= rm_mbhc_set_VDDIO(FALSE);
				WCD_DEBUG_INFO_3("=WCD_RM: <rm_mbhc_block_control, After disabling VDDIO, cmd=(0x%x), audio_status=(0x%x), playback_usecase_count=(0x%x)> \n", cmd, audio_status, playback_usecase_count);
			}
			rc |= rm_mbhc_pause_polling(FALSE);
			WCD_DEBUG_INFO_0("=WCD_RM: <rm_mbhc_block_control, Resumed Polling for HS> \n");
			break;
		}
		default:
		{
			break;
		}
	}

	WCD_DEBUG_INFO_1("-WCD_RM: <rm_mbhc_block_control, Exiting with rc=(0x%x) \n", rc);
	return rc;
}

wcd_result rm_mbhc_enable_headset_micbias(wcd_bool enable, uint16 hs_micbias_voltage)
{
	wcd_result rc = E_WCD_SUCCESS;
	uint16 mbhc_cfilt_k_value=28;

	WCD_DEBUG_INFO_3("=WCD_RM: <rm_mbhc_enable_headset_micbias called, enable=(%u), default_headset_voltage=(%u), headset_voltage_passed=(%u) > \n", enable, rm_default_headset_voltage, hs_micbias_voltage);

	if (enable)
	{
		rc |= WCDH_MBIAS_MGR_SELECT_MICBIAS(NULL, 2, NULL, NULL);
		rc |= WCDH_MBIAS_MGR_SELECT_CFILTER(NULL, 2, NULL, NULL);
		if (rm_headset_ext_cap_mode == 1)
		{
			// 1 = Ext_Cap; 2 = No_Ext_Cap
			rc |= WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(NULL, 1, NULL, NULL);
		}
		else
		{
			// 1 = Ext_Cap; 2 = No_Ext_Cap
			rc |= WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(NULL, 2, NULL, NULL);
		}
		rc |= mbhc_state_calc_cfilt_k_value(hs_micbias_voltage, &mbhc_cfilt_k_value);
		rc |= WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(NULL, mbhc_cfilt_k_value, NULL, NULL);
		rc |= WCDH_MBIAS_MGR_ENABLE(NULL, 0, NULL, NULL);
	}
	else
	{
		rc |= WCDH_MBIAS_MGR_SELECT_MICBIAS(NULL, 2, NULL, NULL);
		rc |= WCDH_MBIAS_MGR_SELECT_CFILTER(NULL, 2, NULL, NULL);
		rc |= WCDH_MBIAS_MGR_DISABLE(NULL, 0, NULL, NULL);
	}

	return rc;

}

wcd_result rm_mbhc_enable_vddio_switch(wcd_bool enable)
{
	wcd_result rc = E_WCD_SUCCESS;
	
	WCD_DEBUG_INFO_2("=WCD_RM: <rm_mbhc_enable_vddio_switch called, enable=(%u), default_headset_voltage=(%u) > \n", enable, rm_default_headset_voltage);

	rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_B1_CTL, MASK_MBHC_DIGITAL_OVERRIDE, 0, ENABLE_MBHC_DIGITAL_OVERRIDE);
	WCD_SLEEP(5);

	if (enable)
	{
		// Setup VDDIO
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_2_MBHC, MASK_INS_DET_ISRC_CTL, 0, ENABLE_INS_DET_ISRC_CTL);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_2_MBHC, MASK_INS_DET_ISRC_EN, 0, ENABLE_INS_DET_ISRC_EN);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_2_MBHC, MASK_SCHT_TRIG_EN, 0, DISABLE_SCHT_TRIG_EN);
	}
	else
	{
		// Reset VDDIO
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_2_MBHC, MASK_SCHT_TRIG_EN, 0, DISABLE_SCHT_TRIG_EN);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_MICB_2_MBHC, MASK_INS_DET_ISRC_EN, 0, DISABLE_INS_DET_ISRC_EN);
	}

	rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_B1_CTL, MASK_MBHC_DIGITAL_OVERRIDE, 0, DISABLE_MBHC_DIGITAL_OVERRIDE);

	return rc;
}

wcd_result rm_mbhc_set_VDDIO(wcd_bool enable)
{
	wcd_result rc = E_WCD_SUCCESS;

	WCD_DEBUG_INFO_2("+WCD_RM: <rm_mbhc_set_VDDIO called, plug=(%d), default_headset_voltage=(%u)> \n", RM_MBHC_PLUG, rm_default_headset_voltage);
	WCD_DEBUG_INFO_2("=WCD_RM: <rm_mbhc_set_VDDIO called, old_vddio_state=(%u), enable=(%u)> \n", rm_vddio_state, enable);

	if (rm_vddio_state != enable)
	{
		if ( RM_MBHC_PLUG == RM_MBHC_SPECIAL_HEADSET )
		{
			rc |= rm_mbhc_enable_headset_micbias(enable, (uint16)MBHC_MBIAS_2700);
		}
		else
		{
			if (rm_default_headset_voltage <= 1800)
			{
				rc |= rm_mbhc_enable_vddio_switch(enable);
			}
			else
			{
				rc |= rm_mbhc_enable_headset_micbias(enable, rm_default_headset_voltage);
			}
		}

		rm_vddio_state = enable;
	}

	WCD_DEBUG_INFO_1("-WCD_RM: <rm_mbhc_set_VDDIO, new_vddio_state=(%u)> \n", rm_vddio_state);
	return rc;
}

wcd_result rm_mbhc_pause_polling(wcd_bool enable)
{
	wcd_result rc = E_WCD_SUCCESS;

	if (enable)
	{
		// Pause Polling
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK3, MASK_MBHC_ANA_HS_JACK_SWITCH_INT, 0, DISABLE_MBHC_ANA_HS_JACK_SWITCH_INT);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK0, MASK_MBHC_ALL_INTR, 0, DISABLE_MBHC_ALL_INTR);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_CLK_CTL, MASK_MBHC_RESET, 0, ENABLE_MBHC_RESET);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_EN_CTL, MASK_DCE_STA_BTNPOLL_EN, 0, DISABLE_DCE_STA_BTNPOLL_EN);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_CLK_CTL, MASK_MBHC_RESET, 0, DISABLE_MBHC_RESET);
	}
	else
	{
#if 0
		// Resume Polling
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_CLK_CTL, MASK_MBHC_MBHC_CLK_EN, 0, ENABLE_MBHC_MBHC_CLK_EN);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK0, MASK_MBHC_ALL_INTR, 0, ENABLE_MBHC_BUTTON_POLLING_RELEASE);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK3, MASK_MBHC_ANA_HS_JACK_SWITCH_INT, 0, ENABLE_MBHC_ANA_HS_JACK_SWITCH_INT);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_EN_CTL, MASK_DCE_STA_BTNPOLL_EN, 0, DISABLE_DCE_STA_BTNPOLL_EN);
		WCD_SLEEP(1);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_EN_CTL, MASK_DCE_STA_BTNPOLL_EN, 0, ENABLE_BTNPOLL_EN);
#else
		// Resume Polling
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_CLK_CTL, MASK_MBHC_MBHC_CLK_EN, 0, ENABLE_MBHC_MBHC_CLK_EN);
		WCD_SLEEP(1);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK0, MASK_MBHC_ALL_INTR, 0, ENABLE_MBHC_BUTTON_POLLING_RELEASE);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK3, MASK_MBHC_ANA_HS_JACK_SWITCH_INT, 0, ENABLE_MBHC_ANA_HS_JACK_SWITCH_INT);

		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_CLK_CTL, MASK_MBHC_RESET, 0, ENABLE_MBHC_RESET);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_EN_CTL, MASK_DCE_STA_BTNPOLL_EN, 0, DISABLE_DCE_STA_BTNPOLL_EN);
		WCD_SLEEP(1);
		// rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_EN_CTL, MASK_DCE_STA_BTNPOLL_EN, 0, ENABLE_BTNPOLL_EN);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_CLK_CTL, MASK_MBHC_RESET, 0, DISABLE_MBHC_RESET);
		WCD_SLEEP(1);
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_EN_CTL, MASK_DCE_STA_BTNPOLL_EN, 0, ENABLE_BTNPOLL_EN);
#endif
	}

	WCD_DEBUG_INFO_2("=WCD_RM: <rm_mbhc_pause_polling, pause(1)/Resume(0) state=(%u), rc=(%d)> \n", enable, rc);
	return rc;
}

wcd_result rm_mbhc_setup_intr(wcd_bool enable)
{
	wcd_result rc = E_WCD_SUCCESS;

	if (enable)
	{
		// Enable INTR
		rc = wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK3, MASK_MBHC_ANA_HS_JACK_SWITCH_INT, 0, ENABLE_MBHC_ANA_HS_JACK_SWITCH_INT);
	}
	else
	{
		// Disable INTR
		rc = wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_INTR_MASK3, MASK_MBHC_ANA_HS_JACK_SWITCH_INT, 0, DISABLE_MBHC_ANA_HS_JACK_SWITCH_INT);
	}

	return rc;
}

// TAIKO
wcd_result rm_mbhc_setup_clk_freq_dependent_params(wcd_bool enable)
{
	wcd_result rc = E_WCD_SUCCESS;

	// enable = TRUE  ==> Ext Mclk @ 9.6MHz
	// enable = FALSE ==> RC Osc   @ 12.288MHz

	if(enable) 
	{
		// set reg 0x3C3 to n_ready
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B1_CTL, MASK_MBHC_N_READY, 0, MBHC_MCLK_9600_n_ready);
		// set reg 0x3C4 to n_poll
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B2_CTL, MASK_MBHC_N_POLL, 0, MBHC_MCLK_9600_n_poll);
		// set reg 0x3C5 to BounceWait
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B3_CTL, MASK_MBHC_N_BOUNCE_WAIT, 0, MBHC_MCLK_9600_bc_wait);
		// set reg 0x3C8 to n_cic
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B6_CTL, MASK_MBHC_N_CIC, 0, MBHC_MCLK_9600_n_cic);
		// set bit 6:3 of reg 0x3CF to CIC_gain
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_B2_CTL, MASK_MBHC_GAIN, 3, MBHC_MCLK_9600_gain);
	}
	else 
	{
		// set reg 0x3C3 to n_ready
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B1_CTL, MASK_MBHC_N_READY, 0, MBHC_MCLK_12288_n_ready);
		// set reg 0x3C4 to n_poll
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B2_CTL, MASK_MBHC_N_POLL, 0, MBHC_MCLK_12288_n_poll);
		// set reg 0x3C5 to BounceWait
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B3_CTL, MASK_MBHC_N_BOUNCE_WAIT, 0, MBHC_MCLK_12288_bc_wait);
		// set reg 0x3C8 to n_cic
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_TIMER_B6_CTL, MASK_MBHC_N_CIC, 0, MBHC_MCLK_12288_n_cic);
		// set bit 6:3 of reg 0x3CF to CIC_gain
		rc |= wcd_register_write_sync(E_WCD_CODEC_MAIN, ADDR_CDC_MBHC_B2_CTL, MASK_MBHC_GAIN, 3, MBHC_MCLK_12288_gain);
	}

	return rc;
}

#endif
