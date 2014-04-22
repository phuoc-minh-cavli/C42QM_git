#ifndef __CSD_VSM_H__
#define __CSD_VSM_H__
/*-----------------------------------------------------------------------------
     Copyright (C) 2014 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/csd/utils/vsm/inc/csd_vsm.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

when		   who     what, where, why
--------	-----		---------------------------------------------------
04/16/14    jk     creation

=============================================================================*/

#include "mmdefs.h"
#include "csd_dev_ioctl.h"


/****************************************************************************
 * CSD VSM payload structures                                               *
 ****************************************************************************/

#define CSD_VSM_STREAM_NAME_MAX_SIZE 31

#define CSD_VSM_NONE_HANDLE 0

typedef enum {
  CSD_VSM_HANDLE_STATE_UNINITIALIZED = 0,
  CSD_VSM_HANDLE_STATE_OPEN,
  CSD_VSM_HANDLE_STATE_READY,
  CSD_VSM_HANDLE_STATE_MAX
} csd_vsm_handle_state_t;

typedef enum {
  CSD_VSM_SESSION_STATE_IDLE = 0,
  CSD_VSM_SESSION_STATE_RUNNING,
  CSD_VSM_SESSION_STATE_MAX
} csd_vsm_session_state_t;

typedef enum {
  CSD_VSM_VS_HANDLE = 0,
  CSD_VSM_VC_HANDLE,
  CSD_VSM_VM_HANDLE,
  CSD_VSM_ALL_HANDLE,
  CSD_VSM_HANDLE_MAX
} csd_vsm_handle_enum_t;

typedef struct {
 csd_vsm_handle_enum_t handle_type;
 bool_t                assoc_mode;
} csd_vsm_handle_t;

typedef struct csd_vsm_handle_list_t csd_vsm_handle_list_t;
struct csd_vsm_handle_list_t
{
  uint32_t* buffer;
    /**<
      The buffer to be filled. 
      @verbatim
     
      < example: if handle type is CSD_VSM_VS_HANDLE >
     
      <----- 32 bits ----->
       -------------------
      | csd vs handle     |   
       -------------------
      | csd vs handle     |   
       -------------------    
      |                   |   
      .                   .
      .                   .
     
      < example: if handle type is CSD_VSM_ALL_HANDLE >
     
       <----- 32 bits ----->
       -------------------
      | csd vs handle     |   
       -------------------
      | csd vc handle     |   
       -------------------
      | csd vm handle     |   
       -------------------  
      |                   |   
      .                   .
      .                   .
     
      @endverbatim
     */
  uint32_t buffer_size;
    /**< Size of the buffer in bytes. */
  uint32_t* ret_write_size;
    /**< Number of bytes written. */
  uint32_t* ret_total_size;
    /**< Number of bytes required. */
};

typedef struct csd_vsm_assoc_handle_list_t csd_vsm_assoc_handle_list_t;
struct csd_vsm_assoc_handle_list_t
{
  uint32_t handle;
  uint32_t* buffer;
    /**<
      The buffer to be filled. 
      @verbatim
     
      < example: if handle type is CSD_VSM_VC_HANDLE >
     
      <----- 32 bits ----->
       -------------------
      | csd vc handle     |   
       -------------------
      | csd vc handle     |   
       -------------------    
      |                   |   
      .                   .
      .                   .
      @endverbatim
     */
  uint32_t buffer_size;
    /**< Size of the buffer in bytes. */
  uint32_t* ret_write_size;
    /**< Number of bytes written. */
  uint32_t* ret_total_size;
    /**< Number of bytes required. */
};

typedef struct csd_vsm_session_handle_t csd_vsm_session_handle_t;
struct csd_vsm_session_handle_t
{
  uint32_t handle;
  csd_vsm_handle_state_t state;

};

typedef struct csd_vsm_session_t csd_vsm_session_t;
struct csd_vsm_session_t
{
  char_t name[CSD_VSM_STREAM_NAME_MAX_SIZE];
  csd_vsm_session_handle_t vs_handle;
  csd_vsm_session_handle_t vc_handle;
  csd_vsm_session_handle_t vm_handle;
  csd_vsm_session_state_t state;
};


/****************************************************************************
 * CSD VSM APIs                                                             *
 ****************************************************************************/

uint32_t csd_vsm_init( void );

uint32_t csd_vsm_deinit( void );

uint32_t csd_vsm_cleanup( void );

uint32_t csd_vsm_get_handles(  csd_vsm_handle_t handle_type, 
                               csd_vsm_session_state_t session_state, 
                               void *handle_list );

uint32_t csd_vsm_update_state( uint32_t handle, const char* name, csd_vsm_handle_state_t new_state);


#endif /* __CSD_VSM_H__*/


