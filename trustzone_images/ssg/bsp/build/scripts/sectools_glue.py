# ===============================================================================
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import atexit
from optparse import OptionParser
import sys

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
  """Required SCons method for confirming module existence"""
  return env.Detect('sectools_glue')

# Parse any signing arguments
# We cannot ensure CASS validation until GetSignserverUrl is available,
# but this at least makes the arguments accessible
def generate(env):
  env.AddCommandlineArgument(('SIGNTYPE','SIGNTYPE',''))
  env.AddCommandlineArgument(('SIGNSERVER','SIGNSERVER',''))
  env.AddCommandlineArgument(('SIGNPORT','SIGNPORT',''))
  env.AddMethod(build, "SectoolBuilderGlue")
  env.AddMethod(get_signserver_url, "GetSignServerUrl")
  env.AddMethod(get_signer_params_v2, "GetSignerParams_v2")

  # Validate CASS signing arguments
  if env.get('SIGNTYPE') == 'CASS':
    if not env.get('SIGNSERVER'):
      raise ValueError("Error: For SIGNTYPE CASS SIGNSERVER must be specified as \"SIGNSERVER = <hostname> \" ")
    else:
      os.environ['SECTOOLS_SIGNER_HOST'] = env.GetSignServerUrl()
      if env.get('SIGNPORT'):
        os.environ['SECTOOLS_SIGNER_PORT'] = env.get('SIGNPORT')
def get_signserver_url(env):
    """Reads the signing server from env["SIGNSERVER"] and transforms the value
       into a valid signing url, prepending a proper http* prefix if required.
       Returns the updated signserver url"""
    signserver = env.get("SIGNSERVER")
    if signserver is None:
        raise ValueError("SIGNSERVER must be defined in the environment")

    if not signserver.lower().startswith("http"):

        signserver = "http://{}".format(signserver)

    return signserver

def get_signer_params_v2(env):
    """Assembles and returns appropriate signing server parameters for a
       secimage invocation based on SIGNSERVER/SIGNPORT in the current env."""
    signserver = env.get("SIGNSERVER")
    if not signserver:
        return "--signing-mode TEST"

    signport = env.get("SIGNPORT")
    if not signport:
        raise ValueError("Must provide a valid SIGNPORT")

    signserver_url = env.GetSignServerUrl()

    qti_signer_params = [
        "--signing-mode QTI-REMOTE",
        "--cass-capability {}".format(cass_capability),
        "--qti-remote-signing-server-port {}".format(signport),
        "--qti-remote-signing-server-url {}".format(signserver_url),
    ]

    return " ".join(qti_signer_params)

def build(env,
    glue_target_base_dir,
    glue_source,
    glue_sign_id,
    glue_signer=None,
    glue_qti_sign=False,
    glue_sectools_install_base_dir=None,
    glue_install_file_name=None,
    glue_msmid=None,
    glue_msmid_jtagid_dict=None,
    glue_jtag_id=None,
    glue_config = None,
    glue_config_qti = None,
    glue_soc_hw_version=None,
    glue_app_id=None,
    glue_soc_vers = None,
    glue_max_num_root_certs = None,
    glue_is_step1 = False):

  if glue_is_step1:
    if 'USES_SEC_POLICY_STEP1_QC_SIGN' in env:
      if 'USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN' in env:
        del env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN']
      env['USES_SEC_POLICY_DEFAULT_SIGN'] = True
    sectools_signed_mbn_step1 = env.SectoolBuilder(
            target_base_dir = glue_target_base_dir,
            source=glue_source,
            sign_id=glue_sign_id,
            signer = glue_signer,
            qti_sign = glue_qti_sign,
            sectools_install_base_dir = glue_sectools_install_base_dir,
            install_file_name = glue_install_file_name,
            config = glue_config_qti  if glue_qti_sign else glue_config,
            soc_hw_version=glue_soc_hw_version,
            soc_vers=glue_soc_vers,
            target_image_type_filter = env.SectoolImageTypeSign(),
            max_num_root_certs = glue_max_num_root_certs)
    return  sectools_signed_mbn_step1
  else:
    if 'USES_SEC_POLICY_STEP2_OEM_SIGN' in env:
      if 'USES_SEC_POLICY_DEFAULT_SIGN' in env:
         del env['USES_SEC_POLICY_DEFAULT_SIGN']
      env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN'] = True
    sectools_signed_mbn_step2 = env.SectoolBuilder(
            target_base_dir = glue_target_base_dir,
            source=glue_source,
            sign_id=glue_sign_id,
            signer = glue_signer,
            qti_sign = glue_qti_sign,
            sectools_install_base_dir = glue_sectools_install_base_dir,
            install_file_name = glue_install_file_name,
            config= glue_config,
            soc_hw_version=glue_soc_hw_version,
            soc_vers=glue_soc_vers,
            max_num_root_certs = glue_max_num_root_certs)
    return sectools_signed_mbn_step2
#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------
