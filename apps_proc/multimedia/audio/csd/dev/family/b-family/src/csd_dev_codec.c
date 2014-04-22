/*==============================================================================
 FILE:         csd_dev_codec.c

 OVERVIEW:     This module is responsible for interaction with codec driver.
 APPS controls the codec using this module. It queries acdb for
 codec profiles specific to a device; using such profiles, the
 driver performs register writes to configure the device for a
 specific profile. This module also manages the proper codec
 sequences specified in codec software document.

 Dependencies: Taiko codec driver release v1.0
 @see wcd_intf.h

 Copyright (c) 2008-12, 2016 Qualcomm Technologies Incorporated.
 All rights reserved.
 Qualcomm Confidential and Proprietary
 ==============================================================================*/
/*==============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to this module.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/b-family/src/csd_dev_codec.c#6 $
 $DateTime: 2019/06/11 06:14:45 $
 $Author: pwbldsvc $
 $Change: 19490074 $
 $Revision: #6 $

 when       who     what, where, why
 --------   ---     ---------------------------------------------------------
 11/15/16   rk      Abstracting OSAL layer
 09/01/14   rp      enhanced the csd init debug buffer
 11/09/13   unni    afe side tone fix
 11/08/13   unni    mclk enable before wcd init
 08/09/12   unni    9x25 base
 07/13/12   rp      Added 9x25 AFE support.
 07/24/12   up      PCM Support
 02/28/11   oa      added wcd93xx implementation changes.
 04/27/10   suns    Initial Creation
 ==============================================================================*/

#include "DALSys.h"
#include "csd_dev_manager.h"
#include "csd_dev_pmic.h"
#include "csd_dev_codec.h"
#include "csd_dev_tlmm.h"          /**< for csd_dev_release_reset_codec_gpio >*/
#include "csd_dev_clk.h"           /**< for codec_mclk_Setup >*/
#include "csd_dev_device_info.h"
#include "csd_debug_msg.h"
#include "csd_dev_acdb.h"
#include "wcd_intf.h"
//#include "wcd_exposed_registers.h"   /**< For AANC REGISTER BLOCK DEFINTIONS >*/
#include "csd_dev_codec_private.h"
#include "csd_dev_acdb_private.h"
#include "csd_os_dependencies.h"
#include "csd_debug_info.h"

#define DEVICE_RECORD_INIT_VALUE           0xFF
/** Considering 5 for Audio(Rx/Tx), (2+2)` for Voice, 1 for EC Ref */
#define CSD_DEV_ADIE_CODEC_MAX_RECORDS     10
#define CSD_ACDB_DEFAULT_CODEC_PATH_SIZE   4096
#define WCD_ACDB_COMMAND_MAX               ((CSD_ACDB_DEFAULT_CODEC_PATH_SIZE-\
                                             sizeof(adie_codec_acdb_header))\
                                             /sizeof(wcd_acdb_command))
enum csd_codec_path__steps
{
  CSD_STEP_IDLE = 0,
  CSD_STEP_INITIALIZED,
  CSD_STEP_ENABLED
};
/*
 * Structure for querying acdb for codec profile
 * WCD_ACDB_COMMAND_MAX is the max number of commands that acdb can provide
 */
typedef struct
{
  adie_codec_acdb_header header;
  wcd_acdb_command command[WCD_ACDB_COMMAND_MAX];
} Acdb_WCD93xx_B_CodecProfile;
/*! \def Adie record structure */
typedef struct
{
  uint32_t          device_id;          /** Usage can be deprecated */
  uint32_t          ref_count;          /** Usage Reference Count for the Path: since multiple device_ids can be enabled for same path **/
  uint32_t          step;               /** Setup Stage for the codec */
  uint32_t          sample_rate;        /** Sampling Rate */
  uint32_t          bitwidth;           /** Bit Width */
  uint32_t          codec_path_id;      /** codecPath Identifier: Key Indexing Componanet */
  uint32_t          codec_interface;    /** Codec mode of operation (I2S/Slimbus) */
  uint16_t          afe_port_id;        /** Associated AFE Port ID for the use case */
  uint16_t          direction;          /** Direction of Device */
  void*             codec_route_info;   /** Usage can be Deprecated */
  void*             usecase_handler;    /** Handle/Ptr to the codec use case */
} csd_adie_dev_record_t;
static wcd_ioctl_cmd_codec_info basic_codec_info;
static csd_adie_dev_record_t adie_device_record[CSD_DEV_ADIE_CODEC_MAX_RECORDS];
static bool_t codec_not_present = FALSE; /** Used to skip codec configuration if CODEC not detected */
static bool_t codec_wcd_init = FALSE;    /** Used to indicate whether wcd codec was initailized */

/*=============================================================================

 FUNCTION      csd_dev_oem_override

 DESCRIPTION   

 DEPENDENCIES  n/a

 PARAMETERS    

 RETURN VALUE  Return true if oem override occured
               

 SIDE EFFECTS  none

 =============================================================================*/
static bool_t csd_dev_oem_override(uint32_t cmd, void* payload, int32_t* csd_rc)
{
  int32_t oem_rc = CSD_DEV_OEM_STATUS_NOT_ACTED;
  bool_t override = FALSE;

  switch (cmd)
  {
    case CSD_DEV_OEM_MSG_QUERY_CODEC_BUS_TYPE:
    {
      /* Conveying proper payload for msg */
      oem_rc = csd_dev_oem_msg(cmd, payload, sizeof(uint32_t));
      break;
    }
    case CSD_DEV_OEM_MSG_QUERY_IS_WCD_CODEC_PRESENT:
    {
      /* Conveying proper payload for msg */
      oem_rc = csd_dev_oem_msg(cmd, payload, sizeof(bool_t));
      break;
    }
    default:
    {
      csd_dev_record_t* dev = (csd_dev_record_t*) payload;
      if(dev)
      {
        /* Populate Device Related info for OEM */
        struct csd_oem_dev_enable_entry oem_dev;
        csd_memset(&oem_dev,0,sizeof(oem_dev));
        oem_dev.dev_id  = dev->dev_id;
        oem_dev.freq    = dev->curr_attrib.sample_rate;
        oem_dev.bps     = dev->curr_attrib.bits_per_sample;
        oem_rc = csd_dev_oem_msg(cmd, &oem_dev, sizeof(oem_dev));
      }
      else
      {
        oem_rc = csd_dev_oem_msg(cmd, NULL, 0);
      }
      break;
    }
  }
  /*  Check for return status of OEM */
  if(CSD_DEV_OEM_STATUS_NOT_ACTED != oem_rc)
  {
    /* Overwrite csd_rc based on OEM return code */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_oem_override: "
            "OEM Override occurred for cmd[%d] oem rc[%d]",
            cmd, oem_rc);
    *csd_rc = (CSD_DEV_OEM_STATUS_ACTED == oem_rc)?(CSD_EOK):(CSD_EFAILED);
    override = TRUE;
  }

  return override;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_adie_get_path_dir_type

 DESCRIPTION   Translation for CSD_DEV_(direction) to ADIE_CODEC_(direction)

 DEPENDENCIES

 PARAMETERS    @param csd_dev_type : CSD_DEV(direction)

 RETURN VALUE  ADIE_CODEC_(direction)

 SIDE EFFECTS  none

 =============================================================================*/
static uint32_t csd_dev_codec_adie_get_path_dir_type(uint32_t csd_dev_type)
{
  uint32_t adie_dev_type = 0;
  switch( csd_dev_type )
  {
    case CSD_DEV_RX_DEVICE:
      adie_dev_type = ADIE_CODEC_RX;
    break;
    case CSD_DEV_TX_DEVICE:
      adie_dev_type = ADIE_CODEC_TX;
    break;
    default:
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_adie_get_path_dir_type: "
              "Codec direction is ADIE_CODEC_LB!");
      adie_dev_type = ADIE_CODEC_LB;
    break;
  }
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_adie_get_path_dir_type: "
          "CSD_DEV_TYPE = [%d] ADIE_DEV_TYPE = [%d]",
          csd_dev_type, adie_dev_type);
  return adie_dev_type;
}

/*=============================================================================

 FUNCTION      csd_adie_get_acdb_codec_adie_profile_param_id

 DESCRIPTION   Retrieve the PID to be used for querying ACDB for Codec profile 
               data for given codec on B-family

 DEPENDENCIES  Codec driver used @see wcd

 PARAMETERS    param codec_param_id : codec param id 

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
/*TODO: Unni - can we have profile_param_id as a static variable which is set during wcd_init boot.
 * It doesn't and shouldn't change after boot and if invalid should have failed at boot.
 * Imagine we get a couple of more param ids, then we would have multiple functions
 * which would runtime determine a static value
 */
int32_t csd_adie_get_acdb_codec_adie_profile_param_id( uint32_t *codec_param_id)
{  
  if( !codec_param_id )
    return CSD_EBADPARAM;
  
  switch(basic_codec_info.codec)
  {
    case E_WCD_WCD9320:
      *codec_param_id = ACDB_PID_ADIE_CODEC_PATH_WCD9320;
    break;
    
    case E_WCD_WCD9330:
      *codec_param_id = ACDB_PID_ADIE_CODEC_PATH_WCD9330;
	case E_WCD_WCD9306:                                                    //added
	  *codec_param_id = ACDB_PID_ADIE_CODEC_PATH_WCD9306;
	case E_WCD_WCD9302:  
	  *codec_param_id = ACDB_PID_ADIE_CODEC_PATH_WCD9306;
    break;
	
    
    default:        
      return CSD_EUNSUPPORTED;
  }
  
  return CSD_EOK;
}

