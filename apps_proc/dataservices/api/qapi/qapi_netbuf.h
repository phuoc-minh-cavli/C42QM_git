/* Copyright (c) 2016-2021 Qualcomm Technologies, Inc. and/or its subsidiaries.
   All rights reserved.
   Confidential - Qualcomm Technologies, Inc. - May Contain Trade Secrets
*/

/* Copyright (c) 2015 Qualcomm Atheros, Inc.
   All rights reserved.
   Qualcomm Atheros Confidential and Proprietary.
*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY. 
*/

/**
 * @file qapi_netbuf.h
 *
 * @addtogroup qapi_networking_netbuf
 * @{
 *
 * @details This file describes QAPIs for Network Buffer allocation and deallocation.
 * When an application wants to send data over a communication socket, it must first allocate
 * a network buffer. There are two types of network buffers:
 *
 * - @b Local (@b application) @b buffers: These buffers are allocated from the heap using the
 * #QAPI_NETBUF_APP flag. These buffers are owned and maintained by the application, and the
 * stack copies the data from it (on Tx) and to it (on Rx). The application can reuse the 
 * buffer once it was sent or once done processing the incoming data. 
 *
 * - @b System @b buffers: These buffers are allocated from the internal stack memory using the
 * #QAPI_NETBUF_SYS flag. Using this option enables the Zero-Copy mechanism, which may improve
 * performance and reduce power due to skipping the memory copy from the application memory
 * to the stack memory. \n 
 * 
 * System buffers are shared between the application and the stack. The following
 * restrictions apply: \n
 * - The application does not own the buffer once it was sent, and must allocate a new one 
 * to transmit more data. \n
 * - Upon receiving a system buffer, the application must free it once done processing.
 * Note that a system buffer can be a list of chained buffers. 
 *
 *
 * @code
 *
 * qapi_Net_Buf_t *my_buf;
 *
 * do {
 *   // Allocate a buffer of 1200 bytes
 *   qapi_Net_Buf_t my_buf = qapi_Net_Buf_Alloc(1200, QAPI_NETBUF_SYS);
 *
 *   if(!my_buf) {
 *   	// No memory, wait and retry
 *   	sleep(1);
 *   	continue;
 *   }
 * }
 * while(0);
 * @endcode
 *
 * @}
 */

#ifndef _QAPI_NETBUF_H_
#define _QAPI_NETBUF_H_

#include "stdint.h"

#include "qapi_data_txm_base.h"

/* Net QAPI REQUEST ID DEFINES */

#define  TXM_QAPI_NET_BUF_ALLOC                         TXM_QAPI_NET_BASE + 15
#define  TXM_QAPI_NET_BUF_FREE                          TXM_QAPI_NET_BASE + 16
#define  TXM_QAPI_NET_BUF_UPDATE                        TXM_QAPI_NET_BASE + 17

/** @addtogroup qapi_networking_netbuf
@{ */

/**
 * Structure to hold network buffer information.
 *
 * @details This structure is used when allocating a system buffer.
 * The Network buffer can be chained, and accessed via pk_prev and pk_next.
 */
typedef struct qapi_Net_Buf_s
{
   void *reserved;         /**< Reserved. */
   struct qapi_netbuf_s *pk_Prev;    /**< Previous buffer in chain. */
   struct qapi_netbuf_s *pk_Next;    /**< Next buffer in chain. */
   char     *nb_Buff;      /**< Beginning of raw buffer. */
   uint32_t nb_Blen;       /**< Length of raw buffer. */
   char     *nb_Prot;      /**< Beginning of protocol/data. */
   uint32_t nb_Tlen;       /**< Total length of all nb_prots in pk_next list. */
   uint32_t nb_Plen;       /**< Length of protocol/data. */
} qapi_Net_Buf_t;

/**
 * MAC+IP+UDP = 24+20+8 = 52
 */
#define QAPI_NETBUF_UDP_HEADROOM    (52)

/**
 * MAC+IP+TCP = 24+20+20 = 64
 */
#define QAPI_NETBUF_TCP_HEADROOM    (64)

/**
 * 24+40+8 = 72
 */
#define QAPI_NETBUF_UDP6_HEADROOM   (72)

/**
 * 24+40+24(TCP with option) = 88
 */
#define QAPI_NETBUF_TCP6_HEADROOM   (88)

/* For a non-chained netbuf, use the following macros to update the netbuf. \n */

/**
 * Updates the data start pointer on a system buffer.
 */
#define QAPI_NET_BUF_UPDATE_START(pkt, p) \
	(p) = ((qapi_Net_Buf_t *)(pkt))->nb_Prot

