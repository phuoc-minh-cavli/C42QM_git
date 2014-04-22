/**
@file init_manager.c
@brief 
Implementation and internal functions of WCD initialization manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/init_manager.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/05/14   KRP    Added Mutex for special electrical detection
08/06/13   KNM    Corrected sequence in wcd_init to process buck voltage 
                  passed by client.
06/17/13   KNM    Added support for codec tier detection for wcd9302.
03/07/13   SSP    Driver Unification Update
03/01/13   KNM    Added support for buck voltage selection in wcd init and
                  modified initialize sequence to initialize elements based
                  on codec.
12/15/10   SK     Initial revision
===============================================================================*/

#include "init_manager.h"
#include "usecase_manager.h"
#include "bus_manager.h"
#include "wcd_gen_elements.h"
#include "wcd_utility.h"
#include "target_defs.h"
#ifdef WCD_MBHC_ENABLE
#include "mbhc.h"
#endif
#include "audio_resource_manager.h"


#ifdef SPL_LOG_SUPPORT
	#include "init_manager.tmh"
#endif

/******************************************************************************
 **************            [INTF] DRIVER INIT/DEINIT            ***************
 *****************************************************************************/

/**
 * \brief Driver initialized is used to know if wcd_init call was successful, if not deinit/ioctls cannot be performed.
 */
wcd_bool wcd_driver_initialized = WCD_FALSE;

/**
 * \brief mad global configuration pointer
 */
wcd_configure_mad* mad_global_info = NULL;

/** 
 * \brief Options the driver was initialized with.
 */
wcd_options wcd_init_options;


  void* wcdIoctlMutex = NULL;
  void* ACRMLock = NULL;
#ifdef WCD_MBHC_ENABLE
#ifdef MBHC_HPH_SPECIAL_DET_HANDLING
  void* SPLHPHLock = NULL;
#endif
#endif

/******************************************************************************
 **************    GLOBAL STRUCTURES FOR GPIO and EVENT CB      ***************
 *****************************************************************************/

struct wcd_client_intf_cb wcd_gpio_cb_data;
struct wcd_client_intf_cb wcd_client_event_cb_data;

/** 
 * \brief This function initializes the driver.
 *
 */
wcd_result  wcd_init( void* options )
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_init_param_version* init_param_ver = (wcd_init_param_version*)options;
  wcd_codec_info codec_info;
  wcd_reset_codec_payload_struct wcd_reset_codec_payload;
//#ifdef WCD_MBHC_ENABLE
  rm_params rm_params_send;
//#endif
  
  WCD_DEBUG_INFO_0("+WCD_93XX: <WCD Init.>\n");

//#ifdef WCD_MBHC_ENABLE
  wcdIoctlMutex = WCD_CREATE_MUTEX();
  if (wcdIoctlMutex == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <WCD Init, WCD IOCTL Mutex failed> \n");
    return_code = E_WCD_GENERIC_ERROR;
    return return_code;
  }
  else
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <WCD Init, WCD IOCTL Mutex created successfully> \n");
  }
//#endif

  WCD_MEMSET( (void*)&wcd_init_options, 0xFF, sizeof( wcd_options ) );
  //add default configurations here.
  //wcd_init_options.bustype = E_WCD_SLIMBUS;
  //wcd_init_options.IHGPIO = 0;
  wcd_init_options.vdd_buck = E_WCD_VDD_BUCK_1P8V_2P15V;
  wcd_init_options.mclk_speed = E_WCD_9_6_MHZ;

  WCD_DEBUG_INFO_1("custom message: WCD buck voltage . RC:%d.>\n", wcd_init_options.vdd_buck);
  do
  {
    if(wcd_driver_initialized)
    {
      break; //should we return an error?
    }

    if( options == NULL )
    {
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }


    switch( init_param_ver->MajorVersion )
    {
      case 0:
      {
        wcd_init_param_v0_1* init_param = options;
        switch( init_param_ver->MinorVersion )
        {
          case 1:
          {
            wcd_init_options.mclk_speed = init_param->mclk_speed;
          } //fall through
          case 0:
          {
            wcd_init_options.bustype = init_param->bustype;
            break;
          }
          default: //at this moment the latest version is 0.1 
          {
            return_code =  E_WCD_INITIALIZATION_ERROR;
            break;
          }
        }
        break;
      }//end Major version 0
      case 1:
      {
        wcd_init_param_v1_1* init_param = options;
        switch( init_param_ver->MinorVersion )
        {
          case 1:
          {
            wcd_init_options.mclk_speed = init_param->mclk_speed;
          } //fall through
          case 0:
          {
            wcd_init_options.IHGPIO = init_param->IHGpio;
            wcd_init_options.bustype = init_param->bustype;
            break;
          }
          default: //at this moment the latest version is 1.1 
          {
            return_code =  E_WCD_INITIALIZATION_ERROR;
            break;
          }
        }
        break;
      }//end Major version 1
      case 2:
      {
        wcd_init_param_v2_1* init_param = options;
        switch( init_param_ver->MinorVersion )
        {
          case 1:
          {
            wcd_init_options.mclk_speed = init_param->mclk_speed;
          } //fall through
          case 0:
          {
            wcd_init_options.IHGPIO = init_param->IHGpio;
            wcd_init_options.bustype = init_param->bustype;
            wcd_init_options.vdd_buck = init_param->vdd_buck;
            break;
          }
          default: //at this moment the latest version is 2.1 
          {
            return_code =  E_WCD_INITIALIZATION_ERROR;
            break;
          }
        }
        break;
      }//end Major version 2
      default: //at this moment only major version 0 is supported
      {
        return_code =  E_WCD_INITIALIZATION_ERROR;
        break;
      }
    }
    
    if( return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error parsing initialization options.>\n");
      break;
    }
    // end of initialization options parsing
    
#ifdef WCD_MBHC_ENABLE
    return_code = wcd_reset_codec_external_pin();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Error toggling the external reset pin, Continuing as this is not a fatal error.>\n");
    }
