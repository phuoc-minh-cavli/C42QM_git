/**
 * sns_psm.h
 *
 * The PSM Utility interface
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

  /**
  *  returns TRUE of Q6 power collapse is in progress and false
  *          for other cases
  *  @return
  *     TRUE if Q6 Power Collapse in progress and FALSE other case
  */


#ifdef SNS_AVOID_HANDLING_INTERRUPT_DURING_PC
#include <stdbool.h>
bool pc_in_progress( void );
#endif
