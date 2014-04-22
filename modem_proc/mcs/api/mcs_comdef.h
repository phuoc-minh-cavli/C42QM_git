/*!
  @file
  mcs_comdef.h

  @brief
  Common defines for MCS which are shared between SW and FW.

*/

/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //components/rel/mcs.mpss/5.1/api/mcs_comdef.h#5 $

===========================================================================*/

#ifndef MCS_COMDEF_H
#define MCS_COMDEF_H

#ifndef __ASSEMBLER__
#include <comdef.h>
#include <amssassert.h>
#endif


/*! @brief Static (compile-time) assertion.
    The compiler will generate a "negative array index" error if the assertion
    fails.
    Only use this for constants known at compile time.
    e.g.
    STATIC_ASSERT(sizeof(mystruct) == 12);
*/
#undef STATIC_ASSERT
#define STATIC_ASSERT(expr) \
  /*lint -save -e18 -e19 -e506 -e1066 */ \
  extern char MACRO_CONCAT(MACRO_CONCAT(N,__COUNTER__),MACRO_CONCAT(_THIS_ERROR_MEANS_A_STATIC_ASSERT_FAILURE_AT_LINE_,__LINE__))[ (expr) ? 1 : -1 ] __attribute__((unused)); \
  /*lint -restore */

#define COMPILE_ASSERT(expr)    STATIC_ASSERT(expr)



/*! @brief ASSERT_ARGS macro
    Allows passing arguments to be saved when ASSERT fails.
    They will not be output explicitly because there are no format specifiers
    for them, but they can be extracted from fatal error parameters.
    int err = foo();
    ASSERT(err == 0, err);
    If fails, prints "Assertion (err == 0) failed", "err" arg is saved.
*/
/*lint -emacro((506),ASSERT_ARGS) */
#ifndef ASSERT_ARGS
#define ASSERT_ARGS( expr, ... ) \
  if ( EXPECT( !(expr), 0 ) ) \
  { \
    _ERR_FATAL_V("Assertion (" #expr ") failed: args [0x%08x,0x%08x,0x%08x]", ## __VA_ARGS__,0,0,0); \
  }
#define _ERR_FATAL_V(fmt,a,b,c,...) ERR_FATAL(fmt,a,b,c)
#endif /* #ifndef ASSERT_ARGS */




/*! @brief Lint-only assertion.
    This macro creates an assertion only for the lint pass, and is never
    included in compiled code. It is useful for providing information
    that lint can't infer on its own.
    e.g.
    LINT_ASSERT(x < 10);
    my_array[x] = 0;  // Generates an "out of range array access" otherwise
*/
#ifdef _lint
#define LINT_ASSERT(expr) if (!(expr)) { _lint_assert_fail(); }
/*lint -function(exit,_lint_assert_fail) */
extern void _lint_assert_fail(void);
#else
#define LINT_ASSERT(expr)
#endif



/*! @brief Compiler hint about the value of expression. expr should be
    the full expression, and hint a true/false value. For example:
    e.g.
    if (EXPECT(ptr == NULL, FALSE))
      fatal_error();
*/
#ifdef _lint
#define EXPECT(expr, hint) ( expr )
#else
#define EXPECT(expr, hint)  __builtin_expect( (expr), (hint) )
#endif


/*! @brief Get the offset of "field" within structure/union "type".
    e.g.
    typedef struct {
      int32 a;
      int32 b;
    } my_struct;
    OFFSETOF(my_struct, b)  returns 4.
*/
#ifndef _lint
#define OFFSETOF(type, field) __builtin_offsetof(type, field)
#else
/*lint -emacro((413,545),OFFSETOF) */
#define OFFSETOF(type, field) \
  ( (unsigned int) &( ((type *) 0)->field ) )
#endif

/*! @brief Translate an address to a different address space (for example,
    from virtual to physical space).
    "addr" is relative to "old_base", in the same address space.
    The offset from "old_base" to "addr" is added to "new_base", effectively
    translating into the new address space.
*/
#define ADDR_TRANSLATE(addr, old_base, new_base) ( ((uint32) (addr)) - ((uint32) (old_base)) + ((uint32) (new_base)) )


/*! @brief Bit at position "n", n < 32 . */
#define BIT(n)   (1U << (uint32)(n))

/*! @brief Bit at position "n", n < 64 .*/
#define BITL(n)  (1ULL << (uint32)(n))

/*! @brief Bitmask containing "n" bits, 0 <= n <= 32 */
#define BITMASK(n) ( /*lint -save -e506 */ \
                     (n) >= 32 ? MAX_UINT32 : BIT((n) & 0x1F) - 1 \
                     /*lint -restore */ )

