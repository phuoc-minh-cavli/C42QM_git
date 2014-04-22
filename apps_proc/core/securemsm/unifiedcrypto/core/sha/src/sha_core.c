/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       S E C U R I T Y   S E R V I C E S

                       H A S H I N G   F U N C T I O N S

                   ( A R M   I M P L E M E N T A T I O N S )

GENERAL DESCRIPTION
  This sofware module contains the source code for the Security Services
  Crypto-Interface layer for message digest specific to the ARM.

EXTERNAL FUNCTIONS

  sechsh_create_digest() - Creates the message digest

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/securemsm/unifiedcrypto/core/sha/src/sha_core.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/12   yk      initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sha_core.h"
//#include "comdef.h"
//#include "stdlib.h"
//#include "string.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

typedef union
{
    uuint32 l[16];
    unsigned char c[64];

} CHAR64LONG16;

/* Optimizations */
uuint32 rol30( uuint32 value)
{
  return (value << 30) | ((value >> 2) & 0x3fffffff);
}

uuint32 rol24( uuint32 value)
{
  return (value << 24) | ((value >> 8) & 0x00ffffff);
}

uuint32 rol8( uuint32 value)
{
  return (value << 8) | ((value >> 24) & 0x000000ff);
}

uuint32 rol5( uuint32 value)
{
  return (value << 5) | ((value >> 27) & 0x0000001f);
}

uuint32 rol1( uuint32 value)
{
  return (value << 1) | ((value >> 31) & 0x00000001);
}

/* blk0() and blk() perform the initial expand */

//#if defined(__BIG_ENDIAN)
//#define blk0(block, i)      (block->l[i])
//#else
#define blk0(block, i) (block->l[i]        = (rol24(block->l[i]) & 0xff00ff00) | (rol8(block->l[i]) & 0x00ff00ff))
//#endif
#define blk(block, i)  (block->l[i & 0x0f] = rol1(block->l[(i + 13) & 0x0f] ^ block->l[(i +  8) & 0x0f] ^ block->l[(i +  2) & 0x0f] ^ block->l[(i +  0) & 0x0f]))

/* R0, R1, R2, R3, R4 are the different operations used in SHA1 */

#define R0(block, work, v, w, x, y, z, i) \
    (work[z] += ((work[w]&(work[x]^work[y]))^work[y])           + blk0(block, i)    + 0x5a827999 + rol5(work[v]), work[w] = rol30(work[w]))

#define R1(block, work, v, w, x, y, z, i) \
    (work[z] += ((work[w]&(work[x]^work[y]))^work[y])           + blk(block, i)     + 0x5a827999 + rol5(work[v]), work[w] = rol30(work[w]))

#define R2(block, work, v, w, x, y, z, i) \
    (work[z] += (work[w]^work[x]^work[y])                       + blk(block, i)     + 0x6ed9eba1 + rol5(work[v]), work[w] = rol30(work[w]))

#define R3(block, work, v, w, x, y, z, i) \
    (work[z] += (((work[w]|work[x])&work[y])|(work[w]&work[x])) + blk(block, i)     + 0x8f1bbcdc + rol5(work[v]), work[w] = rol30(work[w]))

#define R4(block, work, v, w, x, y, z, i) \
    (work[z] += (work[w]^work[x]^work[y])                       + blk(block, i)     + 0xca62c1d6 + rol5(work[v]), work[w] = rol30(work[w]))

#if 0
/* Forward declarations of the internal working functions */
void sechsharm_sha1_init(   struct __sechsh_ctx_s* );
void sechsharm_sha1_update( struct __sechsh_ctx_s*, uuint8*, uuint32*, uuint8*, uuint32 );
void sechsharm_sha1_final(  struct __sechsh_ctx_s*, uuint8*, uuint32*, uuint8* );
void sechsharm_sha1_transform( uuint32*, unsigned char* );
#endif

/* ------------------------------------------------------------ */
/* Constants and definitions for SHA-256                          */
/* ------------------------------------------------------------ */
#define HSHSHA_COUNTER_SIZE 8

