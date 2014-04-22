#ifndef _RTPD_RTS_IOCTL_H_
#define _RTPD_RTS_IOCTL_H_
/**
    @file      rtpd_rts_ioctl.h
    @brief  Real-Time Stream IOCTLs
*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2014 Qualcomm Technologies Incorporated.
      All rights reserved.
      Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/api/rtpd/inc/rtpd_rts_ioctl.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/14    vk     Initial version.
===========================================================================*/
#include "mmdefs.h"

/** @addtogroup rtpd_rts
  @{
*/

/*-------------------------------------------------------------------------*/
/* IOCTL Command */
/*-------------------------------------------------------------------------*/

/** Real-Time Stream IOCTL command start. */
#define RTPD_RTS_CMD_START                   0x00012FA1

/** Registers the callback function for the real-time stream session.

  @payload 
  #rtpd_rts_cb

  @latexonly \input{tables/RTPD_RTS_CMD_SET_EVT_CB.tex} @endlatexonly

  @return
  RTPD status code.

  @dependencies
  None.
*/
#define RTPD_RTS_CMD_SET_EVT_CB              0x00012FA1

/**  Allocate PMEM for the real-time stream session.
  @payload
  #rtpd_rts_pmem_info

  @latexonly \input{tables/RTPD_RTS_CMD_ALLOC_PMEM.tex} @endlatexonly

  @return
  RTPD status code.

  @dependencies
  None.
*/
#define RTPD_RTS_CMD_ALLOC_PMEM              0x00012FA2

/**  Free PMEM for the real-time stream session.
  @payload
  #rtpd_rts_pmem_info

  @latexonly \input{tables/RTPD_RTS_CMD_FREE_PMEM.tex} @endlatexonly

  @return
  RTPD status code.

  @dependencies
  None.
*/
#define RTPD_RTS_CMD_FREE_PMEM               0x00012FA3

/** Real-Time Stream IOCTL command end. */
#define RTPD_RTS_CMD_END                     0x00013000

/*-------------------------------------------------------------------------*/
/* Payload for IOCTL command */
/*-------------------------------------------------------------------------*/

/** Callback function type for real-time stream callback.

  @param[in] evnt_id     Event id.
  @param[in] data        Payload for the callback event.
  @param[in] data_len    Payload length.
  @param[in] client_data Client provided data returned in callback function.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*rtpd_rts_cb_fn)(
    uint32_t    evnt_id,
    void        *data,
    uint32_t    data_len,
    void        *client_data
    );

/**
  @brief defines the callback structure for RTPD_RTS_CMD_SET_EVT_CB
*/
struct rtpd_rts_cb
{
   rtpd_rts_cb_fn   cb;     	/**< Callback function provided by client. */
   void             *cb_data;  	/**< Client data returned during callback. */
};
/** @brief Type definition for rtpd_rts_cb_t. */
typedef struct rtpd_rts_cb rtpd_rts_cb_t;

/**
   @name Shared Memory Type
   @{
*/
#define RTPD_RTS_SMEM_EBI     0          /**< Shared memory for the external bus interface. */
#define RTPD_RTS_SMEM_SMI     1          /**< Shared memory for the system memory interface. */
/** @} */ /* end_name Shared Memory Type */

/**.
   @name PMEM Allocation Flag Type
   @{
*/
#define RTPD_RTS_PMEM_FLAGS_PHYS_CONTIG       0x00000001
#define RTPD_RTS_PMEM_FLAGS_PHYS_NON_CONTIG   0x00000010
/** @} */ /* end_name PMEM Allocation Flag Type */

/**
  @brief Payload of #RTPD_RTS_CMD_ALLOC_PMEM/#RTPD_RTS_CMD_FREE_PMEM IOCTLs.
*/
struct rtpd_rts_pmem_info
{
   uint32_t size;       /**< Size of the PMEM buffer to be allocated or the
                             allocated size. */
   uint8_t *va;         /**< Pointer to the virtual address for the allocated
                             PMEM in the client process. */
};
/* Type definition for #rtpd_rts_pmem_info_t. */
typedef struct rtpd_rts_pmem_info rtpd_rts_pmem_info_t;

/*-------------------------------------------------------------------------*/
/*  Event IDs */
/*-------------------------------------------------------------------------*/

/**	Start of the real-time stream event ID. */
#define RTPD_RTS_EVT_START             0x00013001

/** This event communicates information about the real-time stream to the
    client.

    @payload
    #rtpd_rts_evt_rt_proxy_status

    @latexonly \input{tables/RTPD_RTS_EVT_RT_PROXY_STATUS.tex} @endlatexonly

    @return
    None.

    @dependencies
    None.
*/
#define RTPD_RTS_EVT_RT_PROXY_STATUS           (RTPD_RTS_EVT_START)

/** End of the real-time stream event ID. */
#define RTPD_RTS_EVT_END                    0x00013050

/*-------------------------------------------------------------------------*/
/*	Payload for event ids */
/*-------------------------------------------------------------------------*/

/**
   @name Real-Time Stream Event Type
   @{
*/
/** Type of event sent when a real-time proxy is started. */
#define RTPD_RTS_EVT_TYPE_RT_PROXY_START               0

/** Type of event sent when a real-time proxy is stopped. */
#define RTPD_RTS_EVT_TYPE_RT_PROXY_STOP                1

/** Type of event sent when a low watermark is reached. */
#define RTPD_RTS_EVT_TYPE_RT_PROXY_LOW_WATER_MARK      2

/**  Type of event sent when a high watermark is reached. */
#define RTPD_RTS_EVT_TYPE_RT_PROXY_HIGH_WATER_MARK     3

/** Invalid event. */
#define RTPD_RTS_EVT_TYPE_RT_PROXY_INVALID             0xFFFF
/** @} */ /* end_name Real-Time Stream Event Type  */

/**
  @brief Payload of #RTPD_RTS_EVT_RT_PROXY_STATUS.
*/
struct rtpd_rts_evt_rt_proxy_status
{
  uint32_t  dev_id;        /**< Device id. */
  uint16_t  event_type;    /**< Type of the event. Supported values:\n
                               - RTPD_RTS_EVT_TYPE_RT_PROXY_START               0
                               - RTPD_RTS_EVT_TYPE_RT_PROXY_STOP                1
                               - RTPD_RTS_EVT_TYPE_RT_PROXY_LOW_WATER_MARK      2
                               - RTPD_RTS_EVT_TYPE_RT_PROXY_HIGH_WATER_MARK     3
                               - RTPD_RTS_EVT_TYPE_RT_PROXY_INVALID             0xFFFF */
};
/* Type definition for #rtpd_rts_evt_rt_proxy_status_t. */
typedef struct rtpd_rts_evt_rt_proxy_status rtpd_rts_evt_rt_proxy_status_t;

/** @} */ /* end_addtogroup rtpd_rts */

#endif /* _RTPD_RTS_IOCTL_H_*/
