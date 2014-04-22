#===========================================================================

#  helper script for emergency programmer for blank emmc devices

# REFERENCES

#  $Header: //components/rel/core.glue/1.3/tools/cmm/common/mdm9205/kickstart/sps_common.py#1 $
#  $DateTime: 2019/01/17 22:25:40 $ 
#  $Author: pwbldsvc $

# when          who     what, where, why 
# --------      ---     ------------------------------------------------------- 
# 2012-05-03    ah      moved pyserial data2string conversion to absolute final write() function
#                       SHA256 support + stream_command_trace.txt log
# 2012-03-26    ah      Various updates - better error messages
# 2011-10-11    ab      Modified device writing to loop till complete
# 2011-07-28    ab      Added function to split data into HDLC packets
# 2011-07-21    ah      More feedback and error checking
# 2011-07-18    ah      Minor changes, more feedback
# 2011-07-01    ab      Initial release

# Copyright (c) 2007-2010
# Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
# ===========================================================================*/

import subprocess as sub
import struct
import hashlib
import sys,os
try:
    import serial
except Exception, x:
    print "\nERROR: This program makes use of PySerial to talk to COM ports"
    print "Please install this from here http://pyserial.sourceforge.net/ for Python 2.x\n"
    sys.exit()
from datetime import datetime
import inspect,re
from types import *
from time import strftime, localtime, sleep

lsusb       = None
PhoneMode   = None

CMD_SERIALNUMBER	= 0x14  # can also be 0x16
CMD_HWID	        = 0x17

PRTN_EMMCUSER       = 0x21 # For programming eMMC chip (singleimage.mbn)
PRTN_EMMCBOOT1      = 0x22
PRTN_EMMCBOOT2      = 0x23

CommandsSent     = 0
UsingHashSecurity= False
CreateHashesOnly = False
NumHashes        = 0        ## Number in HASH_TABLE.bin
port_trace = True
RX_Buffer = []
device_handle = "Aaron"
MAX_READ_RETRIES = 4
MAX_DATA_LENGTH = 1024
NUM_WINDOWS = 2

enable_hdlc_rx = True
enable_hdlc_tx = True

MODE_IS_STREAMING   = 0
JustWroteData       = 0
## These defines are to verify we are in the mode we think we are

DLOAD_ACK_SIZE       = 1
DLOAD_NAK_SIZE       = 3
DLOAD_SWVERSION_SIZE = 0   # variable and needs more processing
DLOAD_POWERDOWN_SIZE = 1   # i.e. confused for STREAMING error

DLOAD_ACK       = 2
DLOAD_NAK       = 3
DLOAD_SWVERSION = 0xD   # i.e. confused for STREAMING log
DLOAD_POWERDOWN = 0xE   # i.e. confused for STREAMING error

DloadCommand = [
    "0x00 = illegal - probably garbage?",
    "0x01 = Write",
    "0x02 = ACK",
    "0x03 = NAK",
    "0x04 = Erase",
    "0x05 = Go",
    "0x06 = NO-OP",
    "0x07 = Param Request",
    "0x08 = Param Response",
    "0x09 = Mem Dump",
    "0x0A = Reset",
    "0x0B = Unlock",
    "0x0C = SW Ver Request",
    "0x0D = SW Ver Response",
    "0x0E = Powerdown",
    "0x0F = Write with 32 bit addr",
    "0x10 = Mem Debug query",
    "0x11 = Mem Debug info",
    "0x12 = Mem Read Request",
    "0x13 = Mem Read Response",
    "0x14 = Serial Number request/response V7",
    "0x15 = Memory unframed read response",
    "0x16 = Serial Number request/response V8",
    "0x17 = HW ID request/response",
    "0x18 = HW ID request",
]

DloadNAKerrorCodes = [
    "0x0000 = illegal",
    "0x0001 = Invalid frame FCS",
    "0x0002 = Invalid destination address",
    "0x0003 = Invalid length",
    "0x0004 = Unexpected end of packet",
    "0x0005 = Data length too large for buffer",
    "0x0006 = Unknown/invalid command",
    "0x0007 = Operation failed",
    "0x0008 = Wrong flash intelligent ID",
    "0x0009 = Bad programming voltage",
    "0x000A = Write-verify failed",
    "0x000B = Not permitted without unlock",
    "0x000C = Incorrect security code",
    "0x000D = Cannot power down phone",
    "0x000E = Operation not permitted at this time",
    "0x000F = Invalid read address",
]

