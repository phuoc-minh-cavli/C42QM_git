/***************************************************************************************************
    @file
    data_util_list.h

    @brief
    Facilitates list related operations by providing list related utilities.

    Copyright (c) 2013, 2021 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef DATA_UTIL_LIST
#define DATA_UTIL_LIST

#include "data_utils_standard.h"
#include "data_util_synchronization.h"
#include "data_util_bit_field.h"

#define DATA_UTIL_LIST_BIT_FIELD_NONE                      ((uint64_t) 0)
#define DATA_UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP           (((uint64_t) 1) << 0)
#define DATA_UTIL_LIST_BIT_FIELD_SORTED                    (((uint64_t) 1) << 1)
#define DATA_UTIL_LIST_BIT_FIELD_AUTO_LOCK                 (((uint64_t) 1) << 2)
#define DATA_UTIL_LIST_BIT_FIELD_USE_COND_VAR              (((uint64_t) 1) << 3)

#define DATA_UTIL_LIST_NODE_BIT_FIELD_NONE                   ((uint64_t) 0)
#define DATA_UTIL_LIST_NODE_BIT_FIELD_USE_SYNC_VAR           (((uint64_t) 1) << 0)
#define DATA_UTIL_LIST_NODE_BIT_FIELD_IS_USERDATA_MODIFIED   (((uint64_t) 1) << 1)

typedef struct data_util_list_node_data_type
{
    data_util_sync_data_type node_sync_data;
    data_util_bit_field_type node_bit_field;
    void *user_data;
}data_util_list_node_data_type;

typedef int (*add_evaluator_type)(data_util_list_node_data_type *to_be_added_data,
                                  data_util_list_node_data_type *to_be_evaluated_data);

typedef void (*delete_evaluator_type)(data_util_list_node_data_type *to_be_deleted_data);

typedef int (*find_evaluator_type)(data_util_list_node_data_type *to_be_found_data);

typedef int (*enumerate_evaluator_type)(data_util_list_node_data_type *to_be_evaluated_data);

typedef struct data_util_list_node_type
{
    data_util_list_node_data_type    node_data;
    struct data_util_list_node_type *prev;
    struct data_util_list_node_type *next;
}data_util_list_node_type;

typedef struct data_util_list_info_type
{
    char                  config_file_name[128];
    data_util_sync_data_type   list_sync_data;
    data_util_list_node_type  *list_head;
    data_util_list_node_type  *list_tail;
    add_evaluator_type    default_add_evaluator;
    delete_evaluator_type default_delete_evaluator;
    data_util_bit_field_type   list_bit_field;
    unsigned int          num_of_node;
}data_util_list_info_type;


data_util_list_node_data_type* data_util_list_find_data_in_list_with_param(
    data_util_list_info_type *list_info,
    int (*find_evaluator)(const data_util_list_node_data_type *to_be_found_data, void *compare_data),
    void* compare_data
);



/***************************************************************************************************
    @function
    data_util_list_create

    @brief
    Creates a list.

    @param[in]
        list_info
           pointer to the info object that is used to hold the meta data of the list
           can be NULL, if the list's meta data is to be stored on heap
        default_add_evaluator
           pointer to function that is used by default to evaluate at which position of the list a
           newly created node needs to be inserted
           need to return TRUE (or FALSE) when the to be added node needs to be inserted
           before (or not) the currently compared node present in the list
        default_delete_evaluator
           pointer to function that is used by default to free the memory of the user data stored
           in a node when it is being removed from the list
        list_bit_field
           list attributes which need to be considered while creating the list
           Supported attributes:
               UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP
                    list's meta data is to be stored on the heap
               UTIL_LIST_BIT_FIELD_SORTED
                    list would be maintained in a sorted order by the user
               UTIL_LIST_BIT_FIELD_AUTO_LOCK
                    locks the list on add/delete/find node operation automatically
               UTIL_LIST_BIT_FIELD_USE_COND_VAR
                    creates conditional variable on which a thread adding a node to the list can
                    wait on until the node has been processed

    @param[out]
        none

    @retval
    pointer to the info object that holds the meta data of the list If list has been created
    successfully, NULL otherwise
***************************************************************************************************/
data_util_list_info_type* data_util_list_create(data_util_list_info_type *list_info,
                                      add_evaluator_type default_add_evaluator,
                                      delete_evaluator_type default_delete_evaluator,
                                      data_util_bit_field_type list_bit_field);




