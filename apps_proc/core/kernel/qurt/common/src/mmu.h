/*
 * Australian Public Licence B (OZPLB)
 *
 * Version 1-0
 *
 * Copyright (c) 2006-2010, Open Kernel Labs, Inc.
 *
 * All rights reserved.
 *
 * Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
 *               National ICT Australia
 *               http://www.ertos.nicta.com.au
 *
 * Permission is granted by Open Kernel Labs, Inc., free of charge, to
 * any person obtaining a copy of this software and any associated
 * documentation files (the "Software") to deal with the Software without
 * restriction, including (without limitation) the rights to use, copy,
 * modify, adapt, merge, publish, distribute, communicate to the public,
 * sublicense, and/or sell, lend or rent out copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimers.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimers in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of Open Kernel Labs, Inc., nor the names of its
 *       contributors, may be used to endorse or promote products derived
 *       from this Software without specific prior written permission.
 *
 * EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
 * PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
 * NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
 * WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
 * REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
 * THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
 * ERRORS, WHETHER OR NOT DISCOVERABLE.
 *
 * TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
 * NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
 * THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
 * LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
 * OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
 * OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
 * OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
 * CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
 * CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
 * DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
 * CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
 * DAMAGES OR OTHER LIABILITY.
 *
 * If applicable legislation implies representations, warranties, or
 * conditions, or imposes obligations or liability on Open Kernel Labs, Inc.
 * or one of its contributors in respect of the Software that
 * cannot be wholly or partly excluded, restricted or modified, the
 * liability of Open Kernel Labs, Inc. or the contributor is limited, to
 * the full extent permitted by the applicable legislation, at its
 * option, to:
 * a.  in the case of goods, any one or more of the following:
 * i.  the replacement of the goods or the supply of equivalent goods;
 * ii.  the repair of the goods;
 * iii. the payment of the cost of replacing the goods or of acquiring
 *  equivalent goods;
 * iv.  the payment of the cost of having the goods repaired; or
 * b.  in the case of services:
 * i.  the supplying of the services again; or
 * ii.  the payment of the cost of having the services supplied again.
 *
 * The construction, validity and performance of this licence is governed
 * by the laws in force in New South Wales, Australia.
 */

#if defined(ASSEMBLY)

#define ASM_TLB_FLUSH_ALL(reg) \
    mov reg, #0; \
    mcr p15, 0, reg, c8, c7, 0;

#else

static inline void
tlb_flush(void)
{
    /* Set the control register */
    write_cp15_register(C15_tlb, c7, 0, 0UL);
}

static inline void
tlb_flush_asid(asid_t asid)
{
    word_t val;
    l4_ensure(asid < HAL_NUM_ASIDS);

    val = (word_t)asid;
    write_cp15_register(C15_tlb, c7, 2, val);
}

static inline void
tlb_flush_addr_asid(word_t addr, asid_t asid)
{
    word_t val;
    l4_ensure(asid < HAL_NUM_ASIDS);

    val = (word_t)asid;
    val |= (addr & 0xfffffc00UL);

    write_cp15_register(C15_tlb, c7, 1, val);
}

static inline void
write_arm_ttbase0(word_t val)
{
    /* Set the translation table base to use the kspace_phys */
    write_cp15_register(C15_ttbase, C15_CRm_default, 0,
                        val | TTBASE_FLAGS);
}

static inline void
write_arm_ttbase1(word_t val)
{
    write_cp15_register(C15_ttbase, C15_CRm_default, 1,
                        val | TTBASE_FLAGS);
}

#endif /* !defined(ASSEMBLY) */
