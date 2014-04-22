/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         I X F I L E   D R I V E M A P

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS



Copyright (c) {2010,2017} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/securemsm/smecom/ixfile/environment/src/IxFileDriveMap.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/10    nk     Initial Revision

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "IxFile.h"
#include "IxFileSystem.h"
#include "IxFileTypes.h"

#if defined(T_MSM6290) || defined(T_MSM7200) || \
    defined(T_MSM7500) || defined(T_MSM7600) || \
    defined(T_QSC6270)
  #define IXFILE_BREW_ROOT NULL
#else
  #define IXFILE_BREW_ROOT "/brew"
#endif

#ifndef UIONE_SDK
   #define FILESYSTEM_ROOT IXFILE_BREW_ROOT
#else
  #define FILESYSTEM_ROOT NULL
#endif

/*
** Table that maps drive strings to directories.
*/
typedef struct
{
  const char *driveString;
  const char *rootDir;
} IxFileDriveT;

static const IxFileDriveT driveMap[] =
{
  { SYS_SFS_DRIVE, "/sfs" },
  { USR_SFS_DRIVE, USR_SFS_DRIVE_MAP },
  { "drm_ms_cert:", "/__wmdrm_sfs" },
  { "drm_ms_lic:", "/__wmdrm_sfs" },
  { "mtp:", IXFILE_BREW_ROOT },
  { "drmoma:", "/__drm_sfs" },
  { "fs:/card0", "/mmc1" },
  { "fs:", FILESYSTEM_ROOT },
};
