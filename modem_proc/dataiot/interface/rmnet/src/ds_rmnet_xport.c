/*===========================================================================

                D S _ R M N E T _ X P O R T . C

DESCRIPTION

  RMNET transport source file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/src/ds_rmnet_xport.c#1 $
  $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/09/17    skc    Featurizing AT$QCRMCALL command.
05/24/13    wc     Initial file
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ds_headers.h"
#include "ds_rmnet_xport.h"
#include "ds_rmnet_shim.h"
#include "ds_rmnet_smi.h"
#include "ds_crit_sect.h"
#include "ds_qmux.h"
#include "ds_qmap.h"
#include "ds_qmi_svc.h"

/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

// physical watermark definitions
#define RMNET_PHYS_XPORT_WMK_LO   15000
#define RMNET_PHYS_XPORT_WMK_HI   50000
#define RMNET_PHYS_XPORT_WMK_DNE  400000
#define RMNET_PHYS_XPORT_WMK_Q_CNT_DNE  2000

// physical data ports
static rmnet_phys_xport_type * rmnet_phys_xport[RMNET_PHYS_XPORT_MAX];

extern ds_crit_sect_type rmnet_crit_section;
extern rmnet_efs_global_config_s_type rmnet_efs_global_config_params;

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/


/*===========================================================================

                           INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_SIO_OPEN()

  DESCRIPTION
    This function opens an SIO port

  PARAMETERS
    port_id: SIO port to open
    rx_wmk:  RX watermark pointer
    tx_wmk:  TX watermark pointer

  RETURN VALUE
    Stream ID of the opened SIO port

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static sio_stream_id_type rmnet_sio_open
(
  sio_port_id_type        port_id,
  dsm_watermark_type    * rx_wmk,
  dsm_watermark_type    * tx_wmk
)
{
  sio_open_type           open_param;
  sio_ioctl_param_type    ioctl_param;
  sio_stream_id_type      stream_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   * Open the SIO port
  -------------------------------------------------------------------------*/
  memset (&open_param, 0, sizeof (open_param));

  open_param.port_id        = port_id;
  open_param.stream_mode    = SIO_GENERIC_MODE;
  open_param.rx_func_ptr    = NULL;
  open_param.rx_queue       = rx_wmk;
  open_param.tx_queue       = tx_wmk;
  open_param.rx_bitrate     = SIO_BITRATE_BEST;
  open_param.tx_bitrate     = SIO_BITRATE_BEST;
  open_param.tail_char_used = FALSE;
  open_param.rx_flow        = SIO_FCTL_BEST;
  open_param.tx_flow        = SIO_FCTL_BEST;

  #ifndef TEST_FRAMEWORK
    stream_id = sio_open(&open_param);
  #else
    #error code not present
#endif /* TEST_FRAMEWORK */

  if (stream_id == SIO_NO_STREAM_ID)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Failed to open SIO port 0x%x", port_id );
    return stream_id;
  }

   /* We need to set the DROP_PENDING ioctl on SMD ports, in order to
    * enable the SMD module to drop packets when the onward link to apps
    * proc is not available. */
  if ( (port_id >= SIO_PORT_SMD_DATA_FIRST) &&
       (port_id <= SIO_PORT_SMD_DATA_LAST) )
  {
    memset(&ioctl_param, 0, sizeof(sio_ioctl_param_type));
    ioctl_param.drop_asserted = TRUE;
    sio_ioctl(stream_id, SIO_IOCTL_SET_DROP_PEND, &ioctl_param);
  }

  return stream_id;

} /* rmnet_sio_open() */

