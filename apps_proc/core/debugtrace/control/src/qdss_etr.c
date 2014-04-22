/*=============================================================================

FILE:         qdss_etr.c

DESCRIPTION:  This file contains the ETR configuration as well as
              ETR interrupt and timer routines for USB buffered mode

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_etr.h"
#include "qdss_control_priv.h"
#include "qdss_control.h"
#include "qdss_sio.h"
#include "qdss_osal.h"

#if 0
#define QDSS_ATOMIC_INC(_pVal)         okl4_atomic_inc((okl4_atomic_word_t*)_pVal)
#define QDSS_ATOMIC_SET(_pVal, _iVal)  okl4_atomic_set((okl4_atomic_word_t*)_pVal,_iVal)
#endif
/*-------------------------------------------------------------------------*/

/**
  @brief  Configures the ETR trace buffer

  @param   buf_base_phys_addr [in ] :  Start physical addres of the buffer
  @param    buf_size_in_bytes [in ] :  Size of the buffer in bytes

  @return 0 if successful, error code otherwise
 */
int qdss_etr_configure_buffer(uint32 buf_base_phys_addr, uint32 buf_size_in_bytes)
{
   int nErr;
   uint32 buf_size_in_words = buf_size_in_bytes >> 2; //in 32 bit words

   TRY(nErr,DalTMC_SetTraceBuffer(qdss.hTMC,
                                  buf_base_phys_addr,
                                  buf_size_in_words));

   qdss.etr.buf_start_addr = buf_base_phys_addr;
   qdss.etr.sio_tx_ptr = buf_base_phys_addr;
   qdss.etr.buf_end_addr  = buf_base_phys_addr + buf_size_in_bytes ;

   CATCH(nErr){}
   return nErr;
}

#if 0
/*-------------------------------------------------------------------------*/

/**
  @brief ETR interrupt service routine

  @param                pCtxt [in ] :  qdss root structure.

  @return NULL
 */
static void* qdss_etr_isr(void *pCtxt)
{
   int nErr = 0;

   QDSS_ATOMIC_INC(&qdss.etr.num_blocks_to_send);
   qdss.etr.interrupt_count++;
   TRY(nErr,DALSYS_EventCtrl( qdss.etr.hWorkloopEvent,DALSYS_EVENT_CTRL_TRIGGER ));
   CATCH(nErr) {}

   IGNORE(nErr);
   return NULL;
}
#endif

/*-------------------------------------------------------------------------*/

/**
  @brief  QDSS ETR timer call back

  @param                pCtxt [in ] :  qdss root data structure.

  @return NULL
 */
#if 0 
void* qdss_etr_timer_callback(void *pCtxt,
                              uint32 unused1,void* unused2,uint32 unused3)
{
   int nErr  =0;
   qdss.etr.timer_callbacks++;

   qdss_osal_mutex_lock(&qdss.ctrlMutex);

   if (!qdss_trace_sink_is_buffered(qdss.current_trace_sink)) {
      THROW(nErr,QDSS_CONTROL_BAD_STATE)
   }

   nErr = DalTMC_Flush(qdss.hTMC,TMC_FLUSH_NOBLOCK);
   
   #if 0
   TRY(nErr,DALSYS_EventCtrl( qdss.etr.hWorkloopEvent,DALSYS_EVENT_CTRL_TRIGGER ));
   #endif

   CATCH(nErr) {}
   qdss_osal_mutex_unlock(&qdss.ctrlMutex);

   IGNORE(nErr);
   return NULL;

}
#endif


#if 0
typedef enum
{
  QDSS_ETR_INTERRUPT_EVENT = 0,
  QDSS_WORKLOOP_MAX_EVENTS = 1
} qdss_workloop_event_type;

#define   QDSS_WORKLOOP_PRIORITY    0   // Use the default workloop priority
#endif

#define NUM_ETR_WRITE_PTRS  256
uint32 qdss_etr[NUM_ETR_WRITE_PTRS];

uint32 qdss_etr_index=0;

#define SIO_ALIGNMENT_MASK  (~0xF) //Make 16 byte aligned

/*-------------------------------------------------------------------------*/

/**
  @brief    ETR event handler

  This is the real work function. Both the isr and timer cb trigger
  this fucnction. It reads ETR write pointer to determine the amount of
  data to sent.

  @param            evtHandle [in ] :
  @param                 ctxt [in ] :

  @return 0 if successful, error code otherwise
 */
