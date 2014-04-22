/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <kernel.h>
/*======================================================================
                         CONSTANTS AND MACROS
 ======================================================================*/

#define MAX_LIBC_MUTEX 64   //Subrato dinkumware libc requirement

/*======================================================================
                         STRUCT
 ======================================================================*/

typedef struct OKL4MutexObj
{
    okl4_mutex_t OKL4Mutex;
    int used;
}OKL4MutexObj;


static OKL4MutexObj libcMutex[MAX_LIBC_MUTEX] = {0};

static okl4_mutex_t mutex_lock = {0};


/*======================================================================
                         FUNCTIONS
 ======================================================================*/

void sys_Mtxinit(void **mutex)
{
    int i;
    okl4_mutex_attr_t attr;
    
    _okl4_mutex_lock( &mutex_lock );
    for (i = 0; i < MAX_LIBC_MUTEX; i++)
        if (libcMutex[i].used == 0) //available mutex
        {
            _okl4_mutex_attr_init(&attr);
            if(OKL4_OK == okl4_mutex_create(&(libcMutex[i].OKL4Mutex), &attr))
            {   libcMutex[i].used = 1;
                *mutex = (void *)&libcMutex[i];
                _okl4_mutex_unlock( &mutex_lock );
                return;
            }
        }
    _okl4_mutex_unlock( &mutex_lock );
    assert(0);
}

void sys_Mtxdst(void **mutex)
{
    OKL4MutexObj *ptr_mutex = (OKL4MutexObj *)*mutex;
    _okl4_mutex_lock( &mutex_lock );
    _okl4_mutex_delete(&ptr_mutex->OKL4Mutex);
    ptr_mutex->used = 0;
    _okl4_mutex_unlock( &mutex_lock );
}

void sys_Mtxlock(void **mutex)
{
    OKL4MutexObj *ptr_mutex = (OKL4MutexObj *)*mutex;
    _okl4_mutex_lock(&ptr_mutex->OKL4Mutex);    
}

void sys_Mtxunlock(void **mutex)
{
    OKL4MutexObj *ptr_mutex = (OKL4MutexObj *)*mutex;
    _okl4_mutex_unlock(&ptr_mutex->OKL4Mutex);

}
