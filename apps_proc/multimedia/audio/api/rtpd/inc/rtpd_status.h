#ifndef __RTPD_STATUS_H__
#define __RTPD_STATUS_H__
/**
    @file   rtpd_status.h
    @brief  
	Contains the return code for RTPD
*/
/*-----------------------------------------------------------------------------
     Copyright (c\C) 2014 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/api/rtpd/inc/rtpd_status.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/14    vk     Initial version.
===========================================================================*/

/** @addtogroup rtpd_status
  @{
*/

/** For internal use only. */
#define RTPD_ESTATUS_BEGIN                       ( 0x00012F41 )

/** Success. The operation completed, and there were no errors. */
#define RTPD_EOK                                 ( 0x0        )

/** General failure. */
#define RTPD_EFAILED                             ( 0x00012F41 )

/** Invalid operation parameters. */
#define RTPD_EBADPARAM                           ( 0x00012F42 )

/** Unsupported routine or operation. */
#define RTPD_EUNSUPPORTED                        ( 0x00012F43 )

/** Unsupported version. */
#define RTPD_EVERSION                            ( 0x00012F44 )

/** Unexpected problem was encountered. */
#define RTPD_EUNEXPECTED                         ( 0x00012F45 )

/** Unhandled problem occurred. */
#define RTPD_EPANIC                              ( 0x00012F46 )

/** Unable to allocate resources. */
#define RTPD_ENORESOURCE                         ( 0x00012F47 )

/** Invalid handle. */
#define RTPD_EHANDLE                             ( 0x00012F48 )

/** Operation is already processed. */
#define RTPD_EALREADY                            ( 0x00012F49 )

/** Operation is not ready to be processed. */
#define RTPD_ENOTREADY	                         ( 0x00012F4A )

/** Operation is pending completion. */
#define RTPD_EPENDING                            ( 0x00012F4B )

/** Operation cannot be accepted or processed. */
#define RTPD_EBUSY                               ( 0x00012F4C )

/** Operation aborted due to an error. */
#define RTPD_EABORTED                            ( 0x00012F4D )

/** Operation was preempted by a higher priority. */
#define RTPD_EPREEMPTED                          ( 0x00012F4E )

/** Operation requires intervention to complete. */
#define RTPD_ECONTINUE                           ( 0x00012F4F )

/** Operation requires immediate intervention to complete. */
#define RTPD_EIMMEDIATE	                         ( 0x00012F50 )

/** Operation is not implemented. */
#define RTPD_ENOTIMPL                            ( 0x00012F51 )

/** Operation requires more data or resources. */
#define RTPD_ENEEDMORE                           ( 0x00012F52 )

/** Operation is a local procedure call. */
#define RTPD_ELPC                                ( 0x00012F53 )

/** Operation timed out. */
#define RTPD_ETIMEOUT                            ( 0x00012F54 )

/** RTPD did not find the handle or an internal resource required to process the
  request. */
#define RTPD_ENOTFOUND                           ( 0x00012F55 )

/** Operation cannot proceed due to an improper state. */
#define RTPD_EBADSTATE                           ( 0x00012F56 )

/** Qualcomm aDSP return error status. */ 
#define RTPD_EQADSP                              ( 0x00012F57 )

/** Subsystem is in reset. */
#define RTPD_ESSRESET                            ( 0x00012F58 )

/** For internal use only. */ 
#define RTPD_ESTATUS_END                         ( 0x00012FA0 )

/** @} */ /* end_addtogroup rtpd_status */

#endif /* __RTPD_STATUS_H__ */

