#ifndef GNSS_VECTOR_H
#define GNSS_VECTOR_H

/*==============================================================================

                  GNSS Vector Header

GENERAL DESCRIPTION
   This file contains class definition for GNSS vectors

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_vector.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
9/14/2016  ahmadh Initial version


  =============================================================================*/
#include "gnss_hal.h"

namespace gnss_StlNameSpace
{
class gnss_MemoryMgrClass
{
public:
  gnss_MemoryMgrClass(void)
  {
  }

  ~gnss_MemoryMgrClass(void)
  {
  }

  size_t GetSize() const
  {
    return sizeof(*this);
  }
  
  virtual void * MemAlloc(cgps_module_type e_Module, size_t Size) = 0;
  virtual void   MemFree(cgps_module_type e_Module, void **  ptr_ptr) = 0;

protected:
};

template <typename T>
class GnssAllocClass
{
public:
  typedef T              value_type;
  typedef T*             pointer;
  typedef const T*       const_pointer;
  typedef T&             reference;
  typedef const T&       const_reference;

  GnssAllocClass():
  m_pzMemMgr(NULL)
  {
  }

  GnssAllocClass(gnss_MemoryMgrClass &z_MemMgr)
  {
    m_pzMemMgr = &z_MemMgr;
  }

  template<typename U>
    GnssAllocClass(const GnssAllocClass<U> & rhs) 
  {
    m_pzMemMgr = rhs.m_pzMemMgr;
  }

  pointer allocate(size_t Size, 
                   cgps_module_type e_Module=CGPS_COMMON_MODULE_UNKNOWN)
  {
    if(NULL != m_pzMemMgr)
    {
      return static_cast<pointer>(m_pzMemMgr->MemAlloc(e_Module, Size));
    }
    else
    {
      return static_cast<pointer>(gnss_HalClass::MemAlloc(e_Module, Size));
    }
  }

  void deallocate(pointer pz, size_t Size, 
                  cgps_module_type e_Module=CGPS_COMMON_MODULE_UNKNOWN)
  {
    if(NULL != m_pzMemMgr)
    {
      m_pzMemMgr->MemFree(e_Module, (void **) &pz);
    }
    else
    {
      gnss_HalClass::MemFree(e_Module, (void **) &pz, Size);
    }
  }

  pointer address (reference value) const 
  {
    return &value;
  }

  const_pointer address (const_reference value) const 
  {
    return &value;
  }

  template <typename U>
    struct rebind 
  {
    typedef GnssAllocClass<U> other;
  };

    gnss_MemoryMgrClass *m_pzMemMgr;
};

 
template<typename T1, typename T2>
bool operator==(const GnssAllocClass<T1>&,
                const GnssAllocClass<T2>&) throw()
{
  return true;
}

template<typename T1, typename T2>
bool operator != (const GnssAllocClass<T1>&,
                  const GnssAllocClass<T2>&) throw()
{
  return false;
}

template<typename T, typename Allocator = GnssAllocClass<T> >
class gnss_VectorClass 
{
public:

  typedef T *Iterator;

  gnss_VectorClass(cgps_module_type e_Module= CGPS_COMMON_MODULE_VECTOR) :
    m_pArray(NULL), m_Capacity(0), m_NumOfElements(0),
    m_pzAllocator(NULL),
    m_eModule(e_Module)
  {
    Init();
  }

  gnss_VectorClass(size_t Capacity,
                   cgps_module_type e_Module= CGPS_COMMON_MODULE_VECTOR) :
    m_pArray(NULL), m_Capacity(0), m_NumOfElements(0),
    m_pzAllocator(NULL),
    m_eModule(e_Module)
  {
    Init();
    reserve(Capacity);
  }
       
  size_t GetSize() const
  {
    return sizeof(*this);
  }

  void SetAllocator(Allocator &z_Allocator)
  {
    m_pzAllocator = &z_Allocator;
  }

  virtual ~gnss_VectorClass()
  {
    clear();
  }

  void Init(void)
  {
  }

