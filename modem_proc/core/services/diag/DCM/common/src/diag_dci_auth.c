#ifdef DIAG_CONSUMER_API
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task 

General Description
  This file is created to provide an abstraction layer between Diag and the
  clients who register with us to provide DCI Security. Additionally, we
  register with SSM to get notified about DCI status change. 

Copyright (c) 2012-2013,2016,2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.10/services/diag/DCM/common/src/diag_dci_auth.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
17/10/16   nk      NV item for DCI kill switch
02/03/16   nk      Moved DIAG_OPEN_DCI_CHANNEL_SIG to external signals and removed 
                   DIAG_TASK_OFFLINE_SIG signal
04/09/13   sr      Fixed the compiler warnings
11/05/12   sr      File created - Support for DCI Override feature(OCH) and 
                   On Board Diagnostics(OBD)- Secure Sim feature     
      
===========================================================================*/      
#include "comdef.h"
#include <string.h>
#include "diag_dci_auth_v.h"
#include "diag_dci_auth.h"
#include "ssm_och.h"
#include "osal.h"
#include "diagi_v.h"
#include "ULog_Diag.h"

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
/*!
@ingroup diag_security
@brief
   This function initializes the DCI authentication client registration table.

@dependencies
None
@sideeffects
None
@return
 TRUE if the initialization was successful, else FALSE.
*/

static boolean diag_dci_auth_client_tbl_init(void); 

ssm_och_err_t diag_och_ssm_init_val = E_SSM_OCH_FAILED;
static int diag_dci_auth_client_tbl_init_begin = FALSE;
static int diag_dci_auth_client_tbl_initialized = FALSE;
static osal_mutex_arg_t diag_dci_auth_client_tbl_reg_lock;
static diag_dci_cb_type diag_dci_auth_client_cb_tbl[DIAG_DCI_AUTH_LAST_CLIENT_ID+1];


/*===========================================================================

FUNCTION DIAG_DCI_OPEN_QUERY

DESCRIPTION
  This function is called by diag task to query if the dci port should be
  opened or not.This function is also called when either ssm or one of the
  registered clients notifies diag to close the DCI port. This is an internal
  function and is always called in diag's context.
  If the NV item is found and the if its value is set to 1 then DCI is disabled.
  Otherwise, if NV item is not set to 1 or does not exist, 
  then determine DCI state based on remaining logic (OCH, etc.)
RETURN VALUE 
  If the return value is TRUE, then DCI port is opened. Else, DCI port is
  not opened. 

===========================================================================*/	
boolean diag_dci_open_query(void)
{
   /*By default, DCI is closed if SSM is defined*/
   boolean dci_open = FALSE;
   struct fs_stat temp_buf;
   int result; 
   if(efs_stat(DCI_DISABLE_NV_PATH, &temp_buf) == 0)    /* 0 -- file exists   (-1)-- file does not exist*/
   {
      uint8 dci_disable_nv=0; /* To store the EFS NV item contents */
      result = efs_get(DCI_DISABLE_NV_PATH, &dci_disable_nv,sizeof(dci_disable_nv)); /* Returns the number of bytes copied in the buffer */
      if((result == sizeof(dci_disable_nv)) && (dci_disable_nv == DISABLE_DCI))
      {
         /* Disable DCI */
		 dci_open = FALSE;
	     return dci_open;
      }
	  else if(result != sizeof(dci_disable_nv))
	  {
	     ULogFront_RealTimePrintf(diag_debug_ulog_handle, 2, "Error returned by efs_get(). efs_errno = %d result=%d ",efs_errno,result);
      }
   }
   uint32 status;
   int i =0;
	/*Query ssm*/
  ssm_och_ovrr_status_t och_status = ssm_och_get_override_status();

  if (och_status == SSM_OCH_OVERRIDE_SUCCESSFUL)
  {
      dci_open = TRUE;
	  return dci_open;
  } 

  /*Query all the registered clients*/

  for (i = 0; i<= DIAG_DCI_AUTH_LAST_CLIENT_ID; i++)
  {
	  if (diag_dci_auth_client_cb_tbl[i] != NULL)
	  {
/*If there are more clients who register with us to notify DCI status, 
  then different case should be handled in this switch statement*/
	  switch(i)
	  {
		  case DIAG_DCI_AUTH_CLIENT_UIM:

		        status = (diag_obd_auth_status_t) diag_dci_auth_client_cb_tbl[i]();

				if (status == DIAG_OBD_AUTHENTICATION_SUCCESSFUL)
				{
					dci_open = TRUE;
					return dci_open;
				}				
	  }
	  }

  }
return dci_open;
}

/*=========================================================================== 
 
FUNCTION DIAG_OCH_SSM_GET_ERROR

DESCRIPTION
 Return SSM DCI Override init error code.

===========================================================================*/	
ssm_och_err_t diag_och_ssm_get_error( void )
{
  return diag_och_ssm_init_val;  
} /* diag_ssm_get_error */

/*===========================================================================

FUNCTION DIAG_OCH_SSM_INIT

DESCRIPTION
  This function is called by diag task to register callback with ssm. The
  callback is called by ssm to notify diag about change in DCI status.

===========================================================================*/	
void diag_och_ssm_init(void)
{

 diag_och_ssm_init_val = ssm_och_register_cb ((ssm_och_register_cb_type) diag_och_ssm_cb);

 if (diag_och_ssm_init_val != E_SSM_OCH_SUCCESS)
 {
   MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Diag DCI Override Feature Initialization Failed %d", diag_och_ssm_get_error);  
 }
}

