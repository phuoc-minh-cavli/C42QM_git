#ifndef GNSS_LIST_H
#define GNSS_LIST_H

/*==============================================================================

                  GNSS List Header

GENERAL DESCRIPTION
   This file contains class definition for GNSS lists

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2019 Qualcomm Technologies, Inc. 
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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_list.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
6/09/2015  ahmadh Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include "customer.h"
#include "gnss_hal.h"

#define C_GNSS_LIST_MAX_RECURSIVE_LEVEL   (5)

namespace gnss_StlNameSpace
{
  template <typename T>
  class MergeSortCompFunctionBase
  {
  public:
    /*To avoid instantiating base class */
    virtual bool isSortCompFunImplemented() = 0;
    /*=============================================================================

    FUNCTION      sortComp()

    DESCRIPTION
    function to be overriden by derived class to implement comparison function of 
    2 gnss list element's and return true if first and second are in order expected by 
    caller. 

    DEPENDENCIES

    PARAMETERS
    Gnss List Element        - first gnss list element.
    (input)

    Gnss List Element       - second gnss list element .
    (input)

    RETURN VALUE
    bool : true : if first and second are in order( first should follow second in sorted list )
                  expected and need not swaped.
           false : if first and second are NOT in expected order ( second should follow first in sorted
                   list)

    SIDE EFFECTS

    =============================================================================*/
    virtual bool sortComp(T &first, T &second) 
    {
      return false;
    }
  };

  template <typename T>
  struct ListElement
  {
    T * m_pData;
    ListElement * m_pNext;
    ListElement * m_pPrev;

    size_t GetSize() const
    {
      return sizeof(*this);
    }

    T * operator * () 
    { 
      return m_pData; 
    }

    T * operator ++ () 
    {
      if( NULL != m_pNext )
      {
        m_pData = m_pNext->m_pData;
        m_pPrev = m_pNext->m_pPrev;
        m_pNext = m_pNext->m_pNext;
      }

      return m_pData;
    }

    T* GetData()
    {
      return m_pData; 
    }

    ListElement(): m_pData(NULL),m_pNext(NULL),m_pPrev(NULL)
    {
    }
    ~ListElement() 
    {
      /* The list implementation does not delete the content of the objects, caller 
         needs to deallocate the contents */
    }
  };

  template <typename T>
  class gnss_ListClass 
  {
  public:
    size_t GetSize() const
    {
      return sizeof(*this);
    }
    
    struct iterator
    {
      void reset()
      {
        m_Content.m_pData = NULL;
        m_Content.m_pNext = NULL;
        m_Content.m_pPrev = NULL;
      }

      iterator()
      {
        reset();
      }

      iterator(ListElement<T> * pContent)
      {
        if(NULL != pContent)
        {
          m_Content.m_pData = pContent->m_pData;
          m_Content.m_pNext = pContent->m_pNext;
          m_Content.m_pPrev = pContent->m_pPrev;
        }
        else
        {
          reset();
        }
      }

      T * operator * () 
      { 
        return m_Content.m_pData; 
      }

      iterator operator ++ () 
      {
        if(NULL != m_Content.m_pNext)
        {
          m_Content.m_pData = m_Content.m_pNext->m_pData;
          m_Content.m_pPrev = m_Content.m_pNext->m_pPrev;
          m_Content.m_pNext = m_Content.m_pNext->m_pNext;
          return iterator(&m_Content);
        } 
        else
        {
          return iterator();
        }
      }
     
/*=============================================================================

  FUNCTION      Assignment operator

  DESCRIPTION

  DEPENDENCIES  

  PARAMETERS
    rhs - iterator to be assigned
    (input)
 
  RETURN VALUE
     
  SIDE EFFECTS  

=============================================================================*/
      iterator operator = (const iterator & rhs)
      {
        if(this == &rhs)
        {
          return *this;
        }

        m_Content.m_pData = rhs.m_Content.m_pData;
        m_Content.m_pNext = rhs.m_Content.m_pNext;
        m_Content.m_pPrev = rhs.m_Content.m_pPrev;

        return *this;
      }

/*=============================================================================

  FUNCTION      Decrement operator

  DESCRIPTION

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
     
  SIDE EFFECTS  

=============================================================================*/
      iterator operator -- () 
      {
        if(NULL != m_Content.m_pPrev)
        {
          m_Content.m_pData = m_Content.m_pPrev->m_pData;
          m_Content.m_pNext = m_Content.m_pPrev->m_pNext;
          m_Content.m_pPrev = m_Content.m_pPrev->m_pPrev;

          return iterator(&m_Content);
        } 
        else
        {
          return iterator();
        }
      }

      bool operator==(const iterator &rhs) 
      {
        if( (m_Content.m_pData == rhs.m_Content.m_pData) &&
          (m_Content.m_pNext == rhs.m_Content.m_pNext) &&
          (m_Content.m_pPrev == rhs.m_Content.m_pPrev) )
        {
          return true;
        }

        return false;
      }

      bool operator!=(const iterator &rhs) 
      {
        if(*this == rhs)
        {
          return false;
        }

        return true;
      }


      ListElement<T>  m_Content;
    }; 

    void Init(void)
    {
      m_NumOfElements = 0;
      m_pHead = NULL;
      m_End.m_pData = NULL;
      m_End.m_pNext = NULL;
      m_End.m_pPrev = NULL;
      m_qUserFlag = 0;
    }

    gnss_ListClass()
    {
      Init();
    }

    virtual ~gnss_ListClass()
    {
      clear();
    }

    void clear();
    void clearall(bool isDelListElementData = false);
    void clearallint(ListElement<T> * pElement, bool isDelListElementData = false);

    bool push_back(T & Data);
    bool push_front(T & Data);

    T * back();
    T * front();

    void pop_back();
    void pop_front();

    T* pop_front_ext();
    ListElement<T>* push_back_ext(T & Data);
    ListElement<T>* push_front_ext(T & Data);

    bool delete_node(ListElement<T>* pDelNode);

    bool delete_node_with_data(T* pDelData);
    
    ListElement<T>* search_list_for_data(T* pData);

    //  T & operator [](const unsigned int index);
    //  const T & operator [](const unsigned int index) const;
    //  const gnss_ListClass<T> & operator =(const gnss_ListClass<T> & rhs);

    size_t getNumOfElements() const
    {
      return m_NumOfElements;
    }
   
    iterator erase(iterator &pos)
    {
      iterator prevPos = pos;
      iterator nextPos = pos;
      ListElement<T> * pNextElement = NULL;
      ListElement<T> * pPrevElement = NULL;
      ListElement<T> * pThisElement = NULL;

      if(NULL != pos.m_Content.m_pNext)
      {
        pThisElement = pos.m_Content.m_pNext->m_pPrev;
      } 

      --prevPos;
      ++nextPos;

      if( (NULL == pThisElement) ||
          (m_NumOfElements == 0) )
      {
        return iterator(nextPos);
      }
  
      if(NULL != pos.m_Content.m_pPrev)
      {
        pPrevElement = pos.m_Content.m_pPrev;
      }

      if(NULL != pos.m_Content.m_pNext)
      {
        pNextElement = pos.m_Content.m_pNext;
        pNextElement->m_pPrev = pPrevElement;
      }
      else
      {
        m_End.m_pPrev = pPrevElement;
      }

      if(NULL == pPrevElement)
      {
        m_pHead = pNextElement;
      }
      else
      {
        pPrevElement->m_pNext = pNextElement;
      }

      --m_NumOfElements;
      GNSS_DELETE(CGPS_COMMON_MODULE_LIST, ListElement<T>, pThisElement);      

      nextPos.m_Content.m_pPrev = pNextElement->m_pPrev;
      nextPos.m_Content.m_pNext = pNextElement->m_pNext;
      return iterator(nextPos);
    }

    iterator swap(iterator & pos1, iterator & pos2)
    {
      ListElement<T> * pPos1Next = NULL;
      ListElement<T> * pPos1Prev = NULL;
      ListElement<T> * pPos2Next = NULL;
      ListElement<T> * pPos2Prev = NULL;

      ListElement<T> * pPos1 = NULL;
      ListElement<T> * pPos2 = NULL;

      ListElement<T> * pPosTmp = NULL;

      if(NULL != pos1.m_Content.m_pNext)
      {
        pPos1Next = pos1.m_Content.m_pNext;
        pPos1 = pPos1Next->m_pPrev;
      } 

      if(NULL != pos1.m_Content.m_pPrev)
      {
        pPos1Prev = pos1.m_Content.m_pPrev;
      } 

      if(NULL != pos2.m_Content.m_pNext)
      {
        pPos2Next = pos2.m_Content.m_pNext;
        pPos2 = pPos2Next->m_pPrev;
      } 

      if(NULL != pos2.m_Content.m_pPrev)
      {
        pPos2Prev = pos2.m_Content.m_pPrev;
      } 

      if(m_pHead == pPos1)
      {
        m_pHead = pPos2;
      }
      else if(m_pHead == pPos2)
      {
        m_pHead = pPos1;
      }

      pPosTmp = pPos1Next->m_pPrev;
      pPos1Next->m_pPrev = pPos2Next->m_pPrev;
      pPos2Next->m_pPrev = pPosTmp;

      pPosTmp = pPos1Next->m_pPrev;
      pPos1Next->m_pNext = pPos2Next->m_pNext;
      pPos2Next->m_pNext = pPosTmp;

      pPos1Prev->m_pNext = pPos2;
      pPos2Prev->m_pNext = pPos1;

      return pos1;
    }

/*=============================================================================

  FUNCTION      insert()

  DESCRIPTION
    This function inserts an element to a position in a list. 

  DEPENDENCIES  

  PARAMETERS
    pos        - The position where element will be inserted
    (input)
 
    Data       - The data to be inserted at positition.
    (input)

  RETURN VALUE
    true  - If insertion is successful
    false - otherwise
     
  SIDE EFFECTS  

=============================================================================*/
    bool insert(iterator &pos, T & Data)
    {
      ListElement<T> * pElement = NULL;
      ListElement<T> * posElement = NULL;

      if(NULL != pos.m_Content.m_pNext)
      {
        posElement = pos.m_Content.m_pNext->m_pPrev;
      }
      else
      {
        posElement = &m_End;
      }

      GNSS_NEW_NOARGS(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement);
      if(NULL == pElement)
      {
        return false;
      }

      pElement->m_pData = &Data;

      pElement->m_pPrev = pos.m_Content.m_pPrev;

      if(NULL != posElement)
      {
        posElement->m_pPrev = pElement;
      }

      if(pElement->m_pPrev != NULL)
      {
        pElement->m_pNext = posElement;

        pElement->m_pPrev->m_pNext = pElement;
      }
      else
      {
        if(NULL != m_pHead)
        {
          pElement->m_pNext = m_pHead;
        }
        else
        {
          pElement->m_pNext = &m_End;
        }
        m_pHead = pElement;
      }

      ++m_NumOfElements;
      return true;
    }

    iterator begin()
    {
      return iterator(m_pHead);
    } 

    iterator end()
    {
      return iterator(&m_End);
    } 

    iterator rbegin()
    {
      return iterator(m_End.m_pPrev);
    } 

    ListElement<T>* getHead()
    {
      return m_pHead;
    }
    ListElement<T>* getEnd()
    {
      return &m_End;
    }
 
    /*=============================================================================

    FUNCTION      msort()

    DESCRIPTION
    Sort the current list and update mHead and mEnd using merge sort algorithm 

    DEPENDENCIES

    PARAMETERS
    pz_SortedFun        - object pointer derived from MergeSortCompFunctionBase and over rides
    (input)               sortComp() to implement comparison function of List elements.
      
    RETURN VALUE
    bool    - true if list was sorted else false.

    SIDE EFFECTS

    =============================================================================*/
    bool msort(MergeSortCompFunctionBase<T> *pz_SortedFun)
    {
      ListElement<T> *pTail = NULL;
      if (NULL == pz_SortedFun || false == pz_SortedFun->isSortCompFunImplemented()||
           getNumOfElements() < 2)
      {
        return false;
      }
      msortint(&m_pHead,&pTail, pz_SortedFun);
      m_End.m_pPrev = pTail;
      if (NULL != pTail)
      {
        pTail->m_pNext = &m_End;
      }
      return true;
    }

    uint32 getUserFlag()
    {
      return m_qUserFlag;
    }

    void setUserFlag(uint32 q_Val)
    {
      m_qUserFlag = q_Val;
    }

  private:
    ListElement<T> * m_pHead;
    ListElement<T>   m_End;
    size_t m_NumOfElements;
    uint32 m_qUserFlag;
    void splitlist(ListElement<T> *head, ListElement<T> **ppFirstHalfHead, ListElement<T> **ppSecondHalfHead,
                   ListElement<T> **ppFirstHalfTail, ListElement<T> **ppSecondHalfTail);
    void insertionsort(ListElement<T> **ppHead, ListElement<T> **ppTail, MergeSortCompFunctionBase<T> *pz_SortedFun);
    void msortint(ListElement<T> **ppHead, ListElement<T> **ppTail, MergeSortCompFunctionBase<T> *pz_SortedFun, uint32 q_MaxRecursiveLevel = C_GNSS_LIST_MAX_RECURSIVE_LEVEL);

    ListElement<T> * mergeSortedLists(ListElement<T> *pFirstHalfHead, ListElement<T> *pSecondHalfHead,
                                      ListElement<T> *pFirstHalfTail, ListElement<T> *pSecondHalfTail,
                                      ListElement<T> **ppTail, MergeSortCompFunctionBase<T> *pz_SortedFun);
  };

  template <typename T>
  inline
    void gnss_ListClass<T>::clear()
  {
    while(m_NumOfElements > 0)
    {
      pop_back();
    }
  }
  /********************************************************************
  *
  * gnss_ListClass<T>::clearall
  *
  * Function description:
  * This function is used to clear all elements in list.
  *
  * Parameters:
  * isDelListElementData : List is linked list of Elements 
  *               Each Element has Data pointer, pointers to next and previous.
  *               isDelListElementData will specify if Data pointer in Element needs to be freed or not
  *               true : data pointer will be freed.
  *               false : data pointer will not be freed

  *
  * Return:
  * None
  *
  *********************************************************************/
  template <typename T>
  inline
  void gnss_ListClass<T>::clearall(bool isDelListElementData )
  {
    clearallint(m_pHead, isDelListElementData);
    m_pHead = NULL;
    m_End.m_pPrev = NULL;
  }

  template <typename T>
  inline
  void gnss_ListClass<T>::clearallint(ListElement<T> * pElement, bool isDelListElementData)
  {
    if (NULL == pElement || pElement == getEnd())
    {
      return;
    }
    clearallint(pElement->m_pNext, isDelListElementData);
    if (isDelListElementData && NULL != pElement->m_pData)
    {
      GNSS_DELETE(CGPS_COMMON_MODULE_LIST, T, pElement->m_pData);
    }
    GNSS_DELETE(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement);
    --m_NumOfElements;
    return;
  }

  template <typename T>
  inline
    bool gnss_ListClass<T>::push_back(T & Data)
  {
    ListElement<T> * pElement = NULL;

    GNSS_NEW_NOARGS(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement);
    if(NULL == pElement)
    {
      return false;
    }

    pElement->m_pData = &Data;
    pElement->m_pNext = &m_End;
    pElement->m_pPrev = m_End.m_pPrev;
    if(pElement->m_pPrev)
    {
      pElement->m_pPrev->m_pNext = pElement;
    }
    m_End.m_pPrev = pElement;

    if(m_NumOfElements == 0)
    {
      m_pHead = pElement;
    }

    ++m_NumOfElements;
    return true;
  }

  template <typename T>
  inline
    T * gnss_ListClass<T>::back()
  {
    if( (m_NumOfElements > 0) &&
        (NULL != m_End.m_pPrev ) )
    {
      return m_End.m_pPrev->m_pData;
    }

    return NULL;
  }

  template <typename T>
  inline
    void gnss_ListClass<T>::pop_back()
  {
    if( m_End.m_pPrev &&
        (m_NumOfElements > 0) )
    {
      ListElement<T> * pElement = m_End.m_pPrev;

      --m_NumOfElements;
      if(m_NumOfElements > 0)
      {
        m_End.m_pPrev = m_End.m_pPrev->m_pPrev;
        m_End.m_pPrev->m_pNext = &m_End;
      }
      else
      {
        m_pHead = NULL;
        m_End.m_pPrev = NULL;
      }
      GNSS_DELETE(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement);      
    }
  }

  template <typename T>
  inline
    bool gnss_ListClass<T>::push_front(T & Data)
  {
    ListElement<T> * pElement = NULL;

    GNSS_NEW_NOARGS(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement);

    if(NULL == pElement)
    {
      return false;
    }

    pElement->m_pData = &Data;
    pElement->m_pNext = NULL;
    pElement->m_pPrev = NULL;

    if(m_pHead)
    {
      m_pHead->m_pPrev   = pElement;
      pElement->m_pNext = m_pHead;
    }

    m_pHead = pElement;
    if(m_NumOfElements == 0)
    {
      m_End.m_pPrev = m_pHead;
      m_pHead->m_pNext = &m_End;
    }

    ++m_NumOfElements;

    return true;
  }

  template <typename T>
  inline
    T * gnss_ListClass<T>::front()
  {
    if( (m_NumOfElements > 0) &&
        (NULL != m_pHead ) )
    {
      return m_pHead->m_pData;
    }

    return NULL;
  }

  template <typename T>
  inline
    void gnss_ListClass<T>::pop_front()
  {
    if( m_pHead &&
      (m_NumOfElements > 0) )
    {
      ListElement<T> * pElement = m_pHead;

      --m_NumOfElements;
      if(m_NumOfElements > 0)
      {
        m_pHead = m_pHead->m_pNext;
        m_pHead->m_pPrev = NULL; 
      }
      else
      {
        m_pHead = NULL;
        m_End.m_pPrev = NULL;
      }
      GNSS_DELETE(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement); 
    }
  }


