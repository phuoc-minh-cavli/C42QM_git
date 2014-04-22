#ifndef __RTPD_RTS_H__
#define __RTPD_RTS_H__

/* ! \file rtpd_rts.h
    \brief Contains Real-Time Stream's Definitions.
*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2014 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/api/rtpd/inc/rtpd_rts.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/14    vk     Initial version.
===========================================================================*/
#include "mmdefs.h"
#include "rtpd_rts_ioctl.h"

/** @addtogroup rtpd_rts
  @{
*/

/*-------------------------------------------------------------------------*/
/* Open structures for real-time stream open */
/*-------------------------------------------------------------------------*/

/**
   @name Real-Time Stream Opcode
   @{
*/
#define RTPD_RTS_OPEN_OP_R     0x01     /**< Open for read. */
#define RTPD_RTS_OPEN_OP_W     0x02     /**< Open for write. */
/** @} */ /* end_name Real-Time Stream Opcode */

/** 
    @name Real-Time Stream Memory Type
    @{
*/ 
#define RTPD_RTS_BUF_MEM_UNKNOWN     0x00    /**< Invalid memory type. */
#define RTPD_RTS_BUF_MEM_SHARED      0x01    /**< Client uses shared memory. */
/** @} */ /* end_name Real-Time Stream Memory Type */

/** 
    @name Real-Time Stream Call Mode Type
    @{ 
*/ 
#define RTPD_RTS_DATA_MODE_SYNC      0        /**< Synchronous data call. */
/** @} */ /* end_name Real-Time Stream Call Mode Type */

/**
  @brief Opens a Real-Time Stream session.
*/
struct rtpd_rts_open
{
  uint32_t    dev_id;       /**< Device ID  */
  uint32_t    op_code;      /**< Operation code for the real-time stream.
                                 Supported values:\n
                                 - 0x1 -- Open for read.
                                 - 0x2 -- Open for write. */
  uint32_t    buf_mem_type; /**< Memory type for the data buffer. Supported values:
                                 - 0x00 -- Invalid memory type.
                                 - 0x01 -- Client uses shared memory.*/
  uint32_t    data_mode;    /**< Defines the behavior of real-time stream data path APIs.
                                 Currently only supports synchronous calls:\n
                                  - 0   -- Synchronous.
                                  - > 0 -- Reserved. */
  uint32_t    ret_status;   /**< Returns the open status. */
};
/* Type definition for #rtpd_rts_open_t. */
typedef struct rtpd_rts_open rtpd_rts_open_t;

/*-------------------------------------------------------------------------*/
/*  Buffer structure for rtpd_read() and rtpd_write() */
/*-------------------------------------------------------------------------*/
/**
    @name Real-Time Stream Data Buffer
    @{
*/
/**
  @brief Structure accepted by rtpd_write() and rtpd_read().
*/
struct rtpd_rts_buffer{
    uint8_t   *buf_addr;     /**< Pointer to the buffer start. */
    uint32_t  buf_size;      /**< Buffer size in bytes. */
};
/* Type definition for #rtpd_rts_buffer_t. */
typedef struct rtpd_rts_buffer rtpd_rts_buffer_t;


/** @} */  /* end_addtogroup rtpd_rts */

#endif /* __RTPD_RTS_H__ */