/*=============================================================================

 FUNCTION      csd_adie_get_acdb_codec_anc_param_id

 DESCRIPTION   Retrieve the PID to be used for querying ACDB for ANC data for 
               given codec on B-family

 DEPENDENCIES  n/a

 PARAMETERS    param codec_param_id : codec param id 

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_adie_get_acdb_codec_anc_param_id( uint32_t *codec_param_id)
{
  int32_t rc = CSD_EOK;
  if(NULL != codec_param_id)
  {
    switch(basic_codec_info.codec)
    {
      case E_WCD_WCD9320:
        *codec_param_id = ACDB_PID_CODEC_ANC_DATA_WCD9320;
      break;
      default:
        *codec_param_id = 0;
        rc = CSD_EFAILED;
      break;
    }
  }
  else
  {
    rc = CSD_EFAILED;
  }
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_get_codec_path_id

 DESCRIPTION   Function to get codec path id of channels based for both I2S
               and Slimbus

 DEPENDENCIES  n/a

 PARAMETERS    @param adie : adie dev record

 RETURN VALUE  codec_path_id

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_dev_get_codec_path_id(const csd_dev_record_t *dev,
                                         uint32_t *codec_path_id)
{
  int32_t rc = CSD_EOK;

  if( dev && codec_path_id )
  {
    switch( dev->dev_info.interface_type )
    {
      case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
        if((*codec_path_id = dev->dev_info.u.slimbus_dev_info.codec_path_id) ==
              CSD_DEV_INVALID_CODEC_PATH_ID )
        {
          /* Slimbus usecases without wcd codec is Invalid */
          rc = CSD_EFAILED;
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_get_codec_path_id: "
                  "slimbus device with invalid codec path id");
        }

      break;
      case ACDB_DEVICE_TYPE_I2S_PARAM:
        if((*codec_path_id = dev->dev_info.u.i2s_dev_info.codec_path_id) ==
              CSD_DEV_INVALID_CODEC_PATH_ID)
        {
          /*
           * Valid use cases exists where I2S is used without using wcd codec,
           * Aide record won't and shouldn't be created for it. Narrow check
           * for I2S valid codec path requires AFE port id interpretation
           * (only Primary Port is valid) and it is unavailable at codec module
           */
          CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_get_codec_path_id: "
                  "i2s device with invalid codec path id");
        }
      break;
      default:
        CSD_MSG(CSD_PRIO_LOW,"csd_dev_get_codec_path_id: "
                "Assigning Invalid codec path for device type[%d]",
                dev->dev_info.interface_type);
        *codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;
      break;
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_get_codec_path_id: "
            "Bad Param: dev[%p],path[%p]", 
            ((void*)dev), ((void*)codec_path_id));
    rc = CSD_EBADPARAM;
  }
  return rc;
}

/*=============================================================================

 FUNCTION      csd_initialize_adie_device_record

 DESCRIPTION   **Important** device_id 0 is a valid id. All records except
               pointers and bools are initialised to 0xFF(-1)

 DEPENDENCIES

 PARAMETERS    @param adie : reference to adie record

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static void csd_initialize_adie_device_record(csd_adie_dev_record_t* adie)
{
  if( NULL == adie )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_initialize_adie_device_record: "
            "Record reference passed is NULL!");
  }
  else
  {
    csd_memset((void*)(adie), DEVICE_RECORD_INIT_VALUE,
        sizeof(csd_adie_dev_record_t));

    /* make sure all the pointers are initialized to null */
    adie->usecase_handler = NULL;
    adie->codec_route_info = NULL;
  }
}

/*=============================================================================

 FUNCTION      csd_delete_adie_device_record

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_delete_adie_device_record(uint32_t path_id,
                                             uint16_t afe_port)

{
  int32_t rc = CSD_EFAILED;
  wcd_result err = E_WCD_INVALID_HANDLE;
  int32_t iter = 0;
  csd_adie_dev_record_t* adie;

  CSD_MSG(CSD_PRIO_LOW,
                "csd_delete_adie_device_record:  begin");
  for( iter = 0; iter < CSD_DEV_ADIE_CODEC_MAX_RECORDS; iter++ )
  {
    adie = &adie_device_record[iter];
    if( (path_id == adie->codec_path_id) && (afe_port == adie->afe_port_id) )
    {
      if( adie->ref_count > 0 )
      {
        adie->ref_count--;
      }
      else
      {
        rc = CSD_EFAILED;
        CSD_MSG(
            CSD_PRIO_ERROR, "csd_delete_adie_device_record: "
            "Failed deleting the adie_record with ref_count=[%d]!!",
            adie_device_record[iter].ref_count);
        break;
      }
      if( 0 == adie->ref_count )
      {
        err = wcd_close(adie->usecase_handler);
        CSD_MSG(CSD_PRIO_MEDIUM,"csd_delete_adie_device_record: "
                "wcd_close() completed with rc=[%d]", err);

        /* Reset the adie_device_record_details */
        csd_initialize_adie_device_record(adie);
      }
      else
      {
        CSD_MSG(CSD_PRIO_MEDIUM,"csd_delete_adie_device_record: "
                "Skipped deleting adie_record() "
                "new ref_count=[%d]", adie_device_record[iter].ref_count);
      }
      rc = CSD_EOK;
      break;
    }
  } /*end loop*/

  if( CSD_EOK != rc )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_delete_adie_device_record:  failed to find "
             "codec_path_id=[0x%x]", path_id);

  }
  CSD_MSG(CSD_PRIO_LOW,
                "csd_delete_adie_device_record: end");
  return rc;
}

/*=============================================================================

 FUNCTION      csd_query_codec_interface_type

 DESCRIPTION   Helper function to get wcd codec type based on OS specific
               implementation via oem plugin.

 DEPENDENCIES  n/a

 PARAMETERS    n/a

 RETURN VALUE  interface type status @see wcd_types.h

 SIDE EFFECTS  none

 =============================================================================*/
static uint32_t csd_query_codec_interface_type( void )
{
  int32_t rc = CSD_EOK;
  uint32_t bus_type = CSD_OEM_CODEC_INTF_TYPE_INVALID;
  uint32_t interface_type = E_WCD_I2C;

  /* Check if OEM populate CODEC BUS type */
  if( csd_dev_oem_override(CSD_DEV_OEM_MSG_QUERY_CODEC_BUS_TYPE,(void *)&bus_type,&rc) )
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_query_codec_interface_type: "
            "OEM Override occurred on "
            "CSD_DEV_OEM_MSG_QUERY_CODEC_BUS_TYPE, bus_type=[%d],rc=[0x%x]",
            bus_type,rc);

    switch (bus_type)
    {
      case CSD_OEM_CODEC_INTF_TYPE_I2S:
      {
        interface_type = E_WCD_I2C;
        break;
      }
      case CSD_OEM_CODEC_INTF_TYPE_SLIMBUS:
      default:
      {
        interface_type = E_WCD_SLIMBUS;
        break;
      }
    }
  }
  return interface_type;
}

/*=============================================================================

 FUNCTION      csd_dev_is_wcd_codec_present

 DESCRIPTION   Helper function to get whether wcd codec is present based on OS
               specific implementation via oem plugin.

 DEPENDENCIES  n/a

 PARAMETERS    n/a

 RETURN VALUE  boolean flag indicating codec present or not

 SIDE EFFECTS  none

 =============================================================================*/
static bool_t csd_dev_is_wcd_codec_present( void )
{
  int32_t rc = CSD_EOK;
  bool_t oem_wcd_is_present = TRUE; /* temp variable to pass to oem */
  bool_t wcd_is_present = TRUE;     /* Default */

  /* Check if OEM populate CODEC BUS type */
  if( csd_dev_oem_override( CSD_DEV_OEM_MSG_QUERY_IS_WCD_CODEC_PRESENT,
                            (void *)&oem_wcd_is_present, &rc) )
  {
    /* OEM Occured */
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_is_wcd_codec_present: "
            "OEM Override occurred on "
             "CSD_DEV_OEM_MSG_QUERY_IS_WCD_CODEC_PRESENT, wcd_is_present=[%d],"
             "rc=[0x%x]", oem_wcd_is_present, rc);
    wcd_is_present = oem_wcd_is_present;
  }

  return wcd_is_present;
}

/*=============================================================================

 FUNCTION      csd_dev_check_type_match

 DESCRIPTION   Validation type match before creating adie record

 DEPENDENCIES  n/a

 PARAMETERS    param dev : a device record

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_dev_check_type_match(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  wcd_bus_type bustype = (wcd_bus_type)csd_query_codec_interface_type();

  switch( dev->dev_info.interface_type )
  {
    case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
      if(E_WCD_SLIMBUS != bustype)
      {
        /* Type mis-match */
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_check_type_match: "
                "Codec type[%d] is not device type slimbus[%d]",
                bustype, dev->dev_info.interface_type);
        rc = CSD_EBADSTATE;
      }
      break;
    case ACDB_DEVICE_TYPE_I2S_PARAM:
      if(E_WCD_I2C != bustype)
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_check_type_match: "
                "Codec type[%d] is not device type I2C[%d]",
                bustype, dev->dev_info.interface_type);
        rc = CSD_EBADSTATE;
      }
      break;
    default:
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_check_type_match: "
              "Unknown adie type for device type[%x]",
              dev->dev_info.interface_type);
      rc = CSD_EBADPARAM;
      break;
  }
  return rc;
}