template <typename T>
  inline
    T* gnss_ListClass<T>::pop_front_ext()
  {
    T* pRetData = NULL;
    if( m_pHead &&
      (m_NumOfElements > 0) )
    {
      ListElement<T> * pElement = m_pHead;

      --m_NumOfElements;
      if(m_NumOfElements > 0)
      {
        m_pHead = m_pHead->m_pNext;
        m_pHead->m_pPrev = NULL; 
      }
      else
      {
        m_pHead = NULL;
        m_End.m_pPrev = NULL;
      }
      pRetData = pElement->m_pData;
      GNSS_DELETE(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement); 
    }
    return pRetData;
  }
   
template <typename T>
  inline
    ListElement<T>* gnss_ListClass<T>::push_back_ext(T & Data)
  {
    ListElement<T> * pElement = NULL;

    GNSS_NEW_NOARGS(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement);
    if(NULL == pElement)
    {
      return NULL;
    }

    pElement->m_pData = &Data;
    pElement->m_pNext = &m_End;
    pElement->m_pPrev = m_End.m_pPrev;
    if(pElement->m_pPrev)
    {
      pElement->m_pPrev->m_pNext = pElement;
    }
    m_End.m_pPrev = pElement;

    if(m_NumOfElements == 0)
    {
      m_pHead = pElement;
    }

    ++m_NumOfElements;
    return pElement;
  }

  template <typename T>
  inline
    ListElement<T>* gnss_ListClass<T>::push_front_ext(T & Data)
  {
    ListElement<T> * pElement = NULL;

    GNSS_NEW_NOARGS(CGPS_COMMON_MODULE_LIST, ListElement<T>, pElement);

    if (NULL == pElement)
    {
      return pElement;
    }

    pElement->m_pData = &Data;
    pElement->m_pNext = NULL;
    pElement->m_pPrev = NULL;

    if (m_pHead)
    {
      m_pHead->m_pPrev = pElement;
      pElement->m_pNext = m_pHead;
    }

    m_pHead = pElement;
    if (m_NumOfElements == 0)
    {
      m_End.m_pPrev = m_pHead;
      m_pHead->m_pNext = &m_End;
    }

    ++m_NumOfElements;

    return pElement;
  }

