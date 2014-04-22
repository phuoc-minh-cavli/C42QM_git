/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM RFFE   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RFFE commands comming from the diag service 

Copyright (c) 2012  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/rf/common/ftm/src/ftm_rffe.h#1 $ 
$DateTime: 2020/04/16 12:27:00 $ 
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/26/12   jfc     Change RFFE profile command to a generic RFFE test 
09/14/12   jfc     Move RFFE commands to common FTM 
09/07/12   jfc     Add hooks for RFCMD profiling 
08/18/12   jfc     Initial revision
===========================================================================*/

#ifndef FTM_RFFE_H
#define FTM_RFFE_H

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE

#include "comdef.h"
#include "target.h"
#include "ftm.h"
#include "ftm_rf_cmd.h"
#include "ftmdiag.h"
#include "diagpkt.h"

typedef enum{
   FTM_RFFE_CMD,
   FTM_RFFE_DUMP,
   FTM_RFFE_TEST
} ftm_hwtc_rffe_types;


/******************************* RFE DATA STRUCTURE **************************/
typedef PACK(struct)
{
  uint8 ext;
  uint8 readwrite;
  uint8 channel;
  uint8 slave;
  uint16 address;
  uint8 data;
  uint8 halfspeed;
} ftm_rffe_data_type;

typedef PACK(struct)
{
  uint16 rffe_cmd_id;
} ftm_rffe_cmd_type;


/************************* FTM DATA STRUCTURE **************************/

/* hwtc rffe request/response type */
typedef PACK(struct)
{
  diagpkt_subsys_header_type  header;
  ftm_cmd_header_type         ftm_cmd_header;
  ftm_rffe_cmd_type           rffe_cmd_header;
  ftm_rffe_data_type          rffe_params;
} ftm_rffe_pkt_type;


ftm_rsp_pkt_type ftm_rffe_dispatch(ftm_rffe_pkt_type*);

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HWTC_RFFE_H */