StreamingCommand = [
    "0x00 = Illegal - probably garbage?",
    "0x01 = Hello",
    "0x02 = Hello response",
    "0x03 = Read",
    "0x04 = Read response",
    "0x05 = Simple write",
    "0x06 = Wrote okay",
    "0x07 = Stream write",
    "0x08 = Block written",
    "0x09 = NOP",
    "0x0A = NOP response",
    "0x0B = Reset",
    "0x0C = Reset ACK",
    "0x0D = Error",
    "0x0E = Log",
    "0x0F = Unlock",
    "0x10 = Unlocked",
    "0x11 = Power off",
    "0x12 = Powering down",
    "0x13 = Open",
    "0x14 = Opened",
    "0x15 = Close",
    "0x16 = Closed",
    "0x17 = Security mode",
    "0x18 = Security mode received",
    "0x19 = Partition table",
    "0x1A = Partition table received",
    "0x1B = Open multi-image",
    "0x1C = Opened multi-image",
    "0x1D = Erase Flash",
    "0x1E = Flash erased",
    "0x1F = Get ECC state",
    "0x20 = Current ECC state",
    "0x21 = Set ECC",
    "0x22 = Set ECC response",
    "0x23 = Partition Info Command",
    "0x24 = Partition Info Response"
]

EHOST_ERROR_CODES = [
    "Illegal reason (do not use)",
    "Reserved",
    "Invalid destination address",
    "Invalid length",
    "Unexpected end of packet",
    "Invalid command",
    "Reserved",
    "Operation failed",
    "Wrong Flash intelligent ID",
    "Bad programming voltage",
    "Write-verify failed",
    "Reserved",
    "Incorrect security code",
    "Cannot power down phone",
    "NAND Flash programming not supported",
    "Command out of sequence",
    "Close did not succeed",
    "Incompatible feature bits",
    "Out of space",
    "Invalid Security mode",
    "Multi-image NAND not supported",
    "Power off command not supported",
    "Illegal reason (do not use)",
]

def port_is_open():
    return device_is_open(device_handle)

def port_open(portname):
    global device_handle
    device_handle = device_open(portname)

def port_close():
    device_close(device_handle)

def port_write(data_packet):
    return device_write(device_handle, hdlc_encode(data_packet))

def AutoDetectPort9008(search_paths):
    global lsusb,PhoneMode

    if lsusb is None:
        lsusb = find_file("lsusb.py",search_paths)

    if lsusb is None:
        return None

    response = external_call('python %s' % lsusb)

    ## COM13     : ID 05C6:9006 Qualcomm HS-USB Diagnostics 9006 (COM13)
    ## COM11     : ID 05C6:9008 Qualcomm HS-USB QDLoader 9008 (COM11)

    for line in response.strip("\r").split("\n"):
        print "\t"+line                              

        # Check for Qualccomm device
        m = re.search("Qualcomm", line)         
        if type(m) is NoneType:
            continue

        m = re.search("(9008|9006)", line) ## COM11     : ID 05C6:9008 Qualcomm HS-USB QDLoader 9008 (COM11)
        if type(m) is NoneType:
            continue
        else:
            if m.group(1) == '9008':
                PhoneMode = "\t\tPhone is in DLOAD or STREAMING mode"
            else:
                PhoneMode = "\t\tPhone is in MASS STORAGE mode "

        if sys.platform.startswith("linux"):
            PrintBigBanner(PhoneMode + " *assumed* on /dev/ttyUSB0")
            return "/dev/ttyUSB0"
        else:
            m = re.search("(COM\d+)", line)    ## COM11     : ID 05C6:9008 Qualcomm HS-USB QDLoader 9008 (COM11)
            if type(m) is NoneType:
                print "\nERROR: Can't find port it is on??"
                return None

            PrintBigBanner(PhoneMode + " on " + m.group(1).upper())
            return '\\\\.\\' + m.group(1).upper()

    return None

def AutoDetectPortEFSSync(search_paths):
    global lsusb,PhoneMode

    if lsusb is None:
        lsusb = find_file("lsusb.py",search_paths)

    if lsusb is None:
        return None

    response = external_call('python %s' % lsusb)

    ## COM8      : ID 05C6:9048 Qualcomm HS-USB SER4 9048 (COM8)

    for line in response.strip("\r").split("\n"):
        print "\t"+line                              

        # Check for Qualccomm device
        m = re.search("Qualcomm", line)         
        if type(m) is NoneType:
            continue

        m = re.search("(SER4)", line) ## COM8      : ID 05C6:9048 Qualcomm HS-USB SER4 9048 (COM8)
        if type(m) is NoneType:
            continue

        if sys.platform.startswith("linux"):
            PrintBigBanner("EFS sync to happen *assumed* on /dev/ttyUSB0")
            return "/dev/ttyUSB0"
        else:
            m = re.search("(COM\d+)", line)    ## COM8      : ID 05C6:9048 Qualcomm HS-USB SER4 9048 (COM8)
            if type(m) is NoneType:
                print "\nERROR: Can't find port it is on??"
                return None

            PrintBigBanner("EFS sync to happen on " + m.group(1).upper())
            return '\\\\.\\' + m.group(1).upper()

    return None
    
def PrintBigBanner(sz):
    print "\n\n"+"-"*78
    print "-"*78
    print sz
    print "-"*78
    print "-"*78+"\n"

def PrintBanner(sz):
    print "\n"+"-"*78
    print sz
    print "-"*78+"\n"

def ReturnSizeInSectors(size):
    rem = size%512
    if rem != 0:
        return int(size/512)+1
    else:
        return int(size/512)
        
