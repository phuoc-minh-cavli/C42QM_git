/*===========================================================================

                         D S _ Q M I _ A T . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Atcop Service source
 file.

EXTERNALIZED FUNCTIONS

  qmi_at_init()
    Initialize the QMI AT service
    

Copyright (c) 2009 - 2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_ATCOP
#ifdef FEATURE_DATA_QMI_AT
#include <stdio.h>

#include "dsm.h"
#include "nv.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "queue.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_svc_ext.h"
#include "ds_qmi_task.h"
#include "dstaski.h"
#include "ps_in.h"
#include "ds_system_heap.h"
#include "data_msg.h"
#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "ds_qmi_svc.h"
#include "qmi_framework.h"
#include "ds_qmi_at.h"
#include "ds_qmi_fw_common.h"
#include "access_terminal_service_v01.h"
#include "ps_utils.h"
#include <stringl/stringl.h>
#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "qmi_sap.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "common_v01.h"
#include "dsatclient.h"



/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define ATI_MAX_CLIDS    (3)
#define ATI_INVALID_CLIENT_INDEX 255
#define ATI_INVALID_INSTANCE    -1


/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/

#define QMI_AT_FREE_CMD_REG_BUF(cmd_reg_buf)                  \
  do                                                          \
  {                                                           \
    if(cmd_reg_buf)                                           \
    {                                                         \
      if(cmd_reg_buf->at_cmd_list_ptr)                        \
      {                                                       \
        DS_SYSTEM_HEAP_MEM_FREE(cmd_reg_buf->at_cmd_list_ptr);\
        cmd_reg_buf->at_cmd_list_ptr = NULL;                  \
      }                                                       \
      DS_SYSTEM_HEAP_MEM_FREE(cmd_reg_buf);                   \
      cmd_reg_buf = NULL;                                     \
    }                                                         \
  }while(0);

#define ATI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DS_AT_RDR_SIGNAL))

/*---------------------------------------------------------------------------
  AT client registartion state enum type 
  When there is a client registration still pending we move AT registration
  to PENDING.
---------------------------------------------------------------------------*/
typedef enum
{
  ATI_AT_REG_NONE ,
  ATI_AT_REG_DONE
}qmi_ati_at_reg_e_type;

typedef struct
{
  dsat_cmd_list_type *at_cmd_list_ptr;
  uint32 cnt;
  qmi_client_handle client;
  qmi_req_handle pending_cmd_buf;
}qmi_ati_cmd_reg_dereg_buf_type;

typedef qmi_ati_cmd_reg_dereg_buf_type qmi_ati_cmd_reg_buf_type;
typedef qmi_ati_cmd_reg_dereg_buf_type qmi_ati_cmd_dereg_buf_type;



/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI AT Client state info
---------------------------------------------------------------------------*/

typedef struct qmi_ati_client_reg_cmd
{
  q_link_type  link;
  byte         cmd_name[MAX_CMD_SIZE];
}qmi_ati_client_reg_cmd_type;

typedef struct
{
  q_type qmi_ati_reg_cmd_list_q;
  uint8  qmi_ati_reg_cmd_cnt;
}qmi_ati_client_reg_cmd_list_q_type;

typedef struct
{
  q_link_type        link; /* Data Structure required by the queue 
                              library to add this as part of a queue */
  qmi_client_handle  clnt;
  qmi_ati_client_reg_cmd_list_q_type reg_cmd_list;
  qmi_ati_cmd_reg_buf_type * pending_info;
  qmi_ati_cmd_dereg_buf_type * dereg_pending_info;
  boolean is_legacy_reg;  // Flag indicating if registration is for legacy client
  boolean reg_cmdlist_support_token_ex;
}qmi_ati_client_info_type;


/*---------------------------------------------------------------------------
  QMI AT Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_dsdi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_ati_state_info_type;

static qmi_ati_state_info_type       qmi_at_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_ati_inited = FALSE;

typedef struct
{
  qmi_req_handle req_handle;
  qmi_ati_client_info_type* cl_sp;
  int             msg_id;
}ds_qmi_ati_async_cmd_type;

typedef struct
{
  uint16 cmd_id; /* qmi_cmd_id_e_type */
  union
  {

	struct
    {
      boolean  status;
      uint32   client_id;
      void *   user_data;
    } client_reg;

    struct
    {
      boolean  status;
      void *   user_data;
    } cmd_reg;

    struct
    {
      boolean  status;
      void *   user_data;
    } cmd_dereg;

    struct
    {
      uint32  handle;
      void *  user_data;
    } cmd_abort;
  } data;
} qmi_ati_cmd_buf_type;

typedef struct
{
  uint16 cmd_id; /* qmi_cmd_id_e_type */
  /* Memory will be allocated on heap of size MAX_LINE_SIZE*/
  byte *args;
  byte name[MAX_CMD_SIZE];
  dsat_cmd_state_type  at_state;
  uint8 num_args;
  unsigned int op;
  uint32  at_handle;
  uint8   is_support_token_ex;
  void * user_data;
} qmi_ati_fwd_cmd_buf_type;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/


/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

static qmi_csi_cb_error qmi_ati_reset(
  qmi_ati_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);



static qmi_csi_cb_error qmi_ati_send_at_repsonse(
  qmi_ati_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_ati_send_at_urc(
  qmi_ati_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_ati_reg_at_cmd_fwd_ex(
  qmi_ati_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_ati_dereg_at_cmd_fwd_ex(
  qmi_ati_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static boolean qmi_ati_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_ati_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_ati_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_ati_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);


static void qmi_ati_fwd_at_cmd_ind_ex( uint32, uint32, byte*,
              dsat_cmd_state_type, uint8, byte*, uint8, qmi_ati_client_info_type*);
static void qmi_ati_send_at_cmd_abort_ind( uint32, void* );

static void qmi_ati_reset_client
(
   qmi_ati_client_info_type        *clnt_info
);

static void qmi_ati_reg_at_cmd_cback
(
  boolean     cmd_status,
  uint32      client_id,
  void      * user_data   
);
static void qmi_ati_dereg_at_cmd_cback
(
  boolean     cmd_status,
  uint32      client_id,
  void      * user_data   
);
static boolean qmi_ati_fwd_at_cmd_cback
(
  dsat_mode_enum_type   at_cmd_mode,
  const tokens_struct_type    *token_ptr,
  const dsat_cmd_state_type    at_state,    
  uint32                atcop_info,
  uint8                 is_support_token_ex,
  void                  *user_info_ptr
);

static void qmi_ati_abort_at_cmd_cback
(
  void *  user_info_ptr,
  uint32  at_handle
);


static struct
{ 
  boolean inited;
  qmi_ati_at_reg_e_type reg_at_client;
  qmi_ati_client_info_type * abort_client;
  uint32 atcop_cl_id;
} qmi_ati_global = {FALSE,};



/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for AT messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_ati_client_info_type       *clnt_info,
 qmi_req_handle           		req_handle,
 int                      		msg_id,
 void                    		*req_c_struct,
 int                      		req_c_struct_len,
 void                    	    *service_handle
) = 
{
  qmi_ati_reset,							/*Request handler for message ID 0x00 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,  		/* Request handler for message ID 0x01 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x18 - 0x1F*/
  NULL,					                    /* Request handler for message ID 0x20 */
  NULL,					                    /* Request handler for message ID 0x21 */
  qmi_ati_send_at_repsonse,                 /* Request handler for message ID 0x22 */
  NULL,   							        /* Request handler for message ID 0x23 */
  qmi_ati_send_at_urc,                      /* Request handler for message ID 0x24 */
  qmi_ati_reg_at_cmd_fwd_ex,                /* Request handler for message ID 0x25 */
  NULL,					                    /* Request handler for message ID 0x26 */
  qmi_ati_dereg_at_cmd_fwd_ex               /* Request handler for message ID 0x27 */
  
};



static int ati_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_ati_client_info_type *clnt_info = (qmi_ati_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int ati_compare_connection_info_handle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (p_ItemPtr == p_CompareVal) ? 1 : 0 );
}


/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_AT_INIT()

  DESCRIPTION
    Register the AT service with QMI Framework

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_at_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;
  qmi_csi_options       options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,"QMI AT service init %d", qmi_ati_inited);


  if (!qmi_ati_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)reg_set_sig_handler(DS_AT_RDR_SIGNAL, 
                              qmi_ati_msg_hdlr_cb, &qmi_at_state);
    //ds_enable_sig_ex(DS_AT_RDR_SIGNAL);

    /* Set the signal to init again */
    //DS_SET_SIGNAL(DS_AT_RDR_SIGNAL);
    (void)rex_set_sigs( ds_get_ds_task_ptr(),
                      (rex_sigs_type) 1 << (rex_sigs_type)(DS_AT_RDR_SIGNAL) ); 
    return;
  }
  
  os_params.tcb = ds_get_ds_task_ptr();
  os_params.sig = ATI_QCSI_OS_SIGNAL;
  
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);
  
  /*-------------------------------------------------------------------------
    Register QMI AT service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(at_get_service_object_v01(),
                        qmi_ati_svc_connect_cb,
                        qmi_ati_svc_disconnect_cb, 
                        qmi_ati_handle_client_req_cb, 
                        &qmi_at_state, &os_params, 
                        &options,
                        &qmi_at_state.service_handle);
  
  DS_MSG1(MSG_LEGACY_HIGH,"qmi_csi_register %d", rc);
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register AT service! Error %d", rc, 0, 0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(at_get_service_object_v01(), NULL, &qmi_at_state.sap_client_handle);
  DS_MSG1(MSG_LEGACY_HIGH,"qmi_sap_register %d", src);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register AT service with QSAP! Error %d", src, 0, 0);
    return;
  }


  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_at_state.client_q);
  qmi_at_state.num_of_clients = 0;

   if(qmi_ati_global.inited == FALSE)
  {
    memset(&qmi_ati_global, 0, sizeof(qmi_ati_global));
    qmi_ati_global.inited = TRUE;
    qmi_ati_global.atcop_cl_id = DSAT_INVALID_CLIENT_ID;
    qmi_ati_global.abort_client = NULL;

    /* Initialize the queue which will hold AT forwarding command list */
    (void)q_init(&qmi_at_fwd_list.at_fwd_cmd_q);
    qmi_at_fwd_list.at_fwd_cmd_cnt = 0;
  }
  
} /* qmi_at_init */

