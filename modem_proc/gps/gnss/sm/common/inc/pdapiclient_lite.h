#ifndef PDSMCLIENT_LITE_H
#define PDSMCLIENT_LITE_H


#include "gps_variation.h"
/*===========================================================================

       Position Determination Session Manager Header File.

DESCRIPTION
  This header file contains definitions necessary to internally interface
  with PDSMCLIENT.C.

  Note that definitions that are necessary to externally interface with
  PDSMCLIENT.C are provided in PDSM.H


  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc. Qualcomm Technologies Confidential and
  Proprietary. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/inc/pdapiclient_lite.h#1 $ $DateTime: 2020/07/21 21:23:44 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------    
02/17/17   skm     BLE and Barometer support for LPPE Phase 2
09/05/14   ssu     GM LOWI Integration
03/14/12   ss      Added support to send error in PA callback
09/03/12   rh      Added support for Cell Info and NI message injection
06/11/12   rk      Enhanced 'pdsm_lite_client_s_type' struct to have extended function types 
                   used by pdsm_lite_client_pa_reg_ex & pdsm_lite_client_pa_event_ntfy functions. 
03/02/09   jd      Added support for Wiper logging over RPC.
01/02/09   atien   Support XTRA-T
06/12/08   lt      Support for external coarse position injection.
07/20/07   gk      Removed obsoleted functions, move pdapi_init to here
07/02/07   rw      added new XTRA client type
05/14/07   gk      added new client type 
04/03/07   gk      added some missing cmd call back fucntions
12/06/06   gk      added ni cleint id get func
12/04/06   gk      Updated LCS response
11/08/06   gk      PDAPI for converged GPS
===========================================================================*/


#include "pdapi_lite.h"        /* External interface to pdsm.c */
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* <EJECT> */
/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                               CLIENT
---------------------------------------------------------------------------*/

#define PDSM_LITE_INIT_MASK 0x7F

#define PDSMCLIENT_LITE_POOL_SIZE (PDSM_LITE_CLIENT_TYPE_MAX)

/* not used Max active should be < pool_size */

#define PDSMCLIENT_LITE_POOL_CHECK 1

/* Type for PDSM client.
**
** Client structures keep a record of all the PDSM events for which their
** respective client is registered and thus call on their client callback
** functions as needed.
**
** Each PDSM client needs to allocate such a PDSM client structure in its own
** program space.
**
** Clients register and/or de-register for PDSM events dynamically via methods
** that operate on the PDSM client object that is allocated in their own
** program space.
**
** NOTE THAT NONE OF THE PDSM CLIENT STRUCTURE FIELDS SHOULD BE MODIFIED OR
** ACCESSED DIRECTLY BY THE ITS CLIENT. ANY INITIALIZATION OR MANIPULATION
** OF THE PDSM CLIENT STRUCTURE IS TO BE DONE VIA THE PDSM CLIENT METHODS
** SUPPLIED IN THIS MODULE.
*/
typedef struct pdsm_lite_client_s {

    q_link_type               link;
        /* Link element to link clients into a client-list */
        /* this element has to be the first in this structure
           because the address of link is also used as
           the "self pointer" (to save memory space by eliminating
           self_ptr in q_link_type) to access other elemnet in this
           data structure */

    byte                      init_mask;
        /* Indicate whether this object was properly initialized */

    boolean                   is_active;
        /* Indicate whether client object is active. Note that CM
        ** only notifies active clients of CM events */

     pdsm_lite_client_id_type         client_id;
    struct pdsm_lite_client_s        *client;
        /* Client ID - each initialized client struct has
        ** a unique client ID */


    pdsm_lite_pd_event_type           pd_event_mask;

    pdsm_lite_pa_event_type           pa_event_mask;
    pdsm_lite_pd_event_f_type         *pd_event_func;
        /* Function pointer to notify client of call events */
    void                         *pd_data_ptr;

    pdsm_lite_pa_event_f_type         *pa_event_func;
        /* Function pointer to notify client of call events */
    pdsm_lite_pa_event_ex_f_type      *pa_event_func_ex;
        /* Function pointer to notify client registered for all enum based PA events through pdsm_lite_client_pa_reg_ex interface */

    void                         *pa_data_ptr;
    void                         *pa_data_ptr_ex;
    pdsm_lite_pd_cmd_err_f_type       *pd_cmd_err_func;
        /* Function to notify client of call command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients - e.g. data services failed to
        ** originate because phone is offline */

    pdsm_lite_pa_cmd_err_f_type       *pa_cmd_err_func;
    pdsm_lite_pa_cmd_err_f_type       *pa_cmd_err_func_ex;
   
   pdsm_lite_xtra_event_type            xtra_event_mask;
   pdsm_lite_xtra_event_f_type          *xtra_event_func;
   pdsm_lite_xtra_cmd_err_f_type        *xtra_cmd_err_func;
   void                            *xtra_data_ptr;

   pdsm_lite_xtra_t_event_type          xtra_t_event_mask;
   pdsm_lite_xtra_t_event_f_type        *xtra_t_event_func;
   pdsm_lite_xtra_t_cmd_err_f_type      *xtra_t_cmd_err_func;
   void                            *xtra_t_data_ptr;

} pdsm_lite_client_s_type;


