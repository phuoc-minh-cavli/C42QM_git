/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                      FTM RFFE Multi-Bytes Read/Write   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RFFE commands comming from the diag service 

Copyright (c) 2015  by QUALCOMM Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/rf/common/ftm/src/ftm_rffe_multi.h#1 $ 
$DateTime: 2020/04/16 12:27:00 $ 
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/29/15   fhuo    Initial revision

==========================================================================*/

#ifndef FTM_RFFE_MULTI_H
#define FTM_RFFE_MULTI_H

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE

#include "comdef.h"
#include "target.h"
#include "ftm.h"
#include "ftm_rf_cmd.h"
#include "ftmdiag.h"
#include "diagpkt.h"
#include "ftm_rffe.h"  


/******************************* RFE DATA STRUCTURE **************************/
typedef PACK(struct)
{
  uint8 ext;
  uint8 readwrite;
  uint8 channel;
  uint8 slave;
  uint16 address;
  uint8 size;
  uint32 data1;
  uint32 data2;
  uint32 data3;
  uint32 data4;
  uint8 halfspeed;
} ftm_rffe_data_multi_byte_type;



/************************* FTM DATA STRUCTURE **************************/

/* hwtc rffe request/response type */
typedef PACK(struct)
{
  diagpkt_subsys_header_type  header;
  ftm_cmd_header_type         ftm_cmd_header;
  ftm_rffe_cmd_type           rffe_cmd_header;
  ftm_rffe_data_multi_byte_type          rffe_params;
} ftm_rffe_pkt_multi_byte_type;


ftm_rsp_pkt_type ftm_rffe_dispatch_multi(ftm_rffe_pkt_multi_byte_type*);

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_RFFE_MULTI_H */
