/*! \file pm_diag.c
*
*  \brief PMIC Diag initialization related functions.
*  \n
*  &copy; Copyright 2014-2015, 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pm_diag.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/14   kt      Created
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "pm_diag.h"

/*===========================================================================

                     FUNCTION DEFINITION

===========================================================================*/
const char * pm_diag_model_lookup (pm_model_type model_type)
{
  const char *model_name;

  /*-----------------------------------------------------------------------*/
  /* Lookup PMIC Model name based on the specified PMIC Model type.        */
  /*-----------------------------------------------------------------------*/
     
  switch(model_type)
  {
  case PMIC_IS_PM8941:

     model_name = "PM8941";
     break;

  case PMIC_IS_PM8019:

     model_name = "PM8019";
     break;

  case PMIC_IS_PM8841:

     model_name = "PM8841";
     break;

  case PMIC_IS_PM8026:

     model_name = "PM8026";
     break;

  case PMIC_IS_PM8110:

     model_name = "PM8110";
     break;

  case PMIC_IS_PMA8084:

     model_name = "PMA8084";
     break;

  case PMIC_IS_PMI8962:

     model_name = "PMI8962";
     break;

  case PMIC_IS_PM8916:
  
     model_name = "PM8916";
     break;

  case PMIC_IS_PMD9645:

     model_name = "PMD9645";
     break;

  case PMIC_IS_PM8994:

     model_name = "PM8994";
     break;

  case PMIC_IS_PMI8994:

     model_name = "PMI8994";
     break;

  case PMIC_IS_PM8004:

     model_name = "PM8004";
     break;

  case PMIC_IS_PM8909:

     model_name = "PM8909";
     break;

  case PMIC_IS_PM8950:

     model_name = "PM8950";
     break;

  case PMIC_IS_PMI8950:

     model_name = "PMI8950";
     break;

  case PMIC_IS_PMK8001:

     model_name = "PMK8001";
     break;

  case PMIC_IS_PME9205:

     model_name = "PME9205";
     break;

  case PMIC_IS_UNKNOWN:
     
     model_name = "UNKNOWN";
     break;

  case PMIC_IS_INVALID:
     
     model_name = "INVALID";
     break;

  default:
     
     model_name = "NA";
     break;
  }

  return model_name;

}

void pm_diag_init (void)
{
   pm_diag_pwr_rails_init();

   pm_diag_clocks_init();

   pm_diag_gpios_init();

   pm_diag_mpps_init();

   pm_diag_peek_poke_init();

   pm_diag_pmic_info_init();

   pm_diag_pbs_info_init();
}

