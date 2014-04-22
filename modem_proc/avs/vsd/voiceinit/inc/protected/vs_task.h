#ifndef __VS_TASK_H__
#define __VS_TASK_H__

/*
  ============================================================================

   Copyright (C) 2016-2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/voiceinit/inc/protected/vs_task.h#1 $
  $DateTime: 2019/07/09 12:00:19 $
  $Author: pwbldsvc $
*/

#define TASK_PRIORITY( task_pri_order ) ( 255 - task_pri_order )

/* VS Task which handles command and data packets*/
#define VS_THREAD_TASK_NAME ( "VS_HIGH" )
#define VS_THREAD_STACK_SIZE ( 4 * 1024 )

/* VS PDR Task  This has lower priority than VS_HIGH task*/
#define VS_PDR_TASK_NAME ( "VS_PDR" )
#define VS_PDR_TASK_STACK_SIZE ( 2 * 1024 )

/* IVA Task */
#define IVA_TASK_NAME ( "IVA" )
#define IVA_TASK_STACK_SIZE ( 2 * 1024 )

/* GVA Task */
#define GVA_TASK_NAME ( "GVA" )
#define GVA_TASK_STACK_SIZE ( 2 * 1024 )

/* WVA Task */
#define WVA_TASK_NAME ( "WVA" )
#define WVA_TASK_STACK_SIZE ( 2 * 1024 )

/* TVA Task */
#define TVA_TASK_NAME ( "TVA" )
#define TVA_TASK_STACK_SIZE ( 2 * 1024 )

/* CVA Task */
#define CVA_TASK_NAME ( "CVA" )
#define CVA_TASK_STACK_SIZE ( 2 * 1024 )

/* VAGENT Task */
#define VAGENT_TASK_NAME ( "VAGENT" )
#define VAGENT_TASK_STACK_SIZE ( 2 * 1024 )


#ifdef AVS_MPSS_JO

/* AVS thread priorities specific to JO images. */
#define VS_THREAD_TASK_PRIORITY ( 171 )
#define VS_PDR_TASK_PRIORITY ( 167 )

#define GVA_TASK_PRIORITY ( 171 )
#define WVA_TASK_PRIORITY ( 171 )
#define TVA_TASK_PRIORITY ( 171 )
#define CVA_TASK_PRIORITY ( 171 )
#define IVA_TASK_PRIORITY ( 171 )

#define VAGENT_TASK_PRIORITY ( 171 )

#else

/* AVS thread priorities specific to Thor/Tabasco and all future chipsets. */
#define VS_THREAD_TASK_PRIORITY ( 181 )
#define VS_PDR_TASK_PRIORITY ( 136 )

#define GVA_TASK_PRIORITY ( 181 )
#define WVA_TASK_PRIORITY ( 181 )
#define TVA_TASK_PRIORITY ( 181 )
#define CVA_TASK_PRIORITY ( 181 )
#define IVA_TASK_PRIORITY ( 181 )

#define VAGENT_TASK_PRIORITY ( 181 )

#endif /* AVS_MPSS_JO */

#endif /* __VS_TASK_H__ */

