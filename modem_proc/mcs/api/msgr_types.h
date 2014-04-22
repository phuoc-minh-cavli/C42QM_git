/*!
  @file   msgr_types.h

  @brief  Message Router header file

  @detail
  Defines Message Router related types and interfaces.

*/

/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

$Header: //components/rel/mcs.mpss/5.1/api/msgr_types.h#4 $

===========================================================================*/

#ifndef MSGR_TYPES_H
#define MSGR_TYPES_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>    /* Definition of basic types and macros */
#include <IxErrno.h>
#include "msgr_umid.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef int                     msgr_fifo_handle_t;
typedef int                     msgr_client_t;
#define MSGR_MQ_NO_Q            (mqd_t)-1
#define MSGR_MQ_MAX_NAME_SIZE   16

/*! @brief MSGR ID is used to identify the mq being that is being referenced */
typedef int                     msgr_id_t;
#define MSGR_ID_NONE            (msgr_id_t)0
#define MSGR_ID_REX             MSGR_ID_NONE

/*! @brief "NO OFFSET" is used to indicate that the offset to the REX
    cmd_type in the msg data structure does not need to be set by MSGR */
#define MSGR_NO_OFFSET          (uint16)-1

/*! @brief "NO QUEUE" is passed to msgr_client_set/add_rex_q in the free_msg_queue
    param to indicate that MSGR should allocate message buffers from the heap
    rather than a specified free queue */
#define MSGR_NO_QUEUE           (q_type *)-1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef uint8   msgr_proc_t;

typedef struct {
  msgr_proc_t       proc;
  const char *      name;
  uint32            unused1;
  uint32            unused2;
} msgr_init_entry_s;

/* Legacy type used to maintain the external SMD api */
typedef int msgr_init_smd_entry_s;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef uint8            msgr_priority_t;
typedef msgr_priority_t  msgr_priority_type;

#define MSGR_PRIORITY_LOW   0x00
#define MSGR_PRIORITY_HIGH  0x7F

/* Default variant for messages */
#define MSGR_NO_VARIANT 0

/* Variant field is 3 bits wide so the allowed variants are 0-7 */
#define MSGR_MAX_VARIANT 7

/* Number of variants supported in this build */
#ifdef FEATURE_MSGR_VARIANTS
#define MSGR_NUM_VARIANT 8
#else
#define MSGR_NUM_VARIANT 1
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Type for message header.
*/
#ifdef __GNUC__
typedef struct ALIGN(8)
#else
typedef struct
#endif // __GNUC__
{
  msgr_umid_type              id;           /*!< Unique Message ID (UMID). 4 bytes. */
  msgr_tech_module_type       src;          /*!< Sending module. 2 bytes. */
  msgr_proc_t                 proc;         /*!< Sending proc. 1 byte. */
  uint8                       num_attach;   /*!< Number of attachments. 1 byte. */
  msgr_priority_type          priority;     /*!< Priority of the message. 1 byte. */
  uint8                       inst_id;      /*!< Generic unsigned user data field.
                                                 Appropriate conversion to be
                                                 performed by user to pass data
                                                 like sys_modem_as_id_e_type
                                                 inside this field. 1 byte. */
  uint8                       options_mask; /*!< Bit field to set optional params. 1 byte. */
  uint8                       padding;      /*!< Header is 8 byte aligned. Add explicit
                                                 padding to make that alignment clear. 1 byte. */
  uint32                      timestamp;    /*!< Timestamp (in XO resolution) to track
                                                 when UMID is sent. 4 bytes. */
} msgr_hdr_s;
typedef msgr_hdr_s  msgr_hdr_struct_type;

/*! @brief Optional params in the header */
typedef enum {
  NO_OPT = 0x0,
  NO_SIG  = 0x1    // Do not signal after send
} msgr_hdr_opts_e;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The following supervisory messages should be defined by every component using
 * the MSGR_UMID macro.
 * e.g. MSGR_UMID( MSGR_UMB_SRCH, MSGR_TYPE_SPR, MSGR_ID_LOOPBACK )
 */

/*! @brief Supervisory Loopback Message */
#define MSGR_ID_LOOPBACK           0x00

/*! @brief Supervisory Loopback Reply */
#define MSGR_ID_LOOPBACK_REPLY     0x01

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Type for supervisory loopback command.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type   hdr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Message payload */

  /*! Reply message header.  Used internally to reply to the loopback.  The
   *  payload in this message overlays exactly with the loopback reply below.
   *  As such, it is used as the reply message.
   */
  msgr_hdr_struct_type   reply_hdr;
  /*! Token to be included in reply */
  unsigned int           reply_token;

} msgr_spr_loopback_s;
typedef msgr_spr_loopback_s  msgr_spr_loopback_struct_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Type for loopback reply.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type   hdr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*! Message payload - Reply token received in loopback command */
  unsigned int           reply_token;

} msgr_spr_loopback_reply_s;
typedef msgr_spr_loopback_reply_s  msgr_spr_loopback_reply_struct_type;

/*! @brief Type for callback for msgr_send notifications
 */
typedef void (*msgr_send_cb_t)(msgr_tech_module_type,
                               msgr_tech_module_type,
                               msgr_hdr_struct_type*);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* MSGR_TYPES_H */
