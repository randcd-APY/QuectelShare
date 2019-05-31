#ifndef QL_WIFI
#define QL_WIFI

#include <stdio.h>

int QL_WIFI_init();//初始化，创建连接

void QL_WIFI_connect(char *ssid,char *psk);

#endif
