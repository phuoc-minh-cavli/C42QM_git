#ifndef TMS_UTILS_H
#define TMS_UTILS_H
/** vi: tw=128 ts=3 sw=3 et
@file tms_utils.h
@brief This file contains the API for the TMS Utilities, API 0.1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.tx/6.0/debugtools/api/tms_utils.h#1 $
$DateTime: 2020/01/30 22:49:35 $
$Change: 22400605 $
$Author: pwbldsvc $
===============================================================================*/

/**

All function implementations as part of this API must remain reentrant. The
routines specifically do not use any NHLOS API which will block, or otherwise
serialize access to the data parameters.

It is the caller's responsiblity to handle data resource contention issues
as necessary. No routine within this API will internally supply resource
management for supplied data parameters.

All routines are not universally supplied automatically to every PL. The
API is governed by this header file, but that does not mean a specific PL
will have runtime access to 100% of this API. View the source directory
for details of the per PL supported functions.

*/

#include "ctype.h"
#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** @addtogroup tms_utils_api @{ */

/**
API, Typedefs and Enumerations
*/

typedef char tms_chr;                                                            /**< NULL Terminated Buffer */
typedef tms_chr* tms_chr_p;                                                      /**< NULL Terminated Buffer Pointer */

typedef void* tms_os_store;                                                      /**< TMS_OS Client Supplied Resource Storage */

typedef enum
{
   TMS_UTILS_STATUS_SUCCESS = 0,                                                 /**< SUCCESS INDICATION, evaluates as >= TMS_UTILS_STATUS_SUCCESS */
   TMS_UTILS_STATUS_ERROR = -1                                                   /**< FAILURE INDICATION, evaluates as < TMS_UTILS_STATUS_SUCCESS */

} TMS_UTILS_STATUS;

/**

API Compiler Independent Unused Variable Warning Removals

All routines supplied in this API will supply success indication to the caller;
caller must make arrangements to cache or evaluate the return code.

Use of the macro TMS_UTILS_MASK_UNUSED is an explicit acknowledgement by a
developer that:

1. The variable supplied as argument is known to be unused in certain configurations.
These configurations are known, and the compiler is not expected to generate a warning
or error.

2. The function call supplied as argument is known to return success indication. The
success indication is irrelevant to the algorithm. The compiler is not expected to
generate a warning or error.

The macro TMS_UTILS_MASK_UNUSED argument is wrapped in a portable language construct
that will remove compiler warnings when a variable remains unused at compile time.
The macro is expected to generate a code path that is removed by the compiler when
optimization is enabled. The macro is not to be used with an argument that carries
either prefex or suffix  perator (eg, unused++, unused--, ++unused, --unused, ...).

Example

int unused = 0;

TMS_UTILS_MASK_UNUSED(unused);

TMS_UTILS_MASK_UNUSED(tms_utils_fmt(...));

*/

#define TMS_UTILS_MASK_UNUSED(var) do { if (var) {;} } while (0)

/**

API Information for tms_utils_chr_*()

Input Result Buffers will be both NULL Terminated and Size Bounded
Output Result Buffers will always be NULL Terminated and Size Bounded

Example Initializers

tms_chr const* return_result;
tms_chr const buf[] = "const string initializer";
tms_chr out_buf[OUT_BUF_SIZ];

*/

