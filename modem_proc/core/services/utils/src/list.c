/* =========================================================================

DESCRIPTION
   Linked list and ordered link list routines and definitions

Copyright (c) 2000-2011, 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/list.c_v   1.0   24 May 2001 09:34:22   karthick  $
$Header: //components/rel/core.mpss/3.10/services/utils/src/list.c#2 $ $DateTime: 2018/08/24 22:11:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/21/18    kdey   Fixed LLVM errors
08/13/11    sri    Added functions list_destroy and ordered_list_destroy
08/02/11    ab     Removed INTLOCKS and used mutexs in place of them
04/15/09    sri    Call INTFREE_SAVE() in ordered_list_push() and
                   ordered_list_merge()
02/25/09    pc     Call LIST_FREE() in list_peek_prev()
09/01/00    gr     Added merge and split functions for lists and ordered
                   lists.
08/23/00    gr     Improved implementation of ordered_list_push.
08/22/00    gr     Implemented ordered list API.
08/09/00   jct     Created.
============================================================================ */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "list.h"

#ifdef FEATURE_WINCE
   #error code not present
#endif

#if defined(FEATURE_QURT)
  #include <qurt.h>
#elif defined(FEATURE_WINCE)
  #error code not present
#else 
  #include "rex.h"
#endif

/*===========================================================================
MACRO LIST_ASSERT

===========================================================================*/
/**
  @dependencies
  None.

  @sideeffects
  If the condition is FALSE, an ASSERT is raised.
*/
#ifdef LIST_DEBUG
   #ifdef T_WINNT
      #error code not present
#else
      extern void exit(int);
      #define LIST_ASSERT( xx_condition, mutex ) if (!(xx_condition)) exit(xx_condition)
   #endif
#else
   #define LIST_ASSERT( xx_condition, mutex ) \
   if( !(xx_condition) ) {\
      if(mutex){ LIST_MUTEX_FREE(mutex);}\
     return;}
#endif

#ifdef LIST_DEBUG
   #ifdef T_WINNT
      #error code not present
#else
      extern void exit(int);
      #define LIST_ASSERT_2( xx_condition, mutex1, mutex2 ) if (!(xx_condition)) exit(xx_condition)
   #endif
#else
   #define LIST_ASSERT_2( xx_condition, mutex1, mutex2) \
   if( !(xx_condition) ){\
     if(mutex1) {LIST_MUTEX_FREE(mutex1);}\
     if(mutex2) {LIST_MUTEX_FREE(mutex2);}\
     return;}
#endif


#ifdef LIST_DEBUG
   #ifdef T_WINNT
      #error code not present
#else
      extern void exit(int);
      #define LIST_ASSERT_RETURN( xx_condition, mutex ) if (!(xx_condition)) exit(xx_condition)
   #endif
#else
   #define LIST_ASSERT_RETURN( xx_condition, mutex ) \
   if( !(xx_condition) ){\
      if(mutex) {LIST_MUTEX_FREE(mutex);}\
      return 0;}
#endif

/* ------------------------------------------------------------------------
** 
** Unordered Lists
**
** ------------------------------------------------------------------------ */

/* ==================================================================
FUNCTION LIST_INIT
DESCRIPTION
   Initializes a list.
===================================================================== */
void list_init(
   list_type *list_ptr
)
{
   LIST_INTERCEPT_LIST_INIT( list_ptr );
   LIST_ASSERT( list_ptr != NULL, NULL );

// Mutex Initialisation
#ifdef FEATURE_QDSP6
   qurt_pimutex_init ( &(list_ptr->pkcsListCriticalSection));
#else /* FEATURE_QDSP6 */
   #ifdef FEATURE_WINCE
      #error code not present
#else /* FEATURE_WINCE */
      rex_init_crit_sect(&(list_ptr->pkcsListCriticalSection));
   #endif /* FEATURE_WINCE */
#endif /* FEATURE_QDSP6 */
      
   list_ptr->back_ptr  = NULL;
   list_ptr->front_ptr = NULL;
   list_ptr->size      = 0;
   list_ptr->list_initialized = 1;
    return;
} /* END list_init */

/* ==================================================================
FUNCTION LIST_IS_INITIALIZED
DESCRIPTION
   Check if the list is initialized.
===================================================================== */
unsigned int
list_is_initialized(
   list_type *list_ptr
)
{
   LIST_ASSERT_RETURN( list_ptr != NULL, NULL );
   return  list_ptr->list_initialized;
}
/* ==================================================================
FUNCTION LIST_DESTROY
DESCRIPTION
   Destroys the list, if the list is empty.
===================================================================== */
void list_destroy(
   list_type *list_ptr
)
{
   LIST_INTERCEPT_LIST_DESTROY( list_ptr );
   LIST_ASSERT( list_ptr != NULL, NULL );

   /* Assert is list is not free and you get here */
   LIST_ASSERT( list_ptr->size == 0, NULL);

/* Mutex Initialisation */
#ifdef FEATURE_QDSP6
   qurt_pimutex_destroy(&(list_ptr->pkcsListCriticalSection));
#else /* FEATURE_QDSP6 */
   #ifdef FEATURE_WINCE
      #error code not present
#else /* FEATURE_WINCE */
      rex_del_crit_sect( &(list_ptr->pkcsListCriticalSection) );
   #endif /* FEATURE_WINCE */
#endif /* FEATURE_QDSP6 */
   list_ptr->back_ptr         = NULL;
   list_ptr->front_ptr        = NULL;
   list_ptr->size             = 0;
   list_ptr->list_initialized = 0;
   return;
} /* END list_destroy */

/* ==================================================================
FUNCTION LIST_PUSH_FRONT
DESCRIPTION
   Inserts the given item_link_ptr at the front of the list pointed
   to by list_ptr. 
===================================================================== */
void list_push_front(
   list_type      *list_ptr,
   list_link_type *item_link_ptr
)
{
   LIST_INTERCEPT_LIST_PUSH_FRONT( list_ptr, item_link_ptr );
 
   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

    /* if LIST_LOCK returns false -- continue processing anyway */
   /* as we are using the blocking version and should NEVER    */
    /* get a false return value                                             */
   LIST_ASSERT( list_is_valid( list_ptr ),&(list_ptr->pkcsListCriticalSection) );
   LIST_ASSERT( item_link_ptr != NULL, &(list_ptr->pkcsListCriticalSection) );
   LIST_ASSERT( !item_is_in_list( list_ptr, item_link_ptr ) ,&(list_ptr->pkcsListCriticalSection));
   item_link_ptr->next_ptr = list_ptr->front_ptr;
   list_ptr->front_ptr     = item_link_ptr;
   if( list_ptr->back_ptr == NULL )
   {
    list_ptr->back_ptr = item_link_ptr;
   }
   list_ptr->size++;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

   return;

} /* END list_push_front */


/* ==================================================================
FUNCTION LIST_POP_FRONT
DESCRIPTION
   Removes the item at the front of the list.
===================================================================== */
void*
list_pop_front(
   list_type *list_ptr
)
{
   list_link_type *ret_ptr = NULL;
   LIST_INTERCEPT_LIST_POP_FRONT( list_ptr );
   
   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));
   
    LIST_ASSERT_RETURN( list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      if( list_ptr->size > 0 )
      {
         ret_ptr = list_ptr->front_ptr;
         list_ptr->front_ptr = ret_ptr->next_ptr;
         if( list_ptr->front_ptr == NULL )
         {
            list_ptr->back_ptr = NULL;
         }
         list_ptr->size--;      
      }
   
    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));
   
    return ret_ptr;

} /* END list_pop_front */