typedef enum pdsm_lite_cmd_type_e {
    PDSM_LITE_CMD_TYPE_NONE  = -1,      /* For internal use only */
    PDSM_LITE_CMD_TYPE_PD,              /* PD related commands from User */
    PDSM_LITE_CMD_TYPE_PARAM,
    PDSM_LITE_CMD_TYPE_XTRA,            /* XTRA related commands */
    PDSM_LITE_CMD_TYPE_XTRA_T,          /* XTRA-T related commands */
    PDSM_LITE_CMD_TYPE_DPO_STATUS_RPT,  /* DPO status report related command */
    PDSM_LITE_CMD_TYPE_EXT_CELL,        /* External Cell Info commands */
    PDSM_LITE_CMD_TYPE_MAX,             /* for internal use */
    PDSM_LITE_CMD_TYPE_E_SIZE = 0x10000000 /* Fix the enum as int */
} pdsm_lite_cmd_type_e_type;


typedef struct pdsm_lite_pd_cms_end_session_info_s
{
        /*Wherthe to turn the receiver off*/
        boolean                                            receiver_off;
}pdsm_lite_pd_cms_end_session_info_s_type;

/* This structure contains all the relevant information provided by user command.
*/
typedef struct pdsm_lite_pd_cmd_info_s {
    pdsm_lite_pd_option_s_type        option_type;
        /* PD Option type */
    pdsm_lite_pd_qos_type             qos;
        /* PD QOS specified by user */
    pdsm_lite_pd_end_e_type           end_status;            
                /* Session End status, reason for ending PD session */
        pdsm_lite_pd_cms_end_session_info_s_type session_end_info;
        /*Session end info*/
    pdsm_lite_pd_cmd_err_e_type       cmd_err;
    /* Error in command request */
    pdsm_lite_pd_external_time_info_type ext_time_info;
    /* external time info. */
    pdsm_lite_pd_external_position_info_type ext_pos_info;
    /* external position info */
} pdsm_lite_pd_cmd_info_s_type;

typedef struct pdsm_lite_pa_cmd_info_s {
  
    pdsm_lite_pa_info_type             pa_set;
        /* PD service type */
    pdsm_lite_pa_e_type                set_type;

    pdsm_lite_pa_cmd_err_e_type        cmd_err;
    /* Error in command request */

} pdsm_lite_pa_cmd_info_s_type;

/* Type for PD commands.*/
typedef struct pdsm_lite_pd_cmd_s {

    pdsm_lite_pd_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */
          
    void                          *data_block_ptr;
        /* pointer to a client provided callback data block */

    pdsm_lite_client_id_type           client_id;
        /* Id of the client sending this command */
    pdsm_lite_client_s_type            *client;

    pdsm_lite_pd_cmd_e_type            cmd;
        /* pdsm command */

    pdsm_lite_pd_cmd_info_s_type       info;
        /* pdsm command information */

} pdsm_lite_pd_cmd_s_type;

/* Type for XTRA commands.*/

