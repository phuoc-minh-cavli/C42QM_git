/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Position Determination Session Manager Client Module


GENERAL DESCRIPTION
  This module contains the Position Determination Client Interface Block

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2020 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/pdapi/src/pdapiclient_lite.c#1 $ $DateTime: 2020/07/21 21:23:44 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/20   ska     Smart compilation issues
03/14/12   ss      Added support to send error in PA callback
02/01/13   mj      Modify pdsm_set_ready_flag to add IPC calls when TM ready
06/11/12   rk      Added new 'pdsm_client_pa_reg_ex' function for clients to register for 
                   all PA events. Also enhanced 'pdsm_client_pa_event_ntfy' to notify the
                   clients used extended function to register in the first place.
01/24/10   rb      add critical section around access to pdapi client map
10/25/09   gk      Sensors client
07/30/09   gk      ODP 2.0 support
01/02/09   atien   Support XTRA-T
07/28/08   gk      Fix klocwork errors
11/13/07   gk      fix link list corruption
08/01/07   rw      Add support for XTRA feature
07/20/07   gk      Notify verify send default response only if no client is registered, moved pdapi_init here
04/04/07   gk      Added some missing cmd call back fucntiosn
02/14/07   gk      lint
12/06/06   gk      store the LCS client id when lcs_reg is called.
12/04/06   gk      Modified LCS client response function
11/30/06   gk      Updated BETWEEN macro to PDSM_BETWEEN to avoid a conflict
11/16/06   gk      Updated LCS response
11/08/06   gk      PDAPI for Converged GPS

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"    /* Definition for basic types and macros */

#include "pdapi_lite.h"        /* External interface to pd.c */
#include "pdapiclient_lite.h"  /* Interface to this file */
#include "pdapidbg.h"     /* Interface to PDSM debug services */
#include "err.h"          /* Interface to error log services */
#include "msg.h"          /* Interface to F3 messages services */
#include "queue.h"        /* Interface to queue services */
#include "tm_api.h"       /* interface to tm */
#include "aries_os_api.h"

/* unconditional acceptance of SUPL_INIT by pdsm */
#define PDSM_SM_NV_GNSS_OEM_FEATURE_MASK_5_CP_SS_NOTIF_LOOPBK    0x00000010  

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
                             FREE PDSMCLIENT QUEUE
---------------------------------------------------------------------------*/

/* Static pool of Clients
*/
static pdsm_lite_client_s_type  pdsmclient_arr[ PDSMCLIENT_LITE_POOL_SIZE ];

/* Queue to manage free pdsmclient objects. Even though the clients types are
** static it is better to use Free Q's for Flexibility. It allows us to statically
** less number of buffers than the client types supported.
*/
static q_type            pdsmclient_free_q;

/*---------------------------------------------------------------------------
                             CLIENT ID TO PTR MAP LIST
---------------------------------------------------------------------------*/

/* This maptable maps client types and thier handles. The handles are returned to
** the clients when pdsm_client_init( ) is called. The table also stores the allocated
** client object pointer. In case pdsm_client_init( ) gets called more than once for
** the same client type, the buffer pointed by this pointer is re-assigned.
*/

typedef struct pdsm_lite_client_map_s {

  pdsm_lite_client_type_e_type client_type;
      /* Client Type */

  pdsm_client_id_type     client_id;
  /* Client Id (Handle for Client's) */

  pdsm_lite_client_s_type      *client_ptr;
  /* Pointer to client object */

  pdsm_lite_pd_info_s_type     *info_ptr;
  /* This is the info buffer used for reporting events back to clients. */

} pdsm_lite_client_map_s_type;

/* Allocating Separate info buffer for all the clients. Need to
** check if this is really required. Keep it this way till testing.
*/
static pdsm_lite_pd_info_s_type  info_ptr[ PDSMCLIENT_LITE_POOL_SIZE ];

/* The handles can either simple indexes to this table or they can
** be arbitrart number's. For simplicity they can be index's to avoid
** searching the maptable for entries.
*/

static pdsm_lite_client_map_s_type pdsmclient_map_arr[ PDSMCLIENT_LITE_POOL_SIZE ] =
{
  { PDSM_LITE_CLIENT_TYPE_TEST2,    (pdsm_client_id_type) 1001, NULL, info_ptr+0 },
  { PDSM_LITE_CLIENT_TYPE_OEM,      (pdsm_client_id_type) 9001, NULL, info_ptr+1 },
  { PDSM_LITE_CLIENT_TYPE_XTRA,     (pdsm_client_id_type) 8000, NULL, info_ptr+2 },
  { PDSM_LITE_CLIENT_TYPE_XTRA_T,   (pdsm_client_id_type) 8001, NULL, info_ptr+3 },
  { PDSM_LITE_CLIENT_TYPE_PDA,      (pdsm_client_id_type) 3491, NULL, info_ptr+4 },
};


/*---------------------------------------------------------------------------
                                 CLIENT LIST
---------------------------------------------------------------------------*/
/* Client list object (singular). The client list
** object is responsible for maintaining a list of
** PDSM clients and for forwarding PD events to them.
*/
static struct {
  q_type  queue;
      /* Q item to manage the client list */
} client_list;


/* Macro to get a pointer to the client list object.
**
** NOTE! This macro should only be used within the
** client list object scope.
*/
#define CLIENT_LIST_PTR() ( &client_list )


/* Macro to get a pointer the queue element within
** the client list object. NOTE! This macro should
** only be used within the client list object scope.
*/
#define CLIENT_LIST_Q_PTR() ( &(CLIENT_LIST_PTR()->queue) )



/* <EJECT> */
/*===========================================================================
                          FUNCTION PROTOTYPES
===========================================================================*/

static void pdsmclient_lite_list_add( pdsm_lite_client_s_type *client_ptr );
static void pdsmclient_lite_list_del( pdsm_lite_client_s_type *client_ptr );

static os_MutexBlockType z_pdapi_client_mutex;


/* Globals */

static uint32  q_pdapi_gnss_oem_feature_mask = 0;

