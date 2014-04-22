#ifndef DIAG_FS_H
#define DIAG_FS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Flash Logging Internal Header File 

General Description
  Global data declarations for diagnostics flash logging.

Copyright (c) 2022 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#if defined(DIAG_FEATURE_EFS2)

#include "comdef.h"
#include "com_dtypes.h"
#include "diag.h"


/* Diag filestream type: contains file information as well as position for
   reading and max size for writing.
   ASSUMPTION: Writing is always done at the end of the file. */

/* -------------------------------------------------------------------------
** Type definitions
** ------------------------------------------------------------------------- */

typedef struct
{
  dword read_pos;   /* position for reading */
  dword write_pos;  /* position for writing */
  unsigned int max_size;        /* max file size */
  int handle;
  char *fname;
}
diag_fs_type;


/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */

void diag_fs_init (diag_fs_type * fs);
uint32 diag_fs_read (diag_fs_type * fs, void *buf, unsigned int size);
boolean diag_fs_write (diag_fs_type * fs, void *buf,
                  unsigned int size);
void diag_fs_terminate (diag_fs_type * fs, boolean unlink_file);

#endif /* DIAG_FEATURE_EFS2 */


#endif /* DIAG_FS_H */

