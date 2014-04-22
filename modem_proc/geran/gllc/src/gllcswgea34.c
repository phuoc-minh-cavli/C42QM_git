/*
  @file
  gllcswgea34.c

  @brief
  Contains the implementation of the GEA/3 and GEA/4 encryption algorithms
*/
/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gllc/src/gllcswgea34.c#1 $ $DateTime: 2018/02/08 01:15:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/12   NDL     First release
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stringl.h>
#include "customer.h"
#ifndef TEST_FRAMEWORK

#include "gllcswgea34.h"
#include "msg.h"

/*===========================================================================

                          LOCAL VARIABLES

===========================================================================*/

// Table used to extend the key to 128 bits
static const uint8 gfwKeyMask[8] = {0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF};

/*===========================================================================

                    NATIVE FUNCTIONS DECLARATIONS

===========================================================================*/

/*===========================================================================

  FUNCTION: gfw_key_schedule()

===========================================================================*/
void gfw_key_schedule(uint8  *pKey,
                      vect64 *pKOi,
                      vect64 *pKLi); /* extern_in_c_ignore */

/*===========================================================================

  FUNCTION: gfw_kasumi()

===========================================================================*/
vect64 gfw_kasumi(vect64  data,
                  vect64 *pKOi,
                  vect64 *pKLi); /* extern_in_c_ignore */

/*===========================================================================

                    LOCAL FUNCTIONS DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION: gfw_kg_core_init()

===========================================================================*/
static void gfw_kg_core_init(vect64 *pKeyOrig,
                             vect64 *pKeyCopy,
                             vect64 *pModKey,
                             vect64 *pTemp,
                             vect64 *pA,
                             uint32 *pBlkCnt,
                             uint8   ca,
                             uint8   cb,
                             uint32  fn,
                             uint8   dir,
                             GfwKasumiScratchType *pScratch)
{
   vect64 A, temp;

   // Copy over the key
   pKeyCopy[0] = pKeyOrig[0];
   pKeyCopy[1] = pKeyOrig[1];

   // Start by building our global modifier
   temp.d = A.d = 0;

   // Initialise register in an endian correct manner
   A.uw[0] = Q6_R_swiz_R(fn);
   A.ub[4] = (cb << 3) | (dir << 2);
   A.ub[5] = ca;

   // Construct the modified key and then "kasumi" A
   pModKey[0].d = (pKeyOrig[0].d ^ 0x5555555555555555LL);
   pModKey[1].d = (pKeyOrig[1].d ^ 0x5555555555555555LL);

   gfw_key_schedule((uint8 *)pModKey,
                    pScratch->KOi,
                    pScratch->KLi);

   // First encryption to create modifier
   A = gfw_kasumi(A,
                  pScratch->KOi,
                  pScratch->KLi);

   // Final initialisation steps
   *pBlkCnt = 0;
   gfw_key_schedule((uint8 *)pKeyCopy,
                    pScratch->KOi,
                    pScratch->KLi);

   *pA = A;
   *pTemp = temp;

} // end gfw_kg_core_init

/*===========================================================================

  FUNCTION: gfw_kg_core()

===========================================================================*/
static void gfw_kg_core(uint8   ca,
                        uint8   cb,
                        uint32  fn,
                        uint8   dir,
                        vect64 *pKeyOrig,
                        vect64 *pKeyCopy,
                        vect64 *pModKey,
                        vect64 *pvOut,
                        vect64 *pA,
                        vect64 *pTemp,
                        uint32 *pBlkCnt,
                        int32   cl,
                        boolean bInit,
                        GfwKasumiScratchType *pScratch)
{
   vect64 A, temp;
   uint32 blkCnt;
   int32 i, n;
   uint8 *pOut8;

   if (bInit)
   {
      // Initialize registers, keys and counters
      gfw_kg_core_init(pKeyOrig,
                       pKeyCopy,
                       pModKey,
                       pTemp,
                       pA,
                       pBlkCnt,
                       ca,
                       cb,
                       fn,
                       dir,
                       pScratch);
   }

   // Transfer to local copies to accelerate execution
   A = *pA;
   temp = *pTemp;
   blkCnt = *pBlkCnt;

   pOut8 = (uint8 *)pvOut;

   // Now run the key stream generator
   while (cl > 0)
   {
      /* First we calculate the next 64-bits of keystream
         XOR in A and BLKCNT to last value
      */
      temp.d ^= A.d;
      temp.ub[7] ^= blkCnt;

      // KASUMI it to produce the next block of keystream
      temp = gfw_kasumi(temp,
                        pScratch->KOi,
                        pScratch->KLi);

      /* Set <n> to the number of bytes of input data
         we have to modify. (=8 if length <= 64)
      */
      if (cl >= 64)
      {
         n = 8;
      }
      else
      {
         n = (cl + 7) / 8;
      }

      // Copy out the keystream
      for (i = 0; i < n; i++)
      {
         *pOut8++ = temp.ub[i];

         /* MSG_HIGH(" Key Stream Generated %d: 0x%x",i, temp.ub[i], 0); */
      }

      cl -= 64; // done another 64 bits
      blkCnt++; // increment BLKCNT
   }

   *pA = A;
   *pTemp = temp;
   *pBlkCnt = blkCnt;

} // end gfw_kg_core