/*---------------------------------------------------------------------------
                                  PDSM CLIENT
---------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION pdsm_set_gnss_oem_feature_mask #9x05 no calls

FUNCTION pdsm_client_init

DESCRIPTION
  Initializing a client object.

  This function must be called exactly once on a client object before
  it is being used, in any way, place, or form.
  In case this function is called more than once, then same client data structure
  gets reallocated.

  Note that in order to get notified of PD events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with pd_client_act()

DEPENDENCIES
  none

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/

pdsm_client_id_type pdsm_lite_client_init(
    pdsm_lite_client_type_e_type           client_type
            /* Client type */
)
{
  uint8 i;
  pdsm_lite_client_s_type  *pd_client_ptr;
  pdsm_client_id_type pd_client;
  boolean             re_init = FALSE;

  /*----------------------------------------------------------------------*/

  PDSM_FUNC_START( "pdsm_client_init()", 0,0,0 );

  /*
  ** Ascertain if a client object is already allocated for it. If yes,
  ** use the previously allocated object. Otherwise, allocate a new one.
  */

  pd_client_ptr     = NULL;
  pd_client         = -1;                        /* Initialize it to invalid */

  /* this global flag is set by TM when it is done initializing */


  os_MutexLock(&z_pdapi_client_mutex);

  for (i = 0; i < ARR_SIZE(pdsmclient_map_arr); i++)
  {
    if ( pdsmclient_map_arr[i].client_type == client_type )
    {
      if ( pdsmclient_map_arr[i].client_ptr == NULL )
      {
         pdsmclient_map_arr[i].client_ptr =
              (pdsm_lite_client_s_type *) q_get(&pdsmclient_free_q);
      }
      else
      {
        /* This is the case of client_init() getting called
        ** multiple times.
        */
        re_init = TRUE;
      }

      pd_client     = pdsmclient_map_arr[i].client_id;
      pd_client_ptr = pdsmclient_map_arr[i].client_ptr;
      break;

    } /* End if client types match */

  } /* End for all possible client entries */

  if(pd_client_ptr == NULL)
  {
    /* Return error */
    pd_client = (pdsm_client_id_type)-1;
  }
  else
  {
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Mark this object as initialized. */
    pd_client_ptr->init_mask = (pdsm_pd_event_type)PDSM_LITE_INIT_MASK;


    /* Initially client is inactive. */
    if ( (pd_client_ptr->is_active) && re_init )
    {
      /* If this is the case of multiple client_init() and without
      ** client having called client_deact() we should delete the active
      ** client otherwise the link list will be corrupted.
      */
      pdsmclient_lite_list_del( pd_client_ptr );
    }

    pd_client_ptr->is_active = FALSE;

    /* Initialize the link element*/
    (void) q_link( pd_client_ptr, &pd_client_ptr->link );

    /* Set client ID to the address of this client object */
    pd_client_ptr->client          = pd_client_ptr;

    pd_client_ptr->client_id       = pd_client;

    /* Initialize Event masks. */
    pd_client_ptr->pd_event_mask   = 0;

    /* Initially set all callback function pointers
    ** to point to NULL
    */
    pd_client_ptr->pd_event_func   = NULL;

    pd_client_ptr->pd_cmd_err_func = NULL;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return pd_client;

} /* End pdsm_client_init( ) */


/*===========================================================================

FUNCTION pdsm_client_release

DESCRIPTION
  Release a client object.

  This function should be called if a client doesn't need PDSM anymore.
  This call will release the client object. That object can be reused for
  another client. If the client needs to use PDSM again, it should call
  pdsm_client_init function to establish connection.

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_release(

 pdsm_client_id_type               client_id
        /* Client ID pointer*/
)
{
  uint8 i;
  pdsm_lite_client_s_type        *pd_client_ptr;
  pdsm_lite_client_status_e_type status;

  /*---------------------------------------------------------------------*/

  PDSM_FUNC_START( "pdsm_client_release()", 0,0,0 );

  status = PDSM_LITE_CLIENT_ERR_CLIENT_ID;

  /*
  ** Acertain if the client object is already released. If yes, return
  ** failure. Otherwise, put it on the queue.
  */

  os_MutexLock(&z_pdapi_client_mutex);

  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( pdsmclient_map_arr[i].client_id == client_id )
    {
      pd_client_ptr = pdsmclient_map_arr[i].client_ptr;

      if (pd_client_ptr == NULL)
      {
        status =  PDSM_LITE_CLIENT_ERR_CLIENT_ID;
        break;
      }
      else if ( pd_client_ptr->is_active == TRUE )
      {
        /* Client is active, need to deactivate before releasing */
        status = pdsm_lite_client_deact( client_id );
        if ( status != PDSM_LITE_CLIENT_OK )
        {
          break;
        }
      }
      /* Everything is fine, go ahead and release the client.
      */
      q_put( &pdsmclient_free_q, &pdsmclient_map_arr[i].client_ptr->link );
      pdsmclient_map_arr[i].client_ptr = NULL;

      status = PDSM_LITE_CLIENT_OK;
      break;
    }
  } /* End for all the client entries */

  os_MutexUnlock(&z_pdapi_client_mutex);
  return  status;

} /* End pdsm_client_release( ) */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_get_client_ptr

DESCRIPTION
  Returns a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object

SIDE EFFECTS
  None

===========================================================================*/
pdsm_lite_client_s_type* pdsmclient_lite_get_client_ptr(

  pdsm_client_id_type client_id
    /* Client Id */
)
{
  uint8 i;
  pdsm_lite_client_s_type* client_type = NULL;
  /*---------------------------------------------------------------------*/

  os_MutexLock(&z_pdapi_client_mutex);

  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_id == pdsmclient_map_arr[i].client_id )
    {
      client_type = pdsmclient_map_arr[i].client_ptr;
      break;
    }
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_type;

} /* End pdsmclient_get_client_ptr() */

/*===========================================================================

FUNCTION pdsmclient_get_client_ptr

DESCRIPTION
  checks if it is a valid client handle. The client should have already called
  pd_client_init() with the client type to obtain a valid client id.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object

SIDE EFFECTS
  None

===========================================================================*/

boolean pdsmclient_lite_valid_client_id(

  pdsm_client_id_type client_id
    /* Client Id */
)
{
  boolean ret_val = FALSE;
  uint8 i;

  os_MutexLock(&z_pdapi_client_mutex);

  /* Need to go through all the map entries and check if this
  ** is a valid client id.
  */
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_id == pdsmclient_map_arr[i].client_id )
    {
      if ( pdsmclient_map_arr[i].client_ptr != NULL )
      {
        ret_val = TRUE;
        break;
      } /* End if client ptr is initialized */
    } /* End if client id's match */
  } /* For all the possible clients' */

  os_MutexUnlock(&z_pdapi_client_mutex);

  return ret_val;

} /* End pdsmclient_valid_client_id() */

