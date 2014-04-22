#ifndef _CSD_Q6ATL_H_
#define _CSD_Q6ATL_H_
/*============================================================================

   FILE:       csd_q6atl.h

   DESCRIPTION: abstract transport layer interface file.

   PUBLIC CLASSES:  Not Applicable

   INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 - 2012 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ----------------------------------------------------------
04/27/10   mingchen initial draft 
============================================================================*/


#include "mmdefs.h"


typedef int32_t  (*csd_q6atl_session_listener_type)
	             (void* buf, uint32_t buf_len, void *client_data); /**< async session call back */

/*! \fn int32_t csd_atl_init(void)
    \brief This function initializes the csd_atl. Only allow to initialize once. No ref count implemented
    \return int32_t
*/
int32_t csd_atl_init(void);

/*! \fn int32_t csd_atl_dinit(void)
    \brief This function de-initializes the csd_atl. Only allow to de-initialize once. No ref count implemented
    \return int32_t
*/
int32_t csd_atl_dinit(void);

/*! \fn int32_t csd_atl_async_reg(void)
    \brief This function register a per csd_atl session call back for deliving async msg to csd_atl's user
    \return int32_t
*/
int32_t csd_atl_async_reg
(
    uint16_t src_addr,                    /**< src addr */
	uint16_t src_port,                    /**< src port */
    csd_q6atl_session_listener_type cb,   /**< msg notifying cb */
    void* cb_data                         /**< msg cb user data */
);

/*! \fn int32_t csd_atl_async_dereg(void)
    \brief This function de-register the per csd_atl session call back
    \return int32_t
*/
int32_t csd_atl_async_dereg
(
    uint16_t src_addr,                    /**< src addr */
	uint16_t src_port                     /**< src port */
);

/*! \fn int32_t csd_atl_send(void)
    \brief This function send a Q6 msg to Q6 in sync manner or async manner
    \return int32_t
*/
int32_t csd_atl_send
(
    void* send_buf,        /* sending buf ptr */
    void* rsp_buf,         /* rsp buf ptr for sync send only */
	uint32_t* rsp_buf_len, /* rsp buf length in bytes */
	bool_t is_sync         /* 1 = sync send, 0 = async send */
);

/** @brief  This funciton generate a client transaction number
 *  @return ositive number
 *   */
uint32_t csd_q6atl_gen_apr_token( void );

#endif /* _CSD_Q6ATL_H_ */
