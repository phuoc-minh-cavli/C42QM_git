/******************************************************************************

  @file    list.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2013 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    
 *******************************************************************************/

#include "internals.h"

/**
 * @fn lwm2m_list_t * lwm2m_list_add()
 * @brief This function is used to add node to the list
 *
 * @param head  pointer to the first node in the list
 * @param node  pointer to the new node to be added
 *
 * @return    pointer to the head node
 *
 */
lwm2m_list_t * lwm2m_list_add(lwm2m_list_t * head,
    lwm2m_list_t * node)
{
  lwm2m_list_t * target = NULL;

  if (NULL == head) return node;

  if (head->id > node->id)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->id <= node->id)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

/**
 * @fn lwm2m_list_t * lwm2m_list_find()
 * @brief This function is used to find a node in the list
 *
 * @param head  pointer to the first node in the list
 * @param id  id of the node to be searched
 *
 * @return    pointer to the node with give id on success
 *            NULL if id is not in the list
 */
lwm2m_list_t * lwm2m_list_find(lwm2m_list_t * head,
    uint16_t id)
{
  while (NULL != head && head->id != id)
  {
    head = head->next;
  }

  if (NULL != head && head->id == id) return head;

  return NULL;
}

/**
 * @fn lwm2m_list_t * lwm2m_list_remove()
 * @brief This function is used to remove node from list
 *
 * @param head  pointer to the first node in the list
 * @param id  id of the node to be removed
 * @param nodeP double pointer to the list
 *
 * @return    pointer to the head node of the list on success
 *            NULL if id is not in the list
 */
lwm2m_list_t * lwm2m_list_remove(lwm2m_list_t * head,
    uint16_t id,
    lwm2m_list_t ** nodeP)
{
  lwm2m_list_t * target;

  if (head == NULL)
  {
    if (nodeP) *nodeP = NULL;
    return NULL;
  }

  if (head->id == id)
  {
    if (nodeP) *nodeP = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->id != id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->id == id)
  {
    if (nodeP) *nodeP = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (nodeP) *nodeP = NULL;
  }

  return head;
}

/**
 * @fn uint16_t lwm2m_list_newId()
 * @brief This function is used to get new id value
 *
 * @param head  pointer to the first node in the list
 *
 * @return    new id
 *              
 */
uint16_t lwm2m_list_newId(lwm2m_list_t * head)
{
  uint32_t id;
  lwm2m_list_t * target;

  id = 0;
  target = head;

  while (target != NULL && id == target->id)
  {
    id = target->id + 1;
    target = target->next;
  }

  return id;
}

/**
 * @fn void lwm2m_list_free()
 * @brief This function is used to free the list
 *
 * @param head  pointer to the first node in the list
 *
 * @return    void
 *              
 */
void lwm2m_list_free(lwm2m_list_t * head)
{
  if (head != NULL)
  {
    lwm2m_list_t * nextP;

    nextP = head->next;
    lwm2m_free(head);
    lwm2m_list_free(nextP);
  }
}
/**
 * @fn void lwm2m_list_count()
 * @brief This function is used to count the list
 *
 * @param head  pointer to the first node in the list
 *
 * @return    uint8 count
 *              
 */
uint16_t lwm2m_list_count(lwm2m_list_t * head)
{
  uint16_t count = 0;
  while (NULL != head)
  {
    count++;
    head = head->next;
  }
  
  return count;
}

