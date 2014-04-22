#ifndef __SPMIF_H__
#define __SPMIF_H__
/**
  @file spmif_bam.h

  This file contains definitions of constants, data structures, and
  interfaces that provide operational control to the SPMI Fetcher device.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.tx/6.0/hwengines/ecm/spmi_fetcher.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/29/15   rl      Created

===============================================================================
                   Copyright (c) 2015 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "DALSys.h"
#include "bam.h"
/** SPMI fetcher current configured mode */
enum spmif_op_mode
{
	SPMIF_OP_ONESHOT,
	SPMIF_OP_STREAMING
};

/** A buffer returned to its clients. It's important to keep the VA as the first
 *  field in the structure
 */
struct spmif_bam_buf
{
	uint32 va;
	uint32 pa;
	uint32 size;
};


int spmif_start(enum spmif_op_mode mode);
int spmif_stop(void);
int spmif_is_idle(void);
void spmif_set_eot_length(uint32 eot_length);
int spmif_queue_buffer(uint32 pa, uint32 size, void* ctx);
void spmif_clk_on(void);
void spmif_clk_off(void);
int spmif_dev_init(uint32 fifo_va, uint32 fifo_pa, uint32 size, bam_callback_func_type cb, uint32 channel);
void spmif_dev_deinit(void);
void spmif_set_pollgap_time(uint32 time);
void spmif_set_readgap_time(uint32 time);
#endif
