/*============================================================================
@file npa_target_config.c

NPA target configuration data

Copyright (c) 2010-2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.tx/6.0/power/npa/hw/npa_target_config.c#1 $
============================================================================*/
#include "npa_config.h"

#define REQUEST_THREAD_PRIORITY 162

npa_config npa_config_data =
{
  16384,                        /* Log Buffer Size */

  /* pool configuration */
  { 10, 10 },                   /* resources */
  { 20, 10 },                   /* clients */
  { 10, 10 },                   /* events */
  { 10, 10 },                   /* links */
  { 10, 10 },                   /* event queue elements */
  { 32, 16 },                   /* list elements */

  /* Workloop configuration */
  { REQUEST_THREAD_PRIORITY, 1 }, /* Async Request Workloop */
  { 0,  1 },                       /* Async Event Workloop */

  /* End - points to self. Must be the last field */
  (void *)&npa_config_data
};

const npa_config *npa_target_config( void )
{
  return &npa_config_data;
}

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
#include "npa_scheduler_internal.h"

/* default "constants" for npa_scheduler */
npa_scheduler_config npa_scheduler_config_data = {
    32768,     /* Log Buffer Size */

  /* pool configuration */
  { 10, 10 },                   /* scheduled Workloop queue elements */
  { 10, 10 },                   /* scheduled request elements */

  /* Workloop configuration */
  { REQUEST_THREAD_PRIORITY, 1 }, /* Sched Request Workloop */

  /* default constants in QClks */
  20000,     /* min_issue_sched_timeout - TTICKS */
  300,       /* sched_latency - TTICKS */
  20000,     /* min_reschedule_delta - TTICKS */
  20000,     /* fork_lookahead_delta - TTICKS */
  150000,    /* resource request latency - TTICKS */
  15000,     /* resource fork latency - TTICKS */
  300,       /* resource join latency - TTICKS */
  0,         /* sleep wake transition latency - TTICKS */
  1950,      /* timer_queue_latency - TTICKS */
  0,         /* lpr_now_delta - TTICKS */
  1920000,   /* auto_complete_delta - TTICKS */
  307200,    /* The scheduler's Q6 freq request (KHz) */
  1,         /* implied_completes_on_wakeup (boolean) */
  1,         /* planB_implied_completes (boolean) */
  (void *)(&npa_scheduler_config_data) /* always last entry */
};


const npa_scheduler_config *npa_scheduler_target_config( void )
{
  return &npa_scheduler_config_data;
}

/* RPM protocol dependant resource name - Must be NULL terminated */
const char *npa_lpr_issuable_resource_names_data[] = { "/clk/cnoc",
                                                       "/mcpm/voltage/cx",
                                                       "/pmic/client/rail_mx",
                                                       "/mcpm/bus/bimc",
                                                       "/mcpm/bus/snoc",
                                                       "/mcpm/clk/q6",
                                                       "/pmic/client/rf1_tech_1x",
                                                       "/pmic/client/rf1_tech_do",
                                                       "/pmic/client/rf1_tech_gps",
                                                       "/pmic/client/rf1_tech_gsm",
                                                       "/pmic/client/rf1_tech_gsm2",
                                                       "/pmic/client/rf1_tech_lte",
                                                       "/pmic/client/rf1_tech_tdscdma",
                                                       "/pmic/client/rf1_tech_wcdma",
                                                       NULL };

npa_resource_config npa_resource_overrides_data[] = 
{
  /* name, request, fork, join - Units in QClks */
  { "/mcpm/clk/q6", { 4000, 4000, 300 } },
  { "/mcpm/bus/bimc", { 12000, 500, 300 } },
  { "/mcpm/bus/snoc", { 12000, 500, 300 } },
  { "/pmic/client/rail_mx", { 22000, 2000, 300 } },
  { "/mcpm/voltage/cx", { 10000, 2000, 300 } },
  { "/pmic/client/rf1_tech_1x", { 90000, 2000, 300 } },
  { "/pmic/client/rf1_tech_do", { 90000, 2000, 300 } },
  { "/pmic/client/rf1_tech_gps", { 90000, 2000, 300 } },
  { "/pmic/client/rf1_tech_gsm", { 90000, 2000, 300 } },
  { "/pmic/client/rf1_tech_lte", { 90000, 2000, 300 } },
  { "/pmic/client/rf1_tech_tdscdma", { 90000, 2000, 300 } },
  { "/pmic/client/rf1_tech_wcdma", { 90000, 2000, 300 } },
  { NULL, { 0, 0, 0 } } /* List is terminated by name being NULL */ 
};

#endif /* NPA_SCHEDULED_REQUESTS_SUPPORTED */
