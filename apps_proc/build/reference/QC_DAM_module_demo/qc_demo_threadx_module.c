/*=========================================================================
                        qc_demo_threadx_module.c 

GENERAL DESCRIPTION
 
This is a demo of creating applications as a module.(DAM)
- It demonstrates 2 pairs of running threads.
	1) One pair of Event get/set threads.
	2) One pair of Mutex lock/unlock threads.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2016-17  by Qualcomm Technologies, Inc.  All Rights Reserved.
 =========================================================================*/
/* Specify that this is a module!  */

//#define TXM_MODULE


/* ThreadX module definitions.  */
#include "txm_module.h"

//typedef unsigned int ULONG;
//typedef unsigned int UINT;

#define DEMO_THREAD_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     	   1024*10   /*10 KB */



/* Define the pool buffer   */
ULONG                   demo_module_pool_buffer[DEMO_BYTE_POOL_SIZE / 4];

/* Define the ThreadX TCBs  */
TX_THREAD               *thread_evtSet;
TX_THREAD               *thread_evtWait;
TX_THREAD               *thread_mutexDemo1;
TX_THREAD               *thread_mutexDemo2;

/* Define other threadx object control blocks used for demo */
TX_MUTEX                *my_mutex;
TX_EVENT_FLAGS_GROUP    *my_event_flag;
TX_BYTE_POOL            *my_byte_pool;

/*------------- Thread counters ----------------*/
UINT thread_evtSet_counter=0;
UINT thread_evtWait_counter;  //=0; as global data initialized to 0
UINT thread_mutexDemo1_counter=4;	//just to see test global variable's initialization
UINT thread_mutexDemo2_counter=4;



/* Define thread's entry prototypes.  */

void    thread_evtSet_Entry(UINT paramInput);
void    thread_evtWait_Entry(UINT paramInput);
void	thread_mutexTest_Entry(UINT paramInput);


/* Define the module start/entry function.  */

void    demo_module_start(ULONG id)
{
  
    CHAR    *pointer=NULL;

    /* allocate threadx objects from module manager object pool */
	/* allocate thread/TCB objects */
    txm_module_object_allocate(&thread_evtSet, sizeof(TX_THREAD));
    txm_module_object_allocate(&thread_evtWait, sizeof(TX_THREAD));
    txm_module_object_allocate(&thread_mutexDemo1, sizeof(TX_THREAD));
    txm_module_object_allocate(&thread_mutexDemo2, sizeof(TX_THREAD));

	/* allocate signal and mutex required for demo threads*/
    txm_module_object_allocate(&my_mutex, sizeof(TX_MUTEX));
    txm_module_object_allocate(&my_event_flag, sizeof(TX_EVENT_FLAGS_GROUP));
    txm_module_object_allocate(&my_byte_pool, sizeof(TX_BYTE_POOL));


    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(my_byte_pool, "demo byte pool", demo_module_pool_buffer, DEMO_BYTE_POOL_SIZE);

    /*---block pool alocation and free example.---*/
    tx_byte_allocate(my_byte_pool, (VOID **) &pointer, 1024/*1 KB*/, TX_NO_WAIT);
	if(pointer == NULL) return;
    tx_block_release(pointer);
	//------------------------------------------- */



	/* ---------------  Thread's event set/wait demo ---------------- */

    /* Create the event flags group used by threads evtSet and evtWait.  */
    tx_event_flags_create(my_event_flag, "demo event flags");	
    tx_mutex_create(my_mutex, "demo mutex", TX_NO_INHERIT);


	/* Allocate the stack for thread evtSet.  */
    tx_byte_allocate(my_byte_pool, (VOID **) &pointer, DEMO_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(thread_evtSet, "Thread_EvtSet"/*thread name*/, thread_evtSet_Entry/*thread entry function*/, 0/*any input parameter passed to thread*/,  
            pointer/*stack start*/, DEMO_THREAD_STACK_SIZE/*stack size*/, 
            150/*priority*/, 150/*preempt priority threshold*/, TX_NO_TIME_SLICE, TX_AUTO_START/*start scheduling thread immediately after creation*/ );

	/* Allocate the stack for thread evtWait.  */
	  tx_byte_allocate(my_byte_pool, (VOID **) &pointer, DEMO_THREAD_STACK_SIZE, TX_NO_WAIT);
	  tx_thread_create(thread_evtWait, "Thread_EvtWait", thread_evtWait_Entry, 1,	
			  pointer, DEMO_THREAD_STACK_SIZE, 
			  151, 151, TX_NO_TIME_SLICE, TX_AUTO_START);
	/* ----------------------------------------------------------------- */

	

	/* ---------------	Thread's mutex lock/unlock demo ---------------- */

    /* Allocate the stack for thread_mutexDemo1.  */
    tx_byte_allocate(my_byte_pool, (VOID **) &pointer, DEMO_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(thread_mutexDemo1, "Thread_MutexDemo1", thread_mutexTest_Entry, 1,  
            pointer, DEMO_THREAD_STACK_SIZE, 
            152, 152, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread_mutexDemo2.  */
    tx_byte_allocate(my_byte_pool, (VOID **) &pointer, DEMO_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(thread_mutexDemo2, "Thread_MutexDemo2", thread_mutexTest_Entry,2,  
            pointer, DEMO_THREAD_STACK_SIZE, 
            152, 152, TX_NO_TIME_SLICE, TX_AUTO_START);
	/* ----------------------------------------------------------------- */

}



void    thread_evtSet_Entry(UINT paramInput)
{
    UINT        ret;
    while(1)
    {
        thread_evtSet_counter++;

        /* Set my_event_flag to wakeup thread_evtWait waiting on that.  */
        ret =  tx_event_flags_set(my_event_flag, 0x1, TX_OR);
        if (ret != TX_SUCCESS)
            break;
          
        /* Sleep for 1sec or 100 ticks.  */
        tx_thread_sleep(100);
    }
}


void    thread_evtWait_Entry(UINT paramInput)
{

UINT    ret;
ULONG   actual_flags = 0;

    while(1)
    {
        thread_evtWait_counter++;

        /* Wait for event flag  */
        ret =  tx_event_flags_get(my_event_flag, 0x1, TX_OR_CLEAR, /*OR_CLEAR option will clear the flags after reading */ 
                                                &actual_flags, TX_WAIT_FOREVER);

        if ((ret != TX_SUCCESS) || (actual_flags != 0x1))
            break;
    }

}


void    thread_mutexTest_Entry(UINT paramInput)
{

UINT    ret;
    while(1)
    {
        if (paramInput == 1)	/* this paramInput is 4th argument specified during thread creation */
            thread_mutexDemo1_counter++;
        else
            thread_mutexDemo2_counter++;
		
		/* get/lock mutex*/
        ret =  tx_mutex_get(my_mutex, TX_WAIT_FOREVER);
        if (ret != TX_SUCCESS)
            break;

        /* Sleep for 10 ticks to let other thread run till tx_mutex_get  */
        tx_thread_sleep(10);

        /* put/unlock mutex to wake up other thread  */
        ret =  tx_mutex_put(my_mutex);
		
        if (ret != TX_SUCCESS)
            break;
    }
}