/*! @brief Bitmask containing "n" bits, 0 <= n <= 64 */
#define BITMASKL(n) ( /*lint -save -e506 */ \
                      (n) >= 64 ? MAX_UINT64 : BITL((n) & 0x3F) - 1 \
                      /*lint -restore */ )


/*! @brief Quantize "n" to the next-lowest multiple of "q" .
    Result is <= n, (result % q) == 0.
    Defined for positive inputs only.
    Use for constants only, not efficient for runtime (unless q is power of 2).
*/
#define QUANTIZE_DOWN(n,q) ( ( (n) / (q) ) * (q) )


/*! @brief Quantize "n" to the next-highest multiple of "q".
    Result is >= n, (result % q) == 0.
    Defined for positive inputs only.
    Use for constants only, not efficient for runtime (unless q is power of 2).
*/
#define QUANTIZE_UP(n,q) ( QUANTIZE_DOWN( ( (n) + (q) - 1 ), (q) ) )


/*! @brief Return 1 if x is a power of 2, 0 otherwise.
    Efficient to be used at runtime.
*/
#define IS_POW2(x)       ( ( (x) & ( (x) - 1 ) ) == 0 )


/*! @brief Return the highest power-of-2 <= x.
    x must be unsigned 32-bits.
    Use for constants only, not efficient for runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    uint32 FLOOR_POW2(uint32 x) {
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x | (x >> 16);
       return x - (x >> 1);
    }
*/
#define FLOOR_POW2(x) ( /*lint -save -e572 */ \
                        FLOOR_POW2_1_(x) - (FLOOR_POW2_1_(x)>>1) \
                        /*lint -restore */ )
#define FLOOR_POW2_1_(x) (FLOOR_POW2_2_(x)|(FLOOR_POW2_2_(x)>>16))
#define FLOOR_POW2_2_(x) (FLOOR_POW2_3_(x)|(FLOOR_POW2_3_(x)>>8))
#define FLOOR_POW2_3_(x) (FLOOR_POW2_4_(x)|(FLOOR_POW2_4_(x)>>4))
#define FLOOR_POW2_4_(x) (FLOOR_POW2_5_(x)|(FLOOR_POW2_5_(x)>>2))
#define FLOOR_POW2_5_(x) ((x)|((x)>>1))


/*! @brief Return the lowest power-of-2 >= x.
    x must be unsigned 32-bits.
    Use for constants only, not efficient for runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    uint32 CEIL_POW2(uint32 x) {
       x = x - 1;
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x | (x >> 16);
       return x + 1;
    }
*/
#define CEIL_POW2(x) ( /*lint -save -e572 */ \
                       CEIL_POW2_1_(x) + 1  \
                       /*lint -restore */ )
#define CEIL_POW2_1_(x) (CEIL_POW2_2_(x)|(CEIL_POW2_2_(x)>>16))
#define CEIL_POW2_2_(x) (CEIL_POW2_3_(x)|(CEIL_POW2_3_(x)>>8))
#define CEIL_POW2_3_(x) (CEIL_POW2_4_(x)|(CEIL_POW2_4_(x)>>4))
#define CEIL_POW2_4_(x) (CEIL_POW2_5_(x)|(CEIL_POW2_5_(x)>>2))
#define CEIL_POW2_5_(x) (CEIL_POW2_6_(x)|(CEIL_POW2_6_(x)>>1))
#define CEIL_POW2_6_(x) ((x)-1)


/*! @brief Population count, i.e. number of bits set in a word.
    x can be up to 64 bits.
    Result is from 0 (for input=0) to 64 (for input=0xFFFFFFFFFFFFFFFFuLL).
    Use for constants only, not efficient at runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    int POPCOUNT(uint64 x) {
       x = (x & 0x5555555555555555uLL) + ((x >> 1) & 0x5555555555555555uLL);
       x = (x & 0x3333333333333333uLL) + ((x >> 2) & 0x3333333333333333uLL);
       x = (x & 0x0F0F0F0F0F0F0F0FuLL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FuLL);
       x = (x & 0x00FF00FF00FF00FFuLL) + ((x >> 8) & 0x00FF00FF00FF00FFuLL);
       x = (x & 0x0000FFFF0000FFFFuLL) + ((x >> 16) & 0x0000FFFF0000FFFFuLL);
       x = (x & 0x00000000FFFFFFFFuLL) + ((x >> 32) & 0x00000000FFFFFFFFuLL);
     }
*/
#define POPCOUNT(x) ( /*lint -save -e572 */ \
                      (POPCOUNT_1_((uint64) (x))&0x00000000FFFFFFFFuLL)+\
                      ((POPCOUNT_1_((uint64) (x))>>32)&0x00000000FFFFFFFFuLL)\
                      /*lint -restore */ )