/*===========================================================================

FUNCTION pdsmclient_get_client_type

DESCRIPTION
  checks if it is a valid client handle. If the client is valid, the corsp
  client type will be returned to caller

DEPENDENCIES
  None

RETURN VALUE
  Client type for the client ptr

SIDE EFFECTS
  None

===========================================================================*/
pdsm_lite_client_type_e_type pdsmclient_lite_get_client_type_map(
  pdsm_client_id_type client_id
    /* Client Id */
)
{
  uint8 i;
  pdsm_lite_client_type_e_type client_type = PDSM_LITE_CLIENT_TYPE_NONE;

  os_MutexLock(&z_pdapi_client_mutex);

  /* Need to go through all the map entries and check if this
  ** is a valid client id.
  */
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_id == pdsmclient_map_arr[i].client_id )
    {
      client_type = pdsmclient_map_arr[i].client_type;
      break;
    } /* End if client id's match */
  } /* For all the possible clients' */

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_type;
} /* End pdsmclient_get_client_type() */
/*===========================================================================

FUNCTION pdsmclient_get_client_type

DESCRIPTION
  This function provides Client ID based on client type

DEPENDENCIES
  None

RETURN VALUE
  Client type for the client ptr

SIDE EFFECTS
  None

===========================================================================*/
pdsm_lite_client_id_type pdsmclient_lite_get_client_id_map(
  pdsm_lite_client_type_e_type client_type
    /* Client Id */
)
{
  uint8 i;
  pdsm_client_id_type client_id = (pdsm_client_id_type)PDSM_LITE_CLIENT_TYPE_NONE;

  os_MutexLock(&z_pdapi_client_mutex);

  /* Need to go through all the map entries and check if this
  ** is a valid client id.
  */
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_type == pdsmclient_map_arr[i].client_type )
    {
      client_id = pdsmclient_map_arr[i].client_id;
      break;
    } /* End if client id's match */
  } /* For all the possible clients' */

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_id;
} /* End pdsmclient_get_client_id_map() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_get_client_id

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
pdsm_client_id_type pdsmclient_lite_get_client_id(

  pdsm_lite_client_s_type *client_ptr
    /* Client object pointer */
)
{
  uint8 i;
  pdsm_client_id_type client_id = 0;

  os_MutexLock(&z_pdapi_client_mutex);

  /*---------------------------------------------------------------------*/

  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_ptr == pdsmclient_map_arr[i].client_ptr )
    {
      client_id = pdsmclient_map_arr[i].client_id;
      break;
    }
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_id;

} /* End pdsmclient_get_client_id() */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_act

DESCRIPTION
  Activate the client object, so that the client will be notified
  of Session Manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Add the client object to the client list.

===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_act(

  pdsm_client_id_type          client_id
    /* Client ID */
)
{

  pdsm_lite_client_s_type          *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  PDSM_FUNC_START("pdsm_client_act(). client_id=%d", client_id, 0, 0); 


  /* If client is NOT already active, add the client to the client list
  ** and indicate that client is now active,
  */
  if( !client_ptr->is_active )
  {
    pdsmclient_lite_list_add( client_ptr );
    client_ptr->is_active = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, we are trying to activate a client that is already active.
  ** This is not a problem except that it might indicate client confusion.
  */
  else
  {
    PDSM_MSG_HIGH( "Client already active! client_id=%p",client_ptr,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return PDSM_LITE_CLIENT_OK;

} /* End pdsm_client_act( ) */


/* <EJECT> */
/*===========================================================================

FUNCTION pd_client_deact

DESCRIPTION
  Deactivate a client object, so that the client will NOT be notified
  of Session Manager events (for which it is registered). Before releasing a
  client object the client needs to be deactivated using this function.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Remove the client object from the client list.

===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_deact(

    pdsm_client_id_type          client_id
    /* Client ID */
)
{

  pdsm_lite_client_s_type          *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  PDSM_FUNC_START( "pdsm_client_deact() client_id=%p",
                client_id,0,0 );

  /* If client is active, delete the client from the client list
  ** and indicate that client is NOT active,
  */
  if( client_ptr->is_active )
  {
    pdsmclient_lite_list_del( client_ptr );
    client_ptr->is_active = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, we are trying to deactivate a client that is already inactive.
  */
  else
  {
    PDSM_MSG_HIGH( "Client already inactive! client_id=%p",client_ptr,0,0 );
  }

  return PDSM_LITE_CLIENT_OK;

} /* End pdsm_client_deact() */
 	


/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_event_ntfy

DESCRIPTION
  Notify a client of a specified PD event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_lite_client_event_ntfy(

  const pdsm_lite_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_pd_event_type          pd_event,
      /* notify client of this PD event */

  const pdsm_lite_pd_info_s_type   *pd_info_ptr
      /* pointer to pdsm state struct */
)
{
  if (client_ptr == NULL || pd_info_ptr == NULL || client_ptr->init_mask != PDSM_LITE_INIT_MASK)
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If callback function exists and corresponding
  ** event bit is on, notify the client of the PD event.
  */

  if( (client_ptr->pd_event_func != NULL) &&
      (client_ptr->pd_event_mask & pd_event) )
  {
    PDSM_MSG_HIGH("pd_event_func getting called for pd_event = %d ", pd_event,0,0);
    client_ptr->pd_event_func( client_ptr->pd_data_ptr, pd_event, pd_info_ptr );
  }

} /* End pdsm_client_event_ntfy() */

/*===========================================================================

FUNCTION pdsm_client_pa_event_ntfy

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_lite_client_pa_event_ntfy(

  const pdsm_lite_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_pa_event_type          pa_event,
      /* notify client of this PD event */

  const pdsm_lite_pa_info_s_type   *pa_info_ptr
      /* pointer to pdsm state struct */
)
{
  if (client_ptr == NULL || pa_info_ptr == NULL || client_ptr->init_mask != PDSM_LITE_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If callback function exists and corresponding
  ** event bit is on, notify the client of the PD event.
  */

  if( (client_ptr->pa_event_func != NULL) &&
      (client_ptr->pa_event_mask & pa_event) )
  {
    client_ptr->pa_event_func( client_ptr->pa_data_ptr, pa_event, pa_info_ptr );
  }

  // Notify client registered with extended all PA event interface
  if ( client_ptr->pa_event_func_ex != NULL )
  {
    client_ptr->pa_event_func_ex( client_ptr->pa_data_ptr, pa_info_ptr->pa_event, pa_info_ptr, PDSM_LITE_PA_CMD_ERR_NOERR );
  }

} /* End pdsm_client_pa_event_ntfy() */

/*===========================================================================

FUNCTION pdsm_client_pa_event_ntfy_ex

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_lite_client_pa_event_ntfy_ex(

  const pdsm_lite_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_lite_pa_event_type          pa_event,
      /* notify client of this PD event */

  const pdsm_lite_pa_info_s_type   *pa_info_ptr,
  /* pointer to pdsm state struct */

  pdsm_lite_pa_cmd_err_e_type   pa_cmd_err  
      /*CMD Error*/
)
{
  if (client_ptr == NULL || pa_info_ptr == NULL || client_ptr->init_mask != PDSM_LITE_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }


  // Notify client registered with extended all PA event interface
  if ( client_ptr->pa_event_func_ex != NULL )
  {
    client_ptr->pa_event_func_ex( client_ptr->pa_data_ptr, pa_info_ptr->pa_event, pa_info_ptr, pa_cmd_err );
  }

} /* End pdsm_client_pa_event_ntfy() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_ext_status_event_ntfy
#9x05 no calls

FUNCTION pdsm_client_pd_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified PD command error code if the client is
  registered to be notified of OTHER clients PD command errors and the
  client is NOT the one originally sending this PD command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsm_lite_client_pd_cmd_err_ntfy(

  const pdsm_lite_client_s_type         *client_ptr,
      /* pointer to a client struct */

  const pdsm_lite_pd_cmd_s_type         *pd_cmd_ptr,
      /* pointer to pd command struct */

  pdsm_lite_pd_cmd_err_e_type           pd_cmd_err
      /* command error code */
)
{
  if (client_ptr == NULL || pd_cmd_ptr == NULL || client_ptr->init_mask != PDSM_LITE_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pd_cmd_err, PDSM_PD_CMD_ERR_NONE, PDSM_PD_CMD_ERR_MAX) );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != pd_cmd_ptr->client )
  {
    if( client_ptr->pd_cmd_err_func != NULL )
    {
      client_ptr->pd_cmd_err_func( pd_cmd_ptr->client_id,
                                     pd_cmd_ptr->cmd,
                                     pd_cmd_err );

    } /* End if no cmd err func is installed */
  } /* End if client ID's don't match */

} /* End of pdsm_client_cmd_err_ntfy */