static boolean qmi_ati_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_ati_state_info_type *at_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((sig == DS_AT_RDR_SIGNAL) && svc_ptr);

  if (!qmi_ati_inited)
  {
    qmi_ati_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_at_init();
    return TRUE;
  }

  at_sp = (qmi_ati_state_info_type*) svc_ptr;

  qmi_csi_handle_event(at_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_ati_msg_hdlr_cb */


static qmi_csi_cb_error qmi_ati_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_ati_client_info_type    *clnt_info_ptr =NULL;
  qmi_ati_state_info_type     *at_svc_ptr;
  qmi_csi_cb_error             rc;
  boolean                      ret_val = FALSE;
  uint32                       cl_sp = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(connection_handle && service_handle );
  DS_MSG0(MSG_LEGACY_HIGH,"ati_svc_connect_cb: ");


  at_svc_ptr = (qmi_ati_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &at_svc_ptr->client_q,
                                ati_compare_qmi_client_handle,
                                client_handle ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ati_svc_connect_cb: AT client 0x%p present",
                    client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (ATI_MAX_CLIDS == at_svc_ptr->num_of_clients)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ati_svc_connect_cb: Maximum number of clients already allocated %d",
                    ATI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr, sizeof(qmi_ati_client_info_type),
                           qmi_ati_client_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }
  
	if(qmi_ati_global.reg_at_client == ATI_AT_REG_NONE)
    {
    ret_val = dsatclient_register_fwd_client(&cl_sp);
      
        if (ret_val == FALSE)
        {
          qmi_ati_global.reg_at_client = ATI_AT_REG_NONE;
          DS_MSG0(MSG_LEGACY_ERROR,"Alloc clid is failed.Unable to register with ATCoP");
        rc = QMI_CSI_CB_CONN_REFUSED;
        goto bail;
    }

    qmi_ati_global.atcop_cl_id = cl_sp;
    qmi_ati_global.reg_at_client = ATI_AT_REG_DONE;
  } 
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Already registered/pending with ATcop: %d",
                      qmi_ati_global.reg_at_client);
    } 	

  
  memset(clnt_info_ptr, 0, sizeof(qmi_ati_client_info_type));
  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  
  /* Add it to the queue of clients */
  q_put( &(at_svc_ptr->client_q), &(clnt_info_ptr->link) );
  at_svc_ptr->num_of_clients++;

  (void)q_init(&clnt_info_ptr->reg_cmd_list.qmi_ati_reg_cmd_list_q);
  clnt_info_ptr->reg_cmd_list.qmi_ati_reg_cmd_cnt = 0;

  *connection_handle = clnt_info_ptr;
  rc = QMI_CSI_CB_NO_ERR;

  bail:
  return rc;
} /* qmi_ati_svc_connect_cb() */

static void qmi_ati_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_ati_state_info_type    *at_svc_ptr;
  qmi_ati_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);
  at_svc_ptr = (qmi_ati_state_info_type *)service_handle;

  DS_MSG1(MSG_LEGACY_HIGH,"ati_svc_disconnect_cb: Releasing client 0x%p",
                  connection_handle);

  clnt_info_ptr = q_linear_search( &at_svc_ptr->client_q,
                                   ati_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ati_svc_connect_cb: AT client 0x%p not found",
                    connection_handle);
    return;

  }

  qmi_ati_reset_client(clnt_info_ptr);
  

  if ( TRUE != q_delete_ext( &at_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ati_svc_connect_cb: AT client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  DS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  at_svc_ptr->num_of_clients--;
} /* qmi_ati_svc_disconnect_cb() */

static qmi_csi_cb_error qmi_ati_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
)
{
  qmi_csi_cb_error               rc;
  qmi_ati_client_info_type     *clnt_info_ptr;
  qmi_ati_state_info_type      *at_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);

  rc = QMI_CSI_CB_NO_ERR;
  at_svc_ptr = (qmi_ati_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &at_svc_ptr->client_q,
                                   ati_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ati_handle_req_cb: Invalid clnt 0x%p",
                    connection_handle);
    rc = QMI_CSI_CB_INTERNAL_ERR;
    return rc;
  }
  
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_ati_handle_client_req_cb: client 0x%p MSG ID %d",
					clnt_info_ptr,msg_id);
  if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    if(req_handle_table[msg_id])
    {
      rc = req_handle_table[msg_id] (clnt_info_ptr, req_handle, msg_id,
                                     req_c_struct, req_c_struct_len,
                                     service_handle);
    }
    else
    {
      rc = QMI_CSI_CB_UNSUPPORTED_ERR;
      DS_MSG1(MSG_LEGACY_ERROR,"ati_handle_req_cb: NULL message ID handler: %d",
                      msg_id);
    }
  }
  else
  {
    rc = QMI_CSI_CB_UNSUPPORTED_ERR;
    DS_MSG1(MSG_LEGACY_ERROR,"ati_handle_req_cb: Invalid message ID: %d", msg_id);
  }
    DS_MSG2(MSG_LEGACY_HIGH,"qmi_ati_handle_client_req_cb: client 0x%p rc %d",
					clnt_info_ptr,rc);

  return rc;
} /* qmi_ati_handle_client_req_cb() */


/*===========================================================================
  FUNCTION QMI_ATI_PROCESS_AT_CLIENT_REG_STATUS()

  DESCRIPTION
    

  PARAMETERS
    

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ati_process_at_client_reg_status
(
  boolean status,
  uint32   client_id,
  void *userdata
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((status == FALSE))
  {
    DS_ERR_FATAL("Unable to handle status req, AT svc not initialized");
  }

  DS_MSG2(MSG_LEGACY_HIGH,"qmi_ati_process_at_client_reg_status() cld id %d, status %d",client_id,status);
  qmi_ati_global.atcop_cl_id = client_id;
  qmi_ati_global.reg_at_client = ATI_AT_REG_DONE;

} /* qmi_ati_process_at_client_reg_status */

/*===========================================================================
  FUNCTION QMI_ATI_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMI AT command buffer from the PS MEM heap
    based on the QMI CMD type
    
  PARAMETERS
    cmd - QMI command type
   
  RETURN VALUE
    cmd_buf_ptr - Pointer to the allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *qmi_ati_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
)
{
   ds_command_type                  *cmd_ptr =NULL;

  DS_MSG1(MSG_LEGACY_HIGH,"ati_get_cmd_buf cmd_id = %d",cmd_id);
  switch (cmd_id)
  {
    case QMI_CMD_AT_CMD_REG_STATUS:
    case QMI_CMD_AT_CMD_DEREG_STATUS:
    case QMI_CMD_AT_ABORT_CMD:
    case QMI_CMD_AT_CMD_HDLR_CB:
    {
      /*------------------------------------------------------
        Get a DS command buffer
       --- --------------------------------------------------*/
       
   cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_ati_cmd_buf_type));
   if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
   {
     DS_ERR_FATAL("Out of memory");
     return NULL;
   }
   return ((void *)cmd_ptr);
    }

    case QMI_CMD_FWD_AT_CMD:
    {
    /*------------------------------------------------------
        Get a DS command buffer
        --- --------------------------------------------------*/
     cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_ati_fwd_cmd_buf_type));
      if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
     {
       DS_ERR_FATAL("Out of memory");
       return NULL;
      }
      return ((void *)cmd_ptr);
    }

    default:
	  break;
  }
  return NULL;
} /* qmi_ati_get_cmd_buf */

