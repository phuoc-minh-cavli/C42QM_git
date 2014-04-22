/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X   I O

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed
  for MUX input - output processing

  Rx signal handler - Handle all incoming traffic from host
  Tx signal hanlder - Handle all outgoing traffic to host

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_io.c#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "ds_mux_utils.h"
#include "ds_mux_io.h"
#include "ds_mux.h"
#include "ds_mux_defs.h"
#include "ds_mux_logging.h"


extern qti_bridge_qurt_t  qti_bridge_qurt;

ds_mux_io_info_type   *ds_mux_io_info = NULL;

/*===========================================================================
                      LOCAL FUNCTION DECLARATIONS
===========================================================================*/

ds_mux_result_enum_type ds_mux_io_process_rx_frames
(
  ds_mux_io_rx_info        *rx_info
);

ds_mux_result_enum_type ds_mux_io_validate_frame
(
  ds_mux_io_frame_type    *frame
);


/*===========================================================================
                       FUNCTION DEFINATION
===========================================================================*/

void ds_mux_io_init
(

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_io_info = NULL;
}/* ds_mux_io_init */

void ds_mux_io_deinit
(

)
{
  ds_mux_io_frame_struct_type  *item = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL != ds_mux_io_info)
  {

     /*-------------------------------------------------------------------------
       Flush the command and messages queue and  Deinitialize queues
     -------------------------------------------------------------------------*/
     while ( (item = (ds_mux_io_frame_struct_type *) q_get( &ds_mux_io_info->rx_info.decoded_frame_q )) != NULL )
     {
       dsm_free_packet(&item->frame.information_ptr);
       DS_MUX_MEM_FREE(item);
     }
     q_destroy( &ds_mux_io_info->rx_info.decoded_frame_q );

     ds_mux_io_info->rx_info.curr_frame_state = DS_MUX_IO_FRAME_INIT;

     if ( NULL != ds_mux_io_info->rx_info.curr_frame_ptr)
     {
       dsm_free_packet(&ds_mux_io_info->rx_info.curr_frame_ptr->frame.information_ptr);
       DS_MUX_MEM_FREE(ds_mux_io_info->rx_info.curr_frame_ptr);
       ds_mux_io_info->rx_info.curr_frame_ptr   = NULL;
     }

     DS_MUX_MEM_FREE(ds_mux_io_info);
  }

}/* ds_mux_io_deinit */

#if 0
static void  ds_mux_io_dlci0_cmd_low_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlci0_cmd_low_func */

static void  ds_mux_io_dlci0_cmd_hi_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlci0_cmd_hi_func */


static void  ds_mux_io_dlci0_cmd_non_empty_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlci0_cmd_non_empty_func */


static void  ds_mux_io_dlci0_msg_low_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlci0_cmd_low_func */

static void  ds_mux_io_dlci0_msg_hi_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlci0_cmd_hi_func */


static void  ds_mux_io_dlci0_msg_non_empty_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlci0_cmd_non_empty_func */


static void  ds_mux_io_dlciN_cmd_low_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlci0_cmd_low_func */

static void  ds_mux_io_dlciN_cmd_hi_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlciN_cmd_hi_func */


static void  ds_mux_io_dlciN_cmd_non_empty_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlciN_cmd_non_empty_func */


static void  ds_mux_io_dlciN_msg_low_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlciN_cmd_low_func */

static void  ds_mux_io_dlciN_msg_hi_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlciN_cmd_hi_func */


static void  ds_mux_io_dlciN_msg_non_empty_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}/* ds_mux_io_dlciN_cmd_non_empty_func */
#endif

ds_mux_result_enum_type ds_mux_io_initialize_wmk
(

)
{
  ds_mux_result_enum_type  result = DS_MUX_SUCCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_io_info = ( ds_mux_io_info_type*)malloc(sizeof(ds_mux_io_info_type));

  if ( NULL == ds_mux_io_info)
  {
    return DS_MUX_FAILURE;
  }
  /*-----------------------------------------------------------------------
     1.  Initialize Rx info
   -----------------------------------------------------------------------*/
   memset(&ds_mux_io_info->rx_info.decoded_frame_q, 0x0, sizeof(q_type) );

  (void) q_init( &ds_mux_io_info->rx_info.decoded_frame_q );
   ds_mux_io_info->rx_info.curr_frame_state = DS_MUX_IO_FRAME_INIT;
   ds_mux_io_info->rx_info.curr_frame_ptr   = NULL;

  return result;
}/* ds_mux_io_initialize_wmk */

