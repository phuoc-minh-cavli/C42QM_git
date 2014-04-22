/*==============================================================================
                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_log.c_v   1.0   10 May 2002 14:45:42   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_log.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/07/02   kwa     Initial version.
04/28/04   kwa     Added reg_log_umts_nas_eplmn_list.
05/05/04   kwa     Fixed an array overrun in reg_log_umts_nas_eplmn_list.
12/20/04   up      Added reg_log_umts_nas_fplmn_list_update,
                   reg_log_umts_nas_fplmn_list,
                   reg_log_umts_nas_hplmn_search_timer_start,
                   reg_log_umts_nas_hplmn_search_timer_expire,
                   reg_log_umts_nas_hplmn_search_start and
                   reg_log_umts_nas_hplmn_search_end
                   to log updation of FPLMN list, FPLMN list, hplmn timer
                   start/expiry and pplmn search start/end.
02/03/06   hs      Fixed errors introduced due to using RVDS
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "reg_log.h"
#include "naslog_v.h"
#include "msg.h"
#include "event_defs.h"
#if defined (FEATURE_DUAL_SIM) ||defined (FEATURE_SGLTE)
extern boolean mm_dual_standby_mode;
extern boolean mm_sglte_mode;
#endif
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_log_reg_packet

==============================================================================*/

void reg_log_reg_packet
(
  reg_state_e_type                   reg_state,
  sys_network_selection_mode_e_type  network_selection_mode
)
{
  LOG_UMTS_NAS_REG_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_REG_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode  || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_REG_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_REG_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_REG_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      ds_log_ptr->reg_state           = (uint8)reg_state;
      ds_log_ptr->plmn_selection_mode = (uint8)network_selection_mode;
      ds_log_ptr->ue_operation_mode   = 0;

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_REG_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_REG_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_REG_LOG_PACKET_C_type));
    if (log_p)
    {
      log_p->reg_state           = (uint8)reg_state;
      log_p->plmn_selection_mode = (uint8)network_selection_mode;
      log_p->ue_operation_mode   = 0;

      log_commit(log_p);
    }
  }
}


/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_eplmn_list

==============================================================================*/

