#ifndef SHA2_CORE_H
#define SHA2_CORE_H
//#include "sha_shared.h"
#include <stddef.h>
#define EXPLICIT_INT_TYPES
#ifdef EXPLICIT_INT_TYPES
typedef unsigned char uint8_t;
typedef unsigned long long uint64_t;
#endif /* EXPLICIT_INT_TYPES not defined */



#define SW_SHA512_BLOCK_SIZE            128
#define SW_SHA512_DIGEST_SIZE           (512 / 8)

#define SW_SHA384_BLOCK_SIZE            SW_SHA512_BLOCK_SIZE
#define SW_SHA384_DIGEST_SIZE           (384 / 8)

#define SHA512_BLOCK_LENGTH             SW_SHA512_BLOCK_SIZE
#define SHA512_DIGEST_LENGTH            SW_SHA512_DIGEST_SIZE
#define SHA512_DIGEST_STRING_LENGTH     (SHA512_DIGEST_LENGTH * 2 + 1)

#define SHA384_BLOCK_LENGTH             SHA512_BLOCK_LENGTH
#define SHA384_DIGEST_LENGTH            SW_SHA384_DIGEST_SIZE
#define SHA384_DIGEST_STRING_LENGTH     (SHA384_DIGEST_LENGTH * 2 + 1)

struct SHA512_384_CTX
{
  uint64_t  state[8];
  uint64_t  bitcount[2];
  uint8_t   buffer[SHA512_BLOCK_LENGTH];
};

typedef struct SHA512_384_CTX SHA512_384_CTX;
typedef struct SHA512_384_CTX SHA512_CTX;
typedef SHA512_384_CTX SHA384_CTX;
/*** SHA-384/512 Function Prototypes ******************************/
void
SHA384_Init(
  SHA512_384_CTX*);

void
SHA384_Update(
  SHA512_384_CTX*,
  const uint8_t*,
  size_t);

void
SHA384_Final(
  uint8_t[SHA384_DIGEST_LENGTH],
  SHA512_384_CTX*);

void
SHA512_Init(
  SHA512_384_CTX*);

void
SHA512_Update(
  SHA512_384_CTX*,
  const uint8_t*,
  size_t);

void
SHA512_Final(
  uint8_t[SHA512_DIGEST_LENGTH],
  SHA512_384_CTX*);

#endif