ds_mux_result_enum_type ds_mux_io_transmit_cmd_response
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  dsm_item_type*                item_ptr = NULL;
  dsm_item_type**               tx_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //allocate dsm item
  //genereate MUX frames from input info
  // transmit to phy layer
  if ( NULL == (item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
  {
   return DS_MUX_FAILURE;
  }
  tx_ptr = &item_ptr;

  if ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_cmd_input( tx_ptr, frame )))
  {
    result = ds_mux_phys_transmit( *tx_ptr );
  }
  //unable to transmit the data, Free the DSM ITEM
  if ( DS_MUX_FAILURE == result)
  {
    dsm_free_packet( tx_ptr );
  }
  return result;
}/* ds_mux_io_transmit_cmd_response*/

ds_mux_result_enum_type ds_mux_io_transmit_msg_response
(
  ds_mux_io_frame_type    *frame
)
{
  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  dsm_item_type**               information_ptr_ptr = &frame->information_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( FALSE == ds_mux_is_flow_enabled (DS_MUX_DLCI_ID_0) )
  {
    return DS_MUX_FLOW_DISABLED;
  }

  if ( ( NULL != frame->information_ptr ) &&
       ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_msg_input(  information_ptr_ptr, frame ))))
  {
    result = ds_mux_phys_transmit( *information_ptr_ptr);
  }

  return result;
}/* ds_mux_io_transmit_msg_response*/

ds_mux_result_enum_type ds_mux_io_transmit_cl_one_data
(
  ds_mux_dlci_param_type        *dlci_conn_param,
  dsm_item_type                **tx_pkt_ptr_ptr

)
{
  boolean                       processing_complete = FALSE;
  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  ds_mux_io_frame_type          frame;
  dsm_watermark_type           *logical_tx_wmk_ptr     = NULL;
  ds_mux_sig_enum_type          sig;
  dsm_item_type                **head_dsm_frame = NULL;
  dsm_item_type                 *tail_dsm_frame = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&frame, 0x0, sizeof(ds_mux_io_frame_type));

    /*-----------------------------------------------------------------------
    5.5.1  Type 1 - Unstructured Octet Stream
    If packet length is less than negotiated frame size then packet will be inserted into frame directly
    otherwise it will be Split into frame size
  -----------------------------------------------------------------------*/
  ds_mux_util_prepare_data_frame( dlci_conn_param->dlci_id,
    ds_mux_util_convert_frame_type(dlci_conn_param->frame_type),  &frame );

  head_dsm_frame = tx_pkt_ptr_ptr;

  while ( (FALSE == processing_complete) &&  (NULL != head_dsm_frame) &&
          (NULL != *head_dsm_frame)  && (0 != dsm_length_packet(*head_dsm_frame)))
  {
    if ( dsm_length_packet(*head_dsm_frame) <= dlci_conn_param->frame_size_N1 )
    {
      processing_complete = TRUE;
    }
    else
    {
      dsm_split_packet( head_dsm_frame, &tail_dsm_frame, dlci_conn_param->frame_size_N1, DSM_DS_LARGE_ITEM_POOL );
      processing_complete = FALSE;
    }

    frame.information_ptr = *head_dsm_frame;
    frame.length          = dsm_length_packet(frame.information_ptr);
    /*-----------------------------------------------------------------------
      Enqueue into Tx Wm
      -----------------------------------------------------------------------*/
    if ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_data_input( head_dsm_frame, &frame )))
    {
        if( FALSE == ds_mux_is_flow_enabled (dlci_conn_param->dlci_id))
        {
            logical_tx_wmk_ptr = ds_mux_logical_get_tx_wmk( dlci_conn_param->dlci_id );

            if ( NULL != logical_tx_wmk_ptr)
            {
                dsm_enqueue(logical_tx_wmk_ptr, head_dsm_frame);

                sig = ds_mux_get_sig_by_id( dlci_conn_param->dlci_id );

                DS_MUX_SET_SIGNAL(sig);
            }
            else
            {
                result = DS_MUX_FAILURE;
            }
        }
        else
        {
            result = ds_mux_phys_transmit(*head_dsm_frame);
        }
    }

    if ( FALSE == processing_complete )
    {
      *head_dsm_frame = tail_dsm_frame;
      tail_dsm_frame = NULL;
    }
  }

  if ( DS_MUX_FAILURE == result && NULL != head_dsm_frame && NULL != *head_dsm_frame)
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_io_transmit_cl_two_data(): Unable to transmit the packet");
    dsm_free_packet(head_dsm_frame);
  }

  return result;
}/*ds_mux_io_transmit_cl_one_data*/