/*=============================================================================

 FUNCTION      csd_create_adie_device_record

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_create_adie_device_record(const csd_dev_record_t* dev,
                                             uint32_t path_id)
{
  int32_t index = 0;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  int32_t rc = CSD_EOK;
  csd_adie_dev_record_t* adie = NULL;

  CSD_MSG(CSD_PRIO_LOW, "Entering");

  if( CSD_DEV_INVALID_CODEC_PATH_ID == path_id )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_create_adie_device_record: "
            "Attempting to create adie record for device not "
            "using adie, codec profile id [%d]",path_id);
    rc = CSD_EUNEXPECTED;
  }

  if( CSD_EOK == rc )
  {
    /* check for valid device type use case */
    rc = csd_dev_check_type_match(dev);
  }

  if( CSD_EOK == rc )
  {
    /* Find empty record */
    while( index < CSD_DEV_ADIE_CODEC_MAX_RECORDS)
    {
      if( CSD_DEV_ID_INVALID == adie_device_record[index].device_id )
      {
        CSD_MSG(CSD_PRIO_LOW, "csd_create_adie_device_record: "
                "found a spot on the adie records: %d", index);
        adie = &adie_device_record[index];
        break;
      } /*end device is valid*/
      index++;
    } /*end while*/
    if( index == CSD_DEV_ADIE_CODEC_MAX_RECORDS )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_create_adie_device_record: "
              "Cannot add device to adie device record, "
              "record list  is full");
      rc = CSD_ENORESOURCE;
    }
  }

  if( CSD_EOK == rc )
  {
    /* Empty record found and valid profile */
    wcd_rc = wcd_open(&(adie->usecase_handler));
    if( E_WCD_SUCCESS != wcd_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_create_adie_device_record: "
              "Wcd open failed [%d]", wcd_rc);
      adie->usecase_handler = NULL;
      rc = CSD_EFAILED;
    }
  }

  if( CSD_EOK == rc )
  {
    /* Create new adie device record */
    adie->device_id = dev->dev_id;
    adie->ref_count = 1;
    adie->codec_path_id = path_id;
    adie->direction = csd_dev_codec_adie_get_path_dir_type(
                                                  dev->dev_info.direction);
    adie->sample_rate = dev->curr_attrib.sample_rate;
    adie->bitwidth = dev->curr_attrib.bits_per_sample;
    adie->codec_interface = csd_query_codec_interface_type();
    adie->codec_route_info = NULL;
    adie->afe_port_id = dev->dev_info.afe_port_id;
  }

  if( CSD_EOK == rc )
  {
    /* verify if the device requires to use a codec interface or not */
    switch( dev->dev_info.interface_type )
    {
      case ACDB_DEVICE_TYPE_I2S_PARAM:
        if( adie->codec_interface != E_WCD_I2C )
        {
          /*if the interface is not correct return an error*/
          CSD_MSG(CSD_PRIO_ERROR, "csd_create_adie_device_record: "
                  "lpaif interface (I2S) and "
                  "codec interface (%d) do not match", adie->codec_interface);
          rc = CSD_EBADSTATE;
        }
      break;
      case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
        if( adie->codec_interface != E_WCD_SLIMBUS )
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_create_adie_device_record: "
                  "lpaif interface (Slimbus) and "
                  "codec interface (%d) do not match", adie->codec_interface);
          rc = CSD_EBADSTATE;
        }
      break;
      default:
        CSD_MSG(CSD_PRIO_ERROR, "csd_create_adie_device_record: "
                "device interface (%d) not supported on codec",
                dev->dev_info.interface_type);
        rc = CSD_EBADSTATE;
      break;
    } /*end switch*/
    if( CSD_EOK != rc)
    {
      CSD_MSG(CSD_PRIO_HIGH, "csd_create_adie_device_record: "
              "Verification of dev type[%d] with codec type[%d] "
              "Failed! Clearing the created record",
              dev->dev_info.interface_type,adie->codec_interface);
      csd_initialize_adie_device_record(adie);
    }
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_create_adie_device_record: Exiting [%x]",rc);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_get_adie_from_list

 DESCRIPTION   Search for device record with device_id

 DEPENDENCIES

 PARAMETERS    @param: device_id

 RETURN VALUE  index for matching device_id else -1

 SIDE EFFECTS  none

 =============================================================================*/
static csd_adie_dev_record_t* csd_dev_get_adie_from_list(uint32_t codec_path_id,
                                                         uint16_t afe_port_id)
{
  int32_t iter = 0;
  csd_adie_dev_record_t* adie = NULL;
  while( iter < CSD_DEV_ADIE_CODEC_MAX_RECORDS)
  {
    if((codec_path_id == adie_device_record[iter].codec_path_id) &&
       (afe_port_id == adie_device_record[iter].afe_port_id) )
    {
      CSD_MSG(CSD_PRIO_LOW, "csd_dev_get_adie_from_list: index =%x\n", iter);
      adie = &adie_device_record[iter];
      break;
    }
    iter++;
  }
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_get_adie_from_list: adie =%p\n", ((void*)adie));
  return adie;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_set_profile

 DESCRIPTION   Set profile for codec using acdb codec profile

 DEPENDENCIES

 PARAMETERS    @param: *usecase_handler - usecase reference from codec
               @param: *codec_profile - reference to codec profile from acdb

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_dev_codec_set_profile(void* usecase_handler,
                                      Acdb_WCD93xx_B_CodecProfile* codec_profile)
{
  int32_t rc = CSD_EOK;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  wcd_ioctl_cmd_set_profile profile;

  if( NULL == codec_profile || NULL == usecase_handler )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_set_profile: "
            "Bad param codec_profile[%p],usecase_handler[%p]",
            ((void*)codec_profile), ((void*)usecase_handler));
    rc = CSD_EBADPARAM;
  }
  if( CSD_EOK == rc )
  {
    profile.command_count = codec_profile->header.num_actions;
    profile.acdb_commands = codec_profile->command;
//    profile.version = basic_codec_info.version;
    // Temp Fix to get bring up working, need official fix from ACDB
    profile.version = 1;

    wcd_rc = wcd_ioctl(usecase_handler, WCD_IOCTL_SET_PROFILE,
                       (void*) &profile);
    if( wcd_rc != E_WCD_SUCCESS )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_set_profile: "
              "wcd set profile failed err = %d", wcd_rc);
      rc = CSD_EFAILED;
    }
  }

  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_get_adie_record

 DESCRIPTION

 DEPENDENCIES

 PARAMETERS    @param dev : a device record
               @param adie_record : reference pointer used to obtain adie record

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_dev_get_adie_record(const csd_dev_record_t* dev,
                                       csd_adie_dev_record_t** adie)
{
  int32_t rc = CSD_EOK;
  if( (NULL != dev) && (NULL != adie) )
  {
    uint32_t codec_path_id = 0;
    *adie = NULL;

     /* Get the CodecPathId */
    rc = csd_dev_get_codec_path_id(dev, &codec_path_id);
    if(CSD_EOK == rc)
    {
      *adie = csd_dev_get_adie_from_list(codec_path_id,
                                                dev->dev_info.afe_port_id);
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_get_adie_record: "
            " NULL Param dev[%p], adie_record[%p]",
            ((void*)dev), ((void*)adie));
    rc = CSD_EBADPARAM;
  }

  return rc;
}


/*=============================================================================

 FUNCTION      csd_dev_codec_get_profile

 DESCRIPTION   Set profile for codec using acdb codec profile

 DEPENDENCIES

 PARAMETERS   adie: pointer to adie record entry
              adie_profile_data: Address of output buffer where profile data
              will be stored, caller does not allocate this memory but
              is responsible to delete it. 

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  

 =============================================================================*/
static int32_t csd_dev_codec_get_profile(uint32_t codec_path_id, 
                                Acdb_WCD93xx_B_CodecProfile **adie_profile_data)
{
  int32_t rc = CSD_EOK;

  if(NULL != adie_profile_data)
  {
    Acdb_WCD93xx_B_CodecProfile* codec_profile_buf = NULL;
    uint32_t codec_param_id = 0;
    if( NULL != *adie_profile_data )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_profile: "
              "Passing non empty pointer, avoiding overwrite");
      return CSD_EBADPARAM;
    }
    
    rc = csd_adie_get_acdb_codec_adie_profile_param_id(&codec_param_id);
    if(CSD_EOK != rc)
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_profile: "
              "Failed to retrieve PID for Querying ADIE Profile"
              "with Err=[0x%x]!!!", rc);
      return rc;
    }

    csd_malloc(sizeof(Acdb_WCD93xx_B_CodecProfile),
                                    (void**)&codec_profile_buf);
	if(codec_profile_buf == NULL) 
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_profile: "
              "Failed to allocate memory size of (%d)!",
              CSD_ACDB_DEFAULT_CODEC_PATH_SIZE);
      return CSD_ENORESOURCE;
    }

    rc = csd_acdb_get_adie_codec_profile(codec_path_id, codec_param_id,
                                         (uint8_t*)codec_profile_buf);
    if(CSD_EOK == rc)
    {
      adie_codec_acdb_header* profile_header = &(codec_profile_buf->header);
      *adie_profile_data = codec_profile_buf;
      CSD_MSG(CSD_PRIO_LOW,"csd_dev_codec_get_profile: "
              "Retrieved (CODEC_PROFILE) with num_actions=[%lu], "
              "version=[%lu]!",
              profile_header->num_actions, profile_header->version);
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_profile: "
              "Failed to Retrieve (CODEC_PROFILE) for "
              "CodecPathID=[0x%x], codec_pid=[%x]!",
              codec_path_id, codec_param_id);

      csd_free((void*)(codec_profile_buf));
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_profile: "
            "Invalid Parameters adie_profile_data[%p]",
            ((void*)adie_profile_data));
  }
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_wcd_init

 DESCRIPTION   Helper function for initailizing codec

 DEPENDENCIES  static variable codec_wcd_init 

 PARAMETERS    strict : if false, initailization is attempted not mandatory,
                        Non-critical errors are ignored. 

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  

 =============================================================================*/
