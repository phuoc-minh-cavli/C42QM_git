/*=============================================================================

FILE:         qdss_test.c

DESCRIPTION:  QDSS diag test commands implementation

================================================================================
              Copyright (c) 2013, 2019 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_control_priv.h"
#include "qdss_control.h"
#include "qdss_tfunnel.h"
#include "tracer_test.h"
#include "da_main.h"

/*-------------------------------------------------------------------------*/

/**
  @brief Send test tracer_data packets

  @param                pArgs [in ] : test data args

  @return 0 if successful, error code otherwise
 */
static int tracer_data_test(qdss_tracer_data_test_args *pArgs)
{
   int nErr = QDSS_CONTROL_SUCCESS;
   uint8 i = 0;

   if (0==qdss.hTracer) {
      THROW_IF(nErr,TRACER_RETURN_SUCCESS !=
               tracer_register(&qdss.hTracer,
                               TRACER_ENTITY_DEFAULT,
                               TRACER_PROTOCOL_DEFAULT),
               QDSS_CONTROL_ERROR);
   }

   for (i=0; i < pArgs->count; i++) {
      THROW_IF(nErr,TRACER_RETURN_SUCCESS !=
               tracer_data(qdss.hTracer,
                           TRACER_OPT_GUARANTEE | TRACER_OPT_TIMESTAMP,
                           0,
                           "QDSS tracer test data %d",
                           qdss.test_data_count),
               QDSS_CONTROL_ERROR);

      qdss.test_data_count++;
   }

   CATCH(nErr) {}
   return nErr;
}


static int qdss_funnel_test(qdss_funnel_test_args *pArgs)
{
   int nErr = QDSS_CONTROL_ERROR;
   const char *port_name;

   switch (pArgs->port_id) {
   case 0:
      port_name=TFUNNEL_RPM_PORT;
      break;
   case 1:
      port_name=TFUNNEL_MPSS_PORT;
      break;
   case 2:
      port_name=TFUNNEL_ADSP_PORT;
      break;
   case 3:
      port_name=TFUNNEL_SYSTEM_NOC_PORT;
      break;
   case 4:
      port_name=TFUNNEL_APPS_ETM_PORT;
      break;
   case 5:
      port_name=TFUNNEL_MMSS_NOC_PORT;
      break;
   case 6:
      port_name=TFUNNEL_PERIPHERAL_NOC_PORT;
      break;
   case 8:
      port_name=TFUNNEL_RPM_ITM_PORT;
      break;
   case 9:
      port_name=TFUNNEL_MMSS_PORT;
      break;
   case 10:
      port_name=TFUNNEL_PRONTO_PORT;
      break;
   case 11:
      port_name=TFUNNEL_BIMC_PORT;
      break;
   case 12:
      port_name=TFUNNEL_MODEM_PORT;
      break;
   case 13:
      port_name=TFUNNEL_OCMEM_NOC_PORT;
      break;
   case 15:
      port_name=TFUNNEL_STM_PORT;
      break;
   case 16:
      port_name=TFUNNEL_DEBUG_UI_PORT;
      break;
   case 17:
      port_name=TFUNNEL_TPDA_QDSS_PORT;
   case 18:
      port_name=TFUNNEL_TPDA_DCC_PORT;
   default:
      port_name="invalid";
         break;
   }

   if (0xff == pArgs->port_id) {
      nErr=qdss_tfunnel_disable_all_ports();
   }
   else {
      if (pArgs->state) {
         nErr=qdss_tfunnel_enable_port(port_name);
      }
      else {
         nErr=qdss_tfunnel_disable_port(port_name);
      }
   }

   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handler for QDSS diag test commands

  @return 0 if successful, error code otherwise
 */
int qdss_test_handler(qdss_test_req  *pReq,
                      int req_len,
                      qdss_test_rsp *pRsp,
                      int rsp_len)
{
   qdss_tracer_test_args *pArgs;
   int nErr = QDSS_CONTROL_SUCCESS;
   int rval;

   switch(pReq->test_num) {

   case QDSS_TRACER_DATA_TEST:
      nErr = tracer_data_test((qdss_tracer_data_test_args *)pReq->args);
      break;

   case QDSS_FUNNEL_TEST:
      if (req_len < (sizeof(qdss_test_req) +
                     sizeof(qdss_funnel_test_args) - 1) ) {
         nErr = QDSS_CONTROL_FORMAT_ERROR;
      }
      else {
         nErr = qdss_funnel_test((qdss_funnel_test_args *)pReq->args);
      }
      break;

   case QDSS_TRACER_TEST:
      if (req_len < (sizeof(qdss_test_req) +
                     sizeof(qdss_tracer_test_args) - 1) ) {
         nErr = QDSS_CONTROL_FORMAT_ERROR;
      }
      else {
         pArgs = (qdss_tracer_test_args *)pReq->args;
         rval = tracer_test(pArgs->type, pArgs->subtype,
                            pArgs->option, pArgs->mask,
                            pArgs->count1, pArgs->count2);

         nErr = (TRACER_TEST_RVAL_SUCCESS == rval) ? QDSS_CONTROL_SUCCESS :
                ((TRACER_TEST_RVAL_ERROR == rval) ? QDSS_CONTROL_ERROR :
                ((TRACER_TEST_RVAL_REQUEST_ERR == rval) ? QDSS_CONTROL_FORMAT_ERROR :
                ((TRACER_TEST_RVAL_UNSUPPORTED == rval) ? QDSS_CONTROL_UNSUPPORTED :
                                                            QDSS_CONTROL_ERROR)));
      }
      break;

   default:
      nErr = QDSS_CONTROL_ERROR;
   }

   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handler for QDSS diag agent test

  @return 0 if successful,
 */
int qdss_agent_test_handler(qdss_agent_test_req *pReq, int req_len,
                            qdss_agent_test_rsp *pRsp, int rsp_len)
{
   int rval;
   da_status_t test_status;

   pRsp->result = 0xFFFF;
   pRsp->status1 = 0xFFFF;
   pRsp->status2 = 0xFFFF;
   if (req_len >= sizeof(qdss_agent_test_req))
   {
      switch (pReq->test_num)
      {
         case 1:
            rval = da_start((int)pReq->start_cfg_id, &test_status);
            pRsp->result = rval;
            pRsp->status1 = test_status.status1;
            pRsp->status2 = test_status.status2;
            break;
         default:
            break;
      }
   }
   return pRsp->result;
}

