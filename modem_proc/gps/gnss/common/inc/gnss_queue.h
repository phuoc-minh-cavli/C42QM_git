#ifndef GNSS_QUEUE_H
#define GNSS_QUEUE_H

/*==============================================================================

                  GNSS QUEUE Header

GENERAL DESCRIPTION
   This file contains class definition for GNSS queues

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_queue.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
12/15/2016  ahmadh Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include "customer.h"
#include "gnss_list.h"

namespace gnss_StlNameSpace
{
  template <typename T>
  class gnss_QueueClass 
  {
  public:
    size_t GetSize() const
    {
      return sizeof(*this);
    }

    gnss_QueueClass()
    {
      m_List.Init();
    }

    virtual ~gnss_QueueClass()
    {
      clear();
    }

    void clear()
    {
      m_List.clear();
    }

    bool push(T & Data)
    {
      return m_List.push_front(Data);
    }

    /* Removes the element which was inserted first */
    void pop()
    {
      m_List.pop_back();
    }

    /* Returns the element which was inserted last */
    T * back()
    {
      return m_List.front();
    }

    /* Returns the element which was inserted first */
    T * front()
    {
      return m_List.back();
    }

    size_t getNumOfElements() const
    {
      return m_List.getNumOfElements();
    }
   
  private:
    gnss_ListClass<T>  m_List;
  };
}
#endif /* GNSS_QUEUE_H */
