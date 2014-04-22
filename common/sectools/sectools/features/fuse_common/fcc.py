# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
:Authors: sectools

Main interface to the fuseblower functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""

import datetime
import os
import shutil
from collections import namedtuple

from sectools.common.parsegen.elf.header.ident import ELFCLASS32, ELFCLASS64
from sectools.common.parsegen.secdat.format import ParseGenSecdat
from sectools.common.parsegen.secdat.secdatelf.defines import SECDAT_SEGMENT_INDEX
from sectools.common.parsegen.secdat.secdatelf.format import ParseGenSecdatElf
from sectools.common.parsegen.secdat.segment.defines import SEG_TYPE_EFUSE, SEG_TYPE_KEYPROV, SEG_TYPE_ROT
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import store_data_to_file, load_data_from_file, \
    PROGRESS_CB_PYPASS, ProgressNotifier
from sectools.features.fuse_common import datamodel
from sectools.features.fuse_common import defines
from sectools.features.fuse_common.debugrepr import dump_debug_data_model, dump_debug_secdat
from sectools.features.fuse_common.parsegen.secdat import SecdatParseGen as secdat_pg
from sectools.features.fuse_common.validator.secdat_validator import SecDatValidator
from sectools.features.isc.secimage.secimage_core import SecImageCore