/*===========================================================================

FUNCTION pdsm_client_pa_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified Parameter command error code.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsm_lite_client_pa_cmd_err_ntfy(

  const pdsm_lite_client_s_type         *client_ptr,
      /* pointer to a client struct */

  const pdsm_lite_pa_cmd_s_type         *pa_cmd_ptr,
      /* pointer to pa command struct */

  pdsm_lite_pa_cmd_err_e_type           pa_cmd_err
      /* command error code */
)
{
  if (client_ptr == NULL || pa_cmd_ptr == NULL || client_ptr->init_mask != PDSM_LITE_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pa_cmd_err, PDSM_PA_CMD_ERR_NONE, PDSM_PA_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != pa_cmd_ptr->client )
  {
    if( client_ptr->pa_cmd_err_func != NULL )
    {
      client_ptr->pa_cmd_err_func( pa_cmd_ptr->client_id,
                                     pa_cmd_ptr->cmd,
                                     pa_cmd_err );

    } /* End if no cmd err func is installed */
  } /* End if client ID's don't match */
} /* End of pdsm_client_cmd_err_ntfy */
/*===========================================================================

#9x05: FUNCTION pdsm_client_ext_status_cmd_err_ntfy removed as thought not required*/

/*---------------------------------------------------------------------------
                                CLIENT LIST
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION pdsmclient_list_init

DESCRIPTION
  Initializes the client list.

  This function must be called before using the client list is used in any
  way, place or form.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pdsmclient_lite_list_init( void )
{

  PDSM_FUNC_START( "pdsmclient_list_init()",0,0,0 );

  /* Initialize client queue.
  */
  (void) q_init( CLIENT_LIST_Q_PTR() );
}

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_first

DESCRIPTION
  Return a pointer to first client in the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  Return a pointer to first client in the client list.
  If client list is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static pdsm_lite_client_s_type *pdsmclient_lite_list_first( void )
{
  PDSM_FUNC_START( "pdsmclient_list_first()",0,0,0 );

  return( (pdsm_lite_client_s_type*) q_check(CLIENT_LIST_Q_PTR()) );
}



/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_next

DESCRIPTION
  Return a pointer to the next client in the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().
  A lock of z_pdapi_client_mutex is needed before calling this function.

RETURN VALUE
  A pointer to the next client in the client list.
  If the end of the client list is reached, a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static pdsm_lite_client_s_type *pdsmclient_lite_list_next( pdsm_lite_client_s_type *client_ptr )
{
  q_link_type  *link_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (client_ptr == NULL)
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get pointer to next client
  ** (i.e. the link with in the client).
  */
  link_ptr = (q_link_type*) q_next( CLIENT_LIST_Q_PTR(), &client_ptr->link );

  /* If this is not the end of the client list, return a pointer
  ** to the client object containing the link element.
  */
/* Else, we have reached the end of the client list -
  ** return NULL.
  */
  return (pdsm_lite_client_s_type *) link_ptr;

} /* End pdsmclient_list_next() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_add

DESCRIPTION
  Add a client to the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE #9x05
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsmclient_lite_list_add( pdsm_lite_client_s_type *client_ptr )
{

  PDSM_FUNC_START( "pdsmclient_list_add()",0,0,0 );

  /* Make sure this client is NOT already in the client list.
  */
  if(client_ptr == NULL || client_ptr->link.next_ptr != NULL )
  {
    PDSM_MSG_HIGH("Non-Null pointer", 0,0,0);
    return;
  }

#ifndef FEATURE_Q_SINGLE_LINK
  if( client_ptr->link.prev_ptr != NULL )
  {
    PDSM_MSG_HIGH("Non-Null pointer", 0,0,0);
    return;
  }
#endif

#ifndef __GRIFFON__
#ifndef FEATURE_Q_NO_SELF_QPTR
  if( client_ptr->link.q_ptr != NULL )
  {
    PDSM_MSG_HIGH("Non-Null Pointer",0,0,0);
    return;
  }
#endif
#endif  // __GRIFFON__

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);
  q_put( CLIENT_LIST_Q_PTR(), &client_ptr->link );
  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_add() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_del