/* ==================================================================
FUNCTION LIST_PUSH_BACK
DESCRIPTION
   Inserts the given item_link_ptr at the back of the list pointed
   to by list_ptr
===================================================================== */
void list_push_back(
   list_type      *list_ptr,
   list_link_type *item_link_ptr
)
{
   LIST_INTERCEPT_LIST_PUSH_BACK( list_ptr, item_link_ptr );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
    LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT( list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( item_link_ptr != NULL, &(list_ptr->pkcsListCriticalSection) );
      LIST_ASSERT( !item_is_in_list( list_ptr, item_link_ptr ), &(list_ptr->pkcsListCriticalSection));
      item_link_ptr->next_ptr = NULL;
      if ( list_ptr->size == 0 )
      {
         list_ptr->front_ptr = item_link_ptr;
      }
      else
      {
         list_ptr->back_ptr->next_ptr = item_link_ptr;
      }
      list_ptr->back_ptr = item_link_ptr;

      list_ptr->size++;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return;

} /* END list_push_back */


/* ==================================================================
FUNCTION LIST_POP_BACK
DESCRIPTION
   Removes the item at the back of the list.
===================================================================== */
void*
list_pop_back(
   list_type *list_ptr
)
{
   list_link_type *ntl_ptr = NULL;
   list_link_type *ret_ptr = NULL;

   LIST_INTERCEPT_LIST_POP_BACK( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

   LIST_ASSERT_RETURN( list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      if( list_ptr->size > 0 )
      {
         ret_ptr = list_ptr->back_ptr;

         ntl_ptr = list_ptr->front_ptr;
         if ( ntl_ptr == ret_ptr )
         {
            list_ptr->back_ptr = list_ptr->front_ptr = NULL;
         }
         else
         {
            while ( ntl_ptr->next_ptr != ret_ptr )
            {
               ntl_ptr = ntl_ptr->next_ptr;
            }
            list_ptr->back_ptr = ntl_ptr;
            ntl_ptr->next_ptr  = NULL;
         }

         list_ptr->size--;
      }

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return ret_ptr;

} /* END list_pop_back */


/* ==================================================================
FUNCTION LIST_SIZE
DESCRIPTION
   Returns the number of elements in the list.
===================================================================== */
list_size_type
list_size(
   list_type *list_ptr
)
{
   list_size_type size;

   LIST_INTERCEPT_LIST_SIZE( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

   LIST_ASSERT_RETURN( list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      size = list_ptr->size;

   LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

   return size;

} /* END list_size */


/* ==================================================================
FUNCTION LIST_PEEK_FRONT
DESCRIPTION
   Returns a pointer to the element at the front of the list.
===================================================================== */
void*
list_peek_front(
   list_type *list_ptr
)
{
   void *item_ptr;

   LIST_INTERCEPT_LIST_PEEK_FRONT( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

   LIST_ASSERT_RETURN( list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      item_ptr = list_ptr->front_ptr;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return item_ptr;

} /* END list_peek_front */


/* ==================================================================
FUNCTION LIST_PEEK_BACK
DESCRIPTION
   Returns a pointer to the element at the back of the list.
===================================================================== */
void*
list_peek_back(
   list_type *list_ptr
)
{
   void *item_ptr;
   LIST_INTERCEPT_LIST_PEEK_BACK( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

    LIST_ASSERT_RETURN( list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      item_ptr = list_ptr->back_ptr;

   LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return item_ptr;

} /* END list_peek_back */


/* ==================================================================
FUNCTION LIST_PEEK_NEXT
DESCRIPTION
   Returns a pointer to the element that follows the element
   pointed to by item_after_which_to_peek
===================================================================== */
void*
list_peek_next(
   list_type      *list_ptr,
      /*lint -esym(715,list_ptr)
      ** Have lint not complain about the ignored parameter 'list_ptr'.
      */
   list_link_type *item_after_which_to_peek
)
{
   void *item_ptr;

   LIST_INTERCEPT_LIST_PEEK_NEXT( list_ptr, item_after_which_to_peek );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

    LIST_ASSERT_RETURN( list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
    LIST_ASSERT_RETURN( item_is_in_list( list_ptr, item_after_which_to_peek ) ,&(list_ptr->pkcsListCriticalSection));
    item_ptr = item_after_which_to_peek->next_ptr;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return item_ptr;
} /* END list_peek_next */


/* ==================================================================
FUNCTION LIST_PEEK_PREV
DESCRIPTION
   Returns a pointer to the element that precedes the element
   pointed to by item_before_which_to_peek
===================================================================== */
void*
list_peek_prev(
   list_type      *list_ptr,
      /*lint -esym(715,list_ptr)
      ** Have lint not complain about the ignored parameter 'list_ptr'.
      */
   list_link_type *item_before_which_to_peek
)
{
   void           *item_ptr;
   list_link_type *temp_ptr;
  uint8          return_status = 0;

  item_ptr = NULL;

   LIST_INTERCEPT_LIST_PEEK_PREV( list_ptr, item_before_which_to_peek );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

   LIST_ASSERT_RETURN( list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
   LIST_ASSERT_RETURN( item_is_in_list(list_ptr,  item_before_which_to_peek ), &(list_ptr->pkcsListCriticalSection));
   if ( item_before_which_to_peek == list_ptr->front_ptr )
   {
    return_status = 1;
   }

  if(return_status == 0)
  {
   temp_ptr = list_ptr->front_ptr;
   while ( temp_ptr != NULL &&
           temp_ptr->next_ptr != item_before_which_to_peek )
   {
     temp_ptr = temp_ptr->next_ptr;
   }
   item_ptr = temp_ptr;
  }

   LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return item_ptr;

} /* END list_peek_prev */


/* ==================================================================
FUNCTION LIST_PUSH_BEFORE
DESCRIPTION
   Pushes an item before another specified item.
===================================================================== */
void list_push_before(
   list_type      *list_ptr,
   list_link_type *item_to_push_ptr,
   list_link_type *item_to_push_before_ptr
)
{
   list_link_type *temp_ptr;

   LIST_INTERCEPT_LIST_PUSH_BEFORE(
         list_ptr,
         item_to_insert_ptr, 
         item_to_insert_before_ptr
   );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT( list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection) );
      LIST_ASSERT( item_to_push_ptr != NULL, &(list_ptr->pkcsListCriticalSection) );
      LIST_ASSERT( !item_is_in_list( list_ptr, item_to_push_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( item_is_in_list(list_ptr,  item_to_push_before_ptr ), &(list_ptr->pkcsListCriticalSection));
      item_to_push_ptr->next_ptr = item_to_push_before_ptr;
      temp_ptr                   = list_ptr->front_ptr;
      if ( temp_ptr == item_to_push_before_ptr )
      {
         list_ptr->front_ptr = item_to_push_ptr;
      }
      else
      {
         while ( temp_ptr->next_ptr != item_to_push_before_ptr )
         {
            temp_ptr = temp_ptr->next_ptr;
         }
         temp_ptr->next_ptr = item_to_push_ptr;
      }

      list_ptr->size++;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));
   return;
} /* END list_push_before */

/* ==================================================================
FUNCTION LIST_PUSH_AFTER
DESCRIPTION
   Pushes an item after another specified item.
===================================================================== */
void list_push_after(
   list_type      *list_ptr,
   list_link_type *item_to_push_ptr,
   list_link_type *item_to_push_after_ptr
)
{
   LIST_INTERCEPT_LIST_PUSH_AFTER(
      list_ptr,
      item_to_push_ptr, 
      item_to_push_after_ptr
   );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT( list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( item_to_push_ptr != NULL ,&(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( !item_is_in_list( list_ptr, item_to_push_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( item_is_in_list( list_ptr,  item_to_push_after_ptr ), &(list_ptr->pkcsListCriticalSection));
      item_to_push_ptr->next_ptr       = item_to_push_after_ptr->next_ptr;
      item_to_push_after_ptr->next_ptr = item_to_push_ptr;
      if ( list_ptr->back_ptr == item_to_push_after_ptr )
      {
         list_ptr->back_ptr = item_to_push_ptr;
      }

      list_ptr->size++;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

  return;

} /* END list_push_after */

/* ==================================================================
FUNCTION LIST_POP_ITEM
DESCRIPTION
   Pops a specified item off a list.
===================================================================== */
void list_pop_item(
   list_type      *list_ptr,
   list_link_type *item_to_pop_ptr
)
{
   list_link_type *temp_ptr;

   LIST_INTERCEPT_LIST_POP_ITEM( list_ptr, item_to_pop_ptr );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

        LIST_ASSERT( list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( item_is_in_list(list_ptr,  item_to_pop_ptr ), &(list_ptr->pkcsListCriticalSection));
      temp_ptr = list_ptr->front_ptr;
      if ( temp_ptr == item_to_pop_ptr )
      {
         list_ptr->front_ptr = item_to_pop_ptr->next_ptr;
         if ( list_ptr->back_ptr == item_to_pop_ptr )
         {
            list_ptr->back_ptr = NULL;
         }
      }
      else
      {
         while ( temp_ptr->next_ptr != item_to_pop_ptr )
         {
            temp_ptr = temp_ptr->next_ptr;
         }
         temp_ptr->next_ptr = item_to_pop_ptr->next_ptr;
         if ( list_ptr->back_ptr == item_to_pop_ptr )
         {
            list_ptr->back_ptr = temp_ptr;
         }
      }

      list_ptr->size--;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

  return;

} /* END list_pop_item */

/* ==================================================================
FUNCTION LIST_LINEAR_SEARCH
DESCRIPTION
   Searches a list for a matching item. The match is determined by
   means of a user-provided compare function. If there is no matching
   item in the list, NULL is returned. If there is more than one
   matching item, only the first match is returned.
===================================================================== */
void*
list_linear_search(
  list_type             *list_ptr,  
  list_compare_func_type compare_func,
  void                  *compare_val
)
{
   list_link_type *item_ptr = NULL;

   LIST_INTERCEPT_LIST_LINEAR_SEARCH( list_ptr, compare_func, compare_val );

   if(list_ptr->list_initialized != 1)
   {
      list_init(list_ptr);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT_RETURN( list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT_RETURN( compare_func != NULL, &(list_ptr->pkcsListCriticalSection));
      item_ptr = list_ptr->front_ptr;
      while( item_ptr != NULL )
      {
         if( compare_func( item_ptr, compare_val ) != 0 )
         {
            break;
         }
         item_ptr = item_ptr->next_ptr;
      }

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

  return item_ptr;

} /* END list_linear_search */


#if 0
/* ==================================================================
FUNCTION LIST_LINEAR_DELETE
DESCRIPTION
   Searches a list for a matching item and deletes the item. The match
   is determined by means of a user-provided compare function.
===================================================================== */
void
list_linear_delete(
  list_type             *list_ptr,  
  list_compare_func_type compare_func,
  void                  *compare_val
)
{
   return;
} /* END list_linear_delete */
#endif


/* ==================================================================
FUNCTION LIST_MERGE
DESCRIPTION
   Merges two lists by appending the second to the first. After the
   merge, the first list will contain all the elements in both lists.
   The second list will be empty.
===================================================================== */
void list_merge(
   list_type *list1_ptr,
   list_type *list2_ptr
)
{
   if(list1_ptr->list_initialized != 1)
   {
      list_init(list1_ptr);
   }
   if(list2_ptr->list_initialized != 1)
   {
      list_init(list2_ptr);
   }
   LIST_MUTEX_LOCK(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_LOCK(&(list2_ptr->pkcsListCriticalSection));

      LIST_ASSERT_2( list_is_valid( list1_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection));
      LIST_ASSERT_2( list_is_valid( list2_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection));
      if ( list1_ptr->size > 0 )
      {
         list1_ptr->back_ptr->next_ptr = list2_ptr->front_ptr;
      }
      else
      {
         list1_ptr->front_ptr = list2_ptr->front_ptr;
      }
      list1_ptr->back_ptr = list2_ptr->back_ptr;
      list1_ptr->size += list2_ptr->size;

      list2_ptr->front_ptr = list2_ptr->back_ptr = NULL;
      list2_ptr->size  = 0;

   LIST_MUTEX_FREE(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_FREE(&(list2_ptr->pkcsListCriticalSection));

   return;
} /* END list_merge */


/* ==================================================================
FUNCTION LIST_SPLIT
DESCRIPTION
   Splits a list at a specified item. Items before the specified item
   are retained in the original list. The specified item and items
   following it are placed into a new list.
===================================================================== */
void list_split(
   list_type      *list1_ptr,
   list_link_type *item_at_which_to_split_ptr,
   list_type      *list2_ptr
)
{
   list_link_type *temp_ptr = NULL;
   unsigned long   count    = 0;
   unsigned long   total_size;

   if(list1_ptr->list_initialized != 1)
   {
      list_init(list1_ptr);
   }
   if(list2_ptr->list_initialized != 1)
   {
      list_init(list2_ptr);
   }
   LIST_MUTEX_LOCK(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_LOCK(&(list2_ptr->pkcsListCriticalSection));

      LIST_ASSERT_2( list_is_valid( list1_ptr ) ,&(list1_ptr->pkcsListCriticalSection),&(list2_ptr->pkcsListCriticalSection));
      LIST_ASSERT_2( list_is_valid( list2_ptr ) ,&(list1_ptr->pkcsListCriticalSection),&(list2_ptr->pkcsListCriticalSection));
      LIST_ASSERT_2( item_is_in_list( list1_ptr, item_at_which_to_split_ptr ) ,&(list1_ptr->pkcsListCriticalSection),&(list2_ptr->pkcsListCriticalSection));

      total_size = list1_ptr->size;
      /* Figure out the number of elements in the first list after the
      ** split.
      */
      if ( list1_ptr->front_ptr != item_at_which_to_split_ptr )
      {
         count++;
         temp_ptr = list1_ptr->front_ptr;
         while ( temp_ptr->next_ptr != item_at_which_to_split_ptr )
         {
            temp_ptr = temp_ptr->next_ptr;
            count++;
         }
         temp_ptr->next_ptr = NULL;
      }
      else
      {
         list1_ptr->front_ptr = NULL;
      }
    
      list2_ptr->front_ptr = item_at_which_to_split_ptr;
      list2_ptr->back_ptr  = list1_ptr->back_ptr;
      list2_ptr->size      = total_size - count;
      list1_ptr->back_ptr  = temp_ptr;
      list1_ptr->size      = count;

    LIST_MUTEX_FREE(&(list1_ptr->pkcsListCriticalSection));
    LIST_MUTEX_FREE(&(list2_ptr->pkcsListCriticalSection));

   return;
} /* END list_split */


/* ------------------------------------------------------------------------
** 
** Ordered Lists
** - These support all list operations except for push_before and
** - push_after, which would disrupt the order of a list.
** ------------------------------------------------------------------------ */

/* ==================================================================
FUNCTION ORDERED_LIST_INIT
DESCRIPTION
   Initializes an ordered list.
===================================================================== */
void ordered_list_init(
   ordered_list_type       *list_ptr,
   ordered_list_config_type sort_order,
   ordered_list_config_type compare_type
)
{
   LIST_INTERCEPT_ORDERED_LIST_INIT( list_ptr );

#ifdef FEATURE_QDSP6
qurt_pimutex_init ( &(list_ptr->pkcsListCriticalSection));
#else /* FEATURE_QDSP6 */
   #ifdef FEATURE_WINCE
      #error code not present
#else /* FEATURE_WINCE */
      rex_init_crit_sect(&(list_ptr->pkcsListCriticalSection));
   #endif /* FEATURE_WINCE */
#endif /* FEATURE_QDSP6 */   

      LIST_ASSERT( list_ptr != NULL, NULL );
      list_ptr->back_ptr  = NULL;
      list_ptr->front_ptr = NULL;
      list_ptr->size      = 0;
      list_ptr->type      = sort_order | compare_type;
      list_ptr->list_initialized = 1;
    return;
} /* END ordered_list_init */

/* ==================================================================
FUNCTION ORDERED_LIST_IS_INITIALIZED
DESCRIPTION
   Check if the ordered list is initialized.
===================================================================== */
unsigned int
ordered_list_is_initialized(
   ordered_list_type *list_ptr
)
{
   LIST_ASSERT_RETURN( list_ptr != NULL , NULL);
   return  list_ptr->list_initialized;
}
/* ==================================================================
FUNCTION ORDERED_LIST_DESTROY
DESCRIPTION
   Destroys an ordered list if the list is empty.
===================================================================== */
void ordered_list_destroy(
   ordered_list_type       *list_ptr
)
{
   LIST_INTERCEPT_ORDERED_LIST_DESTROY( list_ptr );
   LIST_ASSERT( list_ptr != NULL, NULL );

   /* Assert if the list is not empty and you get here */
   LIST_ASSERT( list_ptr->size == 0, NULL );

#ifdef FEATURE_QDSP6
  qurt_pimutex_destroy(&(list_ptr->pkcsListCriticalSection));
#else /* FEATURE_QDSP6 */
   #ifdef FEATURE_WINCE
      #error code not present
#else /* FEATURE_WINCE */
      rex_del_crit_sect(&(list_ptr->pkcsListCriticalSection));
   #endif /* FEATURE_WINCE */
#endif /* FEATURE_QDSP6 */   
   list_ptr->back_ptr         = NULL;
   list_ptr->front_ptr        = NULL;
   list_ptr->size             = 0;
   list_ptr->list_initialized = 0;
   return;
} /* END ordered_list_destroy */


/* ==================================================================
FUNCTION ORDERED_LIST_PUSH_AFTER
DESCRIPTION
   Pushes an item after another specified item in an ordered list.
   The caller must check to ensure that this operation does not 
   disrupt the order of the list.
   Note: This function is not exported.
===================================================================== */
INLINE void ordered_list_push_after(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_to_push_ptr,
   ordered_list_link_type *item_to_push_after_ptr
)
{
   item_to_push_ptr->next_ptr = item_to_push_after_ptr->next_ptr;
   item_to_push_ptr->prev_ptr = item_to_push_after_ptr;
   item_to_push_after_ptr->next_ptr = item_to_push_ptr;
   if ( list_ptr->back_ptr == item_to_push_after_ptr )
   {
      list_ptr->back_ptr = item_to_push_ptr;
   }
   else
   {
      item_to_push_ptr->next_ptr->prev_ptr = item_to_push_ptr;
   }
   list_ptr->size++;

   return;

} /* END ordered_list_push_after */

/* ==================================================================
FUNCTION ORDERED_LIST_PUSH_BEFORE
DESCRIPTION
   Pushes an item before another specified item in an ordered list.
   The caller must check to ensure that this operation does not 
   disrupt the order of the list.
   Note: This function is not exported.
===================================================================== */
INLINE void ordered_list_push_before(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_to_push_ptr,
   ordered_list_link_type *item_to_push_before_ptr
)
{
   item_to_push_ptr->next_ptr = item_to_push_before_ptr;
   item_to_push_ptr->prev_ptr = item_to_push_before_ptr->prev_ptr;
   item_to_push_before_ptr->prev_ptr = item_to_push_ptr;

   if ( list_ptr->front_ptr == item_to_push_before_ptr )
   {
      list_ptr->front_ptr = item_to_push_ptr;
   }
   else
   {
      item_to_push_ptr->prev_ptr->next_ptr = item_to_push_ptr;
   }
   list_ptr->size++;

   return;

} /* END ordered_list_push_before */

/* ==================================================================
FUNCTION ORDERED_LIST_PUSH
DESCRIPTION
   Pushes an item on an ordered list. The point of insertion depends
   on the weight of the item, and on the type of list (ascending,
   descending, etc.)
===================================================================== */
void ordered_list_push(
   ordered_list_type        *list_ptr,
   ordered_list_link_type   *item_link_ptr,
   ordered_list_weight_type  weight
)
{
   ordered_list_link_type *temp_ptr;
   ordered_list_link_type *back_ptr;

   LIST_INTERCEPT_ORDERED_LIST_PUSH( list_ptr, item_link_ptr, weight );

  /* Disable interrupts and save PSW */
   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT( ordered_list_is_valid( list_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( item_link_ptr != NULL, &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( !item_is_in_ordered_list( list_ptr, item_link_ptr ), &(list_ptr->pkcsListCriticalSection));

      item_link_ptr->weight = weight;

      if ( list_ptr->size == 0 )
      {
         item_link_ptr->next_ptr = item_link_ptr->prev_ptr = NULL;
         list_ptr->front_ptr = list_ptr->back_ptr = item_link_ptr;
         list_ptr->size++;
      }
      else
      {
         temp_ptr = list_ptr->front_ptr;
         back_ptr = list_ptr->back_ptr;
         switch( list_ptr->type )
         {
            case ORDERED_LIST_ASCENDING_PUSH_SLT:
               if ( back_ptr->weight < weight )
               {
                  ordered_list_push_after( list_ptr, item_link_ptr, back_ptr );
               }
               else
               {
                  while ( temp_ptr->weight < weight )
                  {
                     temp_ptr = temp_ptr->next_ptr;
                  }
                  ordered_list_push_before( list_ptr, item_link_ptr, temp_ptr );
               }
               break;

            case ORDERED_LIST_ASCENDING_PUSH_LTE:
               if ( back_ptr->weight <= weight )
               {
                  ordered_list_push_after( list_ptr, item_link_ptr, back_ptr );
               }
               else
               {
                  while ( temp_ptr->weight <= weight )
                  {
                     temp_ptr = temp_ptr->next_ptr;
                  }
                  ordered_list_push_before( list_ptr, item_link_ptr, temp_ptr );
               }
               break;

            case ORDERED_LIST_DESCENDING_PUSH_SLT:
               if ( back_ptr->weight > weight )
               {
                  ordered_list_push_after( list_ptr, item_link_ptr, back_ptr );
               }
               else
               {
                  while ( temp_ptr->weight > weight )
                  {
                     temp_ptr = temp_ptr->next_ptr;
                  }
                  ordered_list_push_before( list_ptr, item_link_ptr, temp_ptr );
               }
               break;

            case ORDERED_LIST_DESCENDING_PUSH_LTE:
               if ( back_ptr->weight >= weight )
               {
                  ordered_list_push_after( list_ptr, item_link_ptr, back_ptr );
               }
               else
               {
                  while ( temp_ptr->weight >= weight )
                  {
                     temp_ptr = temp_ptr->next_ptr;
                  }
                  ordered_list_push_before( list_ptr, item_link_ptr, temp_ptr );
               }
               break;

            default:
        break;
               /* Unknown list type.
               ** Wailing, gnashing of teeth, etc.
               */
    }/* switch */
      }

  LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

   return;

} /* END ordered_list_push */

/* ==================================================================
FUNCTION ORDERED_LIST_POP_FRONT
DESCRIPTION
   Removes an item from the front of an ordered list.
===================================================================== */
void*
ordered_list_pop_front(
   ordered_list_type *list_ptr
)
{
   ordered_list_link_type *ret_ptr = NULL;
   LIST_INTERCEPT_LIST_POP_FRONT( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

    LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));

      if( list_ptr->size > 0 )
      {
         ret_ptr = list_ptr->front_ptr;
         list_ptr->front_ptr = ret_ptr->next_ptr;
         if( list_ptr->front_ptr == NULL )
         {
            list_ptr->back_ptr = NULL;
         }
         else
         {
            list_ptr->front_ptr->prev_ptr = NULL;
         }
         list_ptr->size--;
      }

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));
 
    return ret_ptr;
} /* END ordered_list_pop_front */

/* ==================================================================
FUNCTION ORDERED_LIST_POP_BACK
DESCRIPTION
   Removes an item from the back of an ordered list.
===================================================================== */
void*
ordered_list_pop_back(
   ordered_list_type *list_ptr
)
{
   ordered_list_link_type *ret_ptr = NULL;
   LIST_INTERCEPT_ORDERED_LIST_POP_BACK( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

    LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      if( list_ptr->size > 0 )
      {
         ret_ptr = list_ptr->back_ptr;
         list_ptr->back_ptr = ret_ptr->prev_ptr;
         if ( list_ptr->back_ptr == NULL )
         {
            list_ptr->front_ptr = NULL;
         }
         else
         {
            list_ptr->back_ptr->next_ptr = NULL;
         }
         list_ptr->size--;
      }

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));
    return ret_ptr;
} /* END ordered_list_pop_back */

/* ==================================================================
FUNCTION ORDERED_LIST_SIZE
DESCRIPTION
   Returns the number of elements in an ordered list.
===================================================================== */
list_size_type
ordered_list_size(
   ordered_list_type *list_ptr
)
{
   list_size_type size;

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      size = list_ptr->size;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    LIST_INTERCEPT_ORDERED_LIST_SIZE( list_ptr );

    return size;
} /* END ordered_list_size */

/* ==================================================================
FUNCTION ORDERED_LIST_PEEK_FRONT
DESCRIPTION
   Returns a pointer to the first item in an ordered list, or NULL
   if the list is empty.
===================================================================== */
void*
ordered_list_peek_front(
   ordered_list_type *list_ptr
)
{
   void *item_ptr;
   LIST_INTERCEPT_ORDERED_LIST_PEEK_FRONT( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

    LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
    item_ptr = list_ptr->front_ptr;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

  return item_ptr;
} /* END ordered_list_peek_front */

/* ==================================================================
FUNCTION ORDERED_LIST_PEEK_BACK
DESCRIPTION
   Returns a pointer to the last item in an ordered list, or NULL
   if the list is empty.
===================================================================== */
void*
ordered_list_peek_back(
   ordered_list_type *list_ptr
)
{
   void *item_ptr;

   LIST_INTERCEPT_ORDERED_LIST_PEEK_BACK( list_ptr );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

    LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      item_ptr = list_ptr->back_ptr;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return item_ptr;

} /* END ordered_list_peek_back */

/* ==================================================================
FUNCTION ORDERED_LIST_POP_ITEM
DESCRIPTION
   Removes a specified item from an ordered list.
===================================================================== */
void ordered_list_pop_item(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_to_pop_ptr
)
{
   LIST_INTERCEPT_ORDERED_LIST_POP_ITEM( list_ptr, item_to_pop_ptr );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT( item_is_in_ordered_list( list_ptr, item_to_pop_ptr ) ,&(list_ptr->pkcsListCriticalSection));
      if ( item_to_pop_ptr == list_ptr->front_ptr )
      {
         list_ptr->front_ptr = item_to_pop_ptr->next_ptr;
      }
      else
      {
         item_to_pop_ptr->prev_ptr->next_ptr = item_to_pop_ptr->next_ptr;
      }
      if ( item_to_pop_ptr == list_ptr->back_ptr )
      {
         list_ptr->back_ptr = item_to_pop_ptr->prev_ptr;
      }
      else
      {
         item_to_pop_ptr->next_ptr->prev_ptr = item_to_pop_ptr->prev_ptr;
      }
      list_ptr->size--;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

   return;
} /* END ordered_list_pop_item */

/* ==================================================================
FUNCTION ORDERED_LIST_PEEK_NEXT
DESCRIPTION
   Returns a pointer to the item following a specified item in an
   ordered list, or NULL if the input item is the last item in the
   list.
===================================================================== */
void*
ordered_list_peek_next(
   ordered_list_type      *list_ptr,
      /*lint -esym(715,list_ptr)
      ** Have lint not complain about the ignored parameter 'list_ptr'.
      */
   ordered_list_link_type *item_after_which_to_peek
)
{
   void *item_ptr;

   LIST_INTERCEPT_ORDERED_LIST_PEEK_NEXT( list_ptr, item_after_which_to_peek );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT_RETURN( item_is_in_ordered_list( list_ptr, item_after_which_to_peek ), &(list_ptr->pkcsListCriticalSection));
      item_ptr = item_after_which_to_peek->next_ptr;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

    return item_ptr;

} /* END ordered_list_peek_next */

/* ==================================================================
FUNCTION ORDERED_LIST_PEEK_PREV
DESCRIPTION
   Returns a pointer to the item preceding a specified item in an
   ordered list, or NULL if the input item is the last item in the
   list.
===================================================================== */
void*
ordered_list_peek_prev(
   ordered_list_type      *list_ptr,
      /*lint -esym(715,list_ptr)
      ** Have lint not complain about the ignored parameter 'list_ptr'.
      */
   ordered_list_link_type *item_before_which_to_peek
)
{
   void *item_ptr;

   LIST_INTERCEPT_ORDERED_LIST_PEEK_PREV( list_ptr, item_before_which_to_peek );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

      LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ), &(list_ptr->pkcsListCriticalSection));
      LIST_ASSERT_RETURN( item_is_in_ordered_list( list_ptr, item_before_which_to_peek ), &(list_ptr->pkcsListCriticalSection));
      item_ptr = item_before_which_to_peek->prev_ptr;

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

  return item_ptr;
} /* END ordered_list_peek_prev */

/* ==================================================================
FUNCTION ORDERED_LIST_LINEAR_SEARCH

DESCRIPTION
   Searches an ordered list for a matching item. The match is
   determined by means of a user-provided compare function. If there
   is no matching item in the list, NULL is returned. If there is
   more than one matching item, only the first match is returned.

===================================================================== */
void*
ordered_list_linear_search(
   ordered_list_type      *list_ptr,  
   list_compare_func_type  compare_func,
   void                   *compare_val
)
{
   ordered_list_link_type *item_ptr = NULL;

   LIST_INTERCEPT_ORDERED_LIST_LINEAR_SEARCH(
                                             list_ptr,
                                             compare_func,
                                             compare_val
   );

   if(list_ptr->list_initialized != 1)
   {
      ordered_list_init(list_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list_ptr->pkcsListCriticalSection));

   LIST_ASSERT_RETURN( ordered_list_is_valid( list_ptr ),&(list_ptr->pkcsListCriticalSection));
   LIST_ASSERT_RETURN( compare_func != NULL, &(list_ptr->pkcsListCriticalSection));
   item_ptr = list_ptr->front_ptr;
   while( item_ptr != NULL )
   {
      if( compare_func( item_ptr, compare_val ) != 0 )
      {
         break;
      }
      item_ptr = item_ptr->next_ptr;
   }

    LIST_MUTEX_FREE(&(list_ptr->pkcsListCriticalSection));

   return item_ptr;
} /* END ordered_list_linear_search */

#if 0
/* ==================================================================
FUNCTION ORDERED_LIST_LINEAR_DELETE
DESCRIPTION
   Searches an ordered list for a matching item and deletes the item.
   The match is determined by means of a user-provided compare
   function.
===================================================================== */
void
ordered_list_linear_delete(
  list_type             *list_ptr,  
  list_compare_func_type compare_func,
  void                  *compare_val
)
{
   return;
} /* END ordered_list_linear_delete */
#endif


/* ==================================================================
FUNCTION ORDERED_LIST_APPEND
DESCRIPTION
   Appends an ordered list to another. The caller has to ensure that
   this operation does not disrupt the order of the list.
   NOTE: This function is not exported.
===================================================================== */
INLINE void ordered_list_append(
   ordered_list_type *list1_ptr,
   ordered_list_type *list2_ptr
)
{
   if(list1_ptr->list_initialized != 1)
   {
      ordered_list_init(list1_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   if(list2_ptr->list_initialized != 1)
   {
      ordered_list_init(list2_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_LOCK(&(list2_ptr->pkcsListCriticalSection));

      LIST_ASSERT_2( ordered_list_is_valid( list1_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection));
      LIST_ASSERT_2( ordered_list_is_valid( list2_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection));
      if ( list1_ptr->size > 0 )
      {
         list1_ptr->back_ptr->next_ptr = list2_ptr->front_ptr;
      }
      else
      {
         list1_ptr->front_ptr = list2_ptr->front_ptr;
      }
      if ( list2_ptr->size > 0 )
      {
         list2_ptr->front_ptr->prev_ptr = list1_ptr->back_ptr;
      }

      list1_ptr->back_ptr = list2_ptr->back_ptr;
      list1_ptr->size += list2_ptr->size;

      list2_ptr->front_ptr = list2_ptr->back_ptr = NULL;
      list2_ptr->size  = 0;

   LIST_MUTEX_FREE(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_FREE(&(list2_ptr->pkcsListCriticalSection));

   return;
} /* END ordered_list_append */


/* ==================================================================
FUNCTION ORDERED_LIST_SWAP
DESCRIPTION
   Swaps two ordered lists by swapping their head nodes.
   For internal use only.
===================================================================== */
INLINE void ordered_list_swap(
   ordered_list_type *list1_ptr,
   ordered_list_type *list2_ptr
)
{
   ordered_list_type temp_list_hdr;

   temp_list_hdr = *list1_ptr;
   *list1_ptr    = *list2_ptr;
   *list2_ptr    = temp_list_hdr;

   return;

} /* END ordered_list_swap */


/* ==================================================================
FUNCTION ORDERED_LIST_MERGE
DESCRIPTION
   Merges two ordered lists. The two lists must be of the same type;
   that is, both should be sorted the same way and the insertion rule
   (SLT or LTE) should be the same as well. After the merge, the first
   list will contain all the elements in both lists. The second list
   will be empty.
===================================================================== */
void ordered_list_merge(
   ordered_list_type *list1_ptr,
   ordered_list_type *list2_ptr
)
{
   ordered_list_link_type *temp1_ptr;
   ordered_list_link_type *temp2_ptr;
   ordered_list_link_type *temp3_ptr;
   uint8 return_status = 0;

   if(list1_ptr->list_initialized != 1)
   {
      ordered_list_init(list1_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   if(list2_ptr->list_initialized != 1)
   {
      ordered_list_init(list2_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_LOCK(&(list2_ptr->pkcsListCriticalSection));

    LIST_ASSERT_2( ordered_list_is_valid( list1_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection));
    LIST_ASSERT_2( ordered_list_is_valid( list2_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection));
    if ( list1_ptr->size == 0 )
    {
       list1_ptr->size      = list2_ptr->size;
       list1_ptr->front_ptr = list2_ptr->front_ptr;
       list1_ptr->back_ptr  = list2_ptr->back_ptr;
       list2_ptr->size      = 0;
       list2_ptr->front_ptr = NULL;
       list2_ptr->back_ptr  = NULL;
       return_status = 1;
    }

    else if ( list2_ptr->size == 0 )
    {
       return_status = 1;
    }

  else if ( list1_ptr->type != list2_ptr->type )
      {
    return_status = 1;
      }

  else
  {
      /* If the second list can be appended to the first without destroying
      ** order, do so. This check is done because in some use cases, the
      ** lists merged satisfy this property.
      */
      switch( list2_ptr->type )
      {
         case ORDERED_LIST_ASCENDING_PUSH_SLT:
            if ( list1_ptr->back_ptr->weight <= list2_ptr->front_ptr->weight )
            {
               /* Case 1a: All items in list1 are smaller than or equal to
               ** the smallest item in list2. Append list2 to list1.
               */
               ordered_list_append( list1_ptr, list2_ptr );
          return_status = 1;
            }

        else if ( list2_ptr->back_ptr->weight < list1_ptr->front_ptr->weight )
            {
               /* Case 1b: All items in list2 are smaller than the
               ** smallest item in list1. Append list1 to list2.
               */
               ordered_list_swap( list1_ptr, list2_ptr );
               ordered_list_append( list1_ptr, list2_ptr );
          return_status = 1;
            }

        else
        {
            /* 
            ** Case 2: A simple append is not possible, so do a merge. Scan
            ** both lists, moving items from the second list to the first
            ** where appropriate.
            */
            temp1_ptr = list1_ptr->front_ptr;
            while ( list2_ptr->front_ptr != NULL )
            {
               temp2_ptr = temp3_ptr = list2_ptr->front_ptr;
      
               if ( list1_ptr->back_ptr->weight <= temp2_ptr->weight )
               {
                  /* Append list2 to list1.
                  */
                  list1_ptr->back_ptr->next_ptr  = list2_ptr->front_ptr;
                  list2_ptr->front_ptr->prev_ptr = list1_ptr->back_ptr;
                  list1_ptr->back_ptr            = list2_ptr->back_ptr;
                  list2_ptr->front_ptr           = NULL;
               }
               else
               {
                  while ( temp1_ptr->weight <= temp2_ptr->weight )
                  {
                     temp1_ptr = temp1_ptr->next_ptr;
                  }
                  if ( temp1_ptr->weight > list2_ptr->back_ptr->weight )
                  {
                     temp3_ptr = list2_ptr->back_ptr;
                  }
                  else
                  {
                     while ( temp1_ptr->weight > temp3_ptr->next_ptr->weight )
                     {
                        temp3_ptr = temp3_ptr->next_ptr;
                     }
                  }
                  list2_ptr->front_ptr           = temp3_ptr->next_ptr;
                  if ( list2_ptr->front_ptr != NULL )
                  {
                     list2_ptr->front_ptr->prev_ptr = NULL;
                  }
                  temp2_ptr->prev_ptr            = temp1_ptr->prev_ptr;
                  if ( temp1_ptr->prev_ptr != NULL )
                  {
                     temp1_ptr->prev_ptr->next_ptr = temp2_ptr;
                  }
                  else
                  {
                     list1_ptr->front_ptr = temp2_ptr;
                  }
                  temp3_ptr->next_ptr = temp1_ptr;
                  temp1_ptr->prev_ptr = temp3_ptr;
               }
            }
        }
            break;
            
         case ORDERED_LIST_ASCENDING_PUSH_LTE:
            if ( list1_ptr->back_ptr->weight < list2_ptr->front_ptr->weight )
            {
               /* Case 1a: All items in list1 are smaller than the
               ** smallest item in list2. Append list2 to list1.
               */
                ordered_list_append( list1_ptr, list2_ptr );
          return_status = 1;
            }

        else if ( list2_ptr->back_ptr->weight <= list1_ptr->front_ptr->weight )
            {
               /* Case 1b: All items in list2 are smaller than or equal to
               ** the smallest item in list1. Append list1 to list2.
               */
               ordered_list_swap( list1_ptr, list2_ptr );
               ordered_list_append( list1_ptr, list2_ptr );
          return_status = 1;
            }

        else
        {
            /* 
            ** Case 2: A simple append is not possible, so do a merge. Scan
            ** both lists, moving items from the second list to the first
            ** where appropriate.
            */
            temp1_ptr = list1_ptr->front_ptr;
            while ( list2_ptr->front_ptr != NULL )
            {
               temp2_ptr = temp3_ptr = list2_ptr->front_ptr;
      
               if ( list1_ptr->back_ptr->weight < temp2_ptr->weight )
               {
                  /* Append list2 to list1.
                  */
                  list1_ptr->back_ptr->next_ptr  = list2_ptr->front_ptr;
                  list2_ptr->front_ptr->prev_ptr = list1_ptr->back_ptr;
                  list1_ptr->back_ptr            = list2_ptr->back_ptr;
                  list2_ptr->front_ptr           = NULL;
               }
               else
               {
                  while ( temp1_ptr->weight < temp2_ptr->weight )
                  {
                     temp1_ptr = temp1_ptr->next_ptr;
                  }
                  if ( temp1_ptr->weight >= list2_ptr->back_ptr->weight )
                  {
                     temp3_ptr = list2_ptr->back_ptr;
                  }
                  else
                  {
                     while ( temp1_ptr->weight >= temp3_ptr->next_ptr->weight )
                     {
                        temp3_ptr = temp3_ptr->next_ptr;
                     }
                  }
                  list2_ptr->front_ptr           = temp3_ptr->next_ptr;
                  if ( list2_ptr->front_ptr != NULL )
                  {
                     list2_ptr->front_ptr->prev_ptr = NULL;
                  }
                  temp2_ptr->prev_ptr            = temp1_ptr->prev_ptr;
                  if ( temp1_ptr->prev_ptr != NULL )
                  {
                     temp1_ptr->prev_ptr->next_ptr = temp2_ptr;
                  }
                  else
                  {
                     list1_ptr->front_ptr = temp2_ptr;
                  }
                  temp3_ptr->next_ptr = temp1_ptr;
                  temp1_ptr->prev_ptr = temp3_ptr;
               }
            }
        }
            break;
  
         case ORDERED_LIST_DESCENDING_PUSH_SLT:
            if ( list1_ptr->back_ptr->weight >= list2_ptr->front_ptr->weight )
            {
               /* Case 1a: All items in list1 are greater than or equal to
               ** the biggest item in list2. Append list2 to list1.
               */
                ordered_list_append( list1_ptr, list2_ptr );
          return_status = 1;
            }

        else if ( list2_ptr->back_ptr->weight > list1_ptr->front_ptr->weight )
            {
               /* Case 1b: All items in list2 are greater than the
               ** biggest item in list1. Append list1 to list2.
               */
               ordered_list_swap( list1_ptr, list2_ptr );
               ordered_list_append( list1_ptr, list2_ptr );
          return_status = 1;
            }
        else
        {
            /* 
            ** Case 2: A simple append is not possible, so do a merge. Scan
            ** both lists, moving items from the second list to the first
            ** where appropriate.
            */
            temp1_ptr = list1_ptr->front_ptr;
            while ( list2_ptr->front_ptr != NULL )
            {
               temp2_ptr = temp3_ptr = list2_ptr->front_ptr;
      
               if ( list1_ptr->back_ptr->weight >= temp2_ptr->weight )
               {
                  /* Append list2 to list1.
                  */
                  list1_ptr->back_ptr->next_ptr  = list2_ptr->front_ptr;
                  list2_ptr->front_ptr->prev_ptr = list1_ptr->back_ptr;
                  list1_ptr->back_ptr            = list2_ptr->back_ptr;
                  list2_ptr->front_ptr           = NULL;
               }
               else
               {
                  while ( temp1_ptr->weight >= temp2_ptr->weight )
                  {
                     temp1_ptr = temp1_ptr->next_ptr;
                  }
                  if ( temp1_ptr->weight < list2_ptr->back_ptr->weight )
                  {
                     temp3_ptr = list2_ptr->back_ptr;
                  }
                  else
                  {
                     while ( temp1_ptr->weight < temp3_ptr->next_ptr->weight )
                     {
                        temp3_ptr = temp3_ptr->next_ptr;
                     }
                  }
                  list2_ptr->front_ptr           = temp3_ptr->next_ptr;
                  if ( list2_ptr->front_ptr != NULL )
                  {
                     list2_ptr->front_ptr->prev_ptr = NULL;
                  }
                  temp2_ptr->prev_ptr            = temp1_ptr->prev_ptr;
                  if ( temp1_ptr->prev_ptr != NULL )
                  {
                     temp1_ptr->prev_ptr->next_ptr = temp2_ptr;
                  }
                  else
                  {
                     list1_ptr->front_ptr = temp2_ptr;
                  }
                  temp3_ptr->next_ptr = temp1_ptr;
                  temp1_ptr->prev_ptr = temp3_ptr;
               }
            }
        }
            break;

         case ORDERED_LIST_DESCENDING_PUSH_LTE:
            if ( list1_ptr->back_ptr->weight > list2_ptr->front_ptr->weight )
            {
               /* Case 1a: All items in list1 are greater than the
               ** biggest item in list2. Append list2 to list1.
               */
                ordered_list_append( list1_ptr, list2_ptr );
            return_status = 1;
            }

        else if ( list2_ptr->back_ptr->weight >= list1_ptr->front_ptr->weight )
            {
               /* Case 1b: All items in list2 are greater than or equal to
               ** the biggest item in list1. Append list1 to list2.
               */
               ordered_list_swap( list1_ptr, list2_ptr );
               ordered_list_append( list1_ptr, list2_ptr );
          return_status = 1;
            }

        else
        {
            /* 
            ** Case 2: A simple append is not possible, so do a merge. Scan
            ** both lists, moving items from the second list to the first
            ** where appropriate.
            */
            temp1_ptr = list1_ptr->front_ptr;
            while ( list2_ptr->front_ptr != NULL )
            {
               temp2_ptr = temp3_ptr = list2_ptr->front_ptr;
      
               if ( list1_ptr->back_ptr->weight > temp2_ptr->weight )
               {
                  /* Append list2 to list1.
                  */
                  list1_ptr->back_ptr->next_ptr  = list2_ptr->front_ptr;
                  list2_ptr->front_ptr->prev_ptr = list1_ptr->back_ptr;
                  list1_ptr->back_ptr            = list2_ptr->back_ptr;
                  list2_ptr->front_ptr           = NULL;
               }
               else
               {
                  while ( temp1_ptr->weight > temp2_ptr->weight )
                  {
                     temp1_ptr = temp1_ptr->next_ptr;
                  }
                  if ( temp1_ptr->weight <= list2_ptr->back_ptr->weight )
                  {
                     temp3_ptr = list2_ptr->back_ptr;
                  }
                  else
                  {
                     while ( temp1_ptr->weight <= temp3_ptr->next_ptr->weight )
                     {
                        temp3_ptr = temp3_ptr->next_ptr;
                     }
                  }
                  list2_ptr->front_ptr           = temp3_ptr->next_ptr;
                  if ( list2_ptr->front_ptr != NULL )
                  {
                     list2_ptr->front_ptr->prev_ptr = NULL;
                  }
                  temp2_ptr->prev_ptr            = temp1_ptr->prev_ptr;
                  if ( temp1_ptr->prev_ptr != NULL )
                  {
                     temp1_ptr->prev_ptr->next_ptr = temp2_ptr;
                  }
                  else
                  {
                     list1_ptr->front_ptr = temp2_ptr;
                  }
                  temp3_ptr->next_ptr = temp1_ptr;
                  temp1_ptr->prev_ptr = temp3_ptr;
               }
            }
        }
            break;

         default:
        break;

    } /* switch */

  } /* else */

  if(return_status == 0)
  {
   list1_ptr->size    += list2_ptr->size;
   list2_ptr->size     = 0;
   list1_ptr->back_ptr = list2_ptr->back_ptr;
   list2_ptr->back_ptr = NULL;
  }
    
   LIST_MUTEX_FREE(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_FREE(&(list2_ptr->pkcsListCriticalSection));

   return;
} /* END ordered_list_merge */


/* ==================================================================
FUNCTION ORDERED_LIST_SPLIT
DESCRIPTION
   Splits an ordered list at a specified item. Items before the
   specified item are retained in the original list. The specified
   item and items following it are placed into a new list.
===================================================================== */
void ordered_list_split(
   ordered_list_type      *list1_ptr,
   ordered_list_link_type *item_at_which_to_split_ptr,
   ordered_list_type      *list2_ptr
)
{
   ordered_list_link_type *temp_ptr   = NULL;
   unsigned long           count      = 0;
   unsigned long           total_size = list1_ptr->size;

   if(list1_ptr->list_initialized != 1)
   {
      ordered_list_init(list1_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   if(list2_ptr->list_initialized != 1)
   {
      ordered_list_init(list2_ptr, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_SLT);
   }
   LIST_MUTEX_LOCK(&(list1_ptr->pkcsListCriticalSection));
   LIST_MUTEX_LOCK(&(list2_ptr->pkcsListCriticalSection));

      LIST_ASSERT_2( ordered_list_is_valid( list1_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection));
      LIST_ASSERT_2( ordered_list_is_valid( list2_ptr ), &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection) );
      LIST_ASSERT_2( item_is_in_ordered_list(
                                           list1_ptr,
                                           item_at_which_to_split_ptr
                   ) , &(list1_ptr->pkcsListCriticalSection), &(list2_ptr->pkcsListCriticalSection) );

      /* Figure out the number of elements in the first list after the
      ** split.
      */
      if ( list1_ptr->front_ptr != item_at_which_to_split_ptr )
      {
         count++;
         temp_ptr = list1_ptr->front_ptr;
         while ( temp_ptr->next_ptr != item_at_which_to_split_ptr )
         {
            temp_ptr = temp_ptr->next_ptr;
            count++;
         }
         temp_ptr->next_ptr = NULL;
      }
      else
      {
         list1_ptr->front_ptr = NULL;
      }
 
      item_at_which_to_split_ptr->prev_ptr = NULL;
      list2_ptr->front_ptr = item_at_which_to_split_ptr;
      list2_ptr->back_ptr  = list1_ptr->back_ptr;
      list2_ptr->size      = total_size - count;
      list1_ptr->back_ptr  = temp_ptr;
      list1_ptr->size      = count;

    LIST_MUTEX_FREE(&(list1_ptr->pkcsListCriticalSection));
    LIST_MUTEX_FREE(&(list2_ptr->pkcsListCriticalSection));

   return;

} /* END ordered_list_split */




/* ------------------------------------------------------------------------
** 
** Sanity checks that are invoked in debug mode
**
** ------------------------------------------------------------------------ */

void list_croak( )
{
   LIST_ASSERT( 0, NULL );
} /* END list_croak */


unsigned long
list_is_valid(
   list_type *list_ptr
)
{
   list_link_type *item_ptr;
   unsigned long   cur_size = 0;

   LIST_ASSERT_RETURN( list_ptr != NULL, NULL );

   item_ptr = list_ptr->front_ptr;
   while( item_ptr != NULL )
   {
      cur_size++;
      if ( cur_size > list_ptr->size )
      {
         return 0;
      }
      item_ptr = item_ptr->next_ptr;
   }

   if ( cur_size != list_ptr->size )
   {
      return 0;
   }
   else
   {
      return 1;
   }
} /* END list_is_valid */


unsigned long
item_is_in_list(
   list_type      *list_ptr,
   list_link_type *item_ptr
)
{
   list_link_type *cur_item_ptr;

   LIST_ASSERT_RETURN( list_ptr != NULL, NULL );
   LIST_ASSERT_RETURN( item_ptr != NULL, NULL );

   cur_item_ptr = list_ptr->front_ptr;
   while( cur_item_ptr != NULL && cur_item_ptr != item_ptr )
   {
      cur_item_ptr = cur_item_ptr->next_ptr;
   }

   if ( cur_item_ptr == NULL )
   {
      return 0;
   }
   else
   {
      return 1;
   }
} /* END item_is_in_list */


unsigned long
ordered_list_is_valid( ordered_list_type *list_ptr )
{
   ordered_list_link_type *item_ptr;
   unsigned long   cur_size = 0;

   LIST_ASSERT_RETURN( list_ptr != NULL, NULL );

   item_ptr = list_ptr->front_ptr;
   while( item_ptr != NULL )
   {
      cur_size++;
      if ( cur_size > list_ptr->size )
      {
         return 0;
      }
      item_ptr = item_ptr->next_ptr;
   }

   if ( cur_size != list_ptr->size )
   {
      return 0;
   }
   else
   {
      return 1;
   }
} /* END ordered_list_is_valid */


unsigned long
item_is_in_ordered_list(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_ptr
)
{
   ordered_list_link_type *cur_item_ptr;

   LIST_ASSERT_RETURN( list_ptr != NULL, NULL );
   LIST_ASSERT_RETURN( item_ptr != NULL, NULL);

   cur_item_ptr = list_ptr->front_ptr;
   while( cur_item_ptr != NULL && cur_item_ptr != item_ptr )
   {
      cur_item_ptr = cur_item_ptr->next_ptr;
   }

   if ( cur_item_ptr == NULL )
   {
      return 0;
   }
   else
   {
      return 1;
   }
} /* END item_is_in_ordered_list */