/*===========================================================================
  FUNCTION RMNET_SIO_CLOSE()

  DESCRIPTION
    This function closes an SIO port

  PARAMETERS
    stream_id: SIO stream ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_sio_close
(
  sio_stream_id_type  stream_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef TEST_FRAMEWORK
    sio_close(stream_id, NULL);
  #else
    #error code not present
#endif /* TEST_FRAMEWORK */

} /* rmnet_sio_close() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_OPEN()

  DESCRIPTION
    This function opens a physical data port

  PARAMETERS
    port_id:            SIO port ID
    ep_id:              End point ID
    consumer_pipe_num:  EP consumer pipe number
    producer_pipe_num:  EP producer pipe number

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_phys_xport_open
(
  sio_port_id_type        port_id,
  uint32                  ep_id,
  uint32                  consumer_pipe_num,
  uint32                  producer_pipe_num
)
{
  rmnet_phys_xport_type   * phys_xport;
  sio_port_id_type          sio_port;
  boolean                   is_hw_port = FALSE;
  int                       i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid EP id");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Check if the EP already exists
  -------------------------------------------------------------------------*/
  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport != NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"EP 0x%x already opened", ep_id);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Find an unused phys xport instance
  -------------------------------------------------------------------------*/
  for (i = 0; i < RMNET_PHYS_XPORT_MAX; i++)
  {
    if (rmnet_phys_xport[i] == NULL)
    {
      break;
    }
  }

  if (i >= RMNET_PHYS_XPORT_MAX)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No available phys xport slot");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Use the sio port in the request or allocate a new one if NULL
  -------------------------------------------------------------------------*/
  sio_port = port_id;
  if (port_id == SIO_PORT_NULL)
  {
    sio_port = rmnet_shim_get_phys_sio_port();
    is_hw_port = TRUE; // hw accelerated port
  }

  if (sio_port == SIO_PORT_NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Failed to allocate a phys SIO port");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Allocate phys xport structure
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(phys_xport, sizeof(rmnet_phys_xport_type),
                           rmnet_phys_xport_type*);
  if (phys_xport == NULL)
  {
    return FALSE;
  }

  memset(phys_xport, 0, sizeof(rmnet_phys_xport_type));  
  phys_xport->stream_id         = SIO_NO_STREAM_ID;
  phys_xport->sio_port          = sio_port;
  phys_xport->ep_id             = ep_id;
  phys_xport->consumer_pipe_num = consumer_pipe_num;
  phys_xport->producer_pipe_num = producer_pipe_num;
  phys_xport->is_hw_port        = is_hw_port;
  phys_xport->phys_xport_inst   = i;

  /*-------------------------------------------------------------------------
    Open the accelerated port. For software port it will be opened when
    logical port is opened (phys port == logical port).
  -------------------------------------------------------------------------*/
  if (is_hw_port)
  {
    /*-----------------------------------------------------------------------
      Init watermarks
    -----------------------------------------------------------------------*/
    dsm_queue_init( &phys_xport->tx_wmk,
                    RMNET_XPORT_WMK_MAX,
                    &phys_xport->tx_wmk_q );

    dsm_queue_init( &phys_xport->rx_wmk,
                    RMNET_XPORT_WMK_MAX,
                    &phys_xport->rx_wmk_q );

    phys_xport->tx_wmk.lo_watermark    = 0;
    phys_xport->tx_wmk.hi_watermark    = 0;
    phys_xport->tx_wmk.dont_exceed_cnt = RMNET_PHYS_XPORT_WMK_DNE;
    phys_xport->tx_wmk.dne_q_cnt       = RMNET_PHYS_XPORT_WMK_Q_CNT_DNE;

    phys_xport->rx_wmk.lo_watermark    = RMNET_PHYS_XPORT_WMK_LO;
    phys_xport->rx_wmk.hi_watermark    = RMNET_PHYS_XPORT_WMK_HI;
    phys_xport->rx_wmk.dont_exceed_cnt = RMNET_PHYS_XPORT_WMK_DNE;
    phys_xport->rx_wmk.dne_q_cnt       = RMNET_PHYS_XPORT_WMK_Q_CNT_DNE;

    /*-----------------------------------------------------------------------
     * Open the SIO port
    -----------------------------------------------------------------------*/
    phys_xport->stream_id = rmnet_sio_open(
               sio_port, &phys_xport->rx_wmk, &phys_xport->tx_wmk);

    if (phys_xport->stream_id == SIO_NO_STREAM_ID)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Failed to open phys SIO port 0x%x", sio_port );      
      rmnet_shim_free_phys_sio_port(phys_xport->sio_port);
      dsm_queue_destroy(&phys_xport->tx_wmk);
      dsm_queue_destroy(&phys_xport->rx_wmk);
      DS_SYSTEM_HEAP_MEM_FREE(phys_xport);
      return FALSE;
    }

    /*-----------------------------------------------------------------------
     * Set peripherial EP to SIO port mapping
    -----------------------------------------------------------------------*/
    if (!rmnet_shim_set_per_mapping(phys_xport))
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Failed to set physical to peripheral mapping");      
      rmnet_sio_close(phys_xport->stream_id);
      phys_xport->stream_id = SIO_NO_STREAM_ID;
      rmnet_shim_free_phys_sio_port(phys_xport->sio_port);
      dsm_queue_destroy(&phys_xport->tx_wmk);
      dsm_queue_destroy(&phys_xport->rx_wmk);
      DS_SYSTEM_HEAP_MEM_FREE(phys_xport);
      return FALSE;
    }
  }

  rmnet_phys_xport[i] = phys_xport;

  /*-------------------------------------------------------------------------
   * Set default data format
  -------------------------------------------------------------------------*/
  rmnet_phys_xport_set_data_format(ep_id,
                   NULL, 0, RMNET_SET_DATA_FORMAT_NONE);

  DS_MSG1(MSG_LEGACY_ERROR,"Data EP 0x%x opened", ep_id);

#ifdef FEATURE_DATA_NO_QMI_TEST
  // Fake a DTR high
  rmnet_phys_xport_dtr_cmd(
            ep_id,
            TRUE);
