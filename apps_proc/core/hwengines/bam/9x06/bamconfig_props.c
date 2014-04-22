/**
  @file bamconfig_props.c
  @brief
  This file contains implementation of the interfaces that provide
  target specific base addresses & irq vector data to the BAM driver.

*/
/*
===============================================================================
                            Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/13   SA      Added devcfg props support for tgtcfg and supported BAMs

===============================================================================
                   Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                    Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "bamtgtcfg.h"
#ifdef BAM_XML_PROPS
#ifdef BAM_APPS_CONFIG
#include "bamtgtcfgdata_apss.h"
#elif BAM_MODEM_CONFIG
#include "bamtgtcfgdata.h"
#elif BAM_TZOS_CONFIG
#include "bamtgtcfgdata_tz.h"
#endif
#endif