template <typename T>
  inline
    bool gnss_ListClass<T>::delete_node(ListElement<T>* pDelNode)
  {
    ListElement<T> * pPrevElement = NULL;

    if((NULL == m_pHead)||(NULL == pDelNode))
    {
      /* Nothing to delete */
      return false;
    }
    else
    {
      /* Check if the node to be deleted is from the front of the list */
      if(m_pHead == pDelNode)
      {
        /* Delete at front of the list */
        pop_front();
        return true;
      }
      else if(pDelNode ==  m_End.m_pPrev)
      {
        /* Delete at end of list */
        pop_back();
        return true;
      }
      else
      {

        /* Set up the links for deletion in middle of the list */
        pPrevElement = pDelNode->m_pPrev;
        pPrevElement->m_pNext = pDelNode->m_pNext;            
        pDelNode->m_pNext->m_pPrev = pPrevElement;            
        /* Delete the node */
        --m_NumOfElements;
        GNSS_DELETE(CGPS_COMMON_MODULE_LIST, ListElement<T>, pDelNode);
        return true; 
      }// end else 
    }// end else if(NULL == m_pHead)
    return false;
  }


  template <typename T>
  inline
    ListElement<T>* gnss_ListClass<T>::search_list_for_data(T* pData)
  {
    ListElement<T>* pElement = NULL;
    
    if(NULL == pData)
    {
      return NULL;
    }
    /* search for the data and delete the node */
    pElement = m_pHead;
    while(pElement != &m_End)
    {
      /* Compare the data stored in the list if equal */
      if(0 == memcmp(pData, pElement->m_pData, sizeof(T)))
      {
        /* Element found in the list */
        return pElement;
      }
      pElement = pElement->m_pNext;
    }
    /* Item not found in list */
    return NULL;
  }


  template <typename T>
  inline
    bool gnss_ListClass<T>::delete_node_with_data(T* pDelData)
  {
    ListElement<T> *pDelNode = NULL;
    bool u_Ret = false;

    if(NULL == pDelData)
    {
      return false;
    }

    pDelNode = search_list_for_data(pDelData);
    
    /* Data to be deleted is not found */
    if(NULL == pDelNode)
    {
      return false;
    }
    
    /* Node with the deletion data is found . Go ahead and delete it */
    u_Ret = delete_node(pDelNode);
    return u_Ret;
    
  }
  /*=============================================================================

  FUNCTION      splitlist()

  DESCRIPTION
  Split the list in to two half's and return first and second half's head and tail

  DEPENDENCIES

  PARAMETERS
  head        - head of the list
  (input)

  ppFirstHalfHead       - First half list head.
  (output)

  ppSecondHalfHead       - second half list head.
  (output)

  ppFirstHalfTail       - first half list tail.
  (output)

  ppSecondHalfTail       - second hlf list tail.
  (output)

  RETURN VALUE
 

  SIDE EFFECTS

  =============================================================================*/
  template <typename T>
  void gnss_ListClass<T>::splitlist(ListElement<T> *head, ListElement<T> **ppFirstHalfHead, ListElement<T> **ppSecondHalfHead,
    ListElement<T> **ppFirstHalfTail, ListElement<T> **ppSecondHalfTail)
  {
    ListElement<T> *pz_SingleJump = head, *pz_DoubleJump = head;
    /*handle 0 or 1 node cases*/
    if (head == NULL || head->m_pNext == NULL)
    {
      *ppFirstHalfHead = head;
      *ppSecondHalfHead = NULL;
      *ppFirstHalfTail = head;
      return;
    }
    while (NULL != pz_DoubleJump && NULL != pz_DoubleJump->m_pNext)
    {
      pz_SingleJump = pz_SingleJump->m_pNext;
      if (NULL == pz_DoubleJump->m_pNext->m_pNext)
      {
        pz_DoubleJump = pz_DoubleJump->m_pNext;
        break;
      }
      pz_DoubleJump = pz_DoubleJump->m_pNext->m_pNext;

    }
    if (NULL != pz_SingleJump->m_pPrev)
    {
      pz_SingleJump->m_pPrev->m_pNext = NULL;
    }
    *ppFirstHalfTail = pz_SingleJump->m_pPrev;
    if (NULL != pz_DoubleJump && NULL != pz_DoubleJump->m_pData)
    {
      *ppSecondHalfTail = pz_DoubleJump;
    }
    else
    {
      *ppSecondHalfTail = pz_DoubleJump->m_pPrev;
    }
    (*ppSecondHalfTail)->m_pNext = NULL;
    pz_SingleJump->m_pPrev = NULL;
    *ppFirstHalfHead = head;
    *ppSecondHalfHead = pz_SingleJump;
  }

  template <typename T>
  void gnss_ListClass<T>::insertionsort(ListElement<T> **ppHead, ListElement<T> **ppTail, MergeSortCompFunctionBase<T> *pz_SortedFun)
  {
    ListElement<T> *pCur = NULL, *pNext = NULL, *pWalker = NULL, *pTail = NULL;

    if (NULL == ppHead || NULL == *ppHead || NULL == ppTail)
    {
      return;
    }

    pCur = (*ppHead)->m_pNext;

    while (NULL != pCur && NULL != pCur->m_pData)
    {
      pNext = pCur->m_pNext;
      pWalker = pCur->m_pPrev;

      // to find the right spot
      while (NULL != pWalker && pz_SortedFun->sortComp(*pCur->GetData(), *pWalker->GetData()))
      {
        pWalker = pWalker->m_pPrev;
      }
      
      // remove pCur from the list
      pCur->m_pPrev->m_pNext = pCur->m_pNext;
      if (NULL != pCur->m_pNext)
      {
        pCur->m_pNext->m_pPrev = pCur->m_pPrev;
      }

      if (NULL == pWalker)
      {
        // insert to the head
        pCur->m_pNext = *ppHead;
        if (pCur->m_pNext)
        {
          pCur->m_pNext->m_pPrev = pCur;
        }
        pCur->m_pPrev = pWalker;
        *ppHead = pCur;
      }
      else
      {
        pCur->m_pNext = pWalker->m_pNext;
        if (pCur->m_pNext)
        {
          pCur->m_pNext->m_pPrev = pCur;
        }
        pCur->m_pPrev = pWalker;
        pWalker->m_pNext = pCur;
      }
      *ppTail = pCur; // shift the tail pointer toward the end
      pCur = pNext;
    }

    pTail = *ppHead;
    while (pTail && pTail->m_pNext)
    {
      pTail = pTail->m_pNext;
    }
    *ppTail = pTail;
  }

  template <typename T>
  void gnss_ListClass<T>::msortint(ListElement<T> **ppHead, ListElement<T> **ppTail, MergeSortCompFunctionBase<T> *pz_SortedFun, uint32 q_MaxRecursiveLevel)
  {
    ListElement<T> *pFirstHalfHead = NULL, *pSecondHalfHead = NULL, *pFirstHalfTail = NULL, *pSecondHalfTail = NULL;
    /*handle case of length 0 or 1*/
    if (NULL == *ppHead)
    {
      *ppTail = NULL;
      return;
    }
    if (NULL == (*ppHead)->m_pNext)
    {
      *ppTail = *ppHead;
      return;
    }
    
    /*Split list to 2 */
    splitlist(*ppHead, &pFirstHalfHead, &pSecondHalfHead, &pFirstHalfTail, &pSecondHalfTail);

    /*sort both lists*/
    if (q_MaxRecursiveLevel <= 0)
    {
      /* fallback to non-resursive sorting */
      insertionsort(&pFirstHalfHead, &pFirstHalfTail, pz_SortedFun);
      insertionsort(&pSecondHalfHead, &pSecondHalfTail, pz_SortedFun);
    }
    else
    {
      msortint(&pFirstHalfHead, &pFirstHalfTail, pz_SortedFun, q_MaxRecursiveLevel - 1);
      msortint(&pSecondHalfHead, &pSecondHalfTail, pz_SortedFun, q_MaxRecursiveLevel - 1);
    }

    /*After sorting 2 half , merge both*/
    *ppHead = mergeSortedLists(pFirstHalfHead, pSecondHalfHead, pFirstHalfTail, pSecondHalfTail, ppTail, pz_SortedFun);

    return;
  }


  template <typename T>
  ListElement<T> * gnss_ListClass<T>::mergeSortedLists(ListElement<T> *pFirstHalfHead, ListElement<T> *pSecondHalfHead,
    ListElement<T> *pFirstHalfTail, ListElement<T> *pSecondHalfTail,
    ListElement<T> **ppTail, MergeSortCompFunctionBase<T> *pz_SortedFun)
  {
    ListElement<T> *pCurr1, *pNext1, *pCurr2;

    if (NULL == pFirstHalfHead || NULL == pFirstHalfHead->m_pData)
    {
      *ppTail = pSecondHalfTail;
      return pSecondHalfHead;
    }
    else if (NULL == pSecondHalfHead || NULL == pSecondHalfHead->m_pData)
    {
      *ppTail = pFirstHalfTail;
      return pFirstHalfHead;
    }

    if (!pFirstHalfHead)
    {
      *ppTail = pSecondHalfTail;
      return pSecondHalfHead;
    }
    if (!pSecondHalfHead)
    {
      *ppTail = pFirstHalfTail;
      return pFirstHalfHead;
    }
    
    if (pz_SortedFun->sortComp(*pSecondHalfHead->GetData(), *pFirstHalfHead->GetData()))
    {
      // swap first and second list so that the first list is always smaller
      ListElement<T> *pTempHead = pFirstHalfHead, *pTempTail = pFirstHalfTail;
      pFirstHalfHead = pSecondHalfHead;
      pFirstHalfTail = pSecondHalfTail;
      pSecondHalfHead = pTempHead;
      pSecondHalfTail = pTempTail;
    }

    if (pFirstHalfHead == pFirstHalfTail)
    {
      // only one element in the first list, simply append the second list to the first list
      pFirstHalfHead->m_pNext = pSecondHalfHead;
      pSecondHalfHead->m_pPrev = pFirstHalfHead;
      *ppTail = pSecondHalfTail;
      return pFirstHalfHead;
    }

    pCurr1 = pFirstHalfHead;
    pNext1 = pFirstHalfHead->m_pNext;
    pCurr2 = pSecondHalfHead;

    while (NULL != pCurr1 && NULL != pCurr2)
    {
      if (pz_SortedFun->sortComp(*pCurr1->GetData(), *pCurr2->GetData()) &&
        pz_SortedFun->sortComp(*pCurr2->GetData(), *pNext1->GetData()))
      {
        // insert pCurr2 between pCurr1 and pNext1
        ListElement<T> *pTemp = pCurr2->m_pNext;
        pCurr1->m_pNext = pCurr2;
        pCurr2->m_pPrev = pCurr1;
        pCurr2->m_pNext = pNext1;
        pNext1->m_pPrev = pCurr2;
        pCurr1 = pCurr2;
        pCurr2 = pTemp;
      }
      else
      {
        if (NULL == pNext1->m_pNext)
        {
          pNext1->m_pNext = pCurr2;
          pCurr2->m_pPrev = pNext1;
          *ppTail = pSecondHalfTail;
          return pFirstHalfHead;
        }
        else
        {
          pNext1 = pNext1->m_pNext;
          pCurr1 = pCurr1->m_pNext;
        }
      }
    }

    *ppTail = pFirstHalfTail;
    return pFirstHalfHead;
  }
}// end namespace 
#endif /* GNSS_LIST_H */
