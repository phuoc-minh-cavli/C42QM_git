#!/usr/bin/env python
'''
=======================================================================================================
fb_info_lib.py:

Usage:
   This script takes in a Build ID as input and returns the full findbuild information as a dictionary

 Copyright (c) 2011-2018 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR 
-----------------------------------------------------------------------------------------------
 $Header: //components/rel/build.glue/1.0/lib/fb_info_lib.py#9 $
 $DateTime: 2021/07/06 13:11:49 $
======================================================================================================
'''

import os, sys, shlex, subprocess,platform,json,urllib
url_request_lib = True
if sys.version_info[0] < 3: # python 2.x
   from urllib import urlencode
   try:
      import urllib2 as url_request
   except Exception as x:
      url_request_lib = False
      #print('Install python urllib2 module!')
else: #Python 3.x
   from urllib.parse import urlencode
   xrange = range
   try:
      import urllib.request as url_request
   except Exception as x:
      url_request_lib = False
      #print('Install python urllib2 module!')

# SOAP client library
python_suds_client = True
try:
   import suds.client as suds 
except Exception as x:
   python_suds_client = False
   #print('Install python suds client to use FindBuild WSDL service!')
   #print('Install python urllib2 module!')
   
class convertObject(object):
    def __init__(self, d):
        self.__dict__ = d

def decodekey(encoded_key):
   key_value = ''
   key_len = int(encoded_key[:2])
   encoded_key = encoded_key[2:] 
   for index in xrange(key_len*2):
      if index%2 !=0:
         key_value += encoded_key[index]
   #Return
   return key_value.split(':',1)

# Establish session with host
def connect_to_host(host_url=None):
    authhost = 'https://qclogin.qualcomm.com'
    #Get config
    configfp = None
    if sys.platform.startswith("linux"):
       configfp = open('/prj/qct/targetsw/targetenggmeta/automation/configkey', 'r')
    else:
       configfp = open('\\\\jerry\\targetenggmeta\\automation\\configkey', 'r')	
    line = configfp .readline()
    (key,key_val) = decodekey(line.rstrip('\r\n'))
    configfp.close()
    cookiehand = url_request.HTTPCookieProcessor()
    conn_mngr = url_request.HTTPPasswordMgrWithDefaultRealm()
    conn_mngr.add_password(user=key,passwd=key_val, uri=authhost,realm=None)
    auth_handler = url_request.HTTPBasicAuthHandler(conn_mngr)
    opener = url_request.build_opener(auth_handler,cookiehand)
    url_request.install_opener(opener)
    if host_url: #Try to establish connection with host
       req = url_request.Request(url=host_url, headers={'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:2.0) Gecko/20100101 Firefox/4.0'})
       url_request.urlopen(req).read()

#Send get request
def get(request_url):
    req = url_request.Request(url=request_url, headers={'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:2.0) Gecko/20100101 Firefox/4.0'})
    return url_request.urlopen(req,timeout=600).read()

#send post request
def post(request_url,data):
    req = url_request.Request(url=request_url, data=urlencode(data).encode(), headers={'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:2.0) Gecko/20100101 Firefox/4.0'})
    response = url_request.urlopen(req,timeout=600).read()
    return response

#Check missing security bulletin CRs
def is_security_bulletin_compliant(meta_id, si_build_list):
    data = { 'spbuildid': meta_id,'sibuildids': ",".join(si_build_list)}
    security_bulletin_url = 'http://psitool.qualcomm.com/buildannouncement/issecuritybulletincompliant/'
    connect_to_host(security_bulletin_url)
    response = convertObject(json.loads(post(security_bulletin_url,data)))

    re_try = 1
    if response.StatusCode!=0 and  re_try <=5 :
        print("Error : "+ str(response.StatusCode) + " - " + str(response.Description))
        response = convertObject(json.loads(post(security_bulletin_url, data)))
    if response.StatusCode == 0 :
        #print('********Missing Bulletin CRs***************')
        #print('Missing Bulletin CRs Count : '+ str(response.MissingBulletinCrCount))
        print('Blocking CRs Count : '+ str(response.BlockingCrCount))
        print('Description : '+ str(response.Description))
        print('Exceptions : '+ str(response.Description))
        #print('Data : '+ str(response.Data))
        #print('*******************************************')
        return (response.BlockingCrCount > 0)
    #Unable to establish connection, skip security bulletin crs check
    return False