ds_mux_result_enum_type ds_mux_io_transmit_cl_two_data
(
  ds_mux_dlci_param_type        *dlci_conn_param,
  uint8                          fc_flag,
  uint8                          rtc_asserted_flag,
  uint8                          cd_asserted_flag,
  dsm_item_type                **tx_pkt_ptr_ptr
)
{
  boolean                       processing_complete = FALSE;
  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  ds_mux_io_frame_type          frame;
  dsm_watermark_type           *logical_tx_wmk_ptr     = NULL;
  ds_mux_sig_enum_type          sig;
  dsm_item_type                **head_dsm_frame = NULL;
  dsm_item_type                 *tail_dsm_frame = NULL;
  uint8                         signal_frame_value;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&frame, 0x0, sizeof(ds_mux_io_frame_type));

  signal_frame_value = (DS_MUX_DLCI_MSG_TYPE_EA_MASK | (fc_flag << 1) | (rtc_asserted_flag << 2) |(1 << 3)| (cd_asserted_flag << 7));

  /*-----------------------------------------------------------------------
    5.5.2 Type 2 - Unstructured Octet Stream with Signal byte
    If packet length is less than negotiated frame size then packet will be inserted into frame directly
    otherwise it will be Split into frame size

    Signal Byte will be inserted into every packet at beginning
  -----------------------------------------------------------------------*/
  ds_mux_util_prepare_data_frame( dlci_conn_param->dlci_id,
    ds_mux_util_convert_frame_type(dlci_conn_param->frame_type),  &frame );

  head_dsm_frame = tx_pkt_ptr_ptr;

  while ( (FALSE == processing_complete) &&  (NULL != head_dsm_frame) &&
          (NULL != *head_dsm_frame)  && (0 != dsm_length_packet(*head_dsm_frame)))
  {
    /*-----------------------------------------------------------------------
      As we are gona insert another byte Hence len is less then only
     -----------------------------------------------------------------------*/
    if ( dsm_length_packet(*head_dsm_frame) < dlci_conn_param->frame_size_N1 )
    {
      processing_complete = TRUE;
    }
    else
    {
      dsm_split_packet( head_dsm_frame, &tail_dsm_frame, dlci_conn_param->frame_size_N1, DSM_DS_LARGE_ITEM_POOL );
      processing_complete = FALSE;
    }
    /*-----------------------------------------------------------------------
      Insert signal byte
      -----------------------------------------------------------------------*/
    if (1 != dsm_pushdown(head_dsm_frame, &signal_frame_value, 1, DSM_DS_SMALL_ITEM_POOL))
    {
      result   = DS_MUX_FAILURE;
      processing_complete = FALSE;
      break;
    }

    frame.information_ptr = *head_dsm_frame;
    frame.length          = dsm_length_packet(frame.information_ptr);
    /*-----------------------------------------------------------------------
      Enqueue into Tx Wm
      -----------------------------------------------------------------------*/
    if ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_data_input( head_dsm_frame, &frame )))
    {
      logical_tx_wmk_ptr = ds_mux_logical_get_tx_wmk( dlci_conn_param->dlci_id );

      if ( NULL != logical_tx_wmk_ptr)
      {
        dsm_enqueue(logical_tx_wmk_ptr, head_dsm_frame);

        sig = ds_mux_get_sig_by_id( dlci_conn_param->dlci_id );

        DS_MUX_SET_SIGNAL(sig);
      }
      else
      {
        result = DS_MUX_FAILURE;
      }
    }
    if ( FALSE == processing_complete )
    {
      *head_dsm_frame = tail_dsm_frame;
      tail_dsm_frame = NULL;
    }
  }
  if ( DS_MUX_FAILURE == result && NULL != tx_pkt_ptr_ptr && NULL != *tx_pkt_ptr_ptr)
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_io_transmit_cl_two_data(): Unable to transmit the packet");
    dsm_free_packet(tx_pkt_ptr_ptr);
  }

  return result;
}/*ds_mux_io_transmit_cl_two_data*/