#define POPCOUNT_1_(x) ((POPCOUNT_2_(x)&0x0000FFFF0000FFFFuLL)+\
                        ((POPCOUNT_2_(x)>>16)&0x0000FFFF0000FFFFuLL))
#define POPCOUNT_2_(x) ((POPCOUNT_3_(x)&0x00FF00FF00FF00FFuLL)+\
                        ((POPCOUNT_3_(x)>>8)&0x00FF00FF00FF00FFuLL))
#define POPCOUNT_3_(x) ((POPCOUNT_4_(x)&0x0F0F0F0F0F0F0F0FuLL)+\
                        ((POPCOUNT_4_(x)>>4)&0x0F0F0F0F0F0F0F0FuLL))
#define POPCOUNT_4_(x) ((POPCOUNT_5_(x)&0x3333333333333333uLL)+\
                        ((POPCOUNT_5_(x)>>2)&0x3333333333333333uLL))
#define POPCOUNT_5_(x) (((x)&0x5555555555555555uLL)+\
                        (((x)>>1)&0x5555555555555555uLL))


/*! @brief Count leading zeros in "x".
    x must be unsigned 32-bits.
    Result is from 0 (for input=0xFFFFFFFF) to 32 (for input=0).
    Use for constants only, not efficient at runtime.
    Algorithn (from "Hacker's Delight", Henry S. Warren Jr.):
    int CLZ(uint32 x) {
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x | (x >> 16);
       // return POPCOUNT(~x) changed to avoid "integer overflow" warning
       return 32 - POPCOUNT(x);
    }
*/
#define CLZ(x) ( /*lint -save -e572 */ \
                 32 - POPCOUNT(CLZ_1_(x))  \
                 /*lint -restore */ )
#define CLZ_1_(x) (CLZ_2_(x)|(CLZ_2_(x)>>16))
#define CLZ_2_(x) (CLZ_3_(x)|(CLZ_3_(x)>>8))
#define CLZ_3_(x) (CLZ_4_(x)|(CLZ_4_(x)>>4))
#define CLZ_4_(x) (CLZ_5_(x)|(CLZ_5_(x)>>2))
#define CLZ_5_(x) ((x)|((x)>>1))


/*! @brief Calculate the largest n such that 2^n <= x.
    x must be unsigned 32-bits, x >= 1.
    Use for constants only, not efficient at runtime.
*/
#define FLOOR_LOG2(x) ( 31 - (int32)CLZ(x) )


/*! @brief Calculate the smallest n such that 2^n >= x.
    x must be unsigned 32-bits, x >= 1.
    Use for constants only, not efficient at runtime.
*/
#define CEIL_LOG2(x) ( FLOOR_LOG2( (x) - 1 ) + 1 )


/*! @brief Attribute to declare inline functions.
    Redefine this to force always inline, which is required if functions
    are declared in header files.
*/
#ifdef _lint
#define STATIC_INLINE       inline
#else
#define STATIC_INLINE       static inline __attribute__((always_inline))
#endif
#undef INLINE
#define INLINE              STATIC_INLINE


/*! @brief Attribute to force a function to not be auto-inlined.
    e.g. NOINLINE uint32 my_func(void) { ... }
*/
#define NOINLINE            __attribute__((noinline))


/*! @brief Restrict pointer aliasing. When applied to a pointer type, tells
    the compiler that pointer references a unique location not accessible
    through other means. This is typically useful for buffer processing where
    the read/write pointers are distinct (without RESTRICT, the compiler must
    assume that the write for element [k] must be done before the read for [k+1]).
    RESTRICT is only relevant for pointers that will be used for writes.
    e.g.
    void myfunc(short int *a, short int *b, short int * RESTRICT c) {
       for (i=0; i < 100; ++i) {
          *c++ = *a++ - *b++;
       }
    }
    Be very careful that this is applied accurately, or the compiler may
    generate incorrect code.
*/
#define RESTRICT            __restrict

/*! @brief Mark a function as "static" in normal builds, but export
    in FWTest unit tests.
*/
#ifdef FWTEST_PLATFORM
  #define FWTEST_STATIC
#else
  #define FWTEST_STATIC     static
#endif


/*! @brief Attribute to suppress compiler warnings for unused variables.
    This is used for declarations; for suppressing within the body of a function,
    see NOTUSED().
    e.g.
    void foo(int arg UNUSED) { ... }
    int x UNUSED;
*/
#define UNUSED              __attribute__((unused))


