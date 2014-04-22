/*==============================================================================

  Trace Funnel Configuration    

DESCRIPTION
  This file has port configurations for trace funnels for a given chipset.

REFERENCES
================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtrace/tfunnel/config/9x06/tfunnel_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"

#define IN_FUN0  0
#define TOP_FUN 1


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

TFunnelPortEnableConfig tfunnel_port_rpm[]={
   {IN_FUN0,0,NULL}
};

TFunnelPortEnableConfig tfunnel_port_rpm_itm[]={
   {IN_FUN0,1,NULL}
};

TFunnelPortEnableConfig tfunnel_port_mpss[]={
   {IN_FUN0,2,NULL}
};

TFunnelPortEnableConfig tfunnel_port_bimc[]={
   {IN_FUN0,3,NULL}
};

TFunnelPortEnableConfig tfunnel_port_apps_etm[]={
   {IN_FUN0,4,NULL}
};

/*
TFunnelPortEnableConfig tfunnel_port_system_noc[]={
   {IN_FUN0,5,NULL}
};
*/

TFunnelPortEnableConfig tfunnel_port_mss_vbif[]={
   {IN_FUN0,5,NULL}
};

static TFunnelPortEnableConfig tfunnel_port_top[]={
   {IN_FUN0,6,NULL}
};

TFunnelPortEnableConfig tfunnel_port_peripheral_noc[]={
   {TOP_FUN,0,tfunnel_port_top}
};

TFunnelPortEnableConfig tfunnel_port_debug_ui[]={
   {TOP_FUN,1,tfunnel_port_top}
};

TFunnelPortEnableConfig tfunnel_port_stm[]={
   {IN_FUN0,7,NULL}
};



