
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
#pragma push
#pragma arm

typedef unsigned int atomic_plain_word_t;

typedef struct {
    volatile atomic_plain_word_t value;
} atomic_word_t;

static __inline void atomic_compiler_barrier(void)
{
    __memory_changed();
}


#define FORCE_INLINE static __forceinline

FORCE_INLINE atomic_plain_word_t
atomic_set(atomic_word_t *target, atomic_plain_word_t value)
{
    /* Use tmp1 and tmp2 to prevent load-delay on strex */
    atomic_plain_word_t tmp1, tmp2;
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    __asm {
        L1:    ldrex   tmp1, [ptr]
               strex   tmp2, value, [ptr]
               cmp     tmp2, #0
               bne     L1
    }

    atomic_compiler_barrier();
    return value;
}

/* Bitwise operations. */
FORCE_INLINE atomic_plain_word_t
atomic_and_return(atomic_word_t *target,
        atomic_plain_word_t mask)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;
    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               ands    result, result, mask
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();
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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;


    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               bics    result, result, mask
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();
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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   orig, [ptr]
               and     result, orig, mask
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();
    return orig;
}

FORCE_INLINE atomic_plain_word_t
atomic_or_return(atomic_word_t *target, atomic_plain_word_t mask)
{
    atomic_plain_word_t tmp;
    atomic_plain_word_t result;
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               orrs    result, result, mask
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
        }
    atomic_compiler_barrier();
	
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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               eors    result, result, mask
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();
	
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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target[word].value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   orig, [ptr]
               bics    result, orig, mask
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();

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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               adds    result, result, v
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();

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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               subs    result, result, v
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();

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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               adds    result, result, #1
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();

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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   result, [ptr]
               subs    result, result, #1
               strex   tmp, result, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();

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
    atomic_plain_word_t *ptr = (atomic_plain_word_t *)&target->value;

    atomic_compiler_barrier();
    __asm {
        L1:    ldrex   current_val, [ptr]
               mov     tmp, #0
               cmp     current_val, old_val
               strexeq tmp, new_val, [ptr]
               cmp     tmp, #0
               bne     L1
    }
    atomic_compiler_barrier();

    return current_val == old_val;
}

FORCE_INLINE void
atomic_barrier(void)
{
    atomic_plain_word_t reg = 0;
    __asm ("mcr     p15, 0, reg, c7, c10, 5");
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
#pragma pop

#endif

#endif /* _ARCH_ATOMIC_OPS_H__ */