#endif /* FEATURE_DATA_NO_QMI_TEST */

  return TRUE;

} /* rmnet_phys_xport_open() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_CLOSE()

  DESCRIPTION
    This function closes a physical data port

  PARAMETERS
    ep_id:              End point ID

  RETURN VALUE
    TRUE/FALSE

  DEPENDENCIES
    None
  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_phys_xport_close
(
  uint32  ep_id
)
{
  rmnet_phys_xport_type * phys_xport;
  rmnet_xport_type      * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"EP 0x%x doesn't exist", ep_id);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Unbind any logical ports
  -------------------------------------------------------------------------*/
  while (phys_xport->xport_list != NULL)
  {
    xport_info = phys_xport->xport_list;

    DS_MSG2(MSG_LEGACY_ERROR,"Force unbinding of EP 0x%x mux_id %d",
                    phys_xport->ep_id, xport_info->mux_id);

    // Don't change ep_id and mux_id of xport here.
    // It will be done in xport unbind
    xport_info->phys_xport = NULL;
    phys_xport->xport_list = xport_info->next;

    // Trigger a DTR low
    if (xport_info->dtr_cb != NULL)
    {
      xport_info->dtr_cb(xport_info->user_info, FALSE);
    }
  }

  /*-------------------------------------------------------------------------
   * Close and free the accelerated port. SW port will be closed when
   * logical port is closed.
  -------------------------------------------------------------------------*/
  if (phys_xport->is_hw_port)
  {
    if (RMNET_PHYS_XPORT_IS_ACTIVE(phys_xport))
    {
      /* If any tethered calls up, inform driver that port is being closed */
      if (phys_xport->teth_call_cnt > 0)
      {
        rmnet_shim_notify_tethered_call_state(phys_xport, FALSE);
      }
      rmnet_sio_close(phys_xport->stream_id);
      phys_xport->stream_id = SIO_NO_STREAM_ID;
    }

    rmnet_shim_free_phys_sio_port(phys_xport->sio_port);
    dsm_queue_destroy(&phys_xport->tx_wmk);
    dsm_queue_destroy(&phys_xport->rx_wmk);
  }

  rmnet_phys_xport[phys_xport->phys_xport_inst] = NULL;
  DS_SYSTEM_HEAP_MEM_FREE(phys_xport);

  DS_MSG1(MSG_LEGACY_ERROR,"Data EP 0x%x closed", ep_id);
  return TRUE;

} /* rmnet_phys_xport_close() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_DTR_CHAGNED()

  DESCRIPTION
    This function handles DTR changed event on physical channel

  PARAMETERS
    ep_id:        ep_id of physical channel
    dtr_asserted: is DTR asserted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_phys_xport_dtr_changed
(
  uint32   ep_id,
  boolean  dtr_asserted
)
{
  rmnet_phys_xport_type  * phys_xport;
  rmnet_xport_type       * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG2(MSG_LEGACY_ERROR,"DTR [%d] on EP 0x%x", dtr_asserted, ep_id);

  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"DTR on unknown EP 0x%x", ep_id);
    return;
  }

  if (dtr_asserted)
  {
    // If autoconnect is enabled, automatically bind rmnet 
    //rmnet_meta_sm_autoconnect_bind(ep_id); 
    #ifdef TEST_FRAMEWORK
      #error code not present
#endif
    DS_MSG1(MSG_LEGACY_HIGH,"DTR high EP 0x%x", ep_id);
  }
  else
  {
    // Reset data format
    rmnet_phys_xport_set_data_format(ep_id, NULL, 0,
                            RMNET_SET_DATA_FORMAT_DTR_LOW);
#ifdef FEATURE_DATA_QCRMCALL
    // Reset qcrmcall table
    rmnet_qcrmcall_inst_remove(ep_id);
#endif /* FEATURE_DATA_QCRMCALL */
  }

  phys_xport->dtr_is_high = dtr_asserted;

  /*-------------------------------------------------------------------------
   * Invoke DTR callback
  -------------------------------------------------------------------------*/
  xport_info = phys_xport->xport_list;
  while (xport_info != NULL)
  {
    if (xport_info->dtr_cb != NULL)
    {
      xport_info->dtr_cb(xport_info->user_info, dtr_asserted);
    }
    xport_info = xport_info->next;
  }

} /* rmnet_phys_xport_dtr_changed() */


/*===========================================================================
  FUNCTION RMNET_XPORT_EP_CFG_DONE()

  DESCRIPTION
    This functions handles EP config done command. It invokes the EP config
    done callback registered with rmnet xport

  PARAMETERS
    stream_id:  stream_id of the logical port on which EP config is done

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_xport_ep_cfg_done
(
  sio_stream_id_type    stream_id
)
{
  rmnet_phys_xport_type   * phys_xport;
  rmnet_xport_type        * xport_info;
  int                       i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_MSG1(MSG_LEGACY_ERROR,"EP config done for stream id %d", stream_id);
  if (stream_id == SIO_NO_STREAM_ID)
  {
    return;
  }

  for (i = 0; i < RMNET_PHYS_XPORT_MAX; i++)
  {
    phys_xport = rmnet_phys_xport[i];
    if (phys_xport == NULL)
    {
      continue;
    }

    xport_info = phys_xport->xport_list;
    while (xport_info != NULL)
    {
      if (xport_info->stream_id == stream_id)
      {
        if (xport_info->ep_cfg_cnt > 0)
        {
          xport_info->ep_cfg_cnt--;
        }
        else
        {
          /* No outstanding config, or port closed */
          DS_MSG1(MSG_LEGACY_ERROR,"Recvd EP 0x%x cfg done but EP cfg cnt is 0",
                          phys_xport->ep_id);
          break;
        }

        /* Invoke the callback if there is no outstanding EP cfg */
        if (xport_info->ep_cfg_cnt == 0 && 
            xport_info->ep_cfg_cb != NULL)
        {
          xport_info->ep_cfg_cb(xport_info->user_info);
        }
        else
        {
          DS_MSG2(MSG_LEGACY_ERROR,"EP 0x%x outstanding cfg cnt %d",
                          phys_xport->ep_id, xport_info->ep_cfg_cnt);
        }
        break;
      }
      xport_info = xport_info->next;
    }
  }

} /* rmnet_xport_ep_cfg_done() */

/*===========================================================================
  FUNCTION RMNET_XPORT_SET_PORT_MAPPING()

  DESCRIPTION
    This function notifies HW of logical/physical ports mapping

  PARAMETERS
    xport_info:   rmnet xport structure

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_xport_set_port_mapping
(
  rmnet_xport_type       * xport_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (xport_info == NULL)
  {
    DS_ASSERT(0);
    return FALSE;
  }

  if (!xport_info->is_hw_port)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Port mapping ignored for SW port 0x%x",
                    xport_info->sio_port);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
   * Register EP config done callback if needed
  -------------------------------------------------------------------------*/
/*  if (!cb_registered)
  {
    if (rmnet_shim_register_ep_cfg_done_cb(
                     rmnet_xport_ep_cfg_done_cb, 0))
    {
      cb_registered = TRUE;
    }
  }*/

  return rmnet_shim_set_port_mapping(xport_info);

} /* rmnet_xport_set_port_mapping() */


