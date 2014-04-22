/*============================================================================

   FILE:       csd_error_map.c

        Copyright © 2013 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 $Header:
 $Author:

when         who       what, where, why
--------     ----      ----------------------------------------------------------
08/07/13     vk        creation
============================================================================*/

#include "csd_error_map.h"
#include "csd_debug_msg.h"


int32_t csd_map_apr_error_to_csd(uint32_t apr_rc)
{
  int32_t rc = CSD_EOK;
  switch ( apr_rc )
  {
    case APR_EOK :
      rc = CSD_EOK;
    break;
    case APR_EBADPARAM:	
      rc = CSD_EBADPARAM;
    break;
    case APR_EUNSUPPORTED :
      rc = CSD_EUNSUPPORTED;
    break;
    case APR_EVERSION :
      rc = CSD_EVERSION;
    break;
    case APR_EUNEXPECTED :
      rc = CSD_EUNEXPECTED;
    break;
    case APR_EPANIC : 
      rc = CSD_EPANIC;
    break;
    case APR_ENORESOURCE :
      rc = CSD_ENORESOURCE;
    break;
    case APR_EHANDLE :
      rc = CSD_EHANDLE;
    break;			 
    case APR_EALREADY : 
      rc = CSD_EALREADY;
    break;
    case APR_ENOTREADY :	   
      rc = CSD_ENOTREADY;
    break;
    case APR_EPENDING :
      rc = CSD_EPENDING;
    break;
    case APR_EBUSY : 
      rc = CSD_EBUSY;
    break;
    case APR_EABORTED : 
      rc = CSD_EABORTED;
    break;
    case APR_EPREEMPTED :	   
      rc = CSD_EPREEMPTED;   
    break;
    case APR_ECONTINUE : 
      rc = CSD_ECONTINUE;
    break;
    case APR_EIMMEDIATE :
      rc = CSD_EIMMEDIATE;
    break;
    case APR_ENOTIMPL :
      rc = CSD_ENOTIMPL;
    break;
    case APR_ENEEDMORE :
      rc = CSD_ENEEDMORE;
    break;
    case APR_ELPC :	
      rc = CSD_ELPC;
    break;
    case APR_EFAILED :
    case APR_ENOMEMORY :
    case APR_ENOTEXIST :
    case APR_ENOSERVICE :
    case APR_EDENIED :
      rc = CSD_EFAILED;
    break;	
    default:
      rc = CSD_EFAILED;
    break;
  }
  return rc;
}

int32_t csd_map_acdb_error_to_csd( int32_t acdb_error )
{
  int32_t rc = CSD_EFAILED;
  switch (acdb_error)
  {
    case ACDB_ERROR:
      rc = CSD_EFAILED;
    break;
    case ACDB_BADPARM:
      rc = CSD_EBADPARAM;
    break;
    case ACDB_BADSTATE:
      rc = CSD_EBADSTATE;
    break;
    case ACDB_INSUFFICIENTMEMORY:
      rc = CSD_ENORESOURCE;
    break;
    case ACDB_NULLPOINTER:
      rc = CSD_EBADPARAM;
    break;
    case ACDB_NOTSUPPORTED:
      rc = CSD_EUNSUPPORTED;
    break;
    case ACDB_MISMATCH_TARGET_VERSION:
    case ACDB_MISMATCH_DATA_SW_VERSION:
    case ACDB_VOLUME_STEP_NOT_FOUND:
      rc = CSD_EBADPARAM;
    break;
    case ACDB_DEVICENOTFOUND:
    case ACDB_SAMPLERATENOTFOUND:
    case ACDB_NETWORKNOTFOUND:
    case ACDB_CODECSAMPLERATENOTFOUND:
    case ACDB_MODULENOTFOUND:
    case ACDB_PARMNOTFOUND:
    case ACDB_APPLICATIONTYPENOTFOUND:
    case ACDB_CODECPATHNOTFOUND:
    case ACDB_INPUT_PARAMS_NOT_FOUND:
    case ACDB_DATA_NOT_FOUND:
      rc = CSD_ENOTFOUND;
    break;
    default:
      rc = CSD_EUNEXPECTED;
    break;
  }
  return rc;
}


	