DESCRIPTION
  Delete a client from the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsmclient_lite_list_del( pdsm_lite_client_s_type *client_ptr )
{

  if( client_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /* Make sure this client is indeed in the client list.
  */
  if( client_ptr->link.next_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

#ifndef FEATURE_Q_SINGLE_LINK
  if( client_ptr->link.prev_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
#endif


#ifndef __GRIFFON__
#ifndef FEATURE_Q_NO_SELF_QPTR
  if( client_ptr->link.q_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
#endif
#endif  // __GRIFFON__
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Delete the client from the client list.
  */
  os_MutexLock(&z_pdapi_client_mutex);
//
//#ifdef FEATURE_Q_NO_SELF_QPTR
//  q_delete(CLIENT_LIST_Q_PTR(), (q_link_type *) &client_ptr->link );
//#else
//  q_delete( (q_link_type *) &client_ptr->link );
//
//#endif
//

#if defined(__GRIFFON__) || defined(FEATURE_Q_NO_SELF_QPTR)
  q_delete(CLIENT_LIST_Q_PTR(), (q_link_type *) &client_ptr->link );
#else
  q_delete( (q_link_type *) &client_ptr->link );

#endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the client's link item to make sure its next and prev
  ** pointers are no longer pointing at other clients.
  */
  (void) q_link( client_ptr, &client_ptr->link );

  os_MutexUnlock(&z_pdapi_client_mutex);
} /* End pdsmclient_list_del() */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_pd_event_ntfy #9x05 no calls to this api

DESCRIPTION
  Notify all the clients in the client list of a specified PD event. Only the
  registered clients other than those which were active are informed about
  this event. The active client are passed through list_ptr.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void pdsmclient_lite_reg_list_pd_event_ntfy(

  pdsm_pd_event_type           pd_event,
      /* notify clients of this PD event */

  const pdsm_lite_pd_info_s_type    *pd_info_ptr,
      /* pointer to PD information struct */

  pdsm_lite_client_s_type           **list_ptr
      /* List of clients which are active and would be informed separately */
)
{
  pdsm_lite_client_s_type *client_ptr, *next_ptr;

  boolean found = FALSE;
  uint8 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( pd_event != (pdsm_pd_event_type)PDSM_PD_EVENT_MASK );
  if( pd_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */

  client_ptr = pdsmclient_lite_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_lite_list_next(client_ptr);

    /* Compare the registred client with the active client */
    for ( i=0; i < PDSMCLIENT_LITE_POOL_CHECK; i++ )
    {
        if ( client_ptr == list_ptr[i] )
        {
            /* This registered client was active and is already informed, so
            ** skip him.
            */
            found = TRUE;
            break;
        }
    } /* End for all the Active Client's */

    if ( found == FALSE )
    {
        /* This is a registered client so inform this client about a pd_event */
        pdsm_lite_client_event_ntfy( client_ptr, pd_event, pd_info_ptr );
    }


    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);
} /* End pdsm_reg_list_event_ntfy() */



/*===========================================================================

FUNCTION pdsmclient_list_pd_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_lite_list_pd_event_ntfy(

  pdsm_pd_event_type   pd_event,
      /* notify clients of this PD event */

  const pdsm_lite_pd_info_s_type    *pd_info_ptr
      /* pointer to PD information struct */
)
{
  pdsm_lite_client_s_type *client_ptr , *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( pd_event != (pdsm_pd_event_type)PDSM_PD_EVENT_MASK );
  if( pd_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */

  client_ptr = pdsmclient_lite_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_lite_list_next(client_ptr);

    pdsm_lite_client_event_ntfy( client_ptr, pd_event, pd_info_ptr );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_event_ntfy() */

/*===========================================================================

#9x05: FUNCTION pdsmclient_list_pa_event_ntfy removed and using ex*/

/*===========================================================================

FUNCTION pdsmclient_list_pa_event_ntfy_ex

DESCRIPTION
  Notify all the clients in the client list of a specified PA event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_lite_list_pa_event_ntfy(

  pdsm_pa_event_type   pa_event,
      /* notify clients of this PD event */

  const pdsm_lite_pa_info_s_type    *pa_info_ptr,
      /* pointer to PD information struct */

  pdsm_lite_pa_cmd_err_e_type   pa_cmd_err
    /*CMD Error*/

)
{
  pdsm_lite_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( pa_event != PDSM_PA_EVENT_MASK );
  if( pa_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */
  client_ptr = pdsmclient_lite_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_lite_list_next(client_ptr);

    pdsm_lite_client_pa_event_ntfy_ex( client_ptr, pa_event, pa_info_ptr,pa_cmd_err );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_event_ntfy() */


/*===========================================================================

#9x05 FUNCTION pdsmclient_list_ext_event_ntfy removed not used it seems*/
/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_pd_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PDSM command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_lite_list_pd_cmd_err_ntfy(

  pdsm_lite_pd_cmd_err_e_type      pd_cmd_err,
      /* PD command error code */

  const pdsm_lite_pd_cmd_s_type    *pd_cmd_ptr
      /* pointer to PD command struct */

)
{
  pdsm_lite_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pd_cmd_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pd_cmd_err, PDSM_PD_CMD_ERR_NONE, PDSM_PD_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  client_ptr = pdsmclient_lite_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_lite_list_next(client_ptr);

    pdsm_lite_client_pd_cmd_err_ntfy( client_ptr,
                                 pd_cmd_ptr,
                                 pd_cmd_err );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_pd_cmd_err_ntfy() */


/*===========================================================================

FUNCTION pdsmclient_list_pd_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PDSM command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_lite_list_pa_cmd_err_ntfy(

  pdsm_lite_pa_cmd_err_e_type      pa_cmd_err,
      /* PD command error code */

  const pdsm_lite_pa_cmd_s_type    *pa_cmd_ptr
      /* pointer to PD command struct */

)
{
  pdsm_lite_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pa_cmd_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pa_cmd_err, PDSM_PA_CMD_ERR_NONE, PDSM_PA_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  client_ptr = pdsmclient_lite_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_lite_list_next(client_ptr);

    pdsm_lite_client_pa_cmd_err_ntfy( client_ptr,
                                 pa_cmd_ptr,
                                 pa_cmd_err );
    /* Set the current client to the next client in list */
    client_ptr = next_ptr;

  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_pd_cmd_err_ntfy() */
/*===========================================================================

#9x05: FUNCTION pdsmclient_list_ext_status_cmd_err_ntfy seems not used*/

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_pd_reg

DESCRIPTION
  This function allows a client to register the following PD-related
  callbacks and events with PDSM:

   1. Callback function to receive registered PD events
   2. Register and de-register PD events
   3. Callback function to receive other clients' PD-command errors.

   If a client registers a callback function to receive PD event
   notification, it also needs to register one or more PD events it is
   interested in receiving.

  If a client registers a callback function to receive other clients'
  pd-command errors, it will receive notifications on all possible
  pd-command errors caused by other clients.The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' pd-command errors, it can set pd_cmd_err_func = NULL.
  However, if the pd_event_func is set to NULL, all registered PD events
  are also de-registered.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_pd_reg(

  pdsm_lite_client_id_type           client_id,
      /* client id */

  void                          *client_data_ptr,

  pdsm_lite_pd_event_f_type          *pd_event_func,
      /* Pointer to a callback function to notify the client of PD
      ** events */

  pdsm_lite_client_event_reg_e_type  event_reg_type,

  pdsm_lite_pd_event_type            pd_event_mask,
      /* register for this event mask */

  pdsm_lite_pd_cmd_err_f_type        *pd_cmd_err_func
    /* pointer to a callback function to notify the client of PD
    ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct pdsm_lite_client_s  *client_ptr;
  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  if( pd_event_func == NULL )
  {

   /* deregister all of client's PD events */
   client_ptr->pd_event_mask = 0;

   /* update PD event and other clients' command error callback functions */

   client_ptr->pd_event_func = pd_event_func;
   client_ptr->pd_cmd_err_func = pd_cmd_err_func;
   return PDSM_LITE_CLIENT_OK;

  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->pd_event_mask) & (~PDSM_PD_EVENT_MASK) )
  {
      return PDSM_LITE_CLIENT_ERR_EVENT_MASK;
  }

  switch(event_reg_type)
  {
     case PDSM_LITE_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case PDSM_LITE_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return PDSM_LITE_CLIENT_ERR_EVENT_REG_TYPE;
  }

  PDSM_MSG_HIGH("pdsm_client_pd_reg(). CDPtr %p", client_data_ptr, 0, 0); 

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;

  if (reg_flag == TRUE)
  {
    client_ptr->pd_event_mask = pd_event_mask;

    /* update PD event and other clients' command error callback functions */
    client_ptr->pd_event_func = pd_event_func;
    client_ptr->pd_cmd_err_func = pd_cmd_err_func;
  }
  else
  {
    /* the client wants to de-register, just clear the event bit nask.
    */
    client_ptr->pd_event_mask &= ~pd_event_mask;
  }

  return PDSM_LITE_CLIENT_OK;

}/* End of pdsm_client_pd_reg( ) */


/* <EJECT> */
//Todo:This function has been reinstated but will be removed once translation of messages
//is moved in sm



/*===========================================================================

#9x05:FUNCTION pdsm_client_pa_reg removed, felt not needed in addition to ex*/

/* <EJECT> */

/*===========================================================================

FUNCTION pdsm_client_pa_reg_ex #9x05 discuss with akash

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks with PDSM:

   1. Callback function to receive Parameter events, all PA events will be
      reported, client will not be able to selectively register certain
      events
   2. Register and de-register Parameter events
   3. Callback function to receive other clients' PD-command errors.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  If client calls this function to register PA events, all PA events 
  will be reported via the registered callback function "pa_event_func_ex", 
  including all PA operations up to PDSM_PA_LPP_CONFIG_INFO. 

  If client also calls legacy pdsm_client_pa_reg() function to register for the 
  old bitmask-style PA event reporting, client will get event via the registered
  callback function "pa_event_func" only for PA operation up to 
  PDSM_PA_LPP_CONFIG_INFO (client will get event twice in this case, client
  needs to properly handle such case).
 
  If client register using both methods (pdsm_client_pa_reg() & pdsm_client_pa_reg_ex())
  it should also call both legacy & new interface to deregister completely.
===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_pa_reg(

   pdsm_lite_client_id_type              client_id,
   /* Requesting client */
   void                             *client_data_ptr,

   pdsm_lite_pa_event_f_type             *pa_event_func,
   /* Pointer to a callback function to notify the client of PA
                                             ** events */
   /*~ PARAM pa_event_func POINTER */
   pdsm_lite_client_event_reg_e_type     event_reg_type,

   pdsm_lite_pa_event_type               event_mask,
   /* register from this event (including) */

   pdsm_lite_pa_cmd_err_f_type           *pa_cmd_err_func

      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM pa_cmd_err_func POINTER */ 
)
{
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (NULL == client_ptr) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  if ( NULL == pa_event_func ) // considered as deregister
  {
    /* update PD event and other clients' command error callback functions */
    client_ptr->pa_event_func = pa_event_func;
    client_ptr->pa_cmd_err_func_ex = pa_cmd_err_func;

    return PDSM_LITE_CLIENT_OK;
  }


  switch (event_reg_type)
  {
  case PDSM_LITE_CLIENT_EVENT_REG:
    // In case of registration init the fields.
    client_ptr->pa_event_func = pa_event_func;
    client_ptr->pa_cmd_err_func_ex = pa_cmd_err_func;
    break;

  case PDSM_LITE_CLIENT_EVENT_DEREG:
    // If client wants to deregister, just clear the callback functions
    client_ptr->pa_event_func = NULL;
    client_ptr->pa_cmd_err_func_ex = NULL;
    break;

  default: 
    return PDSM_LITE_CLIENT_ERR_EVENT_REG_TYPE;
  }

  // use the latest client data pointer.
  client_ptr->pa_data_ptr_ex = client_data_ptr;

  return PDSM_LITE_CLIENT_OK;
}
/*~ FUNCTION pdsm_client_pa_reg_ex */

/* <EJECT> */
/*===========================================================================

#9x05: FUNCTION pdsm_client_ext_status_reg removed felt not needed

#reinstated for now
*/

/*===========================================================================

FUNCTION pdsm_client_pa_reg_ex

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks with PDSM:

   1. Callback function to receive Parameter events, all PA events will be
      reported, client will not be able to selectively register certain
      events
   2. Register and de-register Parameter events
   3. Callback function to receive other clients' PD-command errors.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  If client calls this function to register PA events, all PA events 
  will be reported via the registered callback function "pa_event_func_ex", 
  including all PA operations up to PDSM_PA_LPP_CONFIG_INFO. 

  If client also calls legacy pdsm_client_pa_reg() function to register for the 
  old bitmask-style PA event reporting, client will get event via the registered
  callback function "pa_event_func" only for PA operation up to 
  PDSM_PA_LPP_CONFIG_INFO (client will get event twice in this case, client
  needs to properly handle such case).
 
  If client register using both methods (pdsm_client_pa_reg() & pdsm_client_pa_reg_ex())
  it should also call both legacy & new interface to deregister completely.
===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_pa_reg_ex(

    pdsm_lite_client_id_type              client_id,
        /* Requesting client */
    void                             *client_data_ptr,

    pdsm_lite_pa_event_ex_f_type          *pa_event_func_ex,
        /* Pointer to a callback function to notify the client of PA
        ** events */
    /*~ PARAM pa_event_func_ex POINTER */
    pdsm_lite_client_event_reg_e_type     event_reg_type,

    pdsm_lite_pa_cmd_err_f_type           *pa_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM pa_cmd_err_func POINTER */ 
)
{
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (NULL == client_ptr) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  if ( NULL == pa_event_func_ex ) // considered as deregister
  {
    /* update PD event and other clients' command error callback functions */
    client_ptr->pa_event_func_ex = pa_event_func_ex;
    client_ptr->pa_cmd_err_func_ex = pa_cmd_err_func;

    return PDSM_LITE_CLIENT_OK;
  }


  switch (event_reg_type)
  {
  case PDSM_LITE_CLIENT_EVENT_REG:
    // In case of registration init the fields.
    client_ptr->pa_event_func_ex = pa_event_func_ex;
    client_ptr->pa_cmd_err_func_ex = pa_cmd_err_func;
    break;

  case PDSM_LITE_CLIENT_EVENT_DEREG:
    // If client wants to deregister, just clear the callback functions
    client_ptr->pa_event_func_ex = NULL;
    client_ptr->pa_cmd_err_func_ex = NULL;
    break;

  default: 
    return PDSM_LITE_CLIENT_ERR_EVENT_REG_TYPE;
  }

  // use the latest client data pointer.
  client_ptr->pa_data_ptr_ex = client_data_ptr;

  return PDSM_LITE_CLIENT_OK;
}
/*~ FUNCTION pdsm_client_pa_reg_ex */

/*===========================================================================
FUNCTION PDAPI_INIT

DESCRIPTION
  Initialize the Position Determination API - called from the PD task

RETURN VALUE
  None

DEPENDENCIES
  Called only once from PDSM_TASK or lsm_task

===========================================================================*/

void pdapi_lite_init( void )
{
  /* Now initialize different queue's. When P-FIFO is used a new queue will be used,
  ** which stores the request's in P-FIFO manner.
  */

  (void)os_MutexInit(&z_pdapi_client_mutex, MUTEX_DATA_ONLY_CONTEXT);

  /* Initialize the Client free q */
  pdsmclient_lite_free_q_init();

  /* Init the client list Q, Queue to maintain list of active clients.
  */
  pdsmclient_lite_q_init();

} /* End pdapi_init() */

/*===========================================================================

FUNCTION pdsm_client_xtra_reg

DESCRIPTION
This function allows a client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_xtra_reg
(
  pdsm_lite_client_id_type           client_id,
  /* Requesting client */
  void                          *client_data_ptr,

  pdsm_lite_xtra_event_f_type          *xtra_event_func,
  /* Pointer to a callback function to notify the client of PD
  ** events */

  pdsm_lite_client_event_reg_e_type  event_reg_type,
  /* Register or deregister events */

  pdsm_lite_xtra_event_type            event_mask,
  /* register from this event (including) */

  pdsm_lite_xtra_cmd_err_f_type        *xtra_cmd_err_func
  /* pointer to a callback function to notify the client of XTRA
  ** command errors of OTHER clients */
)
{
#ifdef FEATURE_CGPS_XTRA
  boolean reg_flag;
  struct pdsm_lite_client_s  *client_ptr;

  /* Only XTRA client can register for events */
  if (pdsmclient_lite_get_client_type_map(client_id) != PDSM_LITE_CLIENT_TYPE_XTRA)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA: invalid client id %d", client_id, 0, 0);
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  if( xtra_event_func == NULL )
  {
    /* deregister all of client's XTRA events */
    client_ptr->xtra_event_mask = PDSM_XTRA_EVENT_NONE;

    /* update event and other clients' command error callback functions */
    client_ptr->xtra_event_func = xtra_event_func;
    client_ptr->xtra_cmd_err_func = xtra_cmd_err_func;

    return PDSM_LITE_CLIENT_OK;
  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->xtra_event_mask) & ~(PDSM_XTRA_EVENT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_EVENT_MASK;
  }

  /* Determine if this request if for registering or deregistering */
  switch(event_reg_type)
  {
  case PDSM_LITE_CLIENT_EVENT_REG:
    reg_flag=TRUE;
    break;

  case PDSM_LITE_CLIENT_EVENT_DEREG:
    reg_flag=FALSE;
    break;

  default:
    return PDSM_LITE_CLIENT_ERR_EVENT_REG_TYPE;
  }

  /* use the latest client data pointer */
  client_ptr->xtra_data_ptr = client_data_ptr;

  if ( reg_flag == TRUE)
  {
    /* In case of registration init the fields */
    client_ptr->xtra_event_mask = event_mask;

    /* update XTRA event and other client command error callback functions */
    client_ptr->xtra_event_func = xtra_event_func;
    client_ptr->xtra_cmd_err_func = xtra_cmd_err_func;
  }
  else
  {
    /* If client wants to deregister, just clear his event mask */
/*lint -e{502} as client_ptr does not negative number, this is safe */
    client_ptr->xtra_event_mask &= ~event_mask;
  }


  return PDSM_LITE_CLIENT_OK;
#else
  return PDSM_LITE_CLIENT_ERR_CLIENT_INIT_FAIL;
#endif /* FEATURE_CGPS_XTRA */
} /* pdsm_client_xtra_reg */



/*===========================================================================

FUNCTION pdsm_client_xtra_t_reg

DESCRIPTION
This function allows a client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
pdsm_lite_client_status_e_type pdsm_lite_client_xtra_t_reg
(
  pdsm_lite_client_id_type           client_id,
  /* Requesting client */
  void                          *client_data_ptr,

  pdsm_lite_xtra_t_event_f_type      *xtra_t_event_func,
  /* Pointer to a callback function to notify the client of PD
  ** events */

  pdsm_lite_client_event_reg_e_type  event_reg_type,
  /* Register or deregister events */

  pdsm_lite_xtra_t_event_type        xtra_t_event_mask,
  /* register from this event (including) */

  pdsm_lite_xtra_t_cmd_err_f_type    *xtra_t_cmd_err_func
  /* pointer to a callback function to notify the client of XTRA-T
  ** command errors of OTHER clients */
)
{
#ifdef FEATURE_CGPS_XTRA_T
  boolean reg_flag;
  struct pdsm_lite_client_s  *client_ptr;

  /* Only XTRA-T client can register for events */
  if (pdsmclient_lite_get_client_type_map(client_id) != PDSM_LITE_CLIENT_TYPE_XTRA_T)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"XTRA-T: invalid client id %d", client_id, 0, 0);
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_CLIENT_ID;
  }

  if( xtra_t_event_func == NULL )
  {
    /* deregister all of client's XTRA-T events */
    client_ptr->xtra_t_event_mask = PDSM_XTRA_T_EVENT_NONE;

    /* update event and other clients' command error callback functions */
    client_ptr->xtra_t_event_func = xtra_t_event_func;
    client_ptr->xtra_t_cmd_err_func = xtra_t_cmd_err_func;

    return PDSM_LITE_CLIENT_OK;
  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->xtra_t_event_mask) & ~(PDSM_XTRA_T_EVENT_MASK) )
  {
    return PDSM_LITE_CLIENT_ERR_EVENT_MASK;
  }

  /* Determine if this request if for registering or deregistering */
  switch(event_reg_type)
  {
  case PDSM_LITE_CLIENT_EVENT_REG:
    reg_flag=TRUE;
    break;

  case PDSM_LITE_CLIENT_EVENT_DEREG:
    reg_flag=FALSE;
    break;

  default:
    return PDSM_LITE_CLIENT_ERR_EVENT_REG_TYPE;
  }

  /* use the latest client data pointer */
  client_ptr->xtra_t_data_ptr = client_data_ptr;

  if ( reg_flag == TRUE)
  {
    /* In case of registration init the fields */
    client_ptr->xtra_t_event_mask = xtra_t_event_mask;

    /* update XTRA-T event and other client command error callback functions */
    client_ptr->xtra_t_event_func = xtra_t_event_func;
    client_ptr->xtra_t_cmd_err_func = xtra_t_cmd_err_func;
  }
  else
  {
    /* If client wants to deregister, just clear his event mask */
    client_ptr->xtra_t_event_mask &= ~xtra_t_event_mask;
  }

  return PDSM_LITE_CLIENT_OK;
