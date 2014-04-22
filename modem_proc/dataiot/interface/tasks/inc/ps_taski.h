#ifndef PS_TASKI_H
#define PS_TASKI_H
/*===========================================================================

                        P S _ T A S K I . H

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  all the definitions, functions, and structures for other software tasks
  to communicate with the Data Protocol Task.

EXTERNALIZED FUNCTIONS
  ps_enable_sig()
    Allow a signal to be processed

  ps_disable_sig()
    Disallow a signal to be processed

  ps_set_sig_handler()
    Set a handler to process a signal in PS context

  ps_send_cmd()
    Post a command to the Data Protocol Task

  ps_set_cmd_handler()
    Set a handler to process a cmd in PS context

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/dataiot.mpss/2.0/interface/tasks/inc/ps_taski.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/
/*--------------------------------------------------------------------------
NOTE: No Platform indepdent header inclusion...eg: rex.h
--------------------------------------------------------------------------*/
#include "comdef.h"
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "rex.h"
#include "ps_svc.h"
#include "queue.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Data Protocol Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
  PS Task Command queues and Command Data Buffers
---------------------------------------------------------------------------*/

typedef struct
{
  q_link_type link;		           /* For linking onto cmd queue             */
  ps_cmd_enum_type cmd;          /* Actual command to be processed         */
  void *user_data_ptr;           /* Command specific user parameters       */
} psi_cmd_type;


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/
typedef struct
{
  ps_sig_handler_type sig_handler;
  void *user_data_ptr;
} psi_sig_handler_info_type;


/*===========================================================================

                           MACROS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
      The SIG_IDX macro provides the index of the array for the signal sig_num.

      The SIG_OFFSET macro provides the offset for the signal sig_num within 
      the signal dword.
    ---------------------------------------------------------------------------*/
#define SIG_IDX(sig_num) ((uint32)(sig_num) / (8*sizeof(rex_sigs_type)))
#define SIG_OFFSET(sig_num) ((uint32)(sig_num) % (8*sizeof(rex_sigs_type)))
    
    /*--------------------------------------------------------------------------
      PS_CLR_SIG clears the signal sig_num within the mask pointed to by val_ptr
    ---------------------------------------------------------------------------*/
#define PS_CLR_SIG(val_ptr, sig_num)\
    { \
      *(((rex_sigs_type *)(val_ptr)) + SIG_IDX(sig_num)) &= \
        ~((rex_sigs_type) 1 << SIG_OFFSET(sig_num)); \
    }
    
    /*--------------------------------------------------------------------------
      PS_SET_SIG sets the signal sig_num in the mask pointed to by val_ptr
    ---------------------------------------------------------------------------*/
#define PS_SET_SIG(val_ptr, sig_num)\
    {\
      *(((rex_sigs_type *)(val_ptr)) + SIG_IDX(sig_num)) |=  \
        ((rex_sigs_type) 1 << SIG_OFFSET(sig_num)); \
    }
    
    /*--------------------------------------------------------------------------
      PS_IS_SET_SIG verifies if signal sig_num has been set in the mask pointed
      to by val_ptr
    ---------------------------------------------------------------------------*/
#define PS_IS_SET_SIG(val_ptr, sig_num)\
       (*(((rex_sigs_type *)(val_ptr)) + SIG_IDX(sig_num)) & \
           ((rex_sigs_type) 1 << SIG_OFFSET(sig_num)))

/*--------------------------------------------------------------------------
  PS_MAX_SIG_ARR_LEN indicates the array length corresponding to the maximum 
  number of PS signals required.
  For eg. if 
  0 <= PS_MAX_SIGNALS < 32, PS_MAX_SIG_ARR_LEN = 1
  32 <= PS_MAX_SIGNALS < 64, PS_MAX_SIG_ARR_LEN = 2
  64 <= PS_MAX_SIGNALS < 96, PS_MAX_SIG_ARR_LEN = 3
  and so on.
  We add 1 so that PS_EXT_SIG_ARR_LEN is always > 0, i.e. even if extension
  signal array is not required as we cant have an array of size 0.
---------------------------------------------------------------------------*/
#define PS_MAX_SIG_ARR_LEN \
((((uint32)PS_MAX_SIGNALS / (sizeof(rex_sigs_type)*8)) + \
(((uint32)PS_MAX_SIGNALS % (sizeof(rex_sigs_type)*8))? 1:0)) + 1)