/*! @brief Attribute to mark a function, variable, or typedef as deprecated.
    Use of the symbol will generate a compiler warning.
    Note that the tag must be applied to the external declaration in order
    to have an effect.
    e.g. extern void func(int) DEPRECATED;
         extern int x DEPRECATED;
         typedef int my_type DEPRECATED;
*/
#define DEPRECATED          __attribute__((deprecated))

/*! @brief A nop used in assembly for explicit .falign padding to avoid assembler
    warnings. */
#define FALIGN_NOP          nop


/*! @brief Attribute to force a variable or function into a particular section.
    Name must be a quoted string.
    e.g. void my_func(void) SECTION(".tcm_text");
         int my_var SECTION(".data") = 0;
    Separate SECTION_CODE and SECTION_DATA are necessary for assembly to set
    attributes correctly.
*/
#define SECTION(name)       __attribute__((section(name)))
#ifndef __ASSEMBLER__
#define SECTION_CODE(name)  SECTION(name)
#define SECTION_DATA(name)  SECTION(name)
#else
#define SECTION_CODE(name)  .section name, "ax"
#define SECTION_DATA(name)  .section name, "aw"
#endif


/*! @brief Count the arguments in a variadic macro.
    e.g. MACRO_COUNT_ARGS(a,b,c) expands to 3.\,
         MACRO_COUNT_ARGS() expands to 0.
    Supports up to 99 arguments.
    Original idea from Laurent Deniau, comp.std.c */
#define MACRO_COUNT_ARGS(...) _MACRO_COUNT_ARGS_M1(dummy, ## __VA_ARGS__)
#define _MACRO_COUNT_ARGS_M1(...) _MACRO_ARG_100(__VA_ARGS__, \
   99,98,97,96,95,94,93,92,91,90, \
   89,88,87,86,85,84,83,82,81,80, \
   79,78,77,76,75,74,73,72,71,70, \
   69,68,67,66,65,64,63,62,61,60, \
   59,58,57,56,55,54,53,52,51,50, \
   49,48,47,46,45,44,43,42,41,40, \
   39,38,37,36,35,34,33,32,31,30, \
   29,28,27,26,25,24,23,22,21,20, \
   19,18,17,16,15,14,13,12,11,10, \
    9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _MACRO_ARG_100( \
  a00,a01,a02,a03,a04,a05,a06,a07,a08,a09, \
  a10,a11,a12,a13,a14,a15,a16,a17,a18,a19, \
  a20,a21,a22,a23,a24,a25,a26,a27,a28,a29, \
  a30,a31,a32,a33,a34,a35,a36,a37,a38,a39, \
  a40,a41,a42,a43,a44,a45,a46,a47,a48,a49, \
  a50,a51,a52,a53,a54,a55,a56,a57,a58,a59, \
  a60,a61,a62,a63,a64,a65,a66,a67,a68,a69, \
  a70,a71,a72,a73,a74,a75,a76,a77,a78,a79, \
  a80,a81,a82,a83,a84,a85,a86,a87,a88,a89, \
  a90,a91,a92,a93,a94,a95,a96,a97,a98,a99, \
  a100, ...) a100


/*! @brief Concatenate two preprocessor tokens.
    Does expansion on arguments.
    e.g. MACRO_CONCAT(a,b) expands to ab.
         MACRO_CONCAT(a,MACRO_CONCAT(b,c)) expands to abc */
#define MACRO_CONCAT(a,b) _MACRO_CONCAT(a,b)
#define _MACRO_CONCAT(a,b) a ## b

/*! @brief Convert macro argument (with expansion) to string. */
#define MACRO_STRINGIFY(x) _MACRO_STRINGIFY(x)
#define _MACRO_STRINGIFY(x) # x

/*! @brief Remove parentheses around macro arguments.
    Call this without parentheses.
    e.g.
    #define FOO   (1,2,3)
    MACRO_REMOVE_PAREN FOO   -- NOT MACRO_REMOVE_PAREN(FOO)
    expands to 1,2,3 */
#define MACRO_REMOVE_PAREN(...) __VA_ARGS__


/*! @brief Expand the given function/macro+argument for all input arguments.
    e.g. MACRO_APPLY(MY_MACRO,a,  b,c,d) expands to:
         MY_MACRO(a,b)
         MY_MACRO(a,c)
         MY_MACRO(a,d)

         Multiple arguments can be passed via tuples:
         MACRO_APPLY(MY_MACRO, (a,b), (1,2), (3,4), (5,6)) expands to:
         MY_MACRO( (a,b), (1,2) )
         MY_MACRO( (a,b), (3,4) )
         MY_MACRO( (a,b), (5,6) )

         Lists up to 99 elements are supported.
 */