def verify_path(path):
    if os.path.exists(path):
        return True
    else:
        return False
        
def find_file(filename, search_paths):
    #import pdb; pdb.set_trace()
    device_log("\n\n\tLooking for '%s'"%filename,1)
    print "\t"+"-"*40
    for x in search_paths:
        device_log("\tSearching %s"%x,1)
        temp = os.path.join(x, filename)
        if os.path.exists(temp):
            device_log("\n\t**Found %s (%i bytes) (%i sectors)\n" % (temp,os.path.getsize(temp),ReturnSizeInSectors(os.path.getsize(temp))),1)
            return temp

    ## search cwd last
    device_log("\tSearching CWD %s"%os.getcwd(),1)
    if os.path.exists(filename):
        device_log("\n\t**Found %s (%i bytes)\n" % (filename,os.path.getsize(filename)),1)
        return filename

    device_log("\tCound't find file\n",1)
    return None

def non_blocking_call(command):
    print "\tRunning %s" % command
    errors = None
    output = None
    try:
        p = sub.Popen(command, stdout=sub.PIPE, stderr=sub.PIPE, shell=True)
    except Exception, x:
        print "Error executing command '%s'. Reason: %s" % (str(command), x)
        sys.exit(1)
    return p
    
def external_call(command, capture_output=True):
    print "\tRunning %s" % command
    errors = None
    output = None
    try:
        if capture_output:
            p = sub.Popen(command, stdout=sub.PIPE, stderr=sub.PIPE, shell=True)
            output, errors = p.communicate()
        else:
            output = os.system(command)
    except Exception, x:
        print "Error executing command '%s'. Reason: %s" % (str(command), x)
        #clean_up()
        sys.exit(1)
    finally:
        #if not output is None:
        #    print "Result: %s" % output
        if (not errors is None) and (not errors == ""):
            print "Process stderr: %s" % errors
    return output


def enable_full_dump():
    global port_trace
    port_trace = True

def display_warning():
    print "\t                         (_)            "
    print "\t__      ____ _ _ __ _ __  _ _ __   __ _ "
    print "\t\\ \\ /\\ / / _` | '__| '_ \\| | '_ \\ / _` |"
    print "\t \\ V  V / (_| | |  | | | | | | | | (_| |"
    print "\t  \\_/\\_/ \\__,_|_|  |_| |_|_|_| |_|\\__, |"
    print "\t                                   __/ |"
    print "\t                                  |___/ "

def reset_device_log():
    try:
        out_fp = open('port_trace.txt', 'w')
    except Exception, x:
        print "\nERROR: Can't create the file port_trace.txt"
        print "This might be because the file is open and locked"
        print "Or because you are running this from a read-only location, Reason: '%s'\n" % x
        sys.exit()

    try:
        out_fp = open('stream_command_trace.txt', 'w')
    except Exception, x:
        print "\nERROR: Can't create the file stream_command_trace.txt"
        print "This might be because the file is open and locked"
        print "Or because you are running this from a read-only location, Reason: '%s'\n" % x
        sys.exit()

    try:
        out_fp = open('HASH_TABLE.bin', 'wb')
    except Exception, x:
        print "\nERROR: Can't create the file HASH_TABLE.bin"
        print "This might be because the file is open and locked"
        print "Or because you are running this from a read-only location, Reason: '%s'\n" % x
        sys.exit()
        
    out_fp.close()


def device_log(message, display=0, data=None, GuessPacketAtIndex=-1):
    try:
        out_fp = open('port_trace.txt', 'a')
    except Exception, x:
        print "ERROR: could not open 'port_trace.txt' for appending, Reason: '%s'" % x
        return

    if display==1:
        print message

    ## Remove any extra newlines 
    try:
        out_fp.write("%s %s\n" % (strftime("%H:%M:%S", localtime()),message.replace('\n', '')))
    except Exception,x:
        print "ERROR: could not write to 'port_trace.txt', Reason %s" % x
        return

    if not data is None:
        if GuessPacketAtIndex>=0:
            if len(data)>=(GuessPacketAtIndex+1):    # if guess is 0, then len() must be 1
                try:
                    command_id = data[GuessPacketAtIndex]   ## If still HDLC encoded, GuessPacketAtIndex should be 1, else 0
                    if command_id<len(DloadCommand):
                        out_fp.write("\t\tDLOAD ------> "+DloadCommand[command_id]+"\n")
                    if command_id<len(StreamingCommand):
                        out_fp.write("\t\tSTREAMING --> "+StreamingCommand[command_id]+"\n")
                except Exception,x:
                    print "ERROR: could not write to 'port_trace.txt', Reason %s" % x
                    return

        HexPrint(data,display,out_fp)
    out_fp.close()


