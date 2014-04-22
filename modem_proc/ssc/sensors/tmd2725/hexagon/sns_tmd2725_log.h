#pragma once
/*
 * Copyright (c) 2018-2020, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "sns_printf.h"

/*
 * Notes regarding logging
 * -----------------------
 * This driver was developed with the following SCEP versions
 * SCEP 5 00070.1
 * SCEP 6 00011.1
 *
 * There are two levels of debug logging. The first
 * level is activated by defining ENABLE_DEBUG. The first level
 * provides functional logs. That means there are logs at initialization
 * and also when results are reported from the ambient light and
 * proximity sensors.
 *
 * The second level is activated by defining AMS_DEV_DEBUG_ENABLE.
 * This level is extremely verbose.
 *
 */

/* AMS Functional Debug */
#ifdef ENABLE_DEBUG
#define AMS_PRINTF(prio, sensor, ...) do { \
  SNS_PRINTF(prio, sensor, __VA_ARGS__); \
} while (0)
#define AMS_INST_PRINTF(prio, inst, ...) do { \
  SNS_INST_PRINTF(prio, inst , __VA_ARGS__); \
} while (0)

#else

#define AMS_PRINTF(prio, sensor, ...)do { \
  UNUSED_VAR(sensor); \
} while (0)
#define AMS_INST_PRINTF(prio, inst, ...)do { \
  UNUSED_VAR(inst); \
} while (0)
#endif

/* AMS Developer Debug */
#ifdef AMS_DEV_DEBUG_ENABLE
#define AMS_DEV_PRINTF(prio, sensor, ...) do { \
  SNS_PRINTF(prio, sensor, __VA_ARGS__); \
} while (0)
#define AMS_DEV_INST_PRINTF(prio, inst, ...) do { \
  SNS_INST_PRINTF(prio, inst , __VA_ARGS__); \
} while (0)

#else

#define AMS_DEV_PRINTF(prio, sensor, ...)do { \
  UNUSED_VAR(sensor); \
} while (0)
#define AMS_DEV_INST_PRINTF(prio, inst, ...)do { \
  UNUSED_VAR(inst); \
} while (0)
#endif