// Try and optimize this thing
#define SHA2_UNROLL_TRANSFORM

/*** SHA-256 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH    64
#define SHA256_SHORT_BLOCK_LENGTH	(SHA256_BLOCK_LENGTH - 8)

/*** ENDIAN SPECIFIC COPY MACROS **************************************/
#define BE_8_TO_32(dst, cp) do {				\
      (dst) = (uuint32)(cp)[3] | ((uuint32)(cp)[2] << 8) |	\
	 ((uuint32)(cp)[1] << 16) | ((uuint32)(cp)[0] << 24);	\
   } while(0)

#define BE_8_TO_64(dst, cp) do {					\
      (dst) = (uint64_t)(cp)[7] | ((uint64_t)(cp)[6] << 8) |		\
	 ((uint64_t)(cp)[5] << 16) | ((uint64_t)(cp)[4] << 24) |	\
	 ((uint64_t)(cp)[3] << 32) | ((uint64_t)(cp)[2] << 40) |	\
	 ((uint64_t)(cp)[1] << 48) | ((uint64_t)(cp)[0] << 56);		\
   } while (0)

#define BE_64_TO_8(cp, src) do {		\
      (cp)[0] = (uuint8)((src) >> 56);		\
      (cp)[1] = (uuint8)((src) >> 48);		\
      (cp)[2] = (uuint8)((src) >> 40);		\
      (cp)[3] = (uuint8)((src) >> 32);		\
      (cp)[4] = (uuint8)((src) >> 24);		\
      (cp)[5] = (uuint8)((src) >> 16);		\
      (cp)[6] = (uuint8)((src) >> 8);		\
      (cp)[7] = (uuint8)((src));			\
   } while (0)

#define BE_32_TO_8(cp, src) do {		\
      (cp)[0] = (uuint8)((src) >> 24);		\
      (cp)[1] = (uuint8)((src) >> 16);		\
      (cp)[2] = (uuint8)((src) >> 8);		\
      (cp)[3] = (uuint8)((src));			\
   } while (0)

/*
 * Macro for incrementally adding the unsigned 64-bit integer n to the
 * unsigned 128-bit integer (represented using a two-element array of
 * 64-bit words):
 */
#define ADDINC128(w,n) do {			\
      (w)[0] += (uint64_t)(n);			\
      if ((w)[0] < (n)) {			\
	 (w)[1]++;				\
      }						\
   } while (0)

/** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  The naming of R and S appears backwards here (R is a SHIFT and
 *   S is a ROTATION) because the SHA-256/384/512 description document
 *   (see http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf) uses this
 *   same "backwards" definition.
 */
