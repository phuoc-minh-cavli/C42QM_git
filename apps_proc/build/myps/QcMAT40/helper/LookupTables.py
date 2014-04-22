"""
 LookupTables.py

Description: This file provides the value to string OR string to value
             mapping.

 Copyright (c) 2015 - 2016 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/build.tn/4.0/myps/QcMAT40/helper/LookupTables.py#1 $

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 Nov 25, 2015   swapneel  First Draft.
 ==========================================================================
 """
 
 # The Strings used in below code are not added to Constants.

class LookupTable(object):
     def getSymbolTypeString(self, symbolTypeId):
         symbolTypeDict = {0:"NOTYPE",1:"O",2:"F",3:"S",4:"FL",5:"C",6:"T"}
         return symbolTypeDict[symbolTypeId]

     def getSymbolVisibilityString(self, symbolVisibilityId):
         symVisibilityDict = {0:"D",1:"I",2:"H",3:"P"}
         return symVisibilityDict[symbolVisibilityId]

     def getSymbolBindingString(self, symBindingId):
         symBindingDict = {0:"L",1:"G",2:"W"}
         return symBindingDict[symBindingId]
