/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.


==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/inc/jtypes_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _JTYPES_INT_H
#define _JTYPES_INT_H
#include "jtypes.h"
#include <jconfig.h>

#ifndef NULL
#define NULL    ((void *) 0)
#endif

#ifndef MIN
# define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
# define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#ifndef ABS
# define ABS(a)  ((a) < 0 ? (-(a)) : (a))
#endif

#ifndef TRUE
# define TRUE    1
#endif

#ifndef FALSE
# define FALSE   0
#endif

#ifndef CONFIG_BYTE_ORDER
#error "CONFIG_BYTE_ORDER for this configuration is not specified !"
#endif 

/*
 * Host to big endian, host to little endian, big endian to host, and little
 * endian to host byte order functions as detailed in byteorder(9).
 */
#ifndef _SYS_BSDTYPES_H_
#if CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN

#define htobe16(x)      bswap16((x))
#define htobe32(x)      bswap32((x))
#define htole16(x)      ((juint16_t)(x))
#define htole32(x)      ((juint32_t)(x))

#define be16toh(x)      bswap16((x))
#define be32toh(x)      bswap32((x))
#define be64toh(x)      bswap64((x))
#define le16toh(x)      ((juint16_t)(x))
#define le32toh(x)      ((juint32_t)(x))

/* casting from 1 byte pointer in an alignment critical system */
#ifdef CONFIG_ALIGN_CRITICAL

/*
 * The following macros READ from and WRITE to unaligned buffers
 * WITHOUT affecting endianity
 *
 * ua_getXX - read a value from an unaligned buffer (endianity unaffected)
 * ua_setXX - write a value to an unaligned buffer (endianity unaffected)
 *
 * NOTE: Both CPU endianity and BUFFER endianity are IRRELEVANT.
 */
#define ua_get16(ptr)   ua_le16toh(ptr)
#define ua_get32(ptr)   ua_le32toh(ptr)
#define ua_set16(ptr,v) ua_htole16(ptr,v)
#define ua_set32(ptr,v) ua_htole32(ptr,v)

#endif

#elif CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN 

#define htobe16(x)      ((juint16_t)(x))
#define htobe32(x)      ((juint32_t)(x))
#define htole16(x)      ((juint16_t)((((x) >> 8) & 0xff) | ((x) & 0xff) << 8))
#define htole32(x)      bswap32((x))
#define htole64(x)      bswap64((x))

#define be16toh(x)      ((juint16_t)(x))
#define be32toh(x)      ((juint32_t)(x))
#define le16toh(x)      bswap16((x))
#define le32toh(x)      bswap32((x))
#define le64toh(x)      bswap64((x))

/* casting from 1 byte pointer in an alignment critical system */
#ifdef CONFIG_ALIGN_CRITICAL

/*
 * The following macros READ from and WRITE to unaligned buffers
 * WITHOUT affecting endianity
 *
 * ua_getXX - read a value from an unaligned buffer (endianity unaffected)
 * ua_setXX - write a value to an unaligned buffer (endianity unaffected)
 *
 * NOTE: Both CPU endianity and BUFFER endianity are IRRELEVANT.
 */
#define ua_get16(ptr)   ua_be16toh(ptr)
#define ua_get32(ptr)   ua_be32toh(ptr)
#define ua_set16(ptr,v) ua_htobe16(ptr,v)
#define ua_set32(ptr,v) ua_htobe32(ptr,v)

#endif

#else
#error "Invalid CONFIG_BYTE_ORDER specified !"
#endif

#endif 

#ifndef CONFIG_ALIGN_CRITICAL

#define ua_get16(ptr)     (*(juint16_t *)(ptr))
#define ua_set16(ptr,v)   (*(juint16_t *)(ptr) = (v))
#define ua_get32(ptr)     (*(juint32_t *)(ptr))
#define ua_set32(ptr,v)   (*(juint32_t *)(ptr) = (v))

#endif

#endif