/*===========================================================================

  FUNCTION: gfw_build_key()

===========================================================================*/
static void gfw_build_key(const vect64 *pvKey,
                          uint32        len,
                          vect64       *pKey128)
{
   uint8 *ck;
   int32 numBytes, n, sf;

   ck = (uint8 *)pKey128;

   // Number of required bytes
   numBytes = (len + 7) / 8;

   // Limit to 128 bits
   numBytes = Q6_R_min_RR(numBytes, 16);

   // Copy key
   (void)memscpy
   (
     pKey128,
     numBytes,
     pvKey,
     numBytes
   );

   // Any odd key length?
   sf = len % 8;

   /* If the key is less than 128-bits we need to replicate
      it as many times as is necessary to fill the key
   */
   if (len < 128)
   {
      if (sf) // Doesn't align to byte boundaries
      {
         ck[numBytes - 1] &= gfwKeyMask[sf];
         ck[numBytes - 1] += ck[0] << sf;

         for (n = 0; numBytes < 16; n++, numBytes++)
         {
            ck[numBytes] = (ck[n] >> (8 - sf)) + (ck[n + 1] << sf);
         }
      }
      else
      {
        (void)memscpy
        (
          &ck[numBytes],
          16 - numBytes,
          &ck[0],
          16 - numBytes
        );
      }
   }

} // end gfw_build_key

/*===========================================================================

                         EXTERNALIZED FUNCTIONS

===========================================================================*/


/*===========================================================================

  FUNCTION: gfw_gea_34()

===========================================================================*/
void gfw_gea_34(vect64                *pKey,
                int32                  kLen,
                uint32                 input,
                uint8                  direction,
                vect64                *pBlock,
                GfwGea34ControlStruct *pControl)
{
   int32 nBytesGenerated;

#if 0
    uint8 i;

    /* DA_DEBUG */
    for ( i =0; i < kLen/8; i++)
    {
     MSG_HIGH(" Key Octet %d: 0x%x",i, pKey->ub[i], 0);
    }
    /* DA_DEBUG end */
#endif

   // If this is the first call, initialize the number of bytes left and the key
   if (pControl->bFirst)
   {
      pControl->nBytesLeft = pControl->nBytesTotal;
      gfw_build_key(pKey, kLen, pControl->key128);
   }

   // Number of bytes generated in this call. Make sure to handle last call correctly
   nBytesGenerated = Q6_R_min_RR(pControl->nBytesLeft, pControl->nBytesPerCall);

   // Core algorithm
   gfw_kg_core(0xFF,
               0,
               input,
               direction,
               pControl->key128,
               pControl->keyCopy,
               pControl->keyMod,
               pBlock,
               &pControl->mainReg,
               &pControl->tempReg,
               &pControl->blockCount,
               nBytesGenerated * 8,
               pControl->bFirst,
               &pControl->kasumiScratch);

   pControl->bFirst = FALSE;

   // Update number of bytes left. Do not allow it to go negative
   pControl->nBytesLeft -= nBytesGenerated;
   pControl->nBytesLeft = Q6_R_max_RR(pControl->nBytesLeft, 0);

} // end gfw_gea_34
#endif
