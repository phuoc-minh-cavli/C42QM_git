/*===========================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to this module.
 Notice that changes are listed in reverse chronological order.

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/04/13   unni     Decoupling target/OS specific device configuration from
                     common code using oem framework
 08/27/10   oarias   8200A adaptation
 05/18/10   suns		 initial draft
 ============================================================================*/

#include "mmdefs.h"
#include "csd.h"
#include "csd_dev_oem_plugin_intf.h"
#include "csd_dev_oem.h"
#include "csd_debug_msg.h"
#include "csd_os_dependencies.h"
//#include "wcd_exposed_registers.h"

/*! \fn int32_t csd_dev_oem_init()
 \brief This function inits the oem plugin
 \return int32_t
 */
int32_t csd_dev_oem_init()
{
  /* Set Taiko V2 as default codec for Fusion usecases */
 // wcd_codec_info default_value = {E_WCD_WCD9330, E_WCD_VER_1P0};
 // wcd_set_default_codec(default_value);
  return CSD_EOK;
}

/*! \fn int32_t csd_dev_oem_dinit()
 \brief This function deinits the oem plugin
 \return int32_t
 */
int32_t csd_dev_oem_dinit()
{
  return CSD_EOK;
}

/*! \fn uint32_t csd_dev_oem_get_dev_count(void)
 \brief This function returns the total number of supported devices.
 \return uint32
 */
uint32_t csd_dev_oem_get_dev_count(void)
{
  return (CSD_OEM_DEVICE_ID_MAX_NUM - 1);
}

/*! \fn int32_t csd_dev_oem_msg()
 \brief This function handles OEM supported features
 \return int32_t
 */
int32_t csd_dev_oem_msg(uint32_t cmd, void* params, uint32_t size)
{
  int oem_rc = CSD_DEV_OEM_STATUS_NOT_ACTED;
  switch( cmd )
  {
    case CSD_DEV_OEM_MSG_QUERY_CODEC_BUS_TYPE:
      if( size == sizeof(uint32_t) )
      {
        *(uint32_t*) params = CSD_OEM_CODEC_INTF_TYPE_I2S;
        oem_rc = CSD_DEV_OEM_STATUS_ACTED;
      }
      else
      {
        CSD_MSG(CSD_PRIO_ERROR, "Incorrect param size [%d] for cmd [0x%x]",
                size, cmd);
      }
    break;
    case CSD_DEV_OEM_MSG_QUERY_IS_WCD_CODEC_PRESENT:
      if( size == sizeof(bool_t) )
      {
        int csd_rc = csd_is_wcd_codec_present((bool_t*) params);
        if( CSD_EOK == csd_rc )
        {
          oem_rc = CSD_DEV_OEM_STATUS_ACTED;
        }
        else
        {
          CSD_MSG(CSD_PRIO_ERROR, "Error while checking in csd configuration "
                  "for wcd codec present or not, rc=[0x%x]",
                  csd_rc);
        }
      }
      else
      {
        CSD_MSG(CSD_PRIO_ERROR, "Incorrect param size [%d] for cmd [0x%x]",
                size, cmd);
      }
    break;
    default:
      oem_rc = CSD_DEV_OEM_STATUS_NOT_ACTED;
    break;
  }

  return oem_rc;
}