#if 0
DALResult  qdss_etr_event_handler( DALSYSEventHandle evtHandle, void* ctxt)
{
   int nErr;
   uint32 etr_write_ptr = 0;
   uint32 data_size = 0;


   qdss.etr.event_handler_calls++;

   if (SIO_NO_STREAM_ID == qdss.sio.handle) {
      THROW(nErr, DAL_ERROR);
   }

   if (!qdss.bTraceSinkEnabled) {
      //Could happen if DTR is disabled and we got a call back
      THROW(nErr, DAL_ERROR);
   }


   TRY(nErr,DalTMC_GetParam(qdss.hTMC,TMC_ETR_WRITE_PTR,&etr_write_ptr));

   qdss_etr[qdss_etr_index]=etr_write_ptr;
   qdss_etr_index++;
   if (qdss_etr_index >= NUM_ETR_WRITE_PTRS) {
      qdss_etr_index = 0;
   }


   if ( (etr_write_ptr < qdss.etr.buf_start_addr  ) || (etr_write_ptr > qdss.etr.buf_end_addr)) {
      qdss.etr.write_ptr_errs++;
      THROW(nErr, DAL_ERROR);
   }

   if (etr_write_ptr == qdss.etr.sio_tx_ptr) {
      //no data to sent.
      //TODO - Could also happen if we wrapped around exactly once
      return DAL_SUCCESS;
   }

   if (etr_write_ptr > qdss.etr.sio_tx_ptr) {
      data_size = etr_write_ptr - qdss.etr.sio_tx_ptr;
   }
   else {
      data_size = (qdss.etr.buf_end_addr -  qdss.etr.sio_tx_ptr ) +
         (etr_write_ptr - qdss.etr.buf_start_addr);
   }

   data_size &= SIO_ALIGNMENT_MASK;

   TRY(nErr,qdss_sio_send_data(data_size));

   #if 0
   QDSS_ATOMIC_SET(&qdss.etr.num_blocks_to_send,0);
   #endif

   nErr = DAL_SUCCESS;

   CATCH(nErr) {}
   return nErr;
}
#endif

#if 0
/*-------------------------------------------------------------------------*/

/**
  @brief Initializes ETR the workloop (or worker thread).

  @return 0 if successful, error code otherwise
 */
int qdss_etr_init_workloop(void)
{
   int nErr = 0;

   if (NULL != qdss.etr.hWorkloopEvent) {
      return nErr; // already initialized
   }


   TRY(nErr,DALSYS_RegisterWorkLoop( QDSS_WORKLOOP_PRIORITY,
                                     QDSS_WORKLOOP_MAX_EVENTS,
                                     &qdss.etr.hWorkLoop,
                                     NULL ));

   TRY(nErr,DALSYS_EventCreate( DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                &qdss.etr.hWorkloopEvent,
                                NULL ));

   TRY(nErr,DALSYS_AddEventToWorkLoop( qdss.etr.hWorkLoop,
                                       qdss_etr_event_handler,
                                       &qdss,
                                       qdss.etr.hWorkloopEvent,
                                       NULL ));

    CATCH(nErr) {}
    return nErr;
}
#endif
/*-------------------------------------------------------------------------*/

/**
  @brief Starts the ETR timer

  @return 0 if successful, error code otherwise
 */
int qdss_etr_start_timer(void)
{
   int nErr = 0;

   if (NULL == qdss.etr.hTimerEvent) {
      TRY(nErr,DAL_TimerDeviceAttach(DALDEVICEID_TIMER, &qdss.etr.hTimer));
	  
	  #if 0
      TRY(nErr,DALSYS_EventCreate
          (DALSYS_EVENT_ATTR_CALLBACK_EVENT | DALSYS_EVENT_ATTR_TIMER_EVENT,
           &qdss.etr.hTimerEvent,
           &qdss.etr.hTimerObject));

      TRY(nErr, DALSYS_SetupCallbackEvent
          (qdss.etr.hTimerEvent,
           qdss_etr_timer_callback, (DALSYSCallbackFuncCtx) &qdss));
	  #endif
   }

   if (qdss.etr.timer_us > 0) {
      TRY(nErr,DalTimer_Register(qdss.etr.hTimer,qdss.etr.hTimerEvent, qdss.etr.timer_us ));
   }


   CATCH(nErr) {}

   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Stops the ETR timer


  @return 0 if successful, error code otherwise
 */
int qdss_etr_stop_timer(void)
{
   int nErr = 0;

   if ((NULL != qdss.etr.hTimer)  &&  (NULL != qdss.etr.hTimerEvent))  {
      TRY(nErr,DalTimer_UnRegister(qdss.etr.hTimer,qdss.etr.hTimerEvent));
   }

   CATCH(nErr) {}

   return nErr;


}

#if 0
#define QDSS_ETR_BYTECTR_INTERRUPT_ID  198

/*-------------------------------------------------------------------------*/

/**
  @brief Registers the ETR ISR


  @return 0 if successful, error code otherwise
 */
int qdss_etr_register_isr(void)
{
   int nErr = 0 ;

   if (NULL != qdss.hInterruptCtl) {
      return nErr; //already intialized
   }

   TRY(nErr,
       DAL_InterruptControllerDeviceAttach
       (DALDEVICEID_INTERRUPTCONTROLLER,
        &qdss.hInterruptCtl));

   TRY(nErr,DalInterruptController_RegisterISR
       (qdss.hInterruptCtl,
        QDSS_ETR_BYTECTR_INTERRUPT_ID,
        qdss_etr_isr,
        (DALISRCtx)&qdss,
        DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER));


   CATCH(nErr) {}
   return nErr;
}
#endif
