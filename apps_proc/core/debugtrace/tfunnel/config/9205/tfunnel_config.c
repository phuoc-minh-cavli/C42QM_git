/*==============================================================================

  Trace Funnel Configuration    

DESCRIPTION
  This file has port configurations for trace funnels for a given chipset.

REFERENCES
================================================================================
            Copyright (c) 2014, 2019 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtrace/tfunnel/config/9205/tfunnel_config.c#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
1/7/2019   kdey     Changes for BU readiness
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"

#define IN_FUN0     0
#define IN_FUN1     1
#define MERG_FUN    2
#define QATB_FUN    3

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



/*TFunnelPortEnableConfig tfunnel_port_debug_ui[]={
   {IN_FUN1,1,tfunnel_port_top}
};*/

TFunnelPortEnableConfig tfunnel_port_rpm[]={
   {IN_FUN0,0,tfunnel_port_merg0}
};

TFunnelPortEnableConfig tfunnel_port_rpm_itm[]={
   {IN_FUN0,1,tfunnel_port_merg0}
};

TFunnelPortEnableConfig tfunnel_port_peripheral_noc[]={
   {IN_FUN0,3,tfunnel_port_merg0}
};

TFunnelPortEnableConfig tfunnel_port_stm[]={
   {IN_FUN0,7,tfunnel_port_merg0}
};

TFunnelPortEnableConfig tfunnel_port_bimc[]={
   {IN_FUN0,5,tfunnel_port_merg0}
};

TFunnelPortEnableConfig tfunnel_port_mpss[]={

   {IN_FUN1,4,tfunnel_port_merg1}
};

TFunnelPortEnableConfig tfunnel_port_apps_etm[]={
   {IN_FUN1,3,tfunnel_port_merg1}
};

TFunnelPortEnableConfig tfunnel_port_fun0[]={
   {IN_FUN0,6,tfunnel_port_merg0}
};

TFunnelPortEnableConfig tfunnel_port_tpdm[]={
   {QATB_FUN,0,tfunnel_port_fun0}
};

