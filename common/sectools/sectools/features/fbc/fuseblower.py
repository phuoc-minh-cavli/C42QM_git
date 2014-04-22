# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Provides a command line interface to the services provided by fbc.

.. data:: FUSEBLOWER_TOOL_NAME

    Name of the tool

.. data:: FUSEBLOWER_TOOL_VERSION

    Version of the tool

.. data:: DEF_FUSEBLOWER_OUTPUT_DIR_NAME

    Name of the default output directory

.. data:: DEF_FUSEBLOWER_OUTPUT_DIR_PATH

    Absolute path of the default output directory
"""

import sys
import traceback

from sectools.common.utils import c_path
from sectools.common.utils.c_base import dynamicToolStatus
from sectools.common.utils.c_logging import logger
from sectools.features.fbc.fbc import FuseBlowerCore, RootOfTrustCore
from sectools.features.fuse_common.fuse_common import FuseCommonOptionParser, log_to_file

# Tool name for command arg
CMD_ARG_TOOL_NAME = 'fuseblower'

# Name & version of the tool
FUSEBLOWER_TOOL_NAME = 'FuseBlower'
FUSEBLOWER_TOOL_VERSION = '4.0'

# Path definitions
DEF_FUSEBLOWER_OUTPUT_DIR_PATH = dynamicToolStatus.toolDir

__version__ = FUSEBLOWER_TOOL_NAME + ' ' + FUSEBLOWER_TOOL_VERSION

# Modes of tool operation
FB_MODE_SECBOOT = 'secboot'
FB_MODE_ROT = 'rot'

FB_MODE_HANDLER = \
{
    FB_MODE_SECBOOT : FuseBlowerCore,
    FB_MODE_ROT : RootOfTrustCore,
}


class FuseBlowerOptionParser(FuseCommonOptionParser):
    """Parses and validates the command line arguments specific to FuseBlower."""

    _LIST_TAG = 'LIST'

    @property
    def c_description(self):
        """(str) Returns the description of the program."""
        return 'This program generates & validates the sec.dat/sec.elf file.'

    @property
    def c_version(self):
        """(str) Returns the version of the program."""
        return __version__

    def c_add_options(self):
        """Adds the command line args supported by fuseblower."""
        # Generating secdat
        gen_group = self.add_option_group('Generating sec.dat/sec.elf with or without sec.dat/sec.elf as input')
        gen_group.add_option('-u', '--user_config_path', metavar='<file>',
                             help='path to the user config file.')
        gen_group.add_option('-e', '--oem_config_path', metavar='<file>',
                             help='path to the oem config file.')
        gen_group.add_option('-q', '--qti_config_path', metavar='<file>',
                             help='path to the qti config file.')
        gen_group.add_option('-s', '--secdat', metavar='<file>',
                             help='path to the sec.dat/sec.elf file as input for common_output or for validation or signing/re-signing.')
        gen_group.add_option('-p', '--platform', metavar='<platform>',
                             help='specify the platform, to get the config files as input related to platform')

        # Signing sec.elf
        sign_group = self.add_option_group('Signing/Validating sec.elf')
        sign_group.add_option('-c', '--secimage_config_path', metavar='<file>',
                             help='path to the fuseblower secimage config file.')


        # Specifying the output location
        output_group = self.add_option_group('Specifying output location')
        output_group.add_option('-o', '--output_dir', metavar='<dir>',
                                help='directory to store output files. DEFAULT: "./"',
                                default=DEF_FUSEBLOWER_OUTPUT_DIR_PATH)

        # Specifying the mode
        mode_group = self.add_option_group('Specifying the mode of operation')
        mode_group.add_option('-m', '--mode', metavar='<mode>',
                              help='fuseblower mode. Available modes: ' + str(FB_MODE_HANDLER.keys()) + '. DEFAULT: ' + FB_MODE_SECBOOT,
                              default=FB_MODE_SECBOOT)

        # Specifying the operation
        operations_group = self.add_option_group('Operations')
        operations_group.add_option('-g', '--generate', action='store_true',
                                    default=False, help='1. generates the sec.dat/sec.elf file using config files and the cmd is  "-u, -e, -q, -g" '
                                                        '2. generates the sec.dat/sec.elf file using config files & input sec.dat/sec.elf and the cmd is "-u, -e, -q, -g, -s" '
                                                        '3. generates the sec.dat/sec.elf file using platform and the cmd is "-p <platform>" '
                                                        '4. generates the sec.dat/sec.elf file using platform & input sec.dat/sec.elf and the cmd is "-p <platform> -s')
        operations_group.add_option('--dummy', action='store_true',
                                    default=False,
                                    help='generate a sec.elf file which will not result in any fuses being blown on device')
        operations_group.add_option('--sign', action='store_true',
                                    default=False,
                                    help='sign the signed/unsigned sec.elf file')
        operations_group.add_option('-a', '--validate', action='store_true',
                                    default=False,
                                    help='validate the sec.dat/sec.elf file and the cmd is "-u, -e, -q, -a, -s')
        operations_group.add_option('-l', '--verify_inputs', action='store_true',
                                    default=False,
                                    help='verify the command line options.')

    def c_validate(self):
        """Validates the command line args provided by the user.

        :raises: RuntimeError
        """
        args = self.parsed_args
        err = []

        # Check mode is correct
        if args.mode not in FB_MODE_HANDLER.keys():
            err.append('Invalid mode selected: "' + args.mode + '". Available modes of operation are: ' + str(FB_MODE_HANDLER.keys()))

        if not args.platform:
            # Check the config paths
            if not (args.user_config_path and args.oem_config_path and args.qti_config_path):
                err.append('Provide either config files using '
                           '--user_config_path, --oem_config_path, '
                           '--qti_config_path or provide the platform details using -p <platform> ')

        if args.dummy and not args.generate:
            err.append('--dummy option can only be used with --generate option')

        if args.platform and (args.oem_config_path or args.qti_config_path or args.user_config_path or args.secimage_config_path):
            err.append('Provide either config files or platform as a input. '
                       'Do not provide both types of input for generation of sec.dat/sec.elf')

        err += self.c_validate_common_params() + self.c_validate_paths()
        self.c_raise_errors(err)


def print_summary():
    """Prints the summary of the actions performed by FuseBlower"""
    pass


def main(args):
    """Parses the command line arguments, performs any basic operations based on
    the parsed arguments and starts processing using the appropriate module.
    """
    # Log to file
    log_to_file(c_path.join(args.output_dir, FB_MODE_HANDLER[args.mode].FEATURE_DIR), FUSEBLOWER_TOOL_NAME)

    # Print the tool's launch command
    logger.debug('\n\n    FuseBlower launched as: "' + ' '.join(sys.argv) + '"\n')

    # Initialize Core
    core = FB_MODE_HANDLER[args.mode](debug=args.debug)

    # Configure FuseBlowerCore
    if args.oem_config_path or args.qti_config_path:
        core.set_config_paths(oem_config_path=args.oem_config_path,
                              qti_config_path=args.qti_config_path,
                              user_config_path=args.user_config_path,
                              secimage_config_path=args.secimage_config_path,
                              dummy=args.dummy)
    elif args.platform:
        core.set_config_paths(*core._config_dir_obj.get_chipset_mode_config(args.platform, args.mode), dummy=args.dummy)

    if args.secdat:
        core.secdat = args.secdat

    # Set the output dir
    core.output_dir = args.output_dir

    # Process
    core.process(verify_setup=args.verify_inputs,
                 generate=args.generate,
                 sign=args.sign,
                 validate=args.validate,
                 dummy=args.dummy)

    # Print summary of operations
    print_summary()


def parse_args(argv):
    return FuseBlowerOptionParser(argv).parsed_args

if __name__ == '__main__':
    try:
        # Call the main of the tool
        main(parse_args(sys.argv))

    except Exception:
        logger.debug(traceback.format_exc())
        logger.error(sys.exc_info()[1])
        sys.exit(1)

    except KeyboardInterrupt:
        print
        logger.error('Keyboard Interrupt Received. Exiting!')
        sys.exit(1)

    sys.exit(0)