void reg_log_umts_nas_eplmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* eplmn_list_p
)
{
  uint8 i;

  LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {

      ds_log_ptr->as_id = (uint8) reg_as_id;
      /*
      ** Copy the RPLMN into the log packet.
      */
      ds_log_ptr->rplmn.plmn_id[0] = (uint8)rplmn.identity[0];
      ds_log_ptr->rplmn.plmn_id[1] = (uint8)rplmn.identity[1];
      ds_log_ptr->rplmn.plmn_id[2] = (uint8)rplmn.identity[2];

      /*
      ** Copy the equivalent PLMN list into the static variable
      ** to be written to NVRAM.
      */
      ds_log_ptr->num_equivalent_plmns = (uint8)MAX_NUMBER_EQUIVALENT_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < ds_log_ptr->num_equivalent_plmns)
      {
        ds_log_ptr->num_equivalent_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (eplmn_list_p->length < ds_log_ptr->num_equivalent_plmns)
      {
        ds_log_ptr->num_equivalent_plmns = (uint8)eplmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_equivalent_plmns; i++)
      {
        ds_log_ptr->equivalent_plmn[i].plmn_id[0] = eplmn_list_p->plmn[i].identity[0];
        ds_log_ptr->equivalent_plmn[i].plmn_id[1] = eplmn_list_p->plmn[i].identity[1];
        ds_log_ptr->equivalent_plmn[i].plmn_id[2] = eplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the equivalent
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_equivalent_plmns; i < MAX_NUMBER_EQUIVALENT_PLMNS; i++)
      {
        ds_log_ptr->equivalent_plmn[i].plmn_id[0] = 0xFF;
        ds_log_ptr->equivalent_plmn[i].plmn_id[1] = 0xFF;
        ds_log_ptr->equivalent_plmn[i].plmn_id[2] = 0xFF;
      }

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the RPLMN into the log packet.
      */
      log_p->rplmn.plmn_id[0] = (uint8)rplmn.identity[0];
      log_p->rplmn.plmn_id[1] = (uint8)rplmn.identity[1];
      log_p->rplmn.plmn_id[2] = (uint8)rplmn.identity[2];

      /*
      ** Copy the equivalent PLMN list into the static variable
      ** to be written to NVRAM.
      */
      log_p->num_equivalent_plmns = (uint8)MAX_NUMBER_EQUIVALENT_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < log_p->num_equivalent_plmns)
      {
        log_p->num_equivalent_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (eplmn_list_p->length < log_p->num_equivalent_plmns)
      {
        log_p->num_equivalent_plmns = (uint8)eplmn_list_p->length;
      }

      for (i = 0; i < log_p->num_equivalent_plmns; i++)
      {
        log_p->equivalent_plmn[i].plmn_id[0] = eplmn_list_p->plmn[i].identity[0];
        log_p->equivalent_plmn[i].plmn_id[1] = eplmn_list_p->plmn[i].identity[1];
        log_p->equivalent_plmn[i].plmn_id[2] = eplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the equivalent
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_equivalent_plmns; i < MAX_NUMBER_EQUIVALENT_PLMNS; i++)
      {
        log_p->equivalent_plmn[i].plmn_id[0] = 0xFF;
        log_p->equivalent_plmn[i].plmn_id[1] = 0xFF;
        log_p->equivalent_plmn[i].plmn_id[2] = 0xFF;
      }

      log_commit(log_p);

    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_fplmn_list_update

==============================================================================*/

void reg_log_umts_nas_fplmn_list_update
(
  sys_plmn_id_s_type  plmn,
  boolean             is_added
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type* log_p;
#ifdef FEATURE_DUAL_SIM
  LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type *ds_log_ptr;
#endif

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  sys_plmn_get_mcc_mnc
  (
    plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );

  if (is_added)
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= PLMN(%d-%d) is added to FPLMN List", mcc, mnc);
  }
  else
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= PLMN(%d-%d) is removed from FPLMN List", mcc, mnc);
  }
#ifdef FEATURE_DUAL_SIM
  if ( mm_dual_standby_mode )
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) as_id;
      /*
      ** Copy the FPLMN into the log packet.
      */
      ds_log_ptr->fplmn.plmn_id[0] = (uint8)plmn.identity[0];
      ds_log_ptr->fplmn.plmn_id[1] = (uint8)plmn.identity[1];
      ds_log_ptr->fplmn.plmn_id[2] = (uint8)plmn.identity[2];
      ds_log_ptr->is_added = is_added;

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C,
                      sizeof(LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the FPLMN into the log packet.
      */
      log_p->fplmn.plmn_id[0] = (uint8)plmn.identity[0];
      log_p->fplmn.plmn_id[1] = (uint8)plmn.identity[1];
      log_p->fplmn.plmn_id[2] = (uint8)plmn.identity[2];
      log_p->is_added = is_added;

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_fplmn_list

==============================================================================*/

void reg_log_umts_nas_fplmn_list
(
  const reg_sim_plmn_list_s_type* fplmn_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint8 i;

  LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C_type* log_p;
#ifdef FEATURE_DUAL_SIM
  LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode )
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {

      ds_log_ptr->as_id = (uint8) as_id;
      /*
      ** Copy the forbidden PLMN list into the log packet
      */
      ds_log_ptr->num_fplmns = (uint8)MAX_NUMBER_FORBIDDEN_PLMNS;

      if ((uint8)REG_SIM_PLMN_LIST_LENGTH < ds_log_ptr->num_fplmns)
      {
        ds_log_ptr->num_fplmns = (uint8)REG_SIM_PLMN_LIST_LENGTH;
      }

      if (fplmn_list_p->length < ds_log_ptr->num_fplmns)
      {
        ds_log_ptr->num_fplmns = (uint8)fplmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_fplmns; i++)
      {
        ds_log_ptr->fplmns[i].plmn_id[0] = fplmn_list_p->plmn[i].identity[0];
        ds_log_ptr->fplmns[i].plmn_id[1] = fplmn_list_p->plmn[i].identity[1];
        ds_log_ptr->fplmns[i].plmn_id[2] = fplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the forbidden
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_fplmns; i < MAX_NUMBER_FORBIDDEN_PLMNS; i++)
      {
        ds_log_ptr->fplmns[i].plmn_id[0] = 0xFF;
        ds_log_ptr->fplmns[i].plmn_id[1] = 0xFF;
        ds_log_ptr->fplmns[i].plmn_id[2] = 0xFF;
      }

      log_commit(ds_log_ptr);
    }

  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C,
                      sizeof(LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the forbidden PLMN list into the log packet
      */
      log_p->num_fplmns = (uint8)MAX_NUMBER_FORBIDDEN_PLMNS;

      if ((uint8)REG_SIM_PLMN_LIST_LENGTH < log_p->num_fplmns)
      {
        log_p->num_fplmns = (uint8)REG_SIM_PLMN_LIST_LENGTH;
      }

      if (fplmn_list_p->length < log_p->num_fplmns)
      {
        log_p->num_fplmns = (uint8)fplmn_list_p->length;
      }

      for (i = 0; i < log_p->num_fplmns; i++)
      {
        log_p->fplmns[i].plmn_id[0] = fplmn_list_p->plmn[i].identity[0];
        log_p->fplmns[i].plmn_id[1] = fplmn_list_p->plmn[i].identity[1];
        log_p->fplmns[i].plmn_id[2] = fplmn_list_p->plmn[i].identity[2];
      }

      /*
      ** Undefine the unused PLMN ids contained within the forbidden
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_fplmns; i < MAX_NUMBER_FORBIDDEN_PLMNS; i++)
      {
        log_p->fplmns[i].plmn_id[0] = 0xFF;
        log_p->fplmns[i].plmn_id[1] = 0xFF;
        log_p->fplmns[i].plmn_id[2] = 0xFF;
      }

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_timer_start

==============================================================================*/

void reg_log_umts_nas_hplmn_search_timer_start
(
  dword   hplmn_timer
)
{
  LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      ds_log_ptr->hplmn_search_timer = (uint16) (hplmn_timer&0xFFFF);

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C_type));

    if (log_p)
    {
      log_p->hplmn_search_timer = (uint16) (hplmn_timer&0xFFFF);

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_timer_expire

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
void reg_log_umts_nas_hplmn_search_timer_expire
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  sys_modem_as_id_e_type as_id
#else
  void
#endif
)
{
  LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) as_id;
      ds_log_ptr->dummy = 0;

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C_type));

    if (log_p)
    {
      log_p->dummy = 0;

      log_commit(log_p);
    }
  }

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  if ( mm_dual_standby_mode  || mm_sglte_mode)
  {
    uint8 event_packet = (uint8) as_id;
    event_report_payload(EVENT_DS_HPLMN_TIMER_EXPIRED, 1, (void *)&event_packet);
  }
  else