/**
API, NULL Terminated Buffer Append, Result Buffer Always NULL Terminated

if (sizeof(buf) > tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
size_t tms_utils_chr_append(tms_chr* out_buf_p, size_t out_buf_sz, tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Compare

if (0 == tms_utils_chr_compare(buf, sizeof(buf), ...)) match;
if (0 > tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;
if (0 < tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
int32_t tms_utils_chr_compare(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Copy, Result Buffer Always NULL Terminated

if (sizeof(buf) > tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1,3)))
#endif
size_t tms_utils_chr_copy(tms_chr* out_buf_p, size_t out_buf_sz, tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Search, First Occur of Param

if (NULL != tms_utils_chr_first(buf, sizeof(buf), 'r')) found;
if (NULL == tms_utils_chr_first(buf, sizeof(buf), 'r')) not_found;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr                       Input Character
@return        tms_chr*                      Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
tms_chr const* tms_utils_chr_first(tms_chr const* in_buf_p, size_t in_buf_sz, const tms_chr chr);

/**
API, NULL Terminated Buffer Search, Last Occur of Param

if (NULL != tms_utils_chr_last(buf, sizeof(buf), 'r')) found;
if (NULL == tms_utils_chr_last(buf, sizeof(buf), 'r')) not_found;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr                       Input Character
@return        tms_chr*                      Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
tms_chr const* tms_utils_chr_last(tms_chr const* in_buf_p, size_t in_buf_sz, const tms_chr chr);

/**
API, NULL Terminated Buffer Length

if (sizeof(buf) > tms_utils_chr_length(buf, sizeof(buf))) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_length(buf, sizeof(buf))) buffer_overflow;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
size_t tms_utils_chr_length(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Search, First Occurrance of Second Buffer Match

if (NULL != tms_utils_chr_run(buf, sizeof(buf), buf2, sizeof(buf2))) segment_pointer;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        tms_chr*                      Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
tms_chr const* tms_utils_chr_run(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Search, First Occurrance of Any Byte From Second Buffer

if (NULL != tms_utils_chr_sep(buf, sizeof(buf), buf2, sizeof(buf2))) segment_pointer;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        tms_chr*                      Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
tms_chr const* tms_utils_chr_sep(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Initializer

if (sizeof(buf) > tms_utils_chr_set(buf, sizeof(buf), chr)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_set(buf, sizeof(buf), chr)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr                       Output Buffer Initializer
@return        size_t                        Operation Result
*/
size_t tms_utils_chr_set(tms_chr* out_buf_p, size_t out_buf_sz, const tms_chr chr);

/**
API, NULL Terminated Buffer Search, Length of Initial Segment Comprised of Bytes from Second Buffer

if (0 != tms_utils_chr_span(buf, sizeof(buf), buf2, sizeof(buf2))) segment_length;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
size_t tms_utils_chr_span(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Search, Length of Initial Segment Not Comprised of Bytes from Second Buffer

if (0 != tms_utils_chr_span_n(buf, sizeof(buf), buf2, sizeof(buf2))) segment_length_result;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
size_t tms_utils_chr_span_n(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, Formatted Output to Sized Buffer

NOTE: Caller of routine supplies NHLOS protections to Output Buffer if
required. The routine itself does not carry any specific NHLOS details.
This routine supports only the following template expansions, nothing
more.

Supported Template Format Expansions

   %[-][0][ww][type]

   [-] left justification within field
   [0] zero pad numeric output
   [www] field width
   [type] field type
      c  character argument
      s  NULL terminated buffer pointer argument
      b  binary uint32_t argument
      d  decimal int32_t argument
      o  octal uint32_t argument
      x  hex uint32_t argument
      ld decimal int32_t argument
      lx hex uint32_t argument
      X  hex uint32_t argument (caps expansion)
      lX hex uint32_t argument (caps expansion)

Use Case Example

   tms_chr buf[256];
   int32_t size;

   if (sizeof(buf) > (size = tms_utils_fmt(buffer, sizeof(buffer), "%d", 12345)))
   {
      PRINT("%d, %d == strlen(\"%s\")\n", count, strlen(buffer), buffer);
   }
   else
   {
      ERROR(buf_overflow);
   }

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr*                      Format Template Pointer
@param[in]     ...                           Format Template Expansion Arguments
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3), format(printf, 3, 4)))
#endif
int32_t tms_utils_fmt(tms_chr* out_buf_p, size_t out_buf_sz, tms_chr const* fmt_str, ...);

/**
API, Generic FIFO Byte Buffer, Setup for Opaque List Handle

static buf[1024]; // FIFO Byte Store

tms_utils_chr_fifo_t fifo;

tms_chr chr;

if (0 <= tms_utils_chr_fifo_init(&fifo, buf, sizeof(buf))) chr_fifo_success;

if (0 <= tms_utils_chr_fifo_put(&fifo, 'r')) chr_fifo_success;

if (0 <= tms_utils_chr_fifo_get(&fifo, &chr)) chr_fifo_success;

*/