/*===========================================================================

FUNCTION QMI_ATI_STRCMP_IG_SP_CASE

DESCRIPTION
  This function compares two null terminated strings, ignoring case.
  Lower case letters are converted to upper case before the 
  difference is taken.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical, except for case
            otherwise, it returns the difference between the first 
            characters to disagree. The difference is first arg 
            minus second arg.  

SIDE EFFECTS
  None.

===========================================================================*/

int qmi_ati_strcmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2              /*  Pointer to string 2 */
)
{
  byte c1, c2;
  int d;

  do
  {
    c1 = *s1++;
    c2 = *s2++;

    d = UPCASE(c1) - UPCASE(c2);
  }
  while ( d == 0 && c1 != '\0' && c2 != '\0');

  return d;
} /*  qmi_ati_strcmp_ig_sp_case  */

/*===========================================================================

FUNCTION QMI_ATI_STRNCMP_IG_SP_CASE

DESCRIPTION
  This function compares two strings for <n> characters, ignoring case.
  Lower case letters are converted to upper case before the 
  difference is taken.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical till <n> characters, except for 
            case otherwise, it returns the difference between 
            the first characters to disagree. The difference is first arg 
            minus second arg.  

SIDE EFFECTS
  None.

===========================================================================*/
int qmi_ati_strncmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2,             /*  Pointer to string 2 */
  uint32      n               /* Number of characters to compare */
)
{
  byte c1, c2;
  int d;

  if ( n== 0 )
  {
    return -1;
  }
  do
  {
    c1 = *s1++;
    c2 = *s2++;

    d = UPCASE(c1) - UPCASE(c2);
    --n;
  }
  while ( d == 0 && c1 != '\0' && c2 != '\0' && n != 0 );

  return d;
}/* qmi_ati_strncmp_ig_sp_case */


/*===========================================================================
  FUNCTION QMI_ATI_RESET()

  DESCRIPTION
    Reset the issuing AT client's state
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_ati_reset
(
  qmi_ati_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                      			msg_id,
  void                   		   *req_c_struct,
  int                      			req_c_struct_len,
  void                     		   *service_handle
)
{
	 at_reset_resp_msg_v01        resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(at_reset_resp_msg_v01));

  /*-------------------------------------------------------------------------
    Reset the client's state
  -------------------------------------------------------------------------*/
  
  qmi_ati_reset_client(clnt_info);
  
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(at_reset_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_ATI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    clnt_info : client info
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ati_reset_client
(
   qmi_ati_client_info_type        *clnt_info
)

{
  qmi_ati_client_info_type    *client_info_ptr;
  qmi_ati_cmd_reg_buf_type    *cmd_reg_buf = NULL;
  int                          client = 0;
  dsat_cmd_list_type           reg_cmd_list[MAX_CMD_LIST] ;
  qmi_ati_client_reg_cmd_type *reg_cmd_p = NULL;
  qmi_ati_client_reg_cmd_type *temp_reg_cmd_p = NULL;
  uint32 count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_MSG1(MSG_LEGACY_HIGH, "qmi_ati_reset_client 0x%p",clnt_info );
  memset(&reg_cmd_list,0,sizeof(dsat_cmd_list_type)*MAX_CMD_LIST);
  
  /* Clear any pending transactions */
  if(clnt_info->pending_info != NULL)
  {
    cmd_reg_buf = clnt_info->pending_info;
    clnt_info->pending_info = NULL;
	 
    /* Make sure that the pending registration for commands have corresponding de-registeration */
    if( dsatclient_deregister_fwd_at_cmd(qmi_ati_global.atcop_cl_id,
                                         cmd_reg_buf->cnt,
                                         cmd_reg_buf->at_cmd_list_ptr,
                                         NULL,
                                         NULL) == FALSE )
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Error de-registering pending commands for cleint =0x%p",clnt_info );
      DS_ASSERT(0);
    }
    QMI_AT_FREE_CMD_REG_BUF(cmd_reg_buf);
  }

  /* Clear the stored registerd List of commands for this client. */
  /* Find out number of registered commands */
  if (clnt_info->reg_cmd_list.qmi_ati_reg_cmd_cnt > 0)
  {
    while (clnt_info->reg_cmd_list.qmi_ati_reg_cmd_cnt > 0)
    {
      memset(reg_cmd_list, 0x0, sizeof(dsat_cmd_list_type) * MAX_CMD_LIST);
      reg_cmd_p = (qmi_ati_client_reg_cmd_type *)q_check(&clnt_info->reg_cmd_list.qmi_ati_reg_cmd_list_q);
      count = 0;
      /* Deregister commands in chunks , each chunk is of 20 commands.
       * This is for backward compatability with dsatclient
       */
      while(reg_cmd_p != NULL && count < MAX_CMD_LIST)
      {
        memscpy(reg_cmd_list[count].cmd_buff, 
				  MAX_CMD_SIZE,
				  reg_cmd_p->cmd_name,
				  strlen((const char*)reg_cmd_p->cmd_name));
        reg_cmd_list[count].cmd_buff_len = strlen((const char*)reg_cmd_list[count].cmd_buff);
  
        temp_reg_cmd_p = reg_cmd_p;
        reg_cmd_p = q_next(&clnt_info->reg_cmd_list.qmi_ati_reg_cmd_list_q,
                           &(reg_cmd_p->link));
        if (TRUE == q_delete_ext( &clnt_info->reg_cmd_list.qmi_ati_reg_cmd_list_q,
                                  &temp_reg_cmd_p->link ) )
        {
          clnt_info->reg_cmd_list.qmi_ati_reg_cmd_cnt--;
          DS_SYSTEM_HEAP_MEM_FREE(temp_reg_cmd_p);
          temp_reg_cmd_p = NULL;
        }
        else
        {
          ASSERT(0);
        }
        count++;
      }
 
      if ( count > 0)
      {
        if( dsatclient_deregister_fwd_at_cmd(qmi_ati_global.atcop_cl_id,
                                             count,
                                             reg_cmd_list,
                                             NULL,
                                             NULL) == FALSE )
        {
          DS_MSG1(MSG_LEGACY_ERROR, "Error de-registering commands for cleint = %d",client);
          ASSERT(0)
        }
      }
    }
  }
  /* free the client command list queue */
  (void)q_destroy(&clnt_info->reg_cmd_list.qmi_ati_reg_cmd_list_q);

  client_info_ptr = (qmi_ati_client_info_type*)q_check(&qmi_at_state.client_q);
  while(client_info_ptr != NULL)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Reset client 0x%p",client_info_ptr);
    if ( client_info_ptr->reg_cmd_list.qmi_ati_reg_cmd_cnt > 0) 
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Reset client cmd cnt %d ",client_info_ptr->reg_cmd_list.qmi_ati_reg_cmd_cnt);
      break;
    }
    client++;
    client_info_ptr = q_next(&qmi_at_state.client_q,
                             &(client_info_ptr->link));
  }
  
  if ((client >= ATI_MAX_CLIDS) && (qmi_ati_global.reg_at_client == ATI_AT_REG_DONE))
  {
    /* Last client to Reset; Deregister with  ATCoP  */
    DS_MSG0(MSG_LEGACY_HIGH,"Deregistering with Atcop");
    if(dsatclient_deregister_fwd_client(qmi_ati_global.atcop_cl_id) != TRUE)
    {
      DS_ERR_FATAL("Unable to deregister with ATCoP");
    }
    qmi_ati_global.reg_at_client = ATI_AT_REG_NONE;
    qmi_ati_global.atcop_cl_id = DSAT_INVALID_CLIENT_ID;
  }


} /* qmi_ati_reset_client() */

