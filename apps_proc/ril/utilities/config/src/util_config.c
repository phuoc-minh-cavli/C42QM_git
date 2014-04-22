/***************************************************************************************************
    @file
    util_config.c

    @brief
    Facilitates reading configuration file.

    Copyright (c) 2015 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.

***************************************************************************************************/

#include "utils_standard.h"
#include "utils_common.h"

typedef struct keyvalue
{
    char   key[128];
    char   value[128];
}keyvalue;

/***************************************************************************************************
    @function
    util_timer_add_evaluator

    @implementation detail
    None.
***************************************************************************************************/
int util_config_add_evaluator(util_list_node_data_type *to_be_added_data,
                                         util_list_node_data_type *to_be_evaluated_data)
{
    return TRUE;
}






/***************************************************************************************************
    @function
    util_timer_delete_evaluator

    @implementation detail
    None.
***************************************************************************************************/
void util_config_delete_evaluator(util_list_node_data_type *to_be_deleted_data)
{
    if(to_be_deleted_data && to_be_deleted_data->user_data)
    {
        util_memory_free(&to_be_deleted_data->user_data);
    }
}





/***************************************************************************************************
    @function
    getline

    @implementation detail
    None.
***************************************************************************************************/
#ifndef getline
int getline(char **lineptr, size_t *n, int stream) 
{
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    char c;
	int len=0;
    int previous_size = 0;

    if (lineptr == NULL || stream < NIL || n == NULL ) 
	{
    	return -1;
    }

    bufptr = *lineptr;
    size = *n;

    len = efs_read(stream, &c, 1);
    if ( len == 0) {
    	return -1;
    }
    if (bufptr == NULL) 
	{
    	bufptr = malloc(128);
    	if (bufptr == NULL) 
		{
    		return -1;
    	}
    	size = 128;
    }
    p = bufptr;
    while(len != 0) 
	{
    	if ((p - bufptr) > (size - 1)) 
		{
		    previous_size = size;
    		size = size + 128;
    		bufptr = realloc(bufptr, size);
    		if (bufptr == NULL) {
    			return -1;
    		}
            p = bufptr + previous_size;
    	}
		if ( c != '\r' )
		{
	    	*p++ = c;
		}
    	if (c == '\n') 
		{
    		break;
    	}
    	len = efs_read(stream, &c, 1);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif //getline





/***************************************************************************************************
    @function
    util_config_load

    @brief
    Read configuration file.

    @param[in]

    @param[out]
        none

    @retval
    Pointer to config object if loading config file is successful, NULL otherwise.
***************************************************************************************************/
void* util_config_load(const char *file)
{
    int                         fp = INVALID_FILE_HANDLE;
    struct keyvalue            *entry = NULL;
    char                       *line = NULL;
    char                       *saveptr = NULL;
    char                       *tmpptr = NULL;
    size_t                      line_len = 0;
    util_list_info_type        *config_list = NULL;

    do
    {

        if ( file )
        {
            fp = efs_open(file, O_RDONLY);
            if ( fp == INVALID_FILE_HANDLE )
            {
                break;
            }

            config_list = util_list_create(NULL,
                                           util_config_add_evaluator,
                                           util_config_delete_evaluator,
                                           UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP);

            if ( config_list != NULL )
            {
                strlcpy(config_list->config_file_name,
                        file,
                        sizeof(config_list->config_file_name));
                while ( getline(&line, &line_len, fp) != -1 )
                {
                    entry = (struct keyvalue*) util_memory_alloc(sizeof(*entry));
                    tmpptr = strtok_r(line, "=", &saveptr);
                    if (tmpptr)
                    {
                        strlcpy(entry->key, tmpptr, sizeof(entry->key));
                    }
                    tmpptr = strtok_r(NULL, "\n", &saveptr);
                    if (tmpptr )
                    {
                        strlcpy(entry->value, tmpptr, sizeof(entry->value));
                    }
                    util_list_add(config_list, entry, NULL, 0);
                }
            }

        }

        free(line);
        efs_close(fp);

    }while(FALSE);

    return config_list;
}






/***************************************************************************************************
    @function
    util_memory_free

    @brief
    Frees a block of memory and sets the pointer to the memory to NULL.

    @param[in]
    @param[out]
        to_be_freed_memory_ptr
            pointer to the pointer to the memory to be freed, pointer to memory is set to NULL

    @retval
    none
***************************************************************************************************/
void util_config_unload(void *config_list)
{
    util_list_cleanup(config_list, NULL);
}






/***************************************************************************************************
    @function
    util_config_find_evaluator

    @brief
	none

    @param[in]
    @param[out]
	none

    @retval
    none
***************************************************************************************************/
int util_config_find_evaluator(const util_list_node_data_type *node_data, void *compare_data)
{
        int              ret = FALSE;
        struct keyvalue *user_data;

        if ( node_data && compare_data && node_data->user_data )
        {
               user_data = node_data->user_data;
               if ( !strcmp(user_data->key, compare_data) )
               {
                   ret = TRUE;
               }
        }
        return ret;
}






/***************************************************************************************************
    @function
    util_config_get_value

    @implementation detail
    None.
***************************************************************************************************/
int util_config_get_value(void *config_list,
                          char *key,
                          int   type,
                          void *value)
{
        int                       ret = FALSE;
        util_list_node_data_type *data;
        struct keyvalue          *user_data;

        data = util_list_find_data_in_list_with_param(config_list,
                     util_config_find_evaluator,key);

        if ( data != NULL )
        {
                user_data = data->user_data;
                switch(type)
                {
                        case INTEGER:
                                *((int*)value) = atoi(user_data->value);
                                ret = TRUE;
                                break;

                        case STRING:
                                strlcpy(value,
                                        user_data->value,
                                        sizeof(user_data->value));
                                ret = TRUE;
                                break;

                        default:
                                break;
                }
        }

        return ret;
}





/***************************************************************************************************
    @function
    util_config_set_value

    @implementation detail
    None.
***************************************************************************************************/
/*void util_config_set_value(void *config_list,
                                    char *key,
                                    int   type,
                                    void *value)
{
        util_list_node_data_type *data;
        struct keyvalue          *user_data;
        struct keyvalue          *new_entry = NULL;

        data = util_list_find_data_in_list_with_param(config_list,
                     util_config_find_evaluator,key);

        if ( data != NULL )
        {
                user_data = data->user_data;
                switch(type)
                {
                        case INTEGER:
                            snprintf(user_data->value,
                                     sizeof(user_data->value),
                                     "%d",
                                     (char *)value);
                                break;

                        case STRING:
                            snprintf(user_data->value,
                                     sizeof(user_data->value),
                                     "%s",
                                     (char *)value);
                            break;

                        default:
                                break;
                }
        }
        else
        {
            // new config item. Add entry in list.
            new_entry = (struct keyvalue*) util_memory_alloc(sizeof(*new_entry));
            if ( new_entry != NULL )
            {
                strlcpy(new_entry->key, key, sizeof(new_entry->key));
                switch(type)
                {
                        case INTEGER:
                            snprintf(new_entry->value,
                                     sizeof(user_data->value),
                                     "%d",
                                     (char *)value);
                                break;

                        case STRING:
                            snprintf(new_entry->value,
                                     sizeof(user_data->value),
                                     "%s",
                                     (char *)value);
                                break;

                        default:
                                break;
                }
                util_list_add(config_list, new_entry, NULL, 0);
            }
        }

        return;
}*/



