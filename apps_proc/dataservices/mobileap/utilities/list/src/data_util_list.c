/***************************************************************************************************
    @file
    data_util_list.c

    @brief
    Implements functions supported in util_list.h.

    Copyright (c) 2013,2019,2021 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include "data_util_list.h"
#include "data_util_memory.h"
#include "data_utils_common.h"



/***************************************************************************************************
    @function
    data_util_list_delete_helper

    @brief
    Deletes user data from the the list given the actual node.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list
        to_be_deleted_node
           pointer to the node that needs to be deleted from the list
        is_delete_data
           TRUE (or FALSE) if user data needs to be free (or not)
        delete_evaluator
           pointer to function that is used to free the memory of the user data stored
           in a node when it is being removed from the list
           can be NULL if a default delete evaluator was provided while creating the list
           or if is_delete_data is FALSE

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void data_util_list_delete_helper(data_util_list_info_type *list_info,
                                    data_util_list_node_type *to_be_deleted_node,
                                    int is_delete_data,
                                    delete_evaluator_type delete_evaluator);





/***************************************************************************************************
    @function
    data_util_list_auto_lock_list

    @brief
    Auto locks the list depending on the attributes of the list (UTIL_LIST_BIT_FIELD_AUTO_LOCK).

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list

    @param[out]
        none

    @retval
    ESUCCESS If the list has been locked successfully, appropriate error code otherwise
***************************************************************************************************/
static int data_util_list_auto_lock_list(data_util_list_info_type *list_info);





/***************************************************************************************************
    @function
    data_util_list_auto_unlock_list

    @brief
    Auto unlocks the list depending on the attributes of the list (UTIL_LIST_BIT_FIELD_AUTO_LOCK).

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list

    @param[out]
        none

    @retval
    ESUCCESS If the list has been unlocked successfully, appropriate error code otherwise
***************************************************************************************************/
static int data_util_list_auto_unlock_list(data_util_list_info_type *list_info);





/***************************************************************************************************
    @function
    data_util_list_init_list_sync_data

    @brief
    Creates synchronization object for the list.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list
        list_sync_data_bit_field
           attributes that need to be considered while creating the synchronization object

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void data_util_list_init_list_sync_data(data_util_list_info_type *list_info,
                                          data_util_bit_field_type list_sync_data_bit_field);





/***************************************************************************************************
    @function
    data_util_list_destroy_list_sync_data

    @brief
    Destroys synchronization object of the list.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void data_util_list_destroy_list_sync_data(data_util_list_info_type *list_info);





/***************************************************************************************************
    @function
    data_util_list_init_node_sync_data

    @brief
    Creates synchronization object for the node.

    @param[in]
        node_info
           pointer to the node data object
        node_sync_data_bit_field
           attributes that need to be considered while creating the synchronization object

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void data_util_list_init_node_sync_data(data_util_list_node_type *node_info,
                                          data_util_bit_field_type node_sync_data_bit_field);





/***************************************************************************************************
    @function
    data_util_list_destroy_node_sync_data

    @brief
    Destroys synchronization object of the node.

    @param[in]
        node_info
           pointer to the node data object

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void data_util_list_destroy_node_sync_data(data_util_list_node_type *node_info);






/***************************************************************************************************
    @function
    data_util_list_auto_lock_list

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_auto_lock_list(data_util_list_info_type *list_info)
{
    int ret;

    ret = ESUCCESS;

    if (NULL != list_info &&
        data_util_list_is_bits_set_in_list_bit_field(list_info,
                                                DATA_UTIL_LIST_BIT_FIELD_AUTO_LOCK,
                                                FALSE))
    {
        ret = data_util_list_lock_list(list_info);
    }

    return ret;
}

/***************************************************************************************************
    @function
    data_util_list_auto_unlock_list

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_auto_unlock_list(data_util_list_info_type *list_info)
{
    int ret;

    ret = ESUCCESS;

    if (NULL != list_info &&
        data_util_list_is_bits_set_in_list_bit_field(list_info,
                                                DATA_UTIL_LIST_BIT_FIELD_AUTO_LOCK,
                                                FALSE))
    {
        ret = data_util_list_unlock_list(list_info);
    }

    return ret;
}


/***************************************************************************************************
    @function
    data_util_list_delete_helper

    @implementation detail
    None.
***************************************************************************************************/
void data_util_list_delete_helper(data_util_list_info_type *list_info,
                             data_util_list_node_type *to_be_deleted_node,
                             int is_delete_data,
                             delete_evaluator_type delete_evaluator)
{
    if(list_info && to_be_deleted_node)
    {
        if(to_be_deleted_node == list_info->list_head)
        {
            list_info->list_head = to_be_deleted_node->next;
        }

        if(to_be_deleted_node == list_info->list_tail)
        {
            list_info->list_tail = to_be_deleted_node->prev;
        }

        if(to_be_deleted_node->prev)
        {
            to_be_deleted_node->prev->next = to_be_deleted_node->next;
        }

        if(to_be_deleted_node->next)
        {
            to_be_deleted_node->next->prev = to_be_deleted_node->prev;
        }

        to_be_deleted_node->prev = NULL;
        to_be_deleted_node->next = NULL;
        list_info->num_of_node--;

        if(is_delete_data)
        {
            if(!delete_evaluator)
            {
                if(list_info->default_delete_evaluator)
                {
                    delete_evaluator = list_info->default_delete_evaluator;
                }
//              else
//              {
//                  UTIL_LOG_MSG("data not freed - "
//                               "need to supply either default or custom delete evaluator");
//              }
            }

            if(delete_evaluator)
            {
                (*delete_evaluator) (&to_be_deleted_node->node_data);
                to_be_deleted_node->node_data.user_data = NULL;
            }
        }
        to_be_deleted_node->node_data.node_bit_field = NIL;
        data_util_list_destroy_node_sync_data(to_be_deleted_node);
        data_util_memory_free((void**) &to_be_deleted_node);
    }
}

