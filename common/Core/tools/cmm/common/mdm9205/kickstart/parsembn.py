import os, sys
import getopt
import time,struct
import re
import binascii
from types import *

def Read4Bytes():

    print Bytes

def ReturnSizeString(size):
    if size>(1024*1024*1024):
        return "%.2fGB" % (size/(1024.0*1024.0*1024.0))
    elif size>(1024*1024):
        return "%.2fMB" % (size/(1024.0*1024.0))
    elif size>(1024):
        return "%.2fKB" % (size/(1024.0))
    else:
        return "%iB" % (size)
    
def PrintValue(value,message,explain,size=False):
    if size is True:
        print "%s= (0x%.8X) %10s   %10i <-- %s" % (message,value,ReturnSizeString(value),value,explain)
    else:
        print "%s= (0x%.8X) %10s   %10i <-- %s" % (message,value,"",value,explain)
    #print Bytes
    #print " "
    
def Read4BytesFromFile(ipfile):
    bytes_read = ipfile.read(4)
    Bytes = struct.unpack("%dB" % len(bytes_read),bytes_read)        
    value = Bytes[0] | Bytes[1]<<8 | Bytes[2]<<16 | Bytes[3]<<24
    return value

## Where was this script started
dir = os.getcwd()

if dir[-1] == "/" or dir[-1] == "\\": ## makes it have to have an anchor at the end
    dir = dir[:-1]  ## Take the left of what matched - get rid of slash   

dir.replace("\\", "/")
#dir.replace("\", "/")

dir = "%s/" % dir

print "Script was started in '%s'\n" % dir

PathToQDST = sys.path[0]

print "\nChanging to sys.path[0]=",PathToQDST
os.chdir(PathToQDST)
print "I'm currently in ",os.getcwd()

if len(sys.argv)<=1:
    print "need path/filename to parse mbn"
    sys.exit(1)

FILE = sys.argv[1]

if os.path.exists(FILE) is False:
    print "\nERROR: The file you specified '%s' does not exist" % (FILE)
    sys.exit(1)

filesize = os.path.getsize(FILE)
print "\n%s is of size %i\n" % (FILE,filesize)

if filesize == 0:
    print "\n\nFilesize of mbn is 0, can't continue without this file EXITING"
    sys.exit(1)


try:
    ipfile = open(FILE, "r+b")
except:
    print "ERROR: Could not open %s, cwd=%s" % (FILE,os.getcwd() )
    sys.exit()

value = Read4BytesFromFile(ipfile)

FortyByteHeader = True
ElfHeader       = False

if value == 1179403647:
    ElfHeader       = True
    # I must skip this 4KB elf header to get to the MBN header
    try:
        ipfile.seek(4096)
    except Exception, x:
        print "Could not move to location 4096 in file"
        print "REASON: %s" % (x)
        sys.exit(1)
    
    value = Read4BytesFromFile(ipfile)

if value>100 or value==0:
    FortyByteHeader = False
    
if FortyByteHeader is True:
    PrintValue( value,                     "image_id\t","arbitrary number chosen")
    PrintValue( Read4BytesFromFile(ipfile) ,"header_vsn_num\t","current header version")
    PrintValue( Read4BytesFromFile(ipfile) ,"image_src\t","Offset of image from location 0 in bytes")
    PrintValue( Read4BytesFromFile(ipfile) ,"image_dest_ptr\t","Pointer to location to store in RAM")
    PrintValue( Read4BytesFromFile(ipfile) ,"image_size\t","Size of Image+Signature+Certs",True)
    PrintValue( Read4BytesFromFile(ipfile) ,"code_size\t","Size of Image",True)
    PrintValue( Read4BytesFromFile(ipfile) ,"signature_ptr\t","Offset of signature from end of header")
    PrintValue( Read4BytesFromFile(ipfile) ,"signature_size\t","Size of the signature in bytes",True)
    PrintValue( Read4BytesFromFile(ipfile) ,"cert_chain_ptr\t","Offset of cert chain from  end of header")
    PrintValue( Read4BytesFromFile(ipfile) ,"cert_chain_size\t","Size of the cert chain in bytes",True)

else:
    PrintValue( value,                      "codeword\t","defining flash type info")
    PrintValue( Read4BytesFromFile(ipfile) ,"magic\t\t","magic number")
    PrintValue( Read4BytesFromFile(ipfile) ,"image_id\t","arbitrary number chosen")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_1\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_2\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"image_src\t","Offset of image from location 0 in bytes")
    PrintValue( Read4BytesFromFile(ipfile) ,"image_dest_ptr\t","Pointer to location to store in RAM")
    PrintValue( Read4BytesFromFile(ipfile) ,"image_size\t","Size of Image+Signature+Certs",True)
    PrintValue( Read4BytesFromFile(ipfile) ,"code_size\t","Size of Image",True)
    PrintValue( Read4BytesFromFile(ipfile) ,"signature_ptr\t","Offset of signature from end of header")
    PrintValue( Read4BytesFromFile(ipfile) ,"signature_size\t","Size of the signature in bytes",True)
    PrintValue( Read4BytesFromFile(ipfile) ,"cert_chain_ptr\t","Offset of cert chain from  end of header")
    PrintValue( Read4BytesFromFile(ipfile) ,"cert_chain_size\t","Size of the cert chain in bytes",True)
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_3\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_4\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_5\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_6\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_7\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_8\t","RESERVED")
    PrintValue( Read4BytesFromFile(ipfile) ,"reserved_9\t","RESERVED")


ipfile.close()

print ""

if ElfHeader is True:
    print "This file has an ELF header. The MBN header was read from location 4096"
if FortyByteHeader is True:
    print "This file has a 40 byte MBN header\n"
else:
    print "This file has an 80 byte MBN header\n"

#Write4BytesToFile(opfile,"image_id",27)                     # image_id - Identifies the type of image this header represents (OEM SBL, AMSS, Apps boot loader, etc.).
#Write4BytesToFile(opfile,"header_vsn_num",3)                # Header version number
#Write4BytesToFile(opfile,"image_src",40)                    # Location of image in flash: 
#Write4BytesToFile(opfile,"image_dest_ptr",0)                # Pointer to location to store image in RAM. Also, entry point at which image execution begins.
#Write4BytesToFile(opfile,"image_size",size+SigSize+CertChainSize)   # Size of complete image in bytes 
#Write4BytesToFile(opfile,"code_size",size)                  # Size of code region of image in bytes 
#Write4BytesToFile(opfile,"signature_ptr",size)              # Pointer to images attestation signature 
#Write4BytesToFile(opfile,"signature_size",SigSize)          # Size of the attestation signature in bytes 
#Write4BytesToFile(opfile,"cert_chain_ptr",size+SigSize)     # Pointer to the chain of attestation certificates associated with the image
#Write4BytesToFile(opfile,"cert_chain_size",CertChainSize)   # Size of the attestation chain in bytes 