#else
  return PDSM_LITE_CLIENT_ERR_CLIENT_INIT_FAIL;
#endif /* FEATURE_CGPS_XTRA_T */
} /* pdsm_client_xtra_t_reg */


/*===========================================================================

FUNCTION pdsmclient_free_q_init

DESCRIPTION
  This is used to initialize the free q for Client object's.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request statuss

SIDE EFFECTS
  none

===========================================================================*/

void pdsmclient_lite_free_q_init( void )
{
  uint8 i;

  os_MutexLock(&z_pdapi_client_mutex);

  (void)q_init( &pdsmclient_free_q );

  for( i=0; i < ARR_SIZE(pdsmclient_arr); i++ )
  {
    (void) q_link( &pdsmclient_arr[i], &pdsmclient_arr[i].link );
    q_put( &pdsmclient_free_q, &pdsmclient_arr[i].link );
  } /* End for client array element's */

  os_MutexUnlock(&z_pdapi_client_mutex);
} /* End pdsmclient_free_q_init() */


/*===========================================================================

FUNCTION pdsmclient_q_init

DESCRIPTION
  This is used to initialize the client object q.

DEPENDENCIES

RETURN VALUE
  request statuss

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_lite_q_init( void )
{
  PDSM_FUNC_START(" pdsmclient_list_init()",0,0,0);

  /* Init the Client Queue.
  */
  os_MutexLock(&z_pdapi_client_mutex);
  (void)q_init( CLIENT_LIST_Q_PTR() );
  os_MutexUnlock(&z_pdapi_client_mutex);
}

