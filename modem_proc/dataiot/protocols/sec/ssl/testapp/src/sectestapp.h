#ifndef SECTESTAPP_H
#define SECTESTAPP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
           T E S T    A P P L I C A T I O N    F I L E
                             
GENERAL DESCRIPTION
  This task level file for the Security Services Test Application task
  defines the timers, signals, queues, etc. necessary for the test 
  application to exercise the interfaces and implementation of Security
  Services.  The UI task generates commands to the SECTESTAPP task to 
  invoke the various test functions for SSL, crypto, or DRM.  This task
  does not have any test functions for IPSec - the UI invokes the VPN
  interface directly.

EXTERNALIZED FUNCTIONS

  sectestapp_task() - task entry point and main processing loop.

SSL tests:
  sectestapp_ssl_http_start()  - invoke HTTP SSL test
  sectestapp_ssl_func_start()  - invoke SSL functional test
  sectestapp_ssl_multi_start() - invoke SSL multi-session test
  sectestapp_ssl_long_start()  - invoke SSL long-running test
  sectestapp_ssl_concurrent_start() - invoke concurrent SSL test

Crypto tests:
  sectestapp_api_pkx_start() - invoke public key exchange API tests
  sectestapp_api_enc_start() - invoke cipher API tests
  sectestapp_api_hsh_start() - invoke hash API tests
  sectestapp_crypto_start()  - invoke crypto tests for internal PI

Unusual tests:
  sectestapp_custom_start() - invoke custom (unique) tests, normally unused

DRM tests:
  sectestapp_secdrm_test_1() - invoke DRM test 1
  sectestapp_secdrm_test_2() - invoke DRM test 2
  sectestapp_secdrm_test_3() - invoke DRM test 3
  sectestapp_secdrm_test_4() - invoke DRM test 4
  sectestapp_secdrm_test_5() - invoke DRM test 5

INITIALIZATION AND SEQUENCING REQUIREMENTS
  MC must initialize the task and set the start signal for this task, before 
  any other functions can be called.

  Copyright (c) 2000-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/sec/ssl/testapp/src/sectestapp.h#1 $
  $DateTime: 2020/03/17 08:03:16 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/14   jk      ECC time test
08/01/13   nk      AES256 support in secapi interface 
06/28/13   rk      Feature wrapped fuzzing test cases under SEC_HW_CRYPTO
03/27/13   rk      Added Fuzz Tests
05/15/12   ak      Smetest case enhancements
04/30/12   nk      ZUC algorithm ported. 
02/23/10   ejt     Removed all arc4 and des longtests. 
02/18/10   ejt     Removed sectest_data from semtest files. This saves 50K of heap space.
02/08/10   wa      Added support for IxContentHandler Tests
11/20/09   tk      Added labeling test.
09/15/09   ejt     added SECTESTAPP_ROM_LITE_RANDOM_NUM_START 
06/10/09   ejt     removed CS_ISIGNAL
11/05/08   ejt     updated secapi_IxFileSFS_test function prototype to return error count 
05/17/07   avm     dual proc support
04/25/07   rv      Added SFS tests
01/29/07   df      removed ROAP tests
04/05/04   rwh     Added comments.
10/17/03   rwh     Re-inserted deprecated value SECTESTAPP_SSL_RESUME_START.
09/23/03   rwh     Re-inserted deprecated testapp/UI interface functions.
08/26/03   rwh     Update for sectestapp_get_cmd_buf(), restored _LONG_TEST.
11/15/02   rwh     Added SECTESTAPP_CRYPT_DONE_SIG for async callbacks.
05/24/02   om      Added support for HTTPS test app
05/03/02   dld     Added Debug for DSP DME testing
02/13/02   rwh     Added SSL test app timer sig
02/07/02   dld     Added support for event-driven sockets.
12/13/01   dld     Added SSL UPLOAD/DOWNLOAD test enums
11/19/01   dld     Added SSL HTTP test enum
05/24/01    om     Added SSL session resumption and multiple session.
01/26/01   yll     Created Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#define FEATURE_SEC_TESTAPPS
#ifdef FEATURE_SEC_TESTAPPS

