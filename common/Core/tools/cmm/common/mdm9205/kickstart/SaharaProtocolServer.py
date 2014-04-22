#!/usr/bin/python

import shutil
import struct, os, sys, getopt
from time import sleep, time
import subprocess as sub
from time import strftime, localtime, sleep
from os.path import getsize

import os, time
from stat import * # ST_SIZE etc

from sps_common import *
import sps_common

from xml.etree import ElementTree as ET
#from elementtree.ElementTree import ElementTree
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from xml.dom import minidom

from datetime import date, timedelta

print "You are running Python version",sys.version
m = re.search(r"64 bit", sys.version)
if type(m) is not NoneType:
	print "\nERROR: You are running a 64 bit version of Python - This is *not* supported and will fail"
	print "\nERROR: You are running a 64 bit version of Python - This is *not* supported and will fail"
	print "\nERROR: You are running a 64 bit version of Python - This is *not* supported and will fail"
	print "\nPlease install the Qualcomm supported 32 bit Python 2.5.2\n"
	sys.exit(1)

m = re.search(r"2.5.2", sys.version)
if type(m) is NoneType:
	print "\nWARNING: You are not running the Qualcomm supported 32 bit Python 2.5.2"
	print "\nWARNING: You are not running the Qualcomm supported 32 bit Python 2.5.2"
	print "\nThis script might fail. If so, please install the Qualcomm supported 32 bit Python 2.5.2\n"
	sleep(4)


PerformStep2 = 1

Images = {'21':"sbl1.mbn", '25':"tz.mbn", '23':"rpm.mbn", '31':"mba.mbn", '8':"qdsp6sw.mbn", '6':"apps.mbn", '16':"efs1.bin", '17':"efs2.bin", '20':"efs3.bin", '29':"acdb.mbn"}
ImagesToSend = [6,8,16,17,20,21,23,25,29,31]

XMLFile = "SaharaProtocolServer.xml"
HashInstructions       = {}

verbose                 = True
SEARCH_CWD_LAST         = True
search_paths            = []
DidStuff                = False

ks_path             = ""

COMPORT_DLOAD       = ""  # "/dev/ttyHSL1"
COMPORT_SAHARA      = ""  # "/dev/ttyUSB0"
COMPORT_EFSSYNC     = ""  # "/dev/ttyUSB0"
PATHTOIMAGES        = "/system/etc/firmware/"
PATHFORMDMUPLOADS   = "/tombstones/mdm/"
EFSRAW1             = "/dev/block/mmcblk0p19"
EFSRAW2             = "/dev/block/mmcblk0p20"
EFSRAW3             = "/dev/block/mmcblk0p21"
RX_Timeout          = 5
ForceFileUpload     = ""
Verbose             = ""
SkipSerialNumberStep= False

serial_number       = "0"
hw_id               = ""

def AlignVariablesToEqualSigns(sz):
    temp = re.sub("(\t| )+=","=",sz)
    temp = re.sub("=(\t| )+","=",temp)
    return temp

def ReturnArrayFromSpaceSeparatedList(sz):
    temp = re.sub("\s+|\n"," ",sz)
    temp = re.sub("^\s+","",temp)
    temp = re.sub("\s+$","",temp)
    return temp.split(' ')