typedef struct                                                                   /**< INTERNAL, Used as Opaque Handle */
{
  size_t size;                                                                   /**< INTERNAL, Managed FIFO Size */
  tms_chr* buf_p;                                                                /**< INTERNAL, Managed FIFO Store */

  size_t offset_get;                                                             /**< INTERNAL, Managed Get Byte */
  size_t offset_put;                                                             /**< INTERNAL, Managed Put Byte */

} tms_utils_chr_fifo_t, *tms_utils_chr_fifo_p;

/**
API, FIFO Byte Buffer, Initialize

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@param[in]     tms_chr*                      FIFO Byte Buffer Backing Store Pointer
@param[in]     size_t                        FIFO Byte Buffer Backing Store Size
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1,2)))
#endif
int32_t tms_utils_chr_fifo_init(tms_utils_chr_fifo_p fifo_p, tms_chr* buf_p, size_t size);

/**
API, FIFO Byte Buffer, Store FIFO Byte

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int32_t tms_utils_chr_fifo_put(tms_utils_chr_fifo_p fifo_p, tms_chr chr);

/**
API, FIFO Byte Buffer, Retrieve FIFO Byte

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@param[in]     tms_chr*                      FIFO Byte Result Pointer
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1,2)))
#endif
int32_t tms_utils_chr_fifo_get(tms_utils_chr_fifo_p fifo_p, tms_chr* chr_p);

/**
API, FIFO Byte Buffer, FIFO Buffer Actual Size

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
size_t tms_utils_chr_fifo_size(tms_utils_chr_fifo_p fifo_p);

/**
API, FIFO Byte Buffer, FIFO Buffer Current Fill Count

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
size_t tms_utils_chr_fifo_elements(tms_utils_chr_fifo_p fifo_p);

/**
API, Generic Linked List, Setup for Opaque List Handle

TMS_UTILS_LIST(name);

{
   struct { int i; } *data;

   tms_utils_list_init(name);
   data = malloc(sizeof(*data)); data->i = 0;

   FIFO
   tms_utils_list_head_push(name, data);
   if (NULL == (data = tms_utils_head_pop(name))) fifo_empty;

   LIFO
   tms_utils_list_head_push(name, data);
   if (NULL == (data = tms_utils_tail_pop(name))) lifo_empty;
}

*/

typedef void** tms_utils_list_t; /**< Opaque List Handle */

#define TMS_UTILS_LIST(name) static void* name##_list = NULL; static tms_utils_list_t name = (tms_utils_list_t)&name##_list
#define TMS_UTILS_LIST_STRUCT(name) void* name##_list; tms_utils_list_t name
#define TMS_UTILS_LIST_STRUCT_INIT(struct_p, name) do { (struct_p)->name = &((struct_p)->name##_list); (struct_p)->name##_list = NULL; tms_utils_list_init((struct_p)->name); } while(0)