#endif 

    wcd_reset_codec_payload.buck_voltage = wcd_init_options.vdd_buck;
      WCD_DEBUG_ERROR_1("custom message wcd_reset_codec_payload.buck_voltage=%d\n",wcd_reset_codec_payload.buck_voltage);
    
    //initialize bus
    return_code = wcd_bus_init(wcd_init_options.bustype);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing bus.>\n");
      break;
    }

    //detect codec version
    return_code = wcd_get_codec_info(&codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error while obtaining codec information.>\n");
      break;
    }

    //reset codec
    return_code = wcd_reset_codec(&wcd_reset_codec_payload);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error resetting codec.>\n");
      break;
    }

    //get codec tier info
    return_code = wcd_get_codec_tier_info(&codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error while obtaining codec tier info.>\n");
      break;
    }
    
    //initialize elements
    return_code = wcd_initialize_elements(codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing WCD elements.>\n");
      break;
    }

    //initialize decimator mapping for adcs, dmics & slimbus
    return_code = wcd_initialize_codec_decimator_mapping(codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing codec adcs dmics decimator mapping.>\n");
      break;
    }

    //initialize route stack
    return_code = wcd_initialize_route_stack(codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing codec route stack.>\n");
      break;
    }

    //initialize max count for codec TX/RX path
    return_code = wcd_initialize_codec_max_count(codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing max value for codec TX & RX path.>\n");
      break;
    }

    //initialize global usecase list
    return_code = wcd_initialize_global_usecase_list();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing global usecase list.>\n");
      break;
    }

#ifdef WCD_MBHC_ENABLE
#ifdef MBHC_HPH_SPECIAL_DET_HANDLING

	SPLHPHLock = WCD_CREATE_MUTEX();
	  if (SPLHPHLock == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <hph_special_concurrency Mutex creation failed> \n");
      return_code = E_WCD_GENERIC_ERROR;
      break;
    }
    else
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <hph_special_concurrency Mutex creation successful> \n");
    }

#endif
#endif
    ACRMLock = WCD_CREATE_MUTEX();
    if (ACRMLock == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Audio_Resource_Manager_Mutex creation failed> \n");
      return_code = E_WCD_GENERIC_ERROR;
      break;
    }
    else
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Audio_Resource_Manager_Mutex creation successful> \n");
    }

    WCD_DEBUG_INFO_0("=WCD_93XX: <WCD_Module calling rm_audio_sr_ioctl - RM_CMD_INIT> \n");
    rm_params_send.resource_id   = SHARED_RESOURCE_NONE;
    rm_params_send.action        = RM_ENABLE;
    return_code = rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_INIT, &rm_params_send);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <WCD_Module rm_audio_sr_ioctl - RM_CMD_INIT failure, return_code=(0x%x)> \n", return_code);
      return_code = E_WCD_GENERIC_ERROR;
      break;
    }
    else
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <WCD_Module rm_audio_sr_ioctl - RM_CMD_INIT successful> \n");
    }
#ifdef WCD_MBHC_ENABLE
   #ifndef WCD_MBHC_SEPARATE_DEVICE 
        return_code = mbhc_init();
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_1("=WCD_93XX: <mbhc_init failure, return_code=(0x%x)> \n", return_code);
          break;
        }
        else
        {
          WCD_DEBUG_INFO_0("=WCD_93XX: <mbhc_init successful> \n");
        }
#endif
#endif


  } while(WCD_G_FALSE_BOOLEAN);

  if( return_code == E_WCD_SUCCESS )
  {
    wcd_driver_initialized = WCD_TRUE;
  }

  WCD_DEBUG_INFO_1("-WCD_93XX: <WCD Init RC:%d>\n",return_code);
  return return_code;
}

/** 
 * \brief This function deinitializes the driver.
 *
 */
