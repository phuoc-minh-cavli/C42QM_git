/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Error Handler non-init data
                
GENERAL DESCRIPTION
  Contains all data used to log exceptions to EFS.  Since the information is
  stored to RAM first, the logged to NV after reset, the data needs to go 
  untouched by any of the RAM initialization functions.  The scatter load
  file is set up to put any *_nzi.o files in an uninitialized portion of RAM.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2006-2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/08   tbg     Removed unused FEATURE_ERR_SAVE_CORE
10/08/07   tbg     Removed FEATURE_MULTIPROCESSOR for 6k support
10/19/06   tbg     File created.
 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "err.h"
#include "errlog.h"
#include "erri.h"
 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#if defined(__ARMCC_VERSION)
#pragma arm section zidata = ".nzi" // not zero initialized (qc specific link scripts)
#define ATTRIBUTE_NOINIT /* NULL */
#elif defined(__GNUC__)
#define ATTRIBUTE_NOINIT /* __attribute__ ((section (".noinit"))) */ /* unverified */
#endif

uint32                  err_flash_write_counter                         ATTRIBUTE_NOINIT;
err_data_type           err_data                                        ATTRIBUTE_NOINIT;
uint8                   err_efs_max_files                               ATTRIBUTE_NOINIT;


#if defined(__ARMCC_VERSION)
#pragma arm section zidata
#endif