/***************************************************************************************************
    @function
    data_util_list_init_list_sync_data

    @implementation detail
    None.
***************************************************************************************************/
void data_util_list_init_list_sync_data(data_util_list_info_type *list_info,
                                   data_util_bit_field_type list_sync_data_bit_field)
{
    if (NULL != list_info)
    {
        data_util_sync_data_init(&list_info->list_sync_data,
                            list_sync_data_bit_field);
    }
}

/***************************************************************************************************
    @function
    data_util_list_destroy_list_sync_data

    @implementation detail
    None.
***************************************************************************************************/
void data_util_list_destroy_list_sync_data(data_util_list_info_type *list_info)
{
    if (NULL != list_info)
    {
        data_util_sync_data_destroy(&list_info->list_sync_data);
    }
}

/***************************************************************************************************
    @function
    data_util_list_init_node_sync_data

    @implementation detail
    None.
***************************************************************************************************/
void data_util_list_init_node_sync_data(data_util_list_node_type *node_info,
                                   data_util_bit_field_type node_sync_data_bit_field)
{
    if (NULL != node_info)
    {
        data_util_sync_data_init(&node_info->node_data.node_sync_data,
                            node_sync_data_bit_field);
    }
}

/***************************************************************************************************
    @function
    data_util_list_destroy_node_sync_data

    @implementation detail
    None.
***************************************************************************************************/
void data_util_list_destroy_node_sync_data(data_util_list_node_type *node_info)
{
    if (NULL != node_info)
    {
        data_util_sync_data_destroy(&node_info->node_data.node_sync_data);
    }
}