def ParseXML(XML_Filename):
    global COMPORT_DLOAD,COMPORT_SAHARA,ks_path,PATHTOIMAGES,EFSRAW1,EFSRAW2,EFSRAW3,RX_Timeout,PATHFORMDMUPLOADS,serial_number,hw_id,ForceFileUpload

    #import pdb; pdb.set_trace()

    if os.path.exists(XML_Filename) is False:
        device_log("\nERROR: The XML file you specified '%s' does not exist" % (XML_Filename),1)
        sys.exit(1)

    xml_root = ET.parse(XML_Filename)
    try:
        xml_iter = xml_root.iter()
    except AttributeError:
        xml_iter = xml_root.getiterator()
    for xml_element in xml_iter:
        print xml_element.tag
        if xml_element.tag == 'options':
            temp=xml_element.text.split('\n')
            temp=[x.replace(' ', '').split('=') for x in temp if len(x.strip())!=0]
            for x in temp:
                if x[0].lower() == "VERBOSE_FEEDBACK".lower() and x[1].lower()=="true".lower():
                    verbose = True
                elif x[0].lower() == "COMPORT_DLOAD".lower():
                    COMPORT_DLOAD = x[1]
                elif x[0].lower() == "COMPORT_SAHARA".lower():
                    COMPORT_SAHARA = x[1]
                elif x[0].lower() == "serial_number".lower():
                    serial_number = x[1]
                elif x[0].lower() == "hw_id".lower():
                    hw_id = x[1]
                elif x[0].lower() == "PATHTOIMAGES".lower():
                    PATHTOIMAGES = x[1]
                elif x[0].lower() == "PATHFORMDMUPLOADS".lower():
                    PATHFORMDMUPLOADS = x[1]
                elif x[0].lower() == "ForceFileUpload".lower():
                    if x[0].lower() == "0":
                        ForceFileUpload = ""
                    else:
                        ForceFileUpload = " -m" ## space is important here
                elif x[0].lower() == "timeout".lower():
                    RX_Timeout= int(x[1])
        elif xml_element.tag == 'search_paths':
            temp=xml_element.text.split('\n')
            temp=[x.strip().lower() for x in temp if len(x.strip())!=0]
            search_paths.extend(temp)

def usage():
    print """
Usage: python SaharaProtocolServer.py [OPTION...]

Semi-simulates FUSION with an MDM from a PC - Specifically the SAHARA protocol part

 Options:
  -d, --dload                  COM/serial Port to use with any DLOAD transfer
  -p, --port                   COM/serial Port - main COM port to use
  -f, --PATHTOIMAGES           HEX file to transfer over DLOAD (MPRG8660.hex)
  -i, --image                  image to transfer (8660_msimage.mbn)
  -v, --verbose                Verbose
  -?, --help                   Display help/usage
  -s, --searchpath             Search path for files to program

  Examples:    """

    print " "

def PerformDloadOperations(COM_PORT):
    global serial_number,hw_id

    sps_common.MODE_IS_STREAMING = 0

    sps_common.port_open(COM_PORT)

    device_log("DLOAD: Looking for target - Sending DLOAD NOOP",1)
    SendCommand(CMD_NOOP)
    Attempts = 0
    KeepLooking = True
    while KeepLooking:
        sleep(1)
        data_read = GetNextPacket()

        if len(data_read) >= 1:  # could have returned [0]
            if data_read[0]==DLOAD_ACK:
                device_log("DLOAD: Device responded",0)
                KeepLooking = False

                CMD_SERIALNUMBER = 0x16			## according to doc, could be 0x16 as well, so try it first
                device_log("DLOAD: Trying DLOAD Serial Number with command 0x16 (note: not a problem if we get a NAK)",1)
                SendCommand(CMD_SERIALNUMBER)
                data_read = GetNextPacket()

                if data_read[0]==DLOAD_NAK:
                    device_log("DLOAD: Got a NAK, so trying DLOAD Serial Number with command 0x14",1)
                    CMD_SERIALNUMBER = 0x14
                    SendCommand(CMD_SERIALNUMBER)
                    data_read = GetNextPacket()

                if data_read[0]==CMD_SERIALNUMBER:
                    device_log("DLOAD: Device responded with SERIALNUMBER packet",0)
                    ## packet should be [0x14, length in bits, serial number byte for byte]
                    if data_read[1]/8 + 2 != len(data_read):
                        device_log("DLOAD: ERROR: Length of SERIALNUMBER packet is %i, expected it to be %i" % (len(data_read),data_read[1]/8 + 2),1)
                    else:
                        serial_number = "0x%X" % hex_list_to_int(data_read[2:][::-1])
                        device_log("DLOAD: SERIALNUMBER %s" % serial_number,1)
                        sleep(1)
                else:
                    device_log("DLOAD: Phone did not respond to this command. This is *not* an error",1)

                device_log("DLOAD: Trying DLOAD HW ID with (note: not a problem if we get a NAK)",1)
                SendCommand(CMD_HWID)
                data_read = GetNextPacket()

                if data_read[0]==CMD_HWID:
                    device_log("DLOAD: Device responded with CMD_HWID packet",0)
                    ## packet should be [0x14, length in bits, serial number byte for byte]
                    length = hex_list_to_int(data_read[1:3][::-1])

                    ## On some phones it *seems* like this number is not byte swapped
                    if length>64 :
                        length = hex_list_to_int(data_read[1:3])    ## it's not byte swapped then??

                    ## Possibly HACK - Spec says this is two bytes, but just to be safe
                    if length>64 :
                        length = hex_list_to_int(data_read[2])

                    if length/8 + 3 != len(data_read):
                        device_log("DLOAD: ERROR: Length of CMD_HWID packet is %i, expected it to be %i" % (len(data_read),length/8 + 3),1)
                    else:
                        hw_id = hex_list_to_int(data_read[2:])
                        device_log("DLOAD: CMD_HWID 0x%X" % hw_id,1)
                        sleep(1)
                else:
                    device_log("DLOAD: Phone did not respond to this command. This is *not* an error",1)

            else:
                device_log("DLOAD: Not the correct response, re-pinging device...",1,data_read,0)
        else:
            device_log("DLOAD: Nothing received back, re-pinging device...",1)

        Attempts+=1
        if Attempts > 5:
            DisplayResetDeviceErrorMessage(inspect.currentframe(),__file__)
            sys.exit()
        SendCommand(CMD_NOOP)

    sps_common.port_close()

