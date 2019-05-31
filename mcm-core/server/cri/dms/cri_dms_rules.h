
/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef CRI_DMS_RULES
#define CRI_DMS_RULES

#include "utils_common.h"
#include "cri_dms_core.h"

// DMS rule structure.
typedef struct cri_dms_rules_generic_rule_data_type
{
    cri_core_dms_op_mode_enum_type operating_mode;
}cri_dms_rules_generic_rule_data_type;

/***************************************************************************************************
    @function
    cri_dms_rules_set_modem_rule_check_handler

    @brief
    This function returns "true", if the current operatig mode is same as the
    mode mentioned in rule data otherwise "false".

    @param[in]
        rule_data
            rule data for set modem request.

    @param[out]
        none

    @retval
    int - true if the rule is met, false otherwise.
***************************************************************************************************/
int cri_dms_rules_set_modem_rule_check_handler(void *rule_data);


/***************************************************************************************************
    @function
    cri_dms_rules_generic_rule_data_free_handler

    @brief
    Free rule data

    @param[in]
        rule_data
            rule data.

    @param[out]
    none

    @retval
    none
***************************************************************************************************/
void cri_dms_rules_generic_rule_data_free_handler(void *rule_data);


#endif