ds_mux_result_enum_type ds_mux_io_transmit_cl_three_data
(
  ds_mux_dlci_param_type        *dlci_conn_param,
  dsm_item_type                **tx_pkt_ptr_ptr

)
{
  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  ds_mux_io_frame_type          frame;
  dsm_watermark_type           *logical_tx_wmk_ptr     = NULL;
  ds_mux_sig_enum_type          sig;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-----------------------------------------------------------------------
    5.5.3 Type 3 - Uninterruptible Framed Data
    If packet length is less than negotiated frame size then packet will be inserted into frame directly
    otherwise packet will be dropped
  -----------------------------------------------------------------------*/

  ds_mux_util_prepare_data_frame( dlci_conn_param->dlci_id,
    ds_mux_util_convert_frame_type ( dlci_conn_param->frame_type ),  &frame );

  if ( (NULL != tx_pkt_ptr_ptr) && (dsm_length_packet(*tx_pkt_ptr_ptr) > dlci_conn_param->frame_size_N1) )
  {
    DS_MUX_LOG_MSG_ERROR_2("ds_mux_io_transmit_cl_three_data(): packet length is exceeding, pkt len %d frame size %d ",
        dsm_length_packet(*tx_pkt_ptr_ptr), dlci_conn_param->frame_size_N1);

    result = DS_MUX_FAILURE;
  }
  else
  {
    if (NULL != tx_pkt_ptr_ptr)
    {
      frame.information_ptr = *tx_pkt_ptr_ptr;
    }
    frame.length          = dsm_length_packet(frame.information_ptr);

     /*-----------------------------------------------------------------------
       Enqueue into Tx Wm
       -----------------------------------------------------------------------*/
    if ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_data_input( tx_pkt_ptr_ptr, &frame )))
    {
      logical_tx_wmk_ptr = ds_mux_logical_get_tx_wmk( dlci_conn_param->dlci_id );

      if ( NULL != logical_tx_wmk_ptr)
      {
         dsm_enqueue(logical_tx_wmk_ptr, tx_pkt_ptr_ptr);

         sig = ds_mux_get_sig_by_id( dlci_conn_param->dlci_id );

         DS_MUX_SET_SIGNAL(sig);
      }
      else
      {
        result = DS_MUX_FAILURE;
      }
    }
  }
  if ( DS_MUX_FAILURE == result && NULL != tx_pkt_ptr_ptr && NULL != *tx_pkt_ptr_ptr)
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_io_transmit_cl_three_data(): Unable to transmit the packet");
    dsm_free_packet(tx_pkt_ptr_ptr);
  }
  return result;
}/*ds_mux_io_transmit_cl_three_data*/


