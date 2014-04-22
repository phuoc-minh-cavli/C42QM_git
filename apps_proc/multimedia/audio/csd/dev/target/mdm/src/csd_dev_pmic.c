/*==============================================================================
  FILE:         csd_dev_tgt_pmic.c

  OVERVIEW:     This file defines handle the pmic module interaction with csd

  Dependencies:

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/mdm/src/csd_dev_pmic.c#5 $
  $DateTime: 2019/06/21 00:18:23 $
  $Author: pwbldsvc $
  $Change: 19615600 $
  $Revision: #5 $

  when       who       what, where, why
  --------   ---       -------------------------------------------------------
  03/23/13   unni      9x25 PMIC support
  18/07/11   oariasri  9x15 initial version
  27/04/10   suns      Initial Creation
==============================================================================*/
#include "csd_os_dependencies.h"
#include "csd_debug_msg.h"
#include "csd_dev_pmic.h"
#include "DALDeviceId.h"
#include "pm_gpio.h"
#include "pm_err_flags.h"


#include "pm.h"
/*TODO: @Unni for enable/disable: use FET controlled using I2C if taiko faces
        timing issues similar to tabla */

#include "npa.h"
#include "npa_resource.h"
static npa_client_handle pm_codec_handle = NULL;
static int32_t ref_count = 0;
static bool_t codec_exist = FALSE; 

/*=============================================================================

  FUNCTION      csd_dev_pmic_enable_codec_rails

  DESCRIPTION   Enable power rails required for device enable

  DEPENDENCIES

  PARAMETERS    @param dev : device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_enable_codec_rails(const struct csd_dev_record* dev)
{
  if( !codec_exist )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_pmic_enable_codec_rails: "
             "No Codec present, skipping PMIC module");
    return CSD_EOK;
  }
  
  return csd_dev_pmic_enable_basic_codec_rails();
}

/*=============================================================================

  FUNCTION      csd_dev_pmic_disable_codec_rails

  DESCRIPTION   Disable power rails used by devices

  DEPENDENCIES

  PARAMETERS    @param dev : device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_disable_codec_rails(const csd_dev_record_t* dev)
{
  if( !codec_exist )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_pmic_disable_codec_rails: "
             "No Codec present, skipping PMIC module");
    return CSD_EOK;
  }
  
  return csd_dev_pmic_disable_basic_codec_rails();
}

/*=============================================================================

  FUNCTION      csd_dev_pmic_init

  DESCRIPTION   Initialize handle and reference count

  DEPENDENCIES

  PARAMETERS    n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_init(void)
{
  int32_t rc = CSD_EOK;  

  if( pm_codec_handle )
  {
    CSD_MSG( CSD_PRIO_LOW, "csd_dev_pmic_init: already initialized" );
    return CSD_EALREADY;
  }      
  
  rc = csd_is_wcd_codec_present( &codec_exist );
  if( !codec_exist )
  { /* Codec Not Present, Skipping PMIC module */    
    CSD_MSG( CSD_PRIO_HIGH, "csd_dev_pmic_init: "
             "No Codec present, skipping PMIC module");
    return CSD_EOK;
  }    
  
  /* All codec rails are on always-on domain pr EXO_CN 
    which will be turned off automatically during MDM sleep
    No codec node voting is required anymore on WCD9335  */
  
  pm_codec_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_CODEC,
                                            "codec", NPA_CLIENT_REQUIRED );  
  CSD_MSG(CSD_PRIO_LOW,"csd_dev_pmic_init: able to get pm handle" );

  if( pm_codec_handle == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pmic_init: Unable to get pm handle" );
    rc = CSD_EFAILED;
  }
  
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_pmic_init

  DESCRIPTION   Clear handle and reference count

  DEPENDENCIES

  PARAMETERS    n/a

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_deinit(void)
{  
  if( !codec_exist )
  { /* Codec Not Present, Skipping PMIC module */    
    CSD_MSG( CSD_PRIO_HIGH, "csd_dev_pmic_deinit: "
             "No Codec present, skipping PMIC module");
    return CSD_EOK;
  }    
  
  if( pm_codec_handle )
  {
    npa_issue_required_request( pm_codec_handle,
                                PMIC_NPA_MODE_ID_AUDIO_STANDBY );
    npa_destroy_client (pm_codec_handle);
    pm_codec_handle = NULL;    
  }

  return CSD_EOK;
}

/*=============================================================================

  FUNCTION      csd_dev_pmic_enable_codec_rails

  DESCRIPTION   vote for basic codec rail up using internal reference count
                Device information is used if a specific pmic control needs
                to be enabled. Independent of device.

  DEPENDENCIES

  PARAMETERS    @param dev : device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_enable_basic_codec_rails( void )
{
  int32_t rc = CSD_EOK;

  if( !codec_exist )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_pmic_enable_basic_codec_rails: "
             "No Codec present, skipping PMIC module");
    return CSD_EOK;
  }

  /* All codec rails are on always-on domain pr EXO_CN 
    which will be turned off automatically during MDM sleep
    No codec node voting is required anymore on WCD9335 */
  
  if( ref_count == 0 )
  {
    npa_issue_required_request( pm_codec_handle,
                                PMIC_NPA_MODE_ID_AUDIO_ACTIVE_HIGH );
    
    CSD_MSG( CSD_PRIO_HIGH, "csd_dev_pmic_enable_basic_codec_rails: PMIC enabled" );            
  }
  
  ref_count++;
  CSD_MSG( CSD_PRIO_LOW, "csd_dev_pmic_enable_basic_codec_rails: ref_count now[%d]", ref_count );
  
  return rc;
}

/*=============================================================================

  FUNCTION      csd_dev_pmic_disable_codec_rails

  DESCRIPTION   un-vote for basic codec rail down using internal reference count
                Device information is used if a specific pmic control needs
                to be enabled. Independent of device.

  DEPENDENCIES

  PARAMETERS    @param dev : device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_disable_basic_codec_rails( void )
{
  int32_t rc = CSD_EOK;

  if( !codec_exist )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_pmic_disable_basic_codec_rails: "
             "No Codec present, skipping PMIC module");
    return CSD_EOK;
  }
  
  /* All codec rails are on always-on domain pr EXO_CN 
    which will be turned off automatically during MDM sleep
    No codec node voting is required anymore on WCD9335 */
  
  if( --ref_count == 0 )
  {
    npa_issue_required_request ( pm_codec_handle,
                                 PMIC_NPA_MODE_ID_AUDIO_STANDBY );
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_pmic_disable_basic_codec_rails: PMIC disabled" );
  }
  
  CSD_MSG( CSD_PRIO_LOW, "csd_dev_pmic_disable_basic_codec_rails: ref_count now[%d]", ref_count );
  return rc;
}
/*=============================================================================

  FUNCTION      csd_dev_pmic_enable_codec_gpios

  DESCRIPTION   Unused

  DEPENDENCIES

  PARAMETERS    @param dev : device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_enable_codec_gpios(const csd_dev_record_t* dev)
{
  return CSD_EOK;
}

/*=============================================================================

  FUNCTION      csd_dev_pmic_disable_codec_gpios

  DESCRIPTION   Unused

  DEPENDENCIES

  PARAMETERS    @param dev : device record

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pmic_disable_codec_gpios(const csd_dev_record_t* dev)
{
  return CSD_EOK;
}
