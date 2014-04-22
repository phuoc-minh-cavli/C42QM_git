/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2011-2017, 2019, 2022 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_io_sio.c#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/19   kdey    Porting of Qsockets
05/12/17   gn      Moved mutex lock outside diagcomm_io_ioctl to avoid deadlock between Diag and USB threads
07/20/16   nk      Changes to support ThreadX
03/07/16   is      Serialize access to updating channel_connected
02/10/16   is      Add sanity checks for invalid handle in diag_dtr_changed_ext_cb()
10/26/15   is      Cancel HDLC recovery timer when not needed
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
10/08/14   xy      Implement DCI discovery and DIAG DCI over PCIe 
04/03/14   psu     Update diag macros using optimized versions.
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/27/14   is      Command/response separation
01/07/14   xy      Added fusion DCI feature  
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0
10/01/13   xy      Removed code under FEATURE_WINCE 
03/12/13   sa      Corrected the SIO IOCTL sent for Diag over UART
01/25/13   sr      Fixed compiler warnings  
11/27/12   sa      Mainlining DIAG_CENTRAL_ROUTING
11/27/12   is      Resolve issue with channel_connected status not updated
                   on control channel connection
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes 
05/15/12   is      Allow "build time" mask queries from Master Diag 
02/01/12   is      Disable sending DTR ctrl msg to slaves
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
05/03/11   hm      Featurised so apps does not open sio control channel 
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/

#include "customer.h"

#ifdef DIAG_SIO_SUPPORT
#include "diagcomm_io_sio.h"
#include "sio.h"              /* For sio_ioctl_param_type, etc */

#include "msg.h"              /* For MSG_N() macros */
#include "osal.h"
#include "diagcomm_v.h"
#include "diag_fwd_v.h"
#include "diagi_v.h"          /* For DIAG_COMM_SIO_TX_FLUSH_SIG */
#include "diag.h"             /* For DIAG_COMM_OPEN_SIG */
#include "assert.h"           /* For ASSERT() */
#include "diagcomm_ctrl.h"    /* For diagcomm_ctrl_send_mult_pkt */
#include "diagcomm_dci.h"


extern boolean diagpkt_filtered_cmd_set; /* Defined in diagpkt.c */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

extern osal_mutex_arg_t diag_conn_changed_mutex; /* From diag.c */


/*===========================================================================

FUNCTION DIAG_DTR_CHANGED_CB

DESCRIPTION
  Invoked when DTR changes. This is intended to flip a variable that causes
  Diag to vote/not vote for sleep based on connectivity status.

PARAMETERS
  cb_data - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
#if defined (DIAG_MP_MASTER)
extern osal_timer_t diag_hdlc_err_recovery_timer;
extern boolean diag_hdlc_disabled;
extern boolean diag_hdlc_disable_cmd_received;
#endif

void diag_dtr_changed_cb(void)
{
   sio_ioctl_param_type param;
   boolean dte_ready_asserted;

#if defined (DIAG_MP_MASTER)
   int return_val;
#endif
   
   diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]; 

#if defined (DIAG_MP_MASTER)
  
   param.dte_ready_asserted = &dte_ready_asserted;

   diagcomm_io_ioctl( conn, 
            (uint32)SIO_IOCTL_DTE_READY_ASSERTED, 
            (void*)&param );
   
   osal_lock_mutex(&diag_conn_changed_mutex); /* Mutex should be taken after ioctl call
                                                 to prevent deadlock with usb */
   
   if( dte_ready_asserted == FALSE )
   {
     /* If disconnect detected, revert back to HDLC protocol. */
     diag_hdlc_disabled = FALSE;
     diag_hdlc_disable_cmd_received = FALSE;
   
     return_val = osal_reset_timer(&diag_hdlc_err_recovery_timer);
     ASSERT(OSAL_SUCCESS == return_val);
   }
   
   diagcomm_sio_conn_changed( conn, dte_ready_asserted );

   osal_unlock_mutex(&diag_conn_changed_mutex);
#endif
   MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diag_dtr_changed_cb: port=%d, channel=%d, status=%d, tid=0x%x", 
            conn->port_type, conn->channel_type, conn->channel_connected, osal_thread_self());


}  /* diag_dtr_changed_cb */

/*===========================================================================

FUNCTION DIAG_DTR_ENABLE

DESCRIPTION
  Invoked when Diag initializes. This registers a callback with SIO that is
  intended to report when there are DTR changes.

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_dtr_enable( diagcomm_io_conn_type * conn )
{
  sio_ioctl_param_type ioctl_param;

  ASSERT( conn != NULL);
  ASSERT( conn->io_type == DIAGCOMM_IO_TYPE_SIO );
  

/* For Master proc send the SIO_IOCTL_ENABLE_DTR_EVENT for USB/UART port over SIO/PORT1 */
#if defined (DIAG_MP_MASTER)
   if( conn->port_type == DIAGCOMM_PORT_SIO )
   {
      if(conn->port_num == DIAGCOMM_PORT_1)
      {
         ioctl_param.enable_dte_ready_event = diag_dtr_changed_cb;
         diagcomm_io_ioctl( conn,
                         (uint32)SIO_IOCTL_ENABLE_DTR_EVENT,
                         (void*)&ioctl_param );
         return;
      }
   }
