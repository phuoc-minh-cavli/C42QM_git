/*=======================================================================
 * FILE:        nand_stubs.c
 *
 * SERVICES:   Stubs needed for building nand_tools and hostdl. 
 *
 * GENERAL DESCRIPTION
 *   Stubs needed for building nand_tools and hostdl.
 *
 *        Copyright © 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/NandJtagTools/nand_stubs.c#2 $ $DateTime: 2019/03/02 02:13:01 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/29/08     mm      Initial version
 *==================================================================*/
void rex_int_free()
{

}
void rex_int_lock()
{

}
/*  need this optherwise DMOV will complain in dmov_7500.c */
/*void memory_barrier(void)
{
}*/
