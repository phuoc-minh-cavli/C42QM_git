#ifndef __VSS_INOTIFY_PUBLIC_IF_H__
#define __VSS_INOTIFY_PUBLIC_IF_H__

/**
  @file vss_inotify_public_if.h
  @brief This file contains the APR API definitions for the CVD
  registration-based event notification framework.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.6.c2/api/cvd/vss_inotify_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"

/****************************************************************************
 * CVD EVENT NOTIFICAION APIS                                               *
 ****************************************************************************/

/** @ingroup cvd_reg_evt_notify_listen
  Allows the client to listen for a class of event notifications generated by
  a CVD module.

  @apr_msgpayload{vss_inotify_cmd_listen_for_event_class_t}
  @table{weak__vss__inotify__cmd__listen__for__event__class__t}

  @detdesc
  Generated events are sent to the source address and source port of this
  command.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The event class ID provided in the payload must be supported by the module to
  which this command is issued. Otherwise the command fails. 
*/
#define VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS ( 0x00012E56 )

/* Type definition for  #vss_inotify_cmd_listen_for_event_class_t. */
typedef struct vss_inotify_cmd_listen_for_event_class_t vss_inotify_cmd_listen_for_event_class_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_inotify_cmd_listen_for_event_class_t
@{ */
/* Payload structure for the VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS command.
*/
struct vss_inotify_cmd_listen_for_event_class_t
{
   uint32_t class_id;
     /**< Event class ID to listen for. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_inotify_cmd_listen_for_event_class_t */


/** @ingroup cvd_reg_evt_notify_cancel
  Cancels a class of event notifications generated by a CVD module and that
  the client has been listening for.

  @apr_msgpayload{vss_inotify_cmd_cancel_event_class_t}
  @table{weak__vss__inotify__cmd__cancel__event__class__t}

  @detdesc
  Any event class IDs provided in the payload that are not supported by the
  module, or that the client was not listening for, are ignored.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS ( 0x00012E57 )

/* Type definition for vss_inotify_cmd_cancel_event_class_t. */
typedef struct vss_inotify_cmd_cancel_event_class_t vss_inotify_cmd_cancel_event_class_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_inotify_cmd_cancel_event_class_t
@{ */
/* Payload structure for the VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS command.
*/
struct vss_inotify_cmd_cancel_event_class_t
{
   uint32_t class_id;
     /**< Event class ID to cancel. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_inotify_cmd_cancel_event_class_t */


#endif /* __VSS_INOTIFY_PUBLIC_IF_H__ */