/* structure coming down from PDAPI */
typedef struct pdsm_lite_xtra_cmd_info_s {
  /* XTRA ptr to assistance data info */
  byte*                         xtra_data_ptr;
  /* length of data pointed to by data_ptr */
  uint32                        xtra_data_len;
  /* result of download by XTRA.DC */
  pdsm_lite_xtra_dc_status_e_type    xtra_dc_status;
  /* file must be broken into parts by application */
  /* this parameter indicates the current part number */
  byte                          part_number;
  /* total number of parts to be sent by the application */
  byte                          total_parts;
  /* enable/disable XTRA */   
  boolean                       enable_xtra;                  // NOTE - This is not used by the client
  /* enable/disable auto downloads */
  boolean                       enable_auto_download;
  /* download interval (hours) */
  uint16                        download_interval;
  /* Error in command request */

  /* cummulative GPS time (reference date: Jan 6, 1980) (msec) */
  double                        d_TimeMsec;
  /* uncertainty assoc. w/ time (msec) */
  float                         f_TimeUncMsec;
  /* time is referenced to UTC */
  boolean                       b_RefToUtcTime;
  /* force acceptance of time data */
  boolean                       b_ForceFlag;

  pdsm_lite_xtra_cmd_err_e_type      cmd_err;
} pdsm_lite_xtra_cmd_info_s_type;

typedef struct pdsm_lite_xtra_cmd_s {

  pdsm_lite_xtra_cmd_cb_f_type         *cmd_cb_func;
  /* pointer to a client provided callback function */

  void                          *data_block_ptr;
  /* pointer to a client provided callback data block */

  pdsm_lite_client_id_type           client_id;
  /* Id of the client sending this command */
  pdsm_lite_client_s_type            *client;

  pdsm_lite_xtra_cmd_e_type            cmd;
  /* pdsm command */

  pdsm_lite_xtra_cmd_info_s_type       info;
  /* pdsm command information */

} pdsm_lite_xtra_cmd_s_type;


/* ----------------------- XTRA-T ------------------------------------------- */
/* XTRA-T Command Info from PDAPI */
typedef struct pdsm_lite_xtra_t_cmd_info_s 
{
   /* Aries 5.0  - save the command info from XTRA-T client here */

   /* pdsm_lite_xtra_t_get_data related */
   pdsm_lite_xtra_t_get_s_type          get_data;

   /* Error in command request */
   pdsm_lite_xtra_t_cmd_err_e_type      cmd_err;

} pdsm_lite_xtra_t_cmd_info_s_type;


/* XTRA-T Command from PDAPI */
typedef struct pdsm_lite_xtra_t_cmd_s 
{
  pdsm_lite_xtra_t_cmd_cb_f_type     *cmd_cb_func;   /* pointer to a client provided callback function */
  void                          *data_block_ptr;/* pointer to a client provided callback data block */
  pdsm_lite_client_id_type            client_id;     /* Id of the client sending this command */
  pdsm_lite_client_s_type            *client;       

  pdsm_lite_xtra_t_cmd_e_type         cmd;           /* pdsm command */

  pdsm_lite_xtra_t_cmd_info_s_type    info;          /* pdsm command information */
} pdsm_lite_xtra_t_cmd_s_type;
/* Type for PA commands.*/
typedef struct pdsm_lite_pa_cmd_s {

    pdsm_lite_pa_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */
          
    void                       *data_block_ptr;
        /* pointer to a client provided callback data block */

    pdsm_lite_client_id_type        client_id;
        /* Id of the client sending this command */

    pdsm_lite_client_s_type         *client;

    pdsm_lite_pa_cmd_e_type            cmd;
        /* pdsm command */

    pdsm_lite_pa_cmd_info_s_type       info;
        /* pdsm command information */

} pdsm_lite_pa_cmd_s_type;

typedef struct {
  boolean enable;
}pdsm_lite_dpo_status_rpt_cmd_s_type;

