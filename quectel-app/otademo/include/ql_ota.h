#ifndef _QL_OTA_H_
#define _QL_OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

int QL_OTA_Set_Package_Path(const char *path);
int QL_OTA_Start_Update(void);

#ifdef __cplusplus
}
#endif

#endif
