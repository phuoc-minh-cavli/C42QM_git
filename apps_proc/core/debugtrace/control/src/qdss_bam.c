/*=============================================================================

FILE:         qdss_bam.c

DESCRIPTION:

================================================================================
              Copyright (c) 2012 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "stringl/stringl.h"

#include "bam.h"
#include "qdss_throw.h"
#include "qdss_utils.h"
#include "qdss_bam_config.h"
#include "qdss_usb_bam.h"


struct qdss_bam_struct {
   bam_config_type cfg;
   void * mem_handle;
   QDSSMemInfo   mem_info;
   bam_handle handle;
   bam_callback_type  isr;
   bam_pipe_config_type pipe_cfg;
   bam_handle pipe_handle;
   void * fifo_handle;
   QDSSMemInfo fifo_info;
};

static struct qdss_bam_struct qdss_bam;

void qdss_bam_isr_callback(bam_result_type data)
{
}


int qdss_bam_init(void)
{
   int nErr = BAM_FAILED;

   if (0!=qdss_bam.handle) {
      //already initialized
      return BAM_SUCCESS;
   }

   qdss_bam.cfg.bam_va         = 0;
   qdss_bam.cfg.bam_pa         = QDSS_BAM_BASE_PHYS_ADDR;
   qdss_bam.cfg.bam_irq        = 0;
   qdss_bam.cfg.sum_thresh     = 0x10;
   qdss_bam.cfg.bam_irq_mask   = 0;
   qdss_bam.cfg.bam_mti_irq_pa = 0;
   qdss_bam.cfg.options        = 0;
   qdss_bam.isr.func = qdss_bam_isr_callback;
   qdss_bam.isr.data = &qdss_bam;

   TRY(nErr,qdss_dal_memmap(QDSS_BAM_BASE_PHYS_ADDR,
                            QDSS_BAM_SIZE,
                            0,
                            &qdss_bam.mem_handle,
                            &qdss_bam.mem_info));

   qdss_bam.cfg.bam_va = qdss_bam.mem_info.VirtualAddr;

   qdss_bam.handle = bam_init(&qdss_bam.cfg,&qdss_bam.isr);
   if (0 == qdss_bam.handle)
      {
         THROW(nErr,BAM_FAILED);
      }

   CATCH(nErr) {}

   return BAM_SUCCESS;
}


int qdss_bam_deinit(void)
{
   int nErr = BAM_FAILED;;
#define DISABLE_BAM 1


   if (0!=qdss_bam.pipe_handle) {
      TRY(nErr,bam_pipe_deinit(qdss_bam.pipe_handle));
      qdss_bam.pipe_handle = 0;
   }

   if (0!=qdss_bam.handle) {
      TRY(nErr,bam_deinit(qdss_bam.handle, DISABLE_BAM ));
      qdss_bam.handle = 0;
   }

   nErr = BAM_SUCCESS;

   CATCH(nErr) {}

   return nErr;
}

int qdss_bam_reset(void)
{
  int nErr = BAM_FAILED;

  if (0 != qdss_bam.handle) {
     TRY(nErr,bam_reset(qdss_bam.handle));
  }
  nErr = BAM_SUCCESS;

  CATCH(nErr) {}

  return nErr;


}

int qdss_bam_pipe_disable(void)
{
   int nErr = BAM_FAILED;

   if (0!=qdss_bam.pipe_handle) {
      TRY(nErr,bam_pipe_disable(qdss_bam.pipe_handle));
   }

   CATCH(nErr) {}
   return nErr;
}


int qdss_bam_pipe_enable(void)
{
   int nErr = BAM_FAILED;

   if (0!=qdss_bam.pipe_handle) {
      TRY(nErr,bam_pipe_enable(qdss_bam.pipe_handle));
   }

   CATCH(nErr) {}
   return nErr;
}



int qdss_bam_pipe_setup(bam_pipe_config_type *qdss_pipe_config_ptr,
                        bam_handle *qdss_pipe_handle_ptr,
                        uint32 *qdss_pipe_num_ptr)
{
   int nErr;

   if (0!=qdss_bam.pipe_handle) {
      TRY(nErr,bam_pipe_deinit(qdss_bam.pipe_handle));
      qdss_bam.pipe_handle = 0;
   }

   //copy parameters provided by USB driver
   memscpy(&qdss_bam.pipe_cfg, sizeof(qdss_bam.pipe_cfg),
           qdss_pipe_config_ptr, sizeof(bam_pipe_config_type));

   //Override these
   qdss_bam.pipe_cfg.options       = 0;
   qdss_bam.pipe_cfg.dir           = BAM_DIR_PRODUCER;
   qdss_bam.pipe_cfg.mode          = BAM_MODE_BAM2BAM;
   qdss_bam.pipe_cfg.evt_thresh    = 4;


   qdss_bam.pipe_handle = bam_pipe_init(qdss_bam.handle,
                                        QDSS_BAM_PIPE_NUM,
                                        &qdss_bam.pipe_cfg,
                                        NULL);

   if (0 == qdss_bam.pipe_handle) {
      THROW(nErr,BAM_FAILED);
   }

   *qdss_pipe_handle_ptr =  qdss_bam.pipe_handle;
   *qdss_pipe_num_ptr = QDSS_BAM_PIPE_NUM;
   nErr = BAM_SUCCESS;

   CATCH(nErr) {}

   return nErr;
}