/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define R(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define SHA_S32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define SHA_S64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(SHA_S32(2,  (x)) ^ SHA_S32(13, (x)) ^ SHA_S32(22, (x)))
#define Sigma1_256(x)	(SHA_S32(6,  (x)) ^ SHA_S32(11, (x)) ^ SHA_S32(25, (x)))
#define sigma0_256(x)	(SHA_S32(7,  (x)) ^ SHA_S32(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)	(SHA_S32(17, (x)) ^ SHA_S32(19, (x)) ^ R(10,   (x)))



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                  INTERNAL FUNCTION DEFINTIONS FOR SHA-1
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION SECHSHARM_SHA1_TRANSFORM()

DESCRIPTION
  Transform the input block into a digest update.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_transform
(
  uuint32*         iv,
  unsigned char*  buf
)
{
  uuint32 bufi[16];                     /* internal buffer */
  uuint32 work[5];                      /* internal work registers */
  
  CHAR64LONG16*  block = (CHAR64LONG16*) bufi;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Copy operating buffer to buf */
  memcpy( bufi, buf, sizeof( bufi ) );

  /* Copy context->iv[] to working vars */
  work[0] = iv[0];                 /* a */
  work[1] = iv[1];                 /* b */
  work[2] = iv[2];                 /* c */
  work[3] = iv[3];                 /* d */
  work[4] = iv[4];                 /* e */

  /* 4 rounds of 20 operations each. Loop unrolled */
  /* Round 1 */
  R0( block, work, 0, 1, 2, 3, 4,  0 );
  R0( block, work, 4, 0, 1, 2, 3,  1 );
  R0( block, work, 3, 4, 0, 1, 2,  2 );
  R0( block, work, 2, 3, 4, 0, 1,  3 );
  R0( block, work, 1, 2, 3, 4, 0,  4 );
  R0( block, work, 0, 1, 2, 3, 4,  5 );
  R0( block, work, 4, 0, 1, 2, 3,  6 );
  R0( block, work, 3, 4, 0, 1, 2,  7 );
  R0( block, work, 2, 3, 4, 0, 1,  8 );
  R0( block, work, 1, 2, 3, 4, 0,  9 );
  R0( block, work, 0, 1, 2, 3, 4, 10 );
  R0( block, work, 4, 0, 1, 2, 3, 11 );
  R0( block, work, 3, 4, 0, 1, 2, 12 );
  R0( block, work, 2, 3, 4, 0, 1, 13 );
  R0( block, work, 1, 2, 3, 4, 0, 14 );
  R0( block, work, 0, 1, 2, 3, 4, 15 );

  R1( block, work, 4, 0, 1, 2, 3, 16 );
  R1( block, work, 3, 4, 0, 1, 2, 17 );
  R1( block, work, 2, 3, 4, 0, 1, 18 );
  R1( block, work, 1, 2, 3, 4, 0, 19 );

  /* Round 2 */
  R2( block, work, 0, 1, 2, 3, 4, 20 );
  R2( block, work, 4, 0, 1, 2, 3, 21 );
  R2( block, work, 3, 4, 0, 1, 2, 22 );
  R2( block, work, 2, 3, 4, 0, 1, 23 );
  R2( block, work, 1, 2, 3, 4, 0, 24 );
  R2( block, work, 0, 1, 2, 3, 4, 25 );
  R2( block, work, 4, 0, 1, 2, 3, 26 );
  R2( block, work, 3, 4, 0, 1, 2, 27 );
  R2( block, work, 2, 3, 4, 0, 1, 28 );
  R2( block, work, 1, 2, 3, 4, 0, 29 );
  R2( block, work, 0, 1, 2, 3, 4, 30 );
  R2( block, work, 4, 0, 1, 2, 3, 31 );
  R2( block, work, 3, 4, 0, 1, 2, 32 );
  R2( block, work, 2, 3, 4, 0, 1, 33 );
  R2( block, work, 1, 2, 3, 4, 0, 34 );
  R2( block, work, 0, 1, 2, 3, 4, 35 );
  R2( block, work, 4, 0, 1, 2, 3, 36 );
  R2( block, work, 3, 4, 0, 1, 2, 37 );
  R2( block, work, 2, 3, 4, 0, 1, 38 );
  R2( block, work, 1, 2, 3, 4, 0, 39 );

  /* Round 3 */
  R3( block, work, 0, 1, 2, 3, 4, 40 );
  R3( block, work, 4, 0, 1, 2, 3, 41 );
  R3( block, work, 3, 4, 0, 1, 2, 42 );
  R3( block, work, 2, 3, 4, 0, 1, 43 );
  R3( block, work, 1, 2, 3, 4, 0, 44 );
  R3( block, work, 0, 1, 2, 3, 4, 45 );
  R3( block, work, 4, 0, 1, 2, 3, 46 );
  R3( block, work, 3, 4, 0, 1, 2, 47 );
  R3( block, work, 2, 3, 4, 0, 1, 48 );
  R3( block, work, 1, 2, 3, 4, 0, 49 );
  R3( block, work, 0, 1, 2, 3, 4, 50 );
  R3( block, work, 4, 0, 1, 2, 3, 51 );
  R3( block, work, 3, 4, 0, 1, 2, 52 );
  R3( block, work, 2, 3, 4, 0, 1, 53 );
  R3( block, work, 1, 2, 3, 4, 0, 54 );
  R3( block, work, 0, 1, 2, 3, 4, 55 );
  R3( block, work, 4, 0, 1, 2, 3, 56 );
  R3( block, work, 3, 4, 0, 1, 2, 57 );
  R3( block, work, 2, 3, 4, 0, 1, 58 );
  R3( block, work, 1, 2, 3, 4, 0, 59 );

  /* Round 4 */
  R4( block, work, 0, 1, 2, 3, 4, 60 );
  R4( block, work, 4, 0, 1, 2, 3, 61 );
  R4( block, work, 3, 4, 0, 1, 2, 62 );
  R4( block, work, 2, 3, 4, 0, 1, 63 );
  R4( block, work, 1, 2, 3, 4, 0, 64 );
  R4( block, work, 0, 1, 2, 3, 4, 65 );
  R4( block, work, 4, 0, 1, 2, 3, 66 );
  R4( block, work, 3, 4, 0, 1, 2, 67 );
  R4( block, work, 2, 3, 4, 0, 1, 68 );
  R4( block, work, 1, 2, 3, 4, 0, 69 );
  R4( block, work, 0, 1, 2, 3, 4, 70 );
  R4( block, work, 4, 0, 1, 2, 3, 71 );
  R4( block, work, 3, 4, 0, 1, 2, 72 );
  R4( block, work, 2, 3, 4, 0, 1, 73 );
  R4( block, work, 1, 2, 3, 4, 0, 74 );
  R4( block, work, 0, 1, 2, 3, 4, 75 );
  R4( block, work, 4, 0, 1, 2, 3, 76 );
  R4( block, work, 3, 4, 0, 1, 2, 77 );
  R4( block, work, 2, 3, 4, 0, 1, 78 );
  R4( block, work, 1, 2, 3, 4, 0, 79 );

  /* Add the working vars back into context.iv[] */
  iv[0] += work[0];
  iv[1] += work[1];
  iv[2] += work[2];
  iv[3] += work[3];
  iv[4] += work[4];

} /* sechsharm_sha1_transform() */


/*===========================================================================

FUNCTION SECHSHARM_SHA1_INIT()

DESCRIPTION
  Initialize the hash context to prepare for message digest operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_init
(
  struct __sechsh_ctx_s* context             /* context */
)
{
  /* SHA1 initialization constants */
  context->iv[0] = 0x67452301;
  context->iv[1] = 0xefcdab89;
  context->iv[2] = 0x98badcfe;
  context->iv[3] = 0x10325476;
  context->iv[4] = 0xc3d2e1f0;

  context->counter[0] = context->counter[1] = 0;

} /* sechsharm_sha1_init() */


