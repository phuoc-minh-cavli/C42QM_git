/*!
  @file msgr_nas.h

  @brief
   Defines the NAS modules using the message router

  Details...
  
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/09   MNK      Initial version
===========================================================================*/

#ifndef __MSGR_NAS_H__
#define __MSGR_NAS_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <msgr_umid.h>  /* UMID definitions */
#include "msgr_types.h"
#include "queue.h"
#include "IxErrno.h"
#include "rex.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/



#define MSGR_MODULE_EMM                 0x19
#define MSGR_MODULE_ESM                 0x1c
#define MSGR_MODULE_SM                  0x1d
#define MSGR_NAS_EMM           MSGR_TECH_MODULE( MSGR_TECH_NAS, MSGR_MODULE_EMM )
#define MSGR_NAS_ESM           MSGR_TECH_MODULE( MSGR_TECH_NAS, MSGR_MODULE_ESM )
#define MSGR_NAS_SM            MSGR_TECH_MODULE( MSGR_TECH_NAS, MSGR_MODULE_SM )

void emm_msgr_send
(
  /*! Pointer to message to be sent. Note that the first member of the message
   *  that is pointed to by this pointer must be of msgr_hdr_struct_type type
   */
  msgr_hdr_struct_type*      msg_ptr,
  /*! Total message length (header and payload) in bytes */
  uint32                     msg_len
);

void emm_msgr_send_error
(
  /*! Pointer to message to be sent. Note that the first member of the message
   *  that is pointed to by this pointer must be of msgr_hdr_struct_type type
   */
  msgr_hdr_struct_type*      msg_ptr,
  /*! Total message length (header and payload) in bytes */
  uint32                     msg_len
);

void lte_nas_msgr_register
(
  msgr_tech_module_type  mod,
  msgr_client_t *msgr_client_id, //!< Message Client Handle
  msgr_id_t  id, //!< Message Queue ID
  msgr_umid_type umid //!< UMID for which to register
);

void lte_nas_msgr_deregister
(
  msgr_tech_module_type  mod,           //!< TECH,MODULE Performing Operation
  msgr_client_t         *msgr_client_id, //!< Message Client Handle
  msgr_umid_type         umid            //!< UMID for which to register
);

void lte_nas_msgr_client_set_rex_q
(
  msgr_client_t      *msgr_client_id,   //!< Message Client Handle
  rex_tcb_type       *tcb,              //!< REX TCB pointer
  rex_sigs_type       sig,              //!< REX signal to set
  q_type             *msg_queue,        //!< queue to place the msg
  q_type             *free_msg_queue,   //!< queue to get an empty buffer
  uint16              msgr_hdr_offset,  //!< offset to the msgr_hdr
  uint16              cmd_type_offset,  //!< offset to the legacy cmd_type
  uint16              max_msg_size      //!< max size of msgs for the queue
);

void lte_nas_msgr_client_create
(
  msgr_client_t   *msgr_client_id
);


#endif /* __MSGR_NAS_H__ */
