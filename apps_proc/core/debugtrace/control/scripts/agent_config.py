#!/usr/bin/env python

import getopt, sys
import mmap
import os
import re

def usage():
    print """
Usage: python agent_config.py [-c config_file] <apps_image>

Display or modify debug agent startup configuration in an APPS binary image.
If no options are specified current configuration will be displayed.
If no arguments are specified 'apps.mbn' is used as apps_image.

           -c        The startup configuration is replaced with
                     the given config file

           -n        Do not strip comment lines in debug agent config.
                     By default it is stripped.
                     """



class ArgsParser(object):
    start_marker='#DEBUG_AGENT_CONFIG_BUFFER_START\n'
    end_marker='#DEBUG_AGENT_CONFIG_BUFFER_END\n'
    log_level = 0

    def __init__(self):
        self.apps_image='apps.mbn'
        self.strip_comments=True
        self.config_file=''
        self.parse_args()

    def parse_args(self):
        opts, args = getopt.getopt(sys.argv[1:], "c:n")
        for o, a in opts:
            if o == "-c":
                self.config_file=a
            elif o == "-n":
                self.strip_comments=False
           
        self.apps_image = "apps.mbn" if len(args) < 1 else args[0]
        if (not os.path.isfile(self.apps_image)):
            raise Exception('Cannot open apps_image file %s' % self.apps_image)



if __name__ == '__main__':
    try:
        args=ArgsParser()
        with open(args.apps_image, 'r+b') as f:

            #Find debug agent config buf
            mm = mmap.mmap(f.fileno(), 0)
            start=mm.find(args.start_marker)
            if (-1==start):
                raise Exception('Cannot find debug agent config buf start marker')
            start= start + len(args.start_marker)

            end=mm.find(args.end_marker,start)
            if (-1==end):
                raise Exception('Cannot find debug agent config buf end marker')
            da_buf_size=end-start

            if ((da_buf_size <=0) or (da_buf_size >=65336)):
                raise Exception('Unusual debug agent buffer size[%d bytes]' % da_buf_size)


            #Apply new configuration if specified
            script=''
            if args.config_file:
                with open(args.config_file,'r') as sf:
                    script=sf.read()
                script_size=len(script)
                if (script_size < da_buf_size):
                    script_end=start+script_size
                    mm[start:script_end]=script
                    #Erase the rest
                    mm[script_end:end]= '#' * (da_buf_size - script_size)
                    for n in range(script_end+1,end,60):
                        mm[n]='\n'
                    mm[end-1]='\n'
                    
                else:
                    raise Exception('Config file %s [%d bytes] cannot fit in debug agent config buffer [%d bytes]' % (args.config_file,script_size,da_buf_size))

            #Display the updated debug agent configuration
            da_buf=mm[start:end]
            print 'Debug Agent Startup Configuration'
            print '---------------------------------'
            if args.strip_comments:
                comment_pattern=re.compile('#.*$\n',re.MULTILINE)
                print comment_pattern.sub('',da_buf)
            else:
                print da_buf
            
    except getopt.GetoptError, err:
        print str(err) 
        usage()
        sys.exit(2)
    except Exception as e:
        print e

