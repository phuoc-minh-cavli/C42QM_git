/**
  @file mre_set.c

  @brief  APIs to deal with policyman sets
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/mre_set.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "mre_set.h"
#include "mre_util.h"
#include "mre_dbg.h"
#include <stringl/stringl.h>


/*-------- mre_set_t --------*/
struct  mre_set_t
{
  MRE_NAMED_OBJECT_BASE;

  size_t            elemSize;     /* size of a set element */
  size_t            nElems;       /* current number of elements in the set */
  elem_compare_fn_t pfnCompare;   /* pointer to compare function */
  destructor_ptr    pfnDtor;      /* pointer to destructor for the elements */
  boolean           refCounted;   /* does this contain ref-counted objects? */
  dyn_byte_array_t  *pArray;      /* pointer to dynamic array that is the set */
};






/*-------- mre_set_elem_ptr --------*/
void*
mre_set_elem_ptr(
  mre_set_t *pSet,
  size_t    i
  )
{
  void  *pElem = dyn_byte_array_get_ptr_to_offset(pSet->pArray, i * pSet->elemSize);

  if (NULL == pElem)
  {
    MRE_ERR_FATAL( "Invalid index %d passed when max elements is %d",
                   i, dyn_byte_array_length(pSet->pArray)/pSet->elemSize, 0 );
  }
  
  return pElem;
}



/*-------- mre_set_dtor --------*/
static void
mre_set_dtor(
  void  *pObj
  )
{
  mre_set_t *pSet = (mre_set_t *) pObj;
  destructor_ptr  pfnDtor = pSet->pfnDtor;
  size_t          i;

  if (pfnDtor != NULL)
  {
    for (i = 0 ; i < pSet->nElems ; ++i)
    {
      pfnDtor(mre_set_elem_ptr(pSet, i));
    }
  }

  ref_cnt_obj_release(pSet->pArray);
  MRE_MEM_FREE_IF(pSet->pName);
  mre_mem_free(pSet);
}


/*-------- mre_set_new --------*/
mre_set_t *
mre_set_new(
  size_t            elemSize,
  size_t            nInitElems,
  size_t            maxElems,
  elem_compare_fn_t pfnCompare,
  destructor_ptr    pfnDtor
  )
{
  mre_set_t *pSet;
  size_t    nBufElems;
  size_t    maxSize;

  pSet = (mre_set_t *) mre_mem_alloc(sizeof(mre_set_t));
  ref_cnt_obj_init(pSet, mre_set_dtor);

  pSet->elemSize = elemSize;
  pSet->nElems = 0;
  pSet->pfnCompare = pfnCompare;
  pSet->pfnDtor = pfnDtor;
  pSet->refCounted = FALSE;

  nBufElems = MIN(nInitElems, maxElems);
  maxSize = (MRE_SET_NO_MAX_SIZE == maxElems)? DYN_ARRAY_NO_MAX_SIZE : maxElems * elemSize;
  pSet->pArray = dyn_byte_array_new(nBufElems * elemSize, maxSize, NULL);

  return pSet;
}



/*-------- release_element --------*/
static void
release_element(
  void  *pObj
  )
{
  void  *pRefCntObj = *((void **) pObj);
  ref_cnt_obj_release(pRefCntObj);
}


/*-------- mre_set_refcnt_new --------*/
mre_set_t *
mre_set_refcnt_new(
  size_t            nInitElems,
  size_t            maxElems,
  elem_compare_fn_t pfnCompare
  )
{
  mre_set_t *pSet;

  pSet = mre_set_new(sizeof(void *), nInitElems, maxElems, pfnCompare, release_element);
  pSet->refCounted = TRUE;

  return pSet;
}


/*-------- mre_set_find --------*/
void  *
mre_set_find(
  mre_set_t         *pSet,
  elem_compare_fn_t pfnCmp,
  void              *pData
  )
{
  byte  *pTest;
  byte  *pEnd;

  if( pSet->nElems == 0 )
  {
    goto Done;
  }

  pTest = (byte *) mre_set_elem_ptr(pSet, 0);
  pEnd = (byte *) mre_set_elem_ptr(pSet, pSet->nElems - 1);

  for ( ; pTest <= pEnd ; pTest += pSet->elemSize)
  {
    if (pfnCmp(pTest, pData))
    {
      return pTest;
    }
  }

Done:
  return NULL;
}


/*-------- mre_set_find_with_memcmp --------*/
/**
@brief  

@param

@return
  
*/
static void *
mre_set_find_with_memcmp(
  mre_set_t *pSet,
  void      *pElem
  )  
{
  byte  *pTest;
  byte  *pEnd;

  if( pSet->nElems == 0 )
  {
    goto Done;
  }

  pTest = (byte *) mre_set_elem_ptr(pSet, 0);
  pEnd = (byte *) mre_set_elem_ptr(pSet, pSet->nElems - 1);

  for ( ; pTest <= pEnd ; pTest += pSet->elemSize)
  {
    if (memcmp(pTest, pElem, pSet->elemSize) == 0)
    {
      return pTest;
    }
  }

Done:
  return NULL;
}


