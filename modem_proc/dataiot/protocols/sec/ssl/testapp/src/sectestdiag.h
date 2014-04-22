#ifndef _SECTESTDIAG_H
#define _SECTESTDIAG_H

/*===========================================================================

                  S E C U R I T Y   S E R V I C E S

            D I A G   I N T E R F A C E   D E F I N I T I O N S


This file defines the DIAG interface (events, logs) used by SECTEST.

Changes to these enums MUST be coordinated with the diag/tools team.
As of the creation date mail had to be sent to asw.diag.request.

Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/sectestdiag.h_v   1.2   14 Oct 2003 17:12:28   rhummel  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/sec/ssl/testapp/src/sectestdiag.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/11   aj      Remove Compilation warnings
09/29/03   rwh     Added comments about tool output.
09/10/03   rwh     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "event.h"

#ifdef FEATURE_SEC_TESTAPPS


/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
The following indicates the nature of the SECTEST_EVENT.
--------------------------------------------------------------------------*/
typedef enum
{                                       // Display: 
  SECTEST_HTTP_START_EVENT     = 4,     // "HTTP Test Start:"
  SECTEST_HTTP_SUCCESS_EVENT   = 5,     // "HTTP Test Success:"
  SECTEST_HTTP_FAILURE_EVENT   = 6,     // "HTTP Test Failure:"

  SECTEST_SSL_START_EVENT      = 8,     // "SSL Test Start:"
  SECTEST_SSL_SUCCESS_EVENT    = 9,     // "SSL Test Success:"
  SECTEST_SSL_FAILURE_EVENT    = 10,    // "SSL Test Failure:"

  SECTEST_SECAPI_START_EVENT   = 12,    // "SECAPI Test Start:"
  SECTEST_SECAPI_SUCCESS_EVENT = 13,    // "SECAPI Test Success:"
  SECTEST_SECAPI_FAILURE_EVENT = 14,    // "SECAPI Test Failure:"

  SECTEST_SECCRYPT_START_EVENT   = 16,  // "Crypto Test Start:"
  SECTEST_SECCRYPT_SUCCESS_EVENT = 17,  // "Crypto Test Success:"
  SECTEST_SECCRYPT_FAILURE_EVENT = 18,  // "Crypto Test Failure:"

  SECTEST_MAX_EVENT            = 0xffff /* 2-byte length */
} sectest_event_enum_type;


/*--------------------------------------------------------------------------
The payload for the SECTEST_EVENT indicates the nature of the event,
the status, the test identifier, the client identifier, and one reserved 
value.
The display order should be:
"HTTP Test Success: 0xss, Test: 0xyyyy, Client: 0xzzzz, Val1: 0xvvvv"
--------------------------------------------------------------------------*/
typedef struct
{                                           // Display:
   sectest_event_enum_type   specific_event;// (see above)
   uint16                    status;        // 0xss
   uint32                    test_id;       // "Test: " 0xyyyy
   uint32                    client_id;     // "Client: " 0xzzzz
   uint32                    value1;        // "Val1: " 0xvvvv
}sectest_event_payload_type;


/*** Macros to make code more readable ***/
#ifdef FEATURE_SECTEST_DIAG_EVENTS

  #define SECTEST_DIAG_EVENT_ALLOC( payload ) \
    sectest_event_payload_type  (payload)
     
  #define SECTEST_DIAG_EVENT_SET( payload_ptr, event, stat, test, client_ptr)\
    (payload_ptr)->specific_event = (event);                            \
    (payload_ptr)->status         = (stat);                             \
    (payload_ptr)->test_id        = (uint32)(test);                     \
    (payload_ptr)->client_id      = (uint32)(client_ptr);               \
    (payload_ptr)->value1         = 0
    
  #define SECTEST_DIAG_EVENT_REPORT( payload_ptr )                   \
		event_report_payload( EVENT_SECTEST,            \
                                           sizeof( *(payload_ptr) ), \
                                           (payload_ptr) )

#else /* FEATURE_SECTEST_DIAG_EVENTS */

  #define SECTEST_DIAG_EVENT_ALLOC( payload ) 
  #define SECTEST_DIAG_EVENT_SET( payload_ptr, event, stat, test, client_ptr)
  #define SECTEST_DIAG_EVENT_REPORT( payload_ptr )

#endif /* FEATURE_SECTEST_DIAG_EVENTS */


/*--------------------------------------------------------------------------
The SECTEST_EVENT enum value (974) is displayed by QXDM when using a version
of the tool which predates the enum assignment.
The payload is displayed as a sequence of unsigned 8-bit hex values:
2 bytes of specific event;
2 bytes of status ( 0 = success, 1 = failure);
4 bytes of test id (zero so far);
4 bytes of app. client id;
4 bytes of zero.

The display order is network order, thus the values are reversed from the
ARM order, e.g. the ARM value for "HTTP Success" event is 0x0005, 
and is displayed as "05  00"
--------------------------------------------------------------------------*/



#endif   /* FEATURE_SEC_TESTAPPS */

#endif /* _SECTESTDIAG_H */