#include "cmd.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef IMAGE_QDSP6_PROC
#include "ixlock.h"
extern strLockInfo  g_sectestlock; /* Rex based critical section structure */
#define SECTEST_LOCK_INIT()         IxLockInit(&g_sectestlock, LOCK_TASK_ONLY)
#define SECTEST_LOCK()          IxLock(&g_sectestlock);
#define SECTEST_UNLOCK()          IxUnlock(&g_sectestlock);  
#define SECTEST_LOCK_SAV()          IxLock(&g_sectestlock);
#define SECTEST_UNLOCK_SAV()  IxUnlock(&g_sectestlock);  
#define SECTEST_TASK_LOCK()          IxLock(&g_sectestlock);
#define SECTEST_TASK_UNLOCK()  IxUnlock(&g_sectestlock);  
#define SECTEST_INT_TASK_LOCK()  IxLock(&g_sectestlock);
#define SECTEST_INT_TASK_UNLOCK()  IxUnlock(&g_sectestlock);

#else
#define SECTEST_LOCK_INIT() 
#define SECTEST_LOCK()          INTLOCK()
#define SECTEST_UNLOCK()          INTFREE()  
#define SECTEST_LOCK_SAV(isave)     INTLOCK_SAV(isave)
#define SECTEST_UNLOCK_SAV(isave)   INTFREE_SAV(isave)  
#define SECTEST_TASK_LOCK()          TASKLOCK()
#define SECTEST_TASK_UNLOCK()  TASKFREE()  
#define SECTEST_INT_TASK_LOCK()  TASKLOCK();INTLOCK()
#define SECTEST_INT_TASK_UNLOCK()  INTFREE();TASKFREE()  
#endif

/*--------------------------------------------------------------------------
  Security Service Test Application Task Signals
---------------------------------------------------------------------------*/
/* The next 3 sigs are never used explicitly in the sectestapp code.  
 * They are here as place-holders, so that when adding signals, the user does 
 * not re-defined signals already defined system-wide in task.h.  These 
 * signals are referred by their proper name (i.e., TASK_STOP_SIG) in this 
 * Security services code.
 */

#define SECTESTAPP_START_SIG         0x8000    /* Rsvd for TASK_START_SIG   */
#define SECTESTAPP_STOP_SIG          0x4000    /* Rsvd for TASK_STOP_SIG    */
#define SECTESTAPP_OFFLINE_SIG       0x2000    /* Rsvd for TASK_OFFLINE_SIG */

/* The following sigs are all okay, and can be allocated freely */
#define SECTESTAPP_CRYPT_DONE_SIG    0x0008    /* crypto operation done     */
#define SECTESTAPP_NV_CMD_SIG        0x0010    /* NV interaction done       */
#define SECTESTAPP_CMD_Q_SIG         0x0020    /* new cmd in SEC_CMD_Q      */
#define SECTESTAPP_SSLAPP_USR_SIG    0x0040    /* usr-sig for SSL test app  */
#define SECTESTAPP_SSLAPP_SOCKET_READ_SIG  0x0080 /* SSL read signal        */
#define SECTESTAPP_SSLAPP_TIMER_SIG  0x0100  /* SSL timer signal            */

/* Signals for the HTTP test app */
#define SECSSLHTTP_USR_SIG           0x0200
#define SECSSLHTTP_READ_SIG          0x0400
#define SECSSLHTTP_EVENT_SIG         0x0800
#define SECSSLHTTP_PAGE_DONE_SIG     0x1000