ds_mux_result_enum_type ds_mux_io_transmit_cl_four_data
(
  ds_mux_dlci_param_type        *dlci_conn_param,
  dsm_item_type                **tx_pkt_ptr_ptr

)
{
  boolean                       processing_complete = FALSE;
  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  ds_mux_io_frame_type          frame;
  dsm_watermark_type           *logical_tx_wmk_ptr     = NULL;
  ds_mux_sig_enum_type          sig;
  dsm_item_type                **head_dsm_frame = NULL;
  dsm_item_type                 *tail_dsm_frame = NULL;
  boolean                        is_first_frame = TRUE;
  uint8                          cl4_ctl_header = (uint8)CL_FOUR_CTL_HDR_ENTIRE_FRAME;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&frame, 0x0, sizeof(ds_mux_io_frame_type));

    /*-----------------------------------------------------------------------
    5.5.2 Type 2 - Unstructured Octet Stream with Signal byte
    If packet length is less than negotiated frame size then packet will be inserted into frame directly
    otherwise it will be Split into frames

     Meaning of B and F bits
    B   F   Meaning
    1   0   First frame of a sequence
    0   0   Middle frame of a sequence
    0   1   Last frame of a sequence
    1   1   Data is contained entirely within one frame

    Signal Byte will be inserted into every packet at beginning
  -----------------------------------------------------------------------*/
  ds_mux_util_prepare_data_frame( dlci_conn_param->dlci_id,
    ds_mux_util_convert_frame_type(dlci_conn_param->frame_type),  &frame );

  head_dsm_frame = tx_pkt_ptr_ptr;

  while ( (FALSE == processing_complete) &&  (NULL != head_dsm_frame) &&
          (NULL != *head_dsm_frame)  && (0 != dsm_length_packet(*head_dsm_frame)))
  {
    if ( dsm_length_packet(*head_dsm_frame) < dlci_conn_param->frame_size_N1 )
    {
      processing_complete = TRUE;

      if ( TRUE == is_first_frame)
      {
        if ( 1 != dsm_pushdown_packed (head_dsm_frame, &cl4_ctl_header, 1, DSM_DS_SMALL_ITEM_POOL))
          DS_MUX_SET_ERROR_AND_BREAK(result)
      }
      else
      {
        cl4_ctl_header = (uint8)CL_FOUR_CTL_HDR_LAST_FRAME;
        if ( 1 != dsm_pushdown_packed (head_dsm_frame, &cl4_ctl_header, 1, DSM_DS_SMALL_ITEM_POOL))
          DS_MUX_SET_ERROR_AND_BREAK(result)
      }
      is_first_frame = FALSE;
    }
    else
    {
      dsm_split_packet( head_dsm_frame, &tail_dsm_frame, (dlci_conn_param->frame_size_N1 - 1), DSM_DS_LARGE_ITEM_POOL );

      if ( TRUE == is_first_frame)
      {
        cl4_ctl_header = (uint8)CL_FOUR_CTL_HDR_FIRST_FRAME;
        if ( 1 != dsm_pushdown_packed (head_dsm_frame, &cl4_ctl_header, 1, DSM_DS_SMALL_ITEM_POOL))
          DS_MUX_SET_ERROR_AND_BREAK(result)
      }
      else
      {
        cl4_ctl_header = (uint8)CL_FOUR_CTL_HDR_MIDDLE_FRAME;
        if ( 1 != dsm_pushdown_packed (head_dsm_frame, &cl4_ctl_header, 1, DSM_DS_SMALL_ITEM_POOL))
          DS_MUX_SET_ERROR_AND_BREAK(result)
      }
      is_first_frame = FALSE;
      processing_complete = FALSE;
    }

    frame.information_ptr = *head_dsm_frame;
    frame.length          = dsm_length_packet(frame.information_ptr);

    /*-----------------------------------------------------------------------
      Enqueue into Tx Wm
      -----------------------------------------------------------------------*/
    if ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_data_input( head_dsm_frame, &frame )))
    {
      logical_tx_wmk_ptr = ds_mux_logical_get_tx_wmk( dlci_conn_param->dlci_id );

      if ( NULL != logical_tx_wmk_ptr)
      {
        dsm_enqueue(logical_tx_wmk_ptr, head_dsm_frame);

        sig = ds_mux_get_sig_by_id( dlci_conn_param->dlci_id );

        DS_MUX_SET_SIGNAL(sig);
      }
      else
      {
        result = DS_MUX_FAILURE;
      }
    }
    if ( FALSE == processing_complete )
    {
      *head_dsm_frame = tail_dsm_frame;
      tail_dsm_frame = NULL;
    }
  }

  if ( DS_MUX_FAILURE == result && NULL != head_dsm_frame && NULL != *head_dsm_frame)
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_io_transmit_cl_four_data(): Unable to transmit the packet");
    dsm_free_packet(head_dsm_frame);
  }

  return result;
}/*ds_mux_io_transmit_cl_four_data*/

