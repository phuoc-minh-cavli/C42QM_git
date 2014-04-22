#ifndef __RR_SI_CACHE__
#define __RR_SI_CACHE__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR System Information Cache Module

GENERAL DESCRIPTION
   This module contains functions to handle system information message processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 by QUALCOMM Technologies, Inc. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_sys_info_cache.h#5 $ 
$DateTime: 2019/12/02 06:22:48 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
20/01/2014  SJW     Added blacklisting interface
13/05/2013  SJW     Ported to QDSP6
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include "sys_type.h"
#include "sys_cnst.h"
#include "sys_stru.h"
#include "rr_l2_l1.h"
#include "rr_l1.h"
#include "rr_l2.h"
#include "rr_gprs_defs.h"

#ifdef FEATURE_RR_SI_CACHE
/* Only 8 SIs are cached currently. This is specifically used for QXDM logging */
#define MAX_CSI_PER_CGI 10

#define CACHED_SYS_INFO_TC 0xCC

typedef struct
{
  LAI_T   lai; /*23.003 4.3.1 location area identifier*/
  uint16  ci; /*23.003 4.3.1 cell id*/
} rr_csi_cgi_t;

typedef struct
{
  uint8   msg_type;
  ARFCN_T si_arfcn;
  rr_csi_cgi_t cgi;
  uint8  *octet_ptr;
  uint16  counter; /*indicates order in which entries are added*/
} csi_entry_t;

typedef enum
{
  CSI_FLUSH_CACHE_FULL = 0,
  CSI_FLUSH_ARFCN_CHANGE,
  CSI_FLUSH_SI_CHANGE,
  CSI_FLUSH_CACHE_INIT,
  CSI_FLUSH_CONTENTION_FAILURE,
  CSI_FLUSH_DIFF_CELL_ON_ARFCN,
  CSI_FLUSH_NO_CAUSE = 255
} rr_si_cache_flush_cause_T;

typedef enum
{
  CSI_ENTRY_UPDATED = 0,
  CSI_ENTRY_ADDED,
  CSI_ENTRY_NO_STATUS = 255
} rr_si_cache_update_status_T;

typedef enum
{
  CSI_RETRIEVE_FOR_FAST_ACQUISITION = 0,
  CSI_RETRIEVE_ALL_SIS
} rr_si_cache_SIs_to_retrieve_T;

extern void rr_sys_info_cache_flush_data( void );

extern void rr_csi_flush_data(LAI_T   *lai_ptr,
                                uint16  *ci_ptr,
                                const rr_si_cache_flush_cause_T csi_flush_cause
                                 );

extern void rr_csi_flush_arfcn_data(const ARFCN_T arfcn,
                                    const rr_si_cache_flush_cause_T csi_flush_cause
                                     );

extern boolean rr_csi_update_entry(uint8            msg_type,
                                   ARFCN_T             arfcn,
                                   dl_unit_data_ind_T *msg_ptr,
                                   LAI_T              *lai_ptr,
                                   uint16             *ci_ptr);

extern rr_si_cache_SIs_to_retrieve_T rr_csi_is_retrieval_for_fast_acquisition( void );

extern uint32 rr_csi_retrieve_all_SIs(ARFCN_T arfcn, LAI_T *lai_ptr, uint16 *cell_id_ptr, rr_si_cache_SIs_to_retrieve_T SIs_to_retrieve );

extern boolean rr_csi_retrieve_SI3_4(ARFCN_T arfcn, LAI_T *lai_ptr, uint16 *cell_id_ptr );

extern void rr_csi_copy_pend_store_to_cache(ARFCN_T             arfcn,
                                            LAI_T              *lai_ptr,
                                            uint16             *ci_ptr);

extern void rr_csi_deactivate_pend_store( void );

extern void rr_csi_activate_pend_store( void );

extern void rr_csi_get_si_index(csi_entry_t *log_entry_ptr,uint8 *si_index);

extern void rr_csi_get_si_cache_summary(csi_entry_t *log_entry_ptr,
  uint8     *si_length,
  uint8     *cached_si_list,
  uint16    *SI_2quater_indices,
  uint16    *SI_2ter_indices);

extern void rr_csi_init_cache(void);

extern uint8 rr_csi_backup_scell_info(ARFCN_T arfcn, LAI_T *lai_ptr, uint16 *cell_id_ptr, csi_entry_t* scell_csi);
extern void rr_csi_restore_from_backup(csi_entry_t* scell_csi, uint8 count);
#endif /* FEATURE_RR_SI_CACHE */

#endif /*  __RR_SI_CACHE__ */