/**
API, Generic Linked List, Duplicate List

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     tms_utils_list_t              Opaque List Handle
@return        NONE                          No Return
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1, 2)))
#endif
void tms_utils_list_duplicate(tms_utils_list_t list_dst, tms_utils_list_t list_src);

/**
API, Generic Linked List, Initialization

@param[in]     tms_utils_list_t              Opaque List Handle
@return        NONE                          No Return
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1)))
#endif
void tms_utils_list_init(tms_utils_list_t list);

/**
API, Generic Linked List, Return Head

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
void* tms_utils_list_head_peek(tms_utils_list_t list);

/**
API, Generic Linked List, Head Pop, Drop

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
void* tms_utils_list_head_pop(tms_utils_list_t list);

/**
API, Generic Linked List, Head Push, Add

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1, 2)))
#endif
void tms_utils_list_head_push(tms_utils_list_t list, void* item);

/**
API, Generic Linked List, Items In List

@param[in]     tms_utils_list_t              Opaque List Handle
@return        int                           Item Count of List
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int tms_utils_list_length(tms_utils_list_t list);

/**
API, Generic Linked List, Delete From List

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1, 2)))
#endif
void tms_utils_list_middle_delete(tms_utils_list_t list, void* item);

/**
API, Generic Linked List, Insert Within List

@param[in]     void*                         Data Payload Pointer
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1, 2, 3)))
#endif
void tms_utils_list_middle_insert(tms_utils_list_t list, void* item_prev, void* item_new);

/**
API, Generic Linked List, Tail Item

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
void* tms_utils_list_tail_peek(tms_utils_list_t list);

/**
API, Generic Linked List, Tail Pop, Drop

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
void* tms_utils_list_tail_pop(tms_utils_list_t list);

/**
API, Generic Linked List, Tail Push, Append

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1, 2)))
#endif
void tms_utils_list_tail_push(tms_utils_list_t list, void* item);

/**
API, Generic Linked List, Item Following Current

@param[in]     void*                         Data Payload Pointer
@return        void*                         Data Payload Pointer
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
void* tms_utils_list_item_next(void* item);

/**
API, NULL Terminated Buffer Scan as Decimal, Return Numeric Representation

tms_chr const buf[] = "665";
if (665 == tms_utils_num_int32(buf, sizeof(buf))) parsed_decimal;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int32_t                       Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int32_t tms_utils_num_int32(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Hex, Return Numeric Representation

tms_chr const buf[] = "12345678";
if (0x12345678 == tms_utils_num_uint32(buf, sizeof(buf))) parsed_hex;

The scanner does not validate the content of the buffer. The
scanner does not parse leading 0x|0X. All buffer validation must
be performed before use of this routine.

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint32_t                      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
uint32_t tms_utils_num_uint32(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Decimal, Return Numeric Representation

tms_chr const buf[] = "50000";
if (50000 == tms_utils_num_int64(buf, sizeof(buf))) parsed_decimal;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int64_t                       Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int64_t tms_utils_num_int64(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Hex, Return Numeric Representation

tms_chr const buf[] = "aabbccdd";
if (0xaabbccdd == tms_utils_num_uint64(buf, sizeof(buf))) parsed_hex;

The scanner does not validate the content of the buffer. The
scanner does not parse leading 0x|0X. All buffer validation must
be performed before use of this routine.

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint64_t                      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
uint64_t tms_utils_num_uint64(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, Utility Hash Function

tms_chr const buf[] = "string initializer";
uint32_t hash = tms_utils_hash(buf, sizeof(buf));

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint32_t                      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
uint32_t tms_utils_hash(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, Priority Inheritance Semaphore
*/

typedef void* tms_os_pisem;

/**
API, Priority Inheritance Semaphore, Required Client Supplied Resource Storage Size

Client caller is responsible to manage supplied resource storage.

@return        size_t                        Required Client Supplied Resource Storage Size (Bytes)
*/
size_t tms_utils_os_pisem_sz(void);