#define MACRO_APPLY(_func,_arg, ...) \
  MACRO_CONCAT(_MACRO_APPLY_,MACRO_COUNT_ARGS(__VA_ARGS__))(_func,_arg,__VA_ARGS__)
/* No, there isn't a simpler way to do to this. Preprocessor doesn't support recursion. */
#define _MACRO_APPLY_1(_func,_arg,_x,...) _func(_arg,_x)
#define _MACRO_APPLY_2(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_1(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_3(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_2(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_4(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_3(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_5(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_4(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_6(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_5(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_7(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_6(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_8(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_7(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_9(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_8(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_10(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_9(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_11(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_10(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_12(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_11(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_13(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_12(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_14(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_13(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_15(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_14(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_16(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_15(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_17(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_16(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_18(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_17(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_19(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_18(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_20(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_19(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_21(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_20(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_22(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_21(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_23(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_22(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_24(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_23(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_25(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_24(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_26(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_25(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_27(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_26(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_28(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_27(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_29(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_28(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_30(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_29(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_31(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_30(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_32(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_31(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_33(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_32(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_34(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_33(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_35(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_34(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_36(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_35(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_37(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_36(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_38(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_37(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_39(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_38(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_40(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_39(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_41(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_40(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_42(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_41(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_43(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_42(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_44(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_43(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_45(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_44(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_46(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_45(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_47(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_46(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_48(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_47(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_49(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_48(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_50(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_49(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_51(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_50(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_52(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_51(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_53(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_52(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_54(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_53(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_55(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_54(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_56(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_55(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_57(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_56(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_58(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_57(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_59(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_58(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_60(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_59(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_61(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_60(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_62(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_61(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_63(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_62(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_64(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_63(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_65(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_64(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_66(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_65(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_67(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_66(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_68(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_67(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_69(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_68(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_70(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_69(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_71(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_70(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_72(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_71(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_73(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_72(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_74(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_73(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_75(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_74(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_76(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_75(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_77(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_76(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_78(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_77(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_79(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_78(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_80(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_79(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_81(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_80(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_82(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_81(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_83(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_82(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_84(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_83(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_85(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_84(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_86(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_85(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_87(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_86(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_88(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_87(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_89(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_88(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_90(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_89(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_91(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_90(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_92(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_91(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_93(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_92(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_94(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_93(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_95(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_94(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_96(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_95(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_97(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_96(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_98(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_97(_func,_arg,__VA_ARGS__)
#define _MACRO_APPLY_99(_func,_arg,_x,...) _func(_arg,_x) _MACRO_APPLY_98(_func,_arg,__VA_ARGS__)


/*! @brief Nominal XO clock rate (19.2 MHz).
*/
#define XO_NOM_RATE_HZ      (19200000)

#if defined(__HEXAGON_ARCH__)

/*! @brief Size of a cache line, in bytes.
    There may be different cache line sizes at different levels of memory
    hierarchy, so use the appropriate definition:
    ISA_CACHE_LINE_SZ: Cache line size used by the ISA instructions.
       This should be used when the actual cache line size is important, e.g.
       when issuing a dczero instruction.
       However, note that the HW cache line sizes may be larger, and this may
       cause false sharing.
       Guaranteed to be smaller or equal to HW cache line sizes.
    L1_DCACHE_LINE_SZ: Line size of L1 data cache.
       Use for cache manipulation involving L1 cache, such as dcfetch and dcclean.
       This guarantees that every L1 line is updated.
       However, note that L2 line size may be larger, so more data may be cleaned
       than intended, and false sharing can occur.
    L1_ICACHE_LINE_SZ: Line size of L1 instruction cache.
    L2_CACHE_LINE_SZ: Line size of L2 cache.

    In general, using L2_CACHE_LINE_SZ (or L2_CACHE_ALIGN) when declaring
    memory regions is the safest policy to guarantee no sharing occurs.
    When issuing cache instructions, using ISA_CACHE_LINE_SZ is safest to ensure
    every cache line is updated, but may have some redundancy if L1 line size
    is larger.
*/
#define ISA_CACHE_LINE_SZ   32

#if __HEXAGON_ARCH__ < 61
#define L2_CACHE_LINE_SZ    32
#elif __HEXAGON_ARCH__ >= 61
#define L2_CACHE_LINE_SZ    64
#else
#error "Unsupported Hexagon version"
#endif