/*===========================================================================
  FUNCTION QMI_ATI_REG_AT_CMD_FWD_EX()

  DESCRIPTION
    Reg AT cmds for forwarding

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
static qmi_csi_cb_error qmi_ati_reg_at_cmd_fwd_ex
(
 
  qmi_ati_client_info_type         *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{ 
  
   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  uint8    cnt;
  at_req_at_cmd_fwd_ex_req_v01*      req_msg;
  at_req_at_cmd_fwd_ex_resp_v01      resp_msg;
  dsat_cmd_list_type                *at_cmd_list_ptr;
  int                                errval = QMI_ERR_NONE_V01;
  qmi_ati_cmd_reg_buf_type          *cmd_reg_buf = NULL;
  ds_qmi_ati_async_cmd_type*         async_cmd_buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  DS_MSG0(MSG_LEGACY_HIGH,"qmi_ati_reg_at_cmd_fwd_ex");
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  
  req_msg = (at_req_at_cmd_fwd_ex_req_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(at_req_at_cmd_fwd_ex_resp_v01));

  
  DS_SYSTEM_HEAP_MEM_ALLOC( at_cmd_list_ptr, sizeof(dsat_cmd_list_type) * MAX_CMD_LIST,
                            dsat_cmd_list_type*);
  if (at_cmd_list_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  memset(at_cmd_list_ptr, 0, (sizeof(dsat_cmd_list_type ) * MAX_CMD_LIST));
  

  if(clnt_info->pending_info != NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"New reg at cmd req arrived when another one is pending");
    errval = QMI_ERR_INVALID_OPERATION_V01;
    goto send_result;
  }

  if( ATI_AT_REG_DONE != qmi_ati_global.reg_at_client )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"command registration is requested before AT client registration for client = 0x%p",
                             clnt_info);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  if ( req_msg->at_cmd_len > MAX_CMD_LIST )
  {
    DS_MSG2(MSG_LEGACY_ERROR, "MAX commands per request is %d requested count [%d]",
                              MAX_CMD_LIST, req_msg->at_cmd_len);
    errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
    goto send_result;
  }

  clnt_info->reg_cmdlist_support_token_ex = FALSE;
  if (req_msg->support_token_ex_valid == TRUE &&
      req_msg->support_token_ex == 1)
  {
    clnt_info->reg_cmdlist_support_token_ex = TRUE;
  }

  for (cnt = 0; cnt < req_msg->at_cmd_len; cnt++)
  {
    at_cmd_list_ptr[cnt].cmd_buff_len = strlen(req_msg->at_cmd[cnt].at_cmd_name);
    if (MAX_CMD_SIZE < strlen(req_msg->at_cmd[cnt].at_cmd_name))
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Invalid command length in TLV");
      errval = QMI_ERR_MALFORMED_MSG_V01;
      goto send_result;
    }
    memscpy((byte*)at_cmd_list_ptr[cnt].cmd_buff, 
            MAX_CMD_SIZE,
            req_msg->at_cmd[cnt].at_cmd_name,
            strlen(req_msg->at_cmd[cnt].at_cmd_name));
    at_cmd_list_ptr[cnt].is_abortable = req_msg->at_cmd[cnt].abort_flag;
    if(req_msg->support_token_ex_valid == TRUE)
    {
      at_cmd_list_ptr[cnt].is_support_token_ex = req_msg->support_token_ex;
    }
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(cmd_reg_buf, sizeof(qmi_ati_cmd_reg_buf_type),
                           qmi_ati_cmd_reg_buf_type*);
  if(cmd_reg_buf == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  memset(cmd_reg_buf, 0, sizeof( qmi_ati_cmd_reg_buf_type));	
  cmd_reg_buf->at_cmd_list_ptr = at_cmd_list_ptr;
  cmd_reg_buf->cnt = cnt;
  cmd_reg_buf->client = clnt_info->clnt;
  /*Save the cmd_buf to send response later*/
  cmd_reg_buf->pending_cmd_buf = req_handle;

  /*Save the pending buf to check with in response from atcop*/
  clnt_info->pending_info = cmd_reg_buf;

  
  /*send the async buf as the user_data to valiadate later*/
  
  DS_SYSTEM_HEAP_MEM_ALLOC(async_cmd_buf, sizeof( ds_qmi_ati_async_cmd_type),
                           ds_qmi_ati_async_cmd_type*);
  if(async_cmd_buf == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  memset(async_cmd_buf, 0, sizeof( ds_qmi_ati_async_cmd_type));	
  async_cmd_buf->req_handle = req_handle;
  async_cmd_buf->cl_sp = clnt_info;
  async_cmd_buf->msg_id = msg_id;

  if(FALSE == dsatclient_register_fwd_at_cmd
                (
                  qmi_ati_global.atcop_cl_id,
                  cnt,
                  at_cmd_list_ptr,
                  qmi_ati_reg_at_cmd_cback,
                  qmi_ati_fwd_at_cmd_cback,
                  qmi_ati_abort_at_cmd_cback,
                  async_cmd_buf))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Unable to register with ATCoP");
    errval = QMI_ERR_INTERNAL_V01;
    clnt_info->pending_info = NULL;
    goto send_result;
  }

  return QMI_CSI_CB_NO_ERR;
 