static int32_t csd_dev_codec_wcd_init(bool_t strict)
{
  uint32_t rc = CSD_EOK;  
  wcd_result wcd_rc = E_WCD_SUCCESS;
  wcd_init_param_v1_1 wcd_init_param;

  if( TRUE == codec_wcd_init )
  {
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_wcd_init: "
            "skipping, already initailized");
    return CSD_EOK;
  }

  /* Enable MCLK SOURCE via tgt based clk control */
  rc = csd_dev_clk_codec_mclk_control(TRUE);
  //CSD_MSG(CSD_PRIO_ERROR,"Custom Message : GPIO MCLK source called Err=[0x%x]", rc);
  if( CSD_EOK != rc )
  {
    if( strict )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_wcd_init: "
              "Failed to enable MCLK Source!! Err=[0x%x]", rc);
    }
    else
    {
      /* 
       * As MCLK control is via LPASS, it is possible that MCLK couldn't be
       * enabled during boot. Hence, if there is no strict request to 
       * initailize, we can postpone initailizing codec
       */
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_codec_wcd_init: "
              "Skipping wcd init as MCLK isn't available");
      return CSD_EOK;
    }
  }

  //if( CSD_EOK == rc )
  //{ // PMIC voting for codec
  //  rc = csd_dev_pmic_enable_basic_codec_rails();   
  //}

  if( CSD_EOK == rc )
  { //register cb for codec reset
    struct wcd_client_intf_cb wcd_cb;
    wcd_cb.cb = csd_dev_wcd_cb;
    wcd_cb.cb_data = NULL;
    rc = wcd_ioctl(NULL, WCD_IOCTL_CMD_SET_GPIO_INTF_CB, &wcd_cb);  
  }

  if( CSD_EOK == rc )
  { // Enable MCLK GPIO 
    //CSD_MSG(CSD_PRIO_ERROR,"Custom Message : GPIO MCLK called Err=[0x%x]", rc);
    rc = csd_dev_tlmm_codec_mclk_gpio_control(TRUE);
  }

 // rc = csd_dev_tlmm_cdc_ldo_gpio_control(TRUE);


  if( CSD_EOK == rc )
  {
    csd_memset( &wcd_init_param, 0, sizeof(wcd_init_param));
    wcd_init_param.MajorVersion = 1;
    wcd_init_param.MinorVersion = 1;
    wcd_init_param.IHGpio = 0;
    wcd_init_param.bustype = (wcd_bus_type)csd_query_codec_interface_type();

    if(E_WCD_SLIMBUS == wcd_init_param.bustype)
    {
      wcd_init_param.mclk_speed = E_WCD_9_6_MHZ;
    }
    else if(E_WCD_I2C == wcd_init_param.bustype)
    {
      wcd_init_param.mclk_speed = E_WCD_9_6_MHZ;
    }

    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_codec_wcd_init: calling wcd_init, MajorVersion[%d], MinorVersion[%d], bustype[%d], mclk_spped[%d]",
             wcd_init_param.MajorVersion, wcd_init_param.MinorVersion, wcd_init_param.bustype, wcd_init_param.mclk_speed );
             
    wcd_rc = wcd_init( &wcd_init_param );
    if( wcd_rc != E_WCD_SUCCESS )
    {      
      rc = CSD_EBADSTATE;
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_codec_wcd_init: wcd_init failed, wcd_rc[%d]", wcd_rc );
    }
    else
    {
      csd_memset(&basic_codec_info,0,sizeof(basic_codec_info));
      wcd_rc = wcd_ioctl( NULL, WCD_IOCTL_GET_CODEC_INFO, (void*)&basic_codec_info );
      if( E_WCD_SUCCESS != wcd_rc )
      {
        rc = CSD_EFAILED;
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_wcd_init: WCD_IOCTL_GET_CODEC_INFO failed, wcd_rc[%d]", wcd_rc );
      }
            
      /* Possible we can populate the required codec related acdb param ids here
         and check for errors at boot time */
    }
  }

  /* Disable MCLK SOURCE via tgt based clk control */
  (void) csd_dev_clk_codec_mclk_control( FALSE );

  /* Disable MCLK GPIO SOURCE via tgt based clk control */
  (void) csd_dev_tlmm_codec_mclk_gpio_control( FALSE );

// (void) csd_dev_tlmm_cdc_buck_gpio_control( FALSE );

   (void) csd_dev_pmic_disable_basic_codec_rails();  

  if( CSD_EOK == rc )
  {
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_wcd_init: codec initialized" );
    codec_wcd_init = TRUE;
  }

  return rc;
}

/*=============================================================================

FUNCTION      csd_dev_codec_get_sink_paths

DESCRIPTION   Get Sink Paths for a given codec profile.

DEPENDENCIES

PARAMETERS   codec_path_id: codec path id
                 sink_info: Address of output buffer where sink data
                            will be stored, caller allocates this memory.  

RETURN VALUE  csd return status @see csd_status.h

SIDE EFFECTS  

=============================================================================*/
int32_t csd_dev_codec_get_sink_paths(uint32_t codec_path_id,
                                     device_sink_info_t* sink_info)
{
  int32_t rc = CSD_EOK;
  Acdb_WCD93xx_B_CodecProfile* codec_profile = NULL;

  if (sink_info == NULL)
  {
    return CSD_EBADPARAM;
  }

  rc = csd_dev_codec_get_profile(codec_path_id, &codec_profile);
  if(rc == CSD_EOK)
  {
    int32_t i = 0;
    sink_info->number_of_paths = codec_profile->header.number_of_paths;

    for (i=0; i<=codec_profile->header.number_of_paths; i++ )
    {
      sink_info->sinks[i] =  codec_profile->header.sinks[i];
    }
  }
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_init_path

 DESCRIPTION   Initializes the memory structures and assigns a AFEport ID for
               slimbus devices

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_init_path(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  csd_adie_dev_record_t* adie = NULL;
  uint32_t codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;

  CSD_MSG(CSD_PRIO_LOW, "Entering");

  /* Check oem override and update rc */
  if(csd_dev_oem_override(CSD_DEV_OEM_MSG_INIT_CODEC_PATH,(void *) dev,&rc))
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_init_path: "
            "OEM Override occurred csd rc[%d]", rc);
    return rc; 
  }

  if( codec_not_present )
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_init_path: "
            "Codec not present skipping");
    return rc;
  }

  if( FALSE == codec_wcd_init )
  {
    /* Ensure codec was initailized before proceeding to initializing path */
    rc = csd_dev_codec_wcd_init( TRUE );
  }

  if( NULL == dev )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_init_path: NULL DEVICE");
    rc = CSD_EBADPARAM;
  }

  if( CSD_EOK == rc )
  {
    /* Get the CodecPathId */
    rc = csd_dev_get_codec_path_id(dev, &codec_path_id);
  }

  if( CSD_EOK == rc && CSD_DEV_INVALID_CODEC_PATH_ID == codec_path_id )
  {
    /* All remaining steps requires path id to be valid */
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_init_path: "
            "No codec profile, skipping");
  }

  if( CSD_EOK == rc && CSD_DEV_INVALID_CODEC_PATH_ID != codec_path_id )
  {
    adie = csd_dev_get_adie_from_list(codec_path_id, dev->dev_info.afe_port_id);
    if( NULL == adie )
    {
      /* Create a new record */
      rc = csd_create_adie_device_record(dev,codec_path_id);
    }
    else
    {
      adie->ref_count++;
      CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_init_path: "
              "Reference count increase [%d] for unique "
              "afe_port_id,codec_path_id([%d],[%d])",
              adie->ref_count,adie->afe_port_id,adie->codec_path_id);
    }
  }
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_init_path: Exiting [%x]",rc);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_deinit_path(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  uint32_t codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;
  uint16_t afe_port_id = CSD_DEV_INVALID_AFE_PORT_ID;

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_deinit_path: Entering");
  //CSD_MSG(CSD_PRIO_ERROR, "Done REGISTER_DUMP_GIVEN:e");
  //wcd_ioctl(NULL, WCD_IOCTL_REGISTER_DUMP_TO_LOG, NULL);

  /* Check oem override and update rc */
  if( csd_dev_oem_override(CSD_DEV_OEM_MSG_DEINIT_CODEC_PATH, (void *)dev, &rc))
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_deinit_path: "
            "OEM Override occurred csd rc[%d]", rc);
    return rc;
  }

  if( codec_not_present )
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_deinit_path: "
            "Codec not present skipping");
    return rc;
  }

  if( NULL == dev )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_deinit_path: NULL DEVICE");
    rc = CSD_EBADPARAM;
  }

  if( CSD_EOK == rc )
  {
    /* Getting pathid */
    afe_port_id = dev->dev_info.afe_port_id;
    rc = csd_dev_get_codec_path_id(dev, &codec_path_id);
  }

  if( CSD_EOK == rc && CSD_DEV_INVALID_CODEC_PATH_ID == codec_path_id )
  {
    /* All remaining steps requires path id to be valid */
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_deinit_path: "
            "No codec profile, skipping");
  }

  if( CSD_EOK == rc && CSD_DEV_INVALID_CODEC_PATH_ID != codec_path_id )
  {
    /* Deleting valid record based on reference count */
    rc = csd_delete_adie_device_record(codec_path_id, afe_port_id);
  }

  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_deinit_path: "
            "Failed for with codec_path_id(%x),afe_port_id(%x)",
            codec_path_id,afe_port_id);
    rc = CSD_EFAILED;
  }

  CSD_MSG(CSD_PRIO_LOW, "Exiting [%x]",rc);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_procure_path

 DESCRIPTION   reserve resources for usage with WCD driver, this function is
               required since wcd driver does not keep track of the current
               connections and the used configurations.  This function reserves
               the resources to be used with WCD driver, it can be used with
               Slimbus and I2C/I2S devices

 DEPENDENCIES

 PARAMETERS    @param dev : a device record

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_procure_path(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  Acdb_WCD93xx_B_CodecProfile* codec_profile = NULL;
  csd_adie_dev_record_t* adie = NULL;
  bool_t procure_path = FALSE;

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_procure_path: Entering");

  if(csd_dev_oem_override(CSD_DEV_OEM_MSG_DEINIT_CODEC_PATH,(void *) dev,&rc))
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_procure_path: "
            "OEM Override occurred csd rc[%d]", rc);
    return rc;
  }

  if( codec_not_present )
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_procure_path: "
            "Codec not present skipping");
    return rc;
  }

  if( CSD_EOK == rc )
  {
    /* Obtain adie record after validating afe port id and codec path id */
    rc = csd_dev_get_adie_record(dev,&adie);
  }

  if( CSD_EOK == rc && NULL != adie)
  {
    if( adie->ref_count > 1 )
    {
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_procure_path: "
              "path(0x%x) is already configured, ref_cnt=[%d]",
              adie->codec_path_id, adie->ref_count);
    }
    else
    {
      procure_path = TRUE;
    }
  }

  if( CSD_EOK == rc && procure_path )
  {
    /* Get codec profile from ACDB */
    rc = csd_dev_codec_get_profile(adie->codec_path_id, &codec_profile );
  }

  if( CSD_EOK == rc && procure_path )
  {
    /* Set the Profile Data to Codec */
    rc = csd_dev_codec_set_profile(adie->usecase_handler, codec_profile);
  }

  if( CSD_EOK == rc && procure_path )
  {
    /* Profile available, configuring codec */

    wcd_ioctl_cmd_connect_sources_to_sinks sink_source_conns;
    uint32_t curr_path = 0;

    /* Setup Paths (SOURCE<-->SINK Connection details) on the codec */
    csd_memset( &sink_source_conns, 0, sizeof(sink_source_conns));
    sink_source_conns.number_of_connections =
        codec_profile->header.number_of_paths;
    while(curr_path < sink_source_conns.number_of_connections)
    {
      sink_source_conns.sources[curr_path] =
          codec_profile->header.sources[curr_path];
      sink_source_conns.sinks[curr_path] =
          codec_profile->header.sinks[curr_path];
      sink_source_conns.connection_types[curr_path] =
          codec_profile->header.connection_type[curr_path];
      curr_path++;
    }
    CSD_MSG( CSD_PRIO_LOW, "csd_dev_codec_procure_path: "
            "calling WCD_IOCTL_CONNECT_SOURCES_TO_SINKS "
            "with num_of_conn=[%lu]!",
            sink_source_conns.number_of_connections);
    wcd_rc = wcd_ioctl(adie->usecase_handler,
                       WCD_IOCTL_CONNECT_SOURCES_TO_SINKS,
                       (void*) &sink_source_conns);
    if( E_WCD_SUCCESS != wcd_rc )
    {
      rc = CSD_EFAILED;
      CSD_MSG( CSD_PRIO_ERROR,"csd_dev_codec_procure_path: "
               "WCD_IOCTL_CONNECT_SOURCES_TO_SINKS Failed with Err=[%d]!",
               wcd_rc);
    }
  }

  if( CSD_EOK == rc && procure_path )
  {
    adie->step = CSD_STEP_INITIALIZED;
  }

  if(codec_profile)
  {
    /* Freeing codec profile if used */
    csd_free(codec_profile);
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_procure_path: Exiting [%x]",rc);

  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_release_path

 DESCRIPTION   Release the path in codec which was enabled in
               @see csd_dev_codec_procure_path

 DEPENDENCIES

 PARAMETERS    @param dev : Reference to device

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_release_path(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  csd_adie_dev_record_t* adie = NULL;
  uint32_t codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;

  /* Check oem override and update rc */
  if(csd_dev_oem_override(CSD_DEV_OEM_MSG_RELEASE_CODEC_PATH,(void *)dev,&rc))
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_release_path: "
            "OEM Override occurred csd rc[%d]", rc);
    return rc;
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_release_path: Entering");

  if( codec_not_present )
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_release_path: "
            "Codec not present skipping");
    return rc;
  }

  if( CSD_EOK == rc )
  {
    /* Obtain adie record after validating afe port id and codec path id */
    rc = csd_dev_get_adie_record(dev,&adie);
  }

  if( CSD_EOK == rc && NULL != adie )
  {
    if( adie->ref_count > 1)
    {
        CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_codec_release_path: "
                "Adie path(0x%x) has other active devices!! ref_cnt=[%d]!",
                codec_path_id, adie->ref_count);
    }
    else
    {
      CSD_MSG(CSD_PRIO_LOW,"csd_dev_codec_release_path: "
              "calling WCD_IOCTL_DISCONNECT_CONNECTIONS"
              "for adie path(0x%x)",
              codec_path_id);
      wcd_rc = wcd_ioctl(adie->usecase_handler,
                         WCD_IOCTL_DISCONNECT_CONNECTIONS, NULL );
      if( E_WCD_SUCCESS != wcd_rc )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_release_path: "
                "WCD_IOCTL_DISCONNECT_CONNECTIONS Failed "
                "with Err=[%d]", wcd_rc);
        rc = CSD_EFAILED;
      }
    }
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_release_path: Exiting [%x]",rc);
  return rc;
}


