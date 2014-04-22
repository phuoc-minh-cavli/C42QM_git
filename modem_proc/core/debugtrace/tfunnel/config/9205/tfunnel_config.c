/*==============================================================================

  Trace Funnel Configuration    

DESCRIPTION
  This file has port configurations for trace funnels for a given chipset.

REFERENCES

        Copyright © 2011, 2019 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/debugtrace/tfunnel/config/9205/tfunnel_config.c#2 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
1/7/2019   kdey     Changes for BU readiness
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"

#define IN_FUN0     0
#define IN_FUN1     1
#define MERG_FUN    2
/*Port assignments based on QDSS HPG*/
#define STM_PORT_NUM        7 
typedef struct TypeTFunnelPortEnableConfig TFunnelPortEnableConfig;

/*
This structure defines the topology of a funnel configuration.
*/
struct TypeTFunnelPortEnableConfig{
   uint32 funnel_id;
   uint32 port_id;
   TFunnelPortEnableConfig *pFunnelConfig;
};


/*
Define supported funnel configurations.
*/

TFunnelPortEnableConfig tfunnel_port_merg0[]={
   {MERG_FUN,0,NULL}
};

TFunnelPortEnableConfig tfunnel_port_merg1[]={
   {MERG_FUN,1,NULL}
};

TFunnelPortEnableConfig tfunnel_port_stm[]={
   {IN_FUN0,7,tfunnel_port_merg0}
};


