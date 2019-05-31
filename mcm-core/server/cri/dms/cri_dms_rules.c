/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "cri_dms_rules.h"
#include "cri_dms_utils.h"


/***************************************************************************************************
    @function
    cri_dms_rules_set_modem_rule_check_handler

    @brief
    This function returns "true", if the current operatig mode is same as the
    mode mentioned in rule data otherwise "false".

    @param[in]
        rule_data
            rule data.

    @param[out]
        none

    @retval
    int - true if the rule is met, false otherwise.
***************************************************************************************************/
int cri_dms_rules_set_modem_rule_check_handler(void *rule_data)
{
    int ret_code;
    cri_core_dms_op_mode_enum_type opr_mode;
    cri_dms_rules_generic_rule_data_type *cri_dms_rules_generic_rule_data;

    UTIL_LOG_FUNC_ENTRY();

    ret_code = FALSE;

    cri_dms_rules_generic_rule_data = (cri_dms_rules_generic_rule_data_type*)rule_data;

    if(rule_data)
    {
        cri_dms_utils_get_current_operating_mode(&opr_mode);
        if ( opr_mode == cri_dms_rules_generic_rule_data->operating_mode )
        {
            ret_code = TRUE;
        }
        else
        {
            ret_code = FALSE;
        }
    }

    UTIL_LOG_FUNC_EXIT_WITH_RET(ret_code);
    return ret_code;
}


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
void cri_dms_rules_generic_rule_data_free_handler(void *rule_data)
{

    UTIL_LOG_FUNC_ENTRY();

    util_memory_free((void**) &rule_data);

    UTIL_LOG_FUNC_EXIT();
}



