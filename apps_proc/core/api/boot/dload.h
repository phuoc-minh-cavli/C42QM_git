#ifndef DLOAD_H
#define DLOAD_H
/*===========================================================================

              B O O T   D O W N L O A D   S E R V I C E S
                      H E A D E R    F I L E 

DESCRIPTION
  This file contains declarations necessary to start the boot download
  services.

Copyright (c) 1993-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath:  L:/src/asw/MSM6050/vcs/dload.h_v   1.1   23 Oct 2001 18:33:46   donb  $
  $Header: //components/rel/core.tx/6.0/api/boot/dload.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/09    msm    Changes for CMI.
08/04/09   niting  Added boot_downloader to provide support for OSBL dload
05/13/03   tbg     Added check for HW specific version of dload_xxxx.h
08/24/00   jcw     Merge of MSM5000 and MSM3300 versions. removed comdef include
08/16/99   kmp     Removed keyword _cdecl from prototype.
08/11/99   kk      Merge from SS baseline.
07/22/93   jah     Changed entry point routine from download() to dload_entry()
                   per code review issue with namespace pollution.
01/11/93   jah     Initial revision

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE 

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DLOAD_ENTRY

DESCRIPTION
  Do a bootstrap download.  Initialize the MSM dataport and perform memory
  operations at the direction of an external agent via the MSM dataport.

DEPENDENCIES
  Called from a DMSS Task during offline mode, or as a part of the Boot
  module's error recovery.

RETURN VALUE
  This routine does not return.  It may branch to a new program loaded
  into RAM.

SIDE EFFECTS
  The watchdog timer is reset, interrupts are disabled.

===========================================================================*/
extern void dload_entry( void );

/*===========================================================================

 FUNCTION BOOT_DOWNLOADER

 DESCRIPTION
   Do a bootstrap download.  Initialize the USB and perform memory
   operations at the direction of an external agent via the USB.

 FORMAL ARGUMENTS
   None

 DEPENDENCIES
   Called from the Diagnostic Task during offline mode, or as a part of
   the Boot modules error recovery.

 RETURN VALUE
   This routine does not return.  It may branch to a new program loaded
   into RAM.

 SIDE EFFECTS
   The watchdog timer is reset, interrupts are disabled.  The timeout
   counter is initialized and timeout checking is enabled/disabled.
============================================================================*/
extern void boot_downloader(void);

#endif  /* DLOAD_H */