send_result:
  DS_SYSTEM_HEAP_MEM_FREE(at_cmd_list_ptr);  
  /* This is to avoid double free in cmd_reg_buf*/
  if(cmd_reg_buf != NULL)
  {
    cmd_reg_buf->at_cmd_list_ptr = NULL;
  }
  DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf); 
  QMI_AT_FREE_CMD_REG_BUF(cmd_reg_buf);

  clnt_info->reg_cmdlist_support_token_ex = FALSE;
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(at_req_at_cmd_fwd_ex_resp_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_ati_reg_at_cmd_fwd_ex() */


/*===========================================================================
  FUNCTION QMI_ATI_DEREG_AT_CMD_FWD_EX()

  DESCRIPTION
    Deregister AT commands registered for forwarding by modem.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  
===========================================================================*/
static qmi_csi_cb_error qmi_ati_dereg_at_cmd_fwd_ex
(
 
  qmi_ati_client_info_type        * clnt_info,
  qmi_req_handle                    req_handle,
  int                      			msg_id,
  void                   		   *req_c_struct,
  int                      			req_c_struct_len,
  void                     		   *service_handle
)
{ 
  
  uint8    cnt;
  qmi_ati_cmd_dereg_buf_type *cmd_dereg_buf = NULL;
  at_dereg_at_cmd_fwd_ex_req_v01*   req_msg;
  at_dereg_at_cmd_fwd_ex_resp_v01  resp_msg;
  dsat_cmd_list_type  *at_cmd_list_ptr;
  int                             	 errval = QMI_ERR_NONE_V01;
  ds_qmi_ati_async_cmd_type* async_cmd_buf = NULL;
  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"qmi_ati_reg_at_cmd_fwd_ex");
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
		   service_handle);
  
	
  req_msg = (at_dereg_at_cmd_fwd_ex_req_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(at_dereg_at_cmd_fwd_ex_resp_v01));
  
  DS_SYSTEM_HEAP_MEM_ALLOC( at_cmd_list_ptr, sizeof(dsat_cmd_list_type) * MAX_CMD_LIST,
                            dsat_cmd_list_type*);
  if (at_cmd_list_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  memset(at_cmd_list_ptr, 0, (sizeof(dsat_cmd_list_type ) * MAX_CMD_LIST));
  

  /* Check any pending transactions */
  if( clnt_info->dereg_pending_info != NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Command deregister requested while another command is pending");
    errval = QMI_ERR_INVALID_OPERATION_V01;
    goto send_result;
  }
  

  /* Check the number of dereg commands requested are less the registered commands */
  if ( (clnt_info->reg_cmd_list.qmi_ati_reg_cmd_cnt == 0) ||
       (req_msg->at_cmd_list_len > MAX_CMD_LIST) )
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Dereg requested no. of commands %d is more than current command count [%d]",
                             req_msg->at_cmd_list_len, clnt_info->reg_cmd_list.qmi_ati_reg_cmd_cnt);
    errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
    goto send_result;
  }

  for (cnt = 0; cnt < req_msg->at_cmd_list_len; cnt++)
  {
    at_cmd_list_ptr[cnt].cmd_buff_len = strlen(req_msg->at_cmd_list[cnt].at_cmd_name);
    if (MAX_CMD_SIZE < strlen(req_msg->at_cmd_list[cnt].at_cmd_name))
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Invalid command length in TLV");
      errval = QMI_ERR_MALFORMED_MSG_V01;
      goto send_result;
    }
    memscpy((byte*)at_cmd_list_ptr[cnt].cmd_buff, 
            MAX_CMD_SIZE,
            req_msg->at_cmd_list[cnt].at_cmd_name,
            strlen(req_msg->at_cmd_list[cnt].at_cmd_name));
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(cmd_dereg_buf, sizeof(qmi_ati_cmd_dereg_buf_type),
                           qmi_ati_cmd_dereg_buf_type*);
  if(cmd_dereg_buf == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  memset(cmd_dereg_buf, 0, sizeof(qmi_ati_cmd_dereg_buf_type));
  cmd_dereg_buf->at_cmd_list_ptr = at_cmd_list_ptr;
  cmd_dereg_buf->cnt = cnt;
  cmd_dereg_buf->client = clnt_info->clnt;
  /*Save the cmd_buf to send response later*/
  cmd_dereg_buf->pending_cmd_buf = req_handle;
  /*Save the pending buf to check with in response from atcop*/
  clnt_info->dereg_pending_info = cmd_dereg_buf;
  
  /*send the async buf as the user_data to valiadate later*/
  
  DS_SYSTEM_HEAP_MEM_ALLOC(async_cmd_buf, sizeof( ds_qmi_ati_async_cmd_type),
							 ds_qmi_ati_async_cmd_type*);
  if(async_cmd_buf == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  memset(async_cmd_buf, 0, sizeof(ds_qmi_ati_async_cmd_type));
  async_cmd_buf->req_handle=req_handle;
  async_cmd_buf->cl_sp=clnt_info;
  async_cmd_buf->msg_id=msg_id;

  
  /* Deregister the  registered List of commands for this client. */
  if(FALSE == dsatclient_deregister_fwd_at_cmd
                (
                  qmi_ati_global.atcop_cl_id,
                  cnt,
                  at_cmd_list_ptr,
                  qmi_ati_dereg_at_cmd_cback,
                  async_cmd_buf))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Unable to deregister command with ATCoP");
    errval = QMI_ERR_INTERNAL_V01;
    clnt_info->pending_info = NULL;
    goto send_result;
  }

  return QMI_CSI_CB_NO_ERR;
send_result:
  DS_SYSTEM_HEAP_MEM_FREE(at_cmd_list_ptr);  
  /* This is to avoid double free in cmd_reg_buf*/
  if(cmd_dereg_buf != NULL)
  {
    cmd_dereg_buf->at_cmd_list_ptr = NULL;
  }
 // ds_qmi_fw_free_async_cmd_buf(async_cmd_buf);
  
  DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf);
  
  QMI_AT_FREE_CMD_REG_BUF(cmd_dereg_buf);
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(at_dereg_at_cmd_fwd_ex_resp_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_ati_dereg_at_cmd_fwd_ex() */


/*===========================================================================
  FUNCTION QMI_ATI_SEND_AT_RESPONSE()

  DESCRIPTION
  Sends the response to an AT command that was previously forwarded
  to the control point from the modem.   


  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
static qmi_csi_cb_error qmi_ati_send_at_repsonse
(
  qmi_ati_client_info_type        * clnt_info,
  qmi_req_handle                    req_handle,
  int                      			msg_id,
  void                   		   *req_c_struct,
  int                      			req_c_struct_len,
  void                     		   *service_handle
)
{
  dsm_item_type *    sdu_dup = NULL;
  int   errval = QMI_ERR_NONE_V01; 
  at_fwd_resp_at_cmd_req_v01*  req_msg;
  at_fwd_resp_at_cmd_resp_v01 resp_msg;
  uint16    temp =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"qmi_ati_send_at_repsonse");
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
		   service_handle);
  
	
  req_msg = (at_fwd_resp_at_cmd_req_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(at_fwd_resp_at_cmd_resp_v01)); 

  if(req_msg->resp_at_cmd_req.at_resp_len > 0)
  {
     temp = PS_UTILS_DSM_PUSHDOWN_PACKED(&sdu_dup,
                                     req_msg->resp_at_cmd_req.at_resp,
                                     req_msg->resp_at_cmd_req.at_resp_len,
                                     DSM_DS_SMALL_ITEM_POOL);
   }

  dsatclient_ext_send_response(TRUE, 
                               qmi_ati_global.atcop_cl_id,
                               (dsat_client_result_enum_type) req_msg->resp_at_cmd_req.result,
                               (dsat_fwd_resp_enum_type) req_msg->resp_at_cmd_req.response_type,
                               sdu_dup,
                               req_msg->resp_at_cmd_req.at_handle);

  /* Response is sent for the command; Clean up the abortability. */
  qmi_ati_global.abort_client = NULL;
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(at_fwd_resp_at_cmd_resp_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_ati_send_at_repsonse() */

/*===========================================================================
  FUNCTION QMI_ATI_SEND_AT_URC()

  DESCRIPTION
    This function/message is used by Apps to send any AT URC's. 

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  
===========================================================================*/
static qmi_csi_cb_error  qmi_ati_send_at_urc
(
  qmi_ati_client_info_type        * clnt_info,
  qmi_req_handle                    req_handle,
  int                      			msg_id,
  void                   		   *req_c_struct,
  int                      			req_c_struct_len,
  void                     		   *service_handle
)
{
  
  int   errval = QMI_ERR_NONE_V01; 
  at_send_at_urc_req_v01*  req_msg;
  at_send_at_urc_resp_v01 resp_msg;  
  dsm_item_type *    sdu_dup = NULL;
  uint16 temp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"qmi_ati_send_at_urc");
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
		   service_handle);

  req_msg = (at_send_at_urc_req_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(at_send_at_urc_resp_v01));   

  temp = PS_UTILS_DSM_PUSHDOWN_PACKED(&sdu_dup,
								 req_msg->urc_req.at_urc,
								 req_msg->urc_req.at_urc_len,
								 DSM_DS_SMALL_ITEM_POOL);
  if (0 == temp)
  {
    errval = QMI_ERR_INTERNAL_V01;
  }
  
  dsatclient_ext_send_urc(req_msg->urc_req.urc_type, 
                          qmi_ati_global.atcop_cl_id,
                          sdu_dup
                         );

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(at_send_at_urc_resp_v01));

  return QMI_CSI_CB_NO_ERR;

  
}/* qmi_ati_send_at_urc */

/*===========================================================================
  FUNCTION QMI_ATI_REG_AT_CMD_CBACK()

  DESCRIPTION
    

  PARAMETERS
    

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ati_reg_at_cmd_cback
(
  boolean     cmd_status,
  uint32      client_id,
  void      * user_data   
)
{
  qmi_ati_cmd_buf_type            *cmd_payload = NULL;  
  ds_command_type                  *cmd_ptr =NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (client_id != qmi_ati_global.atcop_cl_id)
  {
    DS_MSG2(MSG_LEGACY_ERROR, "Ignoring callback for cl_id %d, expected cl_id %d",
                     client_id, qmi_ati_global.atcop_cl_id );
    return;
  }

  cmd_ptr = (ds_command_type *)qmi_ati_get_cmd_buf(QMI_CMD_AT_CMD_REG_STATUS);
  if( cmd_ptr == NULL)
  {
    return;
  }
  cmd_ptr->hdr.cmd_id = DS_QMI_AT_SVC_CMD;
  cmd_payload = (qmi_ati_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;
  cmd_payload->cmd_id = QMI_CMD_AT_CMD_REG_STATUS;
  cmd_payload->data.cmd_reg.status = cmd_status;
  cmd_payload->data.cmd_reg.user_data = user_data;

  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
  FUNCTION QMI_ATI_FWD_AT_CMD_CBACK()

  DESCRIPTION
    

  PARAMETERS
    

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_ati_fwd_at_cmd_cback
(
  dsat_mode_enum_type   at_cmd_mode,
  const tokens_struct_type    *token_ptr,
  const dsat_cmd_state_type    at_state,    
  uint32                atcop_info,
  uint8                 is_support_token_ex,
  void                  *user_info_ptr
)
{
  qmi_ati_fwd_cmd_buf_type *cmd_payload = NULL;  
  ds_command_type                  *cmd_ptr =NULL;
  byte * begin_arg_ptr;
  byte * curr_arg_ptr;
  uint16  len,total_len=0;
  int8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = (ds_command_type *)qmi_ati_get_cmd_buf(QMI_CMD_FWD_AT_CMD);
  if( cmd_ptr == NULL)
  {
    return FALSE;
  }

  //memset((void *)cmd_ptr,0,sizeof(qmi_ati_fwd_cmd_buf_type));
  
  cmd_ptr->hdr.cmd_id = DS_QMI_AT_SVC_CMD;  
  cmd_payload = (qmi_ati_fwd_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;

  cmd_payload->cmd_id = QMI_CMD_FWD_AT_CMD;

  strlcpy((char *)cmd_payload->name,
          (const char *)token_ptr->name,
          MAX_CMD_SIZE);

  if (token_ptr->args_found > at_state.dsat_max_arg_size)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Num token args (%d) greater than max allowed (%d)",
                    token_ptr->args_found, at_state.dsat_max_arg_size);
    DS_ASSERT(0);
    return FALSE;
  }
  cmd_payload->num_args = token_ptr->args_found;

  if(cmd_payload->args == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(cmd_payload->args, MAX_LINE_SIZE, byte*);
    if(cmd_payload->args == NULL)
    {
      return FALSE;
    }
    memset(cmd_payload->args, 0, MAX_LINE_SIZE*sizeof(byte));
  }

  begin_arg_ptr = cmd_payload->args;
  curr_arg_ptr = begin_arg_ptr;

  for(i = 0; i < token_ptr->args_found;  i++)
  {
    /* Since we need to copy num_args arguments each of whom are NULL terminated, 
       strlcat will result in NULL character being removed which is uncalled for.
       Hence strlcpy is used here.

       strlcpy(dst,src,size_t): strlcpy expects sizeof dst buffer in size_t field.
       Also it adds a NULL character at the end of each copy. So NULL character 
       is not accounted for in calculating len.
       Hence effective dst size = MAX_LINE_SIZE - total_len - 1(for NULL character)
    */

    len = strlcpy((char *)curr_arg_ptr,
                  (const char *)token_ptr->arg[i],
                  (MAX_LINE_SIZE - total_len));

    if(len >= (MAX_LINE_SIZE - total_len))
    {
       ERR_FATAL("Buffer copy fail in AT token list", 0,0,0);
    }
    /* After each copy, current_arg_ptr is moved next to NULL character. */    
    curr_arg_ptr += len + 1;
    total_len += len + 1;
  }

  cmd_payload->op = token_ptr->op;
  cmd_payload->at_handle = atcop_info;
  cmd_payload->user_data = user_info_ptr;
  cmd_payload->at_state = at_state;
  cmd_payload->is_support_token_ex = is_support_token_ex;
  
  ds_put_cmd( cmd_ptr);

  return TRUE;
}

