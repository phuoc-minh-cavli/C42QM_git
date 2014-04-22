/*=============================================================================

                            BAM DMUX LIST

 BAM Data Multiplexer list

 Copyright  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/inc/bam_dmux_list.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

#ifndef BAM_DMUX_LIST_H
#define BAM_DMUX_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/

#define BAM_DMUX_LIST(type, name) \
  struct { \
    type *head; \
    type *tail; \
    unsigned int count; \
  } name

#define BAM_DMUX_LINK(type, link) \
  struct { \
    type *prev; \
    type *next; \
  } link

#define BAM_DMUX_LIST_INIT(list) do {  \
  (list).head = (list).tail = NULL; \
  (list).count = 0; \
} while(0)

#define BAM_DMUX_LINK_INIT(link) (link).next = (link).prev = NULL

#define BAM_DMUX_LIST_HEAD(list) (list.head)

#define BAM_DMUX_LIST_TAIL(list) (list.tail)

#define BAM_DMUX_LIST_CNT(list) (list.count)

#define BAM_DMUX_LIST_REMOVE(list, node, link) \
  do { \
    if((node)->link.prev) \
      (node)->link.prev->link.next = (node)->link.next; \
    else /* node at the front of the list */ \
      list.head = (node)->link.next; \
    if((node)->link.next) \
      (node)->link.next->link.prev = (node)->link.prev; \
    else /* node at the end of the list */ \
      list.tail = (node)->link.prev; \
    list.count--; \
  } while(0)

#define BAM_DMUX_LIST_ADD(list, node, link) \
  do { \
    if(!list.tail) \
    { \
      /* first node on the list */ \
      list.tail = list.head = (node); \
    } \
    else \
    { \
      (node)->link.prev = list.tail; \
      list.tail->link.next = (node); \
      list.tail = (node); \
    } \
    list.count++; \
  } while(0)

#define BAM_DMUX_LIST_FIND(list, iter, link, test) do {  \
  iter = (list).head; \
  while(iter) { \
    if(test) {  \
      break;  \
    } \
    iter = (iter)->link.next;  \
  } \
} while(0)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* BAM_DMUX_LIST_H */
