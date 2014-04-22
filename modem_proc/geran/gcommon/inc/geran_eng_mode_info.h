/*****************************************************************************
***
*** TITLE
***
***  GERAN ENG MODE INFO INTERFACE
***
***  $Header: //components/rel/geran.mpss/5.2.0/gcommon/inc/geran_eng_mode_info.h#3 $
***
*** DESCRIPTION
***
***  Provides APIs to extract information from the GSM/GPRS Access Stratum
***
*** FUNCTION SUMMARY
***
***
***
*** Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/
#ifndef GERAN_ENG_MODE_INFO_H
#define GERAN_ENG_MODE_INFO_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "geran_eng_mode_read_api.h"
#include "geran_dual_sim.h" 

typedef struct
{
  geran_eng_mode_info_bitmask_t requested_bitmask;
} eng_mode_modem_stats_info_t;

typedef struct
{
  void *func_ptr;
  geran_eng_mode_cb_behaviour_t behaviour;
} geran_eng_mode_cb_t;

typedef struct
{
  geran_eng_mode_info_t       geran_eng_mode_info;
  geran_eng_mode_cb_t         eng_mode_cb_funcs[ENG_MODE_MAX_NUM_INFO_TYPE_ELEMENTS][MAX_NUM_CALLBACK_REGISTRATIONS];
  eng_mode_modem_stats_info_t geran_stats_info;
  boolean                     store_initialised;
} geran_eng_mode_data_t;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*==========================================================================

  FUNCTION    geran_eng_mode_data_write

  DESCRIPTION

    This function is intended to write the eng mode public store


  PARAMETERS

    geran_eng_mode_info_type:  geran_eng_mode_info_type_t type

    param_copy            :   void

    valid_flag:               The value of the parameter is valid or not

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_eng_mode_data_write(geran_eng_mode_info_type_t geran_eng_mode_info_type, void *param_copy,boolean valid_flag );

/*===========================================================================

FUNCTION geran_multisim_eng_mode_refresh_cm

DESCRIPTION

  Refreshes CM with the stored eng mode data.

PARAMETERS

  geran_eng_mode_info_type - data for refresh
  gas_id_t gas_id


RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void geran_multisim_eng_mode_refresh_cm(geran_eng_mode_info_type_t geran_eng_mode_info_type
                                         );



extern geran_eng_mode_data_t* geran_get_eng_mode_data_ptr(sys_modem_as_id_e_type as_id);
extern void geran_eng_initialise_store(sys_modem_as_id_e_type as_id);

#endif /* GERAN_ENG_MODE_INFO_H */

/* EOF */

