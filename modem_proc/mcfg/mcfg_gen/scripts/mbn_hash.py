#==============================================================================
#  $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/scripts/mbn_hash.py#1 $
#  $DateTime: 2020/02/19 23:07:48 $
#  $Author: pwbldsvc $
#  $Change: 22644654 $
#==============================================================================
#*
#* Copyright (c) 2016 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#==============================================================================
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/18/15   kushk      Create
#
#==============================================================================


import hashlib
import sys
BLOCKSIZE = 65536
def generate_hash(filename):
    with open(filename, 'rb') as f:
        m = hashlib.sha256()
        buf = f.read(BLOCKSIZE)
        while len(buf) > 0:
            m.update(buf)
            buf = f.read(BLOCKSIZE)
        sha_bin = m.digest()
    print (sha_bin)
    
if __name__ == "__main__":
    src = sys.argv[1]
    generate_hash(src)
