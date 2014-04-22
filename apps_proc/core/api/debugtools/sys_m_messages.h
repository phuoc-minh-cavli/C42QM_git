#ifndef SYS_M_MESSAGES_H
#define SYS_M_MESSAGES_H
/*===========================================================================

           S Y S _ M . H

DESCRIPTION

Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/api/debugtools/sys_m_messages.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/16   nk      Changes to update notification id to correct value

===========================================================================*/

//Notifications that you may register for
#define SYS_M_SSR_LPASS_BEFORE_SHUTDOWN      "ssr:lpass:before_shutdown"
#define SYS_M_SSR_WCNSS_BEFORE_SHUTDOWN      "ssr:wcnss:before_shutdown"
#define SYS_M_SSR_DSPS_BEFORE_SHUTDOWN       "ssr:dsps:before_shutdown"
#define SYS_M_SSR_MODEM_BEFORE_SHUTDOWN      "ssr:modem:before_shutdown"
#define SYS_M_SSR_GSS_BEFORE_SHUTDOWN        "ssr:gnss:before_shutdown"
#define SYS_M_SSR_EXT_MODEM_BEFORE_SHUTDOWN  "ssr:ext_modem:before_shutdown"
#define SYS_M_SSR_ADSP_BEFORE_SHUTDOWN       "ssr:adsp:before_shutdown"

#define SYS_M_SSR_LPASS_AFTER_POWERUP        "ssr:lpass:after_powerup"
#define SYS_M_SSR_WCNSS_AFTER_POWERUP        "ssr:wcnss:after_powerup"
#define SYS_M_SSR_DSPS_AFTER_POWERUP         "ssr:dsps:after_powerup"
#define SYS_M_SSR_MODEM_AFTER_POWERUP        "ssr:modem:after_powerup"
#define SYS_M_SSR_GSS_AFTER_POWERUP          "ssr:gnss:after_powerup"
#define SYS_M_SSR_EXT_MODEM_AFTER_POWERUP    "ssr:ext_modem:after_powerup"
#define SYS_M_SSR_ADSP_AFTER_POWERUP         "ssr:adsp:after_powerup"

#define SYS_M_SSR_FUSION_SFR                 "ssr:retrieve:sfr"

#define SYS_M_ACK                            "ssr:ack"
#define SYS_M_VER                            "info:version"



//Internal error callback 
#define SYS_M_ERR_CB_PREFLUSH                "sys_m_err_cb_preflush"
#define SYS_M_ERR_CB_POSTFLUSH               "sys_m_err_cb_postflush"

//Internal shutdown notifications
//Do NOT use these messages - leverage rcinit/tmc shutdown mechanism
#define SYS_M_POWEROFF_ACK                   SYS_M_ACK
#define SYS_M_SHUTDOWN_ACK                   SYS_M_ACK

#define SYS_M_SHUTDOWN                       "ssr:shutdown"
#define SYS_M_POWEROFF                       "ssr:poweroff"
#define SYS_M_SYSTEM_RESET                   "system:reset"
#define SYS_M_SYSTEM_SHUTDOWN                "system:shutdown"
#define SYS_M_SYSTEM_RESET_ACK               "system:ack"
#define SYS_M_SYSTEM_SHUTDOWN_ACK            "system:ack"

/** Hibernation Message & Hibernation ACK
 *
 * Any client which need hibernation information (i.e.) when system is going
 * into hibernation mode, need to register for below message either as a
 * callback type (using rcecb) or event/signal type (using rcevt), in case
 * of event/signal type, client needs to provide ACK using ACK message.
 *
 */
#define SYS_M_HIBERNATION       "system:hibernation"
#define SYS_M_HIBERNATION_ACK   "system:hibernation_ack"

#endif  /* SYS_M_MESSAGES_H */