/*===========================================================================

FUNCTION SECHSHARM_SHA1_UPDATE()

DESCRIPTION
  Continue the SHA1 digest operation, process input in block-sized chunks,
  save the leftover input, update the context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_update
(
  struct __sechsh_ctx_s* context,            /* context */
  uuint8*                 leftover,           /* leftover input in context */
  uuint32*                leftover_size,
  uuint8*                 data,
  uuint32                 len
)
{
  int i, j;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Compute number of bytes mod 64 */
  j = (context->counter[0] >> 3) & 63;

  /* Update number of bits */
  context->counter[0] += (len << 3);
  if ( context->counter[0] < (len << 3) )
  {
    context->counter[1]++;
  }
  context->counter[1] += (len >> 29);

  /* Transform as many times as possible. */
  if ( (j + len) > SECAPI_HSH_SHA_BLOCK_BYTE_LEN-1 )
  {
    i = SECAPI_HSH_SHA_BLOCK_BYTE_LEN - j;

    memcpy( &leftover[j], data, i );

    j = 0;

    sechsharm_sha1_transform( context->iv, /*context->*/leftover );

    while (i + SECAPI_HSH_SHA_BLOCK_BYTE_LEN-1 < len)
    {
      sechsharm_sha1_transform( context->iv, &data[i] );
      i += SECAPI_HSH_SHA_BLOCK_BYTE_LEN;
    }
  }
  else
  {
    i = 0;
  }

  /* Buffer remaining input */
  memcpy( &leftover[j], &data[i], (len - i) );

  /* Update context var for fallback */
  *leftover_size = (uuint32)(j + len - i);
} /* sechsharm_sha1_update() */