/* external cell info command type */
typedef struct {
   /**< id of caller */
   pdsm_lite_client_id_type        client_id;

   /**< callback notification function */
   pdsm_lite_pd_cmd_cb_f_type      *cmd_cb_func;

   /**< client provided reference data */
   void*                      client_data_ptr;

   /**< injected data: external cell info */
   pdsm_lite_cell_info_s_type      cell_info;

} pdsm_lite_ext_cell_data_cmd_s_type;

typedef union pdsm_lite_cmd_u {
    pdsm_lite_pd_cmd_s_type      pd;
    pdsm_lite_pa_cmd_s_type      pa;
    pdsm_lite_xtra_cmd_s_type      xtra;
    pdsm_lite_xtra_t_cmd_s_type    xtra_t;
    pdsm_lite_dpo_status_rpt_cmd_s_type dpo_status_rpt;
    pdsm_lite_ext_cell_data_cmd_s_type ext_cell;
} pdsm_lite_cmd_u_type;

typedef struct pdsm_lite_cmd_s  {
    q_link_type             link;
    pdsm_lite_cmd_type_e_type    cmd_type;
    pdsm_lite_cmd_u_type         cmd;
} pdsm_lite_cmd_s_type;

/*===========================================================================

FUNCTION pdsm_lite_client_event_ntfy

DESCRIPTION
  Notify a client of a specified phone event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsm_lite_client_event_ntfy(

    const pdsm_lite_client_s_type  *client_ptr,
        /* pointer to a client struct */

    pdsm_lite_pd_event_type         pd_event,
        /* notify client of this phone event */

    const pdsm_lite_pd_info_s_type          *pd_info_ptr
        /* pointer to PD information struct */
);
/*===========================================================================

FUNCTION pdsm_lite_client_pa_event_ntfy_ex

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

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
  
);


/*===========================================================================

FUNCTION pdsm_lite_client_pa_event_ntfy

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsm_lite_client_pa_event_ntfy(

    const pdsm_lite_client_s_type    *client_ptr,
        /* pointer to a client struct */

    pdsm_lite_pa_event_type          pa_event,
        /* notify client of this PD event */

    const pdsm_lite_pa_info_s_type   *pa_info_ptr
        /* pointer to pdsm state struct */
);



/* <EJECT> */
/*---------------------------------------------------------------------------
                                CLIENT LIST
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION pdsmclient_lite__list_init

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
extern void pdsmclient_lite_list_init( void );



/*===========================================================================

FUNCTION pdsmclient_lite__list_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_lite__list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_lite_list_event_ntfy(

    pdsm_lite_pd_event_type   pd_event,
        /* notify clients of this PD event */

    const pdsm_lite_pd_info_s_type    *pd_info_ptr
        /* pointer to PD information struct */
);

/*===========================================================================

FUNCTION PDSMCLIENT_LITE_REG_LIST_PD_EVENT_NTFY

DESCRIPTION
  This function is used to notify all registered clients other than active clients
  about the PD events.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern void pdsmclient_lite_reg_list_pd_event_ntfy( pdsm_lite_pd_event_type,const pdsm_lite_pd_info_s_type *info_ptr, pdsm_lite_client_s_type **list_ptr);

/*===========================================================================

FUNCTION PDSMCLIENT_LITE_LIST_PD_EVENT_NTFY

DESCRIPTION
  This function is used inform client list about the PD events.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern void pdsmclient_lite_list_pd_event_ntfy( pdsm_lite_pd_event_type,const pdsm_lite_pd_info_s_type *info_ptr );

/*===========================================================================

FUNCTION pdsmclient_lite__list_pa_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PA event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_lite__list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_lite_list_pa_event_ntfy(

  pdsm_lite_pa_event_type   pa_event,
      /* notify clients of this PD event */

  const pdsm_lite_pa_info_s_type    *pa_info_ptr,
      /* pointer to PD information struct */

  pdsm_lite_pa_cmd_err_e_type   pa_cmd_err
    /*CMD Error*/

);


/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_lite__list_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_lite__list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_lite_list_pd_cmd_err_ntfy(

    pdsm_lite_pd_cmd_err_e_type      pd_cmd_err,
        /* PD command error code */

    const pdsm_lite_pd_cmd_s_type    *pd_cmd_ptr
        /* pointer to PD command struct */

);