/*===========================================================================

FUNCTION PSMCLIENT_CLIENT_INFO_ALLOC

DESCRIPTION
  This is used to allocate the info buffer for client. All the clients have
  a statically allocated buffer for reporting client info. This buffer is reused
  whenever any new info is required to be passed to the user. Currently all the
  active clients have their own client id field set when the data is send back,
  so we require a per client buffer.

DEPENDENCIES

RETURN VALUE
  request statuss

SIDE EFFECTS
  none

===========================================================================*/
pdsm_lite_pd_info_s_type *pdsmclient_lite_info_alloc( pdsm_lite_client_s_type   *client_ptr )
{
  uint8 i;
  pdsm_lite_pd_info_s_type* client_type_ptr = NULL;

  /*---------------------------------------------------------------------*/

  os_MutexLock(&z_pdapi_client_mutex);
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_ptr == pdsmclient_map_arr[i].client_ptr )
    {
      client_type_ptr = pdsmclient_map_arr[i].info_ptr;
      break;
    }
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_type_ptr;
} /* End pdsmclient_info_alloc() */
/*===========================================================================

FUNCTION PDSM_XTRA_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/

boolean pdsm_lite_xtra_client_request
(
 pdsm_xtra_event_type      xtra_event,        /* Type of XTRA event */
 pdsm_lite_xtra_info_s_type     *xtra_info_ptr     /* data to be sent to XTRA client */
)
{
  pdsm_lite_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( xtra_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return FALSE;
  }

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and find the XTRA client
  ** send the event to it.
  */
  client_ptr = pdsmclient_lite_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_lite_list_next(client_ptr);

    /* call the XTRA registered call back function */
    if ( (client_ptr->xtra_event_func != NULL) &&
         (client_ptr->xtra_event_mask & xtra_event) )
    {
      /* Set the client ID */
      xtra_info_ptr->client_id = client_ptr->client_id;

      client_ptr->xtra_event_func(client_ptr->xtra_data_ptr, xtra_event, xtra_info_ptr);
    }

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return TRUE;

} /* End pdsm_notification_verification_request( ) */


/*===========================================================================

FUNCTION PDSM_XTRA_T_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA-T client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/
boolean pdsm_lite_xtra_t_client_request
(
 pdsm_xtra_t_event_type      xtra_t_event,        /* Type of XTRA-T event */
 pdsm_lite_xtra_t_info_s_type     *xtra_t_info_ptr     /* data to be sent to XTRA-T client */
)
{
  pdsm_lite_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( xtra_t_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("xtra_t_info_ptr Null", 0,0,0);
    return FALSE;
  }


  /* Traverse the client list and find the XTRA-T client
  ** send the event to it.
  */
  client_ptr = pdsmclient_lite_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_lite_list_next(client_ptr);

    /* call the XTRA-T registered call back function */
    if ( (client_ptr->xtra_t_event_func != NULL) &&
         (client_ptr->xtra_t_event_mask & xtra_t_event) )
    {
      /* Set the client ID */
      xtra_t_info_ptr->client_id = client_ptr->client_id;

      client_ptr->xtra_t_event_func(client_ptr->xtra_data_ptr, xtra_t_event, xtra_t_info_ptr);
    }

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  return TRUE;

} /* pdsm_xtra_t_client_request */