/*===========================================================================

                           EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_OPEN_CLOSE_CMD()

  DESCRIPTION
    This function sends a command to DCC task to open or close
    a physical data port

  PARAMETERS
    cmd:                rmnet xport open or close command
    port_id:            SIO port ID
    ep_id:              End point ID
    consumer_pipe_num:  EP consumer pipe number
    producer_pipe_num:  EP producer pipe number

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_open_close_cmd
(
  rmnet_xport_cmd_e_type  cmd,
  sio_port_id_type        port_id,
  uint32                  ep_id,
  uint32                  consumer_pipe_num,
  uint32                  producer_pipe_num
)
{
  rmnet_xport_cmd_type * cmd_payload;
  ds_command_type      *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
   Get a DS command buffer
 -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(rmnet_xport_cmd_type));
  if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
      DS_ERR_FATAL("Out of memory");
      return FALSE;
  }
  cmd_ptr->hdr.cmd_id = DS_QMI_RMNET_XPORT_CMD;
  cmd_payload = (rmnet_xport_cmd_type *)cmd_ptr->cmd_payload_ptr;

  cmd_payload->cmd               = cmd;
  cmd_payload->port_id           = port_id;
  cmd_payload->ep_id             = ep_id;
  cmd_payload->consumer_pipe_num = consumer_pipe_num;
  cmd_payload->producer_pipe_num = producer_pipe_num;

  ds_put_cmd(cmd_ptr);
  return TRUE;

} /* rmnet_phys_xport_open_close_cmd() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_DTR_CMD()

  DESCRIPTION
    This function sends a command to DCC task to handle DTR event on a
    physical data channel.

  PARAMETERS
    ep_id:           End point ID
    dtr_asserted:    Is DTR asserted

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_dtr_cmd
(
  uint32    ep_id,
  boolean   dtr_asserted
)
{
  rmnet_xport_cmd_type * cmd_payload;
  ds_command_type       *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a DS command buffer
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(rmnet_xport_cmd_type));
  if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
      DS_ERR_FATAL("Out of memory");
      return FALSE;
  }
  cmd_ptr->hdr.cmd_id = DS_QMI_RMNET_XPORT_CMD;
  cmd_payload = (rmnet_xport_cmd_type *)cmd_ptr->cmd_payload_ptr;
 
  cmd_payload->cmd             = RMNET_XPORT_CMD_DTR_CHANGED;
  cmd_payload->ep_id           = ep_id;
  cmd_payload->dtr_asserted    = dtr_asserted;

  ds_put_cmd(cmd_ptr);

  return TRUE;

} /* rmnet_phys_xport_dtr_cmd() */

