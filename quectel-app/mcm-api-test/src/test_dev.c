#include <ql_in.h>
#include "test_base.h"

extern func_api_test_t t_dev_test;

st_api_test_case at_dev_testlist[] =
{
    {0,     "QL_DEV_GetDeviceModel"},
    {1,     "QL_Module_About_Get"},

    {-1,   "Return to main menu"}
};

#define BUF_SIZE 32
static int test_dev(void)
{
    int    cmdIdx  = 0;
    int    ret     = E_QL_OK;
    char    buf[BUF_SIZE] = {0};    

    show_group_help(&t_dev_test);

    while(1)
    {
        printf("please input cmd index(-1 exit): ");
        ret = scanf("%d", &cmdIdx);
        if(ret != 1)
        {
            char c;
            while(((c=getchar()) != '\n') && (c != EOF))
            {   
                ;
            }
            continue;
        }
        if(cmdIdx == -1)
        {
            break;
        }
        switch(cmdIdx)
        {
            case 0:
                {
                    char model[30];
                    size_t modelLen = 30;
                    memset(model,0,sizeof(model));
                    ret = QL_DEV_GetDeviceModel(model,modelLen);
                    printf("QL_DEV_GetDeviceModel ret = %d model = %s\n",ret,model);
                    break;
                }
            case 1://"QL_MCM_NW_SetConfig"
                {
                    ql_module_about_info_s about;
                    ret = QL_Module_About_Get(&about);
                    printf("QL_Module_About_Get ret = %d\n", ret);
                    printf("firmware_version : %s .\n",about.firmware_version);
                    printf("firmware_date : %s .\n",about.firmware_date);
                    printf("product_name : %s .\n",about.product_name);
                    printf("manufacturer : %s .\n",about.manufacturer);
                    break;
                }
            default:
                show_group_help(&t_dev_test);
        }
    }
    return 0;
}


func_api_test_t t_dev_test = {"mcm_dev", at_dev_testlist, test_dev};