/*===========================================================================

FUNCTION DIAG_OCH_SSM_CB

DESCRIPTION
  This function is the callback called by ssm when there is a change in
  DCI status.

===========================================================================*/	
void diag_och_ssm_cb (ssm_och_ovrr_status_t och_stat)
{

  if (och_stat == SSM_OCH_OVERRIDE_SUCCESSFUL)
		{
	        osal_sigs_t return_sigs;
			int return_val=0;
			MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_LOW, "Request from OCH to open the DCI channel %d", och_stat);	
            return_val = osal_set_sigs(diag_task_tcb, DIAG_OPEN_DCI_CHANNEL_SIG, &return_sigs);
            ASSERT(OSAL_SUCCESS == return_val);
		}
  else if(och_stat == SSM_OCH_OVERRIDE_NOT_SUCCESSFUL)
        {
	        MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_LOW, "Request from OCH to close the DCI channel %d", och_stat);	
		    diag_set_internal_sigs(DIAG_INT_CLOSE_DCI_CHANNEL_SIG);  
        }
		
}

/*===========================================================================

FUNCTION DIAG_DCI_AUTH_CLIENT_TBL_INIT

DESCRIPTION
  This function initializes the DCI authentication client registration table.

===========================================================================*/
static boolean diag_dci_auth_client_tbl_init(void)
{
	int return_val_mutex;
	int i = 0;

	/*To ensure that the initialization happens only once*/
 if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_dci_auth_client_tbl_init_begin, FALSE, TRUE))
    return FALSE;

  /*Initialize the table*/
 for (i=0;i<= DIAG_DCI_AUTH_LAST_CLIENT_ID;i++)
 {
	 diag_dci_auth_client_cb_tbl[i] = NULL;	 
 }

  /*initialize the mutex used during registration of client table*/
  diag_dci_auth_client_tbl_reg_lock.name = "MUTEX_DIAG_DCI_AUTH_CLIENT_TBL_REG";
	return_val_mutex = osal_init_mutex(&diag_dci_auth_client_tbl_reg_lock);
    ASSERT(OSAL_SUCCESS == return_val_mutex);

  diag_dci_auth_client_tbl_initialized = TRUE;
    return TRUE;
}

/*===========================================================================

FUNCTION DIAG_DCI_AUTH_REG_CLIENT

DESCRIPTION
  This is the function that Diag provides to register clients so that they can
  notify Diag about change in DCI status

===========================================================================*/
diag_dci_auth_reg_err_t diag_dci_auth_reg_client(diag_dci_auth_client_id_type client_id, diag_dci_cb_type cb)
{
  if(diag_dci_auth_client_tbl_initialized == FALSE )
  {
    /* Make sure this is initialized */
    diag_dci_auth_client_tbl_init ();

      if( diag_dci_auth_client_tbl_initialized == FALSE )
      {
        return DIAG_DCI_REG_FAILED;
      }   
  }
	/* If callback is NULL or if the client_id is wrong or if the client already registered a callback return failure */
  if (( cb == NULL ) || ( client_id > DIAG_DCI_AUTH_LAST_CLIENT_ID )
	  ||(diag_dci_auth_client_cb_tbl[client_id] != NULL))
    return DIAG_DCI_REG_FAILED;


 /*Lock the mutex before registering to prevent race condition*/
  osal_lock_mutex(&diag_dci_auth_client_tbl_reg_lock);

   diag_dci_auth_client_cb_tbl[client_id] = cb;

  /*Unlock the mutex*/
	osal_unlock_mutex(&diag_dci_auth_client_tbl_reg_lock);
    return DIAG_DCI_REG_SUCCESS;
}


/*===========================================================================

FUNCTION DIAG_DCI_AUTH_NOTIFY

DESCRIPTION
  This is the function that clients call to notify Diag about change in DCI
  status.

===========================================================================*/
void diag_dci_auth_notify (diag_dci_auth_client_id_type client_id, uint32 status)
{
	switch (client_id)
	{
	   case DIAG_DCI_AUTH_CLIENT_UIM:
		   if (status == DIAG_OBD_AUTHENTICATION_SUCCESSFUL)
		   {
               osal_sigs_t return_sigs;
			   int return_val=0;
			   MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_LOW, "Request from OBD to open the DCI channel %d", status);
               return_val = osal_set_sigs(diag_task_tcb, DIAG_OPEN_DCI_CHANNEL_SIG, &return_sigs);
               ASSERT(OSAL_SUCCESS == return_val);  
		   }	
		   else
		   {
			   MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_LOW, "Request from OBD to close the DCI channel %d", status);
			   diag_set_internal_sigs(DIAG_INT_CLOSE_DCI_CHANNEL_SIG);
		   }
		   
	}

}

/*============================================================================

FUNCTION DCI_EFS_CONF_FILE_UPDATE

DESCRIPTION
  Update the conf file with nv file location.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void dci_efs_conf_file_update(void)
{

  /* File Descriptor */
   int32  fd;
   struct fs_stat temp_buf;
   char str[] = DCI_DISABLE_NV_PATH "\n";

  /* If the conf file does not exist create a new conf file */
  if ( efs_stat(DCI_DISABLE_NV_CONFIG_FILE_PATH, &temp_buf)!= 0 )    /* 0 -- file exists   (-1)-- file does not exist*/
  {

    /* Open the DCI_DISABLE_NV CONF EFS file. If its not present then create it. */
    fd = efs_open( DCI_DISABLE_NV_CONFIG_FILE_PATH, O_CREAT|O_AUTODIR|O_WRONLY|O_TRUNC);
    if (fd < 0)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_FATAL,"Error opening EFS file %d", fd);
      return;
    }

    efs_write( fd, str, (size_t)strlen(str) );

    /* Close the file */
    efs_close( fd );
  }

}
#endif /* DIAG_CONSUMER_API */

