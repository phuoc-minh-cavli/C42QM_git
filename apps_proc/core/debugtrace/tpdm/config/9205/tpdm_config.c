/*==============================================================================

  TPDM configuration.

DESCRIPTION
  This file has attributes for TPDMs in the SoC.

REFERENCES

        Copyright © 2015, 2019 QUALCOMM Technologies Inc.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtrace/tpdm/config/9205/tpdm_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "DDITPDM.h"
#include "../../dal/src/DalTPDM.h"


TPDMDeviceProp tpdm_device_attributes[]={
   {0x06C28000,"qdss","qdss",TPDM_DSB,NULL},
};

const TableLength tpdm_device_attributes_len[] = 
{
{sizeof(tpdm_device_attributes)/sizeof(TPDMDeviceProp)}
};