def device_open(portname):
    try:
        ser = serial.Serial(portname, 115200, timeout=0.1)
    except Exception, x:
        device_log("ERROR: Could not open '%s'" % portname,1)
        device_log("ERROR: If your device is on this port, try resetting it?, Reason: '%s'\n\n" % x,1)
        sys.exit(1)

    device_log("Port %s opened\n" % portname,1)

    #device_log("ser.portstr\t%s" % ser.portstr,1)
    #device_log("ser.parity\t%s" % ser.parity,1)
    #device_log("ser.baudrate\t%d" % ser.baudrate,1)
    #device_log("ser.bytesize\t%d" % ser.bytesize,1)
    #device_log("ser.stopbits\t%d\n" % ser.stopbits,1)

    return ser

def device_write(ser, data):
    #bytes_written = 0
    #while bytes_written < len(data):
    #    x = ser.write(data[bytes_written:])
    #    if type(x) is NoneType:
    #        bytes_written += len(data)  # Linux does this, no idea why yet
    #    else:
    #        bytes_written += x
    ser.write(''.join([chr(x) for x in data]))
    ser.flush()
    if port_trace:
        device_log("device_write(%i bytes)" % len(data), 0, data,1) # guess at 1 since it's encoded
    #return x
    return len(data)

def HexPrint(data,tostdout=0,fp=None):
    szNum = ""
    szAsc = ""
    digest= ""
    doSHA256 = 1
    P = 16

    if len(data)==0:
        szHexDump = "\n\tBuffer holds %d bytes\n\n" % len(data)
    else:
        
        if doSHA256==1 and MODE_IS_STREAMING==1 and JustWroteData==1:
            if data[0] == 0x7e and data[-1] == 0x7e:
                szTemp = ''    ## don't want to show HDLC encoded SHA256
            else:
                try:
                    szTemp = ''.join([chr(x) for x in data])
                except Exception, x:
                    import pdb; pdb.set_trace()
                    szTemp = ''.join(data)
                
                digest = '(sha256 '+hashlib.sha256(szTemp).hexdigest() + ')' # data must be a string
                
            #try:
            #    szTemp = ''.join(data)
            #except Exception, x:
            #    try:
            #        szTemp = ''.join([chr(x) for x in data])
            #    except Exception, x:
            #        import pdb; pdb.set_trace()
            #        print data
            #digest = '\nsha256 '+hashlib.sha256(szTemp).hexdigest() # data must be a string
                

        if len(data)>32:
            szHexDump = "\n\tTruncating %d bytes\n\t" % (len(data))
            for i in range(16):
                v = data[i]
                szNum += "%.2X " % v
                if v>=0x20 and v<=0x7E:
                    szAsc += "%c"    % v
                else:
                    szAsc += "."
            szNum += "\t"+szAsc+"\n\t...\n\t"
            szAsc = ""
            for i in range(len(data)-16,len(data)):
                v = data[i]
                szNum += "%.2X " % v
                if v>=0x20 and v<=0x7E:
                    szAsc += "%c"    % v
                else:
                    szAsc += "."
            
        else:
            szHexDump = "\n\tShowing %d bytes\n\t" % (len(data))
            for i in range(len(data)):
                v = data[i]
    
                if i>0 and i % 16==0:
                    szHexDump += "%-48s\t%s\n\t" % (szNum,szAsc)
                    szNum = ""
                    szAsc = ""
        
                szNum += "%.2X " % v
                if v>=0x20 and v<=0x7E:
                    szAsc += "%c"    % v
                else:
                    szAsc += "."
    
        szHexDump += "%-48s\t%s\n%s\n\n" % (szNum,szAsc,digest)

    if fp!=None:
        try:
            fp.write(szHexDump)
        except Exception,x:
            print "ERROR: could not write to '%s',Reason %s" % (fp.name,x)
            return

    if tostdout==1 or fp==None:
        print szHexDump
        

def device_read(ser, read_length=64):
    data = ser.read(read_length)
    if port_trace:
        if (len(data) > 0):
            device_log("device_read(%i of %i bytes requested)" % (len(data),read_length), 0, data,1)    # guess at 1 since encoded
    return data

def device_close(ser):
    ser.close()
    ser = None
    device_log("Port closed device_close()\n\n")

def device_is_open(ser):
    if ser is None:
        return False
    return ser.isOpen()

AHDLC_FLAG  = 0x7e
AHDLC_ESCAPE= 0x7d
AHDLC_ESC_M = 0x20

def PrintYouAreNotInStreamingMode(QuitOnFail=1):
    global MODE_IS_STREAMING
    device_log("\n\n"+"="*78,1)
    device_log("WARNING: You are *NOT* in STREAMING DOWNLOAD MODE\n",1)
    device_log("WARNING: You are *NOT* in STREAMING DOWNLOAD MODE\n",1)
    device_log("WARNING: You are *NOT* in STREAMING DOWNLOAD MODE",1)
    device_log("\nNOTE: Your device is not in the state expected",1)
    MODE_IS_STREAMING = 0
    device_log("="*78+"\n",1)
    if QuitOnFail==1:
        device_log("NOTE: Please reset your device",1)
        sys.exit(1)