#if (__HEXAGON_ARCH__ <= 61)
#define L1_DCACHE_LINE_SZ   32
#define L1_ICACHE_LINE_SZ   32
#elif  __HEXAGON_ARCH__ == 62
#define L1_DCACHE_LINE_SZ   32
#define L1_ICACHE_LINE_SZ   64
#elif (__HEXAGON_ARCH__ >= 65) && (__HEXAGON_ARCH__ <= 67)
#define L1_DCACHE_LINE_SZ   64
#define L1_ICACHE_LINE_SZ   64
#else
#error "Unsupported Hexagon version"
#endif

/*! @brief Directive to cache-align a variable or struct (use like "ALIGN").
*/
#define ISA_CACHE_ALIGN     ALIGN(ISA_CACHE_LINE_SZ)
#define L1_DCACHE_ALIGN     ALIGN(L1_DCACHE_LINE_SZ)
#define L1_ICACHE_ALIGN     ALIGN(L1_ICACHE_LINE_SZ)
#define L2_CACHE_ALIGN      ALIGN(L2_CACHE_LINE_SZ)

/*! @brief Flags for whether L1 caches are tagged by virtual or physical address
 */
#if (__HEXAGON_ARCH__ <= 61)
/* I$ and D$ are physically tagged */
#elif (__HEXAGON_ARCH__ >= 62) && (__HEXAGON_ARCH__ <= 67)
#define L1_ICACHE_TAG_VIRT
#else
#error "Unsupported Hexagon version"
#endif
#endif

/* If included from a FW standalone build, the following are already defined. */
#if !defined(__ASSEMBLER__)

extern uint64 mcs_get_utimer(void);

/*! @brief Wait for at least the specified number of XO counts (19.2MHz).
*/
INLINE void mcs_wait_xo(uint32 interval_xo)
{
  uint64 start_time = mcs_get_utimer();
  while ((mcs_get_utimer() - start_time) <= (uint64) interval_xo) ;
}

/*! @brief Wait for at least the specified number of microseconds.
*/
INLINE void mcs_wait_usec(uint32 interval_usec)
{
  mcs_wait_xo( (interval_usec * 96 + 4) / 5 ); /* 19.2 = 96/5, round up */
}

#if defined(__HEXAGON_ARCH__)

#if !defined(MODEM_FW_H)
/* Duplicates of modem_fw.h definitions */

#include <hexagon_protos.h>


#define MAX_UINT64 0xFFFFFFFFFFFFFFFFuLL

/*! @brief Wrap the input x to between [0,mod-1].
    x must be in the range [-mod,2*mod-1].
    If you are sure only one wraparound direction is possible (e.g. just
    after adding a positive value), use mod_wrap_high() or
    mod_wrap_low().

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
INLINE int32 mod_wrap(int32 x, int32 mod) {
  return Q6_R_modwrap_RR(x,mod);
}

/*! @brief Wrap the input x to be between [0,mod-1].
    x must be in the range [0,2*mod-1].
    Negative wraparound is not checked.

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
INLINE int32 mod_wrap_high(int32 x, int32 mod) {
  return mod_wrap(x,mod);
}

/*! @brief Wrap the input x to be between [0,mod-1].
    x must be in the range [-mod,mod-1].
    Positive wraparound is not checked.

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
INLINE int32 mod_wrap_low(int32 x, int32 mod) {
  return mod_wrap(x,mod);
}

/*! @brief Wrap the input x to be from [-mod/2,(mod+1)/2-1].
    Useful for calculating the signed difference between two numbers
    (x = a - b).
    x must be in the range [-(mod+mod/2),mod+(mod+1)/2-1].

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
INLINE int32 mod_wrap_signed(int32 x, int32 mod) {
  if (x >= ((mod+1)/2))
  {
    x -= mod;
  } else if (x < -(mod/2))
  {
    x += mod;
  }
  return x;
}


/*! @brief 32-bit signed maximum. */
INLINE int32 maxw(int32 x, int32 y) {
  return Q6_R_max_RR(x,y);
}

/*! @brief 32-bit unsigned maximum. */
INLINE uint32 maxuw(uint32 x, uint32 y) {
  return Q6_R_maxu_RR(x,y);
}

/*! @brief 64-bit signed maximum. */
INLINE int64 maxd(int64 x, int64 y) {
  return Q6_P_max_PP(x,y);
}

/*! @brief 64-bit unsigned maximum. */
INLINE uint64 maxud(uint64 x, uint64 y) {
  return Q6_P_maxu_PP(x,y);
}

/*! @brief 32-bit signed minimum. */
INLINE int32 minw(int32 x, int32 y) {
  return Q6_R_min_RR(x,y);
}

/*! @brief 32-bit unsigned minimum. */
INLINE uint32 minuw(uint32 x, uint32 y) {
  return Q6_R_minu_RR(x,y);
}