/*===========================================================================
  FUNCTION RMNET_XPORT_CMD_HDLR()

  DESCRIPTION
    This is the command handler for rmnet xport commands

  PARAMETERS
    cmd:                DCC_RMNET_XPORT_CMD
    user_data_ptr:      Pointer to rmnet_xport_cmd_type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_xport_cmd_hdlr
(
  ds_command_enum_type   cmd,
  void              * user_data_ptr
)
{
  rmnet_xport_cmd_type * cmd_ptr;
  boolean                result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (user_data_ptr == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr = (rmnet_xport_cmd_type *)user_data_ptr;

  DS_MSG1(MSG_LEGACY_ERROR,"Recvd rmnet xport cmd %d", cmd_ptr->cmd);

  switch (cmd_ptr->cmd)
  {
    case RMNET_XPORT_CMD_PORT_OPEN:
      result = rmnet_phys_xport_open(cmd_ptr->port_id, cmd_ptr->ep_id,
                cmd_ptr->consumer_pipe_num, cmd_ptr->producer_pipe_num); 
      qmux_rmnet_inst_open_close_result(
            result, QMI_CMD_RMNET_OPEN_INSTANCE, cmd_ptr->ep_id);
      break;
  
    case RMNET_XPORT_CMD_PORT_CLOSE:
      result = rmnet_phys_xport_close(cmd_ptr->ep_id); 
      qmux_rmnet_inst_open_close_result(
            result, QMI_CMD_RMNET_CLOSE_INSTANCE, cmd_ptr->ep_id);
      break;

    case RMNET_XPORT_CMD_EP_CFG_DONE:
      rmnet_xport_ep_cfg_done(cmd_ptr->stream_id);
      break;

    case RMNET_XPORT_CMD_DTR_CHANGED:
      rmnet_phys_xport_dtr_changed(cmd_ptr->ep_id, cmd_ptr->dtr_asserted);
      break;

    default:
      DS_ASSERT(0);
      break;
  }

  //DS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);Taken care by the ds task

} /* rmnet_xport_cmd_hdlr() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_FROM_EP_ID()

  DESCRIPTION
    This function retrieves the rmnet physical xport from EP id.

  PARAMETERS
    ep_id:    End point ID

  RETURN VALUE
    Pointer to the rmnet phys xport which has the specified ep_id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_phys_xport_type * rmnet_phys_xport_from_ep_id
(
  uint32  ep_id
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ep_id == 0)
  {
    return NULL;
  }

  for (i = 0; i < QMI_INSTANCE_MAX; i++)
  {
    if (rmnet_phys_xport[i] != NULL &&
               rmnet_phys_xport[i]->ep_id == ep_id)
    {
      return rmnet_phys_xport[i];
    }
  }

  return NULL;

} /* rmnet_phys_xport_from_ep_id() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_SET_DATA_FORMAT()

  DESCRIPTION
    This function sets the data format of a physical xport

  PARAMETERS
    ep_id:              End point ID
    data_format:        Data format structure (NULL for default)
    data_format_mask:   Data format mask
    client_type:        Set data format client

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_set_data_format
(
  uint32                    ep_id,
  rmnet_data_format_type  * data_format,
  uint32                    data_format_mask,
  rmnet_set_data_fmt_e_type client_type
)
{
  rmnet_phys_xport_type    * phys_xport;
  rmnet_xport_type         * xport_info;
  rmnet_data_format_type     data_format_to_set;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unknown EP 0x%x", ep_id);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Check if data calls are up. For DTR LOW, just set data format no matter
   * calls are up or not because all calls will be torn down. RmNet uses
   * data format cache while in a call not the one in the phys xport.
  -------------------------------------------------------------------------*/
  if (client_type != RMNET_SET_DATA_FORMAT_DTR_LOW)
  {
    xport_info = phys_xport->xport_list;
    while (xport_info != NULL)
    {
      if ( (xport_info->in_call_cb != NULL) &&
           (xport_info->in_call_cb(xport_info->user_info)) )
      {
        DS_MSG1(MSG_LEGACY_ERROR,"EP 0x%x is in call, can't change data format", ep_id);
        if (data_format != NULL)
        {
          *data_format = phys_xport->data_format;
        }
        return TRUE;
      }
      xport_info = xport_info->next;
    }
  }

  /*---------------------------------------------------------------------
   * If data_format == NULL, use default data format.
   * Otherwise it is customized data format
  ---------------------------------------------------------------------*/

  memset(&data_format_to_set, 0, sizeof(data_format_to_set));
  if (rmnet_efs_global_config_params.eth_mode_enabled)
  {
    data_format_to_set.link_prot = RMNET_ETHERNET_MODE;
  }
  else
  {
    data_format_to_set.link_prot = RMNET_IP_MODE;
  }
  data_format_to_set.qos = 0;
  data_format_to_set.ul_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
  data_format_to_set.dl_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
  data_format_to_set.qos_format = RMNET_QOS_FORMAT_6_BYTE;

  if (data_format != NULL)
  {
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_QOS)
    {
      data_format_to_set.qos = data_format->qos;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_LINK_PROT)
    {
      data_format_to_set.link_prot = data_format->link_prot;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_UL_DATA_AGG)
    {
      data_format_to_set.ul_data_agg_protocol = data_format->ul_data_agg_protocol;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_DL_DATA_AGG)
    {
      data_format_to_set.dl_data_agg_protocol = data_format->dl_data_agg_protocol;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_NDP_SIGNATURE)
    {
      data_format_to_set.ndp_signature = data_format->ndp_signature;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_DL_DATA_AGG_NUM)
    {
      data_format_to_set.dl_data_agg_max_num = data_format->dl_data_agg_max_num;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_DL_DATA_AGG_SIZE)
    {
      data_format_to_set.dl_data_agg_max_size = data_format->dl_data_agg_max_size;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_DL_MIN_PADDING)
    {
      data_format_to_set.dl_min_padding = data_format->dl_min_padding;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_QOS_FORMAT)
    {
      data_format_to_set.qos_format = data_format->qos_format;
    }
    if (data_format_mask & RMNET_DATA_FORMAT_MASK_FLOW_CONTROL)
    {
      data_format_to_set.te_flow_control = data_format->te_flow_control;
    }
  }

  /*-------------------------------------------------------------------------
   * Link protocol
  -------------------------------------------------------------------------*/
  if (data_format_to_set.link_prot & RMNET_IP_MODE)
  {
    data_format_to_set.link_prot = RMNET_IP_MODE;
  }
  else if (!rmnet_efs_global_config_params.eth_mode_enabled)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Ethernet mode not allowed!");
    data_format_to_set.link_prot = RMNET_IP_MODE;
  }
  else
  {
    data_format_to_set.link_prot = RMNET_ETHERNET_MODE;
  }

  /*-------------------------------------------------------------------------
   * Check global data format mask 
  -------------------------------------------------------------------------*/
  if (data_format_to_set.ul_data_agg_protocol != RMNET_DISABLE_DATA_AGG)
  {
    if (!RMNET_DATA_AGG_IS_QMAP(data_format_to_set.ul_data_agg_protocol))
    {
       DS_MSG1(MSG_LEGACY_ERROR,"Ul data_aggr_protocol can not be %d",
               data_format_to_set.ul_data_agg_protocol);
       return FALSE;
    }
    if ( (rmnet_efs_global_config_params.ul_agg_prot_mask &
            (1 << (data_format_to_set.ul_data_agg_protocol - 1))) == 0 )
    {
      data_format_to_set.ul_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
    }
  }

  if (data_format_to_set.dl_data_agg_protocol != RMNET_DISABLE_DATA_AGG)
  {
    if (!RMNET_DATA_AGG_IS_QMAP(data_format_to_set.dl_data_agg_protocol))
    {
       DS_MSG1(MSG_LEGACY_ERROR,"Ul data_aggr_protocol can not be %d",
               data_format_to_set.dl_data_agg_protocol);
       return FALSE;
    }
    if ( (rmnet_efs_global_config_params.dl_agg_prot_mask &
            (1 << (data_format_to_set.dl_data_agg_protocol - 1))) == 0 )
    {
      data_format_to_set.dl_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
    }
  }

  if (RMNET_DATA_AGG_IS_QMAP(data_format_to_set.dl_data_agg_protocol))
  {
    // No aggregation by default
    if (data_format_to_set.dl_data_agg_max_num == 0)
    {
      data_format_to_set.dl_data_agg_max_num = 1;
    }

    if (data_format_to_set.dl_data_agg_max_size == 0)
    {
      data_format_to_set.dl_data_agg_max_size = 0xFFFFFFFF;
    }
  }
  else
  {
    // DL min padding is for QMAP only
    data_format_to_set.dl_min_padding = 0;
  }

  /*-------------------------------------------------------------------------
   * Set driver data format
  -------------------------------------------------------------------------*/
  if ( ! rmnet_shim_set_data_format
                     (phys_xport, &data_format_to_set) )
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Failed to set data format UL [%d] DL [%d]",
                    data_format_to_set.ul_data_agg_protocol,
                    data_format_to_set.dl_data_agg_protocol);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Success
  -------------------------------------------------------------------------*/
  if (data_format != NULL)
  {
    *data_format = data_format_to_set;
  }

  phys_xport->data_format = data_format_to_set;
  phys_xport->data_format.data_format_client = client_type; 

  DS_MSG3(MSG_LEGACY_ERROR,"Set data format EP [0x%x] UL [%d] DL [%d]",
                  ep_id,
                  data_format_to_set.ul_data_agg_protocol,
                  data_format_to_set.dl_data_agg_protocol);

  return TRUE;

} /* rmnet_phys_xport_set_data_format() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_GET_DATA_FORMAT()

  DESCRIPTION
    This function queries the data format of a physical xport

  PARAMETERS
    ep_id:           End point ID
    data_format:     Pointer to data format result structure

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_get_data_format
(
  uint32                    ep_id,
  rmnet_data_format_type  * data_format
)
{
  rmnet_phys_xport_type    * phys_xport;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unknown EP 0x%x", ep_id);
    return FALSE;
  }

  if (data_format != NULL)
  {
    *data_format = phys_xport->data_format;
  }

  return TRUE;

} /* rmnet_phys_xport_get_data_format() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_DATA_FORMAT_CLIENT()

  DESCRIPTION
    This function queries the last client who sets the data format

  PARAMETERS
    ep_id:           End point ID

  RETURN VALUE
    Data format client type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_set_data_fmt_e_type rmnet_phys_xport_data_format_client
(
  uint32  ep_id
)
{
  rmnet_phys_xport_type    * phys_xport;
  rmnet_set_data_fmt_e_type  data_format_client;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    data_format_client = RMNET_SET_DATA_FORMAT_NONE;
  }
  else
  {
    data_format_client = phys_xport->data_format.data_format_client;
  }

  return data_format_client;

} /* rmnet_phys_xport_data_format_client() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_GET_QMAP_SETTINGS()

  DESCRIPTION
    This function queries QMAP settings

  PARAMETERS
    ep_id:           End point ID
    qmap_settings:   Pointer to QMAP settings result structure

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_get_qmap_settings
(
  uint32                          ep_id,
  rmnet_qmap_settings_type      * qmap_settings
)
{
  rmnet_phys_xport_type    * phys_xport;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unknown EP 0x%x", ep_id);
    return FALSE;
  }

  if (qmap_settings != NULL)
  {
    *qmap_settings = phys_xport->qmap_settings;
  }

  return TRUE;

} /* ds_phys_xport_get_qmap_settings() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_SET_QMAP_SETTINGS()

  DESCRIPTION
    This function sets QMAP settings of a physical xport

  PARAMETERS
    ep_id:              End point ID
    qmap_settings:      QMAP settings structure

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_set_qmap_settings
(
  uint32                          ep_id,
  rmnet_qmap_settings_type      * qmap_settings
)
{
  rmnet_phys_xport_type    * phys_xport;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unknown EP 0x%x", ep_id);
    return FALSE;
  }

  if (qmap_settings != NULL)
  {
    phys_xport->qmap_settings = *qmap_settings;
  }

  return TRUE;

} /* rmnet_phys_xport_set_qmap_settings() */
/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_INIT()

  DESCRIPTION
    This function initialize a logical data transport

  PARAMETERS
    xport_info:   Pointer to rmnet xport structure
    init_cb:      Callback function to init the structure
    user_data:    User data for the rmnet xport

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_init
(
  rmnet_xport_type          * xport_info,
  rmnet_xport_init_cb_type    init_cb,
  void                      * user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (xport_info == NULL || init_cb == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid parameters");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Initialize xport
  -------------------------------------------------------------------------*/
  if (xport_info->inited == FALSE)
  {
    if (!init_cb(xport_info))
    {
      DS_MSG0(MSG_LEGACY_ERROR,"xport init failed");
      return FALSE;
    }
    xport_info->user_info = user_data;    
    xport_info->stream_id = SIO_NO_STREAM_ID;
    xport_info->inited = TRUE;
  }

  return TRUE;

} /* rmnet_logical_xport_init() */

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_OPEN()

  DESCRIPTION
    This function opens a logical data transport

  PARAMETERS
    xport_info:  Pointer to rmnet xport structure

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    rmnet xport must be initialized

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_open
(
  rmnet_xport_type          * xport_info
)
{
  sio_port_id_type       port_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (xport_info == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid RmNet xport");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Check if the logical port is already opened
  -------------------------------------------------------------------------*/
  if (RMNET_XPORT_IS_ACTIVE(xport_info))
  {
    /* We will close the port when calls are terminated. Port mapping and
     * data format won't change if in a call so no need to do port mapping
     * config if port is already opened */
    DS_MSG1(MSG_LEGACY_ERROR,"Logical SIO port 0x%x already active",
                    xport_info->sio_port);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
   * If HW port, pick one unused SIO port. For SW port use the phys SIO port
  -------------------------------------------------------------------------*/
  if (xport_info->is_hw_port)
  {
    port_id = rmnet_shim_get_logical_sio_port();
  }
  else
  {
    port_id = xport_info->phys_xport->sio_port;
  }

  if (port_id == SIO_PORT_NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No available SIO port");
    return FALSE;
  }

  xport_info->sio_port = port_id;
  xport_info->ep_cfg_cnt = 0;

  /*-------------------------------------------------------------------------
   * Open logical port. Stream ID is incremental so normally we wont' get the
   * same stream ID if a port is closed then re-opened. But if the old stream
   * ID which has the EP config pending is being re-used, the EP config event
   * for the old stream ID might get errorenously delivered. 
  -------------------------------------------------------------------------*/
  xport_info->stream_id = rmnet_sio_open(
                               xport_info->sio_port,
                               &xport_info->rx_wmk,
                               &xport_info->tx_wmk);

  if (xport_info->stream_id == SIO_NO_STREAM_ID)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Failed to open logical SIO port 0x%x",
                    xport_info->sio_port );
    xport_info->stream_id = SIO_NO_STREAM_ID;
    if (xport_info->is_hw_port)
    {
      rmnet_shim_free_logical_sio_port(xport_info->sio_port);
    }
    return FALSE;
  }

  xport_info->rx_wmk_fc = FALSE;
  xport_info->tx_wmk_fc = FALSE;

  /*-------------------------------------------------------------------------
   * Update port config mapping
  -------------------------------------------------------------------------*/
  if (!rmnet_xport_set_port_mapping(xport_info))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Failed to set port mapping config");
    rmnet_logical_xport_close(xport_info);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Update data format cache. Data format cache is used for RmNet calls.
   * This is needed if phys port is closed but RmNet is in the process
   * of tearing down. Phys data format is reset but RmNet needs to know the
   * original data format (for cleanup purpose).
  -------------------------------------------------------------------------*/
  if (xport_info->phys_xport != NULL)
  {
    xport_info->data_format_cache = xport_info->phys_xport->data_format;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Unable to update data format cache");
    rmnet_logical_xport_close(xport_info);
    return FALSE;
  }

  return TRUE;

} /* rmnet_logical_xport_open() */

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_CLOSE()

  DESCRIPTION
    This function closes a logical data transport

  PARAMETERS
    xport_info:  Pointer to the rmnet xport structure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_close
(
  rmnet_xport_type    * xport_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!RMNET_XPORT_IS_ACTIVE(xport_info))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"RmNet xport not active");
    return;
  }

  rmnet_logical_xport_clear_wmk_queues(xport_info);

  /*-------------------------------------------------------------------------
   * Close the logical port
  -------------------------------------------------------------------------*/
  rmnet_sio_close(xport_info->stream_id);
  xport_info->stream_id = SIO_NO_STREAM_ID;

  if (xport_info->is_hw_port)
  {
    rmnet_shim_free_logical_sio_port(xport_info->sio_port);
  }
  xport_info->sio_port = SIO_PORT_NULL;
  xport_info->ep_cfg_cnt = 0;

} /* rmnet_logical_xport_close() */

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_BIND()

  DESCRIPTION
    This function binds a logical data port to a physical port

  PARAMETERS
    xport_info:  rmnet logical xport
    phys_xport:  rmnet physical xport
    mux_id:      mux_id of the binding

  RETURN VALUE
    TRUE: Success
    FALSE: Failure    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_bind