/**
 * Updates 4 bytes of data in the buffer.
 */
#define QAPI_NET_BUF_UPDATE_INT32(p, val) \
	*(uint32_t *)(p) = (uint32_t)(val); (p) += sizeof(uint32_t)

/**
 * Updates 2 bytes of data in the buffer.
 */
#define QAPI_NET_BUF_UPDATE_INT16(p, val) \
	*(uint16_t *)(p) = (uint16_t)(val); (p) += sizeof(uint16_t)

/**
 * Updates 1 byte of data in the buffer.
 */
#define QAPI_NET_BUF_UPDATE_INT8(p, val) \
	*(uint8_t *)(p) = (uint8_t)(val); (p) += sizeof(uint8_t)

/**
 * Copies application data into a non-chained buffer.
 */
#define QAPI_NET_BUF_UPDATE_DATA(p, data, len) \
	memcpy((p), (data), (len)); (p) += (len)

/**
 * Updates buffer size on a system buffer.
 */
#define QAPI_NET_BUF_UPDATE_END(pkt, len) \
	((qapi_Net_Buf_t *)(pkt))->nb_Plen = \
    ((qapi_Net_Buf_t *)(pkt))->nb_Tlen = (len)

/** Definitions used for ID in calls to qapi_Net_Buf_Alloc() and qapi_Net_Buf_Free(). \n */

/**
 * Allocates a network buffer from the application heap. \n
 * If the application does not enable the Zero-Copy option for a socket,
 * it should allocate memory using #QAPI_NETBUF_APP for sending packets.
 * Memory will be allocated from the heap.
 */
#define QAPI_NETBUF_APP     0x0

/**
 * Allocates a network buffer from the system stack. \n
 * If the application enables the Zero-Copy option for a socket,
 * it must allocate memory using #QAPI_NETBUF_SYS for sending packets.
 * Memory will be allocated from the packet buffer pool.
 */
#define QAPI_NETBUF_SYS     0x1

/**
 * Allocates a network buffer for a secure connection. \n
 * If the application enables the Zero-Copy option for a socket and uses a secure
 * connection over it, the applicaiton must allocate memory using one of these two flag combinations: \n
 * - (QAPI_NETBUF_APP | QAPI_NETBUF_SSL)
 * - (QAPI_NETBUF_SYS | QAPI_NETBUF_SSL)
 */
#define QAPI_NETBUF_SSL     0x2

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_Net_Buf_Alloc(a,b)                              ((void *) (_txm_module_system_call12)(TXM_QAPI_NET_BUF_ALLOC                     , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_Buf_Free(a,b)                               ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_BUF_FREE                        , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_Buf_Update(a,b,c,d,e)                       ((UINT) (_txm_module_system_call12)(TXM_QAPI_NET_BUF_UPDATE                      , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#else

/**
 * Allocates a network buffer.
 *
 * @details Call this function to allocate a network buffer.
 *
 * @param [in] size  The size, in bytes, of the buffer.
 * @param [in] id    See #QAPI_NETBUF_APP, #QAPI_NETBUF_SYS, and #QAPI_NETBUF_SSL.
 *
 * @return       A pointer to the network buffer, or NULL in case no memory is available.
 *               This is usually not a fatal error, the application has to sleep and retry.
 *
 */
void * qapi_Net_Buf_Alloc(uint32_t size, uint32_t id);

/**
 * Frees a network buffer.
 *
 * @details Call this function to free a network buffer.
 *
 * @param [in] buf  A pointer to the buffer to be freed.
 * @param [in] id   See #QAPI_NETBUF_APP, #QAPI_NETBUF_SYS, and #QAPI_NETBUF_SSL.
 *                  The same flag that was used to allocate it must be applied.
 *
 * @return       0 if operation succeeded, -1 otherwise.
 *
 */
int32_t qapi_Net_Buf_Free(void *buf, uint32_t id);

/**
 * Updates data in a network buffer.
 *
 * @details Call this function to seamlessly update data in a network buffer, even in chained ones.
 *
 * @param [in] netbuf  A pointer to the buffer to be updated.
 * @param [in] offset  Data offset.
 * @param [in] srcbuf  A pointer to the data source.
 * @param [in] len     Data length.
 * @param [in] id      See #QAPI_NETBUF_APP, #QAPI_NETBUF_SYS, and #QAPI_NETBUF_SSL.
 *
 * @return
 * 0 on success, -1 otherwise.
 */
int32_t qapi_Net_Buf_Update(void *netbuf, uint32_t offset, void *srcbuf, uint32_t len, uint32_t id);

/** @} */
#endif

#endif /* _QAPI_NETBUF_H_ */
