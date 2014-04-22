/*! \file cad_list.c
  \brief This source file implements generic list utility functions
  
  Copyright (c) 2010 - 2012, 2016 QUALCOMM Technologies Incorporated.
  All Rights Reserved. 
  Qualcomm Proprietary and Confidential.
*/
/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/src/csd_list.c#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/16     rk        Abstracting OSAL layer

============================================================================*/
#include "csd.h"
#include "csd_list.h"
#include "DALSys.h"

int32_t csd_list_init(struct csd_list *list)
{
  if (NULL == list)
  {
    return CSD_EFAILED;
  }
  else if (list->lock != NULL)
  {
    goto end;
  }
  csd_memset(list, 0, sizeof(*list));
  if ( CSD_OSAL_EOK != csd_lock_create (&(list->lock)))
  {
    return CSD_ENORESOURCE;
  }
end:
  return CSD_EOK;
}

int32_t csd_list_dinit(struct csd_list *list)
{
  int32_t	rc = CSD_EOK;
  if (list == NULL || list->lock == NULL)
  {
    goto end;
  }
  if (list->list_head != NULL)
  {
    rc = CSD_EFAILED;
  }
  csd_lock_destroy(list->lock);
  csd_memset(list, 0, sizeof(*list));
end:
  return rc;
}

int32_t csd_list_add_head(struct csd_list *list, struct csd_list_node *node)
{
  if (node == NULL || list == NULL || list->lock == NULL)
  {
    goto err;
  }
  csd_lock_enter(list->lock);
  node->next = NULL;
  if (list->list_head == NULL)
  {
    list->list_head = node;
    list->list_tail = node;
  }
  else
  {
    node->next = list->list_head;
    list->list_head = node;
  }
  list->count++;
  csd_lock_leave(list->lock);
  return CSD_EOK;
err:
  return CSD_EFAILED;
}

int32_t csd_list_add_tail_nolock(struct csd_list *list, struct csd_list_node *node)
{
  if (node == NULL || list == NULL || list->lock == NULL)
  {
    goto err;
  }
  node->next = NULL;
  if (list->list_head == NULL)
  {
    list->list_head = node;
    list->list_tail = node;
  }
  else
  {
    list->list_tail->next = node;
    list->list_tail = node;
  }
  list->count++;
  return CSD_EOK;
err:
  return CSD_EFAILED;
}

int32_t csd_list_get_head(struct csd_list *list, struct csd_list_node **node)
{
  if (node == NULL || list == NULL || list->lock == NULL)
  {
    goto err1; //address klocwork ID 737
  }
  *node = NULL;
  csd_lock_enter(list->lock);
  *node = list->list_head;
  if (list->list_head == NULL)
  {
    goto err;
  }
  if (list->list_head->next == NULL)
  {
    list->list_head = NULL;
    list->list_tail = NULL;
  }
  else
  {
    list->list_head = list->list_head->next;
  }
  list->count--;
  (*node)->next = NULL;
  csd_lock_leave(list->lock);
  return CSD_EOK;
err:
  csd_lock_leave(list->lock);
err1:
  return CSD_EFAILED;
}

int32_t csd_list_read_head(struct csd_list *list, struct csd_list_node **node)
{
  if (node == NULL || list == NULL || list->lock == NULL)
  {
    return CSD_EFAILED;
  }
  csd_lock_enter(list->lock);
  if (NULL == list->list_head)
  {
    *node = NULL;
    csd_lock_leave(list->lock);
    return CSD_EFAILED;
  }
  *node = list->list_head;
  csd_lock_leave(list->lock);
  return CSD_EOK;
}

int32_t csd_list_read_next_nolock(struct csd_list *list, struct csd_list_node **node)
{
  struct csd_list_node *next = NULL;
  if (node == NULL)
  {
    return CSD_EFAILED;
  }

  if (NULL == list->list_head)
  {
    *node = NULL;
    return CSD_ENOTFOUND;
  }
  // Empty input => read head node
  if (NULL == *node)
  {
    next = list->list_head;
  }
  // Read next node from the curent node
  else
  {
    next = (*node)->next;
  }
  *node = next;

  if (NULL == next)
  {
    return CSD_ENOTFOUND;
  }
  return CSD_EOK;
}

int32_t csd_list_read_next(struct csd_list *list, struct csd_list_node **node)
{
  int32_t rc = CSD_EOK;
  if (list == NULL || list->lock == NULL)
  {
    return CSD_EFAILED;
  }
  csd_lock_enter(list->lock);
  rc = csd_list_read_next_nolock(list, node);
  csd_lock_leave(list->lock);
  return rc;
}

int32_t csd_list_init_key(struct csd_list *list, csd_list_find_fn find_fn)
{
  int32_t rc = csd_list_init(list);
  if (rc != CSD_EOK)
  {
    return rc;
  }
  list->find_fn = find_fn;
  list->count = 0;
  return rc;
}

int32_t csd_list_find(struct csd_list *list, struct csd_list_node **node, uint32_t key)
{
  int32_t rc = CSD_EOK;

  if (NULL == list)
  {
    return CSD_EFAILED;
  }

  csd_lock_enter(list->lock);
  rc = csd_list_find_nolock(list, node, key);
  csd_lock_leave(list->lock);

  return rc;
}