def PrintYouAreNotInDloadMode(QuitOnFail=1):
    global MODE_IS_STREAMING
    device_log("\n\n"+"="*78,1)
    device_log("WARNING: You are *NOT* in DLOAD MODE\n",1)
    device_log("WARNING: You are *NOT* in DLOAD MODE\n",1)
    device_log("WARNING: You are *NOT* in DLOAD MODE\n",1)
    device_log("\nNOTE: Your device is not in the state expected",1)
    MODE_IS_STREAMING = 1
    device_log("="*78+"\n",1)
    if QuitOnFail==1:
        device_log("NOTE: Please reset your device",1)
        sys.exit(1)

def TestIfWeAreInTheModeWeThinkWeAreIn(packet):
    #device_log("TestIfWeAreInTheModeWeThinkWeAreIn");

    #if MODE_IS_STREAMING:
    #    device_log("Current mode is believed to be STREAMING",0)
    #else:
    #    device_log("Current mode is believed to be DLOAD",0)

    if packet[0]==DLOAD_NAK and len(packet)==DLOAD_NAK_SIZE:
        if MODE_IS_STREAMING==1:
            PrintYouAreNotInStreamingMode()
            return False

    elif packet[0]==DLOAD_ACK and len(packet)==DLOAD_ACK_SIZE:
        if MODE_IS_STREAMING==1:
            PrintYouAreNotInStreamingMode()
            return False

    elif packet[0]==DLOAD_POWERDOWN:
        if MODE_IS_STREAMING==0 and len(packet)!=DLOAD_POWERDOWN_SIZE:
            PrintYouAreNotInDloadMode()
            return False

    elif packet[0]==DLOAD_SWVERSION:
        if MODE_IS_STREAMING==0:
            # if we really are in DLOAD mode, then packet[1] holds the length of 
            # the rest of this packet, Therefore lets see
            if (packet[1]+2)!=len(packet):
                PrintYouAreNotInDloadMode()
                return False

def HDLC_Decode():
    global RX_Buffer,MODE_IS_STREAMING
    debugthis=1


    ShowPacketToUser = 0    # Overrides if it's an error

    FramingOk   = 0
    FrameStart  = None
    FrameEnd    = None

    ##print "len(RX_Buffer)=",len(RX_Buffer)
    
    for i in range(len(RX_Buffer)):
        if RX_Buffer[i] == AHDLC_FLAG:
            # Found first 0x7E
            FrameStart = i
            FramingOk+=1
            break

    if FramingOk == 0:
        device_log("WARNING: HDLC_Decode(%i bytes), something is wrong with this buffer, could not find start of frame" % len(RX_Buffer), 0, RX_Buffer)
        print "HDLC_Decode() Returning [0]"
        return [0]  # Nothing to decode

    i = i + 1   # i is where we found the start of the frame, so we throw this away

    ##print "FrameStart=%i and i=%i" % (FrameStart,i)
    ##device_log("RX_Buffer",1,RX_Buffer)
    

    RX_Buffer = RX_Buffer[i:]

    for i in range(len(RX_Buffer)):
        if RX_Buffer[i] == AHDLC_FLAG:
            # Found last 0x7E
            FrameEnd = i + FrameStart
            FramingOk+=1
            break

    if FramingOk == 1:  # should be 2 at this point if we found both
        device_log("WARNING: HDLC_Decode(%i bytes), something is wrong with this buffer, could not find end of frame" % len(RX_Buffer), 0, RX_Buffer)
        print "HDLC_Decode() Returning [0]"
        return [0]  # Nothing to decode

    packet    = RX_Buffer[:i]    # Gets everything else
    packet_length = len(packet)
    
    if packet_length<=2:
        print "packet_length<=2 - should be impossible!!"
        return []
    
    RX_Buffer = RX_Buffer[i+1:]  # Gets what is left over

    
    #print "packet_length=",packet_length
    
    i = 0
    while i < packet_length:
        if packet[i] == AHDLC_ESCAPE and packet[i+1] == 0x5e:
            packet.pop(i)
            packet.pop(i)
            packet.insert(i, AHDLC_FLAG)
            packet_length = packet_length - 1
        if packet[i] == AHDLC_ESCAPE and packet[i+1] == 0x5d:
            packet.pop(i)
            packet.pop(i)
            packet.insert(i, AHDLC_ESCAPE)
            packet_length = packet_length - 1
        i = i + 1

    temp = packet[-2:]
    try:    #HACK
        recv_crc = ((temp[1] << 8) & 0xFF00) | (temp[0] & 0x00FF)
    except Exception, x:
        try:
            recv_crc = ((ord(temp[1]) << 8) & 0xFF00) | (ord(temp[0]) & 0x00FF)
        except Exception, x:
            print "Hit exception '%s'" % x
            import pdb; pdb.set_trace()
            a = 5
            b = 6

    packet = packet[:-2]
    calc_crc = 0xffff & calculate_crc(packet, len(packet)*8)
    if recv_crc != calc_crc:
        device_log("CRC doesn't match", 1)
        device_log("Packet CRC: 0x%.4X" % recv_crc)
        device_log("Expect CRC: 0x%.4X\n" % calc_crc)
        print "HDLC_Decode() Returning [0]"
        return [0]

    device_log("Returning packet (HDLC coding removed)",0,packet,0) # final 0 here means to guess at the packet at index 0, i.e. it's decoded
    ##device_log("HDLC_Decode() Returning packet=",packet," of type",type(packet)

    # There is a chance we're actually in DLOAD mode when we think we're in STREAMING
    # Test for this by seeing if the response is actually an ACK/NACK
    # ==========================================================
    TestIfWeAreInTheModeWeThinkWeAreIn(packet)
    # ==========================================================    

    ## Note: if we *weren't* in the correct state above, i.e. thought in DLOAD but really in STREAMING
    ##       THEN WE WON'T MAKE IT THIS FAR

    if MODE_IS_STREAMING==1:
        if packet[0]<len(StreamingCommand):
            szCommand = StreamingCommand[packet[0]]
        else:
            szCommand = "command not in StreamingCommand string table?"
    else:
        if packet[0]>=len(DloadCommand):
            szCommand = "command not in DloadCommand string table?"
        else:
            szCommand = DloadCommand[packet[0]]

            if packet[0]==DLOAD_NAK:
                ShowPacketToUser = 1
                device_log(" _       __    _    ",1)
                device_log("| |\\ |  / /\\  | |_/ ",1)
                device_log("|_| \\| /_/--\\ |_| \\ ",1)
                device_log("      ( NAK )")

                if packet[2]>len(DloadNAKerrorCodes):
                    szCommand = "NAK (0x%X) reason not in DloadNAKerrorCodes string table? Possibly it's Illegal reason (do not use)" % packet[2]
                else:
                    szCommand += " REASON: " + DloadNAKerrorCodes[packet[2]]


    device_log("="*78,ShowPacketToUser)
    device_log("Command 0x%X (%s)" % (packet[0],szCommand),ShowPacketToUser)    ## it's been decoded at this point, so [0] not [1]
 
    return packet

