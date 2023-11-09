#=============================================================================
#
#                                    qurt_config.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
#!/usr/bin/env python

class qurt_config:
    def genheader_subcommand(self, arglist):
        from lib.genheader import genheader_cmd
        return genheader_cmd(arglist)
    def genkernel_subcommand(self, arglist):
        from lib.genkernel import genkernel_cmd
        return genkernel_cmd(arglist)
    def update_subcommand(self, arglist):
        from lib.merge import merge_cmd
        return merge_cmd(arglist)
    def usage(self):
        cmds = sorted([z.rsplit('_',1)[0] for z in dir(self) if z.endswith('_subcommand')])
        str = 'First argument must be one of:\n  ' + ', '.join(cmds)
        raise Exception(str)
    def run_command(self, argv):
        from traceback import format_exc as tbstr
        progname = argv[0]
        try:
            print ' '.join(argv)
            raw_args = argv[1:]
            args = [s for s in raw_args if not s == '--traceback']
            if args == raw_args:
                tbstr = None
            try:
                subfunc = getattr(self, '%s_subcommand' % args[0])
            except StandardError:
                self.usage()
            return subfunc(args[1:])
        except (SystemExit, KeyboardInterrupt):
            raise
        except Exception, err:
            import sys
            if tbstr:
                print >> sys.stderr, tbstr()
            print >> sys.stderr, '%s: Error:\n*** %s' % (progname, err)
        except:
            raise
        return 1
    def main(self):
        import sys
        sys.exit(self.run_command(sys.argv))

def attach_config_vars(cfg):
    #
    #  This function is called by the kernel build procedure
    #   to get access to the configuration variable database.
    #
    import os
    import imp
    lib = imp.load_module('lib',*imp.find_module('lib',[os.path.dirname(__file__)]))
    from lib.genkernel import QurtVars
    QurtVars(cfg)

if __name__ == '__main__':
    qurt_config().main()    # Never returns

# Signatures of the files that this depends on
# 052be90a17634dc6a80c1283d5b4b14b /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/cust_config_template.c
# 0ddf20cbb7065ef0abdd7c98f1de87c7 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/default_build_config.def
# 624174a9dc7c550e21bdb27da4fd160a /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/static_build_config.def
# 5bdb8cd95cf82507b4b103ed57bad354 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/qurt_tlb_unlock.xml
# b4be7c533cc89c3952ef6014d8c5cf67 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/qurt_default.lcs
# 51389469219be118b3971c1ec6638da1 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/qurt_whitelist.txt
# ea706f2613f4343bac4875af4b5850c5 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/qdi_init_template.c
# 82fc44b8836571a8c447b8f56aec59d9 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/__init__.py
# df95cfea7f7213272a0c81d88cce6d91 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/basefile.py
# 37273d2e962e1a73176b92c08210e2f0 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/build_qurt_config.py
# 55dbbf6bfbb4d869adbf951edf632611 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/build_xml.py
# f1b458a69f39974a3e8a9e63bc56f02d /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/cfgvars.py
# 840ddad5f700e28e26186c3a6ad48758 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/elf_info_patch.py
# 857cfdbdbcb81b388444d7d47a4bd916 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/ezxml.py
# 9abba5ff04b9d874acb683fe26123b2b /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/genheader.py
# 4e79327e51e36fcd5df84df0eca744ce /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/genkernel.py
# c083132b08dbc9fe4f0e2ad37e4cab81 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/interrupt_xml.py
# a265895ea8c931aeeabd45fd06008524 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/ecc_xml.py
# bcc7c4f580db9f4938f3571a8febfc43 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/kernel_xml.py
# f25b832c996f766f5c536d582879add2 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/l2cfg_xml.py
# 3ee4d31adec6bf419d6cc1536039001f /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/machine_xml.py
# faa356cf7e46cd1096f7776d4b4e7aea /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/memsection_xml.py
# 2bf1b965356d740d21c09dd18e1b146a /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/merge.py
# 3d5008ec9d0bc9b925dc044210fbdc25 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/parse_build_params.py
# afb36c8d258364d2f190f5bd2eaee94f /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/parse_spec.py
# 46f893f06e21a958de1a7a35c96bb2ee /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/physpool_xml.py
# 6b8f94c54a43b2a7fae1d609834dd874 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/program_xml.py
# ca6baa8f299f113b3f93a8cb4c1f45ec /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/qiblog.py
# 997b71770902dfa65472b7937fdc4cd9 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/qurt.py
# e71c9b3c76488ecfee06a27e40f1c657 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/StaticSyscfgComposer.py
# bd80403bcebd1926c5ad5611b929499f /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/build_params.txt
# 052be90a17634dc6a80c1283d5b4b14b /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/Input/cust_config.c
# 14ce34b304d560171fa46c99bf7a1ffe /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/qurt-image-build.py
# d3d8093112c5075fd639927e80fdb3c4 /local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/kernel/qurt/build/pageonly/qdsp6/9205.ltenbge.prod/install/modemv67X_512/scripts/lib/qurt_consts.py
