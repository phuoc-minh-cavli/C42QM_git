#ifndef _CSD_LIST_H_
#define _CSD_LIST_H_

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/inc/csd_list.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/16   rk      Abstracting OSAL layer
07/06/09   jzhou   Initial Creation

============================================================================*/

/*! \file csd_list.h
    \brief This file defines list utility functions for CSD
    
    Copyright (c) 2010 - 2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved. 
    Qualcomm Proprietary and Confidential.

*/
#include "mmdefs.h"
#include "DALSys.h"
#include "csd_os_dependencies.h"


struct csd_list_node 
{
    struct csd_list_node *next;
    void*   data;
};

typedef bool_t (*csd_list_find_fn)(struct csd_list_node* node, uint32_t key);
typedef bool_t (*csd_list_find_fn2)(struct csd_list_node* node, void *pKeyData);
struct csd_list {
    csd_lock_t    lock;
    csd_list_find_fn    find_fn;
    csd_list_find_fn2   find_fn2;
    uint32_t            count;
    bool_t              locked;
    struct csd_list_node*   list_head;
    struct csd_list_node*   list_tail;
};

int32_t csd_list_init(struct csd_list* list);
int32_t csd_list_dinit(struct csd_list* list);
int32_t csd_list_add_head(struct csd_list* list, struct csd_list_node* node);
int32_t csd_list_add_tail(struct csd_list* list, struct csd_list_node* node);
int32_t csd_list_get_head(struct csd_list* list, struct csd_list_node** node);
int32_t csd_list_read_head(struct csd_list* list, struct csd_list_node** node); // Peek Head Node without removing it from the list
int32_t csd_list_read_next(struct csd_list* list, struct csd_list_node** node); // Read next item in the list without removing it


/* linked list enhancement */
int32_t csd_list_init_key(struct csd_list* list, csd_list_find_fn find_fn);
int32_t csd_list_del(struct csd_list* list, struct csd_list_node* node);
uint32_t csd_list_get_count(struct csd_list* list);
int32_t csd_list_add(struct csd_list* list, struct csd_list_node* node, void* data);
int32_t csd_list_find(struct csd_list* list, struct csd_list_node** node, uint32_t key);
int32_t csd_list_find_next(struct csd_list* list, struct csd_list_node** node, uint32_t key);
int32_t csd_list_lock(struct csd_list* list);
int32_t csd_list_unlock(struct csd_list* list);

/* no lock version of functions. The call must get the lock before call this function :: useful for recursive iterations on the list */
int32_t csd_list_find_nolock(struct csd_list* list, struct csd_list_node** node, uint32_t key);
int32_t csd_list_add_tail_nolock(struct csd_list* list, struct csd_list_node* node);
int32_t csd_list_find_next_nolock(struct csd_list* list, struct csd_list_node** node, uint32_t key);
int32_t csd_list_read_next_nolock(struct csd_list* list, struct csd_list_node** node); // Read next item in the list without removing it

/* multi key search/addition list enhancement */
int32_t csd_list_init_key2(struct csd_list* list, csd_list_find_fn2 find_fn);
int32_t csd_list_set_key2(struct csd_list* list, csd_list_find_fn2 find_fn);
int32_t csd_list_find2(struct csd_list* list, struct csd_list_node** node, void* pKeyData);
int32_t csd_list_find_nolock2(struct csd_list* list, struct csd_list_node** node, void* pKeyData);

#endif /* _CSD_LIST_H_ */
