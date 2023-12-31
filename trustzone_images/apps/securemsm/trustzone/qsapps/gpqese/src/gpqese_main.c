#include <stddef.h>
#include <stdint.h>
#include "gpqese_cmds.h"
#include "qsee_ese_service.h"
#include "qsee_log.h"
#include "stringl.h"
#include "tee_internal_api.h"

/* ----------------------------------------------------------------------------
 *   Trusted Application Entry Points
 * ---------------------------------------------------------------------------- */

TEE_Result TA_CreateEntryPoint(void)
{
#ifdef GPQESE_QSEE_LOG_SUPPORTED
  qsee_log_set_mask(QSEE_LOG_MSG_DEBUG | qsee_log_get_mask());
#else
  qsee_log_set_mask(QSEE_LOG_MSG_ERROR | QSEE_LOG_MSG_FATAL);
#endif

  QSEE_ESE_ENTER();

  TEE_Result       retval        = TEE_ERROR_OUT_OF_MEMORY;
  GPQESE_Instance *pInstanceData = TEE_Malloc(sizeof(GPQESE_Instance), TEE_MALLOC_FILL_ZERO);

  if (pInstanceData)
  {
    TEE_SetInstanceData(pInstanceData);

    retval = TEE_SUCCESS;
  }

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
} // TA_CreateEntryPoint()

void TA_DestroyEntryPoint(void)
{
  QSEE_ESE_ENTER();

  GPQESE_Instance *pInstanceData = TEE_GetInstanceData();

  if (pInstanceData)
  {
    TEE_Free(pInstanceData);

    TEE_SetInstanceData(NULL);
  }

  QSEE_ESE_EXIT();

} // TA_DestroyEntryPoint()

TEE_Result TA_OpenSessionEntryPoint(uint32_t  nParamTypes,
                                    TEE_Param pParams[MAX_GP_PARAMS],
                                    void **   ppSessionContext)
{
  TEE_Result retval = TEE_ERROR_BAD_PARAMETERS;

  QSEE_ESE_ENTER();

  do
  {
    if (nParamTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE))
    {
      break;
    }

    if (ppSessionContext == NULL)
    {
      break;
    }

    *ppSessionContext = TEE_Malloc(sizeof(GPQESE_Session), TEE_MALLOC_FILL_ZERO);

    if (*ppSessionContext == NULL)
    {
      retval = TEE_ERROR_OUT_OF_MEMORY;
      break;
    }

    retval = TEE_SUCCESS;
  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
} // TA_OpenSessionEntryPoint()

void TA_CloseSessionEntryPoint(void *pSessionContext)
{
  QSEE_ESE_ENTER();

  if (pSessionContext)
  {
    GPQESE_Session *pSession = (GPQESE_Session *)pSessionContext;

    // Attempt to close the service
    GPQESE_Close(pSession, TEE_PARAM_TYPE_NONE);

    // Need secure_memset as context scParams could be sensitive
    secure_memset(pSession, 0, sizeof(*pSession));

    TEE_Free(pSession);
  }

  QSEE_ESE_EXIT();
} // TA_CloseSessionEntryPoint()

TEE_Result TA_InvokeCommandEntryPoint(void *    pSessionContext,
                                      uint32_t  nCommandID,
                                      uint32_t  nParamTypes,
                                      TEE_Param pParams[MAX_GP_PARAMS])
{
  TEE_Result retval = TEE_ERROR_BAD_PARAMETERS;

  QSEE_ESE_ENTER("appCommandId[0x%08X], nParamTypes[0x%08X]", nCommandID, nParamTypes);

  do
  {
    // Every command is cancellable. GP TEE SE API deals with cancellations
    TEE_UnmaskCancellation();

    switch (nCommandID)
    {
      case GPQESE_CMD_OPEN:
        retval = GPQESE_Open(pSessionContext, nParamTypes);
        break;

      case GPQESE_CMD_CLOSE:
        retval = GPQESE_Close(pSessionContext, nParamTypes);
        break;

      case GPQESE_CMD_TRANSCEIVE:
        retval = GPQESE_Transceive(pSessionContext, nParamTypes, pParams);
        break;

      case GPQESE_CMD_TRANSCEIVE_RAW:
        retval = GPQESE_TransceiveRaw(pSessionContext, nParamTypes, pParams);
        break;

      case GPQESE_CMD_GENERIC:
        retval = GPQESE_Generic(pSessionContext, nParamTypes, pParams);
        break;
    }
  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
} // TA_InvokeCommandEntryPoint()
