/*	$NetBSD: rtld_start.S,v 1.8 2004/08/21 11:12:03 rearnsha Exp $	*/

/*-
 * Copyright (c) 1998, 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matt Thomas and by Charles M. Hannum.
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

/*==============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

	.align	0
	.globl	_rtld_bind_start
	.type	_rtld_bind_start,%function
/*
 *	r15 has object pointer
 *	r14 has PLT index
 */
_rtld_bind_start:
    allocframe(#104);
    memd(sp + # 0) = r17:16;
    memd(sp + # 8) = r19:18;
    memd(sp + #16) = r21:20;
    memd(sp + #24) = r23:22;
    memd(sp + #32) = r25:24;
    memd(sp + #40) = r27:26;
    memd(sp + #48) = r1:0;
    memd(sp + #56) = r3:2;
    memd(sp + #64) = r5:4;
    memd(sp + #72) = r7:6;
    memd(sp + #80) = r9:8;
    memd(sp + #88) = r11:10;
    memd(sp + #96) = r13:12;

    {
        r0 = r15;
        r1 = r14;
        call _rtld_bind@PLT;
    }

    r15 = r0;

    r17:16 = memd(sp + # 0);
    r19:18 = memd(sp + # 8);
    r21:20 = memd(sp + #16);
    r23:22 = memd(sp + #24);
    r25:24 = memd(sp + #32);
    r27:26 = memd(sp + #40);
    r1:0 = memd(sp + #48);
    r3:2 = memd(sp + #56);
    r5:4 = memd(sp + #64);
    r7:6 = memd(sp + #72);
    r9:8 = memd(sp + #80);
    r11:10 = memd(sp + #88);
    r13:12 = memd(sp + #96);

    deallocframe;
    jumpr r15;

    .size _rtld_bind_start, . - _rtld_bind_start

