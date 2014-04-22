#ifndef HEADER_VMAC_H
#define HEADER_VMAC_H

/* --------------------------------------------------------------------------
 * VMAC and VHASH Implementation by Ted Krovetz (tdk@acm.org) and Wei Dai.
 * This implementation is herby placed in the public domain.
 * The authors offers no warranty. Use at your own risk.
 * Please send bug reports to the authors.
 * Last modified: 17 APR 08, 1700 PDT
 * ----------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 * User definable settings.
 * ----------------------------------------------------------------------- */
#define VMAC_TAG_LEN   64 /* Must be 64 or 128 - 64 sufficient for most    */
#define VMAC_KEY_LEN  128 /* Must be 128, 192 or 256                       */
#define VMAC_NHBYTES  128 /* Must 2^i for any 3 < i < 13. Standard = 128   */
#define VMAC_PREFER_BIG_ENDIAN  0  /* Prefer non-x86 */
#define VMAC_HZ (448e6)  /* Set to hz of host machine to get speed        */
#define VMAC_HASH_ONLY 0  /* Set to non-zero to time hash only (not-mac)   */
/* Speeds of cpus I have access to
#define hz (2400e6)  glyme Core 2 "Conroe"
#define hz (2000e6)  jupiter G5
#define hz (1592e6)  titan
#define hz (2793e6)  athena/gaia
#define hz (1250e6)  isis G4
#define hz (2160e6)  imac Core 2 "Merom"
#define hz (266e6)   ppc/arm
#define hz (400e6)   mips
*/

/* --------------------------------------------------------------------------
 * This implementation uses uint32_t and uint64_t as names for unsigned 32-
 * and 64-bit integer types. These are defined in C99 stdint.h. The
 * following may need adaptation if you are not running a C99 or
 * Microsoft C environment.
 * ----------------------------------------------------------------------- */
#define VMAC_USE_STDINT 1  /* Set to zero if system has no stdint.h        */
 
#if VMAC_USE_STDINT && !_MSC_VER /* Try stdint.h if non-Microsoft          */
#ifdef  __cplusplus
#define __STDC_CONSTANT_MACROS
#endif
#include <stdint.h>
#elif (_MSC_VER)                  /* Microsoft C does not have stdint.h    */
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#define UINT64_C(v) v ## UI64
#else                             /* Guess sensibly - may need adaptation  */
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
#define UINT64_C(v) v ## ULL
#endif

typedef struct {
	uint64_t nhkey  [(VMAC_NHBYTES/8)+2*(VMAC_TAG_LEN/64-1)];
	uint64_t polykey[2*VMAC_TAG_LEN/64];
	uint64_t l3key  [2*VMAC_TAG_LEN/64];
	uint64_t polytmp[2*VMAC_TAG_LEN/64];
	int first_block_processed;
} vhash_ctx_t;

/* --------------------------------------------------------------------- */
#ifdef  __cplusplus
extern "C" {
#endif
/* --------------------------------------------------------------------------
 *                        <<<<< USAGE NOTES >>>>>
 *
 * Given msg m (mbytes in length) and nonce buffer n
 * this function returns a tag as its output. The tag is returned as
 * a number. When VMAC_TAG_LEN == 64, the 'return'ed integer is the tag,
 * and *tagl is meaningless. When VMAC_TAG_LEN == 128 the tag is the
 * number y * 2^64 + *tagl where y is the function's return value.
 * If you want to consider tags to be strings, then you must do so with
 * an agreed upon endian orientation for interoperability, and convert
 * the results appropriately. VHASH hashes m without creating any tag.
 * Consecutive substrings forming a prefix of a message may be passed
 * to vhash_update, with vhash or vmac being called with the remainder
 * to produce the output.
 *
 * Requirements:
 * - On 32-bit architectures with SSE2 instructions, ctx and m MUST be
 *   begin on 16-byte memory boundaries.
 * - m MUST be your message followed by zeroes to the nearest 16-byte
 *   boundary. If m is a length multiple of 16 bytes, then it is already
 *   at a 16-byte boundary and needs no padding. mbytes should be your
 *   message length without any padding. 
 * - The first bit of the nonce buffer n must be 0. An i byte nonce, is made
 *   as the first 16-i bytes of n being zero, and the final i the nonce.
 * - vhash_update MUST have mbytes be a positive multiple of VMAC_NHBYTES
 * ----------------------------------------------------------------------- */

void vhash_init(vhash_ctx_t* ctx, void(*wait)(void), void(*ready)(void*));

uint64_t vhash(unsigned char m[],
          unsigned int mbytes,
          uint64_t *tagl,
          vhash_ctx_t* ctx);


/* --------------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif

#endif /* HEADER_AES_H */
