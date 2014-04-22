/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                FTM RFFE Multi-Bytes Read/Write COMMANDS

GENERAL DESCRIPTION
  This is the FTM RFFE file which contains RFFE READ/WRITE mult-bytes commands

Copyright (c) 2015   by QUALCOMM Technologies Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfa.mpss/3.10/rf/common/ftm/src/ftm_rffe_multi.c#1 $
  $DateTime: 2020/04/16 12:27:00 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/09/15   fhuo     Initial revision
===========================================================================*/
#include "rfa_variation.h"
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_rffe_multi.h"  

#include "rf_hal_rffe.h"
#include "msg.h"                     /* Debug related stuff     */
extern void rfhal_cmd_intf_profile(void);


/*===========================================================================
                Define LOCAL variables  
===========================================================================*/


static void ftm_rffe_cmd_multi(ftm_rffe_data_multi_byte_type* rffe_data)
{
  /* Set variables */
  rf_hal_bus_resource_script_settings_type settings;
  rf_hal_bus_rffe_type script;
  rf_hal_bus_result_type err; 
  int32 byte_count;
  uint8 data_size = sizeof(rffe_data->data1);

  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  memset(&script, 0, sizeof(rf_hal_bus_rffe_type));
  settings.half_rate = rffe_data->halfspeed;
  settings.rffe_force_ext = rffe_data->ext;

  /* Set the RFFE transaction settings */ 
  script.addr = rffe_data->address;
  script.channel = rffe_data->channel;
  script.extended_cmd = 1;
  script.half_rate = rffe_data->halfspeed;
  script.num_bytes = rffe_data->size;
  script.rd_delay = 0;
  script.slave_id = rffe_data->slave;
  script.start_delta = 0;
  script.rd_wr = rffe_data->readwrite;

  /*Start of RFFE reading*/
  if (script.rd_wr == 1)
  {
    rffe_data->data1 = 0;
    rffe_data->data2 = 0;
    rffe_data->data3 = 0;
    rffe_data->data4 = 0;
    /* Record the read value*/
#ifndef FEATURE_RFA_ATLAS_MODEM
    err = rfhal_rffe_read_bytes_ext(&script, rffe_data->channel, 1, &settings); 
    if (err == RF_HAL_BUS_SUCCESS)
    {
      byte_count=0;
      /*Group 4 1-byte uint8 data from script.data array and assign to a 4-byte uint32 
        rffe_data->data for result display, do it for as many bytes (but not greater than
        RF_HAL_BUS_RFFE_MAX_BYTES) that needs to be read with one CCS read*/
      while (byte_count< script.num_bytes && byte_count < RF_HAL_BUS_RFFE_MAX_BYTES) 
      {
        if(byte_count/data_size==0)
        {
          rffe_data->data1 |= (uint32)script.data[byte_count]<<(8*(byte_count%data_size)); 
        }
        else if(byte_count/data_size==1)
        {
          rffe_data->data2 |= (uint32)script.data[byte_count]<<(8*(byte_count%data_size)); 
        } 
        else if(byte_count/data_size==2)
        {
          rffe_data->data3 |= (uint32)script.data[byte_count]<<(8*(byte_count%data_size)); 
        } 
        else if(byte_count/data_size==3)
        {
          rffe_data->data4 |= (uint32)script.data[byte_count]<<(8*(byte_count%data_size)); 
        } 
        else
        {
          MSG_ERROR("FTM RFFE_Mult_bytes Read: Unexpected value of byte_count/data_size: %d", byte_count/data_size, 0, 0);
        }
        ++byte_count; 
      } 
      MSG_4(MSG_SSID_FTM, MSG_LEGACY_ERROR,"FTM RFFE_Mult_bytes write: Script data after read : 0 : %d 1 :%d 2 :%d 3 :%d", script.data[0], script.data[1],script.data[2],script.data[3]);
    }
#endif //ATLAS
  }

  /*Start of RFFE writting*/
  else if (script.rd_wr == 0)
  { 
    /* Record the write value*/
#ifndef FEATURE_RFA_ATLAS_MODEM    
    byte_count = 0;
    while (byte_count< script.num_bytes && byte_count < RF_HAL_BUS_RFFE_MAX_BYTES)
    {
      /*Assign each 4-byte uin32 data to the script.data array, each element in 
        the array is a 1-byte uint8, do it for as many bytes (but not greater
        than RF_HAL_BUS_RFFE_MAX_BYTES) that needs to be written with one CCS write*/
      if(byte_count/data_size==0)
      {
        script.data[byte_count] = (uint8)(rffe_data->data1 >> (8*(byte_count%data_size)));  
      }
      else if(byte_count/data_size==1)
      {
        script.data[byte_count] = (uint8)(rffe_data->data2 >> (8*(byte_count%data_size)));  
      }
      else if(byte_count/data_size==2)
      {
        script.data[byte_count] = (uint8)(rffe_data->data3 >> (8*(byte_count%data_size)));  
      }
      else if(byte_count/data_size==3)
      {
        script.data[byte_count] = (uint8)(rffe_data->data4 >> (8*(byte_count%data_size)));  
      }
      else
      {
        MSG_ERROR("FTM RFFE_Mult_bytes write: Unexpected value of byte_count/data_size: %d", byte_count/data_size,0,0);
      }
      ++byte_count; 
    }
    MSG_4(MSG_SSID_FTM, MSG_LEGACY_ERROR,"FTM RFFE_Mult_bytes write: Script data before write : 0 : %d 1 :%d 2 :%d 3 :%d", script.data[0], script.data[1],script.data[2],script.data[3]);
    err = rfhal_rffe_write_bytes_ext(&script, rffe_data->channel, 1, &settings);
#endif //ATLAS  
  }
  else
  {
    MSG_ERROR("FTM RFFE_Mult_bytes: Unexpected read/write option: %d", script.rd_wr,0,0);
  }
}


/*===========================================================================

FUNCTION FTM_RFFE_DISPATCH_MULTI

DESCRIPTION
   This function performs RFFE dispatch operation for multiple(up to 16) bytes read and write.  
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rffe_dispatch_multi(ftm_rffe_pkt_multi_byte_type* rffe)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_rffe_cmd_multi(&rffe->rffe_params);  
  return rsp_pkt;
}


#endif /* FEATURE_FACTORY_TESTMOD0E */
