#ifndef DIAGCOMM_CMD_H 
#define DIAGCOMM_CMD_H
/*==========================================================================

              Diagnostic Communications for Diag Command/Response Channel

General Description
  
Copyright (c) 2015-2016 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.10/services/diag/DCM/common/src/diagcomm_cmd.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/01/16   rs      Added signature/prototype for diagcomm_cmd_status().
01/22/14   sr      Fixed the issue where cmd wmq is used without being 
                   initialized in Diag standalone case 
08/08/13   sg      Command/response separation

===========================================================================*/

#include "diagcomm_io.h"     /* For diagcomm_io_conn_type, diagcomm_enum_port_type */


/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */

boolean diagcomm_cmd_open( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
boolean diagcomm_cmd_status (diagcomm_enum_port_type port);
void diagcomm_cmd_close( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
void diagcomm_cmd_init( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
uint32 diagcomm_cmd_get_rx_wmq_cnt( diagcomm_port_type port_type, diagcomm_enum_port_type port_num );
void diagcomm_cmd_process_slave_tx( diagcomm_enum_port_type port_num );
void diagcomm_cmd_wmq_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num );

#endif //DIAGCOMM_CMD_H