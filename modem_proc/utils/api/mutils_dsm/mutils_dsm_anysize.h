/*!
  @file
  dsm_util_anysize.h

  @brief
  This lib provides the registered clients an utility to allocate an DSM item
  with any required size and pushdown required bytes into that DSM item.
  
  The utility provides 2 basic APIs for DSM allocation: an thread-safe API and
  none-thread safe API. 

  Before using this utility, an client shall register itself via registration 
  API with required information. 

  When allocating the DSM item, clients shall provide an DSM pool type that it 
  anticipate if the current DSM item is used up and required size shall not 
  exceed the size of the DSM item from that pool. 

  The pushdown API shall only happen on the DSM item allocated by this lib. 

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/utils.mpss/2.3.4/api/mutils_dsm/mutils_dsm_anysize.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/16   cx      Initial version
===========================================================================*/

#ifndef MUTILS_DSM_ANYSIZE_H
#define MUTILS_DSM_ANYSIZE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <dsm.h>
#include <mutils_dsm_client.h>
#include <dsm_lock.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*!
    @brief
    The function ptr used by clients to allocate anyszie DSM item
*/
typedef dsm_item_type* (*mutils_dsm_anysize_new_buf_func_ptr)\
  (mutils_dsm_client_e,\
    dsm_mempool_id_type,\
      uint32, const char*, uint32);

/*!
    @brief
    The lock type used by client during registration
*/
typedef enum
{
  MUTILS_DSM_ANYSIZE_REG_LOCKLESS = 0x00,
  MUTILS_DSM_ANYSIZE_REG_LOCK = 0x01,
  MUTILS_DSM_ANYSIZE_REG_MAX
}mutils_dsm_anysize_reg_lock_e;

/*!
    @brief
    The structue of client registration
*/
typedef struct
{ 
  /* clients shall not touch this field */
  uint32 cookie;
  /* the lock type that used by clients */
  mutils_dsm_anysize_reg_lock_e lock;
}mutils_dsm_anysize_reg_param_s;

/*!
    @brief
    the client structure
*/
typedef struct
{ 
  dsm_item_type * parent_ptr;
  dsm_lock_type   lock;
  mutils_dsm_anysize_new_buf_func_ptr new_buffer_func;
}mutils_dsm_anysize_clt_s;

/*!
    @brief
    the client structure
*/
typedef struct
{ 
  uint32 num_parent_alloc;
  uint32 num_dup_alloc;
  uint8  num_clt_reg_lock;
  uint8  num_clt_reg_lockless;
  uint8  num_clt_dereg;
}mutils_dsm_anysize_stat_s;

/*!
    @brief
    the global control block of anysize DSM modual
*/
typedef struct
{    
  mutils_dsm_anysize_clt_s clients[MUTILS_DSM_CLIENT_MAX];
  boolean is_active;
  mutils_dsm_anysize_stat_s stats;
}mutils_dsm_anysize_ctrl_blk_s;

/*===========================================================================

                    EXTERNAL VARIABLE

===========================================================================*/
/*!
    @brief
    the global control block of anysize DSM modual
*/
extern mutils_dsm_anysize_ctrl_blk_s mutils_dsm_anysize_ctrl_blk;


/*===========================================================================

                   EXTERNAL FUNCTION AND API

===========================================================================*/
/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_init()

==============================================================================*/
/*!
    @brief
    Initialize dsm anysize modual. All the clients will be initialized with thread-
    safe API

    @param
    None
    
    @return
    None
*/
/*============================================================================*/
void mutils_dsm_anysize_init();

/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_deinit()

==============================================================================*/
/*!
    @brief
    deinit dsm anysize modual 

    @param
    None
    
    @return
    None
*/
/*============================================================================*/
void mutils_dsm_anysize_deinit();

/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_new_buffer()

==============================================================================*/
/*!
    @brief
    Allocate a new DSM item with required size. It would call a thread-safe 
    API or a none-thread-safe API based on client's registration
    
    @param
    client_id: the client id who is going to allocate the anysize dsm item
    pool_id:   the dsm pool from which the new dsm item is allocated if the current 
               dsm item is used up
    size:      the size required by client
    
    @return
    Success: allocated dsm item
    Fail:    NULL

*/
/*============================================================================*/
#define mutils_dsm_anysize_new_buffer(mutils_dsm_client_e,pool_id,size)\
  mutils_dsm_anysize_ctrl_blk.clients[mutils_dsm_client_e].new_buffer_func\
    ((mutils_dsm_client_e),(pool_id),(size),dsm_file,__LINE__)

/*==============================================================================

  FUNCTION:  mutils_dsmi_util_anysize_pushdown_tail()

==============================================================================*/
/*!
    @brief
    A pushdown API used for anysize DSM item. The pushdown should happen only 
    on DSM item allocate by this lib.. This is not thread safe API
    
    @param
    dsm_ptr: the anysize dsm item to which the bytes are pushed
    buf:   the source from which the bytes are pushed
    size:  the number of bytes that need to be pushed into the dsm item 
    
    @return
    The number of bytes that is pushed into the anysize DSM item
*/
/*============================================================================*/
uint32 mutils_dsmi_anysize_pushdown_tail
(
  dsm_item_type *dsm_ptr,
  void *buf,
  uint32 required_size,
  const char * file,
  uint32 line
);
#define mutils_dsm_anysize_pushdown_tail(dsm_ptr,buf,size)\
  mutils_dsmi_anysize_pushdown_tail((dsm_ptr),(buf),(size),dsm_file,__LINE__)

/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_clt_init_param()

==============================================================================*/
/*!
    @brief
    Initialize dsm anysize modual. All the clients will be initialized with thread-
    safe API

    @param
    None
    
    @return
    None

    @dependency
    This API need to be called before mutils_dsm_anysize_clt_reg() is called
    
*/
/*============================================================================*/
void mutils_dsm_anysize_clt_init_param( mutils_dsm_anysize_reg_param_s *param);

/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_clt_reg()

==============================================================================*/
/*!
    @brief
    The API used for clients registration. The clients need to specify the 
    allocation is thread safe or none-thread safe

    @param
    client_id: the client who needs to allocate anysize DSM item
    param: the pointer points to the parameter structure
    
    @return
    None

    @dependency
    mutils_dsm_anysize_init_param() need to be called before this API is called
*/
/*============================================================================*/
void mutils_dsm_anysize_clt_reg
(
  mutils_dsm_client_e client_id,
  mutils_dsm_anysize_reg_param_s * param
);

/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_clt_dereg()

==============================================================================*/
/*!
    @brief
    DeReg API for the client.

    @param
    client_id: client who does the de-registration
    
    @return
    None

*/
/*============================================================================*/
void mutils_dsm_anysize_clt_dereg
(
  mutils_dsm_client_e client_id 
);

/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_qsh_analysis()

==============================================================================*/
/*!
    @brief
    runing qsh analysis.

    @param
    None
    
    @return
    None
*/
/*============================================================================*/
void mutils_dsm_anysize_qsh_analysis();

/*==============================================================================

  FUNCTION:  mutils_dsm_anysize_qsh_reset()

==============================================================================*/
/*!
    @brief
    running qsh reset.

    @param
    None
    
    @return
    None
*/
/*============================================================================*/
void mutils_dsm_anysize_qsh_reset();


#endif /* MUTIL_DSM_ANYSIZE_H */