  bool empty() const
  {
    if(m_NumOfElements == 0)
    {
      return true;
    }
    return false;
  }

/*=============================================================================

  FUNCTION      GetContentSize()

  DESCRIPTION
    The method returns the overall data size contained in a vector.

  DEPENDENCIES  

  PARAMETERS

 
  RETURN VALUE
    Data content size in the vector
     
  SIDE EFFECTS  

=============================================================================*/
  size_t GetContentSize() const
  {
    return m_Capacity * sizeof(*m_pArray);
  }

/*=============================================================================

  FUNCTION      reserve()

  DESCRIPTION
    The method sets the capacity of the vector. If the vector contains data
    a reallocation will be done. If the existing number of elements in the
    vector is larger than the desired capacity the function will increase
    the expected capacity multiple times to be able to keep the existing data

  DEPENDENCIES  

  PARAMETERS
    Capacity - The expected capacity of the vector.
    (input)
 
    b_Fill   - If true (default) , the vector will be considered as full. This way
    (input)    the data can be accessed by [] operator immediately.
               Note: any push_back() will cause a resize of the vector 
 
 
  RETURN VALUE
    true  - If successful
    false - Otherwise
     
  SIDE EFFECTS  

=============================================================================*/
  bool reserve(size_t Capacity, bool b_Fill=true)
  {
    bool b_Result = false;

    if(Capacity > max_size())
    {
      return false;
    }

    if ( Capacity <= m_Capacity )
    {
      b_Result = true;
    }
    else if ( Capacity > 0 )
    {
      size_t Size = Capacity * sizeof(*m_pArray);
      if (m_pArray == NULL)
      {
        if(NULL != m_pzAllocator)
        {
          m_pArray = static_cast<T *>(m_pzAllocator->allocate(Size, m_eModule));
        }
        else
        {
          m_pArray = static_cast<T *>(gnss_HalClass::MemAlloc(m_eModule, Size));
        }

        if (NULL != m_pArray)
        {
          memset(m_pArray, 0x00, Size);
          m_Capacity = Capacity;
          m_NumOfElements = 0;
          construct();
          b_Result = true;
        }
      }
      else 
      {
        T *pz_NewArray = NULL;
        size_t NewSize = Size;
        size_t NewCapacity = Capacity;

        if (m_NumOfElements > Capacity)
        {
          /* The desired capacity is not large enough to hold existing
             items*/
          while (NewCapacity <= m_Capacity)
          {
            NewCapacity += Capacity;
          }
          NewSize = NewCapacity * sizeof(*m_pArray);
        }

        if(NULL != m_pzAllocator)
        {
          pz_NewArray = static_cast<T *>(m_pzAllocator->allocate(NewSize, m_eModule));
        }
        else
        {
          pz_NewArray = static_cast<T *>(gnss_HalClass::MemAlloc(m_eModule, NewSize));
        }

        if (NULL != pz_NewArray)
        {
          /* Need to relocate the existing elements to the new array */
          size_t old_Capacity      = m_Capacity;
          size_t old_Size          = old_Capacity * sizeof(*m_pArray);
          size_t old_NumOfElements = m_NumOfElements;
          T *pz_oldArray           = m_pArray;

          memscpy(pz_NewArray, NewSize,
                  m_pArray, old_Size);

          clear();
          m_Capacity      = NewCapacity;
          m_pArray        = pz_NewArray;
          m_NumOfElements = old_NumOfElements;

          b_Result = true;
        }
      }
    }

    if (!b_Result)
    {
      HAL_MSG_ERR_1("Failed to set the vector Capacity to:%lu", Capacity);
    }
    else if(b_Fill)
    {
      m_NumOfElements = m_Capacity;
    }

    return b_Result;
  }

/*=============================================================================

  FUNCTION      clear()

  DESCRIPTION
    The method removes all the elements in the current vector. The function
    does not deallocate the memory used by the element. The function deallocates
    the memory used by the pointer in the vector.

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
     
  SIDE EFFECTS  

=============================================================================*/
  void clear()
  {
    if (NULL != m_pArray)
    {
      T * p_ThisElement = m_pArray; 
      for(size_t i=0;i<m_NumOfElements;++i)
      {
        p_ThisElement->~T();
        ++p_ThisElement;
      }

      if(m_pzAllocator != NULL)
      {
        m_pzAllocator->deallocate(m_pArray, sizeof(*m_pArray) * m_Capacity, 
                                  m_eModule);
      }
      else
      {
        gnss_HalClass::MemFree(m_eModule,
                               (void **) &m_pArray,
                               sizeof(*m_pArray) * m_Capacity);
      }
      m_pArray = NULL;
    }
    m_Capacity = 0;
    m_NumOfElements = 0;
  }

/*=============================================================================

  FUNCTION      push_back()

  DESCRIPTION
    The method appends a copy of an element to the end of the vector.

  DEPENDENCIES  

  PARAMETERS
    cz_element - The element to be inserted
 
  RETURN VALUE
    true  - If element can be inseretd to the vector
    false - Otherwise
     
  SIDE EFFECTS
    If there is not enough capacity in the vector, the existing capacity
    will be doubled by memory allocation

=============================================================================*/
  bool push_back(const T & cz_element)
  {
    bool bResult = true;

    if (NULL == m_pArray)
    {
      if (m_Capacity == 0)
      {
        if (!reserve(c_DefaultVectorCapacity, false))
        {
          bResult = false;
        }
      }
      else
      {
        /* This should not happen */
        bResult = false;
      }
    }
    else
    {
      if (m_Capacity == m_NumOfElements)
      {
        /*  We need to increase capacity */
        size_t Capacity = 2 * m_Capacity;

        if (!reserve(Capacity, false))
        {
          bResult = false;
        }
      }
    }

    if (bResult)
    {
      // Store the new element
      m_pArray[m_NumOfElements] = cz_element;
      ++m_NumOfElements;
    }

    return bResult;
  }

/*=============================================================================

  FUNCTION      index operator

  DESCRIPTION
    The method implements the index operator to access vector elements.

  DEPENDENCIES  

  PARAMETERS
    index - The index to the vector
    (input)
 
  RETURN VALUE
    T  - The object at q_index in the vector
     
  SIDE EFFECTS
    None

=============================================================================*/
  T & operator [](const size_t index)
  {
    if(index < m_NumOfElements)
    {
      return m_pArray[index];
    }
    else
    {
      HAL_MSG_ERR_2("Index out of range, index:%lu, range:%lu", 
                    index, m_Capacity);
      return m_pArray[0];
    }
  }

/*=============================================================================

  FUNCTION      index operator for const object

  DESCRIPTION
    The method implements the index operator to access constant vector elements.

  DEPENDENCIES  

  PARAMETERS
    index - The index to the vector
    (input)
 
  RETURN VALUE
    T *  - Pointer to the object at q_index in the vector
     
  SIDE EFFECTS
    None

=============================================================================*/
  T & operator [](const size_t index) const
  {
    return ((gnss_VectorClass<T> *)this)->operator [](index);
  }

/*=============================================================================

  FUNCTION      size()

  DESCRIPTION
    The method returns the number of elements in the vector.

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
    uint32 number of elements in the vector
     
  SIDE EFFECTS

=============================================================================*/
    size_t size() const
    {
      return m_NumOfElements;
    }

/*=============================================================================

  FUNCTION      capacity()

  DESCRIPTION
    The method returns the current capacity of the vector without reallocation.

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
    uint32 
     
  SIDE EFFECTS

=============================================================================*/
    size_t capacity() const
    {
      return m_Capacity;
    }

/*=============================================================================

  FUNCTION      max_size()

  DESCRIPTION
    The method returns the maximum size of a vector.

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
    uint32 
     
  SIDE EFFECTS

=============================================================================*/
    size_t max_size()
    {
      return static_cast<size_t>(c_DefaultVectorMaxSizeBytes/sizeof(*m_pArray));
    }

/*=============================================================================

  FUNCTION      Assignment operator

  DESCRIPTION
    This method implements the assignment operation for vectors.

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
    Vector 
     
  SIDE EFFECTS

=============================================================================*/
    const gnss_VectorClass<T> & operator =(const gnss_VectorClass<T> & rhs)
    {
      size_t NewSize;
      if(&rhs == this)
      {
        return *this;
      }

      clear();
      NewSize = rhs.capacity();

      if(NewSize > 0)
      {
        size_t NumOfElements = rhs.size();

        if(m_pzAllocator != NULL)
        {
          m_pArray = static_cast<T *> (m_pzAllocator->allocate(NewSize * sizeof(*m_pArray)), rhs.m_eModule);
        }
        else
        {
          m_pArray = static_cast<T *> (gnss_HalClass::MemAlloc(rhs.m_eModule,
                                                               NewSize * sizeof(*m_pArray)));
        }

        if(NULL != m_pArray)
        {
          for (size_t q_i = 0; q_i < NumOfElements; ++q_i)
          {
            m_pArray[q_i] = rhs.m_pArray[q_i];
          }

          m_Capacity = NewSize;
          m_NumOfElements = NumOfElements;
          m_eModule = rhs.m_eModule;
        }
        else
        {
          HAL_MSG_ERR_1("Assignment failed, memory alloc fail size:%d [byte]", 
                        NewSize * sizeof(*m_pArray));
        }
      }

      return *this;
    }

/*=============================================================================

  FUNCTION      begin()

  DESCRIPTION
    This method returns a pointer to the first element of a vector.

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
    Vector 
     
  SIDE EFFECTS

=============================================================================*/
    Iterator begin()
    {
      return Iterator(m_pArray);
    }

/*=============================================================================

  FUNCTION      end()

  DESCRIPTION
    This method returns a pointer to the last element of a vector.

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE
    Vector 
     
  SIDE EFFECTS

=============================================================================*/
    Iterator end()
    {
      return Iterator(m_pArray + m_NumOfElements);
    }

  private:
    T * m_pArray;

    Allocator *m_pzAllocator;

    size_t           m_Capacity; /* The max number of elements that can be stored without
                                 reallocation. */
    size_t           m_NumOfElements; /* The number of elements in the vector */
    cgps_module_type m_eModule;


    static const size_t c_DefaultVectorCapacity     = 50;
    static const size_t c_DefaultVectorMaxSizeBytes = 1048576; /* 1 MByte */

    /*=============================================================================

    
    FUNCTION      construct()

    
    DESCRIPTION
    The method calls the default constructor of the elements.

    DEPENDENCIES  

    PARAMETERS
   
    RETURN VALUE
       
    SIDE EFFECTS  

  =============================================================================*/
    void construct()
    {
      if (NULL != m_pArray)
      {
        T * p_ThisElement = m_pArray; 
        for(size_t i=0;i<m_Capacity;++i)
        {
          GNSS_CONSTRUCT_ARGS(T, p_ThisElement);
          ++p_ThisElement;
        }
      }
    }
};

}

#endif /* GNSS_VECTOR_H */