class fbInfo:

   #==============================================================================
   #  Constructor
   #==============================================================================
   def __init__(self, on_linux,logger,fb_nearest = False, local_image=False):
      self.fbPath = None
      self.on_linux = on_linux
      self.logger=logger
      self.fb_nearest = fb_nearest   
      self.local_image = local_image   
      
   #==============================================================================
   #  get_fbInfo(buildID)
   #
   #   Input:
   #      buildID: Required build ID to search
   #   Output:
   #      fbInfo: Dictionary with information from findbuild search
   #==============================================================================
   def get_fbInfo(self, buildID, throw_ex=True):
      '''
      C:\\Users\\qctecmdr>findbuild M8974AAAAANAAM002127.2 -lo
      Build:          M8974AAAAANAAM002127.2
      Location:       \\fosters\builds435\INTEGRATION\M8974AAAAANAAM002127.2
      Comments:       Build completed, labeled, and marked for retention.
      Status:         Approved  [Approved for Testing]
      BuildDate:      08/09/2013 16:25:19
      Type:           INTEGRATION
      ProductLine:    MPSS.DI.1.0 OSVer:  HWPlatform: MSM8974
      MainMake:       modem_proc/build/ms/build.sh 8974.gps.test 8974.gen.test BUILD_VER=002127 -k
      OSFilerPath:    \\fosters\builds435\INTEGRATION\M8974AAAAANAAM002127.2
      LinuxPath:      /prj/qct/asw/crmbuilds/fosters/builds435/INTEGRATION/M8974AAAAANAAM002127.2
      Compiler:       QDSP6gcc
      PLF:            target_main.plf
      PLFP4Location:  //source/qcom/qct/mpss/integration/Dime/1.0/main/latest/target_main.plf#3043
      Submitter:      qctecmdr
      Approver:       qctecmdr
      Archived:       False
      Shippable:      True
      IsSbaStrict:    False
      SiteLocation:   SanDiego
      '''
      #---------------------
      # GET FINDBUILD OUTPUT
      #---------------------
      if  self.local_image:    
         fbInfo = {}      
         fbInfo['Location'] = buildID    
         fbInfo['LinuxPath'] = buildID     
         return fbInfo      
      #########################################      
      fb_output = ""      
      # If it's first time, run the subroutine to set findbuild path
      if not self.fbPath:
         fb_output = self.__set_fbPath(self.on_linux, buildID)
         # If still path is not found abort
         if not self.fbPath and throw_ex:
            raise Exception("Findbuild location to use was not initialized")
      
      # If it's not the first time, findbuild path is already set. Call directly.
      else:
         if self.fb_nearest == True:
            fb_output = self.runCmd([self.fbPath, buildID, "-lo","-nearest"])
         else:
            fb_output = self.runCmd([self.fbPath, buildID, "-lo"]) 
      
      
      # PROCESS FINDBUILD OUTPUT
      #-------------------------
      fbInfo = {}
      self.fb_log ("Findbuild returned:\n" + fb_output)    #Debug message
      
      # Check if build id is valid
      if "No Record Found!" in fb_output:
         return None
         
      fb_output = fb_output.split("\n")
      
      # Parse the output and split it into key value pairs
      for each_line in fb_output:
         if "MainMake:" in each_line:
            each_line = each_line.split(': ',1)
            fbInfo[each_line[0].strip()] = each_line[-1].strip()
         else:
            each_line = each_line.split()
            new_line = []
            for word in each_line:
               if ':' in word or len(new_line) == 0:
                  new_line.append(word)
               else:
                  new_line[-1] = new_line[-1] + " " + word
            for each_pair in new_line:
               each_pair = each_pair.split(':',1)
               fbInfo[each_pair[0].strip()] = each_pair[-1].strip()
          
      return fbInfo
   
   
   #==============================================================================
   #  fb_log function
   #
   #   Input:
   #      String
   #   Output:
   #      if lg object use the lg.log 
   #      else print to stdout
   #==============================================================================
   def fb_log(self, logstmt):
      if self.logger:
         self.logger.log(logstmt)
      else:
         print logstmt

   #==============================================================================
   #  __set_fbPath(on_linux, buildID)
   #      Private function to set findubild path first time
   #
   #   Input:
   #      on_linux: Platform
   #   Output:
   #      findbuild output
   #==============================================================================
   def __set_fbPath(self, on_linux, buildID):
      try:
         self.fb_log("\nfb_info_lib: Calling FindBuild from local environment")  #Debug Statement - self.fb_log FindBuild Command
         self.fbPath = "Findbuild"   
         if self.fb_nearest:
            stuff = self.runCmd([self.fbPath, buildID, "-lo","-nearest"])
         else:
            stuff = self.runCmd([self.fbPath, buildID, "-lo"]) 	
         #self.fb_log("FindBuild returned: " + ' '.join(stuff)+"\n")  #Debug Statement - self.fb_log FindBuild Output
         if len(stuff) == 0 and throw_ex:
            raise Exception("Findbuild returned empty on stdout")
      except:
         self.fb_log("fb_info_lib: Could not use FindBuild from environment.")
         if on_linux:
            self.fb_log("fb_info_lib: Trying /pkg/qct/qctss/linux/bin/FindBuild") #Debug Statement - self.fb_log FindBuild Command
            self.fbPath = "/pkg/qct/qctss/linux/bin/FindBuild"
         else:
            self.fb_log("fb_info_lib: Trying \\\\stone\\CRMTools\\Smart\\nt\\bin\\FindBuild") #Debug Statement - self.fb_log FindBuild Command
            self.fbPath = "\\\\stone\\CRMTools\\Smart\\nt\\bin\\FindBuild"
         try:
            if self.fb_nearest == True:
               stuff = self.runCmd([self.fbPath, buildID, "-lo","-nearest"])
            else:
               stuff = self.runCmd([self.fbPath, buildID, "-lo"]) 			
            #print("FindBuild returned: " + ' '.join(stuff)+"\n") #Debug Statement - Print FindBuild Output
         except:
            stuff = None    # to prevent "variable referenced before assignment" error in return
            self.fb_log("FindBuild could not be found.")
            self.fbPath = None
      
      return stuff
      

   #==============================================================================
   #  runCmd function
   #
   #   Input:
   #      command: String of command that needs to be entered in the command prompt
   #   Output:
   #      cmd_out: The result of running the command
   #==============================================================================
   def runCmd(self,command):   
      # print "Executing:", command, "..."
      # Call process
      if type(command) is not list:
         command = shlex.split(command);
      subproc = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
      stdout, stderr = subproc.communicate()
      if not stderr:
         return stdout
      else:
         self.fb_log(command + " failed! : " + stderr)
         return None