#endif
  {
    event_report(EVENT_HPLMN_TIMER_EXPIRED);
  }
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif 

#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)
/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_start

==============================================================================*/

void reg_log_umts_nas_hplmn_search_start
(
  const sys_plmn_rat_list_s_type* pplmn_list_p
)
{
  uint8 i;

  LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      /*
      ** Copy the higher priority PLMN list into the log packet
      */
      ds_log_ptr->num_pplmns = (uint8)MAX_NUMBER_PRIORITY_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < ds_log_ptr->num_pplmns)
      {
        ds_log_ptr->num_pplmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (pplmn_list_p->length < ds_log_ptr->num_pplmns)
      {
        ds_log_ptr->num_pplmns = (uint8)pplmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_pplmns; i++)
      {
        ds_log_ptr->pplmns[i].plmn.plmn_id[0] = pplmn_list_p->info[i].plmn.identity[0];
        ds_log_ptr->pplmns[i].plmn.plmn_id[1] = pplmn_list_p->info[i].plmn.identity[1];
        ds_log_ptr->pplmns[i].plmn.plmn_id[2] = pplmn_list_p->info[i].plmn.identity[2];
        ds_log_ptr->pplmns[i].rat = (uint8)pplmn_list_p->info[i].rat;
      }

      /*
      ** Undefine the unused PLMN ids contained within the priority
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_pplmns; i < MAX_NUMBER_PRIORITY_PLMNS; i++)
      {
        ds_log_ptr->pplmns[i].plmn.plmn_id[0] = 0xFF;
        ds_log_ptr->pplmns[i].plmn.plmn_id[1] = 0xFF;
        ds_log_ptr->pplmns[i].plmn.plmn_id[2] = 0xFF;
        ds_log_ptr->pplmns[i].rat = (uint8)SYS_RAT_NONE;
      }

      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the higher priority PLMN list into the log packet
      */
      log_p->num_pplmns = (uint8)MAX_NUMBER_PRIORITY_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < log_p->num_pplmns)
      {
        log_p->num_pplmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (pplmn_list_p->length < log_p->num_pplmns)
      {
        log_p->num_pplmns = (uint8)pplmn_list_p->length;
      }

      for (i = 0; i < log_p->num_pplmns; i++)
      {
        log_p->pplmns[i].plmn.plmn_id[0] = pplmn_list_p->info[i].plmn.identity[0];
        log_p->pplmns[i].plmn.plmn_id[1] = pplmn_list_p->info[i].plmn.identity[1];
        log_p->pplmns[i].plmn.plmn_id[2] = pplmn_list_p->info[i].plmn.identity[2];
        log_p->pplmns[i].rat = (uint8)pplmn_list_p->info[i].rat;
      }

      /*
      ** Undefine the unused PLMN ids contained within the priority
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_pplmns; i < MAX_NUMBER_PRIORITY_PLMNS; i++)
      {
        log_p->pplmns[i].plmn.plmn_id[0] = 0xFF;
        log_p->pplmns[i].plmn.plmn_id[1] = 0xFF;
        log_p->pplmns[i].plmn.plmn_id[2] = 0xFF;
        log_p->pplmns[i].rat = (uint8)SYS_RAT_NONE;
      }

      log_commit(log_p);
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_end

==============================================================================*/

void reg_log_umts_nas_hplmn_search_end
(
  const sys_detailed_plmn_list_s_type* avail_plmn_list_p
)
{
  uint8 i;

  LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type* log_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type *ds_log_ptr;

  if ( mm_dual_standby_mode  || mm_sglte_mode)
  {
    ds_log_ptr = (LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C, 
                             sizeof(LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type));

    if (ds_log_ptr)
    {
      ds_log_ptr->as_id = (uint8) reg_as_id;
      /*
      ** Copy the available PLMN list into the log packet
      */
      ds_log_ptr->num_available_plmns = (uint8)MAX_NUMBER_AVAILABLE_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < ds_log_ptr->num_available_plmns)
      {
        ds_log_ptr->num_available_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (avail_plmn_list_p->length < ds_log_ptr->num_available_plmns)
      {
        ds_log_ptr->num_available_plmns = (uint8)avail_plmn_list_p->length;
      }

      for (i = 0; i < ds_log_ptr->num_available_plmns; i++)
      {
        /* Copy PLMN ID */
        ds_log_ptr->info[i].plmn.plmn_id[0]   = avail_plmn_list_p->info[i].plmn.identity[0];
        ds_log_ptr->info[i].plmn.plmn_id[1]   = avail_plmn_list_p->info[i].plmn.identity[1];
        ds_log_ptr->info[i].plmn.plmn_id[2]   = avail_plmn_list_p->info[i].plmn.identity[2];


        /* Copy type of PLMN - User/Operator preferred  b3-b1 */
        ds_log_ptr->info[i].plmn_details      = (uint8)((byte)avail_plmn_list_p->info[i].list_category & 0x07);

        /* Copy RAT of the PLMN b5-b4 */
        ds_log_ptr->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].rat << 3) & 0x18);

        /* Copy Service capability of the PLMN b7-b6 */
        ds_log_ptr->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].plmn_service_capability << 5) & 0x60);

        /* Copy Signal quality b8 */
        ds_log_ptr->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].signal_quality << 7) & 0x80);

        /* Signal Strength */
        ds_log_ptr->info[i].signal_strength   = (uint8)avail_plmn_list_p->info[i].signal_strength;

      }

      /*
      ** Undefine the unused PLMN information contained within the available
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = ds_log_ptr->num_available_plmns; i < MAX_NUMBER_AVAILABLE_PLMNS; i++)
      {
        (void)memset((void *)&(ds_log_ptr->info[i]), 0xFF, sizeof(log_detailed_plmn_info_type));
      }
  
      log_commit(ds_log_ptr);
    }
  }
  else
#endif
  {
    log_p = (LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C,
                    sizeof(LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C_type));

    if (log_p)
    {
      /*
      ** Copy the available PLMN list into the log packet
      */
      log_p->num_available_plmns = (uint8)MAX_NUMBER_AVAILABLE_PLMNS;

      if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < log_p->num_available_plmns)
      {
        log_p->num_available_plmns = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
      }

      if (avail_plmn_list_p->length < log_p->num_available_plmns)
      {
        log_p->num_available_plmns = (uint8)avail_plmn_list_p->length;
      }

      for (i = 0; i < log_p->num_available_plmns; i++)
      {
        /* Copy PLMN ID */
        log_p->info[i].plmn.plmn_id[0]   = avail_plmn_list_p->info[i].plmn.identity[0];
        log_p->info[i].plmn.plmn_id[1]   = avail_plmn_list_p->info[i].plmn.identity[1];
        log_p->info[i].plmn.plmn_id[2]   = avail_plmn_list_p->info[i].plmn.identity[2];


        /* Copy type of PLMN - User/Operator preferred  b3-b1 */
        log_p->info[i].plmn_details      = (uint8)((byte)avail_plmn_list_p->info[i].list_category & 0x07);

        /* Copy RAT of the PLMN b5-b4 */
        log_p->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].rat << 3) & 0x18);

        /* Copy Service capability of the PLMN b7-b6 */
        log_p->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].plmn_service_capability << 5) & 0x60);

        /* Copy Signal quality b8 */
        log_p->info[i].plmn_details     |= (uint8)(((byte)avail_plmn_list_p->info[i].signal_quality << 7) & 0x80);

        /* Signal Strength */
        log_p->info[i].signal_strength   = (uint8)avail_plmn_list_p->info[i].signal_strength;

      }

      /*
      ** Undefine the unused PLMN information contained within the available
      ** PLMN list to make it easier to verify that the log item has
      ** been properly updated.
      */
      for (i = log_p->num_available_plmns; i < MAX_NUMBER_AVAILABLE_PLMNS; i++)
      {
        (void)memset((void *)&(log_p->info[i]), 0xFF, sizeof(log_detailed_plmn_info_type));
      }
  
      log_commit(log_p);
    }
  }
}

