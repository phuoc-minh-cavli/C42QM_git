#ifndef DIAGNV_V_H
#define DIAGNV_V_H
/*==========================================================================
  
General Description
 This module contains prototypes for diag_nv_read & diag_nv_write.
                
Copyright (c) 2009, 2013, 2016-2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //components/rel/core.mpss/3.10/services/diag/DCM/common/src/diagnv_v.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   is     Make diag_nv_write_ext() and diag_nv_read_ext() public APIs
11/02/09   as     Merged DUAL sim support 
07/10/09   as     Mainlined code under FEATURE_DIAG_CACHE
16/03/09   vk     Created file.
                   
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "log_codes.h"
#include "feature.h"
#include "nv.h"
#include "diagnv.h"



/*===========================================================================

FUNCTION DIAGNV_PEEK

DESCRIPTION
  This procedure encapsulates an NV peek operation for the diag task.
  This capability is not recommended and is intended to be deprecated.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_peek (nv_peek_type *peek);



/*===========================================================================

FUNCTION DIAGNV_POKE
DESCRIPTION
  This procedure encapsulates an NV poke operation for the diag task.
  This capability is not recommended and is intended to be deprecated.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_poke (nv_poke_type *poke);


/*===========================================================================

FUNCTION DIAGNV_GET_MEID

DESCRIPTION
  Method to extract MEID.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_get_meid  (qword);

/*===========================================================================

FUNCTION DIAGNV_GET_MEID_EXT

DESCRIPTION
  Method to extract MEID.when context is specified.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_get_meid_ext  (qword, uint16);


/*===========================================================================

FUNCTION DIAGNV_REFRESH_CACHE

DESCRIPTION
  This procedure reads the NV data for the status packet into cache.
  This is done after the mc_task_start call and after OTASP successfully
  completes.

===========================================================================*/
void diagnv_refresh_cache (void);


#endif /* DIAGNV_V_H*/