/*=============================================================================

 FUNCTION      csd_dev_

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_enable(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  csd_adie_dev_record_t* adie = NULL;
  uint32_t codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;
  bool_t enable_codec = FALSE;

  /* Check oem override and update rc */
  if( csd_dev_oem_override(CSD_DEV_OEM_MSG_ENABLE_CODEC, (void *) dev, &rc) )
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_enable: "
            "OEM Override occurred csd rc[%d]", rc);
    return rc;
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_enable: Entering");

  if( codec_not_present )
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_enable: "
            "Codec not present skipping");
    return rc;
  }

  if( CSD_EOK == rc )
  {
    /* Obtain adie record after validating afe port id and codec path id */
    rc = csd_dev_get_adie_record(dev,&adie);
  }

  if( CSD_EOK == rc && NULL != adie )
  {
    if(adie->ref_count > 1)
    {
        CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_codec_enable: "
                "Adie path(0x%x) is already configured!! "
                " ref_cnt=[%d]!", codec_path_id, adie->ref_count);
    }
    else
    {
      /* Flag for all the remaining codec related steps */
      enable_codec = TRUE;
    }
  }

  if( CSD_EOK == rc && enable_codec )
  {
    wcd_ioctl_cmd_enable_usecase wcd_usecase_cmd;
    /*  Enable the Codec */
    csd_memset( &wcd_usecase_cmd, 0, sizeof(wcd_ioctl_cmd_enable_usecase));
    wcd_usecase_cmd.sample_rate = adie->sample_rate;
    wcd_usecase_cmd.bit_width = adie->bitwidth;
    wcd_rc = wcd_ioctl(adie->usecase_handler, WCD_IOCTL_ENABLE_USECASE,
                       (void*) &wcd_usecase_cmd);
    if( wcd_rc != E_WCD_SUCCESS )
    {
      rc = CSD_EFAILED;
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_enable: "
              "WCD_IOCTL_ENABLE_USECASE for path id(0x%x) "
      "Failed with wcd rc =[%d]",
              codec_path_id, wcd_rc);
    }
  }

  if( CSD_EOK == rc && enable_codec &&
      ACDB_DEVICE_TYPE_SLIMBUS_PARAM == dev->dev_info.interface_type )
  {
    /*
     * For Slimbus Device Interface Set up the Shared channels
     * on WCD if the information is read from ACDB
     */
    int32_t i = 0;
    wcd_ioctl_cmd_slimbus_intf_config sb_config;

    csd_memset( &sb_config, 0, sizeof(wcd_ioctl_cmd_slimbus_intf_config));
    sb_config.num_of_channels = dev->dev_info.channel_config;
    sb_config.direction = (wcd_slimbus_direction)adie->direction;
    for( i = 0; i < sb_config.num_of_channels; i++ )
    {
      sb_config.shared_channel_num[i] =
                         dev->dev_info.u.slimbus_dev_info.afe_shared_channel[i];
    }
    CSD_MSG( CSD_PRIO_LOW, "csd_dev_codec_enable: "
              "calling wcd_ioctl(WCD_IOCTL_ENABLE_SLIMBUS_INTF)"
             " for adie path(0x%x)", codec_path_id);
    wcd_rc = wcd_ioctl(adie->usecase_handler, WCD_IOCTL_ENABLE_SLIMBUS_INTF,
                       (void*) &(sb_config));
    if( wcd_rc != E_WCD_SUCCESS )
    {
      rc = CSD_EFAILED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_codec_enable: "
               "wcd_iotcl(WCD_IOCTL_CMD_ENABLE_SLIMBUS_INT) "
               "for CodecPathId=[0x%x] Failed with WcDErr=[0x%x]!!",
               codec_path_id, wcd_rc);
               
      csd_dev_codec_disable(dev);
      
    }
    if(dev->cache_cfg_param.control_params & CSD_DEV_SPKR_PROT_SET)
    {
      /*Configure/Enable the Feed Back Speaker-Tx AFE Port*/
      csd_acdb_dev_info_t tx_dev_info = {0,};
      wcd_ioctl_cmd_enable_fb_spkr_prot wcd_fp_config = {0,};

      rc = csd_dev_acdb_get_dev_info( dev->cache_cfg_param.spkr_calib_cfg.fb_tx_dev_id[0],
                                      (void*)&tx_dev_info);

      wcd_fp_config.shared_channel_num_v =
                           tx_dev_info.u.slimbus_dev_info.afe_shared_channel[0];
      wcd_fp_config.shared_channel_num_i =
                           tx_dev_info.u.slimbus_dev_info.afe_shared_channel[1];

      CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_enable: "
              "Calling wcd_ioctl(WCD_IOCTL_ENABLE_FB_SPKR_PROT) "
              "for adie path(0x%x)", codec_path_id);
      CSD_MSG(CSD_PRIO_LOW, "Slimbus Shared Channels V(%d) - I(%d)",
              wcd_fp_config.shared_channel_num_v,
              wcd_fp_config.shared_channel_num_i);
      wcd_rc = wcd_ioctl( adie->usecase_handler, WCD_IOCTL_ENABLE_FB_SPKR_PROT,
                          (void*)&(wcd_fp_config) );
      if( wcd_rc != E_WCD_SUCCESS )
      {
        rc = CSD_EFAILED;
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_enable: "
                "wcd_iotcl(WCD_IOCTL_ENABLE_FB_SPKR_PROT) for "
                "CodecPathId=[0x%x] Failed with WcDErr=[0x%x]!!",
                codec_path_id, wcd_rc);
        
        /* Should this failure result in use case failure */
        csd_dev_codec_disable(dev);
      }

    }
  }


  if(CSD_EOK == rc && enable_codec )
  {
    adie->step = CSD_STEP_ENABLED;
  }

 CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_enable: Exiting [%x]",rc);
  //CSD_MSG(CSD_PRIO_ERROR, "Done REGISTER_DUMP_GIVEN:e");
  //wcd_ioctl(NULL, WCD_IOCTL_REGISTER_DUMP_TO_LOG, NULL);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_disable

 DESCRIPTION   Disable codec path for a device

 DEPENDENCIES

 PARAMETERS    @param dev : reference to a device record

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_disable(const csd_dev_record_t *dev)
{
  int32_t rc = CSD_EOK;
  csd_adie_dev_record_t* adie = NULL;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  bool_t disable_codec = FALSE;
  uint32_t codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;

  /* Check oem override and update rc */
  if( csd_dev_oem_override(CSD_DEV_OEM_MSG_ENABLE_CODEC, (void *) dev, &rc) )
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_disable: "
            "OEM Override occurred csd rc[%d]", rc);
    return rc;
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_disable: Entering");

  if( codec_not_present )
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_disable: "
            "Codec not present skipping");
    return rc;
  }

  if( CSD_EOK == rc )
  {
    /* Obtain adie record after validating afe port id and codec path id */
    rc = csd_dev_get_adie_record(dev,&adie);
  }

  if( CSD_EOK == rc && NULL != adie )
  {
    if( adie->ref_count > 1 )
    {
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_disable: "
              "Adie path(0x%x) is already configured!! "
      " ref_cnt=[%d]!",
              codec_path_id, adie->ref_count);
    }
    else
    {
      /* Flag for all the remaining codec related steps */
      disable_codec = TRUE;
    }
  }

  if( CSD_EOK == rc && disable_codec )
  {
    if(dev->cache_cfg_param.control_params & CSD_DEV_SPKR_PROT_SET)
    {
      CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_disable: "
              "wcd_ioctl(WCD_IOCTL_DISABLE_FB_SPKR_PROT) for "
              "adie path(0x%x)", codec_path_id);
      wcd_rc = wcd_ioctl( adie->usecase_handler, WCD_IOCTL_DISABLE_FB_SPKR_PROT, NULL );
      if( wcd_rc != E_WCD_SUCCESS )
      {
        rc = CSD_EFAILED;
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_disable: "
                "wcd_iotcl(WCD_IOCTL_DISABLE_FB_SPKR_PROT) for "
                "CodecPathId=[0x%x] Failed with WcDErr=[0x%x]!!",
                codec_path_id, wcd_rc);
      }
    }

    /* Disable the Codec Now */
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_disable: "
            "calling WCD_IOCTL_DISABLE_USECASE) for path(0x%x)",
            codec_path_id);
    wcd_rc = wcd_ioctl( adie->usecase_handler ,WCD_IOCTL_DISABLE_USECASE, NULL);
    if( wcd_rc != E_WCD_SUCCESS )
    {
        rc = CSD_EFAILED;
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_disable: "
                "WCD_IOCTL_DISABLE_USECASE for path(0x%x) "
                "Failed in codec_disable with WcdErr=[0x%x]",
                codec_path_id, wcd_rc);
    }
  }
  
  if( disable_codec )
  {
    adie->step = CSD_STEP_INITIALIZED;
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_disable: Exiting [%x]",rc);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_init

 DESCRIPTION   Initailize code based on hardware configuration

 DEPENDENCIES  n/a

 PARAMETERS    n/a

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_init(void)
{
  int32_t rc = CSD_EOK;
  uint32_t counter = 0;
  csd_adie_dev_record_t* adie = NULL;
  bool_t configure_codec = TRUE;

  /* Check oem override and update rc */
  if(csd_dev_oem_override(CSD_DEV_OEM_MSG_INIT_CODEC,NULL,&rc))
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_init: "
            "OEM Override occurred csd rc[%d]", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
       //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_OVERRIDE);
    return rc;
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_init: Entering");

  /* Initialise records - as the first step */
  for( counter = 0; counter < CSD_DEV_ADIE_CODEC_MAX_RECORDS; counter++ )
  {
    adie = &adie_device_record[counter];
    if( NULL != adie )
    {
      csd_initialize_adie_device_record(adie);
    }
  }

  /* WCD Configuration needs to proceed only if wcd codec is present */
  configure_codec = csd_dev_is_wcd_codec_present();

  if( FALSE == configure_codec )
  {
    /* Do not configure external wcd codec  */
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_codec_init: "
            "Not configuring codec based on config flag");
    codec_not_present = TRUE;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,CSD_EUNSUPPORTED,CSD_DEV_INIT_STAGE,
       //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_PRESENT);
    return rc;
  }
  else
  {
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
       //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_PRESENT);
  }

  if( CSD_EOK == rc )
  {
    /* Initialize pmic */
    rc = csd_dev_pmic_init();
    if( CSD_EOK != rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_init: "
              "Failed to init PMIC!! Err=[0x%x]", rc);
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_PMIC);
  }

  if( CSD_EOK == rc )
  {
    /* Initailzie wcd codec, non-strict attempt */
    rc = csd_dev_codec_wcd_init( FALSE );
    if( CSD_EOK != rc )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_init: "
              "Failed to init wcd codec!! Err=[0x%x]", rc);
      rc = CSD_EOK; /* non-strict attempt, failure is acceptable */
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_WCD_INIT);
  }
  
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_init: Exiting [%x]",rc);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_deinit(void)
{
  int32_t rc = CSD_EOK;
  wcd_result wcd_ret = E_WCD_SUCCESS;

  /* Check oem override and update rc */
  if(csd_dev_oem_override(CSD_DEV_OEM_MSG_DINIT_CODEC,NULL,&rc))
  {
    /* OEM Occured */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_deinit: "
            "OEM Override occurred csd rc[%d]", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_OVERRIDE); 

    return rc;
  }

  CSD_MSG(CSD_PRIO_HIGH, "csd_dev_codec_deinit: Entering");

  /* Reset codec initailized flag */
  codec_wcd_init = FALSE;

  wcd_ret = wcd_deinit();
  if( E_WCD_SUCCESS != wcd_ret )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_deinit: "
            "Codec de_init failed :%d", wcd_ret);
    rc = CSD_EFAILED;
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,wcd_ret,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_WCD_INIT); 


  /*  Take Codec back to Reset by releasing the codec rest pin */
  rc = csd_dev_release_reset_codec_gpio();
  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_deinit: "
                  "Failed to release reset CODEC!! Err=[0x%x]",
                  rc);
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_RESET_CODEC); 



  if( CSD_EOK != csd_dev_pmic_deinit() )
  {
    CSD_MSG(CSD_PRIO_HIGH, "csd_dev_codec_deinit: "
            " pmic deinit failed");
    rc = CSD_EFAILED;
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEV_INIT_STAGE,
         //CSD_INIT_DEV_TARGET_INIT,CSD_INIT_DEV_TARGET_CODEC_INIT,CSD_INIT_DEV_TARGET_CODEC_PMIC); 


  /* Disable MCLK SOURCE via tgt based clk control */
  (void) csd_dev_clk_codec_mclk_control( FALSE );

  /* Disable MCLK GPIO SOURCE via tgt based clk control */
  (void) csd_dev_tlmm_codec_mclk_gpio_control( FALSE );
  
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_deinit Exiting [%x]",rc);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_get_anc_db

 DESCRIPTION   Query acdb and fill anc data

 DEPENDENCIES

 PARAMETERS    @param rx_dev_id : rx device id
               @param adie_anc_data : Reference for adie anc data

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_dev_codec_get_anc_db( uint32_t rx_dev_id,
                                         void** adie_anc_data )
{
  int32_t rc = CSD_EOK;
  void* cdc_anc_db_buf = NULL;
  uint32_t buf_size = CSD_ACDB_DEFAULT_CODEC_PATH_SIZE;
  uint32_t codec_pid = 0;
  
  if( NULL == adie_anc_data )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_anc_db: "
            "NULL Params adie_anc_data");
    rc = CSD_EBADPARAM;
    return rc;
  }

  *adie_anc_data = NULL;

  /* Get the ACDB codec PID for the WCD codec in use */
  rc = csd_adie_get_acdb_codec_anc_param_id(&codec_pid);
  if(CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_anc_db: "
            "Failed to get anc_codec_pid, rc=[0x%x]!", rc);
    return rc;
  }

  /* Allocate for ANC DB Buffer */
  csd_malloc(buf_size, (void**)&cdc_anc_db_buf);
  if ( cdc_anc_db_buf == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_anc_db: "
            "Failed to allocate memory of size (%d)!",buf_size);
    rc = CSD_EFAILED;
  }
  else
  {
    csd_memset( cdc_anc_db_buf, 0, buf_size);
    
    /* Retrieve the ANC DB from ACDB */
    rc = csd_acdb_get_adie_codec_anc_db(rx_dev_id, codec_pid,
                                        cdc_anc_db_buf, &buf_size);
    if(CSD_EOK != rc)
    {
      csd_free(cdc_anc_db_buf);
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_anc_db: "
              "Failed to get ancdb from acdb, rc=[0x%x]!",rc);
    }
    else
    {
      *adie_anc_data = cdc_anc_db_buf;
      CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_get_anc_db: "
              "Retrieved anc acdb data of size=[0x%x]!",buf_size);
    }
  }

  return rc;
}


