import struct, os, sys, getopt,string,re,shutil
import subprocess as sub
from types import *
import time


#needed executables
FH_Loader="fh_loader.exe"
LS_usb="lsusb.exe"
QSaharaServer="QSaharaServer.exe"

tempfilename="tempfile.txt"
firehosefilename="firehosecommand.txt"
scriptdonefilename="fh_load_script_done.txt"

tempfile=None
firehosefile=None
scriptdonefile=None
EDLArgumentOptions = ["searchpath=","noprompt","pathtodeviceprogrammer=","contentsxml=","xml=","files=","porttracename=","notfiles=","loaderpaths=","logdirectory=","memoryname=","skipfh","skipqsahara","verify_programming"]
pathtodeviceprogrammer = None
device_programmer = None
ComPort = None
ContentsXML = None
SendXML = None
Files = None
searchpaths        = []
porttracename="port_trace.txt"
notfiles=None
LoaderPaths="None"
LogDirectory=None
memoryname = "ufs"
verifyprogramming=False
skipfh = False # Skip FireHose (FH) only for T32 debugging.
skipqsahara = False
# Want to match this below
# CD.DO &BOOT_BUILDROOT/&PROGRAMMER LOAD searchpaths=&SEARCHPATHS xml=&XML files=xbl.elf,tz.mbn,devcfg.mbn,emmc_appsboot.mbn,hyp.mbn,rpm.mbn,Pmic.elf


def Step_GetDeviceProgrammer():
  global device_programmer,pathtodeviceprogrammer
  print "\n"*10

  ## output will look like this
  ## Qualcomm HS-USB QDLoader 9008 (COM27)
  sz = "fh_loader.exe "

  if ContentsXML is not None:
      sz += "--contentsxml=%s --noprompt " % ContentsXML
  for ThisPath in searchpaths:
        sz += "--search_path=%s " % ThisPath

  sz += "--verify_build"
  sz += " --memoryname=%s" % memoryname

  print PathToFhloader +"\\" + sz
  #response = os.system(PathToFhloader+sz)
  response = external_call(PathToFhloader +"\\" + sz)

  m = re.search("QSaharaServer.exe -p \\\\\\\\.\\\\COM\d+ -s 13:(.+) -b (.+)", response)

  if type(m) is not NoneType:
      device_programmer     = m.group(1)
      print("\n\ndevice_programmer = %s" % device_programmer)
      print("pathtodeviceprogrammer = %s" % pathtodeviceprogrammer)

      if pathtodeviceprogrammer[-1] == '\r':
          pathtodeviceprogrammer = pathtodeviceprogrammer[0:-2]

  print "\n" * 10


def PrintFailLog():
    global tempfile, scriptdonefile, LogDirectory
    #This is for T32 wrapper to detect failure

    fileio = open(tempfile, "w")
    fileio.write("Result:\nFAILED\n")
    fileio.close()    
    shutil.copyfile(tempfile, scriptdonefile)
    print "Failed, please check the logs in folder " + LogDirectory
    return

def exit_with_failure(failnum):
    PrintFailLog()
    sys.exit(failnum)

        
def Step_Firehose():
    global ComPort,ContentsXML,SendXML,Files,porttracename

    print "\n"*10

    sz = "fh_loader.exe --port=\\\\.\\%s --nop  --noprompt --porttracename=%s " % (ComPort,porttracename)

    if ContentsXML is not None:
        sz += "--contentsxml=%s " % ContentsXML
    if SendXML is not None:
        sz += "--sendxml=%s " % SendXML
    if notfiles is not None:
        sz += "--notfiles=%s " % notfiles
    if Files is not None:
        sz += "--files=%s " % Files
    if verifyprogramming == True:
	    sz += "--verify_programming "

    for ThisPath in searchpaths:
        sz += "--search_path=%s " % ThisPath

    sz += " --memoryname=%s" % memoryname
    firehosecommand = PathToFhloader + "\\" + sz
    print "FirehoseCommand:"
    print firehosecommand
    
    fileio = open(tempfile, "w")
    fileio.write("FirehoseCommand\n")
    fileio.write(firehosecommand + "\n")
    fileio.close()
    
    shutil.copyfile(tempfile, firehosefile)
    response = external_call(firehosecommand)
    if "All Finished Successfully" not in response:
        ## something went wrong, exit with error code 1
        print("\n\nERROR: Something went wrong while programming.\n\n")
        exit_with_failure(-1)
    


def Step_Sahara():
    global device_programmer,ComPort,pathtodeviceprogrammer

    print "\n"*10

    if pathtodeviceprogrammer==None:
        print("ERROR: No Path to DeviceProgrammer provided, i.e. use --pathtodeviceprogrammer=")
        #This is for T32 wrapper to detect failure
        exit_with_failure(-1)

    if pathtodeviceprogrammer[-1]!="\\" and pathtodeviceprogrammer[-1]!="/":
        pathtodeviceprogrammer += "\\"


    device_programmer = None
    try:
        for file in os.listdir(pathtodeviceprogrammer):
            if 'prog_emmc_firehose' in file:
                print "\n\nFOUND possible file '%s'\n\n" % file
                if 'ddr' in file:
                    device_programmer = file
                    break
    except Exception, x:
        print "\nNo DEVICEPROGRAMMER found at '%s'.\nReason: %s\n\n" % (x,pathtodeviceprogrammer)


    if device_programmer==None:
        print("ERROR: No Path to DeviceProgrammer provided")
        
        exit_with_failure(-1)
        
    if ComPort==None:
        print("ERROR: ComPort is None. Did the lsusb.exe step fail?")
        
        exit_with_failure(-1)


    sahara = PathToQSahara + "\\"+ "QSaharaServer.exe -p \\\\.\\%s -s 13:%s -b %s" % (ComPort,device_programmer,pathtodeviceprogrammer)

    print sahara
    
    #response = os.system(sahara)
    response = external_call(sahara)

    if "Sahara protocol completed" not in response:
        ## something went wrong, exit with error code 1
        print("\n\nERROR: Something went wrong\n\n")

        exit_with_failure(-1)

    print "\n" * 10