(
  rmnet_xport_type       * xport_info,
  rmnet_phys_xport_type  * phys_xport,
  uint8                    mux_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( xport_info == NULL || phys_xport == NULL )
  {
    return FALSE;
  }

  if (xport_info->phys_xport != NULL)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"RmNet xport 0x%x already bound to 0x%x",
                    xport_info->sio_port, phys_xport->sio_port);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * SW port only supports 1-to-1 binding
  -------------------------------------------------------------------------*/
  if ((!phys_xport->is_hw_port) && (phys_xport->xport_list != NULL))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"SW port doesn't support mux binding");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Bind xport to phys_xport
  -------------------------------------------------------------------------*/
  xport_info->next = phys_xport->xport_list;
  phys_xport->xport_list = xport_info;

  xport_info->phys_xport = phys_xport;
  xport_info->ep_id      = phys_xport->ep_id;
  xport_info->mux_id     = mux_id;
  xport_info->is_hw_port = phys_xport->is_hw_port;

  /*-------------------------------------------------------------------------
   * Send DTR if is asserted
  -------------------------------------------------------------------------*/
  if (xport_info->dtr_cb != NULL && phys_xport->dtr_is_high)
  {
    xport_info->dtr_cb(xport_info->user_info, phys_xport->dtr_is_high);
  }

  return TRUE;

} /* rmnet_logical_xport_bind() */

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_UNBIND()

  DESCRIPTION
    This function unbinds a logical data port from the phys port

  PARAMETERS
    xport_info:  Pointer to the rmnet xport structure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_unbind