/* 7500 test app sigs */
#ifdef IMAGE_MODEM_PROC
#define SECTESTAPP_TIMER_SIG        0x10000
#endif
/*----
----------------------------------------------------------------------
  Secured Service task command names. 
---------------------------------------------------------------------------*/
typedef enum
{
  SECTESTAPP_CMD_NULL = 0,         /* For sectestapptask internal use only!! */

  /* Test application commands */
  SECTESTAPP_SSL_HTTP_START,                 /* Start an SSL HTTP get/response  */ 
  SECTESTAPP_SSL_FUNC_START,                 /* Start an SSL functional test.   */
  SECTESTAPP_SSL_MULTIPLE_START,             /* Start an SSL multi-session test.*/
  SECTESTAPP_SSL_LONG_START,                 /* Start an SSL large data test.   */
  SECTESTAPP_API_PKX_START,                  /* Start an API PKX test series    */
  SECTESTAPP_API_ENC_START,                  /* Start an API cipher test series */
  SECTESTAPP_API_HSH_START,                  /* Start an API hash test series   */
  SECTESTAPP_CRYPTO_START,                   /* Start crypto-specific tests     */
  SECTESTAPP_CE_START,                       /* Start a ce test             */
  SECTESTAPP_SSL_CONCURRENT_START,           /* Start an SSL large data test    */
  SECTESTAPP_SECDRM_ADD_RIGHT,               /* Add a DRM right                 */
  SECTESTAPP_SECDRM_LOOKUP_RIGHTS,           /* Lookup associated DRM rights    */
  SECTESTAPP_SECDRM_CONSUME_RIGHT,           /* Consume DRM rights              */
  SECTESTAPP_SECDRM_DEL_ASSO_RIGHTS,         /* Delete associated DRM rights    */
  SECTESTAPP_SECDRM_DELETE_RIGHT,            /* Delete DRM right                */
  SECTESTAPP_AES128_SW_TIMETEST_START,       /* Run AES 128 Time tests          */
  SECTESTAPP_ZUC_TIMETEST_START,             /* Run ZUC Time tests            */ 
  SECTESTAPP_HSH_HMAC_SW_TIMETEST_START,     /* Run HSH/HMAC Time tests         */
  SECTESTAPP_PKX_TIMETEST_START,             /* Run PKX Time tests              */
  SECTESTAPP_SFS_TEST1_START,                /* Run SFS Test                   */
  SECTESTAPP_ROM_LITE_CHIPSET_API_START,     /* Run ROM Lite Chipset API tests   */
  SECTESTAPP_ROM_LITE_ISA_SEC_SERVER_START,  /* Run ROM Lite ISA Security tests  */
  SECTESTAPP_ROM_LITE_RANDOM_NUM_START,      /* Run Crypto Random num test       */
  SECTESTAPP_ROM_LITE_LABEL_START,           /* Run Label test                  */
  SECTESTAPP_IXCONTENTHANDLER_TEST_START,    /* Run IxContentHandler test     */
  SECTESTAPP_SFP_TEST_START,                 /* Run SFP Test                  */
  SECTESTAPP_REMOTE_FS_START,                /* Run Remote test                 */
  SECTESTAPP_SFS_START,                      /* Run SFS test                    */
  SECTESTAPP_SECURE_TAR_START,               /* Run Secure tar test             */
  SECTESTAPP_SECURE_CHANNEL_START,           /* Run Secure channel test         */
  SECTESTAPP_SECURE_HW_PRNG_START,           /* Run Hardware PRNG test          */
  SECTESTAPP_SECURE_HW_KEY_START,            /* Run Hardware key test           */
  SECTESTAPP_AES256_CE_TIMETEST_START ,      /* Run AES 256 Time tests          */
  SECTESTAPP_AES256_SW_TIMETEST_START,       /* Run AES 256 Time tests          */
  SECTESTAPP_AES128_CE_TIMETEST_START ,      /* Run AES 128 Time tests          */
  SECTESTAPP_HSH_HMAC_CE_TIMETEST_START,     /* Run HSH/HMAC Time tests         */
  SECTESTAPP_SNOW3G_TIMETEST_START,          /* Run SNOW3G Time tests              */
  SECTESTAPP_XPU_CFG_START,                  /* run xpu configuration validation test */
  SECTESTAPP_KASUMI_TIMETEST_START,          /* Run Kasumi Time tests              */
#if SECTESTAPP_CE_FUZZ
  SECTESTAPP_FUZZ_TEST_START,				/* For ce crypto fuzz tests */
#endif /*SECTESTAPP_CE_FUZZ*/
  SECTESTAPP_ECC_TIMETEST_START,             /* Run ECC Time test                 */
  SECTESTAPP_CMD_MAX                       /* For sectestapptask internal use only!! */

} sectestapp_cmd_name_type;

/*--------------------------------------------------------------------------
  Secured Service task command status enum type. This servers as a 
  semaphore to check if the command has finished processing.
---------------------------------------------------------------------------*/
typedef enum {
  SECTESTAPP_DONE_S,            /* General OK return */
  SECTESTAPP_BUSY_S,            /* Command is being processed */
  SECTESTAPP_BADCMD_S,          /* Invalid command */
  SECTESTAPP_BADPARM_S          /* Bad Parameters */
} sectestapp_cmd_status_type;

