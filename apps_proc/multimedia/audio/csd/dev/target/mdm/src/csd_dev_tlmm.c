/*==============================================================================
  @file csd_dev_tlmm.c

  OVERVIEW:     The objective of this module is to provide an control over
                GPIO related operations. Since the GPIO is related to hardware,
                we have the hardware related details hard cored in this file.
                The format of GPIO configurations are like so:-

                DAL_GPIO_CFG(<gpio number>, <function select>, <direction>,
                              <pull>, <drive strength>)

  Dependencies: @see TOP LEVEL MODE MULTIPLEXOR DEVICE ACCESS LAYER CHIPSET
                     PIN CONFIGURATION GUIDE

                Copyright (c) 2008-12, 2016 Qualcomm Technologies Incorporated.
                All rights reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/mdm/src/csd_dev_tlmm.c#8 $
  $DateTime: 2019/06/21 00:18:23 $
  $Author: pwbldsvc $
  $Change: 19615600 $
  $Revision: #8 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  11/15/16   rk      Abstracting OSAL layer
  06/16/14   jyuan   GPIO Protection using TLMM 2.0 API
  10/21/13   vk      changes related to TLMM GPIO
  07/17/13   unni    Adversarial use case support
  06/28/13   rp      Added RT proxy device support
  05/31/13   rp      Added SGLTE External VFR support
  10/05/12   unni    9x25 Tested and cleaned
  08/07/12   unni    9x25 base
  07/24/12   unni    Support for PCM
  07/13/12   rp      Added 9x25 AFE support.
  04/27/10   suns    Initial version
==============================================================================*/

#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITlmm.h"
#include "DALSys.h"
#include "csd_dev_tlmm.h"
#include "csd_debug_msg.h"
#include "adsp_afe_service_commands.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "csd_os_dependencies.h"
#include "csd_dev_gpio_defs.h"
#include "csd_debug_info.h"
#include "csd_dev_pmic.h"
#include "pm_gpio.h"
#include "pm_err_flags.h"

/* GPIO reference counts for keeping track of shared GPIO */
csd_gpio_info gpio_info[GPIO_USECASE_MAX];

/* dal variables shared across multiple enable/disable */
static DalDeviceHandle* csd_tlmm_handle  = NULL;
static csd_lock_t csd_tlmm_mutex  = NULL;
static bool_t configure_codec = FALSE;
static DalChipInfoFamilyType chip_family;
static DalPlatformInfoPlatformInfoType platform_info;

/*=============================================================================

  FUNCTION      csd_dev_tlmm_enable_gpio_use_case

  DESCRIPTION   enable GPIO use case

  DEPENDENCIES  none

  PARAMETERS    @use_case : use case defined in csd_gpio_use_case

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=========================================== */

