# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import re
import shutil
from re import search
from subprocess import STDOUT, check_output

from sectools.common.utils import c_misc, c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_process import CoreSubprocess, run_command
from sectools.features.isc.signer.cass.keystore import IdentityKeystoreFactory, TrustKeystore
from sectools.features.isc.signer.signer_errors import ExternalSignerError
from .__secfile__ import DIR_PATH
from .defines import CASS_DIR

CASSCONNECTOR_DIR = DIR_PATH


class CassConnector(object):
    CASS_CLIENT_REFAPP_DIR = os.path.join(CASSCONNECTOR_DIR, 'cass-client-refapp')
    CLIENT_REFAPP_JAR = os.path.join('bin', 'java', 'cass-client-refapp')
    CLIENT_REFAPP_JAR_V2 = os.path.join(CASS_CLIENT_REFAPP_DIR, 'bin', 'java', 'CassConnector')
    CASS_CLIENT_REFAPP_CONFIG_DIR = os.path.join(CASS_CLIENT_REFAPP_DIR, 'config')
    CASS_CLIENT_REFAPP_INPUT_DIR = os.path.join(CASS_CLIENT_REFAPP_DIR, 'input')
    CASS_CLIENT_REFAPP_OUTPUT_DIR = os.path.join(CASS_CLIENT_REFAPP_DIR, 'output')
    SCHEMA_XSD_FILE_PATH = os.path.join(CASS_CLIENT_REFAPP_CONFIG_DIR, 'signature_package.xsd')
    SIGNATURE_PACKPAGE_REL_PATH = os.path.join("signingpackage", "signingpackage.xml")

    DEFAULT_HOST_VALIDATION_MODE = "STAGE_PROD"
    TIMEOUT = 300

    MESG_TIMEOUT = "Connection to CASS server timeout after {0} seconds"
    MESG_WRONG_JAVA_VERSION = 'JRE 32 bit (1.8) is required to provide SunPKCS11 implementation.\n' \
                              'Run "java -version" and check that JRE 32 bit (1.8) is installed and in the path environment variable.\n' \
                              'Run "which java" to make sure the path points to the correct version.'\
                              'If it points to c:\\Windows\\system32\\java, remove the java binary from c:\\Windows\\system32 and add the explicit path to the path environment variable'

    ERR_WRONG_JAVA_VERSION = "java.lang.NoClassDefFoundError: sun/security/pkcs11/SunPKCS11"
    ERR_WRONG_JAVA_VERSION2 = "java.lang.NoClassDefFoundError: sun/security/pkcs11/wrapper/PKCS11"
    ERR_UNABLE_TO_ACCESS_KEYSTORE = "UNABLE TO ACCESS KEYSTORE"
    ERR_SIGNATURE_PACKAGE_INVALID = "FAILURE : SIGNATURE PACKAGE INVALID OR NOT FOUND"
    ERR_VERBOSE1 = "Individual element FAILURE: Response Sequence ID 1 - (BASEEX1) (BASEEX1) Signing request is invalid:"
    ERR_VERBOSE2 = "Individual element FAILURE: Response Sequence ID 1 - (SPBOEX2)"
    ERR_CASS_SERVER = "The CASS server returned the following error:"

    def __init__(self, cass_signer_attributes, signing_package_file_name, output_dir):
        self.cass_signer_attributes = cass_signer_attributes
        self.signing_package_file_name = signing_package_file_name
        self.output_dir = output_dir
        self._initialize()

    def _initialize(self):
        self._get_keystore()
        self._get_trust_keystore()

    def _get_keystore(self):
        self.identity_keystore = IdentityKeystoreFactory.get_key_store(self.cass_signer_attributes.user_identity)

    def _get_trust_keystore(self):
        self.trust_keystore = None
        self.host_validation_mode = self.DEFAULT_HOST_VALIDATION_MODE
        if self.cass_signer_attributes.server:
            self.trust_keystore = TrustKeystore(
                    filepath=self.cass_signer_attributes.server.trust_keystore,
                    password=self.cass_signer_attributes.server.trust_keystore_password,
                    keystoreType=self.cass_signer_attributes.server.trust_keystore_type)
            self.host_validation_mode = self.cass_signer_attributes.server.host_validation_mode

    @property
    def host_validation_mode(self):
        return self._host_validation_mode

    @host_validation_mode.setter
    def host_validation_mode(self, value):
        if value:
            self._host_validation_mode = value
        else:
            self._host_validation_mode = self.DEFAULT_HOST_VALIDATION_MODE

    @staticmethod
    def get_java_version_installed_on_system():
        """For java version 10 or above "java -version" command returns java version in format of XX.a.b and
        before java 10 it returns in format of 1.X.a where X is the java major version and a is minor version"""
        try:
            return int(next(iter(filter(None, search(r'version "1.(\d+)|version "(\d+)',
                                                     check_output(["java", "-version"], stderr=STDOUT).
                                                     decode("utf-8")).groups()))))
        except Exception:
            raise RuntimeError("CASS requires that Java version 8 or higher be installed.")

    def sign(self):
        java_version = self.get_java_version_installed_on_system()
        if java_version == 8:
            logger.debug("Invoking cass-client-refapp jar.")
            response = self.sign_java8()
        elif java_version >= 9:
            logger.debug("Invoking CassConnector jar.")
            response = self.sign_java9()
        else:
            raise RuntimeError("CASS requires that Java version 8 or higher be installed.")
        return response

    def sign_java8(self):
        signature_package_response = None
        self._get_cmds()
        cass_server = self.cass_signer_attributes.server.host if self.cass_signer_attributes.server else "default CASS server"
        logger.info("Connecting to {0}".format(cass_server))
        output = self._execute_cmds()
        logger.debug(output)
        signature_package_file_path = os.path.join(self.output_dir, self.SIGNATURE_PACKPAGE_REL_PATH)
        if os.path.isfile(signature_package_file_path):
            logger.info("Signature package retrieved from server: " + c_path.normalize(signature_package_file_path))
            signature_package_response = c_misc.load_data_from_file(signature_package_file_path)

            # clean up
            path, _ = os.path.split(signature_package_file_path)
            shutil.rmtree(path)
        return signature_package_response

    def _get_cmds(self):
        if os.path.exists(self.output_dir) is None:
            os.makedirs(self.output_dir)

        signing_package_file = c_path.normalize(self.signing_package_file_name)
        cmds = ["java"]
        if self.cass_signer_attributes.server:
            cmds.append("-Dcass_server.host={0}".format(self.cass_signer_attributes.server.host))
            cmds.append("-Dcass_server.sslport={0}".format(self.cass_signer_attributes.server.port))
            cmds.append("-Dtrust_anchor.file={0}".format(self.trust_keystore.file))
            cmds.append("-Dtrust_anchor.password={0}".format(self.trust_keystore.password))
            cmds.append("-Dtrust_anchor.keystoretype={0}".format(self.trust_keystore.type))
            cmds.append("-Dcn_verify.mode={0}".format(self.host_validation_mode))
        cmds.append("-Dsig_authority.file={0}".format(self.identity_keystore.file))
        cmds.append("-Dsig_authority.password={0}".format(self.identity_keystore.password))
        cmds.append("-Dsig_authority.keystoretype={0}".format(self.identity_keystore.type))
        if self.identity_keystore.token_driver_home:
            cmds.append("-Detoken_driver_home={0}".format(self.identity_keystore.token_driver_home))
        cmds.append("-Doutput.location={0}".format(self.output_dir))
        cmds.append("-Dschema.location={0}".format(c_path.normalize(self.SCHEMA_XSD_FILE_PATH)))

        cmds.extend(['-jar', self.CLIENT_REFAPP_JAR, signing_package_file])

        # only enable -v if debug is enabled
        if logger.verbosity < logger.DEBUG:
            cmds.append('-v')

        self.cmds = cmds

    def _execute_cmds(self):
        # Need to run in cass-client-refapp directory
        returnValue, returnError, f_timeout, f_retcode, f_output = CassCoreSubprocess.executeCommand(
                            launchCommand=self.cmds,
                            retcode=0,
                            timeout=self.TIMEOUT,
                            successString="SUCCESS",
                            workingDir=self.CASS_CLIENT_REFAPP_DIR,
                            stderr=CoreSubprocess.STDERR_STDOUT)

        if returnValue is False:
            if f_timeout is True:
                raise ExternalSignerError(self.MESG_TIMEOUT.format(self.TIMEOUT))
            else:
                error_mesg = self._process_error(f_output)
                raise ExternalSignerError(error_mesg)
        return f_output

    def sign_java9(self):
        signature_package = None
        cmd = ["java", "-jar", self.CLIENT_REFAPP_JAR_V2]

        if self.cass_signer_attributes.server:
            logger.info("Connecting to {0}".format(self.cass_signer_attributes.server.host))
            cmd.extend(["--url", self.cass_signer_attributes.server.host])
            cmd.extend(["--port", str(self.cass_signer_attributes.server.port)])
            cmd.append("sign")
            cmd.extend(["--truststore", self.trust_keystore.file])
            cmd.extend(["--truststore-file-type", self.trust_keystore.type])
            cmd.extend(["--truststore-password", self.trust_keystore.password])
            cmd.extend(["--host-validation-mode", self.cass_signer_attributes.server.host_validation_mode])
        else:
            logger.info("Connecting to default CASS server")
            cmd.append("sign")
            cmd.extend(["--truststore", os.path.join(CASS_DIR, 'cass-client-refapp', 'input', 'prod_cass_root.jks')])
            cmd.extend(["--truststore-password", "123456"])

        cmd.extend(["--keystore-type", self.identity_keystore.type])
        if self.identity_keystore.type == "PKCS12":
            cmd.extend(["--sig-authority-file", self.identity_keystore.file])
        else:
            cmd.extend(["--pkcs11-config", str(self.identity_keystore.file)])
        cmd.extend(["--keystore-password", self.identity_keystore.password])
        cmd.extend(["--signing-package", c_path.normalize(self.signing_package_file_name)])
        signature_package_file = os.path.join(self.output_dir, self.SIGNATURE_PACKPAGE_REL_PATH)
        cmd.extend(["--outfile", signature_package_file])

        logger.debug("CassConnector command: " + self.mask_private_cmdline_args(" ".join(cmd), ["--keystore-password"]))
        run_command(cmd, log=False)

        if os.path.isfile(signature_package_file):
            logger.info("Signature package retrieved from server: " + c_path.normalize(signature_package_file))
            signature_package = c_misc.load_data_from_file(signature_package_file)
            # Cleanup.
            path, _ = os.path.split(signature_package_file)
            shutil.rmtree(path)

        return signature_package

    @staticmethod
    def mask_private_cmdline_args(cmd, args_to_mask):
        for arg_to_mask in args_to_mask:
            val_to_mask = re.search(arg_to_mask + r"(?:\s|=)+(\S*)", cmd)
            if val_to_mask:
                cmd = cmd.replace(val_to_mask.group(1), "****")
        return cmd

    def _process_error(self, output):
        error_message = output

        if output.lower().find(self.ERR_WRONG_JAVA_VERSION.lower()) > 0 or output.lower().find(self.ERR_WRONG_JAVA_VERSION2.lower()) > 0:
            error_message = self.MESG_WRONG_JAVA_VERSION
        elif output.lower().find(self.ERR_UNABLE_TO_ACCESS_KEYSTORE.lower()) > 0:
            self.identity_keystore.reset_password()
        elif output.lower().find(self.ERR_SIGNATURE_PACKAGE_INVALID.lower()) > 0:
            error_message = self.ERR_CASS_SERVER + error_message \
                .replace(self.ERR_VERBOSE1, "") \
                .replace(self.ERR_VERBOSE2, "") \
                .replace(self.ERR_SIGNATURE_PACKAGE_INVALID, "") \
                .lstrip("\n") \
                .rstrip("\n")

        return error_message


class CassCoreSubprocess(CoreSubprocess):

    def _maskPassword(self, string_to_replace):
        string_replaced = string_to_replace
        keyword_to_search = "password="

        try:
            index_start = string_to_replace.index(keyword_to_search) + len(keyword_to_search)
            index_end = len(string_to_replace)
        except ValueError:
            # It is ok if password is not found
            pass
        else:
            value = '*' * (index_end - index_start)
            string_replaced = string_to_replace[:index_start] + str(value) + string_to_replace[index_end + 1:]

        return string_replaced

    # This is a hook for caller to format the command
    # line string for printing
    def formatLaunchCommandForPrinting(self, cmd):
        formatted_cmd = []

        for eachArg in cmd:
            eachArg = self._maskPassword(eachArg)
            formatted_cmd.append(eachArg)

        return formatted_cmd

    # Suppress printing Finish command
    def printFinish(self, cmd):
        pass