/*--------------------------------------------------------------------------
  Command header type.  Each command to SECTESTAPP task starts with this header. 
---------------------------------------------------------------------------*/

typedef struct {
  cmd_hdr_type                   cmd_hdr;      /* Command header */
  sectestapp_cmd_name_type       cmd;          /* Command name */
  sectestapp_cmd_status_type     status;       /* Completion code status */
} sectestapp_cmd_hdr_type;

/*--------------------------------------------------------------------------
  sectestapp_cmd_type is an union of all the above command types.
---------------------------------------------------------------------------*/
typedef struct
{
  sectestapp_cmd_hdr_type                    hdr;
} sectestapp_cmd_type;

/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION SECTESTAPP_GET_CMD_BUF

DESCRIPTION
  This function will get a command buffer from sectestapp_cmd_free_q. 
  
DEPENDENCIES
  The SECTESTAPP task must have been started up already. 
  Sectestapp_cmd_free_q has been initiated using q_init.

RETURN VALUE
  If there is a free buffer available, a pointer to this buffer is returned.
  Otherwise, a NULL pointer is returned.

SIDE EFFECTS
  None.
===========================================================================*/
extern sectestapp_cmd_type* sectestapp_get_cmd_buf
(
  void
);


/*===========================================================================

FUNCTION SECTESTAPP_CMD

DESCRIPTION
  This function enqueues a command to the Security task's appropriate 
  command queue based on its command type, and sets a signal to inform 
  the sectestapp task. Note that this function runs at the caller's 
  context.

  The function returns to the caller after setting the status to busy 
  indicaton.  This does _not_ mean that the command has been executed yet, 
  just that it is waiting to be executed.

  An example is as follows:

  sectestapp_cmd_type *cmd_ptr;
  cmd_ptr = sectestapp_get_cmd_buf();                -- Get the sec cmd buffer
  cmd_ptr->hdr.cmd = SECTESTAPP_xxx;                 -- Set the command type
  sectestapp_cmd(cmd_ptr);                           -- Enqueue the command.
  

DEPENDENCIES
  The SEC task must have been started up already.  All of the items in
  the sectestapp_cmd_type must be already set. 

RETURN VALUE
  None directly.  The status variable of the sectestapp_cmd_type will be 
  updated to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The sectestapp_cmd_type is placed on the SEC command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/
extern void sectestapp_cmd
(
  sectestapp_cmd_type*      cmd_ptr
);


/*==========================================================================
 FUNCTION SECTESTAPP_WAIT

 DESCRIPTION 
   This function performs the SEC task waiting function. This function
   will not return until a signal from the requested mask is set.  To 
   ensure that the watchdog stays happy, this function internally makes
   sure that the watchdog is petted periodically.
   
   
 DEPENDENCIES 
   CAN ONLY BE CALLED FROM THE SECTESTAPP TASK CONTEXT.  IF CALLED FROM 
   ANOTHER TASK, THEN THAT TASK WILL HANG FOREVER...

 RETURN VALUE 
   A signal mask respresenting the signals which are currently set.  At
   least one signal in the requested mask will be set.  Any other signals
   which are set, which may or may not be spec'd in the signal mask, are
   also returned.

 SIDE EFFECTS
   None
===========================================================================*/
rex_sigs_type sectestapp_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
) ;

/*===========================================================================

FUNCTION SECTESTAPP_TASK

DESCRIPTION
  This function is the entry point and main processing loop for the security
  services task.  This function contains the infinite while loop.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sectestapp_task
( 
  dword dummy                            /* unused dummy var               */
);


/*===========================================================================

FUNCTION SECTESTAPP_*_START

DESCRIPTION
  This are external functions called by the UI to start inidividual
  tests.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/* SSL related tests */
extern void sectestapp_ssl_http_start( void );
extern void sectestapp_ssl_func_start( void );
extern void sectestapp_ssl_multi_start( void );
extern void sectestapp_ssl_long_start(void);
extern void sectestapp_ssl_concurrent_start(void);

#endif /* FEATURE_SEC_TESTAPPS */

#endif /* SECTESTAPP_H */