def hdlc_encode(packet):
    #if port_trace:
    #    device_log("hdlc_encode(%i bytes)" % len(packet), 0, packet)

    write_size = len(packet)
    crc_value = calculate_crc(packet, write_size*8)
    encoded_buffer = []
    #encoded_buffer.append(chr(AHDLC_FLAG))
    encoded_buffer.append(AHDLC_FLAG)

    # Add data, escaping when necessary
    decodeIndex = 0
    while decodeIndex < write_size:
        try:
            value = ord(packet[decodeIndex])    #HACK shouldn't be needed if data is all clean
        except Exception, x:
            value = packet[decodeIndex]
        decodeIndex = decodeIndex + 1
        if value == AHDLC_FLAG or value == AHDLC_ESCAPE:
            value ^= AHDLC_ESC_M
            #encoded_buffer.append(chr(AHDLC_ESCAPE))
            encoded_buffer.append(AHDLC_ESCAPE)
        #encoded_buffer.append(chr(value))
        encoded_buffer.append(value)

    # Byte order crc
    byteOrderedCRC = [(crc_value & 0x00ff), ((crc_value >> 8) & 0x00ff)]

    # Add crc
    c = 0
    while c < 2:
        value = byteOrderedCRC[c]
        c = c + 1
        if value == AHDLC_FLAG or value == AHDLC_ESCAPE:
            value ^= AHDLC_ESC_M
            #encoded_buffer.append(chr(AHDLC_ESCAPE))
            encoded_buffer.append(AHDLC_ESCAPE)

        #encoded_buffer.append(chr(value))
        encoded_buffer.append(value)

    # Add trailing flag
    #encoded_buffer.append(chr(AHDLC_FLAG))
    encoded_buffer.append(AHDLC_FLAG)
    return encoded_buffer

CRC_16_L_POLYNOMIAL = 0x8408
CRC_16_L_SEED = 0xFFFF
CRC_16_L_OK = 0x0F47
CRC_SIZE = 2
CRC_TABLE_SIZE = 256

crc_table = [
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
]

def calculate_crc (buffer, bitLen):
    pBuf = 0
    crc_value = CRC_16_L_SEED
    while bitLen >= 8:
        try:
            crc_value = crc_table[(crc_value ^ ord(buffer[pBuf])) & 0x00ff] ^ (crc_value >> 8)
        except Exception, x:
            crc_value = crc_table[(crc_value ^ buffer[pBuf]) & 0x00ff] ^ (crc_value >> 8)
        bitLen = bitLen - 8
        pBuf = pBuf + 1
    #device_log("\nCRC is 0x%X, 0x%X, 0x%x" % (crc_value,~crc_value,0xFFFF&(~crc_value)) #timmy
    return (0xFFFF&(~crc_value))


