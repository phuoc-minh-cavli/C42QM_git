#============================================================================
#  Name:                                                                     
#    tbc_cleanpack.py 
#
#  Description:                                                              
#    None 
#                                                                            
# Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.        
#============================================================================
#                                                                            
# *** AUTO GENERATED FILE - DO NOT EDIT                                      
#                                                                            
#============================================================================
def exists(env):
   return env.Detect('tbc_cleanpack')

def generate(env):
   env.AddUsesFlags('USES_CLEAN_PACK',  True)