#endif /*defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)*/


/*==============================================================================
Function Name: REG_LOG_UMTS_NAS_PPLMN_LIST

Description: This function sends the log packet for Preferred PLMN list

Return Value: None
==============================================================================*/

void reg_log_umts_nas_pplmn_list
(
  const reg_sim_plmn_w_act_s_type* pplmn_list_p,
  uint16                           num_opplmn,
  uint16                           num_upplmn,
  uint16                           num_pplmn,
  sys_modem_as_id_e_type           as_id
)
{
  uint16 i = 0;
  uint16 total_num_pplmn = 0;
  LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C_type* log_p = NULL;

  if((num_opplmn + num_upplmn) > 0)
  {
    total_num_pplmn = num_opplmn + num_upplmn;
  }
  else
  {
    total_num_pplmn = num_pplmn;
  }  

  if(pplmn_list_p != NULL && total_num_pplmn > 0)
  {
    log_p = (LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C_type*)log_alloc(LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C,
                      sizeof(LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C_type));

    if(log_p)
    {
      log_p->version = (uint8)1;

      log_p->as_id = (uint8)as_id + 1;

      log_p->num_pplmns = total_num_pplmn;
      if(log_p->num_pplmns > (uint16)REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH)
      {
        log_p->num_pplmns = (uint16)REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH;
      }

      for (i = 0; i < log_p->num_pplmns; i++)
      {
        /* Populate PLMN ID */
        log_p->pplmn_list[i].plmn.plmn_id[0] = pplmn_list_p[i].plmn.identity[0];
        log_p->pplmn_list[i].plmn.plmn_id[1] = pplmn_list_p[i].plmn.identity[1];
        log_p->pplmn_list[i].plmn.plmn_id[2] = pplmn_list_p[i].plmn.identity[2];

        /* Populate PLMN category */
        if((num_opplmn + num_upplmn) == 0)
        {
          log_p->pplmn_list[i].category = (uint8)SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED;
        }
        else if(i < num_upplmn)
        {
          log_p->pplmn_list[i].category = (uint8)SYS_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED;
        }
        else
        {
          log_p->pplmn_list[i].category = (uint8)SYS_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED;
        }

        /* Populate Access Technologies */
        log_p->pplmn_list[i].num_rats = 0;
#ifdef FEATURE_LTE
        /* LTE */
        if((pplmn_list_p[i].act[0] & 0x40) == 0x40)
        {
          log_p->pplmn_list[i].rat[log_p->pplmn_list[i].num_rats] = (uint8)SYS_RAT_LTE_RADIO_ACCESS;
	      log_p->pplmn_list[i].num_rats = log_p->pplmn_list[i].num_rats + 1;
        }
#endif
        /* UMTS */
        if((pplmn_list_p[i].act[0] & 0x80) == 0x80)
        {
          log_p->pplmn_list[i].rat[log_p->pplmn_list[i].num_rats] = (uint8)SYS_RAT_UMTS_RADIO_ACCESS;
	      log_p->pplmn_list[i].num_rats = log_p->pplmn_list[i].num_rats + 1;
        }
        /* GSM */
        if((pplmn_list_p[i].act[1] & 0x80) == 0x80)
        {
          log_p->pplmn_list[i].rat[log_p->pplmn_list[i].num_rats] = (uint8)SYS_RAT_GSM_RADIO_ACCESS;
	      log_p->pplmn_list[i].num_rats = log_p->pplmn_list[i].num_rats + 1;
        }

        /* ALL */
        if((log_p->pplmn_list[i].num_rats == 0) &&
           (pplmn_list_p[i].act[0] == 0x00) &&
           (pplmn_list_p[i].act[1] == 0x00))
        {
#ifdef FEATURE_LTE
           log_p->pplmn_list[i].num_rats = 3;
           log_p->pplmn_list[i].rat[0] = (uint8)SYS_RAT_LTE_RADIO_ACCESS;
           log_p->pplmn_list[i].rat[1] = (uint8)SYS_RAT_UMTS_RADIO_ACCESS;
           log_p->pplmn_list[i].rat[2] = (uint8)SYS_RAT_GSM_RADIO_ACCESS;
#else
           log_p->pplmn_list[i].num_rats = 2;
           log_p->pplmn_list[i].rat[0] = (uint8)SYS_RAT_UMTS_RADIO_ACCESS;
           log_p->pplmn_list[i].rat[1] = (uint8)SYS_RAT_GSM_RADIO_ACCESS;
#endif
        }
	
      } /* End for loop */

      log_commit(log_p);
    }
  }
}