def ReadCOMPort(read_length=1024):
    global RX_Buffer

    device_log("ReadCOMPort() Attempting to read %i bytes" % (read_length))
    retries = 0
    data_read = []
    while retries < MAX_READ_RETRIES and len(data_read) == 0:
        data_read = [ord(x) for x in device_handle.read(read_length)]   ## Orig treated as chars, as in 't','i','m','m','y'
                                                                        ## now it will be [116,105,109,109,121]
        if len(data_read) == 0:
            retries = retries + 1
            device_log("0 bytes received - Retry %i of %i" % (retries,MAX_READ_RETRIES))
            sleep(0.1)
    if retries == MAX_READ_RETRIES:
        device_log("ReadCOMPort(read_length=%d) 0 bytes received, Max read retries reached (TIMEOUT)"%read_length,0)
        return 0

    if len(data_read) > 0:
        device_log("%i new bytes read from COM port" % len(data_read), 0, data_read)
        #print [hex(ord(x)) for x in data_read])
        #print [chr(ord(x)) for x in data_read])

        RX_Buffer = RX_Buffer + data_read   ## append the data

    return len(data_read)


CMD_NOOP            = 0x06
CMD_RESET           = 0x0A

def GetNextPacket():
    global RX_Buffer,JustWroteData

    JustWroteData = 0   # used in HexPrint - only want to print SHA256 of written data
    
    device_log("Entering GetNextPacket()")

    AttemptsAtGettingFullPacket = 10
    NumAttemptsAtFullPacket     = 0
    FoundCompletePacket         = 0
    NumBytesRead                = 0

    while(NumAttemptsAtFullPacket < AttemptsAtGettingFullPacket):
        ## First, does buffer hold at least 5 bytes, i.e. 7E XX crc crc 7E, where XX is some command
        if len(RX_Buffer)>4:
            #device_log("len(RX_Buffer)>4")
            ## To be this far means we have enough data to satisfy HDLC encoding
            if RX_Buffer[0] != AHDLC_FLAG:
                device_log("WARNING: RX_Buffer is most likely holding garbage since it doesn't begin with 0x%X" % AHDLC_FLAG,0,RX_Buffer)
                ## Now begin hunting for the first AHDLC_FLAG byte
                Found = 0
                for i in range(len(RX_Buffer)):
                    if RX_Buffer[i] == AHDLC_FLAG:
                        Found = 1
                        break
                if Found==1:
                    device_log("Found the beginning AHDLC_FLAG")
                    RX_Buffer = RX_Buffer[i:]
                else:
                    device_log("WARNING: RX_Buffer only held garbage. It is now empty")
                    RX_Buffer = []

                device_log("Continuing in loop")
                next    ## continue on in either case, doesn't change NumAttemptsAtFullPacket
                        ## want the above checks to happen again

            else:
                device_log("begins with AHDLC_FLAG")
                ## To be this far means we have len(RX_Buffer)>4 and RX_Buffer[0] == AHDLC_FLAG:
                if RX_Buffer[-1] == AHDLC_FLAG:
                    ## buffer holds one complete packet OR multiple complete packets
                    device_log("FoundCompletePacket = 1")
                    FoundCompletePacket = 1
                    break
                else:
                    device_log("RX_Buffer[-1] != AHDLC_FLAG")
                    ## buffer may hold multiple complete packets and one incomplete OR just 1 incomplete packet
                    for i in range(4,len(RX_Buffer)):   # Buffer *must* look like this here 7E XX crc crc XX, so skip first 4 bytes
                        if RX_Buffer[i] == AHDLC_FLAG:
                            FoundCompletePacket = 1
                            device_log("FoundCompletePacket = 1")
                            break
                    if FoundCompletePacket==1:
                        break
                    else:
                        device_log("WARNING: RX_Buffer doesn't yet hold a complete packet")
                        NumBytesRead = ReadCOMPort()   # appends to RX_Buffer
                        NumAttemptsAtFullPacket += 1
        else:
            ## Buffer does *not* hold enough data
            if NumAttemptsAtFullPacket == 0:
                device_log("RX_Buffer only has %d bytes, not enough for complete packet. NumAttemptsAtFullPacket=%i out of %i" % (len(RX_Buffer),NumAttemptsAtFullPacket,AttemptsAtGettingFullPacket),0,RX_Buffer)
            else:
                device_log("RX_Buffer only has %d bytes, this is not enough to form a complete packet, will attempt to collect more, NumAttemptsAtFullPacket=%i out of %i" % (len(RX_Buffer),NumAttemptsAtFullPacket,AttemptsAtGettingFullPacket))
            NumBytesRead = ReadCOMPort()   # appends to RX_Buffer
            NumAttemptsAtFullPacket += 1

    if FoundCompletePacket == 1:
        temp = HDLC_Decode()
        #device_log("GetNextPacket() Returning from HDLC_Decode temp=",temp,"of type",type(temp)
        #device_log("-"*60
        return temp    # works on RX_Buffer
    else:
        # didn't find a complete packet. Must me something is wrong
        device_log("\nERROR? GetNextPacket() Didn't find a complete packet?? Returning [0]")
        return [0]