/*===========================================================================
  FUNCTION QMI_ATI_ABORT_AT_CMD_CBACK()

  DESCRIPTION
    

  PARAMETERS
    

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ati_abort_at_cmd_cback
(
  void *  user_info_ptr,
  uint32  at_handle
)
{
  qmi_ati_cmd_buf_type   *cmd_payload = NULL;
  ds_command_type                  *cmd_ptr =NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = (ds_command_type *)qmi_ati_get_cmd_buf(QMI_CMD_AT_ABORT_CMD);
  if( cmd_ptr == NULL)
  {
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_QMI_AT_SVC_CMD;
  cmd_payload = (qmi_ati_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;

  cmd_payload->cmd_id = QMI_CMD_AT_ABORT_CMD;
  cmd_payload->data.cmd_abort.handle = at_handle;
  cmd_payload->data.cmd_abort.user_data = user_info_ptr;

  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
  FUNCTION QMI_ATI_DEREG_AT_CMD_CBACK()

  DESCRIPTION
  This function is to post dereg command status to dcc task
  to process final response

  PARAMETERS
  cmd_status  - Deregister command status
  client_id   - Deregister requested client ID
  user_data   - Pointer to user info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ati_dereg_at_cmd_cback
(
  boolean     cmd_status,
  uint32      client_id,
  void      * user_data   
)
{
  qmi_ati_cmd_buf_type *cmd_payload = NULL;
  ds_command_type                  *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (client_id != qmi_ati_global.atcop_cl_id)
  {
    DS_MSG2(MSG_LEGACY_ERROR, "Ignoring callback for cl_id %d, expected cl_id %d",
                     client_id, qmi_ati_global.atcop_cl_id );
    return;
  }

  cmd_ptr = (ds_command_type *)qmi_ati_get_cmd_buf(QMI_CMD_AT_CMD_DEREG_STATUS);
  if( cmd_ptr == NULL)
  {
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_QMI_AT_SVC_CMD;
  cmd_payload = (qmi_ati_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;

  cmd_payload->cmd_id = QMI_CMD_AT_CMD_DEREG_STATUS;
  cmd_payload->data.cmd_reg.status = cmd_status;
  cmd_payload->data.cmd_reg.user_data = user_data;

  ds_put_cmd(cmd_ptr);
}




/*===========================================================================
  FUNCTION QMI_ATI_PROCESS_AT_CMD_REG_STATUS()

  DESCRIPTION
    This function handles the response sent for command registration

  PARAMETERS
    status   - boolean , result of the command
    user_data  - cmd_buf sent in request

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ati_process_at_cmd_reg_status
(
  boolean status,
  void * user_data
)
{
  qmi_ati_client_info_type     * at_cl_sp;
  qmi_req_handle                 req_handle;
  at_req_at_cmd_fwd_ex_resp_v01  resp_msg;
  int                            errval = QMI_ERR_NONE_V01;
  qmi_ati_cmd_reg_buf_type     * cmd_reg_buf = NULL;
  ds_qmi_ati_async_cmd_type    * async_cmd_buf = NULL;
  uint32                         index =0;
  qmi_ati_client_reg_cmd_type  * cmd_reg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  async_cmd_buf = (ds_qmi_ati_async_cmd_type *)user_data;
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_ati_process_at_cmd_reg_status:%d, ud:0x%x",status, user_data);
  
  DS_ASSERT(async_cmd_buf && async_cmd_buf->req_handle);
  memset(&resp_msg,0,sizeof(at_req_at_cmd_fwd_ex_resp_v01));
  
  req_handle=( qmi_req_handle)async_cmd_buf->req_handle;
  at_cl_sp = (qmi_ati_client_info_type *) async_cmd_buf->cl_sp;
  cmd_reg_buf = at_cl_sp->pending_info;
 
  /*make way  for new commands as this handled correctly*/
  at_cl_sp->pending_info = NULL;

  if((cmd_reg_buf != NULL))
  	{
  	DS_MSG1(MSG_LEGACY_ERROR,"Pending command buf 0x%p", cmd_reg_buf->pending_cmd_buf );
	DS_MSG1(MSG_LEGACY_ERROR,"At command list pointer 0x%p", cmd_reg_buf->at_cmd_list_ptr );
	
  	}
  if((cmd_reg_buf == NULL) || 
     (cmd_reg_buf->pending_cmd_buf == NULL) || 
     (cmd_reg_buf->pending_cmd_buf != req_handle) ||
     (cmd_reg_buf->at_cmd_list_ptr == NULL))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid pending_info stored at client: 0x%p", at_cl_sp);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if(status == TRUE)
  {
    errval = QMI_ERR_NONE;
    index = 0;
    /* Save the commands to trace the client to send the forward indication to */
    while (index < cmd_reg_buf->cnt) 
    {
      DS_SYSTEM_HEAP_MEM_ALLOC( cmd_reg, sizeof(qmi_ati_client_reg_cmd_type),
                                qmi_ati_client_reg_cmd_type*);

      /* Sending error from here causes inconsistency between QMI client register command
       * list and ATCoP forwarding command list as forwarding list is already updated.
       */
      if(cmd_reg == NULL)
      {
        errval = QMI_ERR_NO_MEMORY;
        ASSERT(0);
        goto send_result;
      }

      memset(cmd_reg, 0x0, sizeof(qmi_ati_client_reg_cmd_type));

      memscpy(cmd_reg->cmd_name,
              MAX_CMD_SIZE,
              cmd_reg_buf->at_cmd_list_ptr[index].cmd_buff,
              strlen((const char *)cmd_reg_buf->at_cmd_list_ptr[index].cmd_buff));

      (void) q_link(cmd_reg, &cmd_reg->link );
      q_put(&(at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_list_q), &(cmd_reg->link));

      at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_cnt++;
      index++;
     }
     DS_MSG1(MSG_LEGACY_MED,"Client list updated current command count is [%d]",
                             at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_cnt);
  }
  else
  {
    errval = QMI_ERR_INVALID_ARG;
  }

 send_result:
  QMI_AT_FREE_CMD_REG_BUF(cmd_reg_buf);
 
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;

  if (at_cl_sp->reg_cmdlist_support_token_ex == TRUE && errval == QMI_ERR_NONE_V01)
  {
    resp_msg.token_ex_support_valid = TRUE;
    resp_msg.token_ex_support = 1;
  }
  at_cl_sp->reg_cmdlist_support_token_ex = FALSE;
  qmi_csi_send_resp(async_cmd_buf->req_handle,async_cmd_buf->msg_id, &resp_msg, 
                    sizeof(at_req_at_cmd_fwd_ex_resp_v01));
  
  DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf);

} /* qmi_ati_process_at_cmd_reg_status() */