/*==============================================================================
Function Name: reg_log_umts_wwan_advance_scan_param

Description: This function sends the log packet for gps wwan advance scan param

Return Value: None
==============================================================================*/

void reg_log_umts_wwan_advance_scan_param
(
  sys_wwan_adavnce_scan_pram_s_type gps_wwan_scan_info,
  sys_modem_as_id_e_type           as_id
)
{
  uint8 i = 0;
  uint8 j = 0;
  LOG_WWAN_ADVANCE_SCAN_PARAM_LOG_C_type* log_p = NULL;

    log_p = (LOG_WWAN_ADVANCE_SCAN_PARAM_LOG_C_type*)log_alloc(LOG_WWAN_ADVANCE_SCAN_PARAM_LOG_C,
                      sizeof(LOG_WWAN_ADVANCE_SCAN_PARAM_LOG_C_type));

    if(log_p)
    {
      log_p->version = (uint8)1;

      log_p->as_id = (uint8)as_id + 1;

      log_p->wwan_adavnce_scan_pram.control_param.max_cells_per_channel = gps_wwan_scan_info.control_param.max_cells_per_channel;

	  log_p->wwan_adavnce_scan_pram.assist_param.rat = (log_radio_access_tech_e_type)gps_wwan_scan_info.assist_param.rat;

	  if(gps_wwan_scan_info.assist_param.rat == SYS_RAT_GSM_RADIO_ACCESS)
	  {
	    log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.is_gsm_blind_scan_req = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.is_gsm_blind_scan_req;
		log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.max_scan_duration_gsm_sec = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.max_scan_duration_gsm_sec;
		log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.num_gsm_ad_element = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.num_gsm_ad_element;

		for(i=0;i<gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.num_gsm_ad_element;i++)
		{
           log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].arfcn.band = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].arfcn.band;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].arfcn.num = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].arfcn.num;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].bsic = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].bsic;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].decode_gcid = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].decode_gcid;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].measurement_power = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].measurement_power;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].measure_cell_timing = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.gsm_params.gsm_ad_elems[i].measure_cell_timing;	 	
		}
	  }
	  else if(gps_wwan_scan_info.assist_param.rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
	  {
	    log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.is_m1_blind_scan_req = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.is_m1_blind_scan_req;
	    log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.max_scan_duration_m1_sec = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.max_scan_duration_m1_sec;
	    log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.num_m1_ad_element = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.num_m1_ad_element;
	    for(i=0;i<gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.num_m1_ad_element;i++)
		{
           log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].earfcn = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].earfcn;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].decode_sib1 = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].decode_sib1;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].measure_power_and_time = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].measure_power_and_time;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].num_cells_to_return = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].num_cells_to_return;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].num_of_m1_pci_ad_element = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].num_of_m1_pci_ad_element;		   
           for(j=0;j<gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].num_of_m1_pci_ad_element;j++)
           {
             log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].is_lte_cell_id = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].is_lte_cell_id;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].is_system_frame_offset_present = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].is_system_frame_offset_present;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].phy_id = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].phy_id;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].system_frame_offset = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.m1_scan_params.m1_ad_elems[i].wwan_scan_ad_per_pci_m1[j].system_frame_offset;
           }
		}
	  }
	  else if(gps_wwan_scan_info.assist_param.rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
	  {
	    log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.is_nb1_blind_scan_req = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.is_nb1_blind_scan_req;
	    log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.max_scan_duration_nb1_sec = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.max_scan_duration_nb1_sec;
	    log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.num_nb1_ad_element = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.num_nb1_ad_element;
	    for(i=0;i<gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.num_nb1_ad_element;i++)
		{
           log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].earfcn = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].earfcn;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].decode_sib1 = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].decode_sib1;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].measure_power_and_time = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].measure_power_and_time;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].num_cells_to_return = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].num_cells_to_return;
		   log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].num_of_nb1_pci_ad_element = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].num_of_nb1_pci_ad_element;		   
           for(j=0;j<gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].num_of_nb1_pci_ad_element;j++)
           {
             log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].is_earfcn_offset_present = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].is_earfcn_offset_present;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].is_raster_offset_present = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].is_raster_offset_present;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].is_system_frame_offset_present = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].is_system_frame_offset_present;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].nb1_earfcn_offset = (log_nb1_earfcn_offset_e_type)gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].nb1_earfcn_offset;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].nb1_raster_offset = (log_nb1_raster_offset_e_type)gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].nb1_raster_offset;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].phy_id = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].phy_id;
			 log_p->wwan_adavnce_scan_pram.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].system_frame_offset = gps_wwan_scan_info.assist_param.wwan_rat_scan_param.nb1_scan_params.nb1_ad_elems[i].wwan_scan_ad_per_pci_nb1[j].system_frame_offset;
           }
		}
	  }	  
      log_commit(log_p);
  }
}
void reg_log_umts_wwan_gsm_scan_log_p
(
 uint8 low, uint8 high, uint8 iteration,
 sys_loc_wwan_scan_result_per_rat_s_type *wwan_scan_result_gsm,
 sys_modem_as_id_e_type as_id
)
{
	uint8 j = 0;
	uint8 k = 0;
	LOG_WWAN_SCAN_RESULT_GSM_LOG_C_type* log_p = NULL;

	log_p = (LOG_WWAN_SCAN_RESULT_GSM_LOG_C_type*)log_alloc(LOG_WWAN_SCAN_RESULT_GSM_LOG_C,
						  sizeof(LOG_WWAN_SCAN_RESULT_GSM_LOG_C_type));
	
		if(log_p)
		{
		  log_p->version = (uint8)1;
	
		  log_p->as_id = (uint8)as_id + 1;
		  log_p->wwan_scan_result_gsm.num_scan_result_per_rat = wwan_scan_result_gsm->num_scan_result_per_rat;
		  log_p->wwan_scan_result_gsm.rat = (log_radio_access_tech_e_type)wwan_scan_result_gsm->rat;
		  log_p->wwan_scan_result_gsm.scan_ref_time = wwan_scan_result_gsm->scan_ref_time;
		  log_p->wwan_scan_result_gsm.scan_status_per_rat = (log_plmn_list_status_e_type)wwan_scan_result_gsm->scan_status_per_rat;
	
		  for(k=low;k<high &&  k<SYS_MAX_RESULT_PER_RAT && ( ((k-iteration*LOG_MAX_RESULT_PER_RAT) >= 0) && ((k-iteration*LOG_MAX_RESULT_PER_RAT)<11) ) &&  iteration<3;k++)
		  {
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.arfcn.band = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.arfcn.band;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.arfcn.num = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.arfcn.num;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.bsic = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.bsic;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.cell_info.rat = (log_radio_access_tech_e_type)(SYS_RAT_GSM_RADIO_ACCESS);
		  
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.lac = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.lac;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.cell_identity = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.cell_identity;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn.identity[0] = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn.identity[0];
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn.identity[1] = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn.identity[1];
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn.identity[2] = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn.identity[2];
 

   
	
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.cell_timing = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.cell_timing;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.channel_scan_result_status = (log_channel_scan_result_e_type)wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.channel_scan_result_status;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.is_bsic_valid = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.is_bsic_valid;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.is_cell_timing_present = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.is_cell_timing_present;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.is_global_cell_id_present = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.is_global_cell_id_present;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.is_rx_power_meas_present = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.is_rx_power_meas_present;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.rx_power_meas.power_measurement.rsrp = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.rx_power_meas.power_measurement.rsrp;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel = wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel;
		  log_p->wwan_scan_result_gsm.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_gsm.rx_power_meas.rx_power_meas_sel = (log_wwan_rx_power_meas_e_type)wwan_scan_result_gsm->wwan_scan_result_per_rat[k].wwan_scan_result_gsm.rx_power_meas.rx_power_meas_sel;
	
		 
		}
		  log_commit(log_p);
		}

}
void reg_log_umts_wwan_m1_scan_log_p
(
  uint8 low,uint8 high,uint8 iteration,
  sys_loc_wwan_scan_result_per_rat_s_type *wwan_scan_result_m1,
  sys_modem_as_id_e_type           as_id
)
{
	uint8 i = 0;
	 uint8 j = 0;
	 uint8 k = 0;
	 LOG_WWAN_SCAN_RESULT_M1_LOG_C_type* log_p = NULL;
	
	   log_p = (LOG_WWAN_SCAN_RESULT_M1_LOG_C_type*)log_alloc(LOG_WWAN_SCAN_RESULT_M1_LOG_C,
						 sizeof(LOG_WWAN_SCAN_RESULT_M1_LOG_C_type));
	
	   if(log_p)
	   {
		 log_p->version = (uint8)1;
	
		 log_p->as_id = (uint8)as_id + 1;
		 log_p->wwan_scan_result_m1.num_scan_result_per_rat = wwan_scan_result_m1->num_scan_result_per_rat;
		 log_p->wwan_scan_result_m1.rat = (log_radio_access_tech_e_type)wwan_scan_result_m1->rat;
		 log_p->wwan_scan_result_m1.scan_ref_time = wwan_scan_result_m1->scan_ref_time;
		 log_p->wwan_scan_result_m1.scan_status_per_rat = (log_plmn_list_status_e_type)wwan_scan_result_m1->scan_status_per_rat;

		 MSG_HIGH_DS_1(REG_SUB, "=EMM= num_scan_result_per_rat%d", log_p->wwan_scan_result_m1.num_scan_result_per_rat);
	
		 for(k=low;k<high &&  k<SYS_MAX_RESULT_PER_RAT &&( ((k-iteration*LOG_MAX_RESULT_PER_RAT) >= 0) && ((k-iteration*LOG_MAX_RESULT_PER_RAT)<11) ) && iteration<3;k++)
		 {
		 log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.channel_scan_result_status = (log_channel_scan_result_e_type)wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.channel_scan_result_status;
		 log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.earfcn = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.earfcn;
		 log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.num_result_per_earfcn_m1 = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.num_result_per_earfcn_m1;
	
		 for(i=0;i<wwan_scan_result_m1->wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.num_result_per_earfcn_m1;i++)
		 {
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat = (log_radio_access_tech_e_type)(SYS_RAT_LTE_M1_RADIO_ACCESS);

		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.global_cell_id = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.global_cell_id;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.tac = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.tac;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.num_plmn = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.num_plmn;
		   for(j=0;j<wwan_scan_result_m1->wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.num_plmn;j++)
		   {
			   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.plmn[j].identity[0] = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.plmn[j].identity[0];
			   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.plmn[j].identity[1] = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.plmn[j].identity[1];
			   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.plmn[j].identity[2] = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].cell_info.rat_cell.m1_Cell.plmn[j].identity[2];
		   }

	
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].delta_time_offset = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].delta_time_offset;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_cell_info_present = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_cell_info_present;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_delta_time_offset_present = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_delta_time_offset_present;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_lte_cell = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_lte_cell;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_rx_power_meas_present = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_rx_power_meas_present;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_serving_cell = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_serving_cell;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_system_frame_offset_present = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].is_system_frame_offset_present;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].phy_id = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].phy_id;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].rx_power_meas.power_measurement.rsrp = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].rx_power_meas.power_measurement.rsrp;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].rx_power_meas.power_measurement.rxlevel = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].rx_power_meas.power_measurement.rxlevel;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].rx_power_meas.rx_power_meas_sel = (log_wwan_rx_power_meas_e_type)wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].rx_power_meas.rx_power_meas_sel;
		   log_p->wwan_scan_result_m1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].system_frame_offset = wwan_scan_result_m1->wwan_scan_result_per_rat[k].wwan_scan_result_m1.wwan_result_per_earfcn_m1[i].system_frame_offset;
		 }
		 
	   }
		 log_commit(log_p);
	   }
		
}
void reg_log_umts_wwan_nb1_scan_log_p
(
  uint8 low,uint8 high,uint8 iteration,
  sys_loc_wwan_scan_result_per_rat_s_type *wwan_scan_result_nb1,
  sys_modem_as_id_e_type           as_id
)
{
	uint8 i = 0;
	 uint8 j = 0;
	 uint8 k = 0;
	 LOG_WWAN_SCAN_RESULT_NB1_LOG_C_type* log_p = NULL;
	
	   log_p = (LOG_WWAN_SCAN_RESULT_NB1_LOG_C_type*)log_alloc(LOG_WWAN_SCAN_RESULT_NB1_LOG_C,
						 sizeof(LOG_WWAN_SCAN_RESULT_NB1_LOG_C_type));
	
	   if(log_p)
	   {
		 log_p->version = (uint8)1;
	
		 log_p->as_id = (uint8)as_id + 1;
		 log_p->wwan_scan_result_nb1.num_scan_result_per_rat = wwan_scan_result_nb1->num_scan_result_per_rat;
		 log_p->wwan_scan_result_nb1.rat = (log_radio_access_tech_e_type)wwan_scan_result_nb1->rat;
		 log_p->wwan_scan_result_nb1.scan_ref_time = wwan_scan_result_nb1->scan_ref_time;
		 log_p->wwan_scan_result_nb1.scan_status_per_rat = (log_plmn_list_status_e_type)wwan_scan_result_nb1->scan_status_per_rat;

		  MSG_HIGH_DS_1(REG_SUB, "=EMM= num_scan_result_per_rat%d",log_p->wwan_scan_result_nb1.num_scan_result_per_rat);
	
		 for(k=low;k<high && k<SYS_MAX_RESULT_PER_RAT &&( ((k-iteration*LOG_MAX_RESULT_PER_RAT) >= 0) && ((k-iteration*LOG_MAX_RESULT_PER_RAT)<11) ) && iteration<3;k++)
		 {
		 log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.channel_scan_result_status = (log_channel_scan_result_e_type)wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.channel_scan_result_status;
		 log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.earfcn = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.earfcn;
		 log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.num_result_per_earfcn_nb1 = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.num_result_per_earfcn_nb1;
	
		 for(i=0;i<wwan_scan_result_nb1->wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.num_result_per_earfcn_nb1;i++)
		 {
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat = (log_radio_access_tech_e_type)(SYS_RAT_LTE_NB1_RADIO_ACCESS);
	
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.global_cell_id = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.global_cell_id;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.tac = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.tac;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.num_plmn = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.num_plmn;
		   for(j=0;j<wwan_scan_result_nb1->wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.num_plmn;j++)
		   {
			   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.plmn[j].identity[0] = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.plmn[j].identity[0];
			   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.plmn[j].identity[1] = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.plmn[j].identity[1];
			   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.plmn[j].identity[2] = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].cell_info.rat_cell.nb1_cell.plmn[j].identity[2];
		   }
	   
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].delta_time_offset = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].delta_time_offset;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_cell_info_present = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_cell_info_present;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_delta_time_offset_present = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_delta_time_offset_present;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_earfcn_offset_present = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_earfcn_offset_present;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_raster_offset_present = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_raster_offset_present;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_rx_power_meas_present = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_rx_power_meas_present;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_serving_cell = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_serving_cell;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_system_frame_offset_present = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].is_system_frame_offset_present;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].nb1_earfcn_offset = (log_nb1_earfcn_offset_e_type)wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].nb1_earfcn_offset;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].nb1_raster_offset = (log_nb1_raster_offset_e_type)wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].nb1_raster_offset;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].phy_id = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].phy_id;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].rx_power_meas.power_measurement.rsrp = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].rx_power_meas.power_measurement.rsrp; 
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].rx_power_meas.power_measurement.rxlevel = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].rx_power_meas.power_measurement.rxlevel;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].rx_power_meas.rx_power_meas_sel = (log_wwan_rx_power_meas_e_type)wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].rx_power_meas.rx_power_meas_sel;
		   log_p->wwan_scan_result_nb1.wwan_scan_result_per_rat[k-iteration*LOG_MAX_RESULT_PER_RAT].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].system_frame_offset = wwan_scan_result_nb1->wwan_scan_result_per_rat[k].wwan_scan_result_nb1.wwan_result_per_earfcn_nb1[i].system_frame_offset;   
		 }
		 
	   }
		 log_commit(log_p);
	   }
		
}

