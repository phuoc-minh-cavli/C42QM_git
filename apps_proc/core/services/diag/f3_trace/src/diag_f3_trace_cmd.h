#ifndef DIAG_F3_TRACE_DEVCFG_H
#define DIAG_F3_TRACE_DEVCFG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F3 Trace Device Configuration
                
GENERAL DESCRIPTION
  DAL Device Configuration support for F3 Trace Configuration

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.tx/6.0/services/diag/f3_trace/src/diag_f3_trace_cmd.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/11/12   rh      File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "diagpkt.h"    /* For packet macros */

/*===========================================================================

                     EXTERNAL DATA DECLARATIONS

===========================================================================*/

#define DIAG_F3_TRACE_CONFIG 0x00000000

PACK(struct) diag_f3_trace_cmd_hdr {
  diagpkt_subsys_header_type diag_cmd_hdr;
  uint32 cmd;
};
typedef struct diag_f3_trace_cmd_hdr diag_f3_trace_cmd_hdr_req_type;
typedef struct diag_f3_trace_cmd_hdr diag_f3_trace_cmd_hdr_rsp_type;

typedef PACK(struct) {
  diag_f3_trace_cmd_hdr_req_type hdr;
  uint32 control;
  uint32 detail;
} diag_f3_trace_config_cmd_type;

typedef PACK(struct) {
  diag_f3_trace_cmd_hdr_rsp_type hdr;
} diag_f3_trace_config_rsp_type;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION diag_f3_trace_cmd_reg

DESCRIPTION
  Registers the command to perform runtime configuration of f3 trace

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void diag_f3_trace_cmd_reg(void);

#endif /* DIAG_F3_TRACE_DEVCFG_H */

