/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S _  M U X _ P L A T F O R M . C

DESCRIPTION

Platform/OS specific defination 


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2016,2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/dev/data.mpss/3.4/adubey.data.mpss.3.4.ds_mux_changes/interface/tasks/src/ds_mux_platform.c#1 
  $ $DateTime: 2021/05/05 04:35:02 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/07/10    am     created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ds_mux_defs.h"
#include "ds_mux_platform.h"

#ifdef MOB_TX
#include "offtarget_stubs.h"
#endif

/*===========================================================================

                      LOCAL DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Definitions and data for the bit-field conversion functions.
  first_set_bits_array_const[i] == X, 0<=X<=7
  where X is the first non-zero bit in binary representation of i.

  Examples:
  first_set_bits_array_const[8] == 3 since binary of 8 is 0001000
  so bit 3 is first non-zero bit.

  Note: first_set_bits_array_const[0] == 8 since no bit is set for 0
  and 8 isn't valid bit number.
---------------------------------------------------------------------------*/
#define SET_BIT_NUM          (8)
#define SET_BIT_NUM_LENGTH   ((1<<SET_BIT_NUM))

static const uint8 first_set_bits_array_const[SET_BIT_NUM_LENGTH] = 
{ 
  8, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0,
  2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0,
  3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
  2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0,
  2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0,
  3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
  2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0,
  2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0,
  3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
  2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/*===========================================================================

                  EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

void ds_mux_init_crit_section
(
  qurt_mutex_t* crit_sect_ptr
)
{
  qurt_mutex_init(crit_sect_ptr);
  return;
}/*ds_mux_init_crit_section*/


void ds_mux_enter_crit_section
(
  qurt_mutex_t*  crit_sect_ptr
)
{
  qurt_mutex_lock(crit_sect_ptr);
  return;
}/*ds_mux_enter_crit_section*/

void ds_mux_leave_crit_section
(
  qurt_mutex_t*  crit_sect_ptr
)
{
  qurt_mutex_unlock(crit_sect_ptr);
  return;
}/*ds_mux_leave_crit_section*/

void ds_mux_destroy_crit_section
(
  qurt_mutex_t*  crit_sect_ptr
)
{
  qurt_mutex_destroy(crit_sect_ptr);
  return;

}/*ds_mux_destroy_crit_section*/

uint32 ds_mux_find_set_bits(uint32* sig_bit_field, uint32 length, uint32* buffer)
{
  uint32 pos;
  uint32 set_bits;
  uint32 i;
  uint32 size;
  uint8 val;
  const uint8* pointer;

  /*-------------------------------------------------------------------------
    init set bits to no bits set
  -------------------------------------------------------------------------*/
  set_bits = 0;

  /*-------------------------------------------------------------------------
    convert the dword into a byte array
  -------------------------------------------------------------------------*/
  pointer = (const uint8*)sig_bit_field;

  /*-------------------------------------------------------------------------
    set number of bytes
  -------------------------------------------------------------------------*/
  size = length * sizeof(uint32);

  /*-------------------------------------------------------------------------
    for each byte do
  -------------------------------------------------------------------------*/
  for ( i = 0; i < size; i++)
  {
    /*-------------------------------------------------------------------------
      get the byte value
    -------------------------------------------------------------------------*/
    val = pointer[i];

    /*-------------------------------------------------------------------------
     initialize the current bit position
    -------------------------------------------------------------------------*/
    pos = (i * SET_BIT_NUM);
    
    /*-------------------------------------------------------------------------
     while there are set bits remaining
    -------------------------------------------------------------------------*/
    while(val != 0)
    {
      /*-------------------------------------------------------------------------
        add the bit to the list of set bits
      -------------------------------------------------------------------------*/
      buffer[set_bits] = (first_set_bits_array_const[val] + pos);
      set_bits++;

      /*-------------------------------------------------------------------------
        update pos to reflect the current bit position
      -------------------------------------------------------------------------*/
      pos = pos + (first_set_bits_array_const[val] + 1);

      /*-------------------------------------------------------------------------
        shift the value to remove the current set bit
      -------------------------------------------------------------------------*/
      val = val >> (first_set_bits_array_const[val] + 1);
    }
  }

  /*-------------------------------------------------------------------------
    return the number of set bits
  -------------------------------------------------------------------------*/
  return set_bits;
} /* ds_mux_find_set_bits() */



 int ds_mux_timer_alloc
(
  qurt_timer_t*    timer_handle,
  qurt_timer_attr_t* timer_attr,
  void (* callback) (void *),
  void *cb_param
)
{
  qurt_timer_attr_set_callback(timer_attr, callback, (void *) cb_param );
  qurt_timer_attr_set_option(timer_attr,QURT_TIMER_NO_AUTO_START);
  qurt_timer_attr_set_duration(timer_attr, 0xFFFFFFFE);
  return qurt_timer_create(timer_handle, timer_attr);
  /*return ps_timer_alloc( callback, cb_param);*/
}/* ds_mux_timer_alloc*/


int ds_mux_timer_start
(
  qurt_timer_t timer_handle,
  qurt_timer_attr_t* timer_attr,
  uint64 time
)
{
  qurt_timer_attr_set_duration(timer_attr, time);
  if (QURT_EOK != qurt_timer_start(timer_handle))
  {
    return DS_MUX_TIMER_FAILURE;
  }
  return DS_MUX_TIMER_SUCCESS;
  /*return ps_timer_start( handle, time);*/
}/* ds_mux_timer_start */


 int ds_mux_timer_free
(
  qurt_timer_t timer_handle
)
{
  return qurt_timer_delete(timer_handle);
  /*return ps_timer_free( handle );*/
}/* ds_mux_timer_free */