/*==============================================================================
Function Name: reg_log_umts_wwan_gsm_scan_result

Description: This function sends the log packet for gps wwan gsm scan result

Return Value: None
==============================================================================*/

void reg_log_umts_wwan_gsm_scan_result
(
  sys_loc_wwan_scan_result_per_rat_s_type *wwan_scan_result_gsm,
  sys_modem_as_id_e_type           as_id
)
{
	reg_log_umts_wwan_gsm_scan_log_p(0,11,0,wwan_scan_result_gsm,as_id);

	if(wwan_scan_result_gsm->num_scan_result_per_rat>=11 && wwan_scan_result_gsm->num_scan_result_per_rat<22)
		{
	reg_log_umts_wwan_gsm_scan_log_p(11,22,1,wwan_scan_result_gsm,as_id);	  
		}
	if(wwan_scan_result_gsm->num_scan_result_per_rat>=22 && wwan_scan_result_gsm->num_scan_result_per_rat<SYS_MAX_RESULT_PER_RAT)
	  {
	reg_log_umts_wwan_gsm_scan_log_p(22,SYS_MAX_RESULT_PER_RAT,2,wwan_scan_result_gsm,as_id);	  
    }
}

/*==============================================================================
Function Name: reg_log_umts_wwan_m1_scan_result

Description: This function sends the log packet for gps wwan m1 scan result

Return Value: None
==============================================================================*/

