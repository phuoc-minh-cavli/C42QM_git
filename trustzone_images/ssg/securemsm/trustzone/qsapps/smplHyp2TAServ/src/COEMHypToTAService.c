// Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#include "IGenericService_invoke.h"
#include "qsee_services.h"
#include "qsee_env.h"
#include "qsee_log.h"
#include "stringl.h"

/* Interpretation of i/p request buffer implementation specific.
   VMApp & TA mutualy agree & define i/p buffer contains / format as per there needs.
   Below is an example usage, where TA expects first 4 byte as command 
   and follwed by payload data if any.
 */
typedef struct
{
  uint32_t   cmd;                    // Let's use first 4 byte for cmd.
  uint8_t    payload[0];             // Remainning data may hold more info on need basis.
} HypToTA_Req_t;

typedef enum
{
  HYP_TO_TA_CMD_GET_ALIPAY_KEY_ID,     // Get the Alipay Key.
  HYP_TO_TA_CMD_GET_LOC_DATA_ID,       // Get the location.
} HypToTA_cmd_t;

uint8_t  alipay_key[] = "It's AliPay Key from SFS / RPMB";  /* Real App may get the Key from RPMB / SFS.
                                                               App can't use Listener when cmd comes from
                                                               Guest VM. App must buffer the data during app
                                                               loading itself OR HLOS Client may send a
                                                               cmd to TA to buffer this data before NS GuestVM
                                                               issue get Alipay Key cmd. */
uint8_t  loc_data[] = "It's Loc Data Received from Modem";  /* App can't use Secure Channel when cmd comes from
                                                               Guest VM. App must buffer the data by receiving other
                                                               cmd from HLOS before it receive this command. */

static int32_t CGenericService_handleCommand(void *me,
                     const void *inBuf_ptr, size_t inBuf_len,
                     const void *outBuf_ptr, size_t outBuf_len, size_t *outBuf_lenout)
{
  // IPC guarantees non-NULL pointers when length is non-zero.
  int      ret = Object_ERROR;

  /* Validate i/p buffer before reading.
   * Based on current definition of HypToTA_Req_t, i/p buffer need to be atleast 4 byte & 4 byte aligned.
   */
  if ((inBuf_len < sizeof(HypToTA_Req_t)) || (0x3 & (uintptr_t)inBuf_ptr))
  {
    /* In sufficient request buffer.*/
    qsee_log(QSEE_LOG_MSG_ERROR, "smplHyp2TAServ : CGenericService_handleCommand() failed, inBuf_ptr : 0x%x, inBuf_len : 0x%x", inBuf_ptr, inBuf_len);
    goto bail;
  }
  HypToTA_Req_t *req = (HypToTA_Req_t *)inBuf_ptr;
  HypToTA_cmd_t cmd = req->cmd;

  switch (cmd)
  {
    case HYP_TO_TA_CMD_GET_ALIPAY_KEY_ID:
    {
      /* Validate o/p buffer before writting. */
      if(outBuf_len >= sizeof(alipay_key))
      {
        for (int i=0; i<sizeof(alipay_key); i++)
        {
          *((uint8_t*)outBuf_ptr + i) = alipay_key[i];
        }
        *outBuf_lenout = sizeof(alipay_key);
        ret = Object_OK;
        goto bail;
      }
      qsee_log(QSEE_LOG_MSG_ERROR, "smplHyp2TAServ : CGenericService_handleCommand() failed, cmd : 0x%x, outBuf_len : 0x%x", cmd, outBuf_len);
      
      break;
    }

    case HYP_TO_TA_CMD_GET_LOC_DATA_ID:
    {
      /* Validate o/p buffer before writting. */
      if(outBuf_len >= sizeof(loc_data))
      {
        for (int i=0; i<sizeof(loc_data); i++)
        {
          *((uint8_t*)outBuf_ptr + i) = loc_data[i];
        }
        *outBuf_lenout = sizeof(loc_data);
        ret = Object_OK;
        goto bail;
      }
      qsee_log(QSEE_LOG_MSG_ERROR, "smplHyp2TAServ : CGenericService_handleCommand() failed, cmd : 0x%x, outBuf_len : 0x%x", cmd, outBuf_len);
      
      break;
    }

  }

bail:
  return ret;

}


// This implementation does not require a context record, so `retain` and
// `release` are no-ops.

#define CGenericService_release(ctx)   Object_OK
#define CGenericService_retain(ctx)    Object_OK

static IGenericService_DEFINE_INVOKE(CGenericService_invoke, CGenericService_, void*)

int32_t COEMHypToTAService_open(Object cred, Object *objOut)
{
   *objOut = (Object) { CGenericService_invoke, NULL };
   return Object_OK;
}
