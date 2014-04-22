/*==============================================================================
  FILE:         csd_dev_port_manager.c

  OVERVIEW:     This module manages dynamic port allocations for CSD. Slimbus
                ports, like any serial port, are identical to one another.
                Hence, we can assign a port at runtime rather that configuring
                it via ACBD module. The restrictions, if any, is imposed by
                afe module.

  Dependencies: None

  Copyright (c) 2008-13, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/b-family/src/csd_dev_port_manager.c#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who    what, where, why
  --------   ---    ----------------------------------------------------------
  11/15/16   rk     Abstracting OSAL layer
  04/15/13   unni   Initial Creation
==============================================================================*/
#include "DALSys.h"
#include "csd_debug_msg.h"
#include "csd_dev_port_manager.h"
#include "adsp_afe_service_commands.h"
#include "csd_os_dependencies.h"

typedef struct port_list
{
  int16_t ref_count;
  uint16_t port_id;
  struct port_list* next;
}port_list_t;

/* Separate port list helps reduce search iterations */
static port_list_t* rx_list = NULL;
static port_list_t* tx_list = NULL;

/*==============================================================================

  FUNCTION      csd_dev_pm_alloc_sb_port

  DESCRIPTION   Allocate slimbus port based on direction

  DEPENDENCIES  @see adsp_afe_service_commands.h

  PARAMETERS    @param direction   : indicates whether Rx or Tx
                @param afe_port_id : reference to be populated with dynamic
                                     slimbus port id

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pm_alloc_sb_port(int8_t direction, uint16_t* afe_port_id)
{
  port_list_t **head = NULL;
  port_list_t *curr = NULL;
  port_list_t *prev = NULL;
  bool_t used = TRUE;

  int32_t rc = CSD_EOK;
  int16_t temp_port_id = 0;

  if(NULL == afe_port_id)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_alloc_sb_port: "
            "Null Param, afe port id[%p]", ((void*)afe_port_id));
    return CSD_EBADPARAM;
  }

  *afe_port_id = AFE_PORT_ID_INVALID;

  switch(direction)
  {
    case CSD_DEV_RX_DEVICE:
      temp_port_id = AFE_PORT_ID_SLIMBUS_RANGE_START;
      head = &rx_list;
      break;
    case CSD_DEV_TX_DEVICE:
      temp_port_id = AFE_PORT_ID_SLIMBUS_RANGE_START + 1;
      head = &tx_list;
      break;
    default:
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_alloc_sb_port: "
              "Unknown direction %d",direction);
      rc = CSD_EBADPARAM;
      break;
  }

  if( CSD_EOK == rc )
  {
    used = TRUE;
    /* Iterating through all possible afe port ids */
    while(temp_port_id <= AFE_PORT_ID_SLIMBUS_RANGE_END)
    {
      used = FALSE;
      curr = *head;
      prev = NULL;

      /* Checking if used or not */
      while(curr)
      {
        if(curr->port_id == temp_port_id)
        {
          used = TRUE;
          break;
        }
        else if(curr->port_id > temp_port_id)
        {
          /* As port ids are allocated in ascending order, this condition
           * indicates that temp_port_id is unused */
          break;
        }
        prev = curr;
        curr = curr->next;
      }

      if(!used)
      {
        /* found unused afe port id */
        break;
      }
      else
      {
        /* port id inuse, try another value */
        temp_port_id +=2 ;
      }
    }/*end of outer-most while*/

    if(used)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_alloc_sb_port: "
              "Unable to find unused port, last checked [%x]",
              temp_port_id);
      rc = CSD_ENORESOURCE;
    }
    else
    {
      /* port id free, allocate it and break */
      port_list_t *temp = NULL;

	  csd_malloc(sizeof(port_list_t), (void**) &(temp));

      if ( temp == NULL )
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_alloc_sb_port: "
                "Unable to malloc size[%d]",
                sizeof(port_list_t));
        rc = CSD_ENORESOURCE;
      }
      else
      {
        /* Populating temp and inserting in list */
        *afe_port_id = temp->port_id = temp_port_id;
        temp->ref_count = 0;
        temp->next = curr;
        if(!prev)
        {
          /* Only happens if head isn't present, creating new list */
          *head = temp;
        }
        else
        {
          prev->next = temp;
        }
      }
    }
  }

  if(CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_alloc_sb_port: "
            "Unable to allocate dynamic port, last checked "
            "port[%x], return afe port[%x], rc[%x]",
            temp_port_id, *afe_port_id, rc);
  }
  else
  {
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_pm_alloc_sb_port: "
            "New assigned port [%x]", *afe_port_id);
  }

  return rc;
}

