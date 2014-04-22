/******************************************************************************

  @file    memtrace.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2015 Bosch Software Innvoations GmbH and others.
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
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/

#include "internals.h"
#include "ds_log.h"


#ifdef LWM2M_MEMORY_TRACE

#undef malloc
#undef free
#undef strdup

typedef struct MemoryEntry {
  struct MemoryEntry* next;
  const char *file;
  const char *function;
  int         lineno;
  size_t      size;
  int         count;
  uint32_t    data[1];
} memory_entry_t;

static memory_entry_t prv_memory_malloc_list = { .next = NULL, .file = "head", .function="malloc", .lineno = 0, .size = 0, .count = 0};
static memory_entry_t prv_memory_free_list = { .next = NULL, .file = "head", .function="free", .lineno = 0, .size = 0, .count = 0};

/**
 * @fn static memory_entry_t* prv_memory_find_previous()
 * @brief This function is used to find previous node from the list
 *
 * @param list      pointer to the list
 * @param memory    data in the node
 *
 * @return    pointer to the memory_entry_t node on success
 *        NULL on failure
 */
static memory_entry_t* prv_memory_find_previous(memory_entry_t* list, 
    void* memory)
{
  while (NULL != list->next)
  {
    if (list->next->data == memory)
    {
      return list;
    }
    list = list->next;
  }
  return NULL;
}

/**
 * @fn static void prv_trace_add_free_list()
 * @brief This function is used to add free list 
 *
 * @param remove  pointer to the list
 * @param file      pointer to file 
 * @param function  pointer to function 
 * @param lineno    line number
 *
 * @return    void
 *        
 */
static void prv_trace_add_free_list(memory_entry_t* remove, 
    const char* file, const char* function, int lineno)
{
  remove->next = prv_memory_free_list.next;
  prv_memory_free_list.next = remove;
  remove->file = file;
  remove->function = function;
  remove->lineno = lineno;

  if (prv_memory_free_list.count < 200)
  {
    ++prv_memory_free_list.count;
  }
  else if (NULL != remove->next)
  {
    while (NULL != remove->next->next)
    {
      remove = remove->next;
    }
    free(remove->next);
    remove->next = NULL;
  }
}


/**
 * @fn char* lwm2m_trace_strdup()
 * @brief This function is used to allocate new memory and copy the str
 *
 * @param str     pointer to the str
 * @param file      pointer to file 
 * @param function  pointer to function 
 * @param lineno    line number
 *
 * @return      pointer to newly allocated memroy
 *        
 */
char* lwm2m_trace_strdup(const char* str, const char* file, 
    const char* function, int lineno)
{
  size_t length = strlen(str);
  char* result = lwm2m_trace_malloc(length +1, file, function, lineno);
  LWM2M_MEMCPY(result, str, length);
  result[length] = 0;
  return result;
}

/**
 * @fn void* lwm2m_trace_malloc()
 * @brief This function is used to allocate memory 
 *
 * @param size      size to be allocated
 * @param file      pointer to file 
 * @param function  pointer to function 
 * @param lineno    line number
 *
 * @return      void pointer to newly allocated memroy
 *        
 */
void* lwm2m_trace_malloc(size_t size, const char* file, 
    const char* function, int lineno)
{
  static int counter = 0;
  memory_entry_t* entry = malloc(size + sizeof(memory_entry_t));
  entry->next = prv_memory_malloc_list.next;
  prv_memory_malloc_list.next = entry;
  ++prv_memory_malloc_list.count;
  prv_memory_malloc_list.size += size;
  prv_memory_malloc_list.lineno = 1;

  entry->file = file;
  entry->function = function;
  entry->lineno = lineno;
  entry->size = size;
  entry->count = ++counter;

  return &(entry->data);
}

/**
 * @fn void lwm2m_trace_free()
 * @brief This function is used to free memory 
 *
 * @param mem     void pointer to be free
 * @param file      pointer to file 
 * @param function  pointer to function 
 * @param lineno    line number
 *
 * @return      void 
 *        
 */
void lwm2m_trace_free(void* mem, const char* file, const char* function, 
    int lineno)
{
  if (NULL != mem)
  {
    memory_entry_t* entry = prv_memory_find_previous(&prv_memory_malloc_list, mem);
    if (NULL != entry)
    {
      memory_entry_t* remove = entry->next;
      entry->next = remove->next;
      --prv_memory_malloc_list.count;
      prv_memory_malloc_list.size -= remove->size;
      prv_memory_malloc_list.lineno = 1;
      prv_trace_add_free_list(remove, file, function, lineno);
    }
    else
    {
      ds_iot_log_strf_high("LWM2M_LOG:  free error (no malloc) %s, %d, %s", file, lineno, function);
      memory_entry_t* entry = prv_memory_find_previous(&prv_memory_free_list, mem);
      if (NULL != entry)
      {
        entry = entry->next;
        ds_iot_log_strf_high("LWM2M_LOG:  already frees at %s, %d, %s", entry->file, entry->lineno, entry->function);
      }
    }
  }
}

/**
 * @fn void trace_print()
 * @brief This function is used to print the memory trace
 *
 * @param loops     number of loops
 * @param level     level number
 *
 * @return      void 
 *        
 */
void trace_print(int loops, int level)
{
  static int counter = 0;
  if (0 == loops)
  {
    counter = 0;
  }
  else
  {
    ++counter;
  }
  if (0 == loops || (((counter % loops) == 0) && prv_memory_malloc_list.lineno))
  {
    prv_memory_malloc_list.lineno = 0;
    if (1 == level)
    {
      size_t total = 0;
      int entries = 0;
      memory_entry_t* entry = prv_memory_malloc_list.next;
      while (NULL != entry)
      {
        ds_iot_log_strf_high("LWM2M_LOG:  #%d, %lu bytes, %s, %d, %s\n", entry->count, (unsigned long) entry->size, entry->file, entry->lineno, entry->function);
        ++entries;
        total += entry->size;
        entry = entry->next;
      }
      if (entries != prv_memory_malloc_list.count)
      {
        ds_iot_ulog_high2("LWM2M_LOG:  error %d entries != %d", prv_memory_malloc_list.count, entries);
      }
      if (total != prv_memory_malloc_list.size)
      {
        ds_iot_ulog_high2("LWM2M_LOG:  error %lu total bytes != %lu", (unsigned long) prv_memory_malloc_list.size, (unsigned long) total);
      }
    }
    ds_iot_ulog_high2("LWM2M_LOG:  %d entries, %lu total bytes", prv_memory_malloc_list.count, (unsigned long) prv_memory_malloc_list.size);
  }
}

/**
 * @fn void trace_status()
 * @brief This function is used to update status
 *
 * @param blocks  pointer to the number of blocks
 * @param size      pointer to the size 
 *
 * @return      void 
 *        
 */
void trace_status(int* blocks, size_t* size)
{
  if (NULL != blocks)
  {
    *blocks = prv_memory_malloc_list.count;
  }

  if (NULL != size)
  {
    *size = prv_memory_malloc_list.size;
  }
}

#endif
