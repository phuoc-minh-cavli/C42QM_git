/*==========================================================================
 * FILE:         extpl_module.h
 *
 * SERVICES:     EXTERNAL PAGER
 *
 * DESCRIPTION:  This file defines the data structures for external
 *               paged modules
 *
 *
=============================================================================*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/perf.mpss/2.1.2.0/extpl/inc/extpl_module.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/18/17   che     Initial Version

===========================================================================*/

#ifndef __EXTPL_MODULE_H__
#define __EXTPL_MODULE_H__

typedef struct extpl_module_t {
    unsigned int rx_start;
    unsigned int rx_end;
    unsigned int ro_start;
    unsigned int ro_end;
    unsigned int rw_start;
    unsigned int rw_end;
    unsigned int bss_start;
    unsigned int bss_end;
    extpl_module_state_t state;
} extpl_module_t;

extern extpl_module_t extpl_modules[];

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_text_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_text_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_rodata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_rodata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_rwdata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_rwdata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_nb1_bss_end__;

extern unsigned int __attribute__ ((weak)) __extpl_rx_swap_pool_start__;
extern unsigned int __attribute__ ((weak)) __extpl_rx_swap_pool_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_m1_rodata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_m1_rodata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_m1_rwdata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_m1_rwdata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_m1_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_m1_bss_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_text_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_text_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_rodata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_rodata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_rwdata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_rwdata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_geran_bss_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_text_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_text_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_rodata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_rodata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_rwdata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_rwdata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_gps_bss_end__;


extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_text_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_text_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_rodata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_rodata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_rwdata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_rwdata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_foo_bss_end__;


extern int extpl_module_loaded(int mid);

#endif