/*===========================================================================

FUNCTION SECHSHARM_SHA1_FINAL()

DESCRIPTION
  Do the final steps of a SHA1 hash, output the digest value,
  clear the context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_final
(
  struct __sechsh_ctx_s* context,            /* context */
  uuint8*                 leftover,           /* leftover input in context */
  uuint32*                leftover_size,
  uuint8*                 digest
)
{
  uuint32 i;
  unsigned char finalcount[8];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( i = 0; i < 8; i++ )
  {
    /* Note: This is endianess-independent */
    finalcount[i] = (unsigned char) ((context->counter[(i >= 4 ? 0 : 1)]
                                      >> ((3-(i & 3)) * 8) ) & 255);
  }

  {
    unsigned char data[] = { 0x80};

    sechsharm_sha1_update( context, leftover, leftover_size, data, sizeof( data ) );
  }

  while ( (context->counter[0] & 0x01f8) != 0x01c0 )
  {
    unsigned char data[] = { 0x00};

    sechsharm_sha1_update( context, leftover, leftover_size, data, sizeof( data ) );
  }

  sechsharm_sha1_update( context, leftover, leftover_size, finalcount, 8 );

  for ( i = 0; i < SW_SHA1_DIGEST_SIZE; i++ )
  {
    digest[i] = (unsigned char)
      ((context->iv[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
  }

  /* Wipe variables */
  i = 0;
  memset( context, 0, sizeof( struct __sechsh_ctx_s) );
  memset( &finalcount, 0, sizeof( finalcount ) );

  sechsharm_sha1_transform( context->iv, /*context->*/leftover );

} /* sechsharm_sha1_final() */

#ifdef SHA2_UNROLL_TRANSFORM

/* Unrolled SHA-256 round macros: */

#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h) do {				\
      BE_8_TO_32(W256[j], data);					\
      data += 4;							\
      T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] + W256[j]; \
      (d) += T1;							\
      (h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));			\
      j++;								\
   } while(0)

#define ROUND256(a,b,c,d,e,f,g,h) do {					\
      s0 = W256[(j+1)&0x0f];						\
      s0 = sigma0_256(s0);						\
      s1 = W256[(j+14)&0x0f];						\
      s1 = sigma1_256(s1);						\
      T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] +	\
	 (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);			\
      (d) += T1;							\
      (h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));			\
      j++;								\
   } while(0)