/***************************************************************************************************
    @function
    data_util_list_cleanup

    @brief
    Cleansup a created list after removing and freeing the nodes.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list
        delete_evaluator
           pointer to function that is used to free the memory of the user data stored
           in a node when it is being removed from the list
           can be NULL if a default delete evaluator was provided while creating the list

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void data_util_list_cleanup(data_util_list_info_type *list_info,
                       delete_evaluator_type delete_evaluator);



/***************************************************************************************************
    @function
    data_util_list_add

    @brief
    Adds user data to the list by creating a new node.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list
        to_be_added_data
           pointer to the user data that needs to be added to the list
        add_evaluator
           pointer to function that is used by to evaluate at which position of the list a
           newly created node needs to be inserted
           need to return TRUE (or FALSE) when the to be added node needs to be inserted
           before (or not) the currently compared node present in the list
           can be NULL if a default add evaluator was provided while creating the list
        node_bit_field
           node attributes which need to be considered while creating the node
           Supported attributes:
               UTIL_LIST_NODE_BIT_FIELD_USE_SYNC_VAR
                    creates synchronization object using which a thread adding a node to the list
                    can wait on until the node has been processed
               UTIL_LIST_NODE_BIT_FIELD_IS_USERDATA_MODIFIED
                    indicates if the user data of the node has been modified since the last
                    enumeration

    @param[out]
        none

    @retval
    ESUCCESS If the user data has been added successfully to the list, appropriate error code
    otherwise
***************************************************************************************************/
int data_util_list_add(data_util_list_info_type *list_info,
                  void *to_be_added_data,
                  add_evaluator_type add_evaluator,
                  data_util_bit_field_type node_bit_field);




/***************************************************************************************************
    @function
    data_util_list_lock_list

    @brief
    Locks the list.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list

    @param[out]
        none

    @retval
    ESUCCESS If the list has been locked successfully, appropriate error code otherwise
***************************************************************************************************/
int data_util_list_lock_list(data_util_list_info_type *list_info);






/***************************************************************************************************
    @function
    data_util_list_unlock_list

    @brief
    Unlocks the list.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list

    @param[out]
        none

    @retval
    ESUCCESS If the list has been unlocked successfully, appropriate error code otherwise
***************************************************************************************************/
int data_util_list_unlock_list(data_util_list_info_type *list_info);



/***************************************************************************************************
    @function
    data_util_list_is_bits_set_in_list_bit_field

    @brief
    Checks if the list contains specific attributes.

    @param[in]
        list_info
           pointer to the info object that holds the meta data of the list
        bits_to_be_checked
           to be checked attributes
        is_partial_match_accepted
           TRUE If a subset of contained attributes is good enough for a successful match,
           FALSE otherwise

    @param[out]
        none

    @retval
    TRUE If the match has been successful, FALSE otherwise
***************************************************************************************************/
int data_util_list_is_bits_set_in_list_bit_field(data_util_list_info_type *list_info,
                                            data_util_bit_field_type bits_to_be_checked,
                                            int is_partial_match_accepted);





/***************************************************************************************************
    @function
    data_util_list_lock_node

    @brief
    Locks the node.

    @param[in]
        node_data
           pointer to the node data object that needs to be locked

    @param[out]
        none

    @retval
    ESUCCESS If the node has been locked successfully, appropriate error code otherwise
***************************************************************************************************/
int data_util_list_lock_node(data_util_list_node_data_type *node_data);





/***************************************************************************************************
    @function
    data_util_list_unlock_node

    @brief
    Unlocks the node.

    @param[in]
        node_data
           pointer to the node data object that needs to be unlocked

    @param[out]
        none

    @retval
    ESUCCESS If the node has been unlocked successfully, appropriate error code otherwise
***************************************************************************************************/
int data_util_list_unlock_node(data_util_list_node_data_type *node_data);





/***************************************************************************************************
    @function
    data_util_list_wait_on_node

    @brief
    Waits on the node.

    @param[in]
        node_data
           pointer to the node data object that needs to be used for waiting
        wait_for_time_seconds
            time (in seconds) to wait on the conditional variable, NIL If wait needs
            to indefinite

    @param[out]
        none

    @retval
    ESUCCESS If wait been performed and the thread has been wokenup,
    appropriate error code otherwise
***************************************************************************************************/
int data_util_list_wait_on_node(data_util_list_node_data_type *node_data,
                           int wait_for_time_seconds);

#endif