ds_mux_result_enum_type  ds_mux_io_transmit_data
(
  uint8               dlci_id,
  uint8               fc_flag,
  uint8               rtc_asserted_flag,
  uint8               cd_asserted_flag,
  dsm_item_type     **tx_pkt_ptr_ptr
)
{

  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    dlci_conn_param = ds_mux_get_dlci_conn_ptr((uint32)dlci_id);

    if ( NULL == dlci_conn_param )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( DS_MUX_DLCI_CONNECTED != dlci_conn_param->state )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    switch( dlci_conn_param ->cl_type )
    {
      case DS_MUX_CL_TYPE_1:
      {
        result = ds_mux_io_transmit_cl_one_data( dlci_conn_param, tx_pkt_ptr_ptr);
        break;
      }
      case DS_MUX_CL_TYPE_2:
      {
        result = ds_mux_io_transmit_cl_two_data( dlci_conn_param, fc_flag, rtc_asserted_flag, cd_asserted_flag, tx_pkt_ptr_ptr);
        break;
      }
      case DS_MUX_CL_TYPE_3:
      {
        result = ds_mux_io_transmit_cl_three_data( dlci_conn_param, tx_pkt_ptr_ptr);
        break;
      }
      case DS_MUX_CL_TYPE_4:
      {result = ds_mux_io_transmit_cl_four_data( dlci_conn_param, tx_pkt_ptr_ptr);
        break;
      }
      default :
      {
        result = DS_MUX_FAILURE;
        break;
      }
    }
  }while (0);

  if ( DS_MUX_FAILURE == result && NULL != tx_pkt_ptr_ptr && NULL != *tx_pkt_ptr_ptr)
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_io_transmit_data(): Unable to transmit the packet");
    dsm_free_packet(tx_pkt_ptr_ptr);
  }
  return result;
}/*ds_mux_io_transmit_data*/

boolean ds_mux_io_tx_sig_hdlr
(
  ds_mux_sig_enum_type      sig,
  void                    * user_data_ptr
)
{
  boolean                             result                 = FALSE;
  dsm_watermark_type                 *logical_tx_wmk_ptr     = NULL;
  uint8                               dlci_id                = DS_MUX_DLCI_ID_1;
  dsm_item_type                      *item_ptr           = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == ds_mux_io_info )
  {
    DS_MUX_LOG_MSG_ERROR_1("ds_mux_io_rx_sig_hdlr(): DS MUX IO is not initialized, sig %d ", sig);
    ASSERT(0);
    return TRUE;
  }
   //Comments


  dlci_id = (uint32) user_data_ptr;

  if( dlci_id > DS_MUX_MAX_LOGICAL_CONNECTION )
  {
    return TRUE;
  }
  /*-----------------------------------------------------------------------
      We will not dequeue the packet if flow is disabled once flow is enabled we will set.
      the tx signals again and will start dequeing the data after that
   -----------------------------------------------------------------------*/
  if( FALSE == ds_mux_is_flow_enabled (dlci_id ))
  {
    return TRUE;
  }

  logical_tx_wmk_ptr = ds_mux_logical_get_tx_wmk( dlci_id );

  if (( NULL == logical_tx_wmk_ptr))
  {
    /*-----------------------------------------------------------------------
        Either queue is empty or logical rx wmk become NULL.
        Note: F3 print avoided purposefully as this block hit every time rx wmk become NULL
     -----------------------------------------------------------------------*/
    return TRUE;
  }
  else
  {
    item_ptr = dsm_dequeue( logical_tx_wmk_ptr );
    if (item_ptr == NULL)
    {
      return TRUE;
    }
    while(item_ptr)
    {
      result = ds_mux_phys_transmit( item_ptr );

      if ( DS_MUX_FAILURE == result)
      {
        dsm_free_packet( &item_ptr );
      }
      item_ptr = dsm_dequeue( logical_tx_wmk_ptr );
    }
  }
  return FALSE;
}/*ds_mux_io_tx_sig_hdlr*/

ds_mux_result_enum_type ds_mux_io_validate_frame
(
  ds_mux_io_frame_type    *frame
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if ( IS_DLCI_INVALID( frame->dlci) )
    {
      break;
    }

    if ( DS_MUX_FRAME_TYPE_INVALID_FRAME == frame->frame_type )
    {
      break;
    }

    if ( DS_MUX_UTIL_BASIC_FRAME_MAX_INFO_LEN < frame->length )
    {
      break;
    }

    return DS_MUX_SUCCESS;

  }while(0);

  DS_MUX_LOG_MSG_ERROR_3("ds_mux_io_validate_frame(): Invalid frame dlci id %d frame type %d len %d",
    frame->dlci, frame->frame_type, frame->length );

  return DS_MUX_FAILURE;
}/* ds_mux_io_validate_frame*/

