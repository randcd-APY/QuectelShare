#ifndef __QL_DEV_H_
#define __QL_DEV_H_

#include <qmi-framework/qmi_client.h>
#include <qmi/device_management_service_v01.h>

#define MIN(a,b) ((a)>(b)?b:a)
#define TRUE  1
#define FALSE  0 
#define boolean unsigned int

/*
 * enum {
 *         RES_OK = 0,
 *         RES_BAD_PARAMETER  = -1,     ///< Parameter is invalid.
 *         RES_IO_NOT_SUPPORT = -2, 
 *         RES_IO_ERROR = -3, 
 *         RES_NOT_IMPLEMENTED = -4
 * };
 * 
 */
typedef struct {
    char manufacturer[33];
    char product_name[33];
    char firmware_version[65];
    char firmware_date[33];                                                                                                                                                      
}ql_module_about_info_s;

extern int QL_DEV_GetDeviceModel(char* model, size_t modelLen);

extern int QL_Module_About_Get(ql_module_about_info_s *about);

#endif
