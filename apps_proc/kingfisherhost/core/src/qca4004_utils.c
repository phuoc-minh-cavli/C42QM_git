/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qca4004_utils.h"


/*-------------------------------------------------------------------------
 * Public Function Definitions
 *-----------------------------------------------------------------------*/

qurt_timer_t start_qca4004_timer(uint32_t timeout, qurt_timer_callback_func_t cbfunc, void *cbdata)
{
	qurt_timer_t timer;
	qurt_timer_attr_t attr;
	/* unit ms */
	qurt_time_t duration = timeout*QURT_TIMER_NEXT_EXPIRY/10;
	//qurt_time_t reloadTime;
	uint32_t option = QURT_TIMER_ONESHOT | QURT_TIMER_AUTO_START;
	
	qurt_timer_attr_init(&attr);
	/* Set-up callback */
    qurt_timer_attr_set_callback(&attr, cbfunc, cbdata);
	/* Set-up the timer duration */
	qurt_timer_attr_set_duration(&attr, duration);
	//qurt_timer_attr_set_reload(&attr, reloadTime);
	qurt_timer_attr_set_option(&attr, option);

	/* Create the timer */
   	if (QURT_EOK != qurt_timer_create(&timer, &attr))
   	{
   		return (qurt_timer_t)0;
   	}
	qurt_timer_start(timer);
	return timer;
}

void stop_qca4004_timer(qurt_timer_t *timer)
{
	if(*timer != NULL)
	{
    	// stop a running timer
    	qurt_timer_stop(*timer);
    	// Undef the timer. Releases memory allocated in qapi_Timer_Def()
    	qurt_timer_delete(*timer);
		*timer = NULL;
	}
}

void qca4004_uxtoa(uint32_t n, char s[])
{
	int i = 8,j = 0;
	while(i--)
	{
		j = n%16;
		n = n/16;
		if(j < 10)
		{
			s[i] = j+'0';
		}
		else
		{
			s[i] = j-10+'A';
		}
	}
	
}

uint32_t qca4004_atoux(char s[])
{
	uint32_t result = 0;
	int i = 0,j = 0;
	for(i=0;i<8;i++)
	{
		if(s[i] >= '0' && s[i] <= '9')
		{
			j = s[i] - '0';
		}
		else if(s[i] >= 'A' && s[i] <= 'F')
		{
			j = s[i] - 'A' + 10;
		}
		else if(s[i] >= 'a' && s[i] <= 'f')
		{
			j = s[i] - 'a' + 10;
		}
		result = result*16 + j;
	}
	return result;
}

int qca4004_strncmpi(char *str1, char *str2, int length)
{
    char temp_str1, temp_str2;
    if (length <= 0)
        return 0;

    while (--length >= 0) 
    {
        //convert both characters to uppercase
        temp_str1 = *str1;
        if((*str1 >= 0x61) && (*str1 <= 0x7a))
        {
            temp_str1 = *str1 - 0x20;
        }

        temp_str2 = *str2;
        if((*str2 >= 0x61) && (*str2 <= 0x7a))
        {
            temp_str2 = *str2 - 0x20;
        }

        if((temp_str1 != temp_str2) || (temp_str1 == '\0'))
        {
            break;
        }

        ++str1;
        ++str2;
    }

    return (temp_str1 - temp_str2);
}

uint32_t qca4004_swap32(uint32_t x)
{
    return (x>>24) |
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