static void sechsharm_sha256_transform
(
  uuint32 state[8],
  const uuint8 data[SHA256_BLOCK_LENGTH]
)
{
   uuint32	a, b, c, d, e, f, g, h, s0, s1;
   uuint32	T1, W256[16];
   int		j;

   /* Initialize registers with the prev. intermediate value */
   a = state[0];
   b = state[1];
   c = state[2];
   d = state[3];
   e = state[4];
   f = state[5];
   g = state[6];
   h = state[7];

   j = 0;
   /* Klocwork: Filter. For the second iteration of the loop j
    * is 15 for ROUND256_0_TO_15(b,c,d,e,f,g,h,a) and is then
    * incremented to 16 at which time it breaks out of the loop.
    */
   do {
      /* Rounds 0 to 15 (unrolled): */
      ROUND256_0_TO_15(a,b,c,d,e,f,g,h);
      ROUND256_0_TO_15(h,a,b,c,d,e,f,g);
      ROUND256_0_TO_15(g,h,a,b,c,d,e,f);
      ROUND256_0_TO_15(f,g,h,a,b,c,d,e);
      ROUND256_0_TO_15(e,f,g,h,a,b,c,d);
      ROUND256_0_TO_15(d,e,f,g,h,a,b,c);
      ROUND256_0_TO_15(c,d,e,f,g,h,a,b);
      ROUND256_0_TO_15(b,c,d,e,f,g,h,a);
   } while (j < 16);

   /* Now for the remaining rounds up to 63: */
   /* Klocwork: Filter. The first time through the loop
    * j is 16 for ROUND256(a,b,c,d,e,f,g,h) and 23
    * for ROUND256(b,c,d,e,f,g,h,a) and the last time
    * through the loop j is 63 for ROUND256(b,c,d,e,f,g,h,a)
    * which will not overflow the array.
    */
   do {
      ROUND256(a,b,c,d,e,f,g,h);
      ROUND256(h,a,b,c,d,e,f,g);
      ROUND256(g,h,a,b,c,d,e,f);
      ROUND256(f,g,h,a,b,c,d,e);
      ROUND256(e,f,g,h,a,b,c,d);
      ROUND256(d,e,f,g,h,a,b,c);
      ROUND256(c,d,e,f,g,h,a,b);
      ROUND256(b,c,d,e,f,g,h,a);
   } while (j < 64);

   /* Compute the current intermediate hash value */
   state[0] += a;
   state[1] += b;
   state[2] += c;
   state[3] += d;
   state[4] += e;
   state[5] += f;
   state[6] += g;
   state[7] += h;

   /* Clean up */
   a = b = c = d = e = f = g = h = T1 = 0;
}

#else /* SHA2_UNROLL_TRANSFORM */

static void sechsharm_sha256_transform
(
  uuint32 state[8],
  const uuint8 data[SHA256_BLOCK_LENGTH]
)
{
   uuint32	a, b, c, d, e, f, g, h, s0, s1;
   uuint32	T1, T2, W256[16];
   int		j;

   /* Initialize registers with the prev. intermediate value */
   a = state[0];
   b = state[1];
   c = state[2];
   d = state[3];
   e = state[4];
   f = state[5];
   g = state[6];
   h = state[7];

   j = 0;
   do {
      BE_8_TO_32(W256[j], data);
      data += 4;
      /* Apply the SHA-256 compression function to update a..h */
      T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];
      T2 = Sigma0_256(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;

      j++;
   } while (j < 16);

   do {
      /* Part of the message block expansion: */
      s0 = W256[(j+1)&0x0f];
      s0 = sigma0_256(s0);
      s1 = W256[(j+14)&0x0f];
      s1 = sigma1_256(s1);

      /* Apply the SHA-256 compression function to update a..h */
      T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] +
	 (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);
      T2 = Sigma0_256(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;

      j++;
   } while (j < 64);

   /* Compute the current intermediate hash value */
   state[0] += a;
   state[1] += b;
   state[2] += c;
   state[3] += d;
   state[4] += e;
   state[5] += f;
   state[6] += g;
   state[7] += h;

   /* Clean up */
   a = b = c = d = e = f = g = h = T1 = T2 = 0;
}

#endif /* SHA2_UNROLL_TRANSFORM */

void sechsharm_sha256_init
(
  struct __sechsh_ctx_s* context             /* context */
)
{
   if (NULL == context)
      return;

  /* SHA256 initialization constants */
   memcpy((uuint8*)context->iv, (uuint8*)sha256_initial_hash_value, sizeof(sha256_initial_hash_value));

   context->counter[0] = 0;
   context->counter[1] = 0;
}