/*=============================================================================

 FUNCTION      csd_dev_codec_set_anc_payload

 DESCRIPTION   Helper function to set the ANC Payload to the Codec

 DEPENDENCIES

 PARAMETERS    @param codec_profile_buf : complete anc payload
               @param codec_anc_db_buf : pointer to anc specific data from acdb
               @param p_anc : command send to wcd module

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
/* */
static int32_t csd_dev_codec_set_anc_payload(void *codec_profile_buf, 
                                             void *codec_anc_db_buf,  
                                             wcd_ioctl_cmd_enable_anc *p_anc)
{
  int32_t rc = CSD_EOK;
  if( (NULL != p_anc) &&
      (NULL != codec_anc_db_buf) &&
      (NULL != codec_profile_buf) )
  {
    adie_codec_acdb_header* header = (adie_codec_acdb_header*)codec_profile_buf;
    p_anc->anc_configuration = (wcd_adie_codec_db_anc_config*)codec_anc_db_buf;
    p_anc->command_count = header->num_actions;
    p_anc->acdb_commands = (wcd_acdb_command*)((uint8_t*)codec_profile_buf
                                              + sizeof(adie_codec_acdb_header));
    p_anc->version = header->version;
    CSD_MSG(CSD_PRIO_LOW,"csd_dev_codec_set_anc_payload: "
            "calling Retrieved (ANC_PROFILE) with "
            "command_count=[%lu], version=[%lu]!",
            p_anc->command_count, p_anc->version);
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_set_anc_payload: "
            "Invalid Parameters codec_profile_buf[%p],"
            "codec_anc_db_buf[%p], p_anc[%p]",
            ((void*)codec_profile_buf), ((void*)codec_anc_db_buf), ((void*)p_anc));
  }
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_adie_anc_control

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_adie_anc_control(const csd_dev_record_t* rx_dev, 
                                       const csd_dev_record_t* tx_dev,
                                       bool_t aanc_enable_mode)
{
  int32_t rc = CSD_EOK;
  csd_adie_dev_record_t* rx_record = NULL;
  wcd_result err = E_WCD_SUCCESS;


  CSD_MSG(CSD_PRIO_LOW,"csd_dev_codec_adie_anc_control: Entering");

  if(codec_not_present)
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_adie_anc_control: "
            "Codec not present, skipping");
    return rc;
  }

  /* Get the adie device record index for Rx Device  */
  rc = csd_dev_get_adie_record(rx_dev,&rx_record);
  if( CSD_EOK != rc || NULL == rx_record )
  {
    if(CSD_EOK == rc)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_adie_anc_control: "
              "Unexpected: got rx_record[%p] while rc=[0x%x]",
              ((void*)rx_record), rc);
      rc = CSD_EUNEXPECTED;
    }
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_adie_anc_control: "
            "Failed to retreive adie record for Rx device!!");
    return rc;
  }

  if(TRUE == aanc_enable_mode)
  {
    wcd_ioctl_cmd_enable_anc wcd_aanc_enable;
    void* codec_anc_db_buf  = NULL;
    Acdb_WCD93xx_B_CodecProfile* codec_profile_buf = NULL;
    uint32_t codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;

    /* Get the adie device record index for Tx Device  */
    rc = csd_dev_get_codec_path_id(tx_dev,&codec_path_id);
    if( CSD_EOK != rc)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_adie_anc_control: "
              "Failed to retrieve codec path from device record for Tx device!!");
      return rc;
    }
    /* Get the profile data for the associated ANC Tx Path */
    rc = csd_dev_codec_get_profile(codec_path_id, &codec_profile_buf);
    if(CSD_EOK != rc)
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_adie_anc_control: "
              "Failed to get Tx Profile Path Data for DevId[%d]"
              " with Err=[0x%x]!", tx_dev->dev_id, rc);
      return rc;
    }
    /* Get the Codec ANC Data */
    rc = csd_dev_codec_get_anc_db(rx_dev->dev_id, &codec_anc_db_buf);
    if(CSD_EOK != rc)
    {
      if(NULL != codec_profile_buf)
      {
        csd_free((void*)(codec_profile_buf));
      }
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_adie_anc_control: "
              "Failed to get codec ANC DB for DevId[%d] with "
              "Err=[0x%x]!", rx_dev->dev_id, rc);
      return rc;
    }
    
    /* Prepare the Codec ANC payload data */
    csd_memset( &wcd_aanc_enable, 0, sizeof(wcd_ioctl_cmd_enable_anc) );
    rc = csd_dev_codec_set_anc_payload(codec_profile_buf, codec_anc_db_buf,
                                       &wcd_aanc_enable );
    
    /* Send ENABLE ANC to codec */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_adie_anc_control: "
            "calling wcd_ioctl(WCD_IOCTL_ENABLE_ANC)!!");
    err = wcd_ioctl( rx_record->usecase_handler, WCD_IOCTL_ENABLE_ANC,
                     &wcd_aanc_enable );
    if( err != E_WCD_SUCCESS )
    {   
      rc = CSD_EFAILED;
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_adie_anc_control: "
              "wcd_iotcl(WCD_IOCTL_ENABLE_ANC) for RxDevice="
              "[0x%x] Failed with WcdErr=[%d]!!", rx_dev->dev_id, err);
    }
    if(NULL != codec_anc_db_buf)
    {
      csd_free((void*)(codec_anc_db_buf));
    }
    if(NULL != codec_profile_buf)
    {
      csd_free((void*)(codec_profile_buf));
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_codec_adie_anc_control: "
            "calling wcd_ioctl(WCD_IOCTL_DISABLE_ANC)!!");
    err = wcd_ioctl( rx_record->usecase_handler, WCD_IOCTL_DISABLE_ANC, NULL );
    if( err != E_WCD_SUCCESS )
    {   
      rc = CSD_EFAILED;
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_adie_anc_control: "
              "wcd_iotcl(WCD_IOCTL_DISABLE_ANC) for RxDevice="
              "[0x%x] Failed with WcdErr=[%d]!!", rx_dev->dev_id, err);
    }
  }
  CSD_MSG(CSD_PRIO_LOW,"csd_dev_codec_adie_anc_control: Exiting rc=[0x%x]", rc);
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_adie_set_anc_state

 DESCRIPTION   Function to retrieve the ADIE CODEC SLIMBUS CONFIG DATA for 
               AFE Service Module

 DEPENDENCIES

 PARAMETERS    @param codec_sb_cfg_data : reference to a pointer
               @param buf_size : return buffer size

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_get_slimbus_cfg_data ( void **codec_sb_cfg_data,
                                             uint32_t *buf_size)
{
  int32_t rc = CSD_EOK;
 /* wcd_codec_enumeration_address *p_wcd_data = NULL;

  if(NULL == codec_sb_cfg_data || NULL == buf_size)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_slimbus_cfg_data: "
            "Null Param codec_sb_cfg_data[%p], buf_size[%p]",
            ((void*)codec_sb_cfg_data),((void*)buf_size));
    return CSD_EBADPARAM;
  }

  p_wcd_data = (wcd_codec_enumeration_address*)wcd_get_codec_enumeration_address();

  if(NULL == p_wcd_data)
  {
    *codec_sb_cfg_data = NULL;
    *buf_size = 0;

    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_get_slimbus_cfg_data: "
            "Enumeration address from wcd is NULL");
    rc = CSD_EUNSUPPORTED;
  }
  else
  {
    *codec_sb_cfg_data = p_wcd_data;
    *buf_size = sizeof(wcd_codec_enumeration_address);
  }
  */
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_pack_slimbus_cfg_data

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
               @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_pack_slimbus_cfg_data(void *codec_data, 
                                            uint32_t codec_data_size, 
                                            afe_cdc_slimbus_slave_cfg_param_v1_t* afe_cfg, 
                                            uint32_t afe_data_size)
{
  int32_t rc = CSD_EOK;
  /*wcd_codec_enumeration_address *wcd_data= NULL;
  
  if( (NULL == afe_cfg) || (NULL == codec_data) )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_pack_slimbus_cfg_data: "
            "Null arguments afe_cfg[%p], codec_data[%p]!!",
            ((void*)afe_cfg), ((void*)codec_data));
    return CSD_EBADPARAM;
  }
  
  if( (sizeof(wcd_codec_enumeration_address) != codec_data_size) )
  {
    CSD_MSG(CSD_PRIO_LOW,"csd_dev_codec_pack_slimbus_cfg_data: "
            "passed with invalid arguments codec_data_size=[%d]",
            codec_data_size);
    return CSD_EBADPARAM;
  }
  
  wcd_data = (wcd_codec_enumeration_address*)codec_data;
  if( NULL != wcd_data )
  {
    afe_cfg->slave_dev_minor_version = AFE_API_VERSION_CDC_SLIMBUS_SLAVE_CFG;
    afe_cfg->device_enum_addr_lsw = wcd_data->device_enum_addr_lsw;
    afe_cfg->device_enum_addr_msw = wcd_data->device_enum_addr_msw;
    afe_cfg->tx_slave_port_offset = wcd_data->tx_slave_port_offset;
    afe_cfg->rx_slave_port_offset = wcd_data->rx_slave_port_offset;
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_pack_slimbus_cfg_data: "
            "has NULL for wcd_data!!");
    rc = CSD_EFAILED;
  }
*/
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_get_aanc_reg_cfg_data

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_get_aanc_reg_cfg_data( void **codec_cfg_data,
                                             uint32_t *codec_reg_count, 
                                             uint32_t *hw_version)
{
  int32_t rc = CSD_EOK;
/*  wcd_register_block_definition *wcd_reg_block = NULL;

  if((NULL == codec_cfg_data) ||
     (NULL == codec_reg_count)||
     (NULL == hw_version))
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_aanc_reg_cfg_data: "
            "Null arguments codec_cfg_data[%p], "
            "codec_reg_count[%p] hw_version[%p]!!",
            ((void*)codec_cfg_data), ((void*)codec_reg_count), ((void*)hw_version));
    return CSD_EBADPARAM;
  }

  wcd_reg_block = (wcd_register_block_definition*)wcd_exposed_register_block(E_WCD_AANC);
  if(NULL == wcd_reg_block)
  {
    *codec_cfg_data = NULL;
    *codec_reg_count = 0;
    *hw_version = 0;
    rc = CSD_EUNSUPPORTED;
  }
  else
  {
    *codec_cfg_data = wcd_reg_block;
    *codec_reg_count = wcd_reg_block->num_elements_in_table;
    *hw_version = wcd_reg_block->hw_version;
  }
*/
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_codec_pack_aanc_cdc_reg_cfg_data

 DESCRIPTION   <>

 DEPENDENCIES

 PARAMETERS    @param <> : <>
 @param  <> : <>

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_pack_aanc_cdc_reg_cfg_data(void *codec_cfg_data,
                                                 afe_cdc_reg_cfg_param_v1_t *afe_cfg_data,
                                                 uint32_t reg_index)
{
  int32_t rc = CSD_EOK;
  /*wcd_register_block_definition *wcd_reg_block = NULL;

  if( (NULL == afe_cfg_data) || (NULL == codec_cfg_data) )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_pack_aanc_cdc_reg_cfg_data: "
            "Null arguments codec_cfg_data[%p], "
            "afe_cfg_data[%p]!!reg_index=[%x]",
            codec_cfg_data, ((void*)afe_cfg_data), reg_index);
    return CSD_EBADPARAM;
  }

  wcd_reg_block  = (wcd_register_block_definition*)codec_cfg_data;

  if(reg_index > wcd_reg_block->num_elements_in_table)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_pack_aanc_cdc_reg_cfg_data: "
            "Input register index=[%u] is greater than expected "
            "register count=[%lu]",
            reg_index, wcd_reg_block->num_elements_in_table);
    rc = CSD_EFAILED;
  }
  else
  {
    afe_cfg_data->cdc_reg_cfg_minor_version = AFE_API_VERSION_CDC_REG_CFG;
    afe_cfg_data->reg_logical_addr    = wcd_reg_block->register_table[reg_index].reg_addr;
    afe_cfg_data->reg_field_type      = wcd_reg_block->register_table[reg_index].type;
    afe_cfg_data->reg_field_bit_mask  = wcd_reg_block->register_table[reg_index].bitmask;
    afe_cfg_data->reg_bit_width       = wcd_reg_block->register_table[reg_index].register_width;
    afe_cfg_data->reg_offset_scale    = wcd_reg_block->register_table[reg_index].offset;
  }
  */
  return rc;
}


/*=============================================================================

 FUNCTION      csd_dev_codec_get_adie_usage_handle

 DESCRIPTION   Get the aide handle for the dev

 DEPENDENCIES

 PARAMETERS    @param dev : dev 
               @param return_handle : the coresponding handle for the dev

 RETURN VALUE  csd return status: 
               CSD_EUNSUPPORTED if codec is not present or any of the
               devices is not an internal codec device.
               CSD_EBADPARAM if incorrect parameter is passed in.
               CSD_EFAILED for any other internal failures
               CSD_EOK on success

 SIDE EFFECTS  none

 =============================================================================*/

int32_t csd_dev_codec_get_adie_usage_handle(const csd_dev_record_t* dev, void **return_handle)
{
  csd_adie_dev_record_t* adie_record = NULL;
  uint32_t codec_path_id = 0;
  int32_t rc = CSD_EOK;

  /* Do some general protections here */
  if(NULL == dev)
  {
    /* Bad params */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_adie_usage_handle: Null Params dev");
    return CSD_EBADPARAM;
  }
  rc = csd_dev_get_codec_path_id(dev, &codec_path_id);
  if(CSD_EOK != rc)
  {
    /* Bad return */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_adie_usage_handle: Cannot get path id[%d]", rc);
    return CSD_EBADPARAM;
  }

  /* Fetch adie records using codec path id and afe port id */
  adie_record = csd_dev_get_adie_from_list(codec_path_id, dev->dev_info.afe_port_id);
  if(NULL == adie_record)
  {
    /* Bad params */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_adie_usage_handle: Null Params adie_record");
    return CSD_EBADPARAM;
  }
  if(NULL == adie_record->usecase_handler)
  {
    /* Bad params */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_adie_usage_handle: Null adie usecase handler");
    return CSD_EFAILED;
  }    

  if(NULL == return_handle)
  {
    /* Bad params */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_get_adie_usage_handle: Null input parameter");
    return CSD_EBADPARAM;
  }
  *return_handle = adie_record->usecase_handler;

  /* Finally, this use case handler is ready to return */
  return CSD_EOK;
}


/*=============================================================================

 FUNCTION      csd_dev_codec_sidetone_ctrl

 DESCRIPTION   Enable/Disable sidetone on codec for given device pair

 DEPENDENCIES

 PARAMETERS    @param rx_dev : Rx device for sidetone 
               @param tx_dev : Tx device for sidetone
               @param sidetone : Sidetone enable/disable settings

 RETURN VALUE  csd return status: 
               CSD_EUNSUPPORTED if codec is not present or any of the
               devices is not an internal codec device.
               CSD_EBADPARAM if incorrect parameter is passed in.
               CSD_EFAILED for any other internal failures
               CSD_EOK on success

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_codec_sidetone_ctrl(const csd_dev_record_t* rx_dev, 
                                    const csd_dev_record_t* tx_dev,
                                    const csd_dev_sidetone_ctrl_t* sidetone)
{
  int32_t rc = CSD_EOK;

  csd_adie_dev_record_t* adie_record_tx = NULL;
  csd_adie_dev_record_t* adie_record_rx = NULL;
  uint32_t codec_path_id_tx = 0;
  uint32_t codec_path_id_rx = 0;

  wcd_adie_codec_db_sidetone_iir_config sidetone_config_buf;

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_sidetone_ctrl: begin");

  if(NULL == rx_dev || NULL == tx_dev || NULL == sidetone)
  {
    /* Bad params */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_codec_sidetone_ctrl: "
            "Null Params rx_dev[%p], tx_dev[%p], sidetone[%p]",
            ((void*)rx_dev), ((void*)tx_dev), ((void*)sidetone));
    return CSD_EBADPARAM;
  }

  csd_memset((uint8_t *)&sidetone_config_buf, 0,
                sizeof(wcd_adie_codec_db_sidetone_iir_config));

  rc = csd_acdb_get_adie_sidetone(sidetone->rx_dev_id,
                                  sidetone->tx_dev_id,
                                  (uint8_t *)&sidetone_config_buf,
                                 sizeof(wcd_adie_codec_db_sidetone_iir_config));
  if(CSD_EOK != rc)
  {
    /* Codec sidetone is unsupported in ACDB
     * CSD could try other alternatives eg:Afe sidetone */
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_sidetone_ctrl: "
            "Can't get ADIE sidetone table from ACDB for "
            "rx_id(%d) tx_id(%d) rc[0x%x]",
            sidetone->rx_dev_id, sidetone->tx_dev_id, rc);
    return CSD_EUNSUPPORTED;
  }

  /* Get the CodecPathId for Rx Device */
  rc = csd_dev_get_codec_path_id(rx_dev, &codec_path_id_rx);
  if (CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_sidetone_ctrl: "
            "Cannot Retrieve CodecPathID for DevID[%d] of "
            "DeviceInterface:[0x%x]",
            rx_dev->dev_id, rx_dev->dev_info.interface_type);
    return rc;
  }

  /* Get the CodecPathId for Tx Device */
  rc = csd_dev_get_codec_path_id(tx_dev, &codec_path_id_tx);
  if (CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_sidetone_ctrl: "
            "Cannot Retrieve CodecPathID for DevID[%d] of "
            "Device Interface:[0x%x] ",
            tx_dev->dev_id, tx_dev->dev_info.interface_type);
    return rc;
  }

  /* check to see if codec sidetone is supported on the given pair
     of devices */
  if(codec_not_present ||
     codec_path_id_tx == CSD_DEV_INVALID_CODEC_PATH_ID ||
     codec_path_id_rx == CSD_DEV_INVALID_CODEC_PATH_ID )
  {
    /* Codec sidetone is unsupported feature if codec isn't used eg:MI2S,PCM,etc
     * CSD could try other alternatives eg:Afe sidetone */
    CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_codec_sidetone_ctrl: "
            "Codec sidetone not supported. Skipping.");
    return CSD_EUNSUPPORTED;
  }

  if( CSD_EOK == rc )
  {
    /* Fetch adie records using codec path id and afe port id */
    adie_record_tx = csd_dev_get_adie_from_list(codec_path_id_tx,
                                                tx_dev->dev_info.afe_port_id);
    adie_record_rx = csd_dev_get_adie_from_list(codec_path_id_rx,
                                                rx_dev->dev_info.afe_port_id);
  }

  /* Validate the adie records and apply the side tone ctrl */
  if((NULL != adie_record_tx) && (NULL != adie_record_rx))
  {
    wcd_result err = E_WCD_SUCCESS;
    wcd_ioctl_cmd_enable_sidetone s_tone = { 0 };
    bool_t enable_cmd = FALSE;

    switch (sidetone->ctrl)
    {
      case CSD_DEV_SIDETONE_ENABLE:
        enable_cmd = TRUE;
        /* dont break here, continue to next case */

      case CSD_DEV_SIDETONE_DEFAULT:
      {
        if(sidetone_config_buf.enable || enable_cmd)
        {
          s_tone.tx_usecase = (void *) adie_record_tx->usecase_handler;
          s_tone.iir_configuration = &sidetone_config_buf;
          s_tone.iir_configuration->enable = 1;

          /* Enable the SideTone on the RX Device ID */
          err = wcd_ioctl(adie_record_rx->usecase_handler,
                          WCD_IOCTL_ENABLE_SIDETONE, (void *) &s_tone);
          if( E_WCD_SUCCESS != err )
          {
            rc = CSD_EFAILED;
            CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_sidetone_ctrl: "
                    "wcd_iotcl(WCD_IOCTL_CMD_ENABLE_SIDETONE)"
                    "Failed with wcd_Err = [%d] ", err);
          }
          break;
        }
      }
      /* only break above if enabling otherwise continue to DISABLE*/
      /* TODO: Unni - I'm concerned, if ACDB's default is disable - I thought
       * the aim was not to enable sidetone. Instead, we would be asking
       * disable adie sidetone. Won't it be problematic to disable something,
       * implicitly - not know whether it was enabled or not. I think
       * acdb DEFAULT - can be a substitute for enable but not disable.
       */

      case CSD_DEV_SIDETONE_DISABLE:
      {
        /* Disable the SideTone on the RX Device ID */
        err = wcd_ioctl(adie_record_rx->usecase_handler, 
                        WCD_IOCTL_DISABLE_SIDETONE, NULL);

        if(E_WCD_SUCCESS != err)
        {
          rc = CSD_EFAILED;
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_sidetone_ctrl: "
                  "wcd_iotcl(WCD_IOCTL_CMD_DISABLE_SIDETONE)"
                  "Failed with wcd_Err = [%d] ", err);
        }
        break;
      }

      default:
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_sidetone_ctrl: "
                "Unsupported options for "
                "CSD_DEV_CMD_SIDETONE_CONTROL!");
        rc = CSD_EBADPARAM;
        break;
      }
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_codec_sidetone_ctrl: "
            "Failed to Retrive Device Record Index for Rx DevID"
            "[%d] or Tx DevID[%d] ", rx_dev->dev_id, tx_dev->dev_id);
    rc = CSD_EFAILED;
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_codec_sidetone_ctrl: end");

  return rc; 
}

int32_t csd_dev_codec_adie_reset(void)
{
  int32_t rc = CSD_EOK;

  CSD_MSG( CSD_PRIO_LOW, "csd_dev_codec_adie_reset" );
#if 0
  if( E_WCD_SUCCESS != wcd_ioctl( NULL, WCD_IOCTL_RESET_CODEC, NULL ) )
  {
    rc = CSD_EFAILED;
  }
#endif

  return rc;
}

wcd_result csd_dev_wcd_cb(void* payload, uint32 payload_size, void* client_data)
{
  wcd_result rc = E_WCD_SUCCESS;
  return rc;
}