int32_t csd_list_find_nolock(struct csd_list *list, struct csd_list_node **node, uint32_t key)
{
  struct csd_list_node *next = NULL;
  next = list->list_head;
  while (next)
  {
    if ((list->find_fn)(next, key) == TRUE)
      break;
    else
      next = next->next;
  }
  *node = next;
  if (next)
    return CSD_EOK;
  else
    return CSD_ENOTFOUND;
}

int32_t csd_list_find_next(struct csd_list *list, struct csd_list_node **node, uint32_t key)
{
  int32_t rc = CSD_EOK;
  csd_lock_enter(list->lock);
  rc = csd_list_find_next_nolock(list, node, key);
  csd_lock_leave(list->lock);
  return rc;
}

int32_t csd_list_find_next_nolock(struct csd_list *list, struct csd_list_node **node, uint32_t key)
{
  struct csd_list_node *next = NULL;
  if (NULL == node || NULL == *node)
  {
    next = list->list_head;
  }
  else
  {
    next = (*node)->next;
  }
  while (next)
  {
    if ((list->find_fn)(next, key) == TRUE)
      break;
    else
      next = next->next;
  }
  if (node)
    *node = next;

  if (next)
    return CSD_EOK;
  else
    return CSD_ENOTFOUND;
}

int32_t csd_list_del(struct csd_list *list, struct csd_list_node *node)
{
  struct csd_list_node *prev = NULL;
  struct csd_list_node *next = NULL;

  if (NULL == list)
  {
    return CSD_EFAILED;
  }

  csd_lock_enter(list->lock);
  next = list->list_head;
  while (next)
  {
    if (next == node)
      break;
    else
    {
      prev = next;
      next = next->next;
    }
  }
  if (next)
  {
    /* found the node */
    if (next != list->list_head && next != list->list_tail)
    {
      prev->next = next->next;
    }
    else
    {
      if (next == list->list_head)
      {
        list->list_head = next->next;
      }
      if (next == list->list_tail)
      {
        list->list_tail = prev;
        if (prev)
        {
          prev->next = NULL;
        }
      }
    }
    next->next = NULL;
    list->count--;
  }
  csd_lock_leave(list->lock);
  return CSD_EOK;
}

uint32_t csd_list_get_count(struct csd_list *list)
{
  int32_t count = 0;

  if (NULL == list)
  {
    return 0;
  }

  csd_lock_enter(list->lock);
  count = list->count;
  csd_lock_leave(list->lock);
  return count;
}

int32_t csd_list_lock(struct csd_list *list)
{
  if (list == NULL || list->lock == NULL)
  {
    return CSD_EFAILED;
  }
  csd_lock_enter(list->lock);
  list->locked = TRUE;
  return CSD_EOK;
}

int32_t csd_list_unlock(struct csd_list *list)
{
  if (list == NULL || list->lock == NULL)
  {
    return CSD_EFAILED;
  }
  if (TRUE != list->locked)
  {
    return CSD_EFAILED;
  }
  csd_lock_leave(list->lock);
  list->locked = FALSE;
  return CSD_EOK;
}

int32_t csd_list_add(struct csd_list *list, struct csd_list_node *node, void *data)
{
  node->data = data;
  return csd_list_add_tail(list, node);
}

int32_t csd_list_add_tail(struct csd_list *list, struct csd_list_node *node)
{
  int32_t rc = CSD_EFAILED;
  if (node == NULL || list == NULL || list->lock == NULL)
  {
    goto end;
  }
  csd_lock_enter(list->lock);
  rc = csd_list_add_tail_nolock(list, node);
  csd_lock_leave(list->lock);
end:
  return rc;
}

int32_t csd_list_set_key2(struct csd_list *list, csd_list_find_fn2 find_fn)
{
  if (list)
  {
    list->find_fn2 = find_fn;
  }

  return CSD_EOK;
}
int32_t csd_list_init_key2(struct csd_list *list, csd_list_find_fn2 find_fn)
{
  int32_t rc = csd_list_init(list);
  if (rc != CSD_EOK)
    return rc;
  list->find_fn2 = find_fn;
  list->count = 0;
  return rc;
}

int32_t csd_list_find2(struct csd_list *list, struct csd_list_node **node, void *pKeyData)
{
  int32_t rc = CSD_EOK;

  if (NULL == list)
  {
    return CSD_EFAILED;
  }

  csd_lock_enter(list->lock);
  rc = csd_list_find_nolock2(list, node, pKeyData);
  csd_lock_leave(list->lock);
  return rc;
}

int32_t csd_list_find_nolock2(struct csd_list *list, struct csd_list_node **node, void *pKeyData)
{
  struct csd_list_node *next = NULL;
  next = list->list_head;
  while (next)
  {
    if (list->find_fn2)
    {
      if ((list->find_fn2)(next, pKeyData) == TRUE)
        break;
      else
        next = next->next;
    }
    else if (list->find_fn)
    {
      if ((list->find_fn)(next, (uint32_t)pKeyData) == TRUE)
        break;
      else
        next = next->next;
    }
  }
  *node = next;
  if (next)
    return CSD_EOK;
  else
    return CSD_ENOTFOUND;
}