wcd_result wcd_deinit(void)
{
  wcd_result return_code = E_WCD_SUCCESS;
//#ifdef WCD_MBHC_ENABLE
  rm_params rm_params_send;
//#endif

  WCD_DEBUG_INFO_0("+WCD_93XX: <WCD Deinit.>\n");

  do
  {
    if( !wcd_driver_initialized )
    {
      break; //should we return an error?
    }

#ifdef WCD_MBHC_ENABLE

#ifndef WCD_MBHC_SEPARATE_DEVICE 
        mbhc_deinit();
#endif
#endif
    WCD_DEBUG_INFO_0("=WCD_93XX: <WCD_Module calling rm_audio_sr_ioctl - RM_CMD_DEINIT> \n");
    rm_params_send.resource_id   = SHARED_RESOURCE_NONE;
    rm_params_send.action        = RM_DISABLE;
    rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_DEINIT, &rm_params_send);



    wcd_gpio_cb_data.cb               = NULL;
    wcd_gpio_cb_data.cb_data          = NULL;
    wcd_client_event_cb_data.cb       = NULL;
    wcd_client_event_cb_data.cb_data  = NULL;

    wcd_deinitialize_global_usecase_list();

    wcd_bus_deinit();

  } while(WCD_G_FALSE_BOOLEAN);
  
#ifdef WCD_MBHC_ENABLE
  #ifdef MBHC_HPH_SPECIAL_DET_HANDLING

  if (SPLHPHLock != NULL)
   {
	 WCD_DESTROY_MUTEX(SPLHPHLock);
	 SPLHPHLock = NULL;
     WCD_DEBUG_INFO_0("=WCD_93XX: <WCD Deinit, Destroyed hph_special_concurrency mutex> \n");
    }
#endif
#endif
  if( ACRMLock != NULL ) 
  {
    WCD_DESTROY_MUTEX(ACRMLock);
    ACRMLock = NULL;
    WCD_DEBUG_INFO_0("=WCD_93XX: <WCD Deinit, Destroyed Audio_Resource_Manager_Mutex> \n");
  }

  if( wcdIoctlMutex != NULL ) 
  {
    WCD_DESTROY_MUTEX(wcdIoctlMutex);
    wcdIoctlMutex = NULL;
    WCD_DEBUG_INFO_0("=WCD_93XX: <WCD Deinit, Destroyed WCD IOCTL Mutex> \n");
  }

  if( return_code == E_WCD_SUCCESS )
  {
    wcd_driver_initialized = WCD_FALSE;
  }

  WCD_DEBUG_INFO_1("-WCD_93XX: <WCD Deinit. RC:%d.>\n", return_code);
  return return_code;
}

/** 
 * \brief This function returns vdd buck voltage that is set during initialization.
 *
 */
wcd_result wcd_get_vdd_buck_voltage(wcd_vdd_buck_voltage *vdd_buck)
{
  wcd_result return_code = E_WCD_SUCCESS;

  if(vdd_buck != NULL)
  {
    *vdd_buck = wcd_init_options.vdd_buck;
  }
  else
  {
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
  }

  return return_code;
}

/**
 * \brief Executes special WCD commands 
 *  such as Save Client GPIO interface CB or Save Client Event CB
 */
wcd_result wcd_execute_special_command(unsigned int cmd, void* params, void *client_data)
{
  wcd_result return_code = E_WCD_SUCCESS;

  switch (cmd)
  {
    case E_WCD_CMD_SET_GPIO_INTF_CB:
    {
      if (params != NULL)
  		{
         wcd_gpio_cb_data.cb       = ((struct wcd_client_intf_cb*)params)->cb;
         wcd_gpio_cb_data.cb_data  = ((struct wcd_client_intf_cb*)params)->cb_data;
         return_code = E_WCD_SUCCESS;
      }
      else
      {
         WCD_DEBUG_ERROR_0("=WCD_93XX: <wcd_execute_special_command: E_WCD_CMD_SET_GPIO_INTF_CB, Can not set GPIO CB function> \n");
         wcd_gpio_cb_data.cb       = NULL;
         wcd_gpio_cb_data.cb_data  = NULL;
         return_code = E_WCD_GENERIC_ERROR;
      }
      break;
    }
    case E_WCD_CMD_SET_EVENT_CB:
    {
      if (params != NULL)
      {
         wcd_client_event_cb_data.cb       = ((struct wcd_client_intf_cb*)params)->cb;
         wcd_client_event_cb_data.cb_data  = ((struct wcd_client_intf_cb*)params)->cb_data;
         return_code = E_WCD_SUCCESS;
      }
      else
      {
         WCD_DEBUG_ERROR_0("=WCD_93XX: <wcd_execute_special_command: E_WCD_CMD_SET_EVENT_CB, Can not set Client Event CB function> \n");
         wcd_client_event_cb_data.cb       = NULL;
         wcd_client_event_cb_data.cb_data  = NULL;
         return_code = E_WCD_GENERIC_ERROR;
      }
      break;
    }
    default:
    {
       WCD_DEBUG_ERROR_0("=WCD_93XX: <wcd_special_command: Unsupported command> \n");
       return_code = E_WCD_INVALID_COMMAND_TYPE;
    }
  }

  HANDLE_UNUSED_PARAMETER(client_data);

  return return_code;
}

/******************************************************************************
 **************            [INTF] DRIVER INIT/DEINIT            ***************
 *****************************************************************************/

