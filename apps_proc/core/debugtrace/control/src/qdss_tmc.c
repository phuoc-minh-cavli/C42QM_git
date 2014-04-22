/*=============================================================================

FILE:         qdss_tmc.c

DESCRIPTION:  This file has the implementation for all TMC diag commands

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_control_priv.h"
#include "qdss_control.h"

#include "qdss_osal.h"

/*-------------------------------------------------------------------------*/

/**
  @brief Handler for QDSS_ETR_SET_BYTE_COUNTER diag TMC command

  @param                 pReq [in ] :  request packet
  @param                 pRsp [in ] :  response packet

  @return 0 if successful, error code otherwise
 */
static int qdss_etr_set_byte_counter_handler(qdss_etr_set_byte_counter_req *pReq,
                                      qdss_etr_set_byte_counter_rsp *pRsp)
{
   int nErr = QDSS_CONTROL_SUCCESS;
   #define ATB_FRAME_SIZE 16
   if ((pReq->counter < ATB_FRAME_SIZE) ||
       (pReq->counter > qdss.buf.etr_buf_size)) {
      THROW(nErr,QDSS_CONTROL_BAD_PARAM);
   }
   qdss.etr.byte_counter = pReq->counter;

   CATCH(nErr){}
   return nErr;
}


/**
  @brief Handler for QDSS_ETR_SET_TIMER diag TMC command

  @param                 pReq [in ] :  request packet
  @param                 pRsp [in ] :  response packet

  @return 0 if successful, error code otherwise
 */
static int qdss_etr_set_timer_handler(qdss_etr_set_timer_req *pReq,
                                      qdss_etr_set_timer_rsp *pRsp)
{
   int nErr = QDSS_CONTROL_SUCCESS;
#define QDSS_ETR_TIMER_MIN 100  //100 ms minim
   if (pReq->timer_ms < QDSS_ETR_TIMER_MIN) {
      THROW(nErr,QDSS_CONTROL_BAD_PARAM);
   }
   qdss.etr.timer_us = pReq->timer_ms * 1000;

   CATCH(nErr){}
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief Handler for QDSS_TMC_READ_START diag command

  @return 0 if successful, error code otherwise
 */
static int qdss_tmc_read_start_handler(qdss_tmc_read_start_req *pReq,
                                qdss_tmc_read_start_rsp *pRsp)
{
      return DalTMC_Flush(qdss.hTMC,TMC_FLUSH_STOP);
}



/**
  @brief Handler for QDSS_TMC_READ diag command

  @return 0 if successful, error code otherwise
 */
static int qdss_tmc_read_handler(qdss_tmc_read_req *pReq,
                          qdss_tmc_read_rsp *pRsp)
{
   int nErr = QDSS_CONTROL_SUCCESS;
   uint32 len = pReq->len;

   if (len > QDSS_TMC_READ_BLOCK_SIZE) {
      len = QDSS_TMC_READ_BLOCK_SIZE;
   }

   TRY(nErr,DalTMC_ReadTrace(qdss.hTMC,
                             pRsp->data,
                             len,
                             (uint32 *)&pRsp->len));

   CATCH(nErr) {}


   return nErr;
}


/**
  @brief Handler for QDSS_TMC_READ_END diag command

  @return 0 if successful, error code otherwise
 */
static int qdss_tmc_read_end_handler(qdss_tmc_read_end_req *pReq,
                                qdss_tmc_read_end_rsp *pRsp)
{
   int nErr1,nErr2 = 0;

   nErr1 = qdss_trace_sink_disable();
   nErr2 = qdss_trace_sink_enable();

   return (nErr1|nErr2);
}


#define QDSS_HANDLE_TMC_CMD(cmd)                                        \
   if (pkt_len < (sizeof(qdss_tmc_req) + sizeof(cmd##_req) -1 )) {      \
      pRsp = diagpkt_err_rsp(DIAG_BAD_LEN_F, pReq, pkt_len);            \
   }                                                                    \
   else {                                                               \
      pRsp =  diagpkt_subsys_alloc                                      \
         (DIAG_SUBSYS_QDSS,                                             \
          pReq->hdr.subsysCmdCode,                                      \
          (sizeof(qdss_tmc_rsp)+sizeof(cmd##_rsp) -1) );                \
      if (NULL != pRsp) {                                               \
         nErr =  cmd##_handler((cmd##_req *)pReq->args,                 \
                               (cmd##_rsp *)(((qdss_tmc_rsp *)pRsp)->args)); \
      }                                                                 \
   }


/*-------------------------------------------------------------------------*/

/**
  @brief Handler for all TMC diag commandds


  @return pointer to a the DIAG response packet
 */
PACK(void *)  qdss_tmc_handler(qdss_tmc_req  *pReq,
                               int pkt_len)
{
   int nErr;
   PACK(void *) pRsp = NULL;

   if (FALSE==qdss.ctrlMutex_enable) {
      return diagpkt_err_rsp(QDSS_CONTROL_BAD_STATE, pReq, pkt_len);
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   switch(pReq->id) {
   case QDSS_ETR_SET_BYTE_COUNTER:
      QDSS_HANDLE_TMC_CMD(qdss_etr_set_byte_counter);
      break;

   case QDSS_TMC_READ_START:
      QDSS_HANDLE_TMC_CMD(qdss_tmc_read_start);
      break;

   case QDSS_TMC_READ:
      QDSS_HANDLE_TMC_CMD(qdss_tmc_read)
      break;

   case QDSS_TMC_READ_END:
      QDSS_HANDLE_TMC_CMD(qdss_tmc_read_end)
      break;

   case QDSS_ETR_SET_TIMER:
      QDSS_HANDLE_TMC_CMD(qdss_etr_set_timer);
      break;


   default:
      nErr=QDSS_CONTROL_ERROR;
   }

   CATCH(nErr){ }


   if  (NULL == pRsp) {
      pRsp = diagpkt_err_rsp(nErr, pReq, pkt_len);
   }
   else {
      ((qdss_tmc_rsp*)pRsp)->id = pReq->id;
      ((qdss_tmc_rsp *)pRsp)->result = nErr;
   }
   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return pRsp;

}

