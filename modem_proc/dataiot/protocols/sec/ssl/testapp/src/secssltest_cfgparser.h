#ifndef SECSSLTESTCFGPARSER_H
#define SECSSLTESTCFGPARSER_H

/*===========================================================================
               S E C S S L    C O N F I G   P A R S E R   F I L E 
 
                            H E A D E R   F I L E 

FILE:  secssltest_cfgparser.h

DESCRIPTION: 

  Since IxConfigparser.cpp is not in the smecom folder, some builds cannot 
  use this file. This file copies IxConfigparser.h and rename the class 
  name to make it useful in smetest.

  
EXTERNALIZED FUNCTIONS : 


  Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/sec/ssl/testapp/src/secssltest_cfgparser.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/



/*===========================================================================
               INCLUDE FILES

============================================================================*/
#include "comdef.h"
#include "IxErrno.h"


#ifdef __cplusplus
class secssltest_cfgparser
{

public:

secssltest_cfgparser ();

~secssltest_cfgparser ();

/*===========================================================================
FUNCTION secsslparser_getelementvalue()

DESCRIPTION
  This method extracts the bytes enclosed within a specific occurrence
  of a specified start tag and a specified end tag and passes it to the
  OUT argument ppValue.

DEPENDENCIES
  NONE

PARAMETERS
  pStartTag : String denoting the start tag
  pEndTag   : String denoting the end tag
  ppValue   : pointer to the memory pointing to the extracted string
  uCount    : The specific occurence of the tag (starts from 0)

RETURN VALUE
  E_SUCCESS on success and appropriate error code on failure

SIDE EFFECTS
  None
===========================================================================*/
IxErrnoType secsslparser_getelementvalue (const char* pStartTag,
                             char**      ppValue,
                             uint32      uCount=0);

/*===========================================================================
FUNCTION secsslparser_init()

DESCRIPTION
  This method must be called before secsslparser_getelementvalue can return any
  information. Parses the input files and loads all the information into
  memory.

DEPENDENCIES
  NONE

PARAMETERS
  pFilePath: config file path

RETURN VALUE
  E_SUCCESS on success and appropriate error code on failure

SIDE EFFECTS
  All the tag information from the config file is loaded into memory
===========================================================================*/
IxErrnoType secsslparser_init (const char *pFilePath);

/*===========================================================================
FUNCTION secssltest_reinit()

DESCRIPTION
  Reinitializes an secssltest_cfgparser object with a new config file.

DEPENDENCIES
  NONE

PARAMETERS
  pFilePath: config file path

RETURN VALUE
  E_SUCCESS on success and appropriate error code on failure

SIDE EFFECTS
  All the tag information from the config file is loaded into memory
===========================================================================*/
IxErrnoType secssltest_reinit (const char *pFilePath);

private:

char* __pBuf;

};
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "secssl.h"

#define CONFIG_FILE_PATH "/config/sslconfig.xml"

IxErrnoType secsslcfg_freeallsesmemory( void );

IxErrnoType secsslcfg_setallsesparam 
(
   secssl_handle_type ssl_session, 
   const char *pConfgFilePath,
   secssl_ses_alert_cb_fptr_type  alert_cb_fptr,
   secssl_ses_socket_fd_pdata_type ex_server_socket,
   boolean  set_ses_server
);

IxErrnoType secsslcfg_isexternalmode
(
   const char *pConfigFilePath,
   boolean * is_external
);

IxErrnoType secsslcfg_is_ipv6
(
   const char *pConfigFilePath,
   boolean * is_ipv6
);

IxErrnoType secsslcfg_getelementvalue
(
  const char *pConfigFilePath,
  const char *pStartTag, 
  char      **ppValue,
  uint32      uCount
);

IxErrnoType secsslcfg_setsesserver
( 
   secssl_handle_type ssl_session,
   secssl_ses_parameter_pdata_union_type  param
);

#ifdef __cplusplus
}
#endif

#endif   /* SECSSLTESTCFGPARSER_H */