ds_mux_result_enum_type ds_mux_io_process_rx_frames
(
  ds_mux_io_rx_info        *rx_info
)
{
  ds_mux_io_frame_struct_type  *item   = NULL;
  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Retrieved all parsed frame
  -------------------------------------------------------------------------*/
  while ( ( DS_MUX_CLD_PROCESSED != result ) &&
          (item = (ds_mux_io_frame_struct_type *) q_get( &rx_info->decoded_frame_q )) != NULL)
  {
    if ( DS_MUX_SUCCESS != ds_mux_io_validate_frame( &item->frame))
    {
      if ( NULL != item->frame.information_ptr)
      {
        DS_MUX_LOG_MSG_ERROR_0("ds_mux_io_process_rx_frames(): DSM info ptr is NULL ");

        dsm_free_packet( &item->frame.information_ptr);
        item->frame.information_ptr = NULL;
      }
      DS_MUX_MEM_FREE(item);
      continue;
    }
    /*-------------------------------------------------------------------------
      Control packets
        All control packet will be routed to DS MUX for processing. DS MUX will generate response message if required.
        DSM item will be freed by DS MUX
      Data packets
        All non-fragmented data packets will routed to logical interface
        All fragmented packets(Convergence layer 4) will be routed to DS MUX for further processing

      Frame pointer will be freed here
    -------------------------------------------------------------------------*/
    if ( DS_MUX_DLCI_ID_0 == item->frame.dlci )
    {
      result = ds_mux_process_dlci_rx_ctl_frames( &item->frame );

    }
    else
    {
      switch ( item->frame.frame_type )
      {
        case DS_MUX_FRAME_TYPE_UTIL_UIH:
        case DS_MUX_FRAME_TYPE_UTIL_UI:
        {
          if ( DS_MUX_CL_TYPE_4 == ds_mux_get_cl_type( item->frame.dlci ))
          {
            result = ds_mux_process_rx_frag_data_frames( &item->frame );
          }
          else
          {
            result = ds_mux_logical_process_rx_data_frames( &item->frame );
          }
        }
        break;
        case DS_MUX_FRAME_TYPE_UTIL_SABM:
        case DS_MUX_FRAME_TYPE_UTIL_UA:
        case DS_MUX_FRAME_TYPE_UTIL_DM:
        case DS_MUX_FRAME_TYPE_UTIL_DISC:
        {
          result = ds_mux_process_dlci_rx_ctl_frames( &item->frame );
        }
        break;
        case DS_MUX_FRAME_TYPE_INVALID_FRAME:
        default:
        {
          result = DS_MUX_FAILURE;
        }
      }
    }
    DS_MUX_MEM_FREE(item);
  }

  return result;
}/* ds_mux_io_process_rx_frames */