/***************************************************************************************************
    @function
    data_util_list_create

    @implementation detail
    None.
***************************************************************************************************/
data_util_list_info_type* data_util_list_create(data_util_list_info_type *list_info,
                                      add_evaluator_type default_add_evaluator,
                                      delete_evaluator_type default_delete_evaluator,
                                      data_util_bit_field_type list_bit_field)
{
    data_util_bit_field_type list_sync_data_bit_field;

    list_sync_data_bit_field = NIL;

    if(!list_info)
    {
        if(data_util_bit_field_is_bits_set(list_bit_field,
                                      DATA_UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP,
                                      FALSE))
        {
            list_info = data_util_memory_alloc(sizeof(*list_info));
        }
//      else
//      {
//          UTIL_LOG_MSG("bit_field must include CREATED_ON_HEAP "
//                       "for list to be allocated on heap");
//      }
    }

    if(list_info)
    {
        memset(list_info,
               0,
               sizeof(*list_info));
        if(data_util_bit_field_is_bits_set(list_bit_field,
                                      DATA_UTIL_LIST_BIT_FIELD_USE_COND_VAR,
                                      FALSE))
        {
            data_util_bit_field_set_bits(&list_sync_data_bit_field,
                                    DATA_UTIL_SYNC_BIT_FIELD_COND_VAR_USED);
        }
        data_util_list_init_list_sync_data(list_info,
                                      list_sync_data_bit_field);
        list_info->list_head = NULL;
        list_info->list_tail = NULL;
        list_info->default_add_evaluator = default_add_evaluator;
        list_info->default_delete_evaluator = default_delete_evaluator;
        list_info->list_bit_field = list_bit_field;
    }
//  else
//  {
//      UTIL_LOG_MSG("Unable to create list\n");
//  }

    return list_info;
}

/***************************************************************************************************
    @function
    data_util_list_lock_list

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_lock_list(data_util_list_info_type *list_info)
{
    int ret;

    ret = ESUCCESS;

    if (NULL != list_info)
    {
        ret = data_util_sync_data_acquire_mutex(&list_info->list_sync_data);
    }

    return ret;
}

/***************************************************************************************************
    @function
    data_util_list_unlock_list

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_unlock_list(data_util_list_info_type *list_info)
{
    int ret;

    ret = ESUCCESS;

    if (NULL != list_info)
    {
        ret = data_util_sync_data_release_mutex(&list_info->list_sync_data);
    }

    return ret;
}


/***************************************************************************************************
    @function
    data_util_list_lock_node

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_lock_node(data_util_list_node_data_type *node_data)
{
    int ret;

    ret = ESUCCESS;

    if (NULL != node_data)
    {
        ret = data_util_sync_data_acquire_mutex(&node_data->node_sync_data);
    }

    return ret;
}

/***************************************************************************************************
    @function
    data_util_list_unlock_node

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_unlock_node(data_util_list_node_data_type *node_data)
{
    int ret;

    ret = ESUCCESS;

    if (NULL != node_data)
    {
        ret = data_util_sync_data_release_mutex(&node_data->node_sync_data);
    }

    return ret;
}

/***************************************************************************************************
    @function
    data_util_list_wait_on_node

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_wait_on_node(data_util_list_node_data_type *node_data,
                           int wait_for_time_seconds)
{
    int ret;

    ret = ESUCCESS;

    if (NULL != node_data)
    {
        ret = data_util_sync_data_wait_on_cond(&node_data->node_sync_data,
                                          wait_for_time_seconds);
    }

    return ret;
}


/***************************************************************************************************
    @function
    data_util_list_add

    @implementation detail
    Supports synchronous post operation by using synhronization objects.
***************************************************************************************************/
int data_util_list_add(data_util_list_info_type *list_info,
                  void *to_be_added_data,
                  add_evaluator_type add_evaluator,
                  data_util_bit_field_type node_bit_field)
{
    data_util_list_node_type *temp_node;
    data_util_list_node_type *iter;
    int is_inserted;
    int ret_code;
    int wait_until_data_is_processed;
    data_util_bit_field_type node_sync_data_bit_field;

    temp_node = NULL;
    iter = NULL;
    is_inserted = FALSE;
    ret_code = EINVAL;
    wait_until_data_is_processed = FALSE;
    node_sync_data_bit_field = NIL;

    if(list_info)
    {
        data_util_list_auto_lock_list(list_info);

        do
        {
            if(!add_evaluator)
            {
                if(list_info->default_add_evaluator)
                {
                    add_evaluator = list_info->default_add_evaluator;
                }
                else
                {
//                  UTIL_LOG_MSG("data not added - "
//                               "need to supply either default or custom add evaluator");
                    break;
                }
            }

            if(!to_be_added_data)
            {
                break;
            }

            temp_node = data_util_memory_alloc(sizeof(*temp_node));
            if(!temp_node)
            {
                break;
            }

            temp_node->node_data.user_data = to_be_added_data;
            temp_node->node_data.node_bit_field = node_bit_field;
            temp_node->prev = NULL;
            temp_node->next = NULL;

            if(!list_info->list_head || !list_info->list_tail)
            {
                list_info->list_head = temp_node;
                list_info->list_tail = temp_node;
            }
            else
            {
                iter = list_info->list_head;
                while(iter &&
                      FALSE == (*add_evaluator) (&temp_node->node_data,
                                                         &iter->node_data))
                {
                    iter = iter->next;
                }

                if(iter)
                {
                    temp_node->next = iter;
                    if(iter->prev)
                    {
                        iter->prev->next = temp_node;
                        temp_node->prev = iter->prev;
                    }
                    iter->prev = temp_node;

                    if(iter == list_info->list_head)
                    {
                        list_info->list_head = temp_node;
                    }

                    is_inserted = TRUE;
                }

                if(!is_inserted)
                {
                    list_info->list_tail->next = temp_node;
                    temp_node->prev = list_info->list_tail;
                    list_info->list_tail = temp_node;
                }
            }

            list_info->num_of_node++;

            ret_code = ESUCCESS;
        } while(FALSE);

        if(ret_code && data_util_bit_field_is_bits_set(node_bit_field,
                                                  DATA_UTIL_LIST_NODE_BIT_FIELD_USE_SYNC_VAR,
                                                  FALSE))
        {
            data_util_bit_field_set_bits(&node_sync_data_bit_field,
                                    DATA_UTIL_SYNC_BIT_FIELD_COND_VAR_USED);
            data_util_list_init_node_sync_data(temp_node,
                                          node_sync_data_bit_field);
            data_util_list_lock_node(&temp_node->node_data);
            wait_until_data_is_processed = TRUE;

        }

        data_util_list_auto_unlock_list(list_info);

        if(wait_until_data_is_processed)
        {
            data_util_list_wait_on_node(&temp_node->node_data,
                                   NIL);
            data_util_list_unlock_node(&temp_node->node_data);
        }
    }

    return ret_code;
}