/*! @brief 64-bit signed minimum. */
INLINE int64 mind(int64 x, int64 y) {
  return Q6_P_min_PP(x,y);
}

/*! @brief 64-bit unsigned minimum. */
INLINE uint64 minud(uint64 x, uint64 y) {
  return Q6_P_minu_PP(x,y);
}

/*! @brief Count number of leading 0-bits in a 32-bit word. */
INLINE int cl0w(int32 x) {
  return Q6_R_cl0_R(x);
}

/*! @brief Count number of leading 0-bits in a 64-bit word. */
INLINE int cl0d(int64 x) {
  return Q6_R_cl0_P(x);
}

/*! @brief Count number of leading 1-bits in a 32-bit word. */
INLINE int cl1w(int32 x) {
  return Q6_R_cl1_R(x);
}

/*! @brief Count number of leading 1-bits in a 64-bit word. */
INLINE int cl1d(int64 x) {
  return Q6_R_cl1_P(x);
}

/*! @brief Count number of leading same-bits in a 32-bit word. */
INLINE int clbw(int32 x) {
  return Q6_R_clb_R(x);
}

/*! @brief Count number of leading same-bits in a 64-bit word. */
INLINE int clbd(int64 x) {
  return Q6_R_clb_P(x);
}

/*! @brief Count number of trailing 0-bits in a 32-bit word. */
INLINE int ct0w(uint32 x) {
  return Q6_R_ct0_R(x);
}

/*! @brief Count number of trailing 0-bits in a 64-bit word. */
INLINE int ct0d(uint64 x) {
  return Q6_R_ct0_P(x);
}

/*! @brief Count number of trailing 1-bits in a 32-bit word. */
INLINE int ct1w(uint32 x) {
  return Q6_R_ct1_R(x);
}

/*! @brief Count number of trailing 1-bits in a 64-bit word. */
INLINE int ct1d(uint64 x) {
  return Q6_R_ct1_P(x);
}


/*! @brief Multiply: signed 16x16 -> signed 32 bits. */
INLINE int32 mpyh(int16 x, int16 y) {
  return ((int32) x * y);
}

/*! @brief Multiply: unsigned 16x16 -> unsigned 32 bits. */
INLINE uint32 mpyuh(uint16 x, uint16 y) {
  return ((uint32) x * y);
}

/*! @brief Multiply: signed 32x32 -> signed 64 bits. */
INLINE int64 mpyw(int32 x, int32 y) {
  return Q6_P_mpy_RR(x,y);
}

/*! @brief Multiply: unsigned 32x32 -> unsigned 64 bits. */
INLINE uint64 mpyuw(uint32 x, uint32 y) {
  return Q6_P_mpyu_RR(x,y);
}


/*! @brief No operation, useful for inserting delays. */
INLINE void nop(void) {
  asm volatile ("nop" : : );
}


/*! @brief Data cache line prefetch. */
INLINE void dcfetch(void *addr) {
  Q6_dcfetch_A(addr);
}

/*! @brief Data cache line clean.
    Write the cache line containing "addr" from cache if dirty. */
INLINE void dccleana(void *addr) {
  asm volatile ("dccleana(%[addr])" : : [addr] "r" (addr) : "memory");
}

/*! @brief Data cache line clean and invalidate.
    Write the cache line containing "addr" from cache if dirty, then
    invalidate that line so it is no longer present in cache. */
INLINE void dccleaninva(void *addr) {
  asm volatile ("dccleaninva(%[addr])" : : [addr] "r" (addr) : "memory" );
}

/*! @brief Data cache line invalidate.
    Invalidate the cache line containing "addr" from cache, discarding
    its contents if dirty.
    NOTE: If addr is not cache line aligned, unrelated data may be
          discarded accidentally. */
INLINE void dcinva(void *addr) {
  asm volatile ("dcinva(%[addr])" : : [addr] "r" (addr) : "memory" );
}

/*! @brief Data cache line zero-allocate. addr must be 32-byte aligned.
    Clean the cache line if dirty, then set all 32 bytes to 0 and mark dirty. */
INLINE void dczeroa(void *addr) {
  asm volatile ("dczeroa(%[addr])" : : [addr] "r" (addr) : "memory" );
}

/*! @brief Instruction cache line invalidate.
    Invalidate the cache line containing "addr" from cache. */
INLINE void icinva(void *addr) {
  asm volatile ("icinva(%[addr])" : : [addr] "r" (addr) : "memory" );
}