boolean ds_mux_io_rx_sig_hdlr
(
  ds_mux_sig_enum_type      sig,
  void                    * user_data_ptr
)
{
  dsm_item_type                  *item_ptr           = NULL;
  dsm_watermark_type             *phy_rx_wmk_ptr     = NULL;
  ds_mux_result_enum_type         result             = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( NULL == ds_mux_io_info )
  {
    DS_MUX_LOG_MSG_ERROR_1("ds_mux_io_rx_sig_hdlr(): DS MUX IO is not initialized, sig %d ", sig);
    ASSERT(0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     1.  Get physical connection Rx Watermark
   -----------------------------------------------------------------------*/
  phy_rx_wmk_ptr = ds_mux_phy_get_rx_wmk();

  if (( NULL == phy_rx_wmk_ptr) || ( NULL == (item_ptr = dsm_dequeue( phy_rx_wmk_ptr ))))
  {
    /*-----------------------------------------------------------------------
     2. Either queue is empty or phy rx wmk become NULL.
        Note: F3 print avoided purposefully as this block hit every time rx wmk become NULL
    -----------------------------------------------------------------------*/
    return TRUE;
  }
  DS_MUX_LOG_PHY_RX_PACKET( item_ptr);
  do
  {
    /*-----------------------------------------------------------------------
         3. Convert Rx DSM item into frames (Invoke utils decode API )
             3.1 A DSM item can have multiple frames
             3.2 A DSM item can have partial frame at the end and remaining part could be in next
                  DSM item
         4. Fetch frames from frame queue and process
            4.1. Contol packets will be routed to DS MUX
            4.2. ( Convergence layer 4) Fragmented packets will be routed to DS MUX
            4.3. Data packets will be routed to logical iface
     -----------------------------------------------------------------------*/
// TBD call different decoding APIs for normal, modem psc or cld recv mode
   switch( ds_mux_get_mux_state() )
   {
     case DS_MUX_STATE_OPENED:
     {
        result = ds_mux_util_decode_input( item_ptr, &ds_mux_io_info->rx_info);
     }
     break;

     case DS_MUX_STATE_PSC_RCVD:
     {

     }
     break;

     case DS_MUX_STATE_IN_POWER_SAVE:
     {
        result = ds_mux_util_process_power_save_flag( item_ptr, &ds_mux_io_info->rx_info);
     }
     break;

     case DS_MUX_STATE_WAKEUP_FLAG_SENT:
     {
       result = ds_mux_util_process_frame_after_wake_up( item_ptr, &ds_mux_io_info->rx_info );
     }
     break;

     case DS_MUX_STATE_CLD_RCVD:
     {

     }
     break;
     default:
     break;
  }
   /*-----------------------------------------------------------------------
      ERROR:.  Unable to parse DSM item. Aborting current DSM item processing
      Note: DSM item supposed to free in decodeder API.
    -----------------------------------------------------------------------*/
    if ( DS_MUX_SUCCESS != result)
    {
      DS_MUX_LOG_MSG_ERROR_2("ds_mux_io_rx_sig_hdlr(): Aborting Rx processing for"
                             "dsm ptr 0x%x result %d ", item_ptr, result);
    }
    /*Step 4*/
    result = ds_mux_io_process_rx_frames ( &ds_mux_io_info->rx_info );

    if ( DS_MUX_SUCCESS != result)
    {
      DS_MUX_LOG_MSG_ERROR_1("ds_mux_io_rx_sig_hdlr():  Rx frame processing  "
                               " result %d ", result);
      break;
    }
  }while(NULL != (item_ptr = dsm_dequeue( phy_rx_wmk_ptr )));

  return FALSE;
}/* ds_mux_io_rx_sig_hdlr */

ds_mux_result_enum_type ds_mux_io_transmit_msg_cmd
(
  ds_mux_io_frame_struct_type           *output_frame
)
{
  ds_mux_result_enum_type         result             = DS_MUX_SUCCESS;
  dsm_item_type                 **dup_data_ptr       = NULL;
  dsm_item_type                  *data_ptr           = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( FALSE == ds_mux_is_flow_enabled (DS_MUX_DLCI_ID_0) )
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_io_transmit_msg_cmd DLCI 0 is flow controlled");

    return DS_MUX_FLOW_DISABLED;
  }
  /*-----------------------------------------------------------------------
   Original packet will be freed after message response. Creating DUP copy of information DSM item.
   UART will be freeing the DUP copy of this DSM item    .
    -----------------------------------------------------------------------*/
  dsm_dup_packet(&data_ptr, output_frame->frame.information_ptr,0,
                             (uint16)output_frame->frame.length);

  dup_data_ptr = &data_ptr;
  result = ds_mux_util_encode_cmd_input( dup_data_ptr, &output_frame->frame );

  if ( DS_MUX_SUCCESS ==  result)
  {
    result = ds_mux_phys_transmit( *dup_data_ptr );

    if ( DS_MUX_SUCCESS !=  result)
    {
      DS_MUX_LOG_MSG_ERROR_0( "ds_mux_io_transmit_msg_cmd unable to transmit the packet");
      dsm_free_packet(dup_data_ptr);
    }
  }
  return result;
}/* ds_mux_io_transmit_msg_cmd */