data_util_list_node_data_type* data_util_list_find_data_in_list_with_param(data_util_list_info_type *list_info, int (*find_evaluator)(const data_util_list_node_data_type *to_be_found_data, void *compare_data), void* compare_data)
{
    data_util_list_node_type *iter;
    data_util_list_node_type *found_node;

    iter = NULL;
    found_node = NULL;

    if(list_info && find_evaluator)
    {
        data_util_list_auto_lock_list(list_info);
        iter = list_info->list_head;
        while(iter && !((*find_evaluator) (&iter->node_data, compare_data)))
        {
            iter = iter->next;
        }
        data_util_list_auto_unlock_list(list_info);
    }
    found_node = iter;

    return (found_node ? &found_node->node_data : NULL);
}


/***************************************************************************************************
    @function
    data_util_list_cleanup

    @implementation detail
    None.
***************************************************************************************************/
void data_util_list_cleanup(data_util_list_info_type *list_info,
                       delete_evaluator_type delete_evaluator)
{
    data_util_list_node_type *iter;
    data_util_list_node_type *temp;

    iter = NULL;
    temp = NULL;

    if(list_info)
    {
        data_util_list_auto_lock_list(list_info);
        iter = list_info->list_head;
        while(iter)
        {
            temp = iter->next;
            data_util_list_delete_helper(list_info,
                                    iter,
                                    TRUE,
                                    delete_evaluator);
            iter = temp;
        }

        data_util_list_auto_unlock_list(list_info);

        data_util_list_destroy_list_sync_data(list_info);
        if(data_util_list_is_bits_set_in_list_bit_field(list_info,
                                                   DATA_UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP,
                                                   FALSE))
        {
            data_util_memory_free((void**) &list_info);
        }
    }
}


/***************************************************************************************************
    @function
    data_util_list_is_bits_set_in_list_bit_field

    @implementation detail
    None.
***************************************************************************************************/
int data_util_list_is_bits_set_in_list_bit_field(data_util_list_info_type *list_info,
                                            data_util_bit_field_type bits_to_be_checked,
                                            int is_partial_match_accepted)
{
    return (list_info &&
            data_util_bit_field_is_bits_set(list_info->list_bit_field,
                                       bits_to_be_checked,
                                       is_partial_match_accepted)
            ) ? TRUE : FALSE;
}