def SendCommand(command_id, **kwargs):
    global JustWroteData,CommandsSent
   
    #packet_buf = [chr(command_id)]
    packet_buf = [command_id]

    JustWroteData = 1   # used in HexPrint - only want to print SHA256 of written data
    
    if 'packet_data' in kwargs:
        packet_buf.extend(kwargs['packet_data'])

    if MODE_IS_STREAMING==0:
        if command_id<len(DloadCommand):
            device_log("DLOAD: SendCommand(0x%.2X) (%s)" % (command_id,DloadCommand[command_id]))
        else:
            device_log("DLOAD: SendCommand(0x%.2X) (%s)" % (command_id,"command not in DloadCommand string table?"))
    else:
        if command_id<len(StreamingCommand):
            device_log("STREAMING: SendCommand(%i bytes) (%s)" % (len(packet_buf),StreamingCommand[command_id]),0,packet_buf)
            stream_command_log(StreamingCommand[command_id],packet_buf)
        else:
            device_log("STREAMING: SendCommand(0x%.2X (%i bytes)) (%s)" % (command_id,len(packet_buf),"command not in StreamingCommand string table?"),0,packet_buf)
            stream_command_log("UNKNOWN",packet_buf)

    if CreateHashesOnly == True and MODE_IS_STREAMING==1:
        device_log("STREAMING: CreateHashesOnly == True, command *not* actually sent over COM port")
        #port_write(packet_buf)
    else:
        port_write(packet_buf)

    CommandsSent+=1
    device_log("STREAMING: CommandsSent=%i (just now incremented) hash table loc %i" % (CommandsSent,(CommandsSent-1)%32),0)
        

def int_to_hex_list(some_int, list_length=None):
    if some_int < 0:
        print "In int_to_hex_list(some_int=",some_int,"list_length=",list_length
        print "In int_to_hex_list(some_int=",type(some_int),"list_length=",type(list_length)
        print "Logic is probably wrong, please double check this"
        sys.exit(1)
    temp = hex(some_int)[2:]
    if temp[-1] == 'L':
        temp=temp[:-1]
    if len(temp)%2 == 1:
        temp = '0'+temp
    #temp_list = [chr(int(temp[x:x+2], 16)) for x in range(0, len(temp), 2)]
    temp_list = [int(temp[x:x+2], 16) for x in range(0, len(temp), 2)]
    if not list_length is None:
        temp_len = len(temp_list)
        while list_length != temp_len:
            #temp_list.insert(0, '\x00')
            temp_list.insert(0, 0x0)
            temp_len = temp_len + 1
    #print "In int_to_hex_list, returning temp_list=",temp_list," and type",type(temp_list),"\n\n"  # timmy
    return temp_list

def hex_list_to_int(some_list):
    if len(some_list)==0:
        device_log("\nWARNING: List lenth of 0 !! Returning 0 from 'hex_list_to_int' !! Is this what you wanted?",1)
        device_log("\nWARNING: List lenth of 0 !! Returning 0 from 'hex_list_to_int' !! Is this what you wanted?",1)
        device_log("\nWARNING: List lenth of 0 !! Returning 0 from 'hex_list_to_int' !! Is this what you wanted?\n",1)
        return 0
        
    if len(some_list)>16:
        device_log("\nWARNING: List lenth of %i is too long for 'hex_list_to_int', truncating to length 16" % len(some_list),1)
        ##import pdb; pdb.set_trace()
        some_list = some_list[:16]

    new_list = []
    for i in range(len(some_list)):
        try:
            x = hex(ord(some_list[i]))[2:]  #HACK shouldn't be needed if all data is clean
        except Exception, x:
            x = hex(some_list[i])[2:]
        if len(x) == 1:
            x = '0' + x
        new_list.append(x)
        
    a = ''.join(new_list)
    return int(a, 16)

def check_for_target():
    device_log("DLOAD: check_for_target()",0)
    SendCommand(CMD_NOOP)
    Attempts = 0
    while True:
        sleep(1)
        data_read = GetNextPacket()
        #device_log("Read",data_read,"of type",type(data_read)
        #device_log("="*50

        if len(data_read) >= 1:  # could have returned [0]
            if data_read[0]==DLOAD_ACK:
                device_log("DLOAD: Device responded",0)
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
                        device_log("DLOAD: SERIALNUMBER 0x%X" % hex_list_to_int(data_read[2:][::-1]),1)
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
                        device_log("DLOAD: CMD_HWID 0x%X" % hex_list_to_int(data_read[2:]),1)
                        sleep(1)
                else:
                    device_log("DLOAD: Phone did not respond to this command. This is *not* an error",1)
                    
                device_log("DLOAD: Trying DLOAD PKHASH with (note: not a problem if we get a NAK)",1)
                SendCommand(CMD_PKHASH)
                data_read = GetNextPacket()
				
                if data_read[0]==CMD_PKHASH:
                    device_log("DLOAD: Device responded with CMD_PKHASH packet",0)
                    
                return True
            else:
                device_log("DLOAD: Not the correct response, re-pinging device...",1,data_read,0)
        else:
            device_log("DLOAD: Nothing received back, re-pinging device...",1)

        Attempts+=1
        if Attempts > 5:
            DisplayResetDeviceErrorMessage(inspect.currentframe(),__file__)
            sys.exit()
        SendCommand(CMD_NOOP)

    return False