def DisplayResetDeviceErrorMessage(c,f):
    print "\n"+"-"*78
    device_log("ERROR: Seems the device is *not* responding\n",1)
    device_log("\t** Try resetting the device AND unplug the USB cable\n",1)
    device_log("\tTry running \"python lsusb.py\" and ensure it is listed as",1)
    device_log("\t\tQualcomm HS-USB QDLoader 9008 <-- or something like this",1)

    device_log("\n\tLINE:"+str(c.f_lineno)+"\t"+f,1)

    print "-"*78 + "\n"

def ParseCommandLineOptions():
    global COMPORT_DLOAD,COMPORT_SAHARA,ks_path,PATHTOIMAGES,EFSRAW1,EFSRAW2,EFSRAW3,RX_Timeout,PATHFORMDMUPLOADS,serial_number,hw_id,ForceFileUpload,CommandLineArgs,Verbose, SkipSerialNumberStep

    CommandLineArgs = ""
    for arg in sys.argv:
        CommandLineArgs += " " + arg

    try:
        opts, args = getopt.getopt(sys.argv[1:], "d:p:f:u:t:1:2:3:s:h:n:mx:vi", ["dload=","port=", "PATHTOIMAGES=", "PATHFORMDMUPLOADS=", "timeout=", "where=", "efsraw1=", "efsraw2=", "efsraw3=", "search_paths=", "hw_id=", "serial_number=","ForceFileUpload","xml=","verbose", "skipserial"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(1)

    for o, a in opts:
        if o in ("-x", "--xml"):
            ParseXML(a.strip())

    ## Command line will over ride anything in the XML file
    for o, a in opts:
        if o in ("-d", "--dload"):
            COMPORT_DLOAD = a
        elif o in ("-p", "--port"):
            COMPORT_SAHARA = a
        elif o in ("-n", "--serial_number"):
            serial_number = a
        elif o in ("-m", "--ForceFileUpload"):
            ForceFileUpload = " -m" ## space is important here
        elif o in ("-h", "--HW_ID"):
            hw_id = a
        elif o in ("-f", "--PATHTOIMAGES"):
            PATHTOIMAGES= a
        elif o in ("-u", "--PATHFORMDMUPLOADS"):
            PATHFORMDMUPLOADS= a
        elif o in ("-t", "--timeout"):
            RX_Timeout= int(a)
        elif o in ("-v", "--verbose"):
            Verbose = "-v"
        elif o in ("-s", "--search_path"):
            search_paths.append(a.strip())	# no tabs or whitespace
        elif o in ("-x", "--xml"):
            pass    ## already handled above
        elif o in ("-i", "--skipserial"):
            SkipSerialNumberStep = True
        elif o in ("-?", "--help"):
            usage()
            sys.exit(0)
        else:
            assert False, "unhandled option %s " % o

    if SEARCH_CWD_LAST:
        search_paths.append(os.getcwd())    ## Put CWD at the end of search paths
    else:
        search_paths.insert(0, os.getcwd()) ## put CWD at the beginning of search paths

    ShowVariables()

def VerifySlashes(sz,name):
    if sz[-1] != "\\" and sz[-1] != "/":
        print "\nERROR: '%s' %s must end with a slash. As in c:\\temp\ or /data/\n" % (sz,name)
        sys.exit(1)

def ShowVariables():
    global COMPORT_DLOAD,COMPORT_SAHARA,ks_path,PATHTOIMAGES,EFSRAW1,EFSRAW2,EFSRAW3,RX_Timeout,PATHFORMDMUPLOADS,serial_number,hw_id,ForceFileUpload

    if COMPORT_DLOAD.lower().startswith('com'):
        COMPORT_DLOAD = '\\\\.\\' + COMPORT_DLOAD.upper()
    print "COMPORT_DLOAD=",COMPORT_DLOAD

    if COMPORT_SAHARA.lower().startswith('com'):
        COMPORT_SAHARA = '\\\\.\\' + COMPORT_SAHARA.upper()
    print "COMPORT_SAHARA=",COMPORT_SAHARA

    PATHTOIMAGES        = FixSlashes(PATHTOIMAGES)
    PATHFORMDMUPLOADS   = FixSlashes(PATHFORMDMUPLOADS)

    PATHTOIMAGES        = VerifyPath( PATHTOIMAGES )
    PATHFORMDMUPLOADS   = VerifyPath( PATHFORMDMUPLOADS )

    search_paths.insert(0,PATHTOIMAGES)
    search_paths.insert(0,PATHFORMDMUPLOADS)

    device_log("\n-- Search Paths ---------------------------\n",1);
    for x in range(len(search_paths)):
        device_log("\t(%i) '%s'" % (x+1,search_paths[x]),1)

    device_log("\n-- ARGUMENTS ------------------------------\n",1);
    device_log("\tCOMPORT_DLOAD     = '%s'" % COMPORT_DLOAD,1);
    device_log("\tCOMPORT_SAHARA    = '%s'" % COMPORT_SAHARA,1);
    device_log("\tPATHTOIMAGES      = '%s'" % PATHTOIMAGES,1);
    device_log("\tPATHFORMDMUPLOADS = '%s'" % PATHFORMDMUPLOADS,1);
    device_log("\tRX_Timeout        = %i"   % RX_Timeout,1);
    device_log("\tForceFileUpload   = '%s' (note -m here would mean QSaharaServer.exe will force files to be uploaded from MDM)" % ForceFileUpload,1);

def VerifyPath(sz):
    if verify_path(sz) is False:
        device_log("\nWARNING: path '%s' does not exist" % sz,1)
        try:
            os.makedirs(sz, 0755)
        except Exception, x:
            device_log("\tERROR: path '%s' does not exist and could not be created, ('%s')\n" % (sz,x),1)
            sys.exit(1)

        if verify_path(sz) is False:
            device_log("\nERROR: path '%s' does not exist and could not be created\n" % sz,1)
            sys.exit(1)

        device_log("\tNOTE: just created '%s'\n" % (sz),1)
        sz = sz.replace(dir,'')
        return sz
    else:
        print "path '%s' exists" % sz
        sz = sz.replace(dir,'')
        return sz

def FixSlashes(sz):
    if sz[-1] == "/" or sz[-1] == "\\": ## makes it have to have an anchor at the end
        sz = sz[:-1]  ## Take the left of what matched - get rid of slash

    if sys.platform=='win32':   ## Needs 0.9.8m or better
        sz = sz.replace("/", "\\")
        return "%s\\" % sz
    else:
        sz = sz.replace("\\", "/")
        return "%s/" % sz

def main():
    global DidStuff,hex_filename,COMPORT_EFSSYNC,COMPORT_DLOAD,COMPORT_SAHARA,ks_path,PATHTOIMAGES,PATHFORMDMUPLOADS,dir,serial_number,CommandLineArgs,Images, SkipSerialNumberStep


    reset_device_log()
    dir = FixSlashes( os.getcwd() )
    device_log("Script was started in '%s'\n" % dir, 1)

    ## Step 1: Get all the variables we need
    ParseCommandLineOptions()   ## this will also parse XML

    ## Step 2: Make sure we can find QSaharaServer
    ks_path = find_file("QSaharaServer.exe",search_paths)
    if ks_path is None:
        print "\nERROR: Could not find 'QSaharaServer.exe' file - This program is essential. Cannot continue\n"
        sys.exit()

    # If ks is found in this directory, remove the long path to make the call string cleaner
    ks_path = ks_path.replace(dir,'')

    if verbose:
        enable_full_dump()

    if len(COMPORT_DLOAD)==0:
        COMPORT_DLOAD = AutoDetectPort9008(search_paths)   # vid:pid
        if COMPORT_DLOAD is None:
            device_log("\nERROR: Could not detect which port device is on, maybe reset it?\n\n",1)
            sys.exit()
        device_log("Using COMPORT_DLOAD='%s'"%COMPORT_DLOAD,1)

    #COMPORT_DLOAD = re.sub('\\\\\\\\\\.\\\\', "", COMPORT_DLOAD) # clean the \\.\COM11 to COM11

    if len(COMPORT_SAHARA)==0:
        COMPORT_SAHARA = COMPORT_DLOAD
        device_log("Using COMPORT_SAHARA='%s'"%COMPORT_SAHARA,1)

    #COMPORT_SAHARA = re.sub('\\\\\\\\\\.\\\\', "", COMPORT_SAHARA) # clean the \\.\COM11 to COM11

    DidStuff = True

    PATHTOIMAGES        = PATHTOIMAGES.replace("\\","/")        ## This version of QSaharaServer needs slashes this way
    PATHFORMDMUPLOADS   = PATHFORMDMUPLOADS.replace("\\","/")   ## This version of QSaharaServer needs slashes this way

    ###########################################################################
    ## First step is to get the serial number
    ###########################################################################

    device_log("STEP 1 - Attempting to get device serial number",1)
    device_log("STEP 1 - Attempting to get device serial number",1)
    device_log("STEP 1 - Attempting to get device serial number",1)

    temp = Images['21']
    temp = find_file(temp,search_paths)

    if temp is None:
        device_log("ERROR: Could not find '%s' - cannot continue" % Images['21'],1)
        sys.exit(1)

    ## if the file is found in cwd, remove that to make a cleaner call string
    temp = temp.replace(dir,'')

    szImages = "-s 21:%s " % (temp)
    device_log(szImages,1)

    sz = "%s -r 21 -c 1 -v 1 %s -w %s -p %s %s" % \
          (ks_path,ForceFileUpload,PATHFORMDMUPLOADS,COMPORT_SAHARA,szImages)

    Purpose = "QSaharaServer.exe to retrieve serial number"
    CallWasSuccessful = 0
    temp_serial_number = ""
    
    if (SkipSerialNumberStep == False):
        print "\n" + "*"*80
        print "CALLING: %s\n\n%s"%(Purpose,sz)     ## This is just to get Serial Number
        print "\n" + "*"*80
        for x in range(5,0,-1):
            print "%2i - To exit this program, break now" % x
            sleep(1)

        print ""  

        response = external_call(sz)
        for line in response.strip("\r").split("\n"):
            print "\t"+line

            m = re.search("Received Byte:\s+0x([\da-fA-F]+)", line)
            if type(m) is not NoneType:
                CallWasSuccessful = 1
                temp_serial_number += m.group(1)

        if CallWasSuccessful == 0:
            device_log("\n%s"%Purpose,1)
            device_log("\nERROR: Something went wrong, please inspect output",1)
            device_log("\nERROR: Something went wrong, please inspect output",1)
            device_log("\nERROR: Something went wrong, please inspect output",1)
            sys.exit(1)
    else:
        print "Skipping Serial Number collection step."

    if temp_serial_number == "":
        device_log("\nThe device did not return a SERIAL NUMBER",1)
        temp_serial_number = serial_number
    else:
        device_log("\nSERIAL NUMBER from device is '0x%s'" % temp_serial_number,1)

    #LookForSerialNumberFile()

    ## If the user specified a serial number, let's make sure it all matches
    if serial_number != "0":

        CommandLineArgs = CommandLineArgs.replace('-n %s' % serial_number,'')

        if temp_serial_number != serial_number:
            device_log("\nWARNING: The serial number you manually specified (%s) does not match the serial number returned by the device (%s)" % (serial_number,temp_serial_number),1)
            serial_number = temp_serial_number
            device_log("The SERIAL_NUMBER has now been updated to '%s'" % serial_number,1)
            print "\n"


    CommandLineArgs = CommandLineArgs.replace('-n %s' % serial_number,'')
    CommandLineArgs += "-n %s" % serial_number

    ThisMDM_PATHFORMDMUPLOADS   = VerifyPath( PATHFORMDMUPLOADS + serial_number )
    ThisMDM_PATHFORMDMUPLOADS   = FixSlashes(ThisMDM_PATHFORMDMUPLOADS)
    ThisMDM_PATHFORMDMUPLOADS   = ThisMDM_PATHFORMDMUPLOADS.replace("\\","/")   ## This version of QSaharaServer needs slashes this way


    ## Look for any files in the ThisMDM_PATHFORMDMUPLOADS folder
    device_log("Looking for files in '%s'\n" % ThisMDM_PATHFORMDMUPLOADS, 1)
    dirList=os.listdir(ThisMDM_PATHFORMDMUPLOADS)
    if len(dirList)==0:
        device_log("WARNING: No files found - Must be first time for this MDM",1)
        print "\n"+"*"*80
        device_log("WARNING: No EFS files were found for this MDM, the default files will be sent",1)
        print "*"*80+"\n"
        for x in range(10,0,-1):
            print x
            sleep(1)
    else:
        ## Not the first time this chip has been synced, so update paths

        if PATHFORMDMUPLOADS in search_paths:
            search_paths.pop(search_paths.index(PATHFORMDMUPLOADS))

        if ThisMDM_PATHFORMDMUPLOADS not in search_paths:
            search_paths.insert(0,ThisMDM_PATHFORMDMUPLOADS)

        ## To any PREPEND OPERATION for the EFS headers
        for x in range(len(dirList)):
            #print "\t\t",dirList[x]
            if dirList[x]=="m9kefs1":
				device_log("\nm9kefs1 *EXISTS* - It will be transferred instead of efs1.bin",1)
				Images['16'] = ThisMDM_PATHFORMDMUPLOADS+dirList[x]
            if dirList[x]=="m9kefs2":
				device_log("\nm9kefs2 *EXISTS* - It will be transferred instead of efs1.bin",1)
				Images['17'] = ThisMDM_PATHFORMDMUPLOADS+dirList[x]

    print ""

    ##if CreatedEFS1==0 or CreatedEFS2==0 or CreatedEFS3==0:
    for x in range(5,0,-1):
        print "%2i - To exit this program, break now" % x
        sleep(1)


    ###########################################################################
    ## Next step is actual image download
    ###########################################################################

    if PerformStep2 == 1:

        device_log("\nSTEP 2 - Image download",1)
        device_log("STEP 2 - Image download",1)
        device_log("STEP 2 - Image download",1)

        for i in ImagesToSend:
            if ("%i" % i) not in Images.keys():
                device_log("\nERROR: Image '%s' specified in ImagesToSend is not in Image list. No filename associated with this" % i,1)
                sys.exit(1)

        szImages = ""
        for i in Images.keys():
            if int(i) in ImagesToSend:
                temp = Images[i]
                temp = find_file(temp,search_paths)

                if temp is None:
                    device_log("ERROR: Could not find '%s', image ID '%s' - cannot continue" % (Images[i],i),1)
                    sys.exit(1)

                ## if the file is found in cwd, remove that to make a cleaner call string
                temp = temp.replace(dir,'')
                szImages += "-s %s:%s " % (i,temp)


        ThisMDM_PATHFORMDMUPLOADS   = VerifyPath( PATHFORMDMUPLOADS + serial_number )
        ThisMDM_PATHFORMDMUPLOADS   = FixSlashes(ThisMDM_PATHFORMDMUPLOADS)
        ThisMDM_PATHFORMDMUPLOADS   = ThisMDM_PATHFORMDMUPLOADS.replace("\\","/")   ## This version of QSaharaServer needs slashes this way

        sz = "%s -r 21 %s%s -w %s -p %s %s" % \
              (ks_path, Verbose,ForceFileUpload,ThisMDM_PATHFORMDMUPLOADS,COMPORT_SAHARA,szImages)

        Purpose = "QSaharaServer.exe for image transfer"
        print "\n" + "*"*80
        print "CALLING: %s\n\n%s"%(Purpose,sz)     ## This is for image transfer
        print "\n" + "*"*80
        for x in range(5,0,-1):
            print "%2i - To exit this program, break now" % x
            sleep(1)

        print ""
        CallWasSuccessful = 0
        response = external_call(sz)
        for line in response.strip("\r").split("\n"):
            print "\t"+line

            m = re.search("Sahara protocol completed", line)
            if type(m) is not NoneType:
                CallWasSuccessful = 1

        if CallWasSuccessful == 0:
            device_log("\n%s"%Purpose,1)
            device_log("\nERROR: Something went wrong, please inspect output",1)
            device_log("\nERROR: Something went wrong, please inspect output",1)
            device_log("\nERROR: Something went wrong, please inspect output",1)
            sys.exit(1)

        CommandLineArgs = CommandLineArgs.replace('-n %s' % serial_number,'')
        CommandLineArgs += "-n %s" % serial_number

        ### Look for any files in the PATHFORMDMUPLOADS folder, if anything there they need to be moved
        #device_log("Looking for files in '%s'\n" % PATHFORMDMUPLOADS, 1)
        #dirList=os.listdir(PATHFORMDMUPLOADS)
        #if len(dirList)==0:
        #    device_log("\tNo files found (i.e. none uploaded to PC) therefore none to move",1)
        #else:
        #    for x in range(len(dirList)):
        #        if os.path.isfile( dirList[x] ):
        #            print "\t(%i) '%s', moving to path '%s'" % (x+1,dirList[x],ThisMDM_PATHFORMDMUPLOADS)
        #        ##shutil.move(os.path.join(PATHFORMDMUPLOADS,dirList[x]),ThisMDM_PATHFORMDMUPLOADS)    ## source,dest


    ###########################################################################
    ## Next step is waiting for EFS sync to occur
    ###########################################################################

    if (SkipSerialNumberStep == True):
        raw_input("Press Enter to continue with EFS sync step")

    if len(COMPORT_EFSSYNC)==0:
        for x in range(30):
            COMPORT_EFSSYNC = AutoDetectPortEFSSync(search_paths)   # vid:pid
            if COMPORT_EFSSYNC is None:
                device_log("\nCould not detect which port device is on. Trying again %i of 30?\n\n" % (x+1),1)
                sleep(0.1)
            else:
                device_log("Using COMPORT_EFSSYNC='%s'"%COMPORT_EFSSYNC,1)
                break

    if type(COMPORT_EFSSYNC) is NoneType or len(COMPORT_EFSSYNC)==0:
        device_log("\nERROR: Could not detect which port is available for EFS SYNC.\n",1)
        device_log("\nERROR: It should look like this \"COM8      : ID 05C6:9048 Qualcomm HS-USB SER4 9048 (COM8)\"\n\n",1)
        sys.exit(1)

    #COMPORT_EFSSYNC = re.sub('\\\\\\\\\\.\\\\', "", COMPORT_EFSSYNC) # clean the \\.\COM11 to COM11

    EFSSyncCount = 0

    ## EFS sync happens forever - QSaharaServer called in a loop

    sz = "%s -l -v 1 -w %s -p %s > efs_sync_messages.txt" % \
          (ks_path,ThisMDM_PATHFORMDMUPLOADS,COMPORT_EFSSYNC)

    efs_process = non_blocking_call(sz)

    device_log("\nSTEP 3 - Waiting for EFS sync - %i completed so far" % EFSSyncCount,1)
    device_log("STEP 3 - Waiting for EFS sync - %i completed so far" % EFSSyncCount,1)
    device_log("STEP 3 - Waiting for EFS sync - %i completed so far" % EFSSyncCount,1)

    Purpose = "QSaharaServer.exe for EFS sync"
    print "\n" + "*"*80
    print "CALLING: %s - %i completed so far\n\n%s"%(Purpose,EFSSyncCount,sz)     ## This is for EFS sync - once
    print "\n" + "*"*80

    print ""
    SavedName = []
    SavedSize = []
    CallWasSuccessful = 0       ## HACK for now, not sure what success looks like here
    SecondsElapsed = 0
    TimeSyncedEFS1 = []
    TimeSyncedEFS2 = []
    EFSSyncCount   = 0
    print "EFS Sync should be running now, saving QSaharaServer.exe output to 'efs_sync_messages.txt'"
    while True:
        sys.stdout.write(".")
        sleep(2)
        SecondsElapsed+=1

        ## Look for any files in the ThisMDM_PATHFORMDMUPLOADS folder, if anything there they need to be moved
        #device_log("\nLooking for files in '%s'\n" % ThisMDM_PATHFORMDMUPLOADS, 1)
        dirList=os.listdir(ThisMDM_PATHFORMDMUPLOADS)
        if len(dirList)==0:
            continue

        for x in range(len(dirList)):
            #print "\t(%i) Found '%s'" % (x+1,dirList[x])
            if dirList[x] == "m9kefs1":
                try:
                    st = os.stat(ThisMDM_PATHFORMDMUPLOADS+dirList[x])
                except IOError:
                    device_log("failed to get information about %s" % (ThisMDM_PATHFORMDMUPLOADS+dirList[x]),1)

                TimeString = time.asctime(time.localtime(st[ST_MTIME]))

                if TimeString not in TimeSyncedEFS1:
                    TimeSyncedEFS1.append( TimeString )
                    EFSSyncCount+=1
                    device_log("\nINFO: EFS SYNC happened at %s for %s\n" % (TimeString, ThisMDM_PATHFORMDMUPLOADS+dirList[x]),1)

            if dirList[x] == "m9kefs2":
                try:
                    st = os.stat(ThisMDM_PATHFORMDMUPLOADS+dirList[x])
                except IOError:
                    device_log("failed to get information about %s" % (ThisMDM_PATHFORMDMUPLOADS+dirList[x]),1)

                TimeString = time.asctime(time.localtime(st[ST_MTIME]))

                if TimeString not in TimeSyncedEFS2:
                    TimeSyncedEFS2.append( TimeString )
                    EFSSyncCount+=1
                    device_log("\nINFO: EFS SYNC happened at %s for %s\n" % (TimeString, ThisMDM_PATHFORMDMUPLOADS+dirList[x]),1)

        #if EFSSyncCount==0 and SecondsElapsed > 600:
        #   device_log("\nERROR: It looks like EFS sync is *not* happening\n",1)
            #efs_process.terminate()
            #break

 ## Here we read the efs_sync_messages.txt
        AllIsWell = 1
        try:
            f = open("efs_sync_messages.txt", "r")
        except Exception,x:
            print "ERROR: could not open 'efs_sync_messages.txt', Reason %s" % (x)
            continue

        try:
            lines = f.readlines()
        except Exception,x:
            print "ERROR: could not read from 'efs_sync_messages.txt', Reason %s" % (x)
            continue

        f.close()

        for line in lines:
            #print line
            m = re.search("Error occurred while reading from COM port", line)
            if type(m) is not NoneType:
                print line
                AllIsWell = 0
                break

        if AllIsWell==0:
            break


    sys.exit(1)



    ## end of EFS sync phase ----------------------------------------------------------------------

    if DidStuff == False:
        print "=" * 78
        print "ERROR: Nothing to do. - Nothing to do - Nothing to do - Nothing to do"
        print "=" * 78



    print ""
    #print "*"*78
    #print "*"*78
    #device_log("\nTo ensure the same EFS files are synced for this device, please launch like this \n\npython%s\n\n" % CommandLineArgs,1)
    #print "*"*78
    #print "*"*78

if __name__ == "__main__":
    #start_time = time()
    main()
    #end_time = time()


def LookForSerialNumberFile():
    global temp_serial_number

    ## Is there a "commandop.bin" file to parse?
    SerialNumberFileExists = 1
    try:
        IN = open("commandop.bin")
    except Exception, e:
        device_log("\nCan't open commandop.bin for reading: %s" % e,1)
        SerialNumberFileExists = 0

    temp_serial_number = serial_number

    if SerialNumberFileExists==1:

        for line in IN.readlines():
            print line
            m = re.search("([\da-fA-F]+)", line)
            if type(m) is not NoneType:
                temp_serial_number = m.group(1)
                device_log("SERIAL_NUMBER discovered to be '%s'" % temp_serial_number,1)
                break

        IN.close()