/*===========================================================================

FUNCTION pdsmclient_lite__list_pa_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_lite__list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_lite_list_pa_cmd_err_ntfy(

    pdsm_lite_pa_cmd_err_e_type      pa_cmd_err,
        /* PD command error code */

    const pdsm_lite_pa_cmd_s_type    *pa_cmd_ptr
        /* pointer to PD command struct */

);

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_lite_client_event_ntfy

DESCRIPTION
  Notify a client of a specified PD event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsm_lite_client_event_ntfy(

    const pdsm_lite_client_s_type       *client_ptr,
        /* pointer to a client struct */

    pdsm_lite_pd_event_type         pd_event,
        /* notify client of this PD event */

    const pdsm_lite_pd_info_s_type    *pd_info_ptr
        /* pointer to PD information struct */
);
/*===========================================================================

FUNCTION pdsmclient_lite__free_q_init

DESCRIPTION
  Initialize the client free queue.

  This function must be called before the pdsmclinet free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_lite_free_q_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_lite__get_client_ptr

DESCRIPTION
  Returns a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object

SIDE EFFECTS
  None

===========================================================================*/
extern pdsm_lite_client_s_type* pdsmclient_lite_get_client_ptr( 
    
    pdsm_lite_client_id_type client_id
    /* Client Id */
);

/*===========================================================================

FUNCTION PDSMCLIENT_LITE_VALID_CLIENT_ID

DESCRIPTION
  This function is used to validate a client handle.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern boolean pdsmclient_lite_valid_client_id( pdsm_lite_client_id_type client_id );

/*===========================================================================

FUNCTION pdsmclient_lite__get_client_type

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
extern pdsm_lite_client_type_e_type pdsmclient_lite_get_client_type_map( pdsm_lite_client_id_type client_id );

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_lite__get_client_id

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
extern pdsm_lite_client_id_type pdsmclient_lite_get_client_id( 
    
    pdsm_lite_client_s_type *client_ptr
    /* Client object pointer */
);

/*===========================================================================

FUNCTION PDSMCLIENT_LITE_Q_INIT

DESCRIPTION
  This function is used for Q initialization

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void pdsmclient_lite_q_init(void);   


/*===========================================================================

FUNCTION PDSMCLIENT_LITE_INFO_ALLOC

DESCRIPTION
  This function is used to allocate info buffer for sending Event reports back to clients.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern pdsm_lite_pd_info_s_type *pdsmclient_lite_info_alloc( pdsm_lite_client_s_type   *client_ptr );

/*===========================================================================
FUNCTION pdsmclient_lite__get_client_type

DESCRIPTION
  This function provides Client ID based on client type
  
DEPENDENCIES
  None

RETURN VALUE
  Client type for the client ptr

SIDE EFFECTS
  None

===========================================================================*/
extern pdsm_lite_client_id_type pdsmclient_lite_get_client_id_map(
  pdsm_lite_client_type_e_type client_type
    /* Client Id */
);
/*===========================================================================

FUNCTION PDAPI_INIT

DESCRIPTION
  Initialize the Position Determination API - called from the PD task

RETURN VALUE
  None

DEPENDENCIES
  Called only once from PDSM_LITE_TASK or lsm_task

===========================================================================*/

extern void pdapi_lite_init( void );


/*===========================================================================

FUNCTION PDSM_LITE_XTRA_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/

extern boolean pdsm_lite_xtra_client_request
(
  pdsm_lite_xtra_event_type      xtra_event,        /* Type of XTRA event */
  pdsm_lite_xtra_info_s_type     *xtra_info_ptr     /* data to be sent to XTRA client */
);



/*===========================================================================

FUNCTION PDSM_LITE_XTRA_T_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA-T client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/
extern boolean pdsm_lite_xtra_t_client_request
(
 pdsm_lite_xtra_t_event_type      xtra_t_event,        /* Type of XTRA-T event */
 pdsm_lite_xtra_t_info_s_type     *xtra_t_info_ptr     /* data to be sent to XTRA-T client */
);





#ifdef __cplusplus
}
#endif


#endif /* PDSMCLIENT_LITE_H */