void reg_log_umts_wwan_m1_scan_result
(
  sys_loc_wwan_scan_result_per_rat_s_type *wwan_scan_result_m1,
  sys_modem_as_id_e_type           as_id
)
{
	reg_log_umts_wwan_m1_scan_log_p(0,11,0,wwan_scan_result_m1,as_id);

	if(wwan_scan_result_m1->num_scan_result_per_rat>=11 &&wwan_scan_result_m1->num_scan_result_per_rat<22)
	  {
	reg_log_umts_wwan_m1_scan_log_p(11,22,1,wwan_scan_result_m1,as_id);	  
		}
	if(wwan_scan_result_m1->num_scan_result_per_rat>=22 && wwan_scan_result_m1->num_scan_result_per_rat<SYS_MAX_RESULT_PER_RAT)
		{
	reg_log_umts_wwan_m1_scan_log_p(22,SYS_MAX_RESULT_PER_RAT,2,wwan_scan_result_m1,as_id);	  
    }
}


/*==============================================================================
Function Name: reg_log_umts_wwan_nb1_scan_result

Description: This function sends the log packet for gps wwan nb1 scan result

Return Value: None
==============================================================================*/

void reg_log_umts_wwan_nb1_scan_result
(
  sys_loc_wwan_scan_result_per_rat_s_type *wwan_scan_result_nb1,
  sys_modem_as_id_e_type           as_id
)
{
	reg_log_umts_wwan_nb1_scan_log_p(0,11,0,wwan_scan_result_nb1,as_id);

	if(wwan_scan_result_nb1->num_scan_result_per_rat>=11 && wwan_scan_result_nb1->num_scan_result_per_rat<22)
		  {
	reg_log_umts_wwan_nb1_scan_log_p(11,22,1,wwan_scan_result_nb1,as_id);	  
		  }
	if(wwan_scan_result_nb1->num_scan_result_per_rat>=22 && wwan_scan_result_nb1->num_scan_result_per_rat<SYS_MAX_RESULT_PER_RAT)
		  {
	reg_log_umts_wwan_nb1_scan_log_p(22,SYS_MAX_RESULT_PER_RAT,2,wwan_scan_result_nb1,as_id);	  
    }
}