(
  rmnet_xport_type       * xport_info
)
{
  rmnet_phys_xport_type  * phys_xport;
  rmnet_xport_type       * xport_prev;
  rmnet_xport_type       * xport_this;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_xport = xport_info->phys_xport;

  /*-------------------------------------------------------------------------
   * Remove xport_info from xport list
  -------------------------------------------------------------------------*/
  if (phys_xport != NULL)
  {
    xport_prev = NULL;
    xport_this = phys_xport->xport_list;
    while (xport_this != NULL)
    {
      if (xport_this == xport_info)
      {
        if (xport_prev == NULL)
        {
          phys_xport->xport_list = xport_this->next;
        }
        else
        {
          xport_prev->next = xport_this->next;
        }
        break;
      }

      xport_prev = xport_this;
      xport_this = xport_this->next;
    }

    // Trigger a DTR low
    if (xport_info->dtr_cb != NULL)
    {
      xport_info->dtr_cb(xport_info->user_info, FALSE);
    }
  }

  xport_info->phys_xport = NULL;
  xport_info->ep_id      = 0;
  xport_info->mux_id     = 0;

} /* rmnet_logical_xport_unbind() */


/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_CLEAR_WMK_QUEUES()

  DESCRIPTION
    Clear any data present in the rx and tx wmks.

  PARAMETERS
    xport_info: ptr to rmnet xport

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_clear_wmk_queues
(
  rmnet_xport_type  * xport_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !RMNET_XPORT_IS_INITED(xport_info) )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Free any buffered data (Cleanup Rm rx and tx watermark queues)
  -------------------------------------------------------------------------*/
  dsm_empty_queue(&xport_info->rx_wmk);
  dsm_empty_queue(&xport_info->tx_wmk);

} /* rmnet_logical_xport_clear_wmk_queues() */

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_GET_WMK_QUEUES()

  DESCRIPTION
    This function returns the pointer to the RX and TX watermark Queues.

  PARAMETERS
    xport_info: ptr to rmnet xport
    rx_wmk (OUT): pointer to RX watermark.
    tx_wmk (OUT): pointer to TX watermark.

  RETURN VALUE
    Failure : -1
    Success :  0

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int16 rmnet_logical_xport_get_wmk_queues
(
  rmnet_xport_type     * xport_info,
  dsm_watermark_type  ** rx_wmk,
  dsm_watermark_type  ** tx_wmk
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !RMNET_XPORT_IS_INITED(xport_info) )
  {
    return -1;
  }

  *rx_wmk = &xport_info->rx_wmk;
  *tx_wmk = &xport_info->tx_wmk;

  return 0;

} /* rmnet_logical_xport_get_wmk_queues() */

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_SET_RX_CB()

  DESCRIPTION
    Specify the callback function that should be invoked when data arrives
    on the data path associated with rmnet xport

  PARAMETERS
    xport_info       :  ptr to rmnet xport
    rx_cb            :  callback to be called when data arrives on the data
                        path associated with rmnet xport
                        1st param = dsm pointer to received packet
                        2nd param = user_data provided
    user_data        :  void * to be passed in the 2nd parameter each time
                        rx_cb is invoked

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    Clears any data previously queued in the rx watermark
===========================================================================*/
void rmnet_logical_xport_set_rx_cb
(
  rmnet_xport_type  * xport_info,
  void (* rx_cb)( dsm_watermark_type *, void * ),
  void              * user_data
)
{
  uint32  bytes_cleared = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !RMNET_XPORT_IS_INITED(xport_info) )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    user data is passed back to the rx_cb as the second parameter (void *)
    to allow the upper layer to distinguish between multiple instances of
    rmnet_sio.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    If passed receive callback is null, set the default rx callback.
  -------------------------------------------------------------------------*/
  xport_info->rx_wmk.non_empty_func_ptr = 
                       rx_cb ? rx_cb : rmnet_xport_default_rx_cb;

  xport_info->rx_wmk.non_empty_func_data = user_data;

  /*-------------------------------------------------------------------------
    When changing the receive function, it is important to clear any
    outstanding data as by changing the receive callback, the caller is
    interrupting the normal data flow and hence the previously registered
    logic will no longer be invoked to handle the previously received data.
  -------------------------------------------------------------------------*/
  if (xport_info->rx_wmk.current_cnt != 0)
  {
    bytes_cleared = xport_info->rx_wmk.current_cnt;
    dsm_empty_queue(&xport_info->rx_wmk);
  }

  if (bytes_cleared)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Cleared %d bytes from Rx WM 0x%x",
                    bytes_cleared, &xport_info->rx_wmk);
  }

} /* rmnet_logical_xport_set_rx_cb() */

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_SET_RX_FLOW()

  DESCRIPTION
    Flow control the link specified by rmnet xport.

  PARAMETERS
    xport_info       : ptr to rmnet xport
    disable          :  FALSE = enable flow
                        TRUE  = disable flow

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_set_rx_flow
(
  rmnet_xport_type  * xport_info,
  boolean             disable
)
{
  sio_ioctl_param_type      ioctl_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!RMNET_XPORT_IS_ACTIVE(xport_info))
  {
    return;
  }

  memset(&ioctl_param, 0, sizeof(ioctl_param));
  if (disable)
  {
    sio_ioctl( xport_info->stream_id,
               SIO_IOCTL_INBOUND_FLOW_DISABLE,
               &ioctl_param );
  }
  else /* enable */
  {
    sio_ioctl( xport_info->stream_id,
               SIO_IOCTL_INBOUND_FLOW_ENABLE,
               &ioctl_param );
  }

  DS_MSG1(MSG_LEGACY_ERROR,"RX FLOW CTRL: %c", disable ? '+' : '-');

} /* rmnet_logical_xport_set_rx_flow() */