/*===========================================================================
  FUNCTION QMI_ATI_PROCESS_AT_CMD_DEREG_STATUS()

  DESCRIPTION
    This function handles the response sent for command registration

  PARAMETERS
    status   - boolean , result of the command
    user_data  - cmd_buf sent in request

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ati_process_at_cmd_dereg_status
(
  boolean status,
  void * user_data
)

{
  boolean cmd_found = FALSE;
  dsat_cmd_list_type *temp_at_cmd_list_ptr = NULL;
  qmi_ati_client_info_type    * at_cl_sp;
  qmi_req_handle             req_handle;
  at_req_at_cmd_fwd_ex_resp_v01  resp_msg;
  int                             	 errval = QMI_ERR_NONE_V01;
  qmi_ati_cmd_dereg_buf_type *cmd_dereg_buf = NULL;
  ds_qmi_ati_async_cmd_type* async_cmd_buf = NULL;
  uint32   index = 0;
  qmi_ati_client_reg_cmd_type *dereg_cmd = NULL;
  qmi_ati_client_reg_cmd_type *temp_dereg_cmd = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  async_cmd_buf = (ds_qmi_ati_async_cmd_type *)user_data;
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_ati_process_at_cmd_dereg_status:%d, ud:0x%x",status, user_data);
  
  DS_ASSERT(async_cmd_buf && async_cmd_buf->req_handle);
  memset(&resp_msg,0,sizeof(at_dereg_at_cmd_fwd_ex_resp_v01));
  
  req_handle=( qmi_req_handle)async_cmd_buf->req_handle;
  at_cl_sp = (qmi_ati_client_info_type *) async_cmd_buf->cl_sp;
  cmd_dereg_buf = at_cl_sp->dereg_pending_info;


  /*make way  for new commands as this handled correctly*/
  at_cl_sp->dereg_pending_info = NULL;

   if((cmd_dereg_buf != NULL))
  	{
  	DS_MSG1(MSG_LEGACY_ERROR,"Pending command buf 0x%p", cmd_dereg_buf->pending_cmd_buf );
	DS_MSG1(MSG_LEGACY_ERROR,"At command list pointer 0x%p", cmd_dereg_buf->at_cmd_list_ptr );
	DS_MSG1(MSG_LEGACY_ERROR,"Request Handle 0x%p",req_handle);
	
  	}

  if((cmd_dereg_buf == NULL) || 
     (cmd_dereg_buf->pending_cmd_buf == NULL) || 
     (cmd_dereg_buf->pending_cmd_buf != req_handle) ||
     (cmd_dereg_buf->at_cmd_list_ptr == NULL))
     
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid pending_info stored at client: 0x%p",at_cl_sp);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if(status == TRUE)
  {
    errval = QMI_ERR_NONE;
    
    dereg_cmd = (qmi_ati_client_reg_cmd_type *)q_check(&at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_list_q);
    while(dereg_cmd != NULL)
    {
      cmd_found = FALSE;
      temp_at_cmd_list_ptr = cmd_dereg_buf->at_cmd_list_ptr;
      for(index = 0; index < cmd_dereg_buf->cnt; index++)
      {
        if( 0 == qmi_ati_strcmp_ig_sp_case( 
                  (const byte *)dereg_cmd->cmd_name, 
                  (const byte *)temp_at_cmd_list_ptr->cmd_buff))
        {
          temp_dereg_cmd = dereg_cmd;
          dereg_cmd = q_next(&at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_list_q,
                             &(dereg_cmd->link));
          cmd_found = TRUE;
          if (TRUE == q_delete_ext( &at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_list_q,
                                    &temp_dereg_cmd->link ) )
          {
            at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_cnt--;
            DS_SYSTEM_HEAP_MEM_FREE(temp_dereg_cmd);
            temp_dereg_cmd = NULL;
          }
          else
          {
            ASSERT(0);
          }
          break;
        }
        temp_at_cmd_list_ptr++;
      }
      if(!cmd_found)
      {
        dereg_cmd = q_next(&at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_list_q,
                           &(dereg_cmd->link));
      }
    }
    DS_MSG2(MSG_LEGACY_HIGH, "Deregistered successful dereg count %d current at fwd count %d",
                              cmd_dereg_buf->cnt, at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_cnt);
  }
  else
  {
    errval = QMI_ERR_INVALID_ARG;
  }

 send_result:
 
  QMI_AT_FREE_CMD_REG_BUF(cmd_dereg_buf);
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(async_cmd_buf->req_handle,async_cmd_buf->msg_id, &resp_msg, 
					  sizeof(at_dereg_at_cmd_fwd_ex_resp_v01));
	
  DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf);
	
} /* qmi_ati_process_at_cmd_dereg_status() */

/*===========================================================================
  FUNCTION QMI_ATI_SEND_AT_CMD_ABORT_IND()

  DESCRIPTION
    Fwd the AT cmd received 

  PARAMETERS
    cmd_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ati_send_at_cmd_abort_ind
(
  uint32 at_handle,
  void * user_data
  
)
{
  at_abort_at_cmd_ind_v01*     ind_msg;
  ds_qmi_ati_async_cmd_type* async_cmd_buf = NULL;
  qmi_ati_client_info_type    * at_cl_sp;
  qmi_csi_error                 rc;
  
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  async_cmd_buf = (ds_qmi_ati_async_cmd_type *)user_data;
  
  DS_MSG1(MSG_LEGACY_HIGH,"qmi_ati_send_at_cmd_abort_ind handle %d", at_handle);

  DS_SYSTEM_HEAP_MEM_ALLOC(ind_msg, sizeof(at_abort_at_cmd_ind_v01),
                           at_abort_at_cmd_ind_v01*);
  if(ind_msg == NULL)
  {
    return;
  }
  memset(ind_msg,0,sizeof(at_abort_at_cmd_ind_v01));
  ind_msg->at_handle = at_handle;

  at_cl_sp = (qmi_ati_client_info_type *) async_cmd_buf->cl_sp;
  if( at_cl_sp == NULL )
  {
    return;
  }
 
   rc = qmi_csi_send_ind(at_cl_sp->clnt,
                     QMI_AT_ABORT_AT_CMD_IND_V01,
                    (void*)ind_msg, 
                    sizeof(at_abort_at_cmd_ind_v01));
  if ( rc != QMI_CSI_NO_ERR )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unable to send at event report indication!", rc);
  }
  DS_SYSTEM_HEAP_MEM_FREE(ind_msg);
} /* qmi_ati_send_at_cmd_abort_ind */

