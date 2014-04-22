//====================================================================================================//
//================================ Meta-Scripts ReadMe.txt ===========================================//
//====================================================================================================//
//
//  Name:                                                                     
//    ReadMe_MSM8996.txt 
//
//  Description:                                                              
//    Basic script intro ReadMe.txt
//                                                                            
// Copyright (c) 2012 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.        
//
//
//
//
//                      EDIT HISTORY FOR FILE
//  This section contains comments describing changes made to the module.
//  Notice that changes are listed in reverse chronological order.
//
// when       who             what, where, why
// --------   ---             ---------------------------------------------------------
// 08/14/2015 JBILLING        Created for 9645 


This document describes the following:
    - Release notes, limitations, fixes, Lauterbach T32 approved versions.
    - Basic instructions for using features available for JTag on Qualcomm MSM chipsets.
    
Release Notes
    Date: 8/14/2015
    Stubbed for 9x45

    
    
Existing limitations
    std_debug
       
    std_loadsim
       


Release 5/25/2015
Lauterbach Trace32 builds verified:
    May 19, 2015 (build 63096.)

This document assumes the following:
    An up to date Lauterbach Trace32 license and (for live debugging) working JTag pod.
    Python 2.6 or later is installed and  available to system path
    Target is properly powered. In the case of using std_Debug scripts
        target has software and successfully boots to subsystem of choice.
    Trace32 is installed and computer is successfully connected to Trace32 pod
    Using Windows for computer OS.
    See chipset help pdf for further details
        
A note on T32 path
    All the scripts described are embedded into t32 path, so they do not require absolute accessing.
    I.E. a simple "do std_debug" will invoke the std_debug script. 
    It is not necessary to call it directly (i.e. "do common\std_debug.cmm" is not necessary)
        
Help menus
    Many of the scripts have help menus to describe functionality and usage. 
    These can be accessed in the following way
        do std_debug help
        do std_cti help
        do std_loadsim help
        do std_loadsyms_mpss help 
        do std_loadsyms_rpm help 
        
        
std_loadbuild
    This is primarily a GUI which allows loading the a build via JTag to the target.
    It has some other features such as individually swapping out images (such as tz or xbl based images)
    You can also set T32's paths for the different image builds here. Pressing the 'map' button
    will intercom to other open T32 windows to change their paths as well.
    Deviceprogrammer is for internal use only.
        
std_debug
    std_debug is a suite of scripts intended to facilitate JTag attach to the core of choice.
    There are two primary features within std_debug:
        std_debug boot up debug
            This is designed to get the user attached to desired subsystem with specified breakpoints and options.
            If 'main' is specified as breakpoint, the subsystem will halt at entry. Otherwise the subsystem
            will go immediately from main(), setting all error breakpoints, which preserves system sync
            This is offered through the primary GUI (obtained by typing 'do std_debug' in the APPS0 T32 window) or
            by command line (type 'do std_debug help' in the APPS0 T32 window for command line instructions)
            
            
        hotattach menus
            These menus are present on most T32 windows and they offer live debug without needing to restart the system.
            Q6 based processors (MPSS) have functionality to wakeup, attach, set breakpoints, and start ETM tracing 
                without halting the processor
                
        Plusvm option has been added to all symbol loading scripts. This is invoked only via command line at this time, via the 'extraoption' option.
            Plusvm is intended for faster ETM trace processig (After a trace is captured, JTag searches DDR to correlate traces with actual memory.
            With Plusvm, ELF gets loaded onto JTag pod and so this second time consuming and invasive step doesn't have to be performed. 
            However, the ELF will take up room on the jtag pod and so tracing room can be limited.
            e.g.: do std_loadsyms_adsp NULL NULL NULL loadsecondelf NULL extraoption=plusvm,silent
                
std_loadsim
    std_loadsim is the suite of scripts designed to open a post-mortem T32 simulator session from a crash dump.
    GUI - the GUI will open automatically when opening a session. If you want to open another GUI, simply type 'do std_loadsim'
    Command line - command line options are available. For more information, type 'do std_loadsim help'
    
        
        
        
        
        
        
        
        
        
        
        
