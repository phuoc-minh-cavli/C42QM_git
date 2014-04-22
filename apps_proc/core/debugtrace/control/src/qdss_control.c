/*=============================================================================

FILE:         qdss_control.c

DESCRIPTION:  Implements handlers for diag comamnds to configure and control
              QDSS.

================================================================================
              Copyright (c) 2012, 2019, 2022 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_control_priv.h"
#include "qdss_control.h"
#include "qdss_diag.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "qdss_fuse_check.h"
#include "qdss_imem.h"
#include "qdss_tfunnel.h"
#include "qdss_tlmm.h"
#include "qdss_osal.h"
#include "DDITPDM.h"
#include "DDITPDA.h"
#include "DDITMC.h"

struct qdss_control qdss;

static __inline uint32 endian_swap32(uint32 x)
{
    return (x>>24) |
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_QUERY_STATUS diag command

  Memory for request and response packets is already allocated
  before calling this function. This applies for all diag command handlers below

  @param                 pReq [in ] :  request packet
  @param              req_len [in ] :  request packet length
  @param                 pRsp [in ] :  response packet
  @param              rsp_len [in ] :  response packet length

  @return 0 if successful, error code otherwise
 */
int qdss_query_status_handler(qdss_query_status_req *pReq,
                              int req_len,
                              qdss_query_status_rsp *pRsp,
                              int rsp_len)
{

   int nErr;

   TRY(nErr,qdss_control_get_sink(&pRsp->trace_sink));
   TRY(nErr,qdss_control_get_stm(&pRsp->stm_enabled));
   TRY(nErr,qdss_control_get_hwe(&pRsp->hw_events_enabled));

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_FILTER_ETM diag command

  @return 0 if successful, error code otherwise
 */

int qdss_filter_etm_handler(qdss_filter_etm_req *pReq,
                          int req_len,
                          qdss_filter_etm_rsp *pRsp,
                          int rsp_len)
{
   int nErr;

   TRY(nErr,qdss_control_set_etm(pReq->state));

   CATCH(nErr){}

   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_FILTER_STM diag command

  @return 0 if successful, error code otherwise
 */
int qdss_filter_stm_handler(qdss_filter_stm_req *pReq,
                          int req_len,
                          qdss_filter_stm_rsp *pRsp,
                          int rsp_len)
{
   int nErr;

   TRY(nErr,qdss_control_set_stm(pReq->state));

   CATCH(nErr){}
   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_FILTER_HWEVENTS_ENABLE diag command

  @return 0 if successful, error code otherwise
 */
int qdss_filter_hwevents_handler(qdss_filter_hwevents_req *pReq,
                          int req_len,
                          qdss_filter_hwevents_rsp *pRsp,
                          int rsp_len)

{
   int nErr;

   TRY(nErr,qdss_control_set_hwe(pReq->state));

   CATCH(nErr){}

   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_FILTER_HWEVENTS_CONFIGURE diag command

  @return 0 if successful, error code otherwise
 */
int qdss_filter_hwevents_configure_handler(qdss_filter_hwevents_configure_req *pReq,
                          int req_len,
                          qdss_filter_hwevents_configure_rsp *pRsp,
                          int rsp_len)

{
   int nErr;

   TRY(nErr,qdss_control_set_hwe_reg(pReq->register_addr,
                                     pReq->register_value));

   CATCH(nErr) {}
   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_map_cti_handler(qdss_map_cti_req *pReq,
                          int req_len,
                          qdss_map_cti_rsp *pRsp,
                          int rsp_len)
{
  int nErr = QDSS_CONTROL_UNSUPPORTED;


  /* Ensure string is null-terminated */
  pReq->block[CTI_BLOCK_SIZE - 1] = 0;

  TRY(nErr, qdss_control_map_cti(pReq->block,
                                    pReq->type,
                                    pReq->trigger,
                                    pReq->channel));

  CATCH(nErr) {}
  pRsp->result = nErr;
  return nErr;
}

int qdss_unmap_cti_handler(qdss_unmap_cti_req *pReq,
                          int req_len,
                          qdss_unmap_cti_rsp *pRsp,
                          int rsp_len)
{

  int nErr = QDSS_CONTROL_UNSUPPORTED;

  /* Ensure string is null-terminated */
  pReq->block[CTI_BLOCK_SIZE - 1] = 0;

  TRY(nErr, qdss_control_unmap_cti(pReq->block,
                                    pReq->type,
                                    pReq->trigger,
                                    pReq->channel));

  CATCH(nErr) {}
  pRsp->result = nErr;
  return nErr;
}

int qdss_reset_cti_handler(qdss_reset_cti_req *pReq,
                          int req_len,
                          qdss_reset_cti_rsp *pRsp,
                          int rsp_len)
{
  int nErr = QDSS_CONTROL_UNSUPPORTED;

  /* Ensure string is null-terminated */
  pReq->block[CTI_BLOCK_SIZE - 1] = 0;

  TRY(nErr, qdss_control_reset_cti(pReq->block));

  CATCH(nErr) {}
  pRsp->result = nErr;
  return nErr;
}

int qdss_query_cti_handler(qdss_query_cti_req *pReq,
                          int req_len,
                          qdss_query_cti_rsp *pRsp,
                          int rsp_len)
{
  int nErr = QDSS_CONTROL_UNSUPPORTED;

  /* Ensure string is null-terminated */
  pReq->block[CTI_BLOCK_SIZE - 1] = 0;

  TRY(nErr, qdss_control_query_cti(pReq->block,
                                    pReq->type,
                                    MAX_TRIGGERS,
                                    &pRsp->length,
                                    pRsp->mappings));

  CATCH(nErr) {}
  pRsp->result = nErr;
  return nErr;
}

int qdss_trigger_cti_handler(qdss_trigger_cti_req *pReq,
                          int req_len,
                          qdss_trigger_cti_rsp *pRsp,
                          int rsp_len)
{

  int nErr = QDSS_CONTROL_UNSUPPORTED;

  /* Ensure string is null-terminated */
  pReq->block[CTI_BLOCK_SIZE - 1] = 0;

  TRY(nErr, qdss_control_trigger_cti(pReq->block,
                                   pReq->channel,
                                   pReq->trigger_type));

  CATCH(nErr) {}
  pRsp->result = nErr;
  return nErr;
}


/*-------------------------------------------------------------------------*/
int qdss_filter_hwevents_set_detect_mask_handler(qdss_filter_hwevents_set_detect_mask_req *pReq,
                        int req_len,
                        qdss_filter_hwevents_set_detect_mask_rsp *pRsp,
                        int rsp_len)
{
   int nErr = QDSS_HWEVENT_UNKNOWN_ERR;
   TRY(nErr,qdss_control_set_hwe_detect_mask(pReq->hwe_mask));
   CATCH(nErr) {}
   pRsp->result=nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/
int qdss_filter_hwevents_set_trigger_mask_handler(qdss_filter_hwevents_set_trigger_mask_req *pReq,
                        int req_len,
                        qdss_filter_hwevents_set_trigger_mask_rsp *pRsp,
                        int rsp_len)
{
   int nErr = QDSS_HWEVENT_UNKNOWN_ERR;
   TRY(nErr,qdss_control_set_hwe_trigger_mask(pReq->hwe_mask));
   CATCH(nErr) {}
   pRsp->result=nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_CORRELATE_GPIO diag command.

  @return 0 if successful, error code otherwise
 */

int qdss_correlate_gpio_handler(qdss_correlate_gpio_req *pReq,
                          int req_len,
                          qdss_correlate_gpio_rsp *pRsp,
                          int rsp_len)
{
  int nErr = QDSS_CONTROL_UNSUPPORTED;

  if (FALSE == qdss.ctrlMutex_enable) {
    nErr = QDSS_CONTROL_BAD_STATE;
    pRsp->result = nErr;
    return nErr;
  }

  qdss_osal_mutex_lock(&qdss.ctrlMutex);
   
  TRY(nErr, qdss_tlmm_config_cti_gpio(pReq->gpio_option));
     
  CATCH(nErr) {}

  pRsp->result = nErr;
  qdss_osal_mutex_unlock(&qdss.ctrlMutex);

  return nErr;
}



/*-------------------------------------------------------------------------*/

/**
  @brief  Ensure QDSS HW is ready to be configure

 This function votes QDSS clocks on, attach to drivers and init hardware
 If this is already done this function returns success

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
 */
int qdss_ensure_hw_ready(void)
{
   int nErr;

   THROW_IF(nErr,(NULL == qdss.npa_client),QDSS_CONTROL_BAD_STATE);
   THROW_IF(nErr,(NULL == qdss.qdss_fuse_present),QDSS_CONTROL_FUSE_BLOWN);


   if ((NULL == qdss.hTMC)||(NULL == qdss.hFunnel)||(NULL == qdss.hSTMCfg)||
      (NULL == qdss.hHWEvent)||(NULL == qdss.hCTI)||(NULL == qdss.hTPDM)||(NULL == qdss.hTPDA)) {

      npa_issue_required_request(qdss.npa_client, CLOCK_QDSS_LEVEL_DEBUG);

      if (NULL == qdss.hTMC) {
         THROW_IF(nErr,DAL_SUCCESS !=
                  DAL_TMCDeviceAttach("DALDEVICEID_TMC",&qdss.hTMC),
                  QDSS_CONTROL_BAD_STATE);
         qdss.current_trace_sink = TMC_TRACESINK_ETB;
      }

      TRY(nErr,qdss_trace_buffer_init());

      if (NULL == qdss.hFunnel) {
         THROW_IF(nErr,DAL_SUCCESS !=
                  DAL_TFunnelDeviceAttach(DALDEVICEID_TFUNNEL_0,&qdss.hFunnel),
                  QDSS_CONTROL_BAD_STATE);
      }

      if (NULL == qdss.hSTMCfg) {
         THROW_IF(nErr,DAL_SUCCESS !=
                  DAL_STMCfgDeviceAttach(DALDEVICEID_STM_CONFIG,&qdss.hSTMCfg),
                  QDSS_CONTROL_BAD_STATE);
         //Keep the STM disabled by default
         THROW_IF(nErr,DAL_SUCCESS !=
                  DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,0),
                  QDSS_CONTROL_BAD_STATE);
      }
/*Attach to the driver but do not program default register set unless commanded*/
      if (NULL == qdss.hHWEvent) {
         THROW_IF(nErr,DAL_SUCCESS !=
                  DAL_HWEventDeviceAttach("DALDEVICEID_HWEVENT",&qdss.hHWEvent),
                  QDSS_CONTROL_BAD_STATE);
      }

      if (NULL == qdss.hCTI) {
         THROW_IF(nErr, DAL_SUCCESS !=
                  DAL_CTIDeviceAttach("DALDEVICEID_CTI", &qdss.hCTI),
                  QDSS_CONTROL_BAD_STATE);
         
         /* Enables Save/Restore logic across APPS power collaspe */
         #if 0
		 L4_MSM_CTI_ENABLE();
		 #endif
      }

	   if (NULL == qdss.hTPDM) {
         THROW_IF(nErr, DAL_SUCCESS !=
                  DAL_TPDMDeviceAttach("DALDEVICEID_TPDM",&qdss.hTPDM),
                  QDSS_CONTROL_BAD_STATE);
      }

      if (NULL == qdss.hTPDA) {
         THROW_IF(nErr, DAL_SUCCESS !=
                  DAL_TPDADeviceAttach("DALDEVICEID_TPDA",&qdss.hTPDA),
                  QDSS_CONTROL_BAD_STATE);
      }

   }

   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr){}

   return nErr;
}

/*-------------------------------------------------------------------------*/

#define QDSS_INVALID_TRACE_SINK  0xFF

/**
  @brief Initializes the QDSS control subsystem.

  This is called from RC init. QDSS HW is not touched or clocked.
  That happens when explicitly turned on by DIAG commands

  @return None
 */
void qdss_control_init(void)
{
   int nErr = QDSS_CONTROL_ERROR;
   qdss_diag_init();
   qdss.hTracer = 0;
   qdss.sio.handle = SIO_NO_STREAM_ID;
   qdss.hTMC = 0;
   qdss.hFunnel = 0;
   qdss.hSTMCfg = 0;
   qdss.hCTI = 0;
   qdss.hTPDM = 0;
   qdss.hTPDA = 0;
   qdss.ctrlMutex_enable = FALSE;
   qdss.npa_client = 0;
   qdss.current_trace_sink = QDSS_INVALID_TRACE_SINK;
   qdss.bTraceSinkEnabled = FALSE;
   qdss.etr.byte_counter = QDSS_ETR_BYTE_COUNTER_DEFAULT;
   qdss.etr.timer_us = QDSS_ETR_TIMER_DEFAULT;
   qdss_osal_mutex_init(&qdss.ctrlMutex);
   qdss.ctrlMutex_enable = TRUE;
   
   qdss.qdss_fuse_present = (uint32)qdss_fuse_trace_access();
   if (qdss.qdss_fuse_present)
   {
      qdss.npa_client = npa_create_sync_client("/clk/qdss", "qdss", NPA_CLIENT_REQUIRED);
   }
   if ((QDSS_SYSTEST_HWEVENTS_ENABLE|QDSS_SYSTEST_RPM_INIT) &
        qdss_get_systest_ctl(qdss_imem_base()))
   {
     /*RPM will setup STM,funnel and set sink to ETB*/
      if(qdss.npa_client!=NULL)
      {
         npa_issue_required_request(qdss.npa_client, CLOCK_QDSS_LEVEL_DEBUG);
      }
      TRY(nErr,DAL_HWEventDeviceAttach("DALDEVICEID_HWEVENT",&qdss.hHWEvent));
      TRY(nErr,DAL_HWEvent_ConfigurePreset(qdss.hHWEvent));
   }

    /* The flag ENABLE_ETM_AT_BOOTUP needs to be defined at target or image level and not in QDSS scons.
       Code under this flag sets DDR as trace sink and configures funnel for MPSS ETM.
       Only these changes won't enable MPSS ETM at bootup, some client in MPSS needs to call the 
       API qurt_etm_enable at bootup to ensure that MPSS ETM gets enabled at bootup.*/
    #ifdef ENABLE_ETM_AT_BOOTUP
      TRY(nErr,qdss_control_set_sink(TMC_TRACESINK_RAM)); // Setting DDR as trace sink
      TRY(nErr,qdss_tfunnel_enable_port(TFUNNEL_MPSS_PORT)); // Configuring funnel for MPSS ETM
    #endif

   CATCH(nErr) {
   }
   IGNORE(nErr);
}

/* Below are qdss control interface functions. Definitions of these
 * are in qdss_contro.h
 */

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_get_stm(uint8 *state)
{
   uint32 uStatus = 0;
   int nErr;

   if (FALSE==qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMStatus(qdss.hSTMCfg,STM_EN,&uStatus),
            QDSS_CONTROL_DEVICE_ERROR);

   *state =  (uStatus)?1:0;

   CATCH(nErr){}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_set_stm(uint8 state)
{
   int nErr;

   if (FALSE==qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,state),
            QDSS_CONTROL_DEVICE_ERROR);

   CATCH(nErr){}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_get_hwe(uint8 *state)
{
   uint32 uStatus = 0;
   int nErr;

   if (FALSE==qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMStatus(qdss.hSTMCfg,STM_HW_EVT_EN,&uStatus),
            QDSS_CONTROL_DEVICE_ERROR);

   *state =  (uStatus)?1:0;

   CATCH(nErr){}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}


/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_DEVICE_ERROR = STM driver error.
*/
int qdss_control_set_hwe(uint8 state)
{
  int nErr;

   if (FALSE==qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_HW_EVT_EN,state),
            QDSS_CONTROL_ERROR);

   /*Enable default pre-compiled HW events if state==1*/
   if (state==0x1)
   {
      THROW_IF(nErr,DAL_SUCCESS !=
               DAL_HWEvent_ConfigurePreset(qdss.hHWEvent),
               QDSS_CONTROL_DEVICE_ERROR);
   }

   CATCH(nErr){}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}
/*-------------------------------------------------------------------------*/
int qdss_control_get_etm(uint8 *state)
{
   *state = qdss.etm_state;
   return QDSS_CONTROL_SUCCESS;
}

void err_fatal_handler ( void );
void _qdss_etm_enable(void)
{
   #if 0
   void *stop_addr=(void *)err_fatal_handler;   
   L4_MSM_ETM_ENABLE(stop_addr);
   #endif
}

void _qdss_etm_disable(void)
{
   #if 0
   L4_MSM_ETM_DISABLE();
   #endif
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_ERROR = Failed.
*/
int qdss_control_set_etm(uint8 state)
{
   int nErr;

   if (FALSE==qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   if (state) {
      TRY(nErr,qdss_tfunnel_enable_port(TFUNNEL_APPS_ETM_PORT));
      _qdss_etm_enable();
   }
   else {
      _qdss_etm_disable();
      TRY(nErr,qdss_tfunnel_disable_port(TFUNNEL_APPS_ETM_PORT));
   }

   nErr = QDSS_CONTROL_SUCCESS;
   qdss.etm_state = state;

   CATCH(nErr){}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_PARAM = Invalid address.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_ERROR = Failed.
*/
int qdss_control_set_hwe_reg(unsigned long addr, unsigned long val)
{
   int nErr;
   uint8 ret_code =(uint8) QDSS_HWEVENT_UNKNOWN_ERR;;

   if (FALSE==qdss.ctrlMutex_enable) {
      return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);

   TRY(nErr,qdss_ensure_hw_ready());

   TRY(nErr,DAL_HWEvent_SetRegister(qdss.hHWEvent, addr, val,
                                    &ret_code));

   nErr = QDSS_HWEVENT_CONFIG_SUCCESS;
   CATCH(nErr) {}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return (QDSS_HWEVENT_CONFIG_SUCCESS == ret_code) ? QDSS_CONTROL_SUCCESS :
          (QDSS_HWEVENT_FUSE_CHECK_ERR == ret_code) ? QDSS_CONTROL_FUSE_BLOWN :
          (QDSS_HWEVENT_ADDR_CHECK_ERR == ret_code) ? QDSS_CONTROL_BAD_PARAM :
                                                      QDSS_CONTROL_ERROR;
}

/*-------------------------------------------------------------------------*/
int qdss_control_set_hwe_detect_mask(uint32 bit_mask)
{
   int nErr;
   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr, DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_HW_EVT_ENABLE_MASK,bit_mask),
            QDSS_CONTROL_ERROR);
   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr) {}
   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/
int qdss_control_set_hwe_trigger_mask(uint32 bit_mask)
{
   int nErr;
   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr, DAL_SUCCESS !=
            DalSTMCfg_STMControl(qdss.hSTMCfg,STM_HW_EVT_TRIGGER_MASK,bit_mask),
            QDSS_CONTROL_ERROR);
   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr) {}
   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_control_map_cti(const char *block,
                            uint8 type,
                            uint8 trigger,
                            uint8 channel)
{
   int nErr = QDSS_CONTROL_UNSUPPORTED;

   if (FALSE == qdss.ctrlMutex_enable) {
       return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr, qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DALCTI_Map(qdss.hCTI, block, type, trigger, channel),
            QDSS_CONTROL_ERROR);

   CATCH(nErr) {}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_control_unmap_cti(const char *block,
                            uint8 type,
                            uint8 trigger,
                            uint8 channel)
{
   int nErr;

   if (FALSE == qdss.ctrlMutex_enable) {
       return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr, qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DALCTI_Unmap(qdss.hCTI, block, type, trigger, channel),
            QDSS_CONTROL_ERROR);

   CATCH(nErr) {}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_control_reset_cti(const char *block)
{
   int nErr;

   if (FALSE == qdss.ctrlMutex_enable) {
       return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr, qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DALCTI_Reset(qdss.hCTI, block),
            QDSS_CONTROL_ERROR);

   CATCH(nErr) {}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_control_query_cti(const char *block,
                            uint8 type,
                            uint8 max_triggers,
                            uint8 *length,
                            __packed uint32 *mappings)
{
   int nErr;
   uint32 i;

   if (FALSE == qdss.ctrlMutex_enable) {
       return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr, qdss_ensure_hw_ready());

   memset((void *) mappings, 0x0, MAX_TRIGGERS * sizeof(uint32));

   THROW_IF(nErr,DAL_SUCCESS !=
            DALCTI_Query(qdss.hCTI,
                         block,
                         type,
                         max_triggers,
                         length,
                         mappings),
            QDSS_CONTROL_ERROR);

   /* Response expects each uint32 to be big-endian. */
   for (i = 0; i < *length; i++) {
       mappings[i] = endian_swap32(mappings[i]);
   }

   CATCH(nErr) {}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}


int qdss_control_trigger_cti(const char *block,
                           uint8 channel,
                           uint8 trigger_type)
{
   int nErr;

   if (FALSE == qdss.ctrlMutex_enable) {
       return QDSS_CONTROL_BAD_STATE;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr, qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DALCTI_Trigger(qdss.hCTI, block, channel,trigger_type),
            QDSS_CONTROL_ERROR);

   CATCH(nErr) {}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}
