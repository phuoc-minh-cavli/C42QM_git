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

#ifndef __ARCH_ATOMIC_OPS_H__
#define __ARCH_ATOMIC_OPS_H__

/*
 * RVCT likes to mess around with our inline asm and makes bad
 * optimisation decisions (e.g. skipping comparisons).
 * For this reason we add atomic compiler barriers after any
 * asm blocks that modify atomic words. -jsok
 */

#if defined(__ATOMIC_OPS_IN_KERNEL__) && !defined(MACHINE_SMP)

/* Ensure that the kernel is non-smp. */
#if CONFIG_NUM_UNITS > 1
#error CONFIG_NUM_UNITS > 1 but smp not defined in machines.py.
#endif

/* The kernel does not require atomic ops on non-smp machines. */

#include <atomic_ops/unsafe_generic.h>

#else

/*
 * RVCT2.2 does not like mixing thumb and arm code directly so we need to
 * declare that all these functions are arm with the pragma.  GCC appears to
 * just do this automagically without needing to be explicitly told.  It
 * appears that this breaks the inlining as bx instructions are needed to
 * switch modes.
 */
#if (defined(__RVCT__) && !defined(__RVCT_4__)) || defined(__RVCT_GNU__)
#pragma push
#pragma arm
#endif

#if defined(__RVCT__)
#define FORCE_INLINE static __forceinline
#endif

FORCE_INLINE atomic_plain_word_t
atomic_set(atomic_word_t *target, atomic_plain_word_t value)
{
    /* Use tmp1 and tmp2 to prevent load-delay on strex */
    atomic_plain_word_t tmp1, tmp2;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        tmp1 = __ldrex(ptr);
        tmp2 = __strex(value, ptr);
    } while (tmp2 != 0);
#else
    __asm (
        "L1:    ldrex   tmp1, [ptr]"
        "       strex   tmp2, value, [ptr]"
        "       cmp     tmp2, #0"
        "       bne     L1"
    );

    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       strex   %1, %3, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (tmp1), "=&r" (tmp2)
        : "r" (ptr), "r" (value)
        : "cc"
    );
#endif
    return value;
}

/* Bitwise operations. */
FORCE_INLINE atomic_plain_word_t
atomic_and_return(atomic_word_t *target,
        atomic_plain_word_t mask)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result = result & mask;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       ands    result, result, mask"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       ands    %0, %0, %3\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "Ir" (mask)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_and(atomic_word_t *target, atomic_plain_word_t mask)
{
    (void)atomic_and_return(target, mask);
}

static atomic_plain_word_t
atomic_nand_return(atomic_word_t *target,
        atomic_plain_word_t mask);
static void
atomic_nand(atomic_word_t *target, atomic_plain_word_t mask);

FORCE_INLINE atomic_plain_word_t
atomic_nand_return(atomic_word_t *target,
        atomic_plain_word_t mask)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result = result & ~mask;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       bics    result, result, mask"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       bics    %0, %0, %3\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "Ir" (mask)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_nand(atomic_word_t *target, atomic_plain_word_t mask)
{
    (void)atomic_nand_return(target, mask);
}

FORCE_INLINE atomic_plain_word_t
atomic_and_return_old(atomic_word_t *target,
        atomic_plain_word_t mask)
{
    atomic_plain_word_t tmp, orig;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        orig = __ldrex(ptr);
        result = orig & mask;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   orig, [ptr]"
        "       and     result, orig, mask"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %2, [%3]\n"
        "       and     %0, %2, %4\n"
        "       strex   %1, %0, [%3]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp), "=&r" (orig)
        : "r" (ptr), "Ir" (mask)
        : "cc"
    );
#endif

    return orig;
}

FORCE_INLINE atomic_plain_word_t
atomic_or_return(atomic_word_t *target, atomic_plain_word_t mask)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result = result | mask;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       orrs    result, result, mask"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
        );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       orrs    %0, %0, %3\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "Ir" (mask)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_or(atomic_word_t *target, atomic_plain_word_t mask)
{
    (void)atomic_or_return(target, mask);
}

FORCE_INLINE atomic_plain_word_t
atomic_xor_return(atomic_word_t *target,
        atomic_plain_word_t mask)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result = result ^ mask;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       eors    result, result, mask"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       eors    %0, %0, %3\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "Ir" (mask)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_xor(atomic_word_t *target, atomic_plain_word_t mask)
{
    (void)atomic_xor_return(target, mask);
}

/* Bitfield operations. */

FORCE_INLINE void
atomic_set_bit(atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(atomic_word_t) * 8);
    atomic_plain_word_t mask =
        1 << (bit % (sizeof(atomic_word_t) * 8));

    (void)atomic_or_return(&target[word], mask);
}

FORCE_INLINE void
atomic_clear_bit(atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(atomic_word_t) * 8);
    atomic_plain_word_t mask =
        1 << (bit % (sizeof(atomic_word_t) * 8));

    (void)atomic_nand_return(&target[word], mask);
}