/*--------------------------------------------------------------------------
  PS_SIG_ARR_LEN specifies the array length required for normal signals (not 
  extension signals).
---------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
#define PS_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
#else
#define PS_SIG_ARR_LEN 1
#endif

/*--------------------------------------------------------------------------
  PS_EXT_SIG_ARR_LEN specifies the length of the PS extension signal mask 
  array. If REX extension signals are supported, we need PS extension 
  mechanism only if the maximum number of PS signals exceeds that supported 
  by REX extension mechanism. Otherwise we set PS extension signal array 
  length to be zero.
  The added 1 in PS_MAX_SIG_ARR_LEN ensures that this array size would always
  be > 0.
---------------------------------------------------------------------------*/
#define PS_EXT_SIG_ARR_LEN (PS_MAX_SIG_ARR_LEN - PS_SIG_ARR_LEN)

/*--------------------------------------------------------------------------
  Minimum signal value which lies in the extension signal range.
---------------------------------------------------------------------------*/
#define PS_MIN_EXT_SIGNAL (sizeof(rex_sigs_type)*PS_SIG_ARR_LEN*8)

/*--------------------------------------------------------------------------
  EXT_SIG_IDX maps the signal number to the index in the ps_ext_sig_mask 
  array.
---------------------------------------------------------------------------*/
#define EXT_SIG_IDX(sig_num) SIG_IDX(sig_num) - PS_SIG_ARR_LEN

/*--------------------------------------------------------------------------
  PS_SET_EXT_SIG sets the bit in the ps_ext_sig_mask array corresponding to 
  the signal number.
---------------------------------------------------------------------------*/
#define PS_SET_EXT_SIG(sig_num) \
{\
  *(((rex_sigs_type *)(ps_ext_sig_mask)) + EXT_SIG_IDX(sig_num)) |= \
    ((rex_sigs_type) 1 << SIG_OFFSET(sig_num));\
}

/*--------------------------------------------------------------------------  
  PS_CLR_EXT_SIG clears the bit corresponding to the specified signal in the
  ps_ext_sig_mask array.
---------------------------------------------------------------------------*/
#define PS_CLR_EXT_SIG(sig_num) \
{ \
  *(((rex_sigs_type *)(ps_ext_sig_mask)) + EXT_SIG_IDX(sig_num)) &= \
    ~ ((rex_sigs_type) 1 << SIG_OFFSET(sig_num));\
}

                                       
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PSI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_CMD_Q_SIGNAL.  It retrieves the
  next command from the PS command queue and processes the command by calling
  the registered cmd handler.

DEPENDENCIES
    An item must be on the PS command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean ps_process_cmd
(
  void
);


/*===========================================================================

FUNCTION DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void default_cmd_handler
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);


/*===========================================================================
	
FUNCTION PS_GET_CMD_BUF()
	
DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
  psi_cmd_type * Buffer

SIDE EFFECTS
  None.
	
===========================================================================*/
psi_cmd_type* ps_get_cmd_buf
(

);

/*===========================================================================

FUNCTION PS_FREE_CMD_BUF()
	
DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
 None
 
 SIDE EFFECTS
  None.
	
==========================================================================*/

void ps_free_cmd_buf
(
  psi_cmd_type *ps_cmd_ptr
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the 
  signals which do not have a registered handler. It merely prints a warning 
  message.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean default_sig_handler
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */ 
);

#endif /* PS_TASKI_H */