/*===========================================================================
  FUNCTION RMNET_XPORT_DEFAULT_RX_CB()

  DESCRIPTION
    Default recieve function for rmnet instance.  Simply frees received
    data.  Controller of rmnet instance must register a real receive
    callback via rmnet_xport_set_rx_cb() in order to connect the data link
    to the link layer data protcol above.

  PARAMETERS
    rx_wm_ptr           -  receive watermark on which rx data is enqueued
    rx_wm_ptr_user_data -  ignored

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_xport_default_rx_cb
(
  dsm_watermark_type *  rx_wm_ptr,
  void *                rx_wm_user_data
)
{
  dsm_item_type *  item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    When no rx_cb set by upper layer, data shouldn't arrive, but if it
    does, just throw it away
  -------------------------------------------------------------------------*/
  while ((item = dsm_dequeue(rx_wm_ptr)) != NULL)
  {
    dsm_free_packet( &item );
  }
} /* rmnet_xport_default_rx_cb() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_INC_TETH_CALL_CNT()

  DESCRIPTION
    Increments count of number of tethered calls on this physical port.

  PARAMETERS
    phys_xport           -  Pointer to physical port

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_phys_xport_inc_teth_call_cnt
(
  rmnet_phys_xport_type   * phys_xport
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL != phys_xport)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Teth call cnt before inc %d %d",phys_xport->teth_call_cnt,0);
    phys_xport->teth_call_cnt++;
    if (phys_xport->teth_call_cnt == 1)
  {
      rmnet_shim_notify_tethered_call_state(phys_xport, TRUE);
    }
  }
   
} /* rmnet_phys_xport_inc_teth_call_cnt() */

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_DEC_TETH_CALL_CNT()

  DESCRIPTION
    Decrements count of number of tethered calls on this physical port.

  PARAMETERS
    phys_xport           -  Pointer to physical port

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_phys_xport_dec_teth_call_cnt
(
  rmnet_phys_xport_type   * phys_xport
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL != phys_xport)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Teth call cnt before dec %d ",phys_xport->teth_call_cnt);
    if (phys_xport->teth_call_cnt > 0)
  {
      phys_xport->teth_call_cnt--;
      
      if (phys_xport->teth_call_cnt == 0)
    {
        rmnet_shim_notify_tethered_call_state(phys_xport, FALSE);
      }
    }
  }  
  
} /* rmnet_phys_xport_dec_teth_call_cnt() */

