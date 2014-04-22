/**
 * @file pcie_phy.h
 * @brief
 * Provides interfaces to access the PCIe PHY HW.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#ifndef __PCIE_PHY_H__
#define __PCIE_PHY_H__

/* ============================================================================
**  Function : pcie_phy_init
** ============================================================================
*/
/**
 * Performs initialization of the PCIe PHY.
 *
 * @param[in]  pcie_dev    Pointer to the pcie device object
 *
 * @return     pcie_return_status
 *    Status of the initialization.
 */
enum _pcie_return_status pcie_phy_init(void);

/* ============================================================================
**  Function : pcie_phy_config_update
** ============================================================================
*/
/**
 * Update PHY additional settings.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_phy_config_update(void);

/* ============================================================================
**  Function : pcie_phy_clamp_interfaces
** ============================================================================
*/
/**
 * CLAMP digital and logical IO.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_phy_clamp_interfaces(void);

/* ============================================================================
**  Function : pcie_phy_unclamp_interfaces
** ============================================================================
*/
/**
 * Unclamp digital and logical IO.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_phy_unclamp_interfaces(void);

#endif /* __PCIE_PHY_H__ */