FORCE_INLINE int
atomic_clear_bit_return_old(atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(atomic_word_t) * 8);
    atomic_plain_word_t mask =
        1 << (bit % (sizeof(atomic_word_t) * 8));
    atomic_plain_word_t tmp, orig;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target[word].value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        orig = __ldrex(ptr);
        result = orig & ~mask;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   orig, [ptr]"
        "       bics    result, orig, mask"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %2, [%3]\n"
        "       bics    %0, %2, %4\n"
        "       strex   %1, %0, [%3]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp), "=&r" (orig)
        : "r" (ptr), "Ir" (mask)
        : "cc"
    );
#endif

    return (orig & mask) != 0;
}

FORCE_INLINE void
atomic_change_bit(atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(atomic_word_t) * 8);
    atomic_plain_word_t mask =
        1 << (bit % (sizeof(atomic_word_t) * 8));

    (void)atomic_xor_return(&target[word], mask);
}


/* Arithmetic operations. */
FORCE_INLINE atomic_plain_word_t
atomic_add_return(atomic_word_t *target, atomic_plain_word_t v)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result = result + v;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       adds    result, result, v"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       adds    %0, %0, %3\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "Ir" (v)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_add(atomic_word_t *target, atomic_plain_word_t v)
{
    (void)atomic_add_return(target, v);
}

FORCE_INLINE atomic_plain_word_t
atomic_sub_return(atomic_word_t *target, atomic_plain_word_t v)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result = result - v;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       subs    result, result, v"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       subs    %0, %0, %3\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "Ir" (v)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_sub(atomic_word_t *target, atomic_plain_word_t v)
{
    (void)atomic_sub_return(target, v);
}

FORCE_INLINE atomic_plain_word_t
atomic_inc_return(atomic_word_t *target)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result++;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       adds    result, result, #1"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       adds    %0, %0, #1\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_inc(atomic_word_t *target)
{
    (void)atomic_inc_return(target);
}

FORCE_INLINE atomic_plain_word_t
atomic_dec_return(atomic_word_t *target)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        result = __ldrex(ptr);
        result--;
        tmp = __strex(result, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   result, [ptr]"
        "       subs    result, result, #1"
        "       strex   tmp, result, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %0, [%2]\n"
        "       subs    %0, %0, #1\n"
        "       strex   %1, %0, [%2]\n"
        "       cmp     %1, #0\n"
        "       bne     1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr)
        : "cc"
    );
#endif

    return result;
}

FORCE_INLINE void
atomic_dec(atomic_word_t *target)
{
    (void)atomic_dec_return(target);
}

/* General operations. */

FORCE_INLINE int
atomic_compare_and_set(atomic_word_t *target,
        atomic_plain_word_t old_val, atomic_plain_word_t new_val)
{
    atomic_plain_word_t current_val, tmp;
    word_t *ptr = (word_t *)&target->value;

#if defined(__RVCT__) || defined(__RVCT_GNU__)
#if defined(__RVCT_4__)
    do {
        current_val = __ldrex(ptr);
        if (current_val != old_val) {
            break;
        }
        tmp = __strex(new_val, ptr);
    } while (tmp != 0);
#else
    atomic_compiler_barrier();
    __asm (
        "L1:    ldrex   current_val, [ptr]"
        "       mov     tmp, #0"
        "       cmp     current_val, old_val"
        "       strexeq tmp, new_val, [ptr]"
        "       cmp     tmp, #0"
        "       bne     L1"
    );
    atomic_compiler_barrier();
#endif
#else
    __asm__ __volatile__ (
        "1:     ldrex   %1, [%2]\n"
        "       mov     %0, #0\n"
        "       cmp     %1, %3\n"
        "       strexeq %0, %4, [%2]\n"
        "       cmp     %0, #0\n"
        "       bne     1b\n"
        : "=&r" (tmp), "=&r" (current_val)
        : "r" (ptr), "Ir" (old_val), "r" (new_val)
        : "cc"
    );
#endif

    return current_val == old_val;
}

FORCE_INLINE void
atomic_barrier(void)
{
#if defined(__RVCT__) || defined(__RVCT_GNU__)
    word_t reg = 0;
    __asm ("mcr     p15, 0, reg, c7, c10, 5");
#else
    __asm__ __volatile__ (
        "   mcr     p15, 0, %0, c7, c10, 5  \n"
        :
        : "r" (0)
        : "memory"
    );
#endif
}

FORCE_INLINE void
atomic_barrier_write(void)
{
    atomic_barrier();
}

FORCE_INLINE void
atomic_barrier_write_smp(void)
{
    atomic_barrier();
}

FORCE_INLINE void
atomic_barrier_read(void)
{
    atomic_barrier();
}

FORCE_INLINE void
atomic_barrier_read_smp(void)
{
    atomic_barrier();
}

FORCE_INLINE void
atomic_barrier_smp(void)
{
    atomic_barrier();
}

/* And now we switch back to original compile mode. */
#if defined(__RVCT__) || defined(__RVCT_GNU__)
#pragma pop
#endif

#endif

#endif /* _ARCH_ATOMIC_OPS_H__ */