/*==============================================================================

  FUNCTION      csd_dev_pm_sb_port_inc_ref_count

  DESCRIPTION   Incrementing reference count

  DEPENDENCIES  @see adsp_afe_service_commands.h

  PARAMETERS    @param afe_port_id : port for which reference is incremented

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_pm_sb_port_inc_ref_count(uint16_t afe_port_id)
{
  int32_t rc = CSD_EOK;
  port_list_t *list = NULL;

  if(afe_port_id < AFE_PORT_ID_SLIMBUS_RANGE_START ||
     afe_port_id > AFE_PORT_ID_SLIMBUS_RANGE_END)
  {
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_pm_sb_port_inc_ref_count: "
            "Port[%x] not managed by this module",afe_port_id);
    return CSD_EBADPARAM;
  }

  if(afe_port_id%2 == 0)
  {
    list = rx_list;
  }
  else
  {
    list = tx_list;
  }

  while(list)
  {
    if(afe_port_id == list->port_id)
    {
      break;
    }
    list = list->next;
  }

  if(NULL == list)
  {
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_pm_sb_port_inc_ref_count: "
            "Unable to find Port[%x]",afe_port_id);
    rc = CSD_ENOTFOUND;
  }
  else
  {
    list->ref_count++;
    CSD_MSG(CSD_PRIO_LOW,"csd_dev_pm_sb_port_inc_ref_count: "
            "Afe port id [%x], ref count [%d]",
            afe_port_id, list->ref_count);
  }

  return rc;
}

/*==============================================================================

  FUNCTION      csd_dev_pm_free_sb_port

  DESCRIPTION   Decrement reference count and free when reference hits zero

  DEPENDENCIES

  PARAMETERS   @param afe_port_id : port which is being freed
               @param freed: flag to indicated whether freed or not

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pm_free_sb_port(uint16_t afe_port_id,bool_t* freed)
{
  int32_t rc = CSD_EOK;
  port_list_t **head = NULL;
  port_list_t *curr = NULL;
  port_list_t *prev = NULL;

  if(NULL == freed)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_free_sb_port: "
            "Null Param, freed[%p]",((void*)freed));
    return CSD_EBADPARAM;
  }

  *freed = FALSE;

  if(afe_port_id < AFE_PORT_ID_SLIMBUS_RANGE_START ||
     afe_port_id > AFE_PORT_ID_SLIMBUS_RANGE_END)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_free_sb_port: "
            "Port[%x] not managed by this module",afe_port_id);
    return CSD_EBADPARAM;
  }

  if(afe_port_id%2 == 0)
  {
    head = &rx_list;
  }
  else
  {
    head = &tx_list;
  }

  curr = *head;
  prev = NULL;
  while(curr)
  {
    if(afe_port_id == curr->port_id)
    {
      break;
    }
    prev = curr;
    curr = curr->next;
  }

  if(!curr)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_pm_free_sb_port: "
            "Unable to find Port[%x]",afe_port_id);
    rc = CSD_ENOTFOUND;
  }
  else
  {
    if(curr->ref_count>0)
    {
      curr->ref_count--;
    }
    else
    {
      /* Not And Error, but a pre-matured disable - Freeing without using */
      CSD_MSG(CSD_PRIO_HIGH,"csd_dev_pm_free_sb_port: "
              "Unexpected ref count [%d], pre-matured freeing: "
              "Freeing without using",
              curr->ref_count);

      /* Setting count to 0 for removal */
      curr->ref_count = 0;

    }

    CSD_MSG(CSD_PRIO_LOW,"csd_dev_pm_free_sb_port: "
            "Afe port id [%x], ref count [%d]",
            afe_port_id, curr->ref_count);

    if(0 == curr->ref_count)
    {
      CSD_MSG(CSD_PRIO_LOW,"csd_dev_pm_free_sb_port: Removing freed port");
      if(!prev)
      {
        /* Deleting first entry */
        *head = curr->next;
      }
      else
      {
        prev->next = curr->next;
      }
      csd_free(curr);
      *freed = TRUE;
    }
  }

  return rc;
}