class FuseCommonCore(object):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform fuse blowing related operations
    (generation, validation of sec.dat).

    User can select the config files and output dir for secdat generation using:

    - :meth:`workspace` and :meth:`chipset`
    - :meth:`oem_config_path`, :meth:`qti_config_path`, :meth:`ui_config_path` and :meth:`output_dir`

    User can specify secdat file for validation using:

    - :meth:`secdat`

    User can finally perform the following operations:

    - :meth:`verify_setup`
    - :meth:`generate`
    - :meth: `sign`
    - :meth:`validate`

    User can also perform multiple operations using :meth:`process`

    :param str config_dir: Path to the directory containing the config files
        for the supported chipsets.
    :param bool debug: If all the underlying modules should work in debug mode
        (save debug logs, etc).

    .. data:: debug

        (bool) Is debug mode enabled. The debug logs will be stored in :meth:`output_dir`

    """

    def __init__(self, config_dir=defines.DEFAULT_CONFIG_DIR, debug=False):
        """Initializes the internal properties."""
        config_dir = c_path.normalize(config_dir)

        # Object representing the workspace, when set.
        # type: :class:`fb_config.WorkspaceDir`
        self._workspace_dir_obj = None

        # Object representing the config directory.
        # type: :class:`fb_config.ConfigDir`
        self._config_dir_obj = self.CONFIG_MODULE.ConfigDir(config_dir)

        # Object representing the config files being used.
        # type: :class:`fb_config.ConfigParser`
        self._config_parser = self.CONFIG_MODULE.ConfigParser()

        # Path of the output directory
        self._output_dir = None

        # Path of the input secdat file
        self._secdat = None

        # secimage file for signing
        self._secimage_config = None

        # Path to output common secdat file
        self.secdat_common = None

        # Public properties
        self.debug = debug
        self.debug_dir = None
        self.debug_prefix = None
        self.debug_suffix = None

        # version drectory
        self.version_dir = None

        # Internal flag to determine if sec.dat has been extracted into temporary file from sec.elf - needed to cleanup
        self._tmp_secdat_created = False

    @property
    def config_dir(self):
        """(str) Path to the config directory provided during initialization of
        the class.
        Cannot be set.
        """
        return self._config_dir_obj.config_dir

    @config_dir.setter
    def config_dir(self, config_dir):
        raise RuntimeError('Config dir cannot be set.')

    @property
    def workspace(self):
        """(str) Path to the workspace directory."""
        if self._workspace_dir_obj is None:
            raise RuntimeError('Workspace is not set.')
        return self._workspace_dir_obj

    @workspace.setter
    def workspace(self, workspace_path):
        workspace_path = c_path.normalize(workspace_path)
        self._workspace_dir_obj = self.CONFIG_MODULE.WorkspaceDir(workspace_path)
        logger.info('Workspace is set to: ' + workspace_path)

    @property
    def available_owners(self):
        """(list[str]) List of owners for which the data model is available.
        Cannot be set.
        """
        return datamodel.defines.SUPPORTED_OWNERS

    @available_owners.setter
    def available_owners(self, owners_list):
        raise RuntimeError('Available owners cannot be set.')

    @property
    def available_operations(self):
        """(list[str]) List of operations which are available to be performed
        on the fuses.
        Cannot be set.
        """
        return datamodel.defines.SUPPORTED_OPERATIONS

    @available_operations.setter
    def available_operations(self, operation_list):
        raise RuntimeError('Available operations cannot be set.')

    @property
    def available_chipsets(self):
        """(list[str]) List of chipsets for which config files are available
        in :meth:`workspace` and :meth:`config_dir`.
        Cannot be set.
        """
        if self._workspace_dir_obj is not None:
            workspace_chipsets = set(self._workspace_dir_obj.chipsets)
        else:
            workspace_chipsets = set()
        config_dir_chipsets = set(self._config_dir_obj.chipsets)
        return list(workspace_chipsets.union(config_dir_chipsets))

    @available_chipsets.setter
    def available_chipsets(self, chipset_list):
        raise RuntimeError('Available chipsets cannot be set.')

    @property
    def chipset(self):
        """(str) Chipset that is selected for the current run. Chipset can only
        be set after :meth:`workspace` is specified. Setting the chipset will
        update the :meth:`oem_config_path`, :meth:`qti_config_path`,
        :meth:`ui_config_path` and :meth:`output_dir`
        """
        if self._config_parser.chipset is not None:
            return self._config_parser.chipset
        else:
            raise RuntimeError('Config paths or chipset is not set.')

    @chipset.setter
    def chipset(self, chipset):
        # Check that the workspace is set
        if self._workspace_dir_obj is None:
            raise RuntimeError('Please set workspace before setting the chipset.')

        # Log if the chipset is changed
        try:
            selected_chipset = self.chipset
            if selected_chipset:
                logger.note('Switching chipset from "' + selected_chipset + '" to "' + chipset + '"')
        except Exception:
            pass

        # Update the workspace
        self._update_workspace(chipset)

        # Set the config paths
        self.set_config_paths(*self._workspace_dir_obj.get_chipset_config_paths(chipset))

        # Set the ouput dir
        self.output_dir = c_path.join(os.path.dirname(self._workspace_dir_obj.config_dir),
                                      'secdat/' + chipset)

        # Add the existing secdat
        secdat = c_path.join(self.output_dir, 'sec.dat')
        if c_path.validate_file(secdat):
            self.secdat = secdat
        else:
            self.secdat = None

    def _update_workspace(self, chipset):
        """Checks if the :meth:`workspace` contains the files for the given
        chipset. If the files dont exist in the workspace, :meth:`config_dir` is
        searched for the files for the chipset. If the files are found in the
        config_dir, they are copied to the workspace, otherwise an exception
        is raised.

        :param str chipset: The chipset to find in the workspace.
        :raise RuntimeError: If files for the chipset are not found in the
            workspace or the config_dir
        """
        # Check if workspace is missing any of the config files for the chipset
        has_oem, has_qti, has_ui, has_user = self._workspace_dir_obj.has_configs(chipset)

        # If any of the files are missing, update the workspace
        if not (has_oem and has_qti and has_ui and has_user):
            # Put these files in the workspace
            self.reset_workspace(chipset, has_oem, has_qti, has_ui, has_user)

    def reset_workspace(self, chipset=None, oem=True, qti=True, ui=True, user=True):
        """Resets the :meth:`workspace` for the chipset that is provided.
        :meth:`config_dir` is searched for the files for the chipset. If the
        files are found in the config_dir, they are copied to the workspace,
        otherwise an exception is raised.

        :param str chipset: The chipset to reset in the workspace.
        :param bool oem: If the oem file should be updated.
        :param bool qti: If the qti file should be updated.
        :param bool ui: If the ui file should be updated.
        :param bool user: If the user file should be updated.
        :raise RuntimeError: If files for the chipset are not found in the
            the config_dir
        """
        if chipset is None:
            chipset = self.chipset

        # Look for the config files in the config_dir
        try:
            template_oem, template_qti, template_ui, template_user = self._config_dir_obj.get_chipset_config_paths(chipset)
        except Exception as e:
            logger.warning('Template config files not found for copying to workspace for chipset: ' + chipset + '\n'
                           '    ' + 'Error: ' + str(e))
            raise

        # Create the directory in the workspace for this chipset
        self._workspace_dir_obj.create_chipset_dir(chipset)
        workspace_chipset_dir = self._workspace_dir_obj.get_chipset_dir(chipset)

        if not oem:
            # Copy the OEM template file
            logger.info('Copying template OEM config file to workspace for chipset: ' + chipset)
            shutil.copy(template_oem, workspace_chipset_dir)
            logger.note('OEM config file created at: ' + workspace_chipset_dir)

        if not qti:
            # Copy the QTI template file
            logger.info('Copying template QTI config file to workspace for chipset: ' + chipset)
            shutil.copy(template_qti, workspace_chipset_dir)
            logger.note('QTI config file created at: ' + workspace_chipset_dir)

        if not ui:
            # Copy the UI template file
            logger.info('Copying template UI config file to workspace for chipset: ' + chipset)
            shutil.copy(template_ui, workspace_chipset_dir)
            logger.note('UI config file created at: ' + workspace_chipset_dir)

        if not user:
            # Copy the USER template file
            logger.info('Copying template USER config file to workspace for chipset: ' + chipset)
            shutil.copy(template_user, workspace_chipset_dir)
            logger.note('USER config file created at: ' + workspace_chipset_dir)

    @property
    def is_workspace_resettable(self):
        """(bool) Is the workspace resettable.
        """
        resettable = True
        try:
            self.workspace
            self.chipset
        except Exception:
            resettable = False
        return resettable

    @property
    def user_config_path(self):
        """(str) OEM config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.user_config_path

    @user_config_path.setter
    def user_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def oem_config_path(self):
        """(str) OEM config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.oem_config_path

    @oem_config_path.setter
    def oem_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def qti_config_path(self):
        """(str) QTI config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.qti_config_path

    @qti_config_path.setter
    def qti_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def ui_config_path(self):
        """(str) UI config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.ui_config_path

    @ui_config_path.setter
    def ui_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    def set_config_paths(self, oem_config_path=None, qti_config_path=None,
                         ui_config_path=None, user_config_path=None, secimage_config_path=None, dummy=None):
        """Sets the config paths to the paths provided.

        :param str oem_config_path: Path to the OEM config file
        :param str qti_config_path: Path to the QTI config file
        :param str ui_config_path: Path to the UI config file
        :param str secimage_config_path: Path to the secimage config file
        """
        # Normalize all paths
        oem_config_path = c_path.normalize(oem_config_path) if oem_config_path is not None else None
        qti_config_path = c_path.normalize(qti_config_path) if qti_config_path is not None else None
        ui_config_path = c_path.normalize(ui_config_path) if ui_config_path is not None else None
        user_config_path = c_path.normalize(user_config_path) if user_config_path is not None else None
        if secimage_config_path:
            self.secimage_config = secimage_config_path

        # Set the paths on the config_parser
        self._config_parser.read(oem_config_path,
                                     qti_config_path,
                                     ui_config_path,
                                     user_config_path,
                                     dummy)


    @property
    def secimage_config(self):
        """(str) Secimage config path that is selected for the current run.
        """
        if self._secimage_config is None:
            raise RuntimeError('Secimage config is not set')
        return self._secimage_config

    @secimage_config.setter
    def secimage_config(self, config_path):
        config_path = c_path.normalize(config_path)
        if not c_path.validate_file(config_path):
            raise RuntimeError('Secimage config is inaccessible at given path: ' + str(config_path))
        self._secimage_config = config_path

    @property
    def output_dir(self):
        """(str) Path to the output directory to use for saving logs, output
        files, debug_files.
        """
        if self._output_dir is not None:
            return self._output_dir
        else:
            raise RuntimeError('Output directory is not set')

    @output_dir.setter
    def output_dir(self, output_dir):
        if output_dir:
            output_dir = c_path.normalize(output_dir)
            try:
                c_path.create_dir(output_dir)
            except Exception as e:
                raise RuntimeError('Cannot write in the given output_dir: ' + output_dir + '\n'
                                   '    ' + 'Error: ' + str(e))
        self._output_dir = output_dir
        logger.info('Output dir is set to: ' + output_dir)

    @property
    def secdat(self):
        """(str) Path to the secdat file to be validated."""
        if self._secdat is not None:
            return self._secdat
        else:
            raise RuntimeError('Secdat path is not set')

    @secdat.setter
    def secdat(self, secdat_path):
        if secdat_path:
            secdat_path = c_path.normalize(secdat_path)
            if not c_path.validate_file(secdat_path):
                raise RuntimeError('Cannot access the secdat file: ' + secdat_path)
        self._secdat = secdat_path
        logger.info('Secdat path is set to: ' + str(secdat_path))

    @property
    def data_model(self):
        """(obj) Handle to the data model that is being worked with.
        Cannot be set.
        """
        return self._config_parser.data_model

    @data_model.setter
    def data_model(self, data_model):
        raise RuntimeError('Data model cannot be set.')

    def get_data_model_by_owner(self, owner):
        """Returns a handle to the data model filtered by the given owner.

        :param str owner: The owner to filter the data model with.
        :returns obj: datamodel
        """
        return datamodel.auto_gen_model.filter_by_owner(self._config_parser.data_model, owner)

    def update_data_model(self, data_model):
        """Updates the internal data model with the one given.

        :param obj data_model: The data model to use to update the internal data
            model
        """
        self._config_parser.data_model += data_model

    def save_workspace(self):
        """Saves the current :meth:`data_model` in the workspace by
        updating the config files for the selected chipset in the workspace.
        This API works only if the :meth:`workspace` and :meth:`chipset` is set.
        """
        if self._workspace_dir_obj is None:
            raise RuntimeError('Workspace is not set.')

        self._config_parser.write()
        self.generate()

    def get_supported_validations(self):
        # TODO: Hook up to the validator
        pass

    def set_validations(self):
        # TODO: Hook up to the validator
        pass

    def _verify_generate(self):
        """Verify that the current arguments are sufficient for generation of
        secdat.
        :returns: (Valid?, Error if not valid)
        :rtype: tuple (bool, str)
        """
        if not (self.oem_config_path or self.qti_config_path):
            return False, 'generation of sec.dat: Missing at least one required config file'
        elif not (self.output_dir):
            return False, 'generation of sec.dat: Missing the output directory'
        else:
            return True, ''

    def _verify_validate(self):
        """Verify that the current arguments are sufficient for validation of
        secdat.
        :returns: (Valid?, Error if not valid)
        :rtype: tuple (bool, str)
        """
        if not (self.oem_config_path or self.qti_config_path):
            return False, 'validation of sec.dat: Missing at least one required config file'
        elif not (self.secdat):
            return False, 'validation of sec.dat: Missing sec.dat path'
        else:
            return True, ''

    @property
    def is_validate_enabled(self):
        """(bool) If validation is enabled"""
        return self._verify_validate()[0]

    def verify_setup(self, generate, validate):
        """Verify that the current arguments are sufficient for validation of
        secdat.
        :returns: (Valid?, Error if not valid)
        :rtype: tuple (bool, str)
        """
        # Ensure the correct set of operations is provided
        if not (generate or validate):
            raise RuntimeError('Please specify one or more operations to perform.')

        retval_gen, reterr_gen = True, ''
        retval_val, reterr_val = True, ''
        if generate:
            retval_gen, reterr_gen = self._verify_generate()
        if validate:
            retval_val, reterr_val = self._verify_validate()
        if not (retval_gen and retval_val):
            raise RuntimeError('Setup is not complete. Based on the operation, setup parameters for one of the two:' + '\n'
                               '    ' + reterr_gen + '\n'
                               '    ' + reterr_val)

    def get_version_dir(self, version):
        return 'v' + str(version)

    # This function checks if all fuse in xml are ignored or not
    # Returns True if all fuse in xml are ignored otherwise False
    def check_enable_fuse_entries(self):

        for module in self.data_model.module:
            for fuse_region in module.fuse_region:
                for fuse in fuse_region.fuse:
                    if fuse.ignore is False:
                        return False

        return True

    # This function checks if provided secdat is dummy or not based on the segment information
    # If provided secdat contains no segments then it will be dummy secdat
    def is_dummy_secdat(self, secdat):
        secdatparser = ParseGenSecdat(data=load_data_from_file(secdat),  debug_dir=self.debug_dir, debug_prefix=self.debug_prefix, debug_suffix=self.debug_suffix)
        if secdatparser.has_segment(SEG_TYPE_EFUSE) or secdatparser.has_segment(SEG_TYPE_KEYPROV) or secdatparser.has_segment(SEG_TYPE_ROT):
            return False
        return True

    def generate(self, dummy=False):
        """Generate the secdat file (sec.dat/sec_unsigned.elf) based on the configs which have been setup.

        :param str secdat: the path to the secdat file to be generated.
        """
        if self.check_enable_fuse_entries():
            dummy = True

        logger.info("\n\nGenerating secdat" + "-" * 80 + "\n\n")
        retval_gen, reterr_gen = self._verify_generate()
        if not retval_gen:
            raise RuntimeError(reterr_gen)

        # Get the version from config file
        ucp = self._config_parser
        ucp_file_version = ucp.get_secdat_file_version()
        ucp_fuse_version = ucp.get_secdat_fuse_version()
        ucp_file_info = ucp.get_secdat_file_info()
        self.version_dir = self.get_version_dir(ucp_file_version)

        #--------------------------------------------------------------------------------------------------------
        # Setting input_secdat path:
        #            @ if input given as -s
        #            @ if sec.dat exists in the keyprovision_output
        #--------------------------------------------------------------------------------------------------------
        in_secdat = None

        # Checking self.secdat
        try:
            in_secdat = self.secdat
        except Exception:
            # if control comes here, it means in_secdat=None
            # Checking common secdat
            common_secdat = c_path.join(self.output_dir, defines.COMMON_DIR, self.version_dir, defines.SEC_DAT)
            if os.path.isfile(common_secdat):
                in_secdat = common_secdat

        # In case in_secdat is sec.elf, convert it to sec.dat for subsequent steps to generate feature sec.dat and common sec.dat
        if in_secdat is not None:
            self._set_debug_dir()
            self._set_debug_prefix_suffix(in_secdat)
            if ucp.elf is not None and get_secdat_type(in_secdat) == defines.SECDAT_TYPE_ELF:
                in_secdat=self._get_secdat_from_secelf(in_secdat)

        # Create list of output secdat
        out_secdats = []
        IO = namedtuple('IO', 'in_data out_path')

        out_secdats.append(IO(None, c_path.join(self.output_dir, self.FEATURE_DIR, self.version_dir, defines.SEC_DAT)))
        out_secdats.append(IO(load_data_from_file(in_secdat) if in_secdat else None,
                              c_path.join(self.output_dir, defines.COMMON_DIR, self.version_dir, defines.SEC_DAT)))

        # Delete temporary file if created to extract sec.dat from sec.elf
        self._cleanup_tmp_file(in_secdat)

        # Generate the secdats:
        for secdat in out_secdats:
            out_dir = os.path.dirname(secdat.out_path)
            c_path.create_dir(out_dir)
            secdat_obj = secdat_pg(self.SECDAT_SEGMENT_TYPE, secdat.in_data, dummy=dummy)
            if not dummy:
                secdat_obj.data_model = self.data_model
            secdat_obj.write(secdat.out_path, ucp_file_version, ucp_fuse_version, ucp_file_info, dummy)
            if not dummy:
                dump_debug_data_model(self.data_model, out_dir, self.debug, self.TOOL_NAME)
            dump_debug_secdat(secdat_obj, out_dir, self.debug)
            logger.info('Secdat generated at: ' + secdat.out_path)

        # output secdats
        self.secdat = c_path.join(self.output_dir, self.FEATURE_DIR, self.version_dir, defines.SEC_DAT)
        self.secdat_common = c_path.join(self.output_dir, defines.COMMON_DIR, self.version_dir, defines.SEC_DAT)

        # sec.dat to sec.elf conversion
        if ucp.elf is not None:
            logger.info("Converting sec.dat to sec_unsigned.elf")

            # Get sec.dat images generated above to be converted to elf
            to_be_elf_images = self._get_output_secdat_images(defines.SEC_DAT)

            for secdat in to_be_elf_images:

                # update debug directory, prefix and suffix as per sec.dat signed
                self._set_debug_dir(secdat)
                self._set_debug_prefix_suffix(secdat)

                # Get sec.elf parser object
                secelfparser = ParseGenSecdatElf(segment_data=load_data_from_file(secdat),
                                                 debug_dir=self.debug_dir,
                                                 debug_prefix=self.debug_prefix,
                                                 debug_suffix=self.debug_suffix,
                                                 elf_class=defines.ELF_CLASS_MAP[ucp.elf.elf_class],
                                                 dummy=dummy)

                # Set physical address
                secelfparser.phys_addr = int(ucp.elf.phys_addr, 16)

                # Output unsigned sec.elf data into a file sec_unsigned.elf
                secelf_data = secelfparser.get_data()
                base_path = os.path.split(secdat)
                secelf_unsigned = c_path.join(base_path[0], defines.SEC_ELF_UNSIGNED)
                store_data_to_file(secelf_unsigned, secelf_data)

                # Remove sec.dat generated earlier as sec_unsigned.elf is generated instead
                if os.path.basename(secdat) == os.path.basename(defines.SEC_DAT):
                    os.remove(secdat)

                # Update secdat/out_common attribute to point to sec_unsigned.elf file
                if not self._is_common_dir_entry(secelf_unsigned):
                    self.secdat = secelf_unsigned
                else:
                    self.secdat_common = secelf_unsigned

                # Print/log information
                logger.info('Generated unsigned sec.elf file at: ' + secelf_unsigned + ', date & time: ' + datetime.datetime.now().strftime('%c'))
                secelf_unsigned_repr = c_path.join(os.path.dirname(secelf_unsigned), 'unsigned_secelf_repr.txt')
                store_data_to_file(secelf_unsigned_repr, repr(secelfparser))
                logger.info('Dumped unsigned sec.elf repr at: ' + secelf_unsigned_repr + ', date & time: ' + datetime.datetime.now().strftime('%c'))

    def validate(self, secdat, dummy=False):
        """Validate the given secdat file. If it is ELF, validate its signature as well

        :param str secdat: the path to the secdat file to be validated.
        """
        logger.info('\n\n')
        secdat = c_path.normalize(secdat)
        retval_val, reterr_val = self._verify_validate()
        if not retval_val:
            raise RuntimeError(reterr_val)

        # sec.elf to sec.dat conversion for validation if input secdat is an elf
        if get_secdat_type(secdat) == defines.SECDAT_TYPE_ELF:
            sec_dat = self._get_secdat_from_secelf(secdat)

        if self.is_dummy_secdat(self.secdat):
            dummy = True

        validator = SecDatValidator(oem_config_path=self.oem_config_path,
                                    qti_config_path=self.qti_config_path,
                                    user_config_path=self.user_config_path,
                                    ui_config_path=self.ui_config_path,
                                    sec_dat_path=self.secdat,
                                    config=self.CONFIG_MODULE,
                                    feature=self.SECDAT_SEGMENT_TYPE,
                                    output_dir = c_path.join(self.output_dir, self.FEATURE_DIR),
                                    debug = self.debug,
                                    TOOL_NAME = self.TOOL_NAME,
                                    dummy=dummy)

        validator.validate()

        if self._tmp_secdat_created:
            self._cleanup_tmp_file(sec_dat) # delete temporary file
            self.secdat = secdat  # revert self.secdat to sec.elf after validation

    def process(self, verify_setup=False, generate=False, sign=False, validate=False,
                progress_cb=PROGRESS_CB_PYPASS, dummy=False):
        """Performs the fuse blowing operations specified from the params.

        :param bool verify_setup: Verify that the configuration of the object
            is correct and return. This will ignore any other params.
        :param bool generate: Generate the sec.dat using the config files.
        :param bool sign: Sign the sec.dat using secimage config file.
        :param bool validate: Validate the sec.dat
        :param cb progress_cb: Callback method to get a status update during
            processing.

            Callback method should have this format:
            ::
                def progress_cb(status_string, progress_percent):
                    \"""
                    :param str status_string: the current status.
                    :param int progress_percent: the progress (in percent)
                    \"""
                    ...

        """
        # Verify setup
        if verify_setup:
            self.verify_setup(generate, validate)
            logger.note('The inputs provided (config, cmd args) are valid.')
            return

        # Total number of stages. This is used for progress notification via the
        # callback.
        _PROGRESS_TOTAL_STAGES = 3

        # Start processing images
        progress = ProgressNotifier(1, progress_cb, _PROGRESS_TOTAL_STAGES)

        # Send a progress notification to the toplevel
        progress.status = 'Processing..'
        progress.cur_stage = 0
        progress.push()

        # Generate the secdat
        if generate:
            self.generate(dummy)
            secdat = None
        # Sign the secdat
        if sign:
            if not generate:
                secdat = self.secdat
            self.sign(secdat=secdat)
        progress.push()

        # Validate the secdat
        if validate:
            if not generate:
                secdat = self.secdat
            self.validate_sign(secdat)
            self.validate(self.secdat, dummy) # this is always done only for feature sec.dat
        progress.push()

        # End processing
        progress.complete()

    def sign(self, secdat=None):
        """ Signs given sec ELF file if provided, else signs both feature sec elf and common sec elf
        :params str sec elf_file:
        if secdat is provided, signs the image
        if secdat is not provided, signs both feature unsigned_sec.elf and common unsigned_sec.elf
        """
        logger.info("\n\nSigning sec.elf/sec_unsigned.elf------------------------------------------------------------------\n\n")

        # Initialize SecImageCore
        isc = SecImageCore(debug=self.debug)
        isc.config_path = self.secimage_config

        # sec.elf images to be signed
        if secdat is None:
            secelfs = self._get_output_secdat_images(defines.SEC_ELF_UNSIGNED)
        else:
            secelfs = [secdat]

        # Get the supported sign_id list
        sign_id_list = isc.available_sign_ids

        # Get version directory
        self.version_dir = self._get_version_dir_from_ucp()

        for secelf in secelfs:
            for sign_id in sign_id_list:
                isc.set_image_path(secelf, sign_id)
                if secdat is None:
                    isc.output_dir = os.path.dirname(secelf)
                else:
                    out_dir = c_path.join(self.output_dir, self.FEATURE_DIR, self.version_dir)
                    c_path.create_dir(out_dir)
                    isc.output_dir = out_dir
                isc.image_info_list[0].dest_image.image_dir_ext = ''

                # Process the sec.elf/sec_unsigned.elf
                isc.process(sign=True, val_sign=True)

                # Check if the signing is done or not
                if isc.image_info_list[0].status.sign.state != isc.image_info_list[0].status.sign.SUCCESS:
                    raise RuntimeError('Failed to sign the sec.elf file: ' + isc.image_info_list[0].image_under_operation)
                logger.info('Signed sec.elf file at: ' + isc.image_info_list[0].image_under_operation)

                if not self._is_common_dir_entry(isc.image_info_list[0].image_under_operation):
                    self.secdat = isc.image_info_list[0].image_under_operation
                else:
                    self.secdat_common = isc.image_info_list[0].image_under_operation

    def validate_sign(self, secdat=None):
        """Validate sec.elf file.
            :param: str secdat
            if secdat is provided, just validate signature for it
            if secdat is not provided, find both feature sec elf and common elf and validate both
        """
        # Get the images to be validated
        to_be_validated_images = []
        if secdat is None:
            to_be_validated_images = self._get_output_secdat_images(defines.SEC_ELF_SIGNED)
        else:
            to_be_validated_images.append(secdat)
        # proceed only if secdat is elf file
        if get_secdat_type(to_be_validated_images[0]) == defines.SECDAT_TYPE_ELF:
            logger.info("\n\nValidating Signature" + "-"*80 + "\n\n")
            self.secdat = to_be_validated_images[0]
            # Initialize SecImageCore
            isc = SecImageCore(debug=self.debug)
            try:
                isc.config_path = self.secimage_config

                # Get the supported sign_id list
                sign_id_list = isc.available_sign_ids

                for secelf in to_be_validated_images:

                    # Set debug dir and debug prefix, suffix
                    self._set_debug_dir(secelf)
                    self._set_debug_prefix_suffix(secelf)

                    logger.info('\nValidating signature for the image ' + secelf)
                    isc.set_image_path(secelf, sign_id_list[0])
                    if secdat is None:
                        isc.output_dir = os.path.dirname(secelf)
                    else:
                        out_dir = c_path.join(self.output_dir, self.FEATURE_DIR, self.version_dir)
                        c_path.create_dir(out_dir)
                        isc.output_dir = out_dir
                    isc.image_info_list[0].dest_image.image_dir_ext = ''

                    # Process the signed elf
                    isc.process(val_sign=True)

                    if isc.image_info_list[0].status.validate_sign.state != isc.image_info_list[0].status.sign.SUCCESS:
                        logger.error('Failed to validate signature of sec elf file: ' + isc.image_info_list[0].image_under_operation)
                    else:
                        logger.info('Successfully validated signature of sec elf file at: ' + isc.image_info_list[0].image_under_operation)
            except:
                # if the control comes here it means secimage_config is not set which is possible for unsigned image is generated and validated
                logger.warning("Signature validation will be skipped because --secimage_config has not been provided")

    def _get_secdat_from_secelf(self, secelf):
        """Gets sec.dat from given sec.elf in a temporary file.
            :param: str secelf file
            if secelf=sec.elf, returns
            if secelf=sec.dat, exception occurs
            Returns sec.dat. Also updates self.secdat with the same if successful
        """
        ucp = self._config_parser
        secelfparser = ParseGenSecdatElf(data=load_data_from_file(secelf),
                                         debug_dir=self.debug_dir,
                                         debug_prefix=self.debug_prefix,
                                         debug_suffix=self.debug_suffix,
                                         elf_class=defines.ELF_CLASS_MAP[ucp.elf.elf_class])
        logger.debug("Extracting sec.dat from sec.elf")
        secdat_content = secelfparser.elfparser.segments[secelfparser.elfparser.phdrs[SECDAT_SEGMENT_INDEX]]
        ret = c_path.create_tmp_file(secdat_content)
        self._tmp_secdat_created = True
        self.secdat = ret
        return ret

    def _cleanup_tmp_file(self, tmp_file_path):
        if self._tmp_secdat_created:
            c_path.remove_tmp_file(tmp_file_path)
            self._tmp_secdat_created = False

    def _set_debug_dir(self, secdat=None):
        """sets debug dir path.
            :param: str sedat file (could be sec.dat/sec.elf)
            if secdat is provided, debug dir is set to same dir as secdat
            if secdat is not provided, debug dir is set feature dir
        """
        if self.debug:
            if secdat is None:
                self.debug_dir = c_path.join(self.output_dir, self.FEATURE_DIR, self.version_dir, defines.DEST_DEBUG_DIR)
            else:
                self.debug_dir = c_path.join(os.path.dirname(secdat), defines.DEST_DEBUG_DIR)

    def _set_debug_prefix_suffix(self, secdat):
        """sets debug prefix and suffix.
            :param: str sedat file (could be sec.dat/sec.elf)
            if secdat is provided, debug prefix and suffix from it
            if secdat is not provided, debug prefix and suffix is created using self.secdat
        """
        if secdat is None:
            secdat = self.secdat
        if secdat is not None:
            self.debug_prefix, self.debug_suffix = os.path.splitext(os.path.basename(secdat))

    def _is_common_dir_entry(self, path):
        """Checks if given path is.common dir or not.
            :param: str path (could be sec.dat/sec.elf)
            if common dir, returns True
            if not common dir, returns False
        """
        split = os.path.split(path)
        split_again = os.path.split(split[0])
        if os.path.basename(split_again[0]) == defines.COMMON_DIR:
            return True
        else:
            return False

    def _get_version_dir_from_ucp(self):
        """Queries sec.dat/sec.elf file version from config and returns version dir name.
        """
        ucp = self._config_parser
        ucp_file_version = ucp.get_secdat_file_version()
        version_dir = self.get_version_dir(ucp_file_version)
        return version_dir

    def _get_output_secdat_images(self, name):
        """Gets list output paths of sec.dat/sec.elf for both feature dir and common dir based on name
            :param: str name - name of the final image
            returns list of containing output paths of sec.dat/sec.elf for both feature dir and common dir based on name
        """
        if self.secdat and self.secdat_common:
            return [self.secdat, self.secdat_common]
        else:
            secdats = []
            feature_secdat = c_path.join(self.output_dir, self.FEATURE_DIR, self.version_dir, name)
            if os.path.exists(feature_secdat):
                secdats.append(feature_secdat)
            common_secdat = c_path.join(self.output_dir, defines.COMMON_DIR, self.version_dir, name)
            if os.path.exists(common_secdat):
                secdats.append(common_secdat)
            return secdats


#===========================================
# Common utility functions
#===========================================
def get_secdat_type(secdat):
    try:
        ParseGenSecdat(data=load_data_from_file(secdat))
        secdat_type = defines.SECDAT_TYPE_BIN
    except:
        try:
            ParseGenSecdatElf(data=load_data_from_file(secdat), elf_class=ELFCLASS32)
            secdat_type = defines.SECDAT_TYPE_ELF
        except:
            try:
                ParseGenSecdatElf(data=load_data_from_file(secdat), elf_class=ELFCLASS64)
                secdat_type = defines.SECDAT_TYPE_ELF
            except:
                raise RuntimeError("Invalid secdat image found at ".format(secdat))
    return secdat_type