/*===========================================================================
  FUNCTION QMI_ATI_FWD_AT_CMD_IND_EX()

  DESCRIPTION
    Fwd the AT cmd received 

  PARAMETERS
    cmd_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ati_fwd_at_cmd_ind_ex
(
  uint32 at_handle,
  uint32 op,
  byte * at_name,
  dsat_cmd_state_type at_state,
  uint8  num_args,
  byte * args,
  uint8 is_support_token_ex,
  qmi_ati_client_info_type* cl_sp
)
{
 
  byte * arg_ptr;
  uint16 len;
  uint16 prev_len;
  
  uint16   i, j;

  at_fwd_at_cmd_ex_ind_v01* ind_msg;
  qmi_csi_error                 rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   

  DS_MSG2(MSG_LEGACY_HIGH,"qmi_ati_fwd_at_cmd_ind_ex handle %d op %d", 
                   at_handle, op);
  
  
  DS_SYSTEM_HEAP_MEM_ALLOC(ind_msg, sizeof(at_fwd_at_cmd_ex_ind_v01),
                           at_fwd_at_cmd_ex_ind_v01*);
  if(ind_msg == NULL)
  {
    return;
  }
  memset(ind_msg,0,sizeof(at_fwd_at_cmd_ex_ind_v01));


  ind_msg->at_cmd_info.at_handle = at_handle;
  ind_msg->at_cmd_info.op_code = op;
  memscpy(ind_msg->at_cmd_info.at_cmd_name, 
          strlen((char*)at_name), 
          at_name, 
          strlen((char*)at_name));  

  /* Populate STATE INFO TLVs */

  /* CMEC 4th param optional TLV  */  
  ind_msg->cmec_val_valid = TRUE;
  ind_msg->cmec_val[3] =  at_state.dsat_cmec_val_4;
            
  memscpy(&(ind_msg->cmec_val[0]), 
          sizeof(at_state.dsat_cmec_val),
          &(at_state.dsat_cmec_val), 
          sizeof(at_state.dsat_cmec_val));                 

  memscpy(&(ind_msg->ccug_val), 
          sizeof(at_state.dsat_ccug_val),
           &(at_state.dsat_ccug_val[0]),
          sizeof(at_state.dsat_ccug_val));

  ind_msg->cmee_val_valid = TRUE;
  ind_msg->cmee_val = at_state.dsat_cmee_val;

  ind_msg->colp_val_valid = TRUE;
  ind_msg->colp_val = at_state.dsat_colp_val;

  ind_msg->clir_val_valid = TRUE;
  ind_msg->clir_val = at_state.dsat_clir_val;

  ind_msg->s4_val_valid = TRUE;
  ind_msg->s4_val = at_state.dsat_s4_val;

  ind_msg->s3_val_valid = TRUE;
  ind_msg->s3_val = at_state.dsat_s3_val;

  ind_msg->q_val_valid = TRUE;
  ind_msg->q_val = at_state.dsat_q_val;

  ind_msg->v_val_valid = TRUE;
  ind_msg->v_val = at_state.dsat_v_val;

  prev_len = 0;
  arg_ptr = args;
  /* Populate  Token TLVs */
  if (is_support_token_ex)
  {
    ind_msg->token_ex_valid = TRUE;
    for(i = 0; i < num_args; i++)
    {
      /* Token strings are terminated with NULL char - 
         to ease the use on the client side */
      len = strlen((char*)arg_ptr) + 1; //for NULL char
      for(j = 0; j < len; j++)
      {
        if((j + prev_len) >= QMI_AT_NUM_TOKEN_MAX_EX_V01)
        {
          dsatclient_ext_send_response( FALSE, 
                                      qmi_ati_global.atcop_cl_id,
                                      DSAT_CLIENT_RESULT_ERROR,
                                      DSAT_RESP_COMPLETE,
                                      NULL,
                                      at_handle);
          DS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }
        ind_msg->token_ex[j + prev_len] = arg_ptr[j];
      }
      arg_ptr += len; //since len includes NULL char, no need to add 1
      prev_len += len;
    }

    ind_msg->token_ex_len = prev_len;
  }
  else
  {
    ind_msg->token_valid = TRUE;
    for(i = 0; i < num_args; i++)
    {
      /* Token strings are terminated with NULL char - 
         to ease the use on the client side */
      len = strlen((char*)arg_ptr) + 1; //for NULL char

      for(j = 0; j < len; j++)
      {
        if((j + prev_len) >= QMI_AT_NUM_TOKEN_MAX_V01)
        {
          dsatclient_ext_send_response( FALSE,
                                        qmi_ati_global.atcop_cl_id,
                                        DSAT_CLIENT_RESULT_ERROR,
                                        DSAT_RESP_COMPLETE,
                                        NULL,
                                        at_handle);
          DS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }
        ind_msg->token[j + prev_len] = arg_ptr[j];
      }
      arg_ptr += len; //since len includes NULL char, no need to add 1
      prev_len += len;
    }  
    ind_msg->token_len = prev_len;
  }

  /* Save the client ID for abort usage */
  DS_MSG1(MSG_LEGACY_HIGH,"Saving abort client id [0x%p]", cl_sp);
  qmi_ati_global.abort_client = cl_sp;

  rc = qmi_csi_send_ind(cl_sp->clnt,
                         QMI_AT_FWD_AT_CMD_EX_IND_V01,
                    	(void*)ind_msg, 
                    	sizeof(at_fwd_at_cmd_ex_ind_v01));
  if ( rc != QMI_CSI_NO_ERR )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unable to send at event report indication!", rc);
  }
  DS_SYSTEM_HEAP_MEM_FREE(ind_msg);
  return;          
} /* qmi_ati_fwd_at_cmd_ind_ex */



/*===========================================================================
  FUNCTION QMI_ATI_GET_AT_CLIENT_BY_CMD_NAME()

  DESCRIPTION
    to get the client registered for the given command name
    

===========================================================================*/
static qmi_ati_client_info_type *  qmi_ati_get_at_client_by_cmd_name
(
  byte *cmd_name
)
{
  qmi_ati_client_info_type * at_cl_sp = NULL;
  qmi_ati_client_info_type * ret_at_cl_sp = NULL;
  qmi_ati_client_reg_cmd_type *client_reg_cmd_name;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  at_cl_sp = (qmi_ati_client_info_type*)q_check(&qmi_at_state.client_q);
  while(at_cl_sp!=NULL)
  {

    client_reg_cmd_name = (qmi_ati_client_reg_cmd_type *)q_check(&at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_list_q);
    while(client_reg_cmd_name != NULL)
    {
      if(( 0 == qmi_ati_strcmp_ig_sp_case( (const byte *)cmd_name, 
                (const byte *)client_reg_cmd_name->cmd_name)))
      {
        ret_at_cl_sp = at_cl_sp;
        break;
      }
      client_reg_cmd_name = q_next(&at_cl_sp->reg_cmd_list.qmi_ati_reg_cmd_list_q,
                                   &(client_reg_cmd_name->link));
    }

    if(ret_at_cl_sp != NULL)
    {
      break;
    }

    at_cl_sp= q_next(&qmi_at_state.client_q,
                     &(at_cl_sp->link));
  }

  if(NULL == ret_at_cl_sp)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid Command received");
  }

  return ret_at_cl_sp;
} /* qmi_ati_get_at_client_by_cmd_name() */

/*===========================================================================
  FUNCTION QMI_ATI_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI AT command or event.

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI AT command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI AT must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ati_process_cmd
(
  ds_command_enum_type  cmd,
  void *cmd_ptr
)
{
  qmi_ati_cmd_buf_type *cmd_buf_ptr = NULL;
  qmi_ati_client_info_type * at_cl_sp = NULL; 
/*-------------------------------------------------------------------------*/

  DS_ASSERT(cmd_ptr != NULL);
  cmd_buf_ptr = (qmi_ati_cmd_buf_type *)cmd_ptr;
  DS_MSG1(MSG_LEGACY_HIGH,"qmi_ati_process_cmd(): cmd id: %d ",cmd_buf_ptr->cmd_id);
 
  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_AT_CMD_REG_STATUS:
      qmi_ati_process_at_cmd_reg_status(cmd_buf_ptr->data.cmd_reg.status,
                                        cmd_buf_ptr->data.cmd_reg.user_data);
      DS_MSG1(MSG_LEGACY_HIGH,"qmi_ati_process_cmd(): cmd id: %d ",cmd_buf_ptr->cmd_id);
      break;
    case QMI_CMD_FWD_AT_CMD : 
    {
      qmi_ati_fwd_cmd_buf_type *fwd_cmd_buf_ptr = (qmi_ati_fwd_cmd_buf_type*)cmd_ptr;

      at_cl_sp = qmi_ati_get_at_client_by_cmd_name(fwd_cmd_buf_ptr->name);
      if (NULL == at_cl_sp)
      {
        DS_MSG0(MSG_LEGACY_HIGH,"Invalid NULL client ");
        break;
      }

      qmi_ati_fwd_at_cmd_ind_ex(fwd_cmd_buf_ptr->at_handle,
                                  fwd_cmd_buf_ptr->op,
                                  fwd_cmd_buf_ptr->name,
                                  fwd_cmd_buf_ptr->at_state,
                                  fwd_cmd_buf_ptr->num_args,
                                  fwd_cmd_buf_ptr->args,
                                  fwd_cmd_buf_ptr->is_support_token_ex,
                                  at_cl_sp);
      DS_SYSTEM_HEAP_MEM_FREE(fwd_cmd_buf_ptr->args);
      break;
    }
    case QMI_CMD_AT_ABORT_CMD :
      qmi_ati_send_at_cmd_abort_ind(cmd_buf_ptr->data.cmd_abort.handle,
                                    cmd_buf_ptr->data.cmd_abort.user_data);
      break;
    case QMI_CMD_AT_CMD_DEREG_STATUS :
      qmi_ati_process_at_cmd_dereg_status(cmd_buf_ptr->data.cmd_dereg.status,
                                          cmd_buf_ptr->data.cmd_dereg.user_data);
      break;
    default:
     DS_MSG1(MSG_LEGACY_ERROR,"Unexpected cmd(%d) recd in QMI AT cmd handler",
                              cmd_buf_ptr->cmd_id);
     DS_ASSERT(0);
  }
  //qmi_ati_free_cmd_buf(cmd_ptr);
} /* qmi_ati_process_cmd() */
 
#endif /*  FEATURE_DATA_QMI_AT*/
#endif /* FEATURE_DATA_ATCOP */

