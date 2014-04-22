#!/usr/bin/env python
#
# EFS restrictions require that qdss debug agent startup file
# be exactly 3000 bytes. This script pads a smaller file with
# comment lines to make it exactly 3000 bytes
#
import sys
import os

PAD_TO_SIZE=3000

try:
    config_file=sys.argv[1] if (len(sys.argv)>=2) else 'agent_cfg_def.txt'
    stat=os.stat(config_file)
    if (stat.st_size > PAD_TO_SIZE):
        raise Exception('%s(%d bytes) larger than %d bytes' % (config_file,stat.st_size,PAD_TO_SIZE))


    with open(config_file,"a") as f:
        comment_line='#' * 30 + '\n'
        for i in range(100):
            f.write(comment_line)
        f.truncate(PAD_TO_SIZE)
    
    stat=os.stat(config_file)
    print('%s : size %d bytes' % (config_file,stat.st_size))
    exit(0)
except Exception as e:
    print(e)
    exit(1)
