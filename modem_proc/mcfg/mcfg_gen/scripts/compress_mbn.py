#==============================================================================
#  $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/scripts/compress_mbn.py#1 $
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
# 6/27/16   asandstr  Create
#
#==============================================================================


import zlib
import sys
def compress_mbn(filename,dest):
    s2 = None
    with open(filename, 'rb') as fn:
        s1 = fn.read() # BINARY mode
        s2 = zlib.compress(s1)

    # Replace dest with compressed contents
    if dest: 
        with open(dest, 'wb') as fn_zip:
            fn_zip.write(s2)
    else:
      # Avoid use of print statement to ensure carriage returns are not appended to output
      sys.stdout.write(s2)
      sys.stdout.flush()  # To ensure stdout is immediately flushed
    
if __name__ == "__main__":
    src = sys.argv[1]
    dest = None
    if len(sys.argv) > 2:
        dest = sys.argv[2]
    compress_mbn(src,dest)
