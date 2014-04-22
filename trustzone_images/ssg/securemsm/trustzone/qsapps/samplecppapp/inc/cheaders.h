/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __CHEADERS_H
#define __CHEADERS_H
/*
 * Including C specific headers in this file.
 * Which will be used by CPP TA as extern.
*/
extern "C"{
  #include <string.h>
  #include <stdint.h>
  #include <stdbool.h>
  #include "qsee_env.h"
  #include "qsee_error.h"
  #include "qsee_fs.h"
  #include "qsee_fuse.h"
  #include "qsee_log.h"
  #include "qsee_services.h"
  #include "qsee_stor.h"
  #include "qsee_sync.h"
  #include "qsee_timer.h"
  #include "qsee_trans_ns_addr.h"
}

/*
These functions need to follow the C linking convention as these are invoked by QTEE TA commonlib.
We need to have a mechanism to ensure that for these decalaration, CPP compiler
is not doing name mangling. 
Name mangling is done by cpp compiler to support feature link polymorphism.
*/

extern "C"{
  void tz_app_init(void);
  void tz_app_shutdown(void);
  void tz_app_cmd_handler( void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
}

#endif //CHEADERS_H