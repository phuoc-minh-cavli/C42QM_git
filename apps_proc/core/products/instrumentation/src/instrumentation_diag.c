/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Instrumention diag 
                
GENERAL DESCRIPTION
  Command handler for collecting instrumented data

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.tx/6.0/products/instrumentation/src/instrumentation_diag.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
3/6/13   dk     File created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "err.h"
#include "diagpkt.h"
#include "diagcmd.h"

#define INSTRUMENTATION_DIAG_SSID 3453

PACK(void *)
call_CovDumpdata (PACK(void *) , uint16 );

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 fuzzing_req;
} instrumentation_req_type;

/* table for diag packet command handler registration */
static const diagpkt_user_table_entry_type instrumentation_diagpkt_tbl[] =
{
  {INSTRUMENTATION_DIAG_SSID, INSTRUMENTATION_DIAG_SSID, call_CovDumpdata}
};

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION product_info_init

DESCRIPTION
  Registers diag command for the product info API

DEPENDENCIES
  diag must be available to register command handler

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void instrumentation_init (void)
{

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_APP_PROC, DIAG_SUBSYS_COREBSP, instrumentation_diagpkt_tbl);
                                         
}


/*===========================================================================

FUNCTION product_info

DESCRIPTION
  Return product info

DEPENDENCIES
  product_info_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr
  Note: should never return

SIDE EFFECTS

===========================================================================*/
PACK(void *)
call_CovDumpdata (PACK(void *) req_ptr, uint16 pkt_len)
{
  instrumentation_req_type *req;
  /*product_info_rsp_type *rsp = NULL;
  pm_err_flag_type PM_ERR_FLAG__STATUS;
  pm_device_info_type pmic_info_raw;
  pm_pbs_info_type pbs_info_raw;
  uint8 i;
  uint32* raw_ptr = NULL;
    DalDeviceHandle      *phChipInfo = NULL;
    DalChipInfoVersionType dload_addr_ChipInfoVersion = 0;


   send response immediately, so tool won't timeout 
  */
  uint32 *rsp =NULL;
  rsp = (uint32 *) diagpkt_subsys_alloc(
     DIAG_SUBSYS_COREBSP,                       
     INSTRUMENTATION_DIAG_SSID,
     sizeof( uint32 ));

  
  req = (instrumentation_req_type *) req_ptr;
  switch(req->fuzzing_req)
  {
	case 1:
	  cov_dumpData();
	  break;
	case 2:
	  cov_reset();
	  open();
	  break;
	case 3:
	  cov_dumpData();
	  cov_reset();
	  open();
	  break;
	case 4:
	  cov_reset();
	  open();
	  cov_dumpData();
	  break;
	default:
	  MSG_HIGH("Invalid parameter",0,0,0);
	  break;
  }

  diagpkt_commit(rsp);

  //  ERR_FATAL("Done collecting coverage data, crashing deliberately", 0, 0, 0);

  return (rsp);
}


