#include <stdint.h>
#include <stdbool.h>

#include "ce.h"

uint32_t generate_rand32(void);
#if 0
int ce_utf8_decode(
    CE_OP *op, uint8_t *inbuf, int inbuflen, uint8_t *outbuf, int *outbuflen)
{
    op = op;
    inbuf = inbuf;
    inbuflen = inbuflen;
    outbuf = outbuf;
    outbuflen = outbuflen;
    return 0;
}
#endif
int RANDOM_FN()
{
    return (int)generate_rand32();
}