/**
API, Priority Inheritance Semaphore, Initialize

Client caller is responsible to manage supplied resource storage.

tms_os_pisem sem;
tms_os_store sem_s;
if (NULL == (sem_s = (tms_os_store)malloc(tms_utils_os_pisem_sz()))) not_initialized;
if (NULL == (sem = tms_utils_os_pisem_init(sem_s, tms_utils_os_pisem_sz()))) not_initialized;

@param[in]     tms_os_store                  Client Supplied Resource Storage Address
@param[in]     size_t                        Client Supplied Resource Storage Size
@return        tms_os_pisem                  Opaque Semaphore Handle
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
tms_os_pisem tms_utils_os_pisem_init(tms_os_store sem_s, size_t sem_sz);

/**
API, Priority Inheritance Semaphore, Destroy

Client caller is responsible to manage supplied resource storage.

if (TMS_UTILS_STATUS_SUCCESS > tms_utils_os_pisem_destroy(sem)) failure;
if (sem_s) free(sem_s);

@param[in]     tms_os_pisem                  Opaque Semaphore Handle
@return        TMS_UTILS_STATUS              Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
TMS_UTILS_STATUS tms_utils_os_pisem_destroy(tms_os_pisem sem);

/**
API, Priority Inheritance Semaphore, Wait

if (TMS_UTILS_STATUS_SUCCESS > tms_utils_os_pisem_wait(sem)) failure;

@param[in]     tms_os_pisem                  Opaque Semaphore Handle
@return        TMS_UTILS_STATUS              Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
TMS_UTILS_STATUS tms_utils_os_pisem_wait(tms_os_pisem sem);

/**
API, Priority Inheritance Semaphore, Post

if (TMS_UTILS_STATUS_SUCCESS > tms_utils_os_pisem_post(sem)) failure;

@param[in]     tms_os_pisem                  Opaque Semaphore Handle
@return        TMS_UTILS_STATUS              Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
TMS_UTILS_STATUS tms_utils_os_pisem_post(tms_os_pisem sem);

/**
API, Priority Inheritance Mutex
*/

typedef void* tms_os_pitex;

/**
API, Priority Inheritance Semaphore, Required Client Supplied Resource Storage Size

Client caller is responsible to manage supplied resource storage.

@return        size_t                        Required Client Supplied Resource Storage Size (Bytes)
*/
size_t tms_utils_os_pitex_sz(void);

/**
API, Priority Inheritance Mutex, Initialize

Client caller is responsible to manage supplied resource storage.

tms_os_pitex tex;
tms_os_store tex_s;
if (NULL == (tex_s = (tms_os_store)malloc(tms_utils_os_pitex_sz()))) not_initialized;
if (NULL == (tex = tms_utils_os_pitex_init(tex_s, tms_utils_os_pitex_sz()))) not_initialized;

@param[in]     tms_os_store                  Client Supplied Resource Storage Address
@param[in]     size_t                        Client Supplied Resource Storage Size
@return        tms_os_pitex                  Opaque Mutex Handle
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
tms_os_pitex tms_utils_os_pitex_init(tms_os_store tex_s, size_t tex_sz);

/**
API, Priority Inheritance Mutex, Destroy

Client caller is responsible to manage supplied resource storage.

if (TMS_UTILS_STATUS_SUCCESS > tms_utils_os_pitex_destroy(tex)) failure;
if (tex_s) free(tex_s);

@param[in]     tms_os_pitex                  Opaque Mutex Handle
@return        TMS_UTILS_STATUS              Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
TMS_UTILS_STATUS tms_utils_os_pitex_destroy(tms_os_pitex tex);

/**
API, Priority Inheritance Mutex, Trylock

if (TMS_UTILS_STATUS_SUCCESS > tms_utils_os_pitex_trylock(tex)) failure;

@param[in]     tms_os_pitex                  Opaque Mutex Handle
@return        TMS_UTILS_STATUS              Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
TMS_UTILS_STATUS tms_utils_os_pitex_trylock(tms_os_pitex tex);

/**
API, Priority Inheritance Mutex, Lock

if (TMS_UTILS_STATUS_SUCCESS > tms_utils_os_pitex_lock(tex)) failure;

@param[in]     tms_os_pitex                  Opaque Mutex Handle
@return        TMS_UTILS_STATUS              Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
TMS_UTILS_STATUS tms_utils_os_pitex_lock(tms_os_pitex tex);

/**
API, Priority Inheritance Mutex, Unlock

if (TMS_UTILS_STATUS_SUCCESS > tms_utils_os_pitex_unlock(tex)) failure;

@param[in]     tms_os_pitex                  Opaque Mutex Handle
@return        TMS_UTILS_STATUS              Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
TMS_UTILS_STATUS tms_utils_os_pitex_unlock(tms_os_pitex tex);

/** @} end_addtogroup tms_utils_api */

#if defined(__cplusplus)
}
#endif

#endif