static int32_t csd_dev_tlmm_enable_gpio_use_case( csd_gpio_use_case use_case )
{
  int32_t rc = CSD_EOK;
  DALResult dal_rc = 0,dal_rc_1 = 0;
  DalTlmm_GpioConfigIdType config;
  DALGpioValueType value = DAL_GPIO_LOW_VALUE;
  char_t gpio_function_name[GPIO_FUNCTION_NAME_MAX_SIZE];

  if( use_case >= GPIO_USECASE_MAX )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_enable_gpio_use_case: Unknown use case[%d]", use_case );
    return CSD_EBADPARAM;
  }

  // Check if GPIO has already been enabled
  if( gpio_info[use_case].ref_count != 0 )
  {
    gpio_info[use_case].ref_count++;
    if(use_case == CDC_BUCK_EN)
     {
     pm_err_flag_type return_val ;
     return_val = pm_gpio_set_current_source_pulls(0x0, PM_GPIO_2 ,PM_GPIO_I_SOURCE_PULL_UP_30uA);
     CSD_MSG(CSD_PRIO_ERROR,"PMIC GPIO Pull Up API Called , returned with val = %d",return_val);
     CSD_SLEEP(10);
     }
    CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_enable_gpio_use_case: "
             "GPIO use case[%d] already enabled, ref_count now[%d]", use_case, gpio_info[use_case].ref_count );
    return CSD_EOK;
  }

  // Fill in GPIO name/configuration
    rc = csd_dev_tlmm_get_gpio_setting( use_case, gpio_function_name, &config );

	//if (use_case != CODEC_INIT) 	// hack to follow exact DTS sequence
	//{
	// Find the GPIO ID based on the function name
  	if( rc == CSD_EOK )
  	{
  		dal_rc = DalTlmm_GetGpioId( csd_tlmm_handle, gpio_function_name, &gpio_info[use_case].gpio_id );
  		if( dal_rc != DAL_SUCCESS )
  		{
  		    rc = CSD_EFAILED;
	  		CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_enable_gpio_use_case: Unable to get GPIO ID for use_case[%d], dal_rc[0x%x]", use_case, dal_rc );
  		}
  	}

	//}

  // Config the GPIO
   if( rc == CSD_EOK )
   {
    	dal_rc = DalTlmm_ConfigGpioId( csd_tlmm_handle, gpio_info[use_case].gpio_id, &config );

      	if (use_case == CODEC_INIT)
  		{
    		value = DAL_GPIO_HIGH_VALUE;
			dal_rc_1 = DalTlmm_GpioIdOut(csd_tlmm_handle,gpio_info[use_case].gpio_id,value);
    		CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_enable_gpio_use_case: CODEC_INIT[0x%x]", dal_rc_1 );
  		}

		if(use_case == CODEC_RESET)
   		{
      		value = DAL_GPIO_LOW_VALUE;
      		dal_rc_1 = DalTlmm_GpioIdOut(csd_tlmm_handle,gpio_info[use_case].gpio_id,value);
      		CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_enable_gpio_use_case: CODEC_RESET[0x%x]", dal_rc_1 );
   		}
		
    //Enabling PMIC GPIO2 FOR TWIZY BU
    //pm_gpio_set_current_source_pulls(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_current_src_pulls_type  i_source_pulls)
    //chip is 0x0, gpio is PM_GPIO_2, 
    //pm_gpio_current_src_pulls_type value
    //for enable    PM_GPIO_I_SOURCE_PULL_UP_30uA 
    //for disable  PM_GPIO_I_SOURCE_PULL_DOWN_10uA
    //Header file: pm_gpio.h */
		if(use_case == CDC_BUCK_EN)
		{   pm_err_flag_type return_val ;
      	value = DAL_GPIO_HIGH_VALUE;
		  	dal_rc_1 = DalTlmm_GpioIdOut(csd_tlmm_handle,gpio_info[use_case].gpio_id,value);
		  	CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_enable_gpio_use_case: CDC_BUCK[0x%x]", dal_rc_1 );

        return_val = pm_gpio_set_current_source_pulls(0x0, PM_GPIO_2 ,PM_GPIO_I_SOURCE_PULL_UP_30uA);
     		CSD_MSG(CSD_PRIO_LOW,"PMIC GPIO Pull Up API Called , returned with val = %d",return_val);
     		CSD_SLEEP(10);
		}
  
    	if( dal_rc != DAL_SUCCESS || dal_rc_1 != DAL_SUCCESS )
    	{
      		rc = CSD_EFAILED;
			CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_enable_gpio_use_case: Unable to config GPIO, dal_rc[0x%x]", dal_rc );
			CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_enable_gpio_use_case: Unable to config GPIO, dal_rc_1[0x%x]", dal_rc_1 );
    	}
    	else
    	{
      		gpio_info[use_case].ref_count++;
      		CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_tlmm_enable_gpio_use_case: GPIO[use_case: %d] configured", use_case );
    	}

   	}
  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_tlmm_disable_gpio_use_case

  DESCRIPTION   enable GPIO use case

  DEPENDENCIES  none

  PARAMETERS    @use_case : use case defined in csd_gpio_use_case

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static int32_t csd_dev_tlmm_disable_gpio_use_case( csd_gpio_use_case use_case )
{
  int32_t rc = CSD_EOK;
  DALResult dal_rc;

  if( use_case >= GPIO_USECASE_MAX )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_disable_gpio_use_case: Unknown use case[%d]", use_case )
    return CSD_EBADPARAM;
  }

  if( gpio_info[use_case].ref_count == 0 )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_disable_gpio_use_case: Use case[%d] not enabled yet", use_case );
    return CSD_EFAILED;
  }

  if( --gpio_info[use_case].ref_count > 0 )
  {
    CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_disable_gpio_use_case: "
             "GPIO use case[%d] still has other clients, ref_count now[%d]", use_case, gpio_info[use_case].ref_count );
    return CSD_EOK;
  }

  // Put GPIO in low power inactive state
  dal_rc = DalTlmm_ConfigGpioIdInactive( csd_tlmm_handle, gpio_info[use_case].gpio_id );
  if( dal_rc != DAL_SUCCESS )
  {
    rc = CSD_EFAILED;
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_disable_gpio_use_case: "
             "Unable to config GPIO[usecase: %d] into inactive state, dal_rc[0x%x]", use_case, dal_rc );
  }

  // Release GPIO
  dal_rc = DalTlmm_ReleaseGpioId( csd_tlmm_handle, gpio_info[use_case].gpio_id );
  if( dal_rc != DAL_SUCCESS )
  {
    rc = CSD_EFAILED;
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_disable_gpio_use_case: "
             "Unable to release GPIO[usecase: %d], dal_rc[0x%x]", use_case, dal_rc );
  }
  else
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_tlmm_disable_gpio_use_case: GPIO[usecase: %d] released", use_case );
  }

  gpio_info[use_case].gpio_id = 0;
    //Disabling PMIC GPIO2 FOR TWIZY BU
    //pm_gpio_set_current_source_pulls(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_current_src_pulls_type  i_source_pulls)
    //chip is 0x0, gpio is PM_GPIO_2, 
    //pm_gpio_current_src_pulls_type value
    //for enable    PM_GPIO_I_SOURCE_PULL_UP_30uA 
    //for disable  PM_GPIO_I_SOURCE_PULL_DOWN_10uA
    //Header file: pm_gpio.h */

     if(rc == 0 && use_case == CODEC_INIT)   //Releasing PMIC GPIO_02 when codec GPIOS are released, added for Twizy, Need to Review Further
     {
     int32_t rc = CSD_EOK ;
     pm_err_flag_type return_val ;
     return_val = pm_gpio_set_current_source_pulls(0x0, PM_GPIO_2 ,PM_GPIO_I_SOURCE_PULL_DOWN_10uA);
     CSD_MSG(CSD_PRIO_LOW,"PMIC GPIO Pull Down API Called , returned with val = %d",return_val);
     CSD_SLEEP(10); 

     rc = csd_dev_tlmm_cdc_buck_gpio_control(FALSE);
     CSD_MSG(CSD_PRIO_LOW,"Buck Disabled with  val = %d",rc);
    }


  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_tlmm_control

  DESCRIPTION   Physical enabling and disabling are depended on devices
                sharing the clock. If there is at least on device using the
                clock, then the clock will not be physically disabled.

  DEPENDENCIES  none

  PARAMETERS    @param dev    : Device info
                @param enable : if true will enable the device else disable

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static int32_t csd_dev_tlmm_control(const struct csd_dev_record* dev,
                                    uint32_t enable)
{
  int32_t rc = CSD_EOK;
  csd_gpio_use_case use_case[GPIO_USECASE_MAX] = { GPIO_USECASE_MAX };
  uint32_t use_case_count = 0;
  uint16_t afeportid = dev->dev_info.afe_port_id;;

  // This code is used to get all the possible configurable pins
  if( afeportid>AFE_PORT_ID_SLIMBUS_RANGE_START &&
      afeportid<=AFE_PORT_ID_SLIMBUS_RANGE_END )
  {
    /* Generalizing Slimbus port for validation via switch */
    afeportid = AFE_PORT_ID_SLIMBUS_RANGE_START;
  }

  switch( afeportid )
  {
    case AFE_PORT_ID_PRIMARY_MI2S_RX:
      use_case[use_case_count++] = MCLK;
      use_case[use_case_count++] = PRI_SYNC_CLK;
      use_case[use_case_count++] = PRI_BIT_CLK;
      use_case[use_case_count++] = PRI_DATA_OUT;
    break;

    case AFE_PORT_ID_PRIMARY_MI2S_TX:
      use_case[use_case_count++] = MCLK;
      use_case[use_case_count++] = PRI_SYNC_CLK;
      use_case[use_case_count++] = PRI_BIT_CLK;
      use_case[use_case_count++] = PRI_DATA_IN;
    break;

    case AFE_PORT_ID_SECONDARY_MI2S_RX:
      use_case[use_case_count++] = MCLK;
      use_case[use_case_count++] = SEC_SYNC_CLK;
      use_case[use_case_count++] = SEC_BIT_CLK;
      use_case[use_case_count++] = SEC_DATA_OUT;
    break;

    case AFE_PORT_ID_SECONDARY_MI2S_TX:
      use_case[use_case_count++] = MCLK;
      use_case[use_case_count++] = SEC_SYNC_CLK;
      use_case[use_case_count++] = SEC_BIT_CLK;
      use_case[use_case_count++] = SEC_DATA_IN;
    break;

    case AFE_PORT_ID_PRIMARY_PCM_RX:
      use_case[use_case_count++] = PRI_SYNC_CLK;
      use_case[use_case_count++] = PRI_BIT_CLK;
      use_case[use_case_count++] = PRI_DATA_OUT;
    break;

    case AFE_PORT_ID_PRIMARY_PCM_TX:
      use_case[use_case_count++] = PRI_SYNC_CLK;
      use_case[use_case_count++] = PRI_BIT_CLK;
      use_case[use_case_count++] = PRI_DATA_IN;
    break;

    case AFE_PORT_ID_SECONDARY_PCM_RX:
      use_case[use_case_count++] = SEC_SYNC_CLK;
      use_case[use_case_count++] = SEC_BIT_CLK;
      use_case[use_case_count++] = SEC_DATA_OUT;
    break;

    case AFE_PORT_ID_SECONDARY_PCM_TX:
      use_case[use_case_count++] = SEC_SYNC_CLK;
      use_case[use_case_count++] = SEC_BIT_CLK;
      use_case[use_case_count++] = SEC_DATA_IN;
    break;

    case AFE_PORT_ID_SLIMBUS_RANGE_START:
    break;

    default:
      rc = CSD_ENORESOURCE;
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_control: "
               "Unknown AFE port[0x%x]", afeportid );
    break;
  }

  // Enable/Disable all use cases
  if( CSD_EOK == rc && use_case_count > 0 )
  {
    uint32_t i = 0;
    uint32_t j = 0;
    for( ; i < use_case_count; i++ )
    {
      if( enable )
      {
        rc = csd_dev_tlmm_enable_gpio_use_case( use_case[i] );
        if( rc != CSD_EOK )
        {
          // Note: MCLK GPIO may not be available in the xml sheet in some
          // use cases (mclk most likely controlled by other processor),
          // this kind of configuration is done "at will" so this workaround
          // is introduced here
          if( use_case[i] == MCLK && configure_codec == FALSE )
          {
            rc = CSD_EOK;
            continue;
          }

          break;
        }
      }
      else
      {
        rc = csd_dev_tlmm_disable_gpio_use_case( use_case[i] );
        if( use_case[i] == MCLK && configure_codec == FALSE )
        {
          rc = CSD_EOK;
          continue;
        }
      }
    }

    // Error handling:
    //  Enable failure: disable all use cases
    //  Disable failure: No-op
    if( rc != CSD_EOK )
    {
      for( ; j < i ; j++ )
      {
        csd_dev_tlmm_disable_gpio_use_case( use_case[j] );
      }
    }
  }

  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_tlmm_gpio_use_case_cleanup

  DESCRIPTION   Disable and release all in-use GPIO use case

  DEPENDENCIES  none

  PARAMETERS

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tlmm_gpio_use_case_cleanup()
{
  int32_t rc = CSD_EOK;
  uint32_t i = 0;

  while( i < GPIO_USECASE_MAX )
  {
    if( gpio_info[i].ref_count != 0 )
    {
      DalTlmm_ConfigGpioIdInactive( csd_tlmm_handle, gpio_info[i].gpio_id );
      DalTlmm_ReleaseGpioId( csd_tlmm_handle, gpio_info[i].gpio_id );
      gpio_info[i].gpio_id = 0;
    }
    i++;
  }

  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_tgt_tlmm_enable

  DESCRIPTION   enable tlmm configuration for a device

  DEPENDENCIES  none

  PARAMETERS    @param dev : reference to a device

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tlmm_enable(const struct csd_dev_record* dev)
{
  int32_t rc = CSD_EOK;
  if( ( NULL != dev) && (!dev->dev_info.is_virtual ) &&
      (dev->dev_info.interface_type != ACDB_DEVICE_TYPE_RT_PROXY_PARAM))
  {
    rc = csd_dev_tlmm_control(dev,TRUE);
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_tlmm_disable

  DESCRIPTION   disable tlmm configuration for a device

  DEPENDENCIES  none

  PARAMETERS    @param dev : reference to a device

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tlmm_disable(const struct csd_dev_record* dev)
{
  int32_t rc = CSD_EOK;
  if( (NULL != dev) && (!dev->dev_info.is_virtual) &&
      (dev->dev_info.interface_type != ACDB_DEVICE_TYPE_RT_PROXY_PARAM))
  {
    rc = csd_dev_tlmm_control(dev,FALSE);
  }
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_tgt_tlmm_init

  DESCRIPTION   Initialize the dal handles shared in subsequent tlmm
                enable/disable functions. Configure codec basic gpio setting.

  DEPENDENCIES  dal tlmm apis

  PARAMETERS    @param  : <>
                @param  <> : <>

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tlmm_init(void)
{
  int32_t rc = CSD_EOK;
  DALResult dal_rc;

  if( NULL != csd_tlmm_handle )
  { /* TLMM handle already present */

    return CSD_EALREADY;
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_HANDLE);


  csd_memset( gpio_info, GPIO_USECASE_MAX*sizeof(csd_gpio_info), 0 );

  /* initialize dal device */
  dal_rc = DAL_DeviceAttach( DALDEVICEID_TLMM, &csd_tlmm_handle );
  if( dal_rc != DAL_SUCCESS )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_init: Unable to attach to TLMM driver, dal_rc[0x%x]", dal_rc );
    rc = CSD_EFAILED;
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
       //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_DEV_ATTACH);

  if( CSD_EOK == rc )
  { /* Open DAL TLMM 2.0 device for TLMM/GPIO configuration */
    dal_rc = DalDevice_Open( csd_tlmm_handle, DAL_OPEN_SHARED);
    if( dal_rc != DAL_SUCCESS )
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_init: Unable to open tlmm device, dal_rc[0x%x]", dal_rc );
      rc = CSD_EFAILED;
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,dal_rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_DEV_OPEN);
    }
    else
    {
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_DEV_OPEN);
    }
  }

  if( CSD_EOK == rc )
  { /* Create SYNC object for reference counter concurrent access protection */
    rc = csd_lock_create( &csd_tlmm_mutex );
    if( CSD_OSAL_EOK != rc )
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_init: Unable to Create SYNC object, dal_rc[0x%x]", dal_rc );
      rc = CSD_EFAILED;
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,dal_rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_SYNC_CREATE);
    }
    else
    {
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_SYNC_CREATE);
    }
  }

  if( CSD_EOK == rc )
  {
    /* Check configuration to see if codec is used */
    rc = csd_is_wcd_codec_present(&configure_codec);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_CODEC_PRESENT);
  }

  if( CSD_EOK == rc )
  {
    /* Get platform info */
    rc = csd_get_cdt_platform_info(&platform_info);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_GET_PLATFORM);
  }

  if( CSD_EOK == rc )
  {
    /* Get chip family */
    rc = csd_get_cdt_chip_family(&chip_family);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_GET_CHIP_INFO);
  }

  if( CSD_EOK == rc && configure_codec )
  { // Reset CODEC

    rc = csd_dev_tlmm_codec_mclk_gpio_control(TRUE);

	if( CSD_EOK == rc )
	{ // Initialize codec

	   rc = csd_dev_pmic_enable_basic_codec_rails();
	   CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_init: Enable pmic codec rails");
		  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
			 //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_CODEC_INIT);
	}

	if( CSD_EOK == rc )
    { // Initialize codec
      CSD_SLEEP(CODEC_RESET_INIT_PERIOD);
      rc = csd_dev_tlmm_enable_gpio_use_case( CDC_BUCK_EN );
      CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_init: codec_buck_en");
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_CODEC_INIT);
    }


	if( CSD_EOK == rc )
    { // Initialize codec
      CSD_SLEEP(CODEC_RESET_INIT_PERIOD);

     rc = csd_dev_tlmm_enable_gpio_use_case( CODEC_RESET );
     CSD_MSG( CSD_PRIO_LOW, "csd_dev_tlmm_init: codec_reset");
     //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_ENABLE_RESET_GPIO);
	}

	 if( rc == CSD_EOK )
     {
	    CSD_SLEEP(CODEC_RESET_INIT_PERIOD);
        rc = csd_dev_tlmm_disable_gpio_use_case( CODEC_RESET );
	 }

     if( CSD_EOK == rc )
     { // Initialize codec
      CSD_SLEEP(CODEC_RESET_INIT_PERIOD);
      
      rc = csd_dev_tlmm_enable_gpio_use_case( CODEC_INIT );
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_CODEC_INIT);
     }


  }

  /* External VFR is configured from ADSP
  if( CSD_EOK == rc )
  { // Enable the external VFR for platform
    rc = csd_dev_tlmm_get_vfr_config( &tlmm_cfg, platform_info.subtype, chip_family );
  }
  */

  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_tgt_tlmm_deinit

  DESCRIPTION   De-initialize  the dal handles shared in tlmm enable/disable
                functions. disable codec basic gpio setting.

  DEPENDENCIES  dal tlmm apis

  PARAMETERS    none

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_tlmm_deinit(void)
{
  int32_t rc = CSD_EOK;

  // Disable and release all GPIO
  csd_dev_tlmm_gpio_use_case_cleanup();
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
     //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_USECASE_CLEANUP);

  /* destroying tlmm handle */
  if( csd_tlmm_handle )
  {
    /* closing dal tlmm handle */
    if( DAL_SUCCESS != DalDevice_Close(csd_tlmm_handle) )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_tlmm_deinit: "
              "Cannot close tlmm device");
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EFAILED,CSD_DEV_INIT_STAGE,
       //  CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_DEV_OPEN);
    }
    else
    {
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_DEV_OPEN);
    }
    /* detaching dal tlmm handle */
    if( DAL_SUCCESS != DAL_DeviceDetach(csd_tlmm_handle) )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_tlmm_deinit: "
              "Cannot detach TLMM driver");
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EFAILED,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_DEV_ATTACH);
    }
    else
    {
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_DEV_ATTACH);
    }
    csd_tlmm_handle = NULL;
  }

  /* destroying tlmm mutex */
  if( csd_tlmm_mutex )
  {
    csd_lock_destroy( csd_tlmm_mutex );
    csd_tlmm_mutex = NULL;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
       //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_TLMM_INIT,CSD_INIT_DEV_TARGET_TLMM_SYNC_CREATE);
  }

  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_reset_codec_gpio

  DESCRIPTION   Stubbed

  DEPENDENCIES  n/a

  PARAMETERS    none

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_reset_codec_gpio()
{
    return csd_dev_tlmm_disable_gpio_use_case( CODEC_INIT );
}
/*=============================================================================

  FUNCTION      csd_dev_release_reset_codec_gpio

  DESCRIPTION   Stubbed

  DEPENDENCIES  n/a

  PARAMETERS    none

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_release_reset_codec_gpio(void)
{
   return csd_dev_tlmm_disable_gpio_use_case( CODEC_INIT );

}
/*=============================================================================

  FUNCTION      csd_dev_tlmm_codec_mclk_gpio_control

  DESCRIPTION   enable/disable tlmm configuration for MCLK sourced from MDM

  DEPENDENCIES  none

  PARAMETERS    @param dev : enable/disable flag

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_tlmm_codec_mclk_gpio_control(bool_t enable)
{
  int32_t rc = CSD_EOK;

  if( enable )
  {
    rc = csd_dev_tlmm_enable_gpio_use_case( MCLK );
  }
  else
  {
    rc = csd_dev_tlmm_disable_gpio_use_case( MCLK );
  }

  return rc;
}

int32_t csd_dev_tlmm_cdc_buck_gpio_control(bool_t enable)
{
  int32_t rc = CSD_EOK;

  if( enable )
  {
    rc = csd_dev_tlmm_enable_gpio_use_case( CDC_BUCK_EN );
     CSD_MSG(CSD_PRIO_ERROR, "cbc_buck_enable");
  }
  else
  {
    rc = csd_dev_tlmm_disable_gpio_use_case( CDC_BUCK_EN );
     CSD_MSG(CSD_PRIO_ERROR, "cbc_buck_disable");
  }

  return rc;
}