#endif

} /* diag_dtr_enable */


/*===========================================================================
FUNCTION DIAGCOMM_SMD_SIO_SET_PORT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_smd_sio_set_port( diagcomm_io_conn_type * conn )
{

#if defined (DIAG_MP_MASTER)     // Apps processor 
    if (DIAGCOMM_PORT_1 == conn->port_num)
      conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
    //else if (DIAGCOMM_PORT_2 == conn->port_num)
      //conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_QDSP;
#else
	#if defined(DIAG_QDSP6_APPS_PROC) //LPASS
	  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
	#else
	  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
	#endif
#endif 
 
} /* diagcomm_smd_sio_set_port */


/*===========================================================================
FUNCTION DIAGCOMM_CTRL_SIO_SET_PORT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_ctrl_sio_set_port( diagcomm_io_conn_type * conn )
{ 
  if( DIAGCOMM_PORT_SMD == conn->port_type )
  {
    #if defined (DIAG_MP_MASTER)     // Apps processor 
      if (DIAGCOMM_PORT_1 == conn->port_num)
        conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
      //else if (DIAGCOMM_PORT_2 == conn->port_num)
        //conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_QDSP;
    #else
	  #if defined(DIAG_QDSP6_APPS_PROC) //LPASS
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
	  #else
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
 	  #endif
    #endif 	
  }
  else if( DIAGCOMM_PORT_SIO == conn->port_type )
  {
    #if !defined (DIAG_RUNTIME_DEVMAP)
      conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
    #endif

    #if defined(DIAG_SIO_USB)
      conn->open_params.sio_params.params.port_id = SIO_PORT_USB_DIAG;
    #endif 
	
	#if defined(DIAG_QDSP6_APPS_PROC) //LPASS
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
	#elif !defined(DIAG_MP_MASTER)
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
 	#endif
	
  }
  
} /* diagcomm_ctrl_sio_set_port */


/*===========================================================================
FUNCTION DIAGCOMM_SIO_SET_PORT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_sio_set_port( diagcomm_io_conn_type * conn )
{

#if !defined (DIAG_RUNTIME_DEVMAP)
  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
#endif

#if defined (DIAG_SIO_USB)
  conn->open_params.sio_params.params.port_id = SIO_PORT_USB_DIAG;
#endif 

#if defined (FEATURE_DIAG_SIO_UART)
  conn->open_params.sio_params.params.port_id = SIO_PORT_UART_MAIN;
#endif

#if defined (DIAG_ON_SDIO_PORT)
  conn->open_params.sio_params.params.port_id = SIO_PORT_SDIOC_DIAG;  
#endif

#if defined (DIAG_MP_MASTER)
	#if defined (DIAG_SIO_USB)
      conn->open_params.sio_params.params.port_id = SIO_PORT_USB_DIAG;
    #endif	
    #if defined (FEATURE_DIAG_SIO_UART)
      conn->open_params.sio_params.params.port_id = SIO_PORT_UART_MAIN;
    #endif
#elif defined(DIAG_QDSP6_APPS_PROC) //LPASS
	  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
#endif


//  if( conn->open_params.sio_params.params.port_id == SIO_PORT_ONDEVICE_DIAG )
  //{
    //diagpkt_filtered_cmd_set = TRUE; /* Port is Ondevice */
  //}
  
  #ifdef DIAG_APPS_ONLY_BRINGUP
  	conn->open_params.sio_params.params.port_id = SIO_PORT_UART_MAIN;
  #endif
    
} /* diagcomm_sio_set_port */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN_CB

DESCRIPTION
  This callback is called when RDM wants diag to open a port on the SIO channel

PARAMETERS
  port_id - SIO port ID to open
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_sio_open_cb (sio_port_id_type port_id)
{
  osal_sigs_t return_sigs;
  int return_val = 0;


  /* Update SIO port id */
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = port_id;
  
  /* Update ctrl channel SIO port id */
  /*if( (port_id == SIO_PORT_ONDEVICE_DIAG) || (port_id == SIO_PORT_ONDEVICE_DIAG_USB_AUTO) )
  {
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = port_id;
  }
  else
  {
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = SIO_PORT_NULL;
  }*/
  
  return_val = osal_set_sigs(&diag_task_tcb, DIAG_COMM_OPEN_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
  
} /* diagcomm_sio_open_cb */



/*===========================================================================

FUNCTION DIAGCOMM_SIO_FUSION_DCI_OPEN_CB

DESCRIPTION
  This callback is called when RDM wants diag to open a new port on the SIO channel

PARAMETERS
  port_id - SIO port ID to open
  
RETURN VALUE
  None
  
===========================================================================*/

#ifdef DIAG_CONSUMER_API
  void
  diagcomm_sio_fusion_dci_open_cb(sio_port_id_type port_id)
  {
 
  /* Update SIO port id */
    diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_FUSION_DCI].open_params.sio_params.params.port_id = port_id;

    (void)diag_set_internal_sigs( DIAG_INT_FUSION_OPEN_SIG );

  
  } /* diagcomm_sio_fusion_dci_open_cb */
#endif /* #ifdef DIAG_CONSUMER_API */

#endif /* DIAG_SIO_SUPPORT */