/*! @brief Issue an L2 fetch.

    This maps directly to the l2fetch() instruction. Refer to the Hexagon
    programmer's reference for details.

    For simple linear transfers, you may want to use l2fetch_buffer() instead,
    as it has simpler inputs.
*/
INLINE void l2fetch
(
  void *addr,    /*!< (Virtual) address to start fetch */
  uint16 width,  /*!< Width (in bytes) of memory block */
  uint16 height, /*!< Height (number of width-sized blocks) */
  uint16 stride, /*!< Offset to increment after each width-sized block. */
  boolean direction  /*!< 0=row major, 1=column major */
)
{
  asm volatile ("l2fetch(%[addr],%[dim])" : :
     [addr] "r" (addr),
     [dim] "r" ( ((uint64) direction << 48) |
                 ((uint64) stride    << 32) |
                 ((uint64) width     << 16) |
                 ((uint64) height) ) : "memory");
}


/*! @brief Initiate a fetch from external memory to L2 cache.

    This function does a one-dimensional transfer, and has a length limit of
    65535*32=2097120 bytes.
    For 2-dimensional transfers, or for longer lengths, use l2fetch().

    A length of 0 *may* do a single line fetch, depending on the address alignment.

    Also see l2fetch_is_active() to check if the fetch is complete.

    Refer to the Hexagon programmers manual for details on the l2fetch instruction.
    Note that a thread switch, or triggering another l2fetch, may cancel the current
    fetch operation.
    In general, this should be issued before processing a section of data from
    external memory, and whether the data is available when it is accessed
    is best-effort.
*/
INLINE void l2fetch_buffer
(
  void *addr,  /*!< (Virtual) address to start fetch */
  uint32 len   /*!< Length (in bytes) to fetch. */
)
{
  /* Cache-align starting address and length. */
  uint32 ofs = ((uint32) addr) & (L2_CACHE_LINE_SZ-1);
  addr = (void *) ((uint32) addr - ofs);
  len  = (len+ofs+L2_CACHE_LINE_SZ-1) / L2_CACHE_LINE_SZ;

  /* Width=cache line, height=# cache lines, stride=cache line */
  asm volatile ("l2fetch(%[addr],%[dim])" : :
     [addr] "r" (addr),
     [dim] "r" ( Q6_P_combine_IR(L2_CACHE_LINE_SZ, Q6_R_combine_RlRl(L2_CACHE_LINE_SZ, len)) )
     : "memory");

} /* l2fetch_buffer() */


/*! @brief Check if l2fetch operation is active.

    Return TRUE is an l2fetch is in progress, FALSE otherwise.
    Note that if there was a thread switch, the fetch in progress may not be
    the one which is expected.

    Polling for a fetch to be complete is counter-productive - the intended
    use of l2fetch is to run in the background while processing a buffer.
    Polling for completion is no better than doing individual loads to each
    buffer line.
    This function would typically be used only for debug/profiling.
*/
INLINE boolean l2fetch_is_active(void)
{
  uint32 usr = 0;
  asm volatile ("%[reg] = USR" : [reg] "=r"(usr));
  return (usr & BIT(31) ? TRUE : FALSE);
}


/*! @brief Ensure that previous instructions have committed. */
INLINE void isync(void) {
  asm volatile ("isync" : : : "memory" );
}

/*! @brief Force all pending transactions to complete. */
INLINE void syncht(void) {
  asm volatile ("syncht" : : : "memory" );
}

/*! @brief A compiler barrier. Inserting this into the code guarantees
    that the compiler will schedule all instructions preceding it
    before any instructions following it.
    It does not necessarily guarantee that those instructions have completed
    (i.e. memory accesses) as seen by a separate CPU.
*/
INLINE void compiler_barrier(void) {
  asm volatile ("" : : : "memory");
}

/*! @brief A memory barrier. Guarantees that reads/writes preceding
    this are sequentially before the following transactions, as viewed
    by the current Q6 thread. It applies ONLY to the current thread - no
    guarantees about whether other threads have completed their transactions.
*/
INLINE void memory_barrier(void) {
  syncht();
}


/*! @brief Wait for the specified number of thread cycles.
    The actual duration is approximate due to effects of DMT and compiler optimization.
    DO NOT use this for precise wait intervals.
*/
INLINE void wait_tcycles(uint32 tcycles) {
  while (tcycles--)
  { // Usually the compiler will generate a 2-cycle loop for this, so the wait
    // is overestimated. But for smaller loop counts, it will be unrolled.
    nop();
  }
}

#endif  // !defined(MODEM_FW_H)

#endif  // !defined(__ASSEMBLER__)

#endif  // defined(__HEXAGON_ARCH__)

#endif  // MCS_COMDEF_H