void sechsharm_sha256_update
(
  struct __sechsh_ctx_s* context,            /* context */
  uuint8*                 leftover,           /* leftover input in context */
  uuint32*                leftover_size,
  uuint8*                 data,
  uuint32                 len
)
{
   uuint32	freespace, usedspace;
   uuint64    bitcounter;

   /* Calling with no data is valid (we do nothing) */
   if (len == 0)
      return;

   memcpy(&bitcounter, context->counter, HSHSHA_COUNTER_SIZE);
   usedspace = ((uuint32)bitcounter >> 3) % SHA256_BLOCK_LENGTH;
   if (usedspace > 0) {
      /* Calculate how much free space is available in the buffer */
      freespace = SHA256_BLOCK_LENGTH - usedspace;

      if (len >= freespace) {
	 /* Fill the buffer completely and process it */
	 memcpy((uuint8*)&leftover[usedspace], data, freespace);
	 bitcounter += (uuint32)(freespace << 3);
	 len -= freespace;
	 data += freespace;
	 sechsharm_sha256_transform(context->iv, leftover);
      } else {
	 /* The buffer is not yet full */
	 memcpy((uuint8*)&leftover[usedspace], data, len);
	 bitcounter += (uuint32)(len << 3);
	 /* Clean up: */
	 usedspace = freespace = 0;
         memcpy(context->counter, &bitcounter, HSHSHA_COUNTER_SIZE);
	 return;
      }
   }
   while (len >= SHA256_BLOCK_LENGTH) {
      /* Process as many complete blocks as we can */
      sechsharm_sha256_transform(context->iv, data);
      bitcounter += SHA256_BLOCK_LENGTH << 3;
      len -= SHA256_BLOCK_LENGTH;
      data += SHA256_BLOCK_LENGTH;
   }
   if (len > 0) {
      /* There's left-overs, so save 'em */
      memcpy((uuint8*)leftover, data, len);
      bitcounter += (uuint32)(len << 3);
   }
   /* Clean up: */
   usedspace = freespace = 0;
   memcpy(context->counter, &bitcounter, HSHSHA_COUNTER_SIZE);
}


static void sechsharm_sha256_pad
(
  struct __sechsh_ctx_s* context,
  uuint8*                leftover           /* leftover input in context */
)
{
   uuint32 usedspace;
   uuint64 bitcounter;

   memcpy(&bitcounter, context->counter, 8);
   usedspace = (context->counter[0] >> 3) % SHA256_BLOCK_LENGTH;
   if (usedspace > 0) {
      /* Begin padding with a 1 bit: */
      leftover[usedspace++] = 0x80;

      if (usedspace <= SHA256_SHORT_BLOCK_LENGTH) {
	 /* Set-up for the last transform: */
	 memset(&leftover[usedspace], 0,
		SHA256_SHORT_BLOCK_LENGTH - usedspace);
      } else {
	 if (usedspace < SHA256_BLOCK_LENGTH) {
	    memset(&leftover[usedspace], 0,
		   SHA256_BLOCK_LENGTH - usedspace);
	 }
	 /* Do second-to-last transform: */
	 sechsharm_sha256_transform(context->iv, leftover);

	 /* Prepare for last transform: */
	 memset(leftover, 0, SHA256_SHORT_BLOCK_LENGTH);
      }
   } else {
      /* Set-up for the last transform: */
      memset(leftover, 0, SHA256_SHORT_BLOCK_LENGTH);

      /* Begin padding with a 1 bit: */
      *leftover = 0x80;
   }
   /* Store the length of input data (in bits) in big endian format: */
    BE_64_TO_8(&leftover[SHA256_SHORT_BLOCK_LENGTH], bitcounter);

   /* Final transform: */
   sechsharm_sha256_transform(context->iv, leftover);

   memcpy(context->counter, &bitcounter, HSHSHA_COUNTER_SIZE);
   /* Clean up: */
   usedspace = 0;
}


void sechsharm_sha256_final
(
  struct __sechsh_ctx_s* context,            /* context */
  uuint8*                 leftover,           /* leftover input in context */
  uuint32*                leftover_size,
  uuint8*                 digest
)
{
   sechsharm_sha256_pad(context, leftover);

   /* If no digest buffer is passed, we don't bother doing this: */
   if (digest != NULL) {
     int	i;

     /* Convert TO host byte order */
     for (i = 0; i < 8; i++)
	 BE_32_TO_8(digest + i * 4, context->iv[i]);

     /* zeroize context structure */
     memset((uuint8*)context, 0, sizeof(*context));

     sechsharm_sha256_transform( context->iv, /*context->*/leftover );
   }
}
