/*!
  @file
  mutils_q.h

  @brief
  Interface for generic lockless queue.
*/

/*==============================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/utils.mpss/2.3.4/api/common/collections/mutils_q.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/04/17   mm      CR 2027602: Initial checkin
==============================================================================*/

#ifndef MUTILS_Q_H
#define MUTILS_Q_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <mutils_common.h>
#include <atomic_ops.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*!
  Link type.
*/
typedef struct mutils_q_link_s mutils_q_link_s;
struct mutils_q_link_s
{
  /*! ptr to next element in queue */
  mutils_q_link_s *  next_ptr;
};

/*!
  Maintains top-level reference to queue.
*/
typedef struct
{
  /*! union to contain links for atomic access */
  union
  {
    struct
    {
      /*! ptr to head element */
      mutils_q_link_s *  head_ptr;

      /*! ptr to tail element */
      mutils_q_link_s *  tail_ptr;
    }       link;

    uint64  atomic;
  }       link_u;

  /*! current number of elements */
  uint32  count;
} mutils_q_s;

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*!
  Macros to pack/unpack links.
*/
#define MUTILS_Q_PACK(atomic64, head_ptr, tail_ptr) \
  do { \
    (atomic64) = ((((uint64) (tail_ptr)) << 32) | ((uint32) (head_ptr))); \
  } while (0)

#define MUTILS_Q_UNPACK(atomic64, head_ptr, tail_ptr) \
  do { \
    (head_ptr) = (mutils_q_link_s *) (atomic64); \
    (tail_ptr) = (mutils_q_link_s *) ((atomic64) >> 32); \
  } while (0)

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  mutils_q_init

==============================================================================*/
/*!
  @brief
  Initializes queue.
*/
/*============================================================================*/
static inline void mutils_q_init
(
  mutils_q_s *   q_ptr
)
{
  q_ptr->link_u.link.head_ptr = NULL;
  q_ptr->link_u.link.tail_ptr = NULL;
  q_ptr->count = 0;
}

/*==============================================================================

  FUNCTION:  mutils_q_push_head

==============================================================================*/
/*!
  @brief
  Head insertion, returning new # links.
*/
/*============================================================================*/
static inline uint32 mutils_q_push_head
(
  mutils_q_s *       q_ptr,
  mutils_q_link_s *  link_ptr
)
{
  mutils_q_link_s * head_ptr;
  mutils_q_link_s * tail_ptr;
  uint64 old64, new64;

  do
  {
    /* get current links */
    old64 = atomic64_read((atomic64_word_t *) &q_ptr->link_u.atomic);

    /* unpack */
    MUTILS_Q_UNPACK(old64, head_ptr, tail_ptr);

    /* handle tail ptr */
    if (tail_ptr == NULL)
    {
      /* if tail ptr is empty, link_ptr is new tail ptr */
      tail_ptr = link_ptr;
    }

    /* chain old head ptr */
    link_ptr->next_ptr = head_ptr;

    /* pack */
    MUTILS_Q_PACK(new64, link_ptr, tail_ptr);
  }
  while (atomic64_compare_and_set(
    (atomic64_word_t *) &q_ptr->link_u.atomic,
    old64,
    new64) == FALSE);

  return atomic_inc_return((atomic_word_t *) &q_ptr->count);
}

/*==============================================================================

  FUNCTION:  mutils_q_push_tail

==============================================================================*/
/*!
  @brief
  Tail insertion, returning new # links.
*/
/*============================================================================*/
static inline uint32 mutils_q_push_tail
(
  mutils_q_s *       q_ptr,
  mutils_q_link_s *  link_ptr
)
{
  mutils_q_link_s * head_ptr;
  mutils_q_link_s * tail_ptr;
  uint64 old64, new64;

  link_ptr->next_ptr = NULL;

  do
  {
    /* get current links */
    old64 = atomic64_read((atomic64_word_t *) &q_ptr->link_u.atomic);

    /* unpack */
    MUTILS_Q_UNPACK(old64, head_ptr, tail_ptr);

    /* handle head ptr */
    if (head_ptr == NULL)
    {
      /* if head ptr is empty, link_ptr is new head ptr */
      head_ptr = link_ptr;
    }

    /* chain old tail ptr */
    if (tail_ptr != NULL)
    {
      tail_ptr->next_ptr = link_ptr;
    }

    /* pack */
    MUTILS_Q_PACK(new64, head_ptr, link_ptr);
  }
  while (atomic64_compare_and_set(
    (atomic64_word_t *) &q_ptr->link_u.atomic,
    old64,
    new64) == FALSE);

  return atomic_inc_return((atomic_word_t *) &q_ptr->count);
}

/*==============================================================================

  FUNCTION:  mutils_q_pop_head

==============================================================================*/
/*!
  @brief
  Removes and returns head element.
*/
/*============================================================================*/
static inline mutils_q_link_s * mutils_q_pop_head
(
  mutils_q_s *   q_ptr
)
{
  mutils_q_link_s * head_ptr;
  mutils_q_link_s * tail_ptr;
  mutils_q_link_s * ret;
  uint64 old64, new64;

  do
  {
    /* get current links */
    old64 = atomic64_read((atomic64_word_t *) &q_ptr->link_u.atomic);

    /* unpack */
    MUTILS_Q_UNPACK(old64, head_ptr, tail_ptr);

    ret = head_ptr;

    if (head_ptr != NULL)
    {
      /* chain next ptr */
      head_ptr = head_ptr->next_ptr;

      if (ret == tail_ptr)
      {
        /* clear tail ptr if removing last link */
        tail_ptr = NULL;
      }
    }

    /* pack */
    MUTILS_Q_PACK(new64, head_ptr, tail_ptr);
  }
  while (atomic64_compare_and_set(
    (atomic64_word_t *) &q_ptr->link_u.atomic,
    old64,
    new64) == FALSE);

  if (ret != NULL)
  {
    atomic_dec((atomic_word_t *) &q_ptr->count);
    ret->next_ptr = NULL;
  }

  return ret;
}

/*==============================================================================

  FUNCTION:  mutils_q_peek_head

==============================================================================*/
/*!
  @brief
  Returns head element without removal.
*/
/*============================================================================*/
static inline mutils_q_link_s * mutils_q_peek_head
(
  mutils_q_s *   q_ptr
)
{
  return q_ptr->link_u.link.head_ptr;
}

/*==============================================================================

  FUNCTION:  mutils_q_peek_tail

==============================================================================*/
/*!
  @brief
  Returns tail element without removal.
*/
/*============================================================================*/
static inline mutils_q_link_s * mutils_q_peek_tail
(
  mutils_q_s *   q_ptr
)
{
  return q_ptr->link_u.link.tail_ptr;
}

/*==============================================================================

  FUNCTION:  mutils_q_peek_next

==============================================================================*/
/*!
  @brief
  Returns next element without removal.
*/
/*============================================================================*/
static inline mutils_q_link_s * mutils_q_peek_next
(
  mutils_q_link_s *  link_ptr
)
{
  return link_ptr->next_ptr;
}

#endif /* MUTILS_Q_H */
