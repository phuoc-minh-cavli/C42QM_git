#ifndef QDSS_CONTROL_PRIV_H
#define QDSS_CONTROL_PRIV_H

/*=============================================================================

FILE:         qdss_control_priv.h

DESCRIPTION:  Private data structures used by the qdss control subsystem

================================================================================
              Copyright (c) 2013, 2019 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "DDISTMCfg.h"
#include "DDITFunnel.h"
#include "DDITMC.h"
#include "DDIHWEvent.h"
#include "DDICTI.h"
#include "DDIInterruptController.h"
#include "DDITimer.h"
#include "qdss_bam.h"
#include "tracer.h"
#include "npa.h"
#include "sio.h"
#include "qdss_control_config.h"
#include "qdss_throw.h"
#include "qdss_utils.h"
#include "qdss_diag.h"
#include "qdss_trace_buffer.h"
#include "qdss_dump.h"
#include "qurt_mutex.h"

typedef struct QDSSEventObj  QDSSEventObj;
struct QDSSEventObj
{
   unsigned long long _bSpace[64/sizeof(unsigned long long)];
};


struct qdss_sio {
   sio_stream_id_type  handle;
   sio_open_type       open_props;
   sio_open_param_type open_param;
   dsm_watermark_type tx_wm_q;
   q_type  tx_q;
};

struct qdss_etr {
   DALSYSWorkLoopHandle hWorkLoop;
   DALSYSEventHandle  hWorkloopEvent;
   DALDEVICEHANDLE    hTimer;
   DALSYSEventHandle  hTimerEvent;
   QDSSEventObj     hTimerObject;

   uint32 buf_start_addr;
   uint32  buf_end_addr;
   uint32  sio_tx_ptr; //next block to send
   uint32  num_blocks_to_send;
   uint32  byte_counter;
   uint32  timer_us;

   //statistics
   uint32 interrupt_count;
   uint32 timer_callbacks;
   uint32 event_handler_calls;
   uint32 buffer_overflows;
   uint32 no_dsm_bufs;
   uint32 chunks_sent;
   uint32 write_ptr_errs;
};

struct qdss_trace_buffer {
   boolean initialized;
   uint32 debug_dmp_phy_addr;
   dump_data_type * etb_regs_dump;
   uint32 etb_regs_dump_start;
   dump_data_type * etr_regs_dump;
   uint32 etr_regs_dump_start;
   dump_data_type * etb_dump;
   uint32 etb_dump_start;
   uint32 etr_phys_base;
   uint32 etr_buf_size;
};


struct qdss_control {
   npa_client_handle npa_client;
   qurt_mutex_t	ctrlMutex;
   boolean 		ctrlMutex_enable;
   DALDEVICEHANDLE hSTMCfg;
   DALDEVICEHANDLE hFunnel;
   DALDEVICEHANDLE hTMC;
   DALDEVICEHANDLE hInterruptCtl;
   DALDEVICEHANDLE hHWEvent;
   DALDEVICEHANDLE hCTI;
   DALDEVICEHANDLE hTPDM;
   DALDEVICEHANDLE hTPDA;
   uint8 current_trace_sink;
   boolean bTraceSinkEnabled;
   tracer_client_handle_t hTracer;
   uint32 test_data_count;
   struct qdss_sio sio;
   struct qdss_etr etr;
   struct qdss_trace_buffer buf;
   uint32 qdss_fuse_present;
   uint8 etm_state;
};

extern struct qdss_control qdss;

int qdss_ensure_minimal_hw_ready(void);
int qdss_ensure_hw_ready(void);
int qdss_trace_sink_enable(void);
int qdss_trace_sink_disable(void);

#define TRACESINK_PCIE   8
boolean qdss_trace_sink_is_buffered(uint8 trace_sink);

#endif //QDSS_CONTROL_PRIV_H
