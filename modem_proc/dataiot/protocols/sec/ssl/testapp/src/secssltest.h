#ifndef SECSSLTEST_H
#define SECSSLTEST_H


/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

                   T E S T   A P P L I C A T I O N

GENERAL DESCRIPTION
  Test application for Security Services / SSL. The following test
  are accessible from the UI test menu:

  - SSL_FUNC: Functional test, opens a session, loops back one packet,
              and closes the session.

  - SSL_LONG: Same as above, but loops back more data for profiling.

  - SSL_RESUME: Opens a session, loops back one packet, closes the session,
                and resumes it.

  - SSL_MULTI: Opens a long session, and runs a SSL_RESUME test in parallel.
               If DNS is enabled, the server name and alias allow the 
               two sessions to go to different servers.

  - SSL_CERT: Opens a session and prints out details of the server
              certificate to the F3 screen.
        

USAGE
  The SSL test server is provisioned via the phone book:

  Location 73 contains the server's IP address in network order,
              i.e. 129.46.50.82 is entered as 129046050082.
  Location 74 contains the server's port first number in network order.
              Multiple client use the subsequent numbers, i.e.,
              if 0451 is provisioned in the phone book, client #1 uses
              port 451, client #2 uses port 452, etc.
              
  If the server's IP address is set to 0, DNS is used to resolve
  the names 'sslsvr01' and 'sslsvr02'.

  NOTE: The IP address must be 12 digits long to be valid !


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secssltest.h_v   1.9   03 Sep 2003 11:08:24   omichael  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/sec/ssl/testapp/src/secssltest.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/19   ds      memory optimization changes for low tier chipsets.
08/31/10   yh      Add current test
12/13/01   dld     Added new Upload/Download test
11/19/01   dld     Added new HTTP/External test
05/31/01   om      Several updates for new tests.
03/01/01   yll     Created Module

===========================================================================*/




/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
#include "secssl.h"

typedef enum 
{
  SECSSLTEST_TEST_FUNC = 0,
  SECSSLTEST_TEST_LONG,
  SECSSLTEST_TEST_MULTIPLE,
  SECSSLTEST_TEST_CONCURRENT,
  SECSSLTEST_TEST_HTML
} secssltest_test_type;

typedef struct
{
  secssl_handle_type session;
  char*              server_name;
  uint32             server_ip;
  uint16             server_port;
  uint16             client_num;
  sint15             app_id;

  boolean            active;
  boolean            failure;
  boolean            close_alert;
  boolean            alert;
  boolean            resume;
  boolean            secure;
  boolean            external;
  secssltest_test_type test_type;

  /* Til here only used by secsslhttp.c */
  uint32             data_size;
  uint16             data_left;
  uint16             block_size;
  boolean            vary_block_size;
  boolean            infinite_repeat;
  boolean            upload;
  boolean            download;
  boolean            echo_server;
  boolean            event_driven;
  int                download_file_handle;
  uint32             download_file_size;
  uint32             required_download_size;
  boolean            is_ipv6;

} secssltest_client_type;

/* Maximum receive buffer size */
#define SECSSLHTTP_MAX_DATA_SIZE           4096

/* Event definitions for event mask */
#define SECSSLHTTP_NETWORK_EVENT           0x00000001
#define SECSSLHTTP_SOCK_OPEN_EVENT         0x00000002
#define SECSSLHTTP_SOCK_READ_EVENT         0x00000004
#define SECSSLHTTP_SOCK_CLOSE_EVENT        0x00000008
#define SECSSLHTTP_DNS_START_EVENT         0x00000010
#define SECSSLHTTP_DNS_SUCCESS_EVENT       0x00000020
#define SECSSLHTTP_DNS_FAILURE_EVENT       0x00000040
#define SECSSLHTTP_CONNECT_START_EVENT     0x00000080
#define SECSSLHTTP_CONNECT_SUCCESS_EVENT   0x00000100
#define SECSSLHTTP_CONNECT_FAILURE_EVENT   0x00000200
#define SECSSLHTTP_AUTH_START_EVENT        0x00000400
#define SECSSLHTTP_AUTH_SUCCESS_EVENT      0x00000800
#define SECSSLHTTP_AUTH_FAILURE_EVENT      0x00001000
#define SECSSLHTTP_SOCK_ACCEPT_EVENT       0x00002000

/* Task tcb the HTTP app runs in */
#define SECSSLHTTP_TCB_PTR                 (&sectestapp_tcb)
#define SECSSLHTTP_WAIT_FUNC               sectestapp_wait
  



/*===========================================================================

                     EXTERNAL FUNCTIONS

===========================================================================*/

/*==========================================================================
 FUNCTION SECSSLTEST
  
 DESCRIPTION 

  
 DEPENDENCIES 
   None

 RETURN VALUE 
   None
  
 SIDE EFFECTS
   None                          
===========================================================================*/

extern void secssltest
( 
  secssltest_test_type  test_type
);


/*===========================================================================
FUNCTION SECSSLTEST_PROC_SSLAPP_USR_SIG

DESCRIPTION
  This function starts to exchange test data over the successfully
  opened SSL session.

DEPENDENCIES
  None

PARAMETERS
  user_data_ptr - void-casted pointer to this client
  _errno        - outcome of the open command

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void sectestapp_proc_sslapp_usr_sig( void );

extern void secsslhttp_event_callback
(
  secssl_handle_type                session_handle,
  secssl_ses_event_enum_type        event,
  void*                             user_data_ptr
);

extern secerrno_enum_type secsslhttp_client_run
(
  secssltest_client_type*  client_ptr
);

#define SECSSLTEST_NUM_CLIENTS           3
extern secssltest_client_type  secssltest_client_list[SECSSLTEST_NUM_CLIENTS];
extern secssltest_client_type*  secsslhttp_client;

extern boolean  secsslhttp_page_done;

#endif /* SECSSLTEST_H */