/*-------- mre_set_find_elem --------*/
/**
@brief  Find a specified element in the set.

@param[in]  pSet  Pointer to the policyman set.
@param[in]  pElem Pointer to an element to be compared to those already in the set.

@return
  Pointer to the first (presumably only) element in the set that matches the
  specified element.  NULL if no matching element was found.
*/
static void *
mre_set_find_elem(
  mre_set_t *pSet,
  void      *pElem
  )
{
  return  (pSet->pfnCompare != NULL)?
              mre_set_find(pSet, pSet->pfnCompare, pElem)
            : mre_set_find_with_memcmp(pSet, pElem)
            ;
}


/*-------- mre_set_offset_of --------*/
size_t
mre_set_offset_of(
  mre_set_t *pSet,
  byte      *pLocation
  )
{
  return pLocation - dyn_byte_array_get_array_ptr(pSet->pArray);
}


/*-------- mre_set_append --------*/
boolean
mre_set_append(
  mre_set_t *pSet,
  void      *pElem
  )
{
  boolean succeeded;

  succeeded = dyn_byte_array_append_bytes(pSet->pArray, pElem, pSet->elemSize);

  if (succeeded)
  {
    ++pSet->nElems;
    if (pSet->refCounted)
    {
      ref_cnt_obj_add_ref(*((void **) pElem));
    }
  }

  return succeeded;
}


/*-------- mre_set_add --------*/
boolean
mre_set_add(
  mre_set_t *pSet,
  void      *pElem
  )
{
  boolean succeeded = TRUE;

  /*  If the element is already present, we have nothing to do.
   */
  if (!mre_set_contains(pSet, pElem))
  {
    succeeded = mre_set_append(pSet, pElem);
  }

  return succeeded;
}


/*-------- mre_set_replace --------*/
boolean
mre_set_replace(
  mre_set_t *pSet,
  void      *pElem
  )
{
  byte    *pOld;
  size_t  offset;

  /*  See if the item is present in the set.
   */
  pOld = (byte *) mre_set_find_elem(pSet, pElem);

  /*  If not, just add it.
   */
  if (pOld == NULL)
  {
    return mre_set_add(pSet, pElem);
  }
  
  /*  If this set contains pointers to reference-counted objects,
   *  add a reference to the new element and release the old.
   */
  if (pSet->refCounted)
  {
    ref_cnt_obj_add_ref(*((void **) pElem));
    ref_cnt_obj_release(*((void **) pOld));
  }

  /*  Copy the element over the old one.
   */
  offset = mre_set_offset_of(pSet, pOld);
  dyn_byte_array_replace_bytes(pSet->pArray, pElem, pSet->elemSize, offset);

  return TRUE;
}



/*-------- mre_set_remove --------*/
void
mre_set_remove(
  mre_set_t *pSet,
  void      *pElem
  )
{
  byte    *pRemove;

  pRemove = (byte *) mre_set_find_elem(pSet, pElem);
  if (pRemove != NULL)
  {
    size_t  offset = mre_set_offset_of(pSet, pRemove);
    dyn_byte_array_delete_bytes(pSet->pArray, offset, pSet->elemSize);
    --pSet->nElems;
  }
}


/*-------- mre_set_contains --------*/
boolean
mre_set_contains(
  mre_set_t *pSet,
  void      *pElem
  )
{
  return mre_set_find_elem(pSet, pElem) != NULL;
}


/*-------- mre_set_num_elems --------*/
size_t
mre_set_num_elems(
  mre_set_t *pSet
  )
{
  return pSet->nElems;
}


/*-------- mre_set_is_empty --------*/
boolean
mre_set_is_empty(
  mre_set_t *pSet
  )
{
  return (pSet == NULL)? TRUE : mre_set_num_elems(pSet) == 0;
}


/*-------- mre_set_get_buf_ptr --------*/
void  *
mre_set_get_buf_ptr(
  mre_set_t *pSet
  )
{
  return dyn_byte_array_get_array_ptr(pSet->pArray);
}


/*-------- mre_set_iterate --------*/
boolean
mre_set_iterate(
  mre_set_t       *pSet,
  elem_iter_fn_t  pfnIter,
  void            *pData1,
  void            *pData2
  )
{
  size_t  i;
  boolean keepGoing;

  if (pSet == NULL)   { return FALSE; }

  for (i = 0, keepGoing = TRUE ; keepGoing && i < pSet->nElems ; ++i)
  {
    void  *pElem = mre_set_elem_ptr(pSet, i);
    keepGoing = pfnIter(pElem, pData1, pData2);
  }

  return keepGoing;
}


/*-------- mre_set_find_named_object --------*/
mre_named_object_t *
mre_set_find_named_object(
  mre_set_t               *pSet,
  char const              *pName,
  mre_named_object_type_t objType
  )
{
  size_t      i;

  for (i = 0 ; i < pSet->nElems ; ++i)
  {
    mre_named_object_t **ppNamedObj;
    mre_named_object_t *pNamedObj;

    ppNamedObj = (mre_named_object_t **) mre_set_elem_ptr(pSet, i);
    pNamedObj = (ppNamedObj != NULL)? *ppNamedObj : NULL;

    if (
          pNamedObj != NULL
       && pNamedObj->objType == objType
       && pNamedObj->pName != NULL
       && strcmp(pNamedObj->pName, pName) == 0
       )
    {
      return pNamedObj;
    }
  }

  return NULL;
}
