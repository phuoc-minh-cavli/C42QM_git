#ifndef ERR_INJECT_CRASH_RPM_H
#define ERR_INJECT_CRASH_RPM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2014 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/28/11   mcg     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                     EXTERNAL DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  ERR_INJECT_ERROR,
  SIZEOF_ERR_INJECT_CRASH_RPM_TYPE
} err_inject_crash_rpm_type;


/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void err_inject_crash_rpm_init(void);
#define ERR_DIAG_PROC_ID DIAG_APP_PROC

#endif /* ERR_INJECT_CRASH_RPM_H */
