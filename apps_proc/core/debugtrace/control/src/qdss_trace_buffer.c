/*=============================================================================

FILE:         qdss_trace_buffer.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014, 2019 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "qdss_trace_buffer.h"
#include "qdss_control_priv.h"
#include "qdss_imem.h"
#include "err.h"
#include "qdss_dump.h"

/*-------------------------------------------------------------------------*/

/**
  @brief  Initializes the Dump Table for SDI

  @return 0 if successful, error code otherwise
 */

int qdss_dump_table_init(void) 
{
   int nErr = DAL_ERROR;
   dump_entry_type dump_entry;

   dump_entry.id = MDM_TMC_DUMP;
   dump_entry.type = MDM_DUMP_TYPE_DATA;
   dump_entry.start_addr = qdss.buf.debug_dmp_phy_addr + QDSS_BUF_ETR_REGS_OFFSET;

   nErr = qdss_debug_dump_add_dump_entry (&dump_entry);
   if (nErr != DAL_SUCCESS) {
      return nErr;
   }

   dump_entry.id = MDM_TMC_2_DUMP;
   dump_entry.type = MDM_DUMP_TYPE_DATA;
   dump_entry.start_addr = qdss.buf.debug_dmp_phy_addr + QDSS_BUF_ETB_REGS_OFFSET;

   nErr = qdss_debug_dump_add_dump_entry (&dump_entry);
   if (nErr != DAL_SUCCESS) {
      return nErr;
   }

   dump_entry.id = MDM_ETB_DUMP;
   dump_entry.type = MDM_DUMP_TYPE_DATA;
   dump_entry.start_addr = qdss.buf.debug_dmp_phy_addr + QDSS_BUF_ETB_DATA_OFFSET;

   nErr = qdss_debug_dump_add_dump_entry (&dump_entry);
   
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief  QDSS fatal error handler callback

This function ETB and ETR registers to DDR and if the configured 
trace sink is ETB, it reads the ETB data to DDR.
  
  @return None
 */

void qdss_error_callback(void) 
{
   int nErr;
   uint8 trace_sink=0xFF;
   uint32 tmp;

   if ((0 == qdss.hTMC) || (FALSE==qdss.buf.initialized)) {
      //QDSS HW or trace buffer is not initialized
      //This check would also ensure qdss clocks are ON.
      return;
   }

   TRY(nErr,DalTMC_GetMode(qdss.hTMC,&trace_sink));

   if ( (TMC_TRACESINK_ETB != trace_sink) &&
        (TMC_TRACESINK_RAM != trace_sink)) {
      return;
   }

   DalTMC_Flush(qdss.hTMC,TMC_FLUSH_STOP);

   if (qdss.buf.etb_regs_dump->magic != SYSDBG_MAGIC_NUMBER) {
      DalTMC_SaveETBRegisters(qdss.hTMC, (byte*)qdss.buf.etb_regs_dump_start,
                              QDSS_TMC_REGS_PAGE_SIZE);

      qdss.buf.etb_regs_dump->magic= SYSDBG_MAGIC_NUMBER;
   }

   if (qdss.buf.etr_regs_dump->magic != SYSDBG_MAGIC_NUMBER) {
      DalTMC_SaveETRRegisters(qdss.hTMC, (byte*)qdss.buf.etr_regs_dump_start,
                              QDSS_TMC_REGS_PAGE_SIZE);

      qdss.buf.etr_regs_dump->magic= SYSDBG_MAGIC_NUMBER;
   }

   if (TMC_TRACESINK_ETB == trace_sink) {

      if (qdss.buf.etb_dump->magic != SYSDBG_MAGIC_NUMBER) {
         DalTMC_ReadTrace(qdss.hTMC, (byte*)qdss.buf.etb_dump_start,
                          qdss.buf.etb_dump->len, &tmp);
         qdss.buf.etb_dump->len = tmp;
         qdss.buf.etb_dump->magic= SYSDBG_MAGIC_NUMBER;
      }
   }

   CATCH(nErr) {}
   IGNORE(nErr);
}

static DALResult qdss_dump_buffer_base_init (void)
{
   DALResult result;
   uint32 debug_trace_buffer;
   uint32 debug_trace_buffer_size;
   dump_data_type *hdr;
   uint32 offset;

   /* Get debug buffer base */
   result = qdss_debug_dump_get_dump_buffer_base (&qdss.buf.debug_dmp_phy_addr,
                                                  &debug_trace_buffer,
                                                  &debug_trace_buffer_size);
   if (result != DAL_SUCCESS)
   {
     return result;
   }
   qdss.buf.etb_regs_dump = (dump_data_type *) (debug_trace_buffer +
                                                QDSS_BUF_ETB_REGS_OFFSET);
   qdss.buf.etr_regs_dump = (dump_data_type *) (debug_trace_buffer +
                                                QDSS_BUF_ETR_REGS_OFFSET);
   qdss.buf.etb_dump = (dump_data_type *) (debug_trace_buffer +
                                           QDSS_BUF_ETB_DATA_OFFSET);
   qdss.buf.etr_phys_base = qdss.buf.debug_dmp_phy_addr +
                            QDSS_BUF_ETR_DATA_OFFSET;
   qdss.buf.etr_buf_size = debug_trace_buffer_size - QDSS_BUF_ETR_DATA_OFFSET;
  
   /* Set up dump_data_type headers for SDI */
  
   hdr = (dump_data_type *) qdss.buf.etb_regs_dump;
   offset = QDSS_BUF_ETB_REGS_OFFSET + QDSS_DUMP_DATA_TYPE_HEADER_LEN;
   hdr->magic = 0;
   hdr->start_addr = qdss.buf.debug_dmp_phy_addr + offset;
   qdss.buf.etb_regs_dump_start = debug_trace_buffer + offset;
   hdr->len = QDSS_BUF_ETR_REGS_OFFSET - offset;

   hdr = (dump_data_type *) qdss.buf.etr_regs_dump;
   offset = QDSS_BUF_ETR_REGS_OFFSET + QDSS_DUMP_DATA_TYPE_HEADER_LEN;
   hdr->magic = 0;
   hdr->start_addr = qdss.buf.debug_dmp_phy_addr + offset;
   qdss.buf.etr_regs_dump_start = debug_trace_buffer + offset;
   hdr->len = QDSS_BUF_ETB_DATA_OFFSET - offset;

   hdr = (dump_data_type *) qdss.buf.etb_dump;
   hdr->magic = 0;
   offset = QDSS_BUF_ETB_DATA_OFFSET + QDSS_DUMP_DATA_TYPE_HEADER_LEN;
   hdr->start_addr = qdss.buf.debug_dmp_phy_addr + offset;
   qdss.buf.etb_dump_start = debug_trace_buffer + offset;
   hdr->len = QDSS_BUF_DCC_REGS_OFFSET - offset;

   return DAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Initialize QDSS trace buffer
  
  Initializes the QDSS DDR trace buffer structures and registers a callback
  with error handler
  
  @return None
 */
 
int qdss_trace_buffer_init(void)
{
   int nErr = DAL_ERROR;
   DALResult result;

   if (qdss.buf.initialized) {
      /*we have already initialized */
      return DAL_SUCCESS;
   }

   qdss_debug_dump_init();
   
   result = qdss_dump_buffer_base_init ();
   if (result != DAL_SUCCESS) {
     return result;
   }

   err_crash_cb_register(qdss_error_callback);

   TRY(nErr,qdss_dump_table_init());

   nErr = DAL_SUCCESS;

   qdss.buf.initialized = TRUE;

   CATCH(nErr){}
   return nErr;
}

