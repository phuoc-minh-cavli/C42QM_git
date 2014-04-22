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

$Header: //components/rel/core.tx/6.0/debugtrace/tpda/config/9205/tpda_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "DalTPDA.h"

/*TPDM name listed in TPDAPortSpec must match name in TPDM config */
TPDAPortSpec tpda_qdss_list[] ={
   {"qdss",PORT10}
};





TPDADeviceProp tpda_device_list[]={
	{0x06004000,"qdss",65,BIT_MASK(PORT0),"port_tpda_qdss",tpda_qdss_list,sizeof(tpda_qdss_list)/sizeof(TPDAPortSpec)},
};

const TableLength tpda_device_list_len[] = {
   {sizeof(tpda_device_list)/sizeof(TPDADeviceProp)}
};
