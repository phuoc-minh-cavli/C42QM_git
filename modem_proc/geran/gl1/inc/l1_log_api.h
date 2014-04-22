#ifndef L1_LOG_API_H
#define L1_LOG_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L1 Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging L1 packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/inc/l1_log_api.h#3 $ $DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
05/03/18   sk       CR2230579 (FR 50568): Porting the Single Voice Session FR changes onto CT.1.0
24/09/13   sp       CR449651 Define new log packet for burst metrics
01/21/02   atm      Initial version.
===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rfcom.h"
#include "sys_type.h"

#include "gl1_defs.h"

#include "geran_dual_sim.h"

/* LOG PACKETS */
/* NOTE -- use a boolean return value if it can be logged on demand */
void    l1_log_burst_metrics          ( gl1_msg_chan_type      channel,
          gl1_msg_metrics_rpt   *rpt_ptr[]  );

void    l1_log_burst_metrics_ver2          ( gl1_msg_chan_type      channel,
          gl1_msg_metrics_rpt   *rpt_ptr[]  );

#endif /* L1_LOG_API_H */
