#ifndef __CPR_H__
#define __CPR_H__
/*
  ====================================================================

  Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  cpr.h
  
  Public API for the CPR driver. 
  
  $Header: //components/rel/core.tx/6.0/api/power/cpr.h#1 $

  ====================================================================
*/
#include "VCSDefs.h"     //core/api/systemdrivers/VCSDefs.h  (VCS enums 
#include "ClockDefs.h"


/**
 * <!-- CPR_Enable -->
 *
 */ 
 DALResult CPR_Enable( VCSRailType eRail );
 
 
 /**
 * <!-- CPR_Disable -->
 *
 *
 */ 
 DALResult CPR_Disable( VCSRailType eRail );
 
 
 
/**
 * <!-- CPR_GetRailVoltageRecommendation -->
 *
 * Besides returning the current recommendation this function currently indicates which corner is being entered. 
 * Currently, when this is called CPR will be off or will be turned off.
 */  
 DALResult CPR_GetRailVoltageRecommendation( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageuV );

 


 /**
 * <!-- CprGetFloorAndCeiling -->
 *
 * @brief Get the floor and ceiling voltage for the specified mode. 
 *
 * @param rail_id : CLOCK_VREG_VDD_MSS (or VCS_RAIL_MSS)
 * @param mode : enum for svs, svs+, nom, turbo etc. 
 * @param floorVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 * @param ceilingVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 *
 * @return DAL_SUCCESS if there are no issues filling the provided pointers. 
 */
 DALResult CprGetFloorAndCeiling( VCSRailType rail_id, VCSCornerType mode, uint32* floorVoltageUV, uint32* ceilingVoltageUV );
 

 /**
 * <!-- CPR_GetEldoVoltageRecommendation -->
 *
 * @brief Return the CPR eLDO voltage for the corner.
 *
 * @param eRail : Which rail to provide information for.
 * @param eCorner : SVS, NOM, etc...
 * @param nVoltageUV : The recommended voltage is returned in this uint32*.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
 DALResult CPR_GetEldoVoltageRecommendation(VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageUV);

 

 /**
 * <!-- CPR_Init_Phase1 -->
 *
 * @brief  Called by rcinit via  currently.  Initializes CPR clocks, ISRs and other setup requirements.
 *
 * @param eRail : Which rail to initialize
 *
 * @return DAL_SUCCESS if the rail is valid.
 */
 DALResult CPR_Init_Phase1(VCSRailType  eRail);   //Get the data structures and fuses read. (called by RC init currently)



 /**
 * <!-- CPR_Init_Phase2 -->
 *
 * @brief  Indication to CPR that the system is in it's lowest power state and can do bootsafe voltage training.
 *
 * @param eRail : Which rail to initialize
 *
 * @return DAL_SUCCESS if the rail is valid.
 */
 DALResult CPR_Init_Phase2(VCSRailType  eRail);  



 /**
 * <!-- CPR_GetSafeVoltage -->
 *
 * @brief Get the bootsafe measured voltage for the corner
 *
 * @param eRail : Which rail to provide information for.
 * @param eCorner : VCS_CORNER_LOW_MINUS, VCS_CORNER_LOW
 * @param nVoltageUV : The recommended voltage is returned in this uint32*.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
 DALResult  CPR_GetSafeVoltage(VCSRailType  eRail, VCSCornerType eCorner, uint32 *nSafeVoltageuV);



/**
 * <!-- CPR_RelinquishedControl_Enter -->
 *
 * @brief CPR will not be operating for an extended period of time.  
 *
 * All CPR closed loop recommendations will be reset to higher safe values and
 * enable/disable calls will do nothing until CPR_RelinquishedControl_Exit is called.
 *
 * @param eRail : Which rail to be switched.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
 DALResult CPR_RelinquishedControl_Enter(VCSRailType  eRail);  

 
 /**
 * <!-- CPR_RelinquishedControl_Exit -->
 *
 * @brief CPR should prepare to be used again after an extended period of time off. 
 *
 * @param eRail : Which rail to be switched.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
 DALResult CPR_RelinquishedControl_Exit(VCSRailType  eRail); 

  

  

#define CPR_MASK_MSS_MODEM_SLEEP     0x1
 /**
 * <!-- CPR_MaskSensors -->
 *
 * @brief Some special cases require a change in the CPR configuration. CPR_MASK_... masks 
 *        are defined in this cpr.h file and can be applied when needed. 
 *
 * @param mask : A CPR_MASK...mask as defined above.
 *
 * @return DAL_SUCCESS if the mask was understood and applied correctly.
 */
DALResult CPR_MaskSensors(uint32 mask);


 /**
 * <!-- CPR_UnmaskSensors -->
 *
 * @brief Some special cases require a change in the CPR configuration. CPR_MASK_... masks 
 *        are defined in this cpr.h file and can be applied when needed. 
 *
 * @param mask : A CPR_MASK...mask as defined above.  
 *
 * @return DAL_SUCCESS if the mask was understood and applied correctly.
 */
DALResult CPR_UnmaskSensors(uint32 mask);

  
  
 #endif