def usage():
    print """
Usage: python edl_qsahara_fh_loader.py [OPTION...]


  """

def external_call(command, capture_output=True):
  global porttracename
  print("\tRunning %s" % command)
  errors = None
  output = ""
  try:
      if capture_output:
          p = sub.Popen(command, stdout=sub.PIPE, stderr=sub.PIPE, shell=True)

          while p.poll() is None:
            line = p.stdout.readline()
            if line:
              print line
              output += line

          output2, errors = p.communicate()
          print output2
          output += output2
      else:
          output = os.system(command)
  except Exception, x:
        print("ERROR: executing command '%s'. Reason: %s" % (str(command), x))
        #clean_up()
        exit_with_failure(-1)
  finally:
      #if not output is None:
      #    print "Result: %s" % output
      if (not errors is None) and (not errors == ""):
          print "Process stderr: %s" % errors
  return output


def Step_lsusb():
  global ComPort
  print "\n"*10

  ## output will look like this
  ## Qualcomm HS-USB QDLoader 9008 (COM27)

  sz = PathToLsusb + "\\" + "lsusb.exe"

  ##response = os.system(sz)
  response = external_call(sz)

  m = re.search("9008 \((COM\d+)\)", response)
  if type(m) is not NoneType:
      ComPort = m.group(1)
      print("ComPort = %s" % ComPort)

  print "\n" * 10





try:
    opts, args = getopt.getopt(sys.argv[1:], "", EDLArgumentOptions)
except getopt.GetoptError, err:
        # print help information and exit:
        print("\mERROR\m")
        print str(err) # will print something like "option -a not recognized"
        print str(err) # will print something like "option -a not recognized"
        print str(err) # will print something like "option -a not recognized"
        print("\n")

        exit_with_failure(-1)


for opt in sys.argv[1:]:
    if opt[0:2]!="--":
        #PrintBanner("ERROR")
        print "This argument '%s'\n\n\tdoesn't start with two dashes, i.e. '--', please correct\n\n" % opt
        
        exit_with_failure(-1)


noprompt = False


for o, a in opts:
    if o == "--searchpath":
        searchpaths.append( a.strip() )
    elif o == "--noprompt":
        noprompt = True
    elif o == "--pathtodeviceprogrammer":
        pathtodeviceprogrammer = a
    elif o == "--contentsxml":
        ContentsXML = a

    elif o == "--xml":
        SendXML = a
    elif o == "--notfiles":
        notfiles = a
    elif o == "--porttracename":
        porttracename= a
    elif o == "--files":
        Files = a
    elif o == "--loaderpaths":
        LoaderPaths = a
    elif o == "--logdirectory":
        LogDirectory = a
    elif o == "--memoryname":
        memoryname = a
    elif o == "--skipfh":
        skipfh=True
    elif o == "--skipqsahara":
        skipqsahara=True
    elif o == "--verify_programming":
        verifyprogramming=True


if  os.path.isfile(LoaderPaths + "\\"+FH_Loader) and\
    os.path.isfile(LoaderPaths + "\\"+LS_usb) and \
    os.path.isfile(LoaderPaths + "\\"+QSaharaServer):
        PathToLsusb   =  LoaderPaths
        PathToQSahara =  LoaderPaths
        PathToFhloader = LoaderPaths

else:
    print "Cannot access LoaderPaths or invalid LoaderPaths given. Exiting"
    exit_with_failure(-1)
        



if os.path.isdir(LogDirectory):
    tempfile=os.path.realpath(LogDirectory + "\\" + tempfilename)
    firehosefile=os.path.realpath(LogDirectory + "\\" + firehosefilename)
    scriptdonefile=os.path.realpath(LogDirectory + "\\" + scriptdonefilename)
else:
    print "Cannot access log directory or invalid log directory given. Exiting"
    exit_with_failure(-1)
    
if os.path.isdir(pathtodeviceprogrammer):
    print "pathtodeviceprogrammer=" + pathtodeviceprogrammer
else:
    print "Cannot access deviceprogrammer directory directory or invalid log directory given. Exiting. Failed path:" + pathtodeviceprogrammer
    exit_with_failure(-1)

Step_GetDeviceProgrammer()
Step_lsusb()
if skipqsahara == False:
    print "Running QSaharaServer"
    Step_Sahara()
else:
    print "Skipping Sending image using QSaharaServer, assuming image was already sent"


# Skip FireHose (FH) only when debugging.
if skipfh == False:
    print "Running fh_loader..."
    Step_Firehose()
else:
    print "Skipping firehose step, that means you need to start the fh_loader later."

print "Finished successfully!"
fileio = open(tempfile, "w")
fileio.write("Result:SUCCESS\n")
fileio.close()
shutil.copyfile(tempfile, scriptdonefile)
sys.exit(0)

